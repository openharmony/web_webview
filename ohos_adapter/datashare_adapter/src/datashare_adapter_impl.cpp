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

#include "nweb_log.h"
#include "foundation/ability/ability_base/interfaces/kits/native/uri/include/uri.h"
#include "foundation/distributeddatamgr/data_share/interfaces/inner_api/consumer/include/datashare_helper.h"
#include "foundation/systemabilitymgr/samgr/interfaces/innerkits/samgr_proxy/include/system_ability_definition.h"
#include "foundation/systemabilitymgr/samgr/interfaces/innerkits/samgr_proxy/include/iservice_registry.h"

namespace OHOS::NWeb {

constexpr char MEDIALIBRARY_DATA_URI[] = "datashare:///media";
// static
DatashareAdapterImpl& DatashareAdapterImpl::GetInstance()
{
    static DatashareAdapterImpl instance;
    return instance;
}

int DatashareAdapterImpl::OpenDataShareUriForRead(const std::string& uriStr) const
{
    auto sam = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        WVLOG_E("open datashare uri read, system ability manager is null");
        return -1;
    }
    auto remoteObj = sam->GetSystemAbility(OHOS::STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        WVLOG_E("open datashare uri read, remoteObj is null");
        return -1;
    }
    auto dataShareHelper = DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI);
    if (dataShareHelper == nullptr) {
        WVLOG_E("open datashare uri read, dataShareHelper is null");
        return -1;
    }
    Uri uri = Uri(uriStr);
    return dataShareHelper->OpenFile(uri, "r");
}

std::string DatashareAdapterImpl::GetFileDisplayName(const std::string& uriStr)
{
    int fd = OpenDataShareUriForRead(uriStr);
    if (fd < 0) {
        WVLOG_E("fd is invaild ,return null");
        return "";
    }

    char buf[PATH_MAX] = {'\0'};
    char filePath[PATH_MAX] = {'\0'};

    int ret = snprintf_s(buf, sizeof(buf), sizeof(buf), "/proc/self/fd/%d", fd);
    if (ret < 0) {
        close(fd);
        WVLOG_E("GetFileDisplayName, snprintf failed with %{public}d", errno);
        return "";
    }

    ret = readlink(buf, filePath, PATH_MAX - 1);
    if (ret < 0) {
        close(fd);
        WVLOG_E("GetFileDisplayName, readlink failed with %{public}d", errno);
        return "";
    }

    close(fd);
    std::string fileName = filePath;
    std::size_t firstSlash = fileName.rfind("/");
    if (firstSlash == fileName.npos) {
        WVLOG_E("GetFileDisplayName, get error path with %{public}s", fileName.c_str());
        return "";
    }
    fileName = fileName.substr(firstSlash + 1, fileName.size() - firstSlash);
    return fileName;
}
}  // namespace OHOS::NWeb
