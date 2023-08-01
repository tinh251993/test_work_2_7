#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <vector>
#include <string>
#include <sstream>

#include "components/content_settings/core/browser/cookie_settings.h"
#include "net/http/http_cache.h"
#include "net/http/http_transaction_factory.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "chrome/browser/content_settings/cookie_settings_factory.h"
#include "chrome/browser/CRM_BrowserWebDataManager.h"
#include "chrome/browser/CRM_BrowserIF.h"

#include "chrome/browser/browsing_data/chrome_browsing_data_remover_delegate.h"

#include "chrome/browser/ui/browser.h"

#include "content/public/browser/browser_task_traits.h"
#include "base/task/bind_post_task.h"

#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"


#include "chrome/browser/browsing_data/cookies_tree_model.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"

#include "chrome/browser/browsing_data/chrome_browsing_data_remover_constants.h"

CRMC_BrowserWebDataManager* CRMC_BrowserWebDataManager::instance  = nullptr;

static std::vector<std::string> CRMG_SplitString(const std::string &c_Str, char sc_Sep)
{
	std::vector<std::string> c_List;
	std::stringstream c_Stream(c_Str);
	std::string c_Buffer;
	while(std::getline(c_Stream, c_Buffer, sc_Sep)) {
		c_List.push_back(c_Buffer);
	}
	return c_List;
}

// ������u��.
static std::string CRMG_ReplaceString(std::string c_BaseStr, std::string c_SearchStr, std::string c_ReplaceStr)
{
	std::string::size_type i_Pos(c_BaseStr.find(c_SearchStr));

	while(i_Pos != std::string::npos)
	{
		c_BaseStr.replace(i_Pos, c_SearchStr.length(), c_ReplaceStr);
		i_Pos = c_BaseStr.find(c_SearchStr, i_Pos + c_ReplaceStr.length());
	}
	return c_BaseStr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
// ChromiumBrowser Web�f�[�^�Ǘ��N���X.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.

// �R���X�g���N�^.
CRMC_BrowserWebDataManager::CRMC_BrowserWebDataManager(Profile* pc_Profile) :
m_pc_Profile(pc_Profile),
m_pc_BrowsingDataRemover(NULL),
m_c_Domain(""),
m_c_Name(""),
m_bl_CacheEnabled(false),
m_bl_WebDataEnabled(false),


m_si_DeleteAllIndexedDBCount(0),
m_si_DeleteAllIndexedDBOriginCount(0),

m_c_WeakPtrFactory(this)
{
	if(NULL != m_pc_Profile) {
		content::StoragePartition* pc_StoragePartition = m_pc_Profile->GetDefaultStoragePartition();
		m_pc_CookieHelper = new browsing_data::CookieHelper(
			pc_StoragePartition,
			CookiesTreeModel::GetCookieDeletionDisabledCallback(m_pc_Profile)
		);
		if(NULL == m_pc_CookieHelper.get()) {
			fprintf(stderr, "[CRMC_BrowserWebDataManager::CRMC_BrowserWebDataManager] m_pc_CookieHelper NULL!\n");
		}
		
		m_pc_LocalStorageHelper = new browsing_data::LocalStorageHelper(pc_StoragePartition);
		if(NULL == m_pc_LocalStorageHelper.get()) {
			fprintf(stderr, "[CRMC_BrowserWebDataManager::CRMC_BrowserWebDataManager] m_pc_LocalStorageHelper NULL!\n");
		}

		m_pc_IndexedDBHelper = new browsing_data::IndexedDBHelper(pc_StoragePartition);
		if(NULL == m_pc_IndexedDBHelper.get()) {
			fprintf(stderr, "[CRMC_BrowserWebDataManager::CRMC_BrowserWebDataManager] m_pc_IndexedDBHelper NULL!\n");
		}
	} else {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::CRMC_BrowserWebDataManager] m_pc_Profile NULL!\n");
	}
	

	m_c_BrowsingDataRemoverList.clear();

	m_c_CookieDirectory.clear();
	m_c_IndexedDbDirectory.clear();
	m_c_LocalStorageDirectory.clear();

}

CRMC_BrowserWebDataManager::~CRMC_BrowserWebDataManager() {

	if(NULL != m_pc_BrowsingDataRemover) {
		m_pc_BrowsingDataRemover->RemoveObserver(this);
	}
}

CRMC_BrowserWebDataManager* CRMC_BrowserWebDataManager::getInstance(Profile* pc_Profile){
	if (nullptr == pc_Profile){
		delete(instance);
		return nullptr;
	}

	if (nullptr == instance){
		instance = new CRMC_BrowserWebDataManager(pc_Profile);
	}

	return instance;
}
// �L���b�V���S�폜.
void CRMC_BrowserWebDataManager::DeleteAllCache() {

	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllCache] m_pc_Profile NULL!\n");

		return;
	}
	m_pc_BrowsingDataRemover = m_pc_Profile->GetBrowsingDataRemover();
	if(NULL == m_pc_BrowsingDataRemover) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllCache] m_pc_BrowsingDataRemover NULL!\n");

		return;
	}

	m_c_BrowsingDataRemoverList.push_back(content::BrowsingDataRemover::DATA_TYPE_CACHE);
	BrowsingDataRemoverStart();

}

void CRMC_BrowserWebDataManager::SetCacheEnable(bool b_Enable, bool b_SaveSetting) {
	if(b_SaveSetting) {
		m_bl_CacheEnabled = b_Enable;
	}
// [������p�l��v2�F�}���`�E�B���h�E�Ή�] End.

// [HeliosMLK][ChromiumVerUp v73->v77]IT�s�No18 start.
	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetCacheEnable] m_pc_Profile NULL!\n");
		return;
	}
	content::StoragePartition* pc_StoragePartition = m_pc_Profile->GetDefaultStoragePartition();
	if (!pc_StoragePartition) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetCacheEnable] pc_StoragePartition is NULL!\n");
		return;
	}
	network::mojom::NetworkContext* pc_NetworkContext = pc_StoragePartition->GetNetworkContext();
	if (!pc_NetworkContext) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetCacheEnable] pc_NetworkContext is NULL!\n");
		return;
	}

}

// �SCookie���擾.
void CRMC_BrowserWebDataManager::GetAllCookiesInfo() {

	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::GetAllCookiesInfo] m_pc_CookieHelper NULL!\n");
		net::CookieList c_CookieList;
		c_CookieList.clear();

		return;
	}

	m_pc_CookieHelper->StartFetching(base::BindOnce(&CRMC_BrowserWebDataManager::OnGetAllCookiesInfo, m_c_WeakPtrFactory.GetWeakPtr()));
}

void CRMC_BrowserWebDataManager::getAllLocalStorage(){
	if (nullptr == m_pc_LocalStorageHelper){
		return;
	}

	DVLOG(0) << "CRMC_BrowserWebDataManager::getAllLocalStorage";
	m_pc_LocalStorageHelper->StartFetching(base::BindOnce(&CRMC_BrowserWebDataManager::OnGetAllLocalStorage, m_c_WeakPtrFactory.GetWeakPtr()));
}

// Cookie�폜.
void CRMC_BrowserWebDataManager::DeleteCookie() {
	std::string c_String(m_psc_ShmCookieData);
	std::vector<std::string> c_List = CRMG_SplitString(c_String, ';');
	m_c_Domain = c_List[CRME_DeleteCookieDataOffset_Domain];
	std::string c_SearchStr(CRMD_CookieAttribute_Domain);
	std::string c_ReplaceStr("");
	m_c_Domain = CRMG_ReplaceString(m_c_Domain, c_SearchStr, c_ReplaceStr);

	m_c_Name = c_List[CRME_DeleteCookieDataOffset_Name];
	c_SearchStr = CRMD_CookieAttribute_Name;
	m_c_Name = CRMG_ReplaceString(m_c_Name, c_SearchStr, c_ReplaceStr);

	m_pc_CookieHelper->StartFetching(base::BindOnce(&CRMC_BrowserWebDataManager::OnGetDeleteCookiesInfo, m_c_WeakPtrFactory.GetWeakPtr()));
}

// Cookie�S�폜.
void CRMC_BrowserWebDataManager::DeleteAllCookies() {
	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllCookies] m_pc_Profile NULL!\n");
		return;
	}
	m_pc_BrowsingDataRemover = m_pc_Profile->GetBrowsingDataRemover();
	if(NULL == m_pc_BrowsingDataRemover) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllCookies] m_pc_BrowsingDataRemover NULL!\n");
		return;
	}

// ������p�l�� �yAR�ۑ�1027455�zstart.
	m_c_BrowsingDataRemoverList.push_back(content::BrowsingDataRemover::DATA_TYPE_COOKIES);
	BrowsingDataRemoverStart();
// ������p�l�� �yAR�ۑ�1027455�zend.
}

// Cookie�T�C�Y�ݒ�.
void CRMC_BrowserWebDataManager::SetCookieSize(CRMS_CookieSizeInfo s_CookieSizeInfo) {

	if(CRMD_CookieInvalidSize == s_CookieSizeInfo.si_MaxDataSize && CRMD_CookieInvalidNum == s_CookieSizeInfo.si_MaxNum) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetCookieSize] InvalidSize!\n");
		return;
	}
}

// Cookie�f�B���N�g���ݒ�.
void CRMC_BrowserWebDataManager::SetCookieDirectory(const base::FilePath& c_Path) {

// ������p�l�� �yAR�ۑ�1025146�zstart.
	if( m_c_CookieDirectory == c_Path ) {
		return;
	}
// ������p�l�� �yAR�ۑ�1025146�zend.

	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetCookieDirectory] m_pc_CookieHelper NULL!\n");
		return;
	}
// ������p�l�� �yAR�ۑ�1025146�zstart.	
	m_c_CookieDirectory = c_Path;
// ������p�l�� �yAR�ۑ�1025146�zend.
}

// ������p�l�� �yAR�ۑ�1027821�zstart.
// Cookie�f�B���N�g���ݒ�(�N���[�Y��p).
void CRMC_BrowserWebDataManager::SetCookieDirectoryForClose(const base::FilePath& c_Path) {

// ������p�l�� �yAR�ۑ�1025146�zstart.
	if( m_c_CookieDirectory == c_Path ) {
// ������p�l�� �yAR�ۑ�1030128�zstart.
		OnSetCookieDirectoryForClose();
// ������p�l�� �yAR�ۑ�1030128�zend.
		return;
	}
// ������p�l�� �yAR�ۑ�1025146�zend.

	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetCookieDirectoryForClose] m_pc_CookieHelper NULL!\n");
		return;
	}
// ������p�l�� �yAR�ۑ�1025146�zstart.
	
	m_c_CookieDirectory = c_Path;

}

void CRMC_BrowserWebDataManager::SetWebDataEnable(bool b_IsEnable, bool b_SaveSetting) {
	if(b_SaveSetting) {
		m_bl_WebDataEnabled = b_IsEnable;
	}
// [������p�l��v2�F�}���`�E�B���h�E�Ή�] End.

	ContentSetting e_Setting = CONTENT_SETTING_ALLOW;
	if(false == b_IsEnable) {
		e_Setting = CONTENT_SETTING_BLOCK;
	}
	CookieSettingsFactory::GetForProfile(m_pc_Profile)->SetDefaultCookieSetting(e_Setting);
}


void CRMC_BrowserWebDataManager::SetWebDataEnableSpecifiedOrigin(bool b_Enable, const GURL& c_Origin) {

	ContentSetting e_Setting = CONTENT_SETTING_ALLOW;
	if(false == b_Enable) {
		e_Setting = CONTENT_SETTING_BLOCK;
	}
	CookieSettingsFactory::GetForProfile(m_pc_Profile)->SetCookieSetting(c_Origin, e_Setting);
}

