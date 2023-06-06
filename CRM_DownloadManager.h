#ifndef CHROME_BROWSER_CRM_DOWNLOADMANAGER_H_
#define CHROME_BROWSER_CRM_DOWNLOADMANAGER_H_

#include <string>
#include "content/public/browser/download_manager.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "url/gurl.h"
#include "base/files/file_path.h"

class Browser;


#define CRMD_DL_RESULT_CANCELED  0
#define CRMD_DL_RESULT_NON_ERR  1

#define CRMD_DL_RESULT_FILE_ERR  2
#define CRMD_DL_RESULT_NETWORK_ERR  3

class CRMC_DownloadManager  {
public:
    CRMC_DownloadManager();
    ~CRMC_DownloadManager();
    static CRMC_DownloadManager* getInstance();
    void StartDownload(Browser* pc_Browser, std::string url);
    void CancelDownload(Browser* pc_Browser);
    void OnDownloadUpdated(int64_t sl_Datasize, uint32_t ui_ItemID, uint32_t ui_state);
    void DownloadFinished(download::DownloadInterruptReason e_DownloadInterruptReason, int32_t si_http_status_code);
    void SetDownloadParam_Fixed( const std::string& c_Url, const std::string& c_HttpMethod, const std::string& c_FilePath, uint32_t ui_Datasize, uint32_t ui_DatasizeHi);
    void SetDownloadParam_Variable( const std::string& c_HttpBody, bool b_isEndData);

    void StartOpenFile();
private:
    void EndDownload(int32_t si_Result, int32_t si_Status_code);
    static CRMC_DownloadManager*	m_pc_DownloadManager;
	uint32_t				m_ui_Maxsize = 2097152000;
	uint32_t				m_ui_ItemID;
	Browser*				m_pc_Browser;
	
	std::string				m_c_Url;			
	std::string				m_c_HttpMethod;		
	std::string				m_c_HttpBody;		
	std::string				m_c_FilePath;		
};


#endif  // CHROME_BROWSER_CRM_DOWNLOADMANAGER_H_