#include "chrome/browser/CRM_DownloadManager.h"
#include "chrome/browser/CRM_BrowserIF.h"
#include "chrome/browser/CRM_DownloadJSON.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "components/download/public/common/download_url_parameters.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/storage_partition.h"

#include "chrome/browser/printing/print_view_manager_common.h"
#include "chrome/browser/printing/print_preview_dialog_controller.h"
#include "chrome/browser/ui/webui/print_preview/print_preview_ui.h"

#include "ui/base/window_open_disposition.h"
#include "mojo/public/c/system/types.h"
#include "chrome/common/chrome_paths.h"
#include "base/path_service.h"
#include "base/memory/ptr_util.h"

#include "chrome/browser/CRM_DownloadObserver.h"

using download::DownloadUrlParameters;
using content::BrowserContext;
using content::DownloadManager;
using content::Referrer;
using content::RenderFrameHost;

CRMC_DownloadManager* CRMC_DownloadManager::m_pc_DownloadManager = NULL;

namespace{



}


CRMC_DownloadManager::CRMC_DownloadManager() :
m_ui_Maxsize(0),
m_ui_ItemID(0),
m_pc_Browser(NULL){

}

CRMC_DownloadManager::~CRMC_DownloadManager(){
    
}

CRMC_DownloadManager* CRMC_DownloadManager::getInstance() {
	if (!m_pc_DownloadManager) {
		m_pc_DownloadManager = new CRMC_DownloadManager();
	}
	return m_pc_DownloadManager;
}


void CRMC_DownloadManager::Init(){
	download_manager = m_pc_Browser->profile()->GetDownloadManager();
	download_manager->AddObserver(this);

	std::vector<download::DownloadItem*> downloads;
	download_manager->GetAllDownloads(&downloads);

	for (std::vector<download::DownloadItem*>::iterator it = downloads.begin();
       it != downloads.end(); ++it) {
    	OnDownloadCreated(download_manager, *it);
  	}
}