void CRMC_BrowserWebDataManager::DeleteLocalStorage(const GURL& c_Origin) {

	if(NULL == m_pc_LocalStorageHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteCookie] m_pc_LocalStorageHelper NULL!\n");
	}
}


void CRMC_BrowserWebDataManager::DeleteAllLocalStorage() {
	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllLocalStorage] m_pc_Profile NULL!\n");
		return;
	}
	m_pc_BrowsingDataRemover = m_pc_Profile->GetBrowsingDataRemover();
	if(NULL == m_pc_BrowsingDataRemover) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllLocalStorage] m_pc_BrowsingDataRemover NULL!\n");
		return;
	}


	m_c_BrowsingDataRemoverList.push_back(content::BrowsingDataRemover::DATA_TYPE_LOCAL_STORAGE);
	BrowsingDataRemoverStart();

}

// LocalStorage�f�B���N�g���ݒ�.
void CRMC_BrowserWebDataManager::SetLocalStorageDirectory(const base::FilePath c_Path) {

// ������p�l�� �yAR�ۑ�1025146�zstart.
	if( m_c_LocalStorageDirectory == c_Path ) {
		return;
	}

	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetLocalStorageDirectory] m_pc_Profile NULL!\n");
		return;
	}
	content::DOMStorageContext* pc_context = m_pc_Profile->GetDefaultStoragePartition()->GetDOMStorageContext();
	if(NULL == pc_context) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetLocalStorageDirectory] pc_context NULL!\n");
		return;
	}

	m_c_LocalStorageDirectory = c_Path;

}


bool CRMC_BrowserWebDataManager::GetCacheEnable() {
	return m_bl_CacheEnabled;
}
// Web�f�[�^�L���ݒ�̎擾.
bool CRMC_BrowserWebDataManager::GetWebDataEnable() {
	return m_bl_WebDataEnabled;
}

void CRMC_BrowserWebDataManager::SetLocalStorageDirectoryForClose(const base::FilePath c_Path) {

	if( m_c_LocalStorageDirectory == c_Path ) {
// ������p�l�� �yAR�ۑ�1030128�zstart.
		OnSetLocalStorageDirectoryForClose();
// ������p�l�� �yAR�ۑ�1030128�zend.
		return;
	}
	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetLocalStorageDirectoryForClose] m_pc_Profile NULL!\n");
		return;
	}
	content::DOMStorageContext* pc_context = m_pc_Profile->GetDefaultStoragePartition()->GetDOMStorageContext();
	if(NULL == pc_context) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetLocalStorageDirectoryForClose] pc_context NULL!\n");
		return;
	}

	m_c_LocalStorageDirectory = c_Path;
}

void CRMC_BrowserWebDataManager::DeleteIndexedDB(const GURL& c_Origin) {

	if(NULL == m_pc_IndexedDBHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteIndexedDB] m_pc_IndexedDBHelper NULL!\n");
	}

}


void CRMC_BrowserWebDataManager::OnDeleteIndexedDB(bool success) {
	if (!success) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::OnDeleteIndexedDB] DeleteIndexedDB failure !\n");
	}
}

void CRMC_BrowserWebDataManager::DeleteAllIndexedDB() {

	if (m_si_DeleteAllIndexedDBCount > 0) {
		m_si_DeleteAllIndexedDBCount++;
		return;
	}

	m_si_DeleteAllIndexedDBCount++;
	// �S�폜����ׂ�IndexedDB��Origin�ꗗ���擾����.
	if(NULL == m_pc_IndexedDBHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAllIndexedDB] m_pc_IndexedDBHelper NULL!\n");
		return;
	}
	
	m_pc_IndexedDBHelper->StartFetching(
		base::BindOnce(&CRMC_BrowserWebDataManager::OnIndexedDBInfoLoadedForDeleteAll, base::Unretained(this)));

}


void CRMC_BrowserWebDataManager::BackupCookie() {

	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::BackupCookie] m_pc_CookieHelper NULL!\n");
		return;
	}
}

// Cookie�̃��X�g�A.
void CRMC_BrowserWebDataManager::RestoreCookie() {

	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::RestoreCookie] m_pc_CookieHelper NULL!\n");
		return;
	}
}
// �N���E�hSSO�F�؊J��v1_Step2 End.

// [IT6_2.1MR][ChromiumVerUp v77->v84] [OP_BTS-28363] Start.
// �S�폜�p��IndexedDB��Origin�ꗗ�擾����.
void CRMC_BrowserWebDataManager::OnIndexedDBInfoLoadedForDeleteAll(const IndexedDBInfoList& indexed_db_info_list) {

	// IndexedDB��Origin�̐����擾.
	m_si_DeleteAllIndexedDBOriginCount = indexed_db_info_list.size();
	// �폜������̂��Ȃ��ꍇ�͒����ɉ�����Ԃ�.
	if (0 == m_si_DeleteAllIndexedDBOriginCount) {
		OnDeleteAllIndexedDB();
		return;
	}
}
// IndexedDB�S�폜����Origin�폜�����ʒm.
void CRMC_BrowserWebDataManager::OnDeleteIndexedDBForDeleteAll(bool success) {
	if (!success) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::OnDeleteIndexedDBForDeleteAll] DeleteIndexedDB failure !\n");
	}
	// �S�Ă̍폜�v�����I������ꍇ������Ԃ�.
	m_si_DeleteAllIndexedDBOriginCount--;
	if (0 == m_si_DeleteAllIndexedDBOriginCount) {
		OnDeleteAllIndexedDB();
	}
}

// IndexedDB�폜����.
void CRMC_BrowserWebDataManager::OnDeleteAllIndexedDB() {
	if (m_si_DeleteAllIndexedDBOriginCount != 0) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::OnDeleteAllIndexedDB] DeleteIndexedDB failure !\n");
	}
	m_si_DeleteAllIndexedDBCount = 0;
}
// [IT6_2.1MR][ChromiumVerUp v77->v84] [OP_BTS-28363] End.

// IndexedDB�f�B���N�g���ݒ�.
void CRMC_BrowserWebDataManager::SetIndexedDbDirectory(const base::FilePath c_Path) {

// ������p�l�� �yAR�ۑ�1025146�zstart.
	if( m_c_IndexedDbDirectory == c_Path ) {
		return;
	}
// ������p�l�� �yAR�ۑ�1025146�zend.
	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::SetIndexedDbDirectory] m_pc_Profile NULL!\n");
		return;
	}


	m_c_IndexedDbDirectory = c_Path;

}

// ������p�l�� �yIT�s�No.6�zstart.
// �p�X���[�h�ŕی�T�C�g�ւ̔F�؍ςݏ����폜����.
void CRMC_BrowserWebDataManager::DeleteAuthenticateData() {
	if(NULL == m_pc_Profile) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAuthenticateData] m_pc_Profile NULL!\n");
		return;
	}
	m_pc_BrowsingDataRemover = m_pc_Profile->GetBrowsingDataRemover();
	if(NULL == m_pc_BrowsingDataRemover) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteAuthenticateData] m_pc_BrowsingDataRemover NULL!\n");
		return;
	}


	m_c_BrowsingDataRemoverList.push_back(chrome_browsing_data_remover::DATA_TYPE_PASSWORDS);

	BrowsingDataRemoverStart();

}

void CRMC_BrowserWebDataManager::DeleteCookieAsync(const net::CookieList& c_CookieList) {

	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteCookieAsync] m_pc_CookieHelper NULL!\n");
		return;
	}
	if(true == c_CookieList.empty()) {
		return;
	}

	net::CookieList c_List = c_CookieList;
	for(net::CookieList::iterator c_iterator = c_List.begin(); c_iterator != c_List.end(); ++c_iterator) {
		if((m_c_Domain == c_iterator->Domain()) && (m_c_Name == c_iterator->Name())) {
			return;
		}
	}
	return;
}

// �SCookie���擾����.
void CRMC_BrowserWebDataManager::OnGetAllCookiesInfo(const net::CookieList& c_CookieList) {

	std::string c_String("");
	net::CookieList c_List = c_CookieList;
	std::string check_url = m_url.spec();
	bool check = false;
	for(net::CookieList::iterator c_iterator = c_List.begin(); c_iterator != c_List.end(); ++c_iterator) {

		if (c_iterator->Domain().find("dones.ai") != std::string::npos || c_iterator->Name().find("dones.ai") != std::string::npos || c_iterator->Value().find("dones.ai") != std::string::npos){
			check = true;
			CRMC_BrowserIF::getInstance()->showBookmark(check);
			break;
		}

	}

	
}

void CRMC_BrowserWebDataManager::OnGetAllLocalStorage(const std::list<content::StorageUsageInfo>& infos) {
	//std::vector<GURL> LocalStorageURL;
	bool is_containt_URL = false;
	size_t count = 0;
	bool check = false;
	for (content::StorageUsageInfo infor : infos ){
		//	LocalStorageURL.push_back(infor.storage_key.origin().GetURL());
		if (infor.storage_key.origin().GetURL().spec().find(m_url.spec()) != std::string::npos){
			DVLOG(0) << infor.storage_key.origin().GetURL().spec();
			DVLOG(0) << m_url.spec();
			check = true;
			count++;
		}
	}
	DVLOG(0) << "CRMC_BrowserWebDataManager::OnGetAllLocalStorage : " + std::to_string(is_containt_URL);
	CRMC_BrowserIF::getInstance()->showBookmark(count > 0 && check);
}

// �SCookie���擾����(Cookie�폜�p).
void CRMC_BrowserWebDataManager::OnGetDeleteCookiesInfo(const net::CookieList& c_CookieList) {

	DeleteCookieAsync(c_CookieList);
}

// Cookie�폜����.
// [HeliosMLK][ChromiumVerUp v73->v77] Start.
void CRMC_BrowserWebDataManager::OnDeleteCookieAsync(bool bl_deleted) {

	if(bl_deleted){
		return;
	}

}
// [HeliosMLK][ChromiumVerUp v73->v77] End.
// ������p�l�� �yIT�s�No.48�zstart.
// Cookie�f�B���N�g���ݒ芮��.
void CRMC_BrowserWebDataManager::OnSetCookieDirectory() {
// ������p�l�� �yAR�ۑ�1025146�zstart.
//	CRMC_BrowserIpcIf::getInstance()->resSetCookieDirectory();
	
// ������p�l�� �yAR�ۑ�1025146�zend.
}
// ������p�l�� �yIT�s�No.48�zend.

// ������p�l�� �yAR�ۑ�1025146�zstart.
void CRMC_BrowserWebDataManager::OnFetchAllCookiesData() {

}
// ������p�l�� �yAR�ۑ�1025146�zend.
// ������p�l�� �yAR�ۑ�1029951�zstart.
// Cookie�f�B���N�g���ݒ芮��(�N���[�Y��p).
void CRMC_BrowserWebDataManager::OnSetCookieDirectoryForClose() {
	
}

// LocalStorage�f�B���N�g���ݒ芮��(�N���[�Y��p).
void CRMC_BrowserWebDataManager::OnSetLocalStorageDirectoryForClose() {
	
}
// ������p�l�� �yAR�ۑ�1029951�zend.
// ������p�l�� �yAR�ۑ�1066312�zstart.
// LocalStorage�f�B���N�g���ݒ芮��.
void CRMC_BrowserWebDataManager::OnSetLocalStorageDirectory() {
	// �������Ȃ�.
}
// ������p�l�� �yAR�ۑ�1066312�zend.

// �u���E�U�֘A�f�[�^�폜����.
void CRMC_BrowserWebDataManager::OnBrowsingDataRemoverDone(uint64_t failed_data_types) {

	m_pc_BrowsingDataRemover->RemoveObserver(this);
// ������p�l�� �yAR�ۑ�1027455�zstart.
//	m_pc_BrowsingDataRemover = NULL;

	int32_t si_RemoveMask = m_c_BrowsingDataRemoverList.front();
	m_c_BrowsingDataRemoverList.pop_front();
// ������p�l�� �yAR�ۑ�1027455�zend.

	switch(si_RemoveMask) {
		case content::BrowsingDataRemover::DATA_TYPE_CACHE:
		
			break;
		case content::BrowsingDataRemover::DATA_TYPE_COOKIES:
			
			break;
		case content::BrowsingDataRemover::DATA_TYPE_LOCAL_STORAGE:
			
			break;
		case content::BrowsingDataRemover::DATA_TYPE_INDEXED_DB:
			
			break;
		default:
			fprintf(stderr, "[CRMC_BrowserWebDataManager::OnBrowsingDataRemoverDone] UnknownData!\n");
			break;
	}

// ������p�l�� �yAR�ۑ�1027455�zstart.
	if( false == m_c_BrowsingDataRemoverList.empty() ) {
		BrowsingDataRemoverStart();
	}
// ������p�l�� �yAR�ۑ�1027455�zend.
}

// �u���E�U�֘A�f�[�^�폜�J�n.
// ������p�l�� �yAR�ۑ�1027455�zstart.
void CRMC_BrowserWebDataManager::BrowsingDataRemoverStart() {
// ������p�l�� �yAR�ۑ�1027455�zend.

// ������p�l�� �yAR�ۑ�1027455�zstart.
	// ���Ɏ��s���̏���������ꍇ�͏I���܂ő҂�.
	if( 1 < m_c_BrowsingDataRemoverList.size() ) {
		return;
	}
	int32_t si_RemoveMask = m_c_BrowsingDataRemoverList.front();
// ������p�l�� �yAR�ۑ�1027455�zend.

	m_pc_BrowsingDataRemover->AddObserver(this);
	m_pc_BrowsingDataRemover->RemoveAndReply(base::Time(), base::Time::Max(),
// ������p�l�� �yAR�ۑ�1027455�zstart.
											 si_RemoveMask,
											 convertOriginTypeMask(si_RemoveMask),
// ������p�l�� �yAR�ۑ�1027455�zend.
											 this);

}

// �N���E�hSSO�F�؊J��v1_Step2 Start.
// Cookie�̃o�b�N�A�b�v����.
void CRMC_BrowserWebDataManager::OnBackupCookie() {
	
}

// Cookie�̃��X�g�A����.
void CRMC_BrowserWebDataManager::OnRestoreCookie() {
	
}
// �N���E�hSSO�F�؊J��v1_Step2 End.

// �u���E�U�֘A�f�[�^��Mask�l�ϊ�.
int32_t CRMC_BrowserWebDataManager::convertOriginTypeMask(int32_t si_RemoveMask) {

	int32_t si_OriginTypeMask = 0;
	switch(si_RemoveMask) {
		case content::BrowsingDataRemover::DATA_TYPE_COOKIES:
		case content::BrowsingDataRemover::DATA_TYPE_INDEXED_DB:
		case content::BrowsingDataRemover::DATA_TYPE_LOCAL_STORAGE:
			si_OriginTypeMask = content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB;
			break;
		default:
			break;
	}
	return si_OriginTypeMask;
}
// [HeliosMLK][ChromiumVerUp v73->v77] Start.
// �Z�b�V�����N�b�L�[�̍폜.
void CRMC_BrowserWebDataManager::DeleteSessionCookiesAsync() {
	if(NULL == m_pc_CookieHelper.get()) {
		fprintf(stderr, "[CRMC_BrowserWebDataManager::DeleteSessionCookiesAsync] m_pc_CookieHelper NULL!\n");
		OnDeleteSessionCookiesAsync(0);
		return;
	}
	
}

// �Z�b�V�����N�b�L�[�̍폜����.
void CRMC_BrowserWebDataManager::OnDeleteSessionCookiesAsync(uint32_t ui_DeleteNum) {
	DVLOG(0)<<ui_DeleteNum;
	// CRMC_BrowserIF::getInstance()->notifyFinalize();
}
// [HeliosMLK][ChromiumVerUp v73->v77] End.
