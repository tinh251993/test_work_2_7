// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CRM_DOWNLOADOBSERVER_H
#define CHROME_BROWSER_CRM_DOWNLOADOBSERVER_H
#include <stddef.h>
#include <stdint.h>

#include <set>
#include <vector>

#include "base/functional/callback_forward.h"
#include "base/memory/raw_ptr.h"
#include "base/run_loop.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_url_parameters.h"
#include "content/public/browser/download_manager.h"

namespace content {

// Detects an arbitrary change on a download item.
// TODO: Rewrite other observers to use this (or be replaced by it).
class CRM_DownloadUpdatedObserver : public download::DownloadItem::Observer {
 public:
  using EventFilter = base::RepeatingCallback<bool(download::DownloadItem*)>;

  // The filter passed may be called multiple times, even after it
  // returns true.
  CRM_DownloadUpdatedObserver(download::DownloadItem* item, EventFilter filter);

  CRM_DownloadUpdatedObserver(const CRM_DownloadUpdatedObserver&) = delete;
  CRM_DownloadUpdatedObserver& operator=(const CRM_DownloadUpdatedObserver&) = delete;

  ~CRM_DownloadUpdatedObserver() override;

  // Returns when either the event has been seen (at least once since
  // object construction) or the item is destroyed.  Return value indicates
  // if the wait ended because the item was seen (true) or the object
  // destroyed (false).
  bool WaitForEvent();

 private:
  // download::DownloadItem::Observer
  void OnDownloadUpdated(download::DownloadItem* item) override;
  void OnDownloadDestroyed(download::DownloadItem* item) override;

  raw_ptr<download::DownloadItem> item_;
  EventFilter filter_;
  bool waiting_;
  bool event_seen_;
};

// Detects changes to the downloads after construction.
//
// Finishes when one of the following happens:
//   - A specified number of downloads change to a terminal state (defined
//     in derived classes).
//   - The download manager was shutdown.
//
// Callers may either probe for the finished state, or wait on it.
class CRMC_DownloadObserver : public DownloadManager::Observer,
                             public download::DownloadItem::Observer {
 public:
  // Action an observer should take if a dangerous download is encountered.
  enum DangerousDownloadAction {
    ON_DANGEROUS_DOWNLOAD_ACCEPT,  // Accept the download
    ON_DANGEROUS_DOWNLOAD_DENY,    // Deny the download
    ON_DANGEROUS_DOWNLOAD_FAIL,    // Fail if a dangerous download is seen
    ON_DANGEROUS_DOWNLOAD_IGNORE,  // Make it the callers problem.
    ON_DANGEROUS_DOWNLOAD_QUIT     // Will set finalgtest/gtest-assertion-result.h state without decision.
  };

  // Create an object that will be considered finished when |wait_count|
  // download items have entered a terminal state.
  CRMC_DownloadObserver(DownloadManager* download_manager,
                       size_t wait_count,
                       DangerousDownloadAction dangerous_download_action);

  CRMC_DownloadObserver(const CRMC_DownloadObserver&) = delete;
  CRMC_DownloadObserver& operator=(const CRMC_DownloadObserver&) = delete;

  ~CRMC_DownloadObserver() override;

  // Wait for one of the finish conditions.
  void WaitForFinished();

  // Return true if we reached one of the finish conditions.
  bool IsFinished() const;

  // download::DownloadItem::Observer
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadDestroyed(download::DownloadItem* download) override;

  // DownloadManager::Observer
  void OnDownloadCreated(DownloadManager* manager,
                         download::DownloadItem* item) override;
  void ManagerGoingDown(DownloadManager* manager) override;

  size_t NumDangerousDownloadsSeen() const;

  size_t NumDownloadsSeenInState(
      download::DownloadItem::DownloadState state) const;

 protected:
  // Only to be called by derived classes' constructors.
  virtual void Init();

  // Called to see if a download item is in a final state.
  virtual bool IsDownloadInFinalState(download::DownloadItem* download) = 0;

 private:
  typedef std::set<download::DownloadItem*> DownloadSet;

  // Maps states to the number of times they have been encountered
  typedef std::map<download::DownloadItem::DownloadState, size_t> StateMap;

  // Called when we know that a download item is in a final state.
  // Note that this is not the same as it first transitioning in to the
  // final state; multiple notifications may occur once the item is in
  // that state.  So we keep our own track of transitions into final.
  void DownloadInFinalState(download::DownloadItem* download);

  void SignalIfFinished();

  // Fake user click on "Accept".
  void AcceptDangerousDownload(uint32_t download_id);

  // Fake user click on "Deny".
  void DenyDangerousDownload(uint32_t download_id);

  // The observed download manager.
  raw_ptr<DownloadManager> download_manager_;

  // The set of download::DownloadItem's that have transitioned to their
  // finished state since construction of this object.  When the size of this
  // array reaches wait_count_, we're done.
  DownloadSet finished_downloads_;

  // The set of download::DownloadItem's we are currently observing.  Generally
  // there won't be any overlap with the above; once we see the final state on a
  // download::DownloadItem, we'll stop observing it.
  DownloadSet downloads_observed_;

  // The map of states to the number of times they have been observed since
  // we started looking.
  // Recorded at the time downloads_observed_ is recorded, but cleared in the
  // constructor to exclude pre-existing states.
  StateMap states_observed_;

  // The number of downloads to wait on completing.
  size_t wait_count_;

  // The number of downloads entered in final state in Init().  We use
  // |finished_downloads_| to track the incoming transitions to final state we
  // should ignore, and to track the number of final state transitions that
  // occurred between construction and return from wait.  But some downloads may
  // be in our final state (and thus be entered into |finished_downloads_|) when
  // we construct this class.  We don't want to count those in our transition to
  // finished.
  int finished_downloads_at_construction_;

  // Whether an internal message loop has been started and must be quit upon
  // all downloads completing.
  bool waiting_;

  // Action to take if a dangerous download is encountered.
  DangerousDownloadAction dangerous_download_action_;

  // Holds the download ids which were dangerous.
  std::set<uint32_t> dangerous_downloads_seen_;

  base::WeakPtrFactory<CRMC_DownloadObserver> weak_factory_{this};
};

class DownloadObserverTerminal : public CRMC_DownloadObserver {
 public:
  // Create an object that will be considered finished when |wait_count|
  // download items have entered a terminal state
  // (download::DownloadItem::IsDone() is true).
  DownloadObserverTerminal(
      DownloadManager* download_manager,
      size_t wait_count,
      DangerousDownloadAction dangerous_download_action);

  DownloadObserverTerminal(const DownloadObserverTerminal&) = delete;
  DownloadObserverTerminal& operator=(const DownloadObserverTerminal&) =
      delete;

  ~DownloadObserverTerminal() override;

 private:
  bool IsDownloadInFinalState(download::DownloadItem* download) override;
};

// Detects changes to the downloads after construction.
// Finishes when a specified number of downloads change to the
// IN_PROGRESS state, or when the download manager is destroyed.
// Dangerous downloads are accepted.
// Callers may either probe for the finished state, or wait on it.
class CRMC_DownloadObserverInProgress : public CRMC_DownloadObserver {
 public:
  // Create an object that will be considered finished when |wait_count|
  // download items have entered state |IN_PROGRESS|.
  CRMC_DownloadObserverInProgress(
      DownloadManager* download_manager, size_t wait_count);

  CRMC_DownloadObserverInProgress(const CRMC_DownloadObserverInProgress&) =
      delete;
  CRMC_DownloadObserverInProgress& operator=(
      const CRMC_DownloadObserverInProgress&) = delete;

  ~CRMC_DownloadObserverInProgress() override;

 private:
  bool IsDownloadInFinalState(download::DownloadItem* download) override;
};

class CRMC_DownloadTestObserverInterrupted : public CRMC_DownloadObserver {
 public:
  // Create an object that will be considered finished when |wait_count|
  // download items are interrupted.
  CRMC_DownloadTestObserverInterrupted(
      DownloadManager* download_manager,
      size_t wait_count,
      DangerousDownloadAction dangerous_download_action);

  CRMC_DownloadTestObserverInterrupted(const CRMC_DownloadTestObserverInterrupted&) =
      delete;
  CRMC_DownloadTestObserverInterrupted& operator=(
      const CRMC_DownloadTestObserverInterrupted&) = delete;

  ~CRMC_DownloadTestObserverInterrupted() override;

 private:
  bool IsDownloadInFinalState(download::DownloadItem* download) override;
};

// The WaitForFlush() method on this class returns after:
//      * There are no IN_PROGRESS download items remaining on the
//        DownloadManager.
//      * There have been two round trip messages through the file and
//        IO threads.
// This almost certainly means that a Download cancel has propagated through
// the system.
class CRMC_DownloadFlushObserver : public DownloadManager::Observer,
                                  public download::DownloadItem::Observer {
 public:
  explicit CRMC_DownloadFlushObserver(DownloadManager* download_manager);

  CRMC_DownloadFlushObserver(const CRMC_DownloadFlushObserver&) = delete;
  CRMC_DownloadFlushObserver& operator=(const CRMC_DownloadFlushObserver&) =
      delete;

  ~CRMC_DownloadFlushObserver() override;

  void WaitForFlush();

  // DownloadsManager observer methods.
  void OnDownloadCreated(DownloadManager* manager,
                         download::DownloadItem* item) override;
  void ManagerGoingDown(DownloadManager* manager) override;

  // download::DownloadItem observer methods.
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadDestroyed(download::DownloadItem* download) override;

 private:
  typedef std::set<download::DownloadItem*> DownloadSet;

  // If we're waiting for that flush point, check the number
  // of downloads in the IN_PROGRESS state and take appropriate
  // action.  If requested, also observes all downloads while iterating.
  void CheckDownloadsInProgress(bool observe_downloads);

  raw_ptr<DownloadManager> download_manager_;
  DownloadSet downloads_observed_;
  bool waiting_for_zero_inprogress_;
  base::RunLoop run_loop_;
};

// Waits for a callback indicating that the download::DownloadItem is about to
// be created, or that an error occurred and it won't be created.
class CRMC_DownloadItemCreationObserver
    : public base::RefCountedThreadSafe<CRMC_DownloadItemCreationObserver> {
 public:
  CRMC_DownloadItemCreationObserver();

  CRMC_DownloadItemCreationObserver(const CRMC_DownloadItemCreationObserver&) =
      delete;
  CRMC_DownloadItemCreationObserver& operator=(
      const CRMC_DownloadItemCreationObserver&) = delete;

  void WaitForDownloadItemCreation();

  uint32_t download_id() const { return download_id_; }
  download::DownloadInterruptReason interrupt_reason() const {
    return interrupt_reason_;
  }
  bool started() const { return called_back_count_ > 0; }
  bool succeeded() const {
    return started() &&
           interrupt_reason_ == download::DOWNLOAD_INTERRUPT_REASON_NONE;
  }

  download::DownloadUrlParameters::OnStartedCallback callback();

 private:
  friend class base::RefCountedThreadSafe<CRMC_DownloadItemCreationObserver>;

  ~CRMC_DownloadItemCreationObserver();

  void DownloadItemCreationCallback(
      download::DownloadItem* item,
      download::DownloadInterruptReason interrupt_reason);

  // The download creation information we received.
  uint32_t download_id_;
  download::DownloadInterruptReason interrupt_reason_;

  // Count of callbacks.
  size_t called_back_count_;

  // We are in the message loop.
  bool waiting_;
};

// Class for mornitoring whether a save package download finishes.
class CRMC_SavePackageFinishedObserver : public download::DownloadItem::Observer,
                                    public DownloadManager::Observer {
 public:
  CRMC_SavePackageFinishedObserver(
      DownloadManager* manager,
      base::OnceClosure callback,
      std::set<download::DownloadItem::DownloadState> final_states = {
          download::DownloadItem::COMPLETE, download::DownloadItem::CANCELLED});

  CRMC_SavePackageFinishedObserver(const CRMC_SavePackageFinishedObserver&) = delete;
  CRMC_SavePackageFinishedObserver& operator=(const CRMC_SavePackageFinishedObserver&) =
      delete;

  ~CRMC_SavePackageFinishedObserver() override;

  // download::DownloadItem::Observer:
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadDestroyed(download::DownloadItem* download) override;

  // DownloadManager::Observer:
  void OnDownloadCreated(DownloadManager* manager,
                         download::DownloadItem* download) override;
  void ManagerGoingDown(DownloadManager* manager) override;

 private:
  raw_ptr<DownloadManager> download_manager_;
  raw_ptr<download::DownloadItem> download_;
  base::OnceClosure callback_;
  std::set<download::DownloadItem::DownloadState> final_states_;
};

}  // namespace content`

#endif  // CHROME_BROWSER_CRM_DOWNLOADOBSERVER_H