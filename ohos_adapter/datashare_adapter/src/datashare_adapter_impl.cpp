/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "datashare_adapter_impl.h"

#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>
#include <cstdlib>

#include "nweb_log.h"
#include <filemanagement/file_uri/oh_file_uri.h>

namespace OHOS::NWeb {
// static
DatashareAdapterImpl& DatashareAdapterImpl::GetInstance()
{
    static DatashareAdapterImpl instance;
    return instance;
}

std::string DatashareAdapterImpl::GetRealPath(const std::string& uriStr)
{
    const char *uri = uriStr.c_str();
    unsigned int length = uriStr.length();
    char *uriResult = nullptr;

    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(uri, length, &uriResult);
    if (ret == ERR_OK) {
        std::string realpath(uriResult);
        free(uriResult);
        return realpath;
    } else {
        return std::string();
    }
}

int DatashareAdapterImpl::OpenDataShareUriForRead(const std::string& uriStr)
{
    const char *uri = uriStr.c_str();
    unsigned int length = uriStr.length();
    char *uriResult = nullptr;

    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(uri, length, &uriResult);
    if (ret != ERR_OK) {
        return -1;
    } else {
        int fd = open(uriResult, O_RDONLY);
        free(uriResult);
        return fd;
    }
}

std::string DatashareAdapterImpl::GetFileDisplayName(const std::string& uriStr)
{
    const char *uri = uriStr.c_str();
    unsigned int length = strlen(uri);
    char *name = nullptr;

    FileManagement_ErrCode ret = OH_FileUri_GetFileName(uri, length, &name);
    if (ret == ERR_OK) {
        std::string fileName(name);
        free(name);
        return fileName;
    } else {
        return std::string();
    }
}
} // namespace OHOS::NWeb
