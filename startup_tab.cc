// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/startup/startup_tab.h"

StartupTab::StartupTab(const GURL& p_url, Type type) : type(type) {
        //change2520
    if (std::string::npos == p_url.spec().find("https://dones.ai/")){
        url = GURL("https://dones.ai/");
    }else{
        url = p_url;
    }
}

StartupTab::~StartupTab() = default;
