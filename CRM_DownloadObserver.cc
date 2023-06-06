// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/CRM_DownloadObserver.h"

#include <vector>

#include "base/check_op.h"
#include "base/containers/contains.h"
#include "base/functional/bind.h"
#include "base/notreached.h"
#include "base/run_loop.h"
#include "components/download/public/common/download_url_parameters.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "base/run_loop.h"
namespace content {
    
void RunMessageLoop() {
  base::RunLoop(base::RunLoop::Type::kNestableTasksAllowed).Run();
}


CRM_DownloadUpdatedObserver::CRM_DownloadUpdatedObserver(
    download::DownloadItem* item,
    CRM_DownloadUpdatedObserver::EventFilter filter)
    : item_(item), filter_(filter), waiting_(false), event_seen_(false) {
  item->AddObserver(this);
}

CRM_DownloadUpdatedObserver::~CRM_DownloadUpdatedObserver() {
  if (item_)
    item_->RemoveObserver(this);
}

bool CRM_DownloadUpdatedObserver::WaitForEvent() {
  if (item_ && filter_.Run(item_.get()))
    event_seen_ = true;
  if (event_seen_)
    return true;

  waiting_ = true;
  RunMessageLoop();
  waiting_ = false;
  return event_seen_;
}

void CRM_DownloadUpdatedObserver::OnDownloadUpdated(download::DownloadItem* item) {
  DCHECK_EQ(item_, item);
  if (filter_.Run(item_.get()))
    event_seen_ = true;
  if (waiting_ && event_seen_)
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
}

void CRM_DownloadUpdatedObserver::OnDownloadDestroyed(
    download::DownloadItem* item) {
  DCHECK_EQ(item_, item);
  item_->RemoveObserver(this);
  item_ = nullptr;
  if (waiting_)
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
}

CRMC_DownloadObserver::CRMC_DownloadObserver(
    DownloadManager* download_manager,
    size_t wait_count,
    DangerousDownloadAction dangerous_download_action)
    : download_manager_(download_manager),
      wait_count_(wait_count),
      finished_downloads_at_construction_(0),
      waiting_(false),
      dangerous_download_action_(dangerous_download_action) {}

CRMC_DownloadObserver::~CRMC_DownloadObserver() {
  for (DownloadSet::iterator it = downloads_observed_.begin();
       it != downloads_observed_.end(); ++it)
    (*it)->RemoveObserver(this);

  if (download_manager_)
    download_manager_->RemoveObserver(this);
}

void CRMC_DownloadObserver::Init() {
  download_manager_->AddObserver(this);
  std::vector<download::DownloadItem*> downloads;
  download_manager_->GetAllDownloads(&downloads);
  for (std::vector<download::DownloadItem*>::iterator it = downloads.begin();
       it != downloads.end(); ++it) {
    OnDownloadCreated(download_manager_, *it);
  }
  finished_downloads_at_construction_ = finished_downloads_.size();
  states_observed_.clear();
}

void CRMC_DownloadObserver::ManagerGoingDown(DownloadManager* manager) {
  CHECK_EQ(manager, download_manager_);
  download_manager_ = nullptr;
  SignalIfFinished();
}

void CRMC_DownloadObserver::WaitForFinished() {
  if (!IsFinished()) {
    waiting_ = true;
    RunMessageLoop();
    waiting_ = false;
  }
}

bool CRMC_DownloadObserver::IsFinished() const {
  return (finished_downloads_.size() - finished_downloads_at_construction_ >=
          wait_count_) ||
         (download_manager_ == nullptr);
}

void CRMC_DownloadObserver::OnDownloadCreated(DownloadManager* manager,
                                             download::DownloadItem* item) {
  // NOTE: This method is called both by DownloadManager when a download is
  // created as well as in DownloadTestObserver::Init() for downloads that
  // existed before |this| was created.
  OnDownloadUpdated(item);
  DownloadSet::const_iterator finished_it(finished_downloads_.find(item));
  // If it isn't finished, start observing it.
  if (finished_it == finished_downloads_.end()) {
    item->AddObserver(this);
    downloads_observed_.insert(item);
  }
}

void CRMC_DownloadObserver::OnDownloadDestroyed(
    download::DownloadItem* download) {
  // Stop observing.  Do not do anything with it, as it is about to be gone.
  DownloadSet::iterator it = downloads_observed_.find(download);
  if (it == downloads_observed_.end()){
    return;
  }

  downloads_observed_.erase(it);
  download->RemoveObserver(this);
}

void CRMC_DownloadObserver::OnDownloadUpdated(download::DownloadItem* download) {
  // Real UI code gets the user's response after returning from the observer.
  if (download->IsDangerous() &&
      !base::Contains(dangerous_downloads_seen_, download->GetId())) {
    dangerous_downloads_seen_.insert(download->GetId());

    // Calling ValidateDangerousDownload() at this point will
    // cause the download to be completed twice.  Do what the real UI
    // code does: make the call as a delayed task.
    switch (dangerous_download_action_) {
      case ON_DANGEROUS_DOWNLOAD_ACCEPT:
        // Fake user click on "Accept".  Delay the actual click, as the
        // real UI would.
        GetUIThreadTaskRunner({})->PostTask(
            FROM_HERE,
            base::BindOnce(&CRMC_DownloadObserver::AcceptDangerousDownload,
                           weak_factory_.GetWeakPtr(), download->GetId()));
        break;

      case ON_DANGEROUS_DOWNLOAD_DENY:
        // Fake a user click on "Deny".  Delay the actual click, as the
        // real UI would.
        GetUIThreadTaskRunner({})->PostTask(
            FROM_HERE,
            base::BindOnce(&CRMC_DownloadObserver::DenyDangerousDownload,
                           weak_factory_.GetWeakPtr(), download->GetId()));
        break;

      case ON_DANGEROUS_DOWNLOAD_FAIL:
        DVLOG(0) << "Unexpected dangerous download item.";
        break;

      case ON_DANGEROUS_DOWNLOAD_IGNORE:
        break;

      case ON_DANGEROUS_DOWNLOAD_QUIT:
        DownloadInFinalState(download);
        break;

      default:
        NOTREACHED();
    }
  }

  if (IsDownloadInFinalState(download))
    DownloadInFinalState(download);
}

size_t CRMC_DownloadObserver::NumDangerousDownloadsSeen() const {
  return dangerous_downloads_seen_.size();
}

size_t CRMC_DownloadObserver::NumDownloadsSeenInState(
    download::DownloadItem::DownloadState state) const {
  StateMap::const_iterator it = states_observed_.find(state);

  if (it == states_observed_.end())
    return 0;

  return it->second;
}

void CRMC_DownloadObserver::DownloadInFinalState(
    download::DownloadItem* download) {
  if (finished_downloads_.find(download) != finished_downloads_.end()) {
    // We've already seen the final state on this download.
    return;
  }

  // Record the transition.
  finished_downloads_.insert(download);

  // Record the state.
  states_observed_[download->GetState()]++;  // Initializes to 0 the first time.

  SignalIfFinished();
}

void CRMC_DownloadObserver::SignalIfFinished() {
  if (waiting_ && IsFinished())
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
}

void CRMC_DownloadObserver::AcceptDangerousDownload(uint32_t download_id) {
  // Download manager was shutdown before the UI thread could accept the
  // download.
  if (!download_manager_)
    return;
  download::DownloadItem* download =
      download_manager_->GetDownload(download_id);
  if (download && !download->IsDone())
    download->ValidateDangerousDownload();
}

void CRMC_DownloadObserver::DenyDangerousDownload(uint32_t download_id) {
  // Download manager was shutdown before the UI thread could deny the
  // download.
  if (!download_manager_)
    return;
  download::DownloadItem* download =
      download_manager_->GetDownload(download_id);
  if (download && !download->IsDone())
    download->Remove();
}

DownloadObserverTerminal::DownloadObserverTerminal(
    DownloadManager* download_manager,
    size_t wait_count,
    DangerousDownloadAction dangerous_download_action)
        : CRMC_DownloadObserver(download_manager,
                               wait_count,
                               dangerous_download_action) {
  // You can't rely on overriden virtual functions in a base class constructor;
  // the virtual function table hasn't been set up yet.  So, we have to do any
  // work that depends on those functions in the derived class constructor
  // instead.  In this case, it's because of |IsDownloadInFinalState()|.
  Init();
}

DownloadObserverTerminal::~DownloadObserverTerminal() {
}

bool DownloadObserverTerminal::IsDownloadInFinalState(
    download::DownloadItem* download) {
  return download->IsDone();
}

CRMC_DownloadObserverInProgress::CRMC_DownloadObserverInProgress(
    DownloadManager* download_manager,
    size_t wait_count)
        : CRMC_DownloadObserver(download_manager,
                               wait_count,
                               ON_DANGEROUS_DOWNLOAD_ACCEPT) {
  // You can't override virtual functions in a base class constructor; the
  // virtual function table hasn't been set up yet.  So, we have to do any
  // work that depends on those functions in the derived class constructor
  // instead.  In this case, it's because of |IsDownloadInFinalState()|.
  Init();
}

CRMC_DownloadObserverInProgress::~CRMC_DownloadObserverInProgress() {
}

bool CRMC_DownloadObserverInProgress::IsDownloadInFinalState(
    download::DownloadItem* download) {
  return (download->GetState() == download::DownloadItem::IN_PROGRESS) &&
         !download->GetTargetFilePath().empty();
}

CRMC_DownloadTestObserverInterrupted::CRMC_DownloadTestObserverInterrupted(
    DownloadManager* download_manager,
    size_t wait_count,
    DangerousDownloadAction dangerous_download_action)
        : CRMC_DownloadObserver(download_manager,
                               wait_count,
                               dangerous_download_action) {
  // You can't rely on overriden virtual functions in a base class constructor;
  // the virtual function table hasn't been set up yet.  So, we have to do any
  // work that depends on those functions in the derived class constructor
  // instead.  In this case, it's because of |IsDownloadInFinalState()|.
  Init();
}

CRMC_DownloadTestObserverInterrupted::~CRMC_DownloadTestObserverInterrupted() {
}

bool CRMC_DownloadTestObserverInterrupted::IsDownloadInFinalState(
    download::DownloadItem* download) {
  return download->GetState() == download::DownloadItem::INTERRUPTED;
}

void PingIOThread(int cycle, base::OnceClosure callback);

// Helper method to post a task to IO thread to ensure remaining operations on
// the IO thread complete.
void PingFileThread(int cycle, base::OnceClosure callback) {
  GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&PingIOThread, cycle, std::move(callback)));
}

