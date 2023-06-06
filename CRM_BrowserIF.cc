#include <string>
#include "chrome/browser/CRM_BrowserIF.h"
#include "chrome/browser/CRM_DownloadManager.h"
#include "chrome/browser/CRM_DownloadJSON.h"
// #include "chrome/ui/views/CRM_Update_request_view.h"
#include "chrome/browser/ui/views/CRM_Update_request_view.h"
#include "chrome/browser/ui/views/CRM_Reload_request_view.h"


#if !BUILDFLAG(IS_ANDROID)
#include "chrome/browser/lifetime/application_lifetime_desktop.h"
#endif


#include <chrono>
#include <thread>


#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "content/public/browser/render_frame_host.h"
#include "components/safe_browsing/core/common/safe_browsing_prefs.h"
#include "components/autofill/core/common/autofill_prefs.h"
#include "components/javascript_dialogs/tab_modal_dialog_manager.h"
#include "chrome/browser/ui/tab_dialogs.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "base/functional/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/browser_message_filter.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "third_party/blink/public/common/web_preferences/web_preferences.h"
#include "ui/gfx/font_fallback_linux.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/http/http_transaction_factory.h"
#include "content/public/browser/navigation_entry.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#include "components/proxy_config/proxy_config_pref_names.h"
#include "components/proxy_config/proxy_config_dictionary.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "third_party/blink/public/web/web_plugin.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/dom/dom_key.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/render_widget_host.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "chrome/common/pref_names.h"
#include "components/spellcheck/browser/pref_names.h"
#include "components/translate/core/browser/translate_pref_names.h"
#include "components/password_manager/core/common/password_manager_pref_names.h"
#include "components/embedder_support/pref_names.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "components/bookmarks/common/bookmark_pref_names.h"
#include "chrome/common/webui_url_constants.h"
#include "components/language/core/browser/pref_names.h"		
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "content/browser/storage_partition_impl.h"
#include "base/threading/thread.h"
#include "content/public/common/content_switches.h"
#include "extensions/browser/pref_names.h"

#include "chrome/browser/pdf/pdf_frame_util.h"
#include "content/browser/site_instance_impl.h"
#include "base/files/file_util.h"
#include "base/time/time.h"




using content::NavigationEntry;
using content::Referrer;
using content::BrowserThread;
using content::OpenURLParams;
using content::WebContents;

#include "components/version_info/version_info.h"


#define UPDATE_TIME 60*60*1000


CRMC_BrowserIF* CRMC_BrowserIF::m_pc_BrowserIF = NULL;

namespace {
// void OnCloseAllBrowsersSucceeded(base::OnceClosure quit_closure,
//                                  const base::FilePath& path) {
//   std::move(quit_closure).Run();
// }
}

CRMC_BrowserIF::CRMC_BrowserIF() :
m_pc_BrowserView(nullptr),
m_pc_Browser(nullptr)
{
}

CRMC_BrowserIF::~CRMC_BrowserIF(){}

CRMC_BrowserIF* CRMC_BrowserIF::getInstance(){
    if (!m_pc_BrowserIF){
        m_pc_BrowserIF = new CRMC_BrowserIF();
    }

    return m_pc_BrowserIF;
}

void CRMC_BrowserIF::showToolBar(){
    return;
    // DVLOG(0) << " Done show toolbar";
    // BrowserView* const browser_view =
    //   BrowserView::GetBrowserViewForBrowser(m_pc_Browser);
    // browser_view->left_side_panel_coordinator()->Toggle();
    // ReloadWebviewIfNeed();
}

void CRMC_BrowserIF::hideToolBar(){
    return;
    // DVLOG(0) << "Done hide toolbar";
    // BrowserView* const browser_view = 
    //     BrowserView::GetBrowserViewForBrowser(m_pc_Browser);
    // browser_view->left_side_panel_coordinator()->Hide();
   // ReloadWebviewIfNeed();
}

void CRMC_BrowserIF::setWebView(raw_ptr<views::WebView> webview){
    if (!webview)
        return;

    webview_ = webview;        
}

