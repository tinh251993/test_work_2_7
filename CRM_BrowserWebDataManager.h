#ifndef CHROME_BROWSER_CRM_BROWSERWEBDATAMANAGER_H_
#define CHROME_BROWSER_CRM_BROWSERWEBDATAMANAGER_H_

#include "base/memory/weak_ptr.h"
#include "content/public/browser/storage_partition.h"
#include "net/cookies/canonical_cookie.h"
#include "net/http/http_cache.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/browsing_data_remover.h"
#include "components/browsing_data/content/browsing_data_helper.h"
#include "components/browsing_data/content/cookie_helper.h"
#include "components/browsing_data/content/local_storage_helper.h"
#include "components/browsing_data/content/indexed_db_helper.h"
#include "chrome/browser/browsing_data/local_data_container.h"
#include <list>

static const char* const CRMD_CLOSE_WEBSTRAGE = "localStorage";
static const char* const CRMD_CLOSE_INDEXEDDB = "OfflineStorage";
static const char* const CRMD_CLOSE_COOKIE =    "Cookies";
// ������p�l�� �yIT�s�No.52�zend.
static const char* const CRMD_CookieAttribute_Name		= "name=";		
static const char* const CRMD_CookieAttribute_Value		= "value=";		
static const char* const CRMD_CookieAttribute_Domain		= "domain=";	
static const char* const CRMD_CookieAttribute_Path		= "path=";		
static const char* const CRMD_CookieAttribute_Secure		= "secure=";	
static const char* const CRMD_CookieAttribute_ExpireDate	= "expires=";	

// Cookie�f�[�^�̒l��`.
static const char* const CRMD_CookieDataValueOn = "1";	
static const char* const CRMD_CookieDataValueOff = "0";	
static const char* const CRMD_CookieDataSeparator = ";";	


enum CRME_CookieDataOffset{
	CRME_CookieDataOffset_Name = 0,
	CRME_CookieDataOffset_Value,
	CRME_CookieDataOffset_Domain,
	CRME_CookieDataOffset_Path,
	CRME_CookieDataOffset_Secure,
	CRME_CookieDataOffset_Expires,
	CRME_CookieDataOffset_Max
};

// ���L�������ɐݒ肷��Cookie�폜�ݒ�l�̏���.
enum CRME_DeleteCookieDataOffset{
	CRME_DeleteCookieDataOffset_Domain = 0,
	CRME_DeleteCookieDataOffset_Name,
	CRME_DeleteCookieDataOffset_Max
};

// Cookie�ő�T�C�Y/�����̖����l.
static const int32_t CRMD_CookieInvalidSize = -1;
static const int32_t CRMD_CookieInvalidNum = -1;

// Cookie�ő�T�C�Y�\����.
struct CRMS_CookieSizeInfo {
	int32_t		si_MaxDataSize;		
	int32_t		si_MaxNum;			
};

namespace content {
struct StorageUsageInfo; 
}


class CRMC_BrowserWebDataManagerHelper;
using IndexedDBInfoList = std::list<content::StorageUsageInfo>; 


class CRMC_BrowserWebDataManager : public content::BrowsingDataRemover::Observer {
public:


	CRMC_BrowserWebDataManager(Profile* pc_Profile);

	~CRMC_BrowserWebDataManager() override;

	static CRMC_BrowserWebDataManager* getInstance(Profile* pc_Profile);

	void DeleteAllCache();


	void SetCacheEnable(bool b_Enable, bool b_SaveSetting);

	void GetAllCookiesInfo();

	void getAllLocalStorage();

	void DeleteCookie();

	void DeleteAllCookies();

	void SetCookieSize(CRMS_CookieSizeInfo s_CookieSizeInfo);

	void SetCookieDirectory(const base::FilePath& c_Path);

	void SetCookieDirectoryForClose(const base::FilePath& c_Path);

	void SetWebDataEnable(bool b_IsEnable, bool b_SaveSetting);


	void SetWebDataEnableSpecifiedOrigin(bool b_Enable, const GURL& c_Origin);

	void DeleteLocalStorage(const GURL& c_Origin);

	void DeleteAllLocalStorage();

	void SetLocalStorageDirectory(const base::FilePath c_Path);

	void DeleteIndexedDB(const GURL& c_Origin);

	void OnDeleteIndexedDB(bool success);

	void DeleteAllIndexedDB();

	void SetIndexedDbDirectory(const base::FilePath c_Path);

	void DeleteAuthenticateData();

	void SetLocalStorageDirectoryForClose(const base::FilePath c_Path);

	bool GetCacheEnable();

	bool GetWebDataEnable();

	void DeleteSessionCookiesAsync();

	void OnDeleteSessionCookiesAsync(uint32_t ui_DeleteNum);

	void BackupCookie();

	void RestoreCookie();

	void SetCheckURL(GURL url){m_url = url;}
	static CRMC_BrowserWebDataManager* instance;	

private:

	void DeleteCookieAsync(const net::CookieList& c_CookieList);

	void OnGetAllCookiesInfo(const net::CookieList& c_CookieList);


	void OnGetDeleteCookiesInfo(const net::CookieList& c_CookieList);

	void OnSetCookieDirectory();

	void OnSetCookieDirectoryForClose();

	void OnSetLocalStorageDirectoryForClose();

	void OnSetLocalStorageDirectory();

	void OnFetchAllCookiesData();

	void OnDeleteCookieAsync(bool bl_deleted);  // [HeliosMLK][ChromiumVerUp v73->v77].


	void OnBrowsingDataRemoverDone(uint64_t failed_data_types) override;


	void BrowsingDataRemoverStart();

	void OnBackupCookie();

	void OnGetAllLocalStorage(const std::list<content::StorageUsageInfo>& infos);

	void OnRestoreCookie();

	int32_t convertOriginTypeMask(int32_t si_RemoveMask);

	void OnIndexedDBInfoLoadedForDeleteAll(const IndexedDBInfoList& indexed_db_info_list);

	void OnDeleteIndexedDBForDeleteAll(bool success);

	void OnDeleteAllIndexedDB();

	

private:
	Profile*  m_pc_Profile;
	content::BrowsingDataRemover* m_pc_BrowsingDataRemover;

	std::list<int32_t> m_c_BrowsingDataRemoverList;
	GURL m_url;
	std::string m_c_Domain;
	std::string m_c_Name;

	char* m_psc_ShmCookieData;
	scoped_refptr<browsing_data::CookieHelper> m_pc_CookieHelper;
	scoped_refptr<browsing_data::LocalStorageHelper> m_pc_LocalStorageHelper;
	scoped_refptr<browsing_data::IndexedDBHelper> m_pc_IndexedDBHelper;
// ������p�l�� �yAR�ۑ�1025146�zstart.
	base::FilePath m_c_CookieDirectory;
	base::FilePath m_c_IndexedDbDirectory;
	base::FilePath m_c_LocalStorageDirectory;

	bool m_bl_CacheEnabled;
	bool m_bl_WebDataEnabled;

	int32_t m_si_DeleteAllIndexedDBCount;       
	int32_t m_si_DeleteAllIndexedDBOriginCount; 

	
	
	base::WeakPtrFactory<CRMC_BrowserWebDataManager> m_c_WeakPtrFactory;
};

#endif  // CHROME_BROWSER_CRM_BROWSERWEBDATAMANAGER_H_.
