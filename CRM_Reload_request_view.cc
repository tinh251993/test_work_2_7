#include "chrome/browser/ui/views/CRM_Reload_request_view.h"

#include <stddef.h>

#include <string>
#include <utility>
#include <vector>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/browser/metrics/metrics_reporting_state.h"
#include "chrome/browser/prefs/session_startup_pref.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/sessions/exit_type_service.h"
#include "chrome/browser/sessions/session_restore.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_list_observer.h"
#include "chrome/browser/ui/bubble_anchor_util.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/chrome_typography.h"
#include "chrome/browser/ui/views/frame/app_menu_button.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/toolbar_button_provider.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/installer/util/google_update_settings.h"
#include "components/strings/grit/components_chromium_strings.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "ui/base/buildflags.h"
#include "ui/base/interaction/element_identifier.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/models/dialog_model.h"
#include "ui/base/window_open_disposition.h"
#include "ui/base/window_open_disposition_utils.h"
#include "ui/views/bubble/bubble_dialog_model_host.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/controls/button/label_button_border.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/separator.h"
#include "ui/views/controls/styled_label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/widget/widget.h"
#include "chrome/browser/CRM_BrowserIF.h"

namespace{
views::BubbleDialogDelegate* g_instance_for_test = nullptr;    
class CRMC_UpdateRequestDelegate : public ui::DialogModelDelegate {
 public:
  explicit CRMC_UpdateRequestDelegate(Profile* profile) {
    if (ExitTypeService* exit_type_service =
            ExitTypeService::GetInstanceForProfile(profile)) {
      crashed_lock_ = exit_type_service->CreateCrashedLock();
    }
  }

  ~CRMC_UpdateRequestDelegate() override { g_instance_for_test = nullptr; }

  void OpenStartupPages(Browser* browser) {
        DVLOG(0) <<"Current do nothing later will check throught JSON";
  }

  void OnWindowClosing() {
        DVLOG(0) << "Current do nothing";
  }

  void RestorePreviousSession(Browser* browser) {
        DVLOG(0) << "Current do nothing";
        CRMC_BrowserIF* brwIf = CRMC_BrowserIF::getInstance();
        if (brwIf)
          brwIf->RequestReload(); 
  }

  void MaybeEnableUma() {
    if (!ignored_)
        DVLOG(0) << "Current do nothing";

    DVLOG(0) << "super error" ;
  }

 private:
  bool ignored_ = true;
  std::unique_ptr<ExitTypeService::CrashedLock> crashed_lock_;
};
}//namcespace end


class CRMC_ReloadRequestView::BrowserRemovalObserver
    : public BrowserListObserver {
 public:
  explicit BrowserRemovalObserver(Browser* browser) : browser_(browser) {
    DCHECK(browser_);
    BrowserList::AddObserver(this);
  }

  BrowserRemovalObserver(const BrowserRemovalObserver&) = delete;
  BrowserRemovalObserver& operator=(const BrowserRemovalObserver&) = delete;

  ~BrowserRemovalObserver() override { BrowserList::RemoveObserver(this); }

  // Overridden from BrowserListObserver.
  void OnBrowserRemoved(Browser* browser) override {
    if (browser == browser_)
      browser_ = nullptr;
  }

  Browser* browser() const { return browser_; }

 private:
  raw_ptr<Browser> browser_;
};


// static
// void SessionCrashedBubble::ShowIfNotOffTheRecordProfile(
//     Browser* browser,
//     bool skip_tab_checking) {
//   DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
//   if (browser->profile()->IsOffTheRecord())
//     return;

//   // Observes possible browser removal before Show is called.
//   auto browser_observer =
//       std::make_unique<SessionCrashedBubbleView::BrowserRemovalObserver>(
//           browser);

//   if (DoesSupportConsentCheck()) {
//     GoogleUpdateSettings::CollectStatsConsentTaskRunner()
//         ->PostTaskAndReplyWithResult(
//             FROM_HERE,
//             base::BindOnce(&GoogleUpdateSettings::GetCollectStatsConsent),
//             base::BindOnce(&SessionCrashedBubbleView::Show,
//                            std::move(browser_observer), skip_tab_checking));
//   } else {
//     SessionCrashedBubbleView::Show(std::move(browser_observer),
//                                    skip_tab_checking, false);
//   }
// }

// static
void CRMC_ReloadRequestView::Show(Browser* browser) {
  // Determine whether or not the UMA opt-in option should be offered. It is
  // offered only when it is a Google chrome build, user hasn't opted in yet,
  // and the preference is modifiable by the user.
//   bool offer_uma_optin = false;
//   bool uma_opted_in_already = false;
//   if (DoesSupportConsentCheck() && !uma_opted_in_already)
//     offer_uma_optin = !IsMetricsReportingPolicyManaged();
  auto browser_observer =
      std::make_unique<CRMC_ReloadRequestView::BrowserRemovalObserver>(
          browser);

  if (browser && (browser->tab_strip_model()->GetActiveWebContents())) {
    ShowBubble(browser);
    return;
  }
}

// static
views::BubbleDialogDelegate* CRMC_ReloadRequestView::GetInstance() {
  return g_instance_for_test;
}

views::BubbleDialogDelegate* CRMC_ReloadRequestView::ShowBubble(
    Browser* browser) {
  views::View* anchor_view = BrowserView::GetBrowserViewForBrowser(browser)
                                 ->toolbar_button_provider()
                                 ->GetAppMenuButton();

  auto bubble_delegate_unique =
      std::make_unique<CRMC_UpdateRequestDelegate>(browser->profile());
  CRMC_UpdateRequestDelegate* bubble_delegate = bubble_delegate_unique.get();

  ui::DialogModel::Builder dialog_builder(std::move(bubble_delegate_unique));
  dialog_builder
      .SetTitle(u"Start Upate")
      .DisableCloseOnDeactivate()
      .SetIsAlertDialog()
      .SetDialogDestroyingCallback(
          base::BindOnce(&CRMC_UpdateRequestDelegate::OnWindowClosing,
                         base::Unretained(bubble_delegate)))
      .AddParagraph(ui::DialogModelLabel(u"Reload to update"));

  const SessionStartupPref session_startup_pref =
      SessionStartupPref::GetStartupPref(browser->profile());

  if (session_startup_pref.ShouldOpenUrls() &&
      !session_startup_pref.urls.empty()) {
    dialog_builder.AddCancelButton(
        base::BindOnce(&CRMC_UpdateRequestDelegate::OpenStartupPages,
                       base::Unretained(bubble_delegate), browser));
  }

  dialog_builder.AddOkButton(
      base::BindOnce(&CRMC_UpdateRequestDelegate::RestorePreviousSession,
                     base::Unretained(bubble_delegate), browser),
      ui::DialogModelButton::Params().SetLabel(
          u"Reload to update"));

  auto bubble = std::make_unique<views::BubbleDialogModelHost>(
      dialog_builder.Build(), anchor_view, views::BubbleBorder::TOP_RIGHT);

  views::BubbleDialogDelegate* bubble_ptr = bubble.get();
  g_instance_for_test = bubble_ptr;
  views::BubbleDialogDelegate::CreateBubble(std::move(bubble))->Show();

  return bubble_ptr;
}