void CRMC_BrowserIF::ReloadWebviewIfNeed(){

    // BrowserView* const browser_view = 
    //     BrowserView::GetBrowserViewForBrowser(m_pc_Browser);
    // browser_view->left_side_panel_coordinator()->ReloadWebview();
}

void CRMC_BrowserIF::setBrowser(Browser* pc_Browser)
{
    m_pc_Browser = pc_Browser;

    if (nullptr == m_pc_Browser){
        return;
    }

    m_pc_BrowserView = BrowserView::GetBrowserViewForBrowser(m_pc_Browser);

    PrefService* pc_PrefService = m_pc_Browser->profile()->GetPrefs();

    if (!pc_PrefService){
       // ////DVLOG(0) <<"pref is null";
        return;
    }

    // pc_PrefService->SetBoolean(prefs::kSafeBrowsingEnabled, true);

    // // pc_PrefService->SetInteger(prefs::kNetworkPredictionOptions, chrome_browser_net::NETWORK_PREDICTION_NEVER);

    // pc_PrefService->SetBoolean(translate::prefs::kOfferTranslateEnabled, false);
    
    // pc_PrefService->SetBoolean(spellcheck::prefs::kSpellCheckEnable, false);
    // pc_PrefService->SetBoolean(spellcheck::prefs::kSpellCheckUseSpellingService, false);

    // pc_PrefService->SetBoolean(prefs::kSafeBrowsingScoutReportingEnabled, false);

    pc_PrefService->SetBoolean(bookmarks::prefs::kShowBookmarkBar, false);

    pc_PrefService->SetBoolean(autofill::prefs::kAutofillEnabledDeprecated, false);

    pc_PrefService->SetBoolean(prefs::kScreenCaptureAllowed, false);

    // // pc_PrefService->SetBoolean(prefs::kAudioCaptureAllowedUrls, false);

    pc_PrefService->SetBoolean(prefs::kShowHomeButton, false);

    // pc_PrefService->SetBoolean(prefs::kAllowDeletingBrowserHistory, true);

    // pc_PrefService->SetBoolean(prefs::kFullscreenAllowed, false);

    // pc_PrefService->SetBoolean(reading_list::prefs::kReadingListDesktopFirstUseExperienceShown, false);

}


// bool CRMC_BrowserIF::showLeftSideBar(){
//     if (!m_pc_BrowserView){
//         //DVLOG(0) << " BrowserView null";
//         return false;
//     }
    
//     m_pc_BrowserView->ShowSidePanel();

//     return true;
// }

// bool CRMC_BrowserIF::showRightSideBar(){
//     if (!m_pc_BrowserView){
//         //DVLOG(0) << " BrowserView null";
//         return false;
//     }
    
//     m_pc_BrowserView->ShowSidePanel();

//     return true;
// }

// bool CRMC_BrowserIF::showToolBar(){
//     if (!m_pc_BrowserView){
//         //DVLOG("m_pc_BrowserView null");
//         return false;
//     }

//     m_pc_BrowserView->UpdateCustomTabBarVisibility(true, true);
//     return true;
// }

// bool CRMC_BrowserIF::hideToolBar(){
//     if (!m_pc_BrowserView){
//         //DVLOG("m_pc_BrowserView null");
//         return false;
//     }

//     m_pc_BrowserView->UpdateCustomTabBarVisibility(false, true);
//     return true;
// }


static std::vector<std::string> split(std::string str, char del){
    
    std::string temp = "";
    std::vector<std::string> re_Vec;

    for(int i=0; i<(int)str.size(); i++){
        if(str[i] != del){
            temp += str[i];
        }
        else{
            re_Vec.push_back(temp);
            temp = "";
        }
    }
    if (temp != ""){
        re_Vec.push_back(temp);
    }
    return re_Vec; 
}

static bool checkversion(std::string version){
    std::vector<std::string> v_version = split(version, '.');
    if (v_version.size() != 3){
        DVLOG(0) << "Size is differrent from 3";
        return false;
    }



    if (version.empty()){
        return false;
    }
    

    if(v_version.size() != 3 )
        return false;
    int major = std::stoi(v_version[0]);
    int minor = std::stoi(v_version[1]);
    int build = std::stoi(v_version[2]);

    DVLOG(0) << "";
    if (major > MAJOR_VERSION){
        return true;
    }else if (major == MAJOR_VERSION){
        if (minor > MINOR_VERSION){
            return true;
        } else if (minor == MINOR_VERSION){
            if (build > BUILD_VERSION){
                return true;
            }
        }
    }

    return false;
}

void CRMC_BrowserIF::CheckForUpdate(){
    if (is_check_for_update)
        return;
    // base::CRMC_BaseIPC* g_baseIPC = base::CRMC_BaseIPC::getInstance();

    // if (g_baseIPC){
    //     bool status = g_baseIPC->getUpdateStatus();

    //     if (status){
    //         DVLOG(0) << "Check for update done";
    //     }
    // }

    DVLOG(0) << "CheckForUpdate";
    // static bool first_time_run = true;
    // static base::Time lastest_update = base::Time::Now();
    
    // if (!first_time_run){
    //     base::Time current_time = base::Time::Now();
    //     double last =  lastest_update.ToDoubleT();
    //     double now = current_time.ToDoubleT();
    //     if (now - last < UPDATE_TIME){
    //         return;
    //     }else{
    //         lastest_update = current_time;
    //     }
    // }
    
    // TabStripModel* pc_TabStripModel = m_pc_Browser->tab_strip_model();   
    // content::WebContents* pc_WebContents = pc_TabStripModel->GetActiveWebContents();

    // GURL url = pc_WebContents->GetURL();
    // std::string m_url = url.spec();

    // if (std::string::npos == m_url.find("dones.ai")){
    //     return;
    // }


    DVLOG(0) << "Update";
    CRMC_DownloadJSON* pc_GetInfor = CRMC_DownloadJSON::GetInstance();
    pc_GetInfor->SetUp();

    std::string version = pc_GetInfor->GetVersion();
    DVLOG(0) << version;
    if(checkversion(version) && m_pc_Browser){
        CRMC_UpdateRequestView::Show(m_pc_Browser, version);
    }
    // first_time_run = false;
}


void CRMC_BrowserIF::Update(){
    is_check_for_update = true;
    CRMC_DownloadJSON* pc_GetInfor = CRMC_DownloadJSON::GetInstance();
    std::string url = pc_GetInfor->GetUrl();

    CRMC_DownloadManager* pc_DownloadManager = CRMC_DownloadManager::getInstance();
    if (pc_DownloadManager){
        pc_DownloadManager->StartDownload(m_pc_Browser, url);
    }
}

void CRMC_BrowserIF::NotUpdate(){
    is_check_for_update = false;
}
void CRMC_BrowserIF::Close(){
    DVLOG(0) << "Close";
    // TabStripModel* pc_TabStripModel = m_pc_Browser->tab_strip_model();
    // if (!pc_TabStripModel)
    //     DVLOG(0) << "pc_TabStripModel is null";
    // m_pc_Browser->profile();
    // base::RunLoop run_loop;
    // BrowserList::GetInstance()->CloseAllBrowsersWithProfile(
    //     m_pc_Browser->profile(),
    //     base::BindRepeating(&OnCloseAllBrowsersSucceeded,
    //                         run_loop.QuitClosure()),
    //     BrowserList::CloseCallback(), false);
    //m_pc_Browser->window()->Closes();
    // std::this_thread::sleep_for(std::chrono::milliseconds(3 * 1000));
    chrome::CloseAllBrowsers();
}

void CRMC_BrowserIF::ShowRequestReload(){
    if (!is_check_for_update)
        return;

    CRMC_ReloadRequestView::Show(m_pc_Browser);
}
void CRMC_BrowserIF::RequestReload(){
    if (!is_check_for_update)
        return;

    // CRMC_DownloadManager* pc_DownloadManager = CRMC_DownloadManager::getInstance();
    // if (pc_DownloadManager){
    //     pc_DownloadManager->StartOpenFile();
    // }
    // Close();
}
