/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nweb_handler_impl_test_for_intercept.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include "nweb_test_log.h"
#include "nweb_url_resource_request.h"
#include "nweb_url_resource_response.h"
#include "securec.h"

namespace OHOS::NWeb {
std::string g_data;
void NWebHandlerImplTestForIntercept::SetNWeb(std::shared_ptr<NWeb> nweb)
{
    nwebweak_ = nweb;
}

bool ReadFileToString(const char* path, std::string& data)
{
    TESTLOG_I(" ReadFileToString-1111%{public}s", path);
    // Implementation adapted from base/file_util.cc
    FILE* file = fopen(path, "rb");
    if (!file) {
        return false;
    }
    char buf[1 << 16];
    size_t len;
    while ((len = fread(buf, 1, sizeof(buf), file)) > 0) {
        data.append(buf, len);
    }
    int ret = fclose(file);
    if (ret < 0) {
        TESTLOG_I("fclose error");
    }
    return true;
}

bool GetResourceDir(std::string& dir)
{
    char buff[1024]; // 1024 size
    ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len == -1) {
        return false;
    }

    buff[len] = 0;
    char* pos = strrchr(buff, '/');
    if (!pos) {
        return false;
    }

    if (strcpy_s(pos + 1, len, "files") != 0) {
        TESTLOG_I(" strcpy_s error");
        return false;
    }
    dir = std::string(buff);
    return true;
}

bool LoadBinaryResource(const char* resource_name, std::string& resource_data)
{
    std::string path;
    if (!GetResourceDir(path)) {
        return false;
    }
    path.append("/");
    path.append(resource_name);
    return ReadFileToString(path.c_str(), resource_data);
}

std::shared_ptr<NWebUrlResourceResponse> NWebHandlerImplTestForIntercept::OnHandleInterceptRequest(
    std::shared_ptr<NWebUrlResourceRequest> request)
{
    std::string url = request->Url();
    if (strstr(url.c_str(), ".png") != nullptr) {
        // system/bin/files
        LoadBinaryResource("2.png", g_data); // define 2.png name image
        std::shared_ptr<NWebUrlResourceResponse> response =
            std::make_shared<NWebUrlResourceResponse>("text/html", "UTF-8", g_data);
        return response;
    }
    return nullptr;
}
} // namespace OHOS::NWeb