void CRMC_DownloadManager::StartDownload(Browser* pc_Browser, std::string url){

    if (NULL == pc_Browser){
        DVLOG(0) << "CRMC_DownloadManager::StartDownload pc_Browser is nullptr";
        return;
    }

    // if (m_c_Url.empty()){
    //     DVLOG(0) << "CRMC_DownloadManager::StartDownload m_c_Url is empty";
    //     return;
    // }

	// if(m_c_HttpMethod.empty()) {
	// 	fprintf(stderr, "CRMC_DownloadManager::StartDownload()  c_HttpMethod is Empty!\n");
	// 	return;
	// }
    
    // if(0 == m_ui_Maxsize) {
        
	// 	fprintf(stderr, "CRMC_DownloadManager::StartDownload()  sl_Datasize is 0!\n");
	// 	return;
	// }

    TabStripModel* pc_TabStripModel = pc_Browser->tab_strip_model();
	if(NULL == pc_TabStripModel) {
		fprintf(stderr, "CRMC_DownloadManager::StartDownload() pc_TabStripModel is NULL!\n");
		return ;
	}

	content::WebContents* pc_WebContents = pc_TabStripModel->GetActiveWebContents();
	if(NULL == pc_WebContents) {
		fprintf(stderr, "CRMC_DownloadManager::StartDownload() pc_WebContents is NULL!\n");
		return ;
	}   
	// content::WaitForLoadStop(tab_strip->GetActiveWebContents());
	// const GURL& c_tempurl = GURL("https://dones.ai/download/dones.exe");
  	// pc_Browser->OpenURL(content::OpenURLParams(
    //   c_tempurl, Referrer(), WindowOpenDisposition::CURRENT_TAB, ui::PAGE_TRANSITION_TYPED, false));
	content::RenderFrameHost* pc_RenderFrameHost = pc_WebContents->GetPrimaryMainFrame();
	if (NULL == pc_RenderFrameHost) {
		fprintf(stderr, "CRMC_DownloadManager::StartDownload() pc_RenderFrameHost is NULL!\n");
		return ;
	}
	m_c_Url = url;
    m_pc_Browser = pc_Browser;
	Init();

	//DownloadManager* download_manager = m_pc_Browser->profile()->GetDownloadManager();
	// std::unique_ptr<content::DownloadObserverTerminal> obs = std::make_unique<content::DownloadObserverTerminal>(
	// 	download_manager, 1,
	// 	content::CRMC_DownloadObserver::ON_DANGEROUS_DOWNLOAD_FAIL);

	// content::CRMC_DownloadObserver* observer(
    //   new content::DownloadObserverTerminal(
    //       download_manager, 1,
    //       content::CRMC_DownloadObserver::ON_DANGEROUS_DOWNLOAD_ACCEPT));


    net::NetworkTrafficAnnotationTag c_traffic_annotation =
      net::DefineNetworkTrafficAnnotation("CRMC_DownloadManager", R"(
        semantics {
          sender: "Download from CRMC_DownloadManager"
          description:
            ""
          trigger:
            ""
          data: ""
          destination: WEBSITE
			}
        policy {
          cookies_allowed: YES
          cookies_store: "user"
          setting: ""
          chrome_policy {
            DownloadRestrictions {
              DownloadRestrictions: 3
		}
	}
    })");

	const GURL& c_tempurl = GURL(url);
	std::unique_ptr<DownloadUrlParameters> c_params(
		new DownloadUrlParameters(
		c_tempurl, pc_RenderFrameHost->GetProcess()->GetID(),
		pc_RenderFrameHost->GetRoutingID(),
		c_traffic_annotation) );

	base::FilePath c_tempfilepath;
	base::PathService::Get(chrome::DIR_DEFAULT_DOWNLOADS, &c_tempfilepath);
		c_tempfilepath = c_tempfilepath.Append(FILE_PATH_LITERAL("Dones.exe"));

	c_params->set_file_path(c_tempfilepath);
	c_params->set_prompt(false);
	c_params->set_transient(false);
	c_params->set_method("GET");
	// c_params->set_referrer_policy(net::ReferrerPolicy::CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE);
	if ("POST"== m_c_HttpMethod) {
		c_params->set_post_body(network::ResourceRequestBody::CreateFromBytes(m_c_HttpBody.data(), m_c_HttpBody.size()));
	}

	m_pc_Browser->profile()->GetDownloadManager()->DownloadUrl(std::move(c_params));

	// observer->WaitForFinished();
	DVLOG(0) << "Download finish";
	// //DownloadManager* manager = m_pc_Browser->profile()->GetDownloadManager();	
 	DownloadManager::DownloadVector items;
  	download_manager->GetAllDownloads(&items);
	// download::DownloadItem* new_item = nullptr;
	for (auto iter = items.begin(); iter != items.end(); ++iter) {
		if ( (*iter)->GetURL().spec().find(url) != std::string::npos){
			if ((*iter)->GetState() == download::DownloadItem::INTERRUPTED || (*iter)->GetState() == download::DownloadItem::CANCELLED){
				DVLOG(0) << url + " ondownload";
				(*iter)->Resume(true);
				break;
			}else if ((*iter)->GetState() == download::DownloadItem::COMPLETE){
				(*iter)->OpenDownload();
				DVLOG(0) << url + " complete";
			}
		}			
	}
	
	// CRMC_BrowserIF::getInstance()->ShowRequestReload();
}	

void CRMC_DownloadManager::StartOpenFile(){

	// DownloadManager* download_manager = m_pc_Browser->profile()->GetDownloadManager();

 	// DownloadManager::DownloadVector items;
  	// download_manager->GetAllDownloads(&items);

	// for (auto iter = items.begin(); iter != items.end(); ++iter) {
	// 	if ( (*iter)->GetURL().spec().find(url) != std::string::npos){
	// 		if ((*iter)->GetState() == download::DownloadItem::INTERRUPTED || (*iter)->GetState() == download::DownloadItem::CANCELLED){
	// 			DVLOG(0) << url + " ondownload";
	// 			(*iter)->Resume(true);
	// 			break;
	// 		}else if ((*iter)->GetState() == download::DownloadItem::COMPLETE){
	// 			(*iter)->OpenDownload();
	// 			DVLOG(0) << url + " complete";
	// 		}
	// 	}			
	// }
	// m_c_Url = url;
	// CRMC_BrowserIF::getInstance()->ShowRequestReload();
}
    // download::DownloadItem::Observer
void CRMC_DownloadManager::OnDownloadUpdated(download::DownloadItem* download){
	download::DownloadItem::DownloadState state =  download->GetState();

	switch (state)
	{
		case download::DownloadItem::DownloadState::COMPLETE :
			if(download->GetURL().spec().find(m_c_Url))
				CRMC_BrowserIF::getInstance()->ShowRequestReload();
			break;
		
		default:
			break;
	}
}
void CRMC_DownloadManager::OnDownloadDestroyed(download::DownloadItem* download){
	DVLOG(0) << "Nothing1";
}

    // // DownloadManager::Observer
void CRMC_DownloadManager::OnDownloadCreated(DownloadManager* manager,
                         download::DownloadItem* item){
	DVLOG(0) << "Nothing";
	OnDownloadUpdated(item);
	item->AddObserver(this);

}
void CRMC_DownloadManager::ManagerGoingDown(DownloadManager* manager){
	DVLOG(0) << "Nothing2";
} 

void CRMC_DownloadManager::CancelDownload(Browser* pc_Browser) {
	if(NULL == pc_Browser) {
		fprintf(stderr, "CRMC_DownloadManager::CancelDownload() pc_Browser is NULL!\n");
		return ;
	}
	
	//�_�E�����[�h�L�����Z��.
	download::DownloadItem* pc_item = pc_Browser->profile()->GetDownloadManager()->GetDownload(m_ui_ItemID);
	if(NULL != pc_item) {
		pc_item->Cancel(true);
	}

    //Endownload without error
	EndDownload(MOJO_RESULT_CANCELLED ,0);
}

// void CRMC_DownloadManager::OnDownloadUpdated(int64_t sl_Datasize, uint32_t ui_ItemID, uint32_t ui_state) {
// 	if( ( NULL == m_pc_Browser ) && ( 0 == m_ui_Maxsize ) ) {
// 		return;
// 	}
// 	m_ui_ItemID = ui_ItemID;

// 	if(download::DownloadItem::INTERRUPTED == ui_state) {
// 		if(NULL == m_pc_Browser) {
// 			fprintf(stderr, "CRMC_DownloadManager::OnDownloadUpdated() m_pc_Browser is NULL!\n");
// 			return ;
// 		}
// 		//�_�E�����[�h�I������.
// 		EndDownload(CRMD_DL_RESULT_CANCELED ,0);
// 	}

// 	if(m_ui_Maxsize < sl_Datasize) {
// 		if(NULL == m_pc_Browser) {
// 			fprintf(stderr, "CRMC_DownloadManager::OnDownloadUpdated() m_pc_Browser is NULL!\n");
// 			return ;
// 		}
		
// 		//�_�E�����[�h�I������.
// 		EndDownload(CRMD_DL_RESULT_FILE_ERR ,0);
// 	}

// 	//CRMC_BrowserIF::getInstance()->notifyDownloadUpdated(sl_Datasize);
// }

// void CRMC_DownloadManager::DownloadFinished(download::DownloadInterruptReason e_DownloadInterruptReason, int32_t si_http_status_code) {
// 	int32_t si_Result = CRMD_DL_RESULT_NON_ERR;

// 	if( ( NULL == m_pc_Browser ) && ( 0 == m_ui_Maxsize ) ) {
// 		// �����o�ϐ�������������Ă���ꍇ�́A���ł�SB�ɒʒm�ς݂Ɣ��f���ď����𔲂���.
// 		if( NULL == m_pc_Browser ) {
// 		}
// 		if( 0 == m_ui_Maxsize ) {
// 		}
// 		return;
// 	}

// 	if( ((100 <= si_http_status_code)&&(200 > si_http_status_code)) ||	
// 		((300 <= si_http_status_code)&&(304 > si_http_status_code)) ||	
// 		((304 <  si_http_status_code)&&(400 > si_http_status_code)) ||
// 		((400 <= si_http_status_code)&&(500 > si_http_status_code)) ||	
// 		((500 <= si_http_status_code)&&(600 > si_http_status_code)))	
// 	{
// 		si_Result = CRMD_DL_RESULT_NETWORK_ERR;
// 	}
	
// 	//�t�@�C���n�G���[�`�F�b�N.
// 	switch(e_DownloadInterruptReason) {
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_FAILED :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_ACCESS_DENIED :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_NO_SPACE :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_NAME_TOO_LONG :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_TOO_LARGE :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_VIRUS_INFECTED :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_TRANSIENT_ERROR :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_BLOCKED :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_SECURITY_CHECK_FAILED :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_TOO_SHORT :
// 		case download::DOWNLOAD_INTERRUPT_REASON_FILE_HASH_MISMATCH :
// 			si_Result = CRMD_DL_RESULT_FILE_ERR;
// 			break;
// 		default:
// 			break;
// 	}

// 	if(NULL == m_pc_Browser) {
// 		fprintf(stderr, "CRMC_DownloadManager::CancelDownload() pc_Browser is NULL!\n");
// 		return ;
// 	}
	
// 	download::DownloadItem* pc_item = m_pc_Browser->profile()->GetDownloadManager()->GetDownload(m_ui_ItemID);
// 	if(NULL != pc_item) {
// 		pc_item->OpenDownload();
// 	}


// 	EndDownload(si_Result, si_http_status_code);
	
// }

void CRMC_DownloadManager::SetDownloadParam_Fixed( const std::string& c_Url, const std::string& c_HttpMethod, const std::string& c_FilePath, uint32_t ui_Datasize, uint32_t ui_DatasizeHi) {
	
	m_c_Url.clear();
	m_c_HttpMethod.clear();
	m_c_HttpBody.clear();
	m_c_FilePath.clear();
	m_ui_Maxsize = 0;
	m_ui_ItemID = 0;

	m_c_Url = c_Url;
	m_c_HttpMethod = c_HttpMethod;
	m_c_FilePath = c_FilePath;
	m_ui_Maxsize = ui_Datasize;
	// uint32_t ui_PaperSizeX = ui_DatasizeHi;
	// uint32_t ui_PaperSizeY = ui_Datasize;
	//base::CRMC_BrowserData::getInstance()->setPaperSize( ui_PaperSizeX, ui_PaperSizeY );
}

void CRMC_DownloadManager::SetDownloadParam_Variable( const std::string& c_HttpBody, bool b_isEndData) {
	m_c_HttpBody += c_HttpBody;
}

// �_�E�����[�h�I������.
void CRMC_DownloadManager::EndDownload(int32_t si_Result, int32_t si_Status_code) {
    DVLOG(0) << "Current do nothing";
    return;

	// m_ui_Maxsize = 0,
	// m_pc_Browser = NULL;
	
	// //SettingBrowser�փ_�E�����[�h�I���ʒm.
	// CRMC_BrowserIF* pc_BrowserIF = CRMC_BrowserIF::getInstance();
	// if(NULL != pc_BrowserIF) {
	// 	pc_BrowserIF->notifyDownloadFinished(si_Result, si_Status_code);
	// }
	// else {
	// 	fprintf(stderr, "CRMC_DownloadManager::EndDownload() CRMC_BrowserIF::getInstance() is NULL!\n");
	// 	return;
	// }
}
