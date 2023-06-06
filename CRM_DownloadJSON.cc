// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/CRM_DownloadJSON.h"

#include "chrome/browser/autofill/autofill_gstatic_reader.h"

#include <list>
#include <memory>
#include <utility>

#include "base/json/json_reader.h"
#include "base/no_destructor.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "net/base/load_flags.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "chrome/browser/CRM_DownloadJSON.h"

static const size_t kMaxDownloadSize = 30 * 1024;

CRMC_DownloadJSON::CRMC_DownloadJSON() {}

CRMC_DownloadJSON::~CRMC_DownloadJSON() {}

void CRMC_DownloadJSON::SetUp() {
  setup_called_ = true;
  LoadDataAsList(GURL("https://api.dones.ai/version/version.json"),
                   "current_version");
}

CRMC_DownloadJSON* CRMC_DownloadJSON::GetInstance() {
  static base::NoDestructor<CRMC_DownloadJSON> instance;
  instance->SetUp();
  return instance.get();
}


std::string CRMC_DownloadJSON::GetVersion() const{
  DCHECK(setup_called_);
  return CRMC_DownloadJSON::GetInstance()->lastest_version;
}

std::string CRMC_DownloadJSON::GetUrl() const{
  DCHECK(setup_called_);
  return CRMC_DownloadJSON::GetInstance()->download_url;
}


void CRMC_DownloadJSON::LoadDataAsList(const GURL& url,
                                           const std::string& key) {
  DCHECK(setup_called_);
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
  resource_request->url = url;
  resource_request->method = "GET";
  net::NetworkTrafficAnnotationTag traffic_annotation =
      net::DefineNetworkTrafficAnnotation("CRM_DownloadJS", R"(
        semantics {
          sender: "Autofill"
          description:
            "Downloads data used to decide when to offer Autofill features, "
            "such as allowlists of eligible websites."
          trigger: "Triggered once on Chrome startup."
          data: "None"
          destination: Dones owner service
        }
        policy {
          cookies_allowed: NO
          setting:
            "You can disable this feature by disabling the Payments and "
            "Addresses settings in Chrome's settings under Autofill. This "
            "feature is always enabled unless both Payments AND Addresses "
            "Autofill are disabled."
          chrome_policy {
            AutofillCreditCardEnabled {
                policy_options {mode: MANDATORY}
                AutofillCreditCardEnabled: true
            }
          }
          chrome_policy {
            AutofillAddressEnabled {                                    
                policy_options {mode: MANDATORY}
                AutofillAddressEnabled: true
            }
          }
        }
        comments: "Both the AutofillAddressEnabled and "
          "AutofillCreditCardEnabled policies needs to be disabled for this "
          "network request to be disabled.")");
  auto simple_loader = network::SimpleURLLoader::Create(
      std::move(resource_request), traffic_annotation);
  simple_loader->SetAllowHttpErrorResults(true);

  // Transfer ownership of the loader into |url_loaders_|. Temporarily hang
  // onto the raw pointer to kick off the request;
  // transferring ownership (std::move) invalidates the |simple_loader|
  // variable.
  auto* raw_simple_loader = simple_loader.get();
  url_loaders_.push_back(std::move(simple_loader));
  if (g_browser_process->system_network_context_manager()->HasInstance()) {
    raw_simple_loader->DownloadToString(
        g_browser_process->system_network_context_manager()
            ->GetSharedURLLoaderFactory()
            .get(),
        base::BindOnce(&CRMC_DownloadJSON::OnSimpleLoaderComplete,
                       base::Unretained(this), --url_loaders_.end(), key),
        kMaxDownloadSize);
  }
}

void CRMC_DownloadJSON::OnSimpleLoaderComplete(
    std::list<std::unique_ptr<network::SimpleURLLoader>>::iterator it,
    const std::string& key,
    std::unique_ptr<std::string> response_body) {
  // Move the loader out of the active loaders list.
  DVLOG(0)<<"OnSimpleLoaderComplete \n" + std::string(response_body->c_str()) + "\n" + "End res_body\n";
  std::unique_ptr<network::SimpleURLLoader> simple_loader = std::move(*it);
  url_loaders_.erase(it);
  ParseListJSON(std::move(response_body), key);
}

// static
void CRMC_DownloadJSON::ParseListJSON(
    std::unique_ptr<std::string> response_body,
    const std::string& key) {
  if (!response_body)
    return;
  absl::optional<base::Value> data = base::JSONReader::Read(*response_body);
  if (data == absl::nullopt || !data->is_dict())
    return;


  std::string* raw_result = data->GetDict().FindString(key);
  if ((*raw_result).empty()) {
    return;
  }
  CRMC_DownloadJSON::GetInstance()->SetListClassVariable(*raw_result, key);


  std::string* raw_url_result = data->GetDict().FindString("download_url");
  if ((*raw_url_result).empty()) {
    return;
  }
  // std::vector<std::string> url_result;
  // for (const base::Value& value : *raw_url_result) {
  //   if (value.is_string())
  //     url_result.push_back(value.GetString());
  // }
  CRMC_DownloadJSON::GetInstance()->SetListClassVariable(*raw_url_result, "download_url");

}

void CRMC_DownloadJSON::SetListClassVariable(
    std::string result,
    const std::string& key) {

  
  if (key == "current_version") {
    DVLOG(0) << "lastest_version: "  + result;
    lastest_version = result;
  } else if (key == "download_url") {
    DVLOG(0) << "download_url: "  + result;
    download_url = result;
  }
}