// Post a task to file thread, and wait for it to be posted back on to the IO
// thread if |cycle| is larger than 1. This ensures that all remaining
// operations on the IO thread complete.
void PingIOThread(int cycle, base::OnceClosure callback) {
  if (--cycle) {
    DownloadManager::GetTaskRunner()->PostTask(
        FROM_HERE, base::BindOnce(&PingFileThread, cycle, std::move(callback)));
  } else {
    GetUIThreadTaskRunner({})->PostTask(FROM_HERE, std::move(callback));
  }
}

CRMC_DownloadFlushObserver::CRMC_DownloadFlushObserver(
    DownloadManager* download_manager)
    : download_manager_(download_manager),
      waiting_for_zero_inprogress_(true) {}

void CRMC_DownloadFlushObserver::WaitForFlush() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  download_manager_->AddObserver(this);
  // The wait condition may have been met before WaitForFlush() was called.
  CheckDownloadsInProgress(true);
  run_loop_.Run();
}

void CRMC_DownloadFlushObserver::OnDownloadCreated(
    DownloadManager* manager,
    download::DownloadItem* item) {
  CheckDownloadsInProgress(true);
}

void CRMC_DownloadFlushObserver::ManagerGoingDown(DownloadManager* manager) {
  download_manager_ = nullptr;
}

void CRMC_DownloadFlushObserver::OnDownloadDestroyed(
    download::DownloadItem* download) {
  // Stop observing.  Do not do anything with it, as it is about to be gone.
  DownloadSet::iterator it = downloads_observed_.find(download);
  if (it == downloads_observed_.end()){
    return;
  }
  downloads_observed_.erase(it);
  download->RemoveObserver(this);
}

void CRMC_DownloadFlushObserver::OnDownloadUpdated(
    download::DownloadItem* download) {
  // No change in download::DownloadItem set on manager.
  CheckDownloadsInProgress(false);
}

CRMC_DownloadFlushObserver::~CRMC_DownloadFlushObserver() {
  if (!download_manager_)
    return;

  download_manager_->RemoveObserver(this);
  for (DownloadSet::iterator it = downloads_observed_.begin();
       it != downloads_observed_.end(); ++it) {
    (*it)->RemoveObserver(this);
  }
}

// If we're waiting for that flush point, check the number
// of downloads in the IN_PROGRESS state and take appropriate
// action. If requested, also observes all downloads while iterating.
void CRMC_DownloadFlushObserver::CheckDownloadsInProgress(
    bool observe_downloads) {
  if (waiting_for_zero_inprogress_) {
    int count = 0;

    std::vector<download::DownloadItem*> downloads;
    download_manager_->GetAllDownloads(&downloads);
    for (std::vector<download::DownloadItem*>::iterator it = downloads.begin();
         it != downloads.end(); ++it) {
      if ((*it)->GetState() == download::DownloadItem::IN_PROGRESS)
        count++;
      if (observe_downloads) {
        if (downloads_observed_.find(*it) == downloads_observed_.end()) {
          (*it)->AddObserver(this);
          downloads_observed_.insert(*it);
        }
        // Download items are forever, and we don't want to make
        // assumptions about future state transitions, so once we
        // start observing them, we don't stop until destruction.
      }
    }

    if (count == 0) {
      waiting_for_zero_inprogress_ = false;
      // Stop observing download::DownloadItems.  We maintain the observation
      // of DownloadManager so that we don't have to independently track
      // whether we are observing it for conditional destruction.
      for (DownloadSet::iterator it = downloads_observed_.begin();
           it != downloads_observed_.end(); ++it) {
        (*it)->RemoveObserver(this);
      }
      downloads_observed_.clear();

      // Trigger next step.  We need to go past the IO thread twice, as
      // there's a self-task posting in the IO thread cancel path.
      DownloadManager::GetTaskRunner()->PostTask(
          FROM_HERE,
          base::BindOnce(&PingFileThread, 2, run_loop_.QuitClosure()));
    }
  }
}

CRMC_DownloadItemCreationObserver::CRMC_DownloadItemCreationObserver()
    : download_id_(download::DownloadItem::kInvalidId),
      interrupt_reason_(download::DOWNLOAD_INTERRUPT_REASON_NONE),
      called_back_count_(0),
      waiting_(false) {}

CRMC_DownloadItemCreationObserver::~CRMC_DownloadItemCreationObserver() {
}

void CRMC_DownloadItemCreationObserver::WaitForDownloadItemCreation() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  if (called_back_count_ == 0) {
    waiting_ = true;
    RunMessageLoop();
    waiting_ = false;
  }
}

void CRMC_DownloadItemCreationObserver::DownloadItemCreationCallback(
    download::DownloadItem* item,
    download::DownloadInterruptReason interrupt_reason) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  if (item)
    download_id_ = item->GetId();
  interrupt_reason_ = interrupt_reason;
  ++called_back_count_;
  DCHECK_EQ(1u, called_back_count_);

  if (waiting_)
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
}

download::DownloadUrlParameters::OnStartedCallback
CRMC_DownloadItemCreationObserver::callback() {
  return base::BindOnce(
      &CRMC_DownloadItemCreationObserver::DownloadItemCreationCallback, this);
}

CRMC_SavePackageFinishedObserver::CRMC_SavePackageFinishedObserver(
    DownloadManager* manager,
    base::OnceClosure callback,
    std::set<download::DownloadItem::DownloadState> final_states)
    : download_manager_(manager),
      download_(nullptr),
      callback_(std::move(callback)),
      final_states_(std::move(final_states)) {
  download_manager_->AddObserver(this);
}

CRMC_SavePackageFinishedObserver::~CRMC_SavePackageFinishedObserver() {
  if (download_manager_)
    download_manager_->RemoveObserver(this);

  if (download_)
    download_->RemoveObserver(this);
}

void CRMC_SavePackageFinishedObserver::OnDownloadUpdated(
    download::DownloadItem* download) {
  if (final_states_.count(download->GetState())) {
    std::move(callback_).Run();
  }
}

void CRMC_SavePackageFinishedObserver::OnDownloadDestroyed(
    download::DownloadItem* download) {
  download_->RemoveObserver(this);
  download_ = nullptr;
}

void CRMC_SavePackageFinishedObserver::OnDownloadCreated(
    DownloadManager* manager,
    download::DownloadItem* download) {
  download_ = download;
  download->AddObserver(this);
}

void CRMC_SavePackageFinishedObserver::ManagerGoingDown(DownloadManager* manager) {
  download_->RemoveObserver(this);
  download_ = nullptr;
  download_manager_->RemoveObserver(this);
  download_manager_ = nullptr;
}

}  // namespace content