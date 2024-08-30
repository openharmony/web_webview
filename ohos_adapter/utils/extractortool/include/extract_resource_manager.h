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

#ifndef OHOS_ABILITY_BASE_EXTRACT_RESOURCE_MANAGER_H
#define OHOS_ABILITY_BASE_EXTRACT_RESOURCE_MANAGER_H

#include <memory>
#include "resource_manager.h"

namespace OHOS {
namespace AbilityBase {
class ExtractResourceManager {
public:
    ExtractResourceManager(const ExtractResourceManager&) = delete;

    ExtractResourceManager& operator=(const ExtractResourceManager&) = delete;
    
    static ExtractResourceManager& GetExtractResourceManager();

    void SetGlobalObject(const std::shared_ptr<Global::Resource::ResourceManager> &resourceManager);

    std::shared_ptr<Global::Resource::ResourceManager> GetGlobalObject();

private:
    ExtractResourceManager();

    ~ExtractResourceManager();

    std::shared_ptr<Global::Resource::ResourceManager> resourceManager_;
};
}  // namespace AbilityBase
}  // namespace OHOS
#endif  // OHOS_ABILITY_BASE_EXTRACT_RESOURCE_MANAGER_H