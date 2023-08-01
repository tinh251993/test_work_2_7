#ifndef BROWSER_IF_H
#define BROWSER_IF_H

#include "base/memory/ref_counted.h"
#include "base/threading/platform_thread.h"
#include "base/threading/thread.h"
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include "third_party/blink/public/platform/web_string.h"
#include "ui/events/event.h"
#include "content/public/browser/page_navigator.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "ui/gfx/geometry/rect.h"
#include "url/gurl.h"
#include "base/files/file_path.h"
#include "chrome/browser/ui/login/login_handler.h"
#include "third_party/blink/public/mojom/loader/resource_load_info.mojom-shared.h"
#include "content/public/common/javascript_dialog_type.h"
#include "chrome/browser/file_select_helper.h"
#include "ipc/ipc_listener.h"
#include "chrome/browser/ui/views/side_panel/side_panel.h"
#include "chrome/browser/ui/views/side_panel/side_panel_coordinator.h"

#include "chrome/browser/CRM_BrowserWebDataManager.h"

#if BUILDFLAG(IS_WIN)
// windows.h must be included before shellapi.h
#include <windows.h>
#include <shellapi.h>
#endif

class BrowserView;
class Browser;
class CRMC_RegisterExtJS;
using blink::WebString;
namespace views {
    class WebView;
}

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define BUILD_VERSION 0


class CRMC_BrowserIF{
public:
    //Init
    CRMC_BrowserIF();

    ~CRMC_BrowserIF();

    static CRMC_BrowserIF* getInstance();

    void setBrowser(Browser* pc_Browser);

    Browser* browser(){return m_pc_Browser;}

    BrowserView* browserView(){
        return m_pc_BrowserView;
    }

    
    //
    //
    // bool showLeftSideBar();

    // bool showRightSideBar();

    void showToolBar();

    void hideToolBar();

    void setWebView(raw_ptr<views::WebView> webview);
    // bool hideToolBar();
    void ReloadWebviewIfNeed();

    void CheckForUpdate();

    void notifyDownloadFinished(int32_t si_Result, int32_t si_Status_code){DVLOG(0) << "Do nothing";}

    void Update();

    void Close();

    void NotUpdate();

    void RequestReload();

    void ShowRequestReload();

    void OpenNewTab();

    bool OpenRunableFile(GURL url);
    
    bool OpenRunableFile(std::string p_url);

    void showBookmark(bool can_open);
private:
    bool is_check_for_update = false;
    friend class CRMC_RegisterExtJS;
    static CRMC_BrowserIF*	m_pc_BrowserIF;
    BrowserView* m_pc_BrowserView;
    Browser* m_pc_Browser;
    bool is_initialize = true;
    raw_ptr<views::WebView> webview_;
    bool is_check = true;
};


#endif //[BrowserIF] end