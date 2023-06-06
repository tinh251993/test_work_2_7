#ifndef CHROME_BROWSER_CRM_DOWNLOADJSON_H_
#define CHROME_BROWSER_CRM_DOWNLOADJSON_H_

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "base/gtest_prod_util.h"
#include "components/keyed_service/core/keyed_service.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "chrome/browser/CRM_DownloadJSON.h"

// Fetches and parses JSON data from Gstatic URLs, and stores them
// so that they can be accessed later.
class CRMC_DownloadJSON {
 public:
  CRMC_DownloadJSON();

  ~CRMC_DownloadJSON();

  // Sets up the class by loading data from the Gstatic URLs if setup hasn't
  // been attempted yet. Should be called only when ChromeAutofillClient
  // starts up and Autofill is enabled.
  // After it has been called, subsequent calls won't have any effect.
  void SetUp();

  static CRMC_DownloadJSON* GetInstance();

  std::string GetVersion() const;

  std::string GetUrl() const;

  std::string lastest_version ="";

  // Merchant domains which are eligible for cloud tokenization.
  std::string download_url = "";

 private:


  // Fetches data stored at |url| which have the following JSON format:
  // { "key": ["list_item_one", "list_item2", ...]}
  // The entry with |key| as key will be saved if it is present.
  void LoadDataAsList(const GURL& url, const std::string& key);

  // Callback which receives the content of |url| from LoadDataAsList(~).
  void OnSimpleLoaderComplete(
      std::list<std::unique_ptr<network::SimpleURLLoader>>::iterator it,
      const std::string& key,
      std::unique_ptr<std::string> response_body);

  // Parses and returns list of strings which are in the format of a
  // list value to a JSON key.
  static void ParseListJSON(
      std::unique_ptr<std::string> response_body,
      const std::string& key);

  void SetListClassVariable(std::string result,
                            const std::string& key);

  bool setup_called_ = false;

  // BIN ranges which are eligible for cloud tokenization.


  // Loaders used for the processing the requests. Each loader is removed upon
  // completion.
  std::list<std::unique_ptr<network::SimpleURLLoader>> url_loaders_;
};



#endif  // CHROME_BROWSER_CRM_DOWNLOADJSON_H_