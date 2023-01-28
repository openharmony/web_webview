/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_HELPER_H
#define NWEB_HELPER_H

#include <iosfwd>
#include <memory>
#include <string>
#include "nweb.h"
#include "nweb_cookie_manager.h"
#include "nweb_data_base.h"
#include "nweb_export.h"
#include "nweb_web_storage.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebHelper {
public:
    static NWebHelper &Instance();
    ~NWebHelper();
    bool Init(bool from_ark = true);
    static void TryPreReadLib(bool isFirstTimeStartUpWeb, const std::string &bundlePath);

    std::shared_ptr<NWeb> CreateNWeb(const NWebCreateInfo &create_info);
    NWebCookieManager *GetCookieManager();
    NWebDataBase *GetDataBase();
    NWebWebStorage *GetWebStorage();
    std::weak_ptr<NWeb> GetNWeb(int32_t nweb_id);
    void SetBundlePath(const std::string &path);

private:
    NWebHelper() = default;
    bool LoadLib(bool from_ark);
    void UnloadLib();

private:
    void *libHandleWebEngine_ = nullptr;
    std::string bundlePath_;
};
} // namespace OHOS::NWeb

#endif // NWEB_HELPER_H
