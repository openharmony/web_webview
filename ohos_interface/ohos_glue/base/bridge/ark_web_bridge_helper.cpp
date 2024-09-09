/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/bridge/ark_web_bridge_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebBridgeHelper::~ArkWebBridgeHelper()
{
    UnloadLibFile();
}

bool ArkWebBridgeHelper::LoadLibFile(int mode, const std::string& libFilePath, bool isPrintLog)
{
    if (libFileHandler_) {
        return true;
    }

    void* libFileHandler = ::dlopen(libFilePath.c_str(), mode);
    if (!libFileHandler) {
        if (isPrintLog) {
            ARK_WEB_WRAPPER_ERROR_LOG("failed to load lib file,lib file path is %{public}s", libFilePath.c_str());
        }

        return false;
    }

    libFileHandler_ = libFileHandler;
    return true;
}

bool ArkWebBridgeHelper::LoadLibFile(int mode, const std::string& libNsName, const std::string& libDirPath,
    const std::string& libFileName, bool isPrintLog)
{
    if (libFileHandler_) {
        return true;
    }

    Dl_namespace dlns;
    dlns_init(&dlns, libNsName.c_str());

    dlns_create(&dlns, libDirPath.c_str());

    void* libFileHandler = dlopen_ns(&dlns, libFileName.c_str(), mode);
    if (!libFileHandler) {
        if (isPrintLog) {
            ARK_WEB_WRAPPER_ERROR_LOG("failed to load lib file,lib file name is "
                                      "%{public}s,lib dir name is %{public}s",
                libFileName.c_str(), libDirPath.c_str());
        }

        return false;
    }

    libFileHandler_ = libFileHandler;
    return true;
}

void ArkWebBridgeHelper::UnloadLibFile()
{
    if (libFileHandler_ != nullptr) {
        ::dlclose(libFileHandler_);
        libFileHandler_ = nullptr;
    }
}

void* ArkWebBridgeHelper::LoadFuncSymbol(const std::string& funcName, bool isPrintLog)
{
    if (!libFileHandler_) {
        if (isPrintLog) {
            ARK_WEB_WRAPPER_ERROR_LOG("lib file handle is nullptr,func name is %{public}s", funcName.c_str());
        }

        return nullptr;
    }

    if (isPrintLog) {
        ARK_WEB_WRAPPER_INFO_LOG("load func %{public}s", funcName.c_str());
    }

    return dlsym(libFileHandler_, funcName.c_str());
}

} // namespace OHOS::ArkWeb
