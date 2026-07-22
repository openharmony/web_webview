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

#include "native_arkweb_utils.h"

#include <mutex>
#include <unordered_map>

#include "nweb_log.h"

using namespace OHOS;

namespace {
std::unordered_map<std::string, std::weak_ptr<OHOS::NWeb::NWeb>> g_nwebMap;
} // namespace

void OH_NativeArkWeb_BindWebTagToWebInstance(const char* webTag, std::weak_ptr<OHOS::NWeb::NWeb> nwebPtr)
{
    if (!webTag) {
        WVLOG_E("native OH_NativeArkWeb_BindWebTagToWebInstance webTag is null");
        return;
    }
    WVLOG_I("native OH_NativeArkWeb_BindWebTagToWebInstance webTag: %{public}s", webTag);
    g_nwebMap[webTag] = nwebPtr;
}

std::weak_ptr<OHOS::NWeb::NWeb> OH_NativeArkWeb_GetWebInstanceByWebTag(const char* webTag)
{
    if (!webTag) {
        WVLOG_E("native OH_NativeArkWeb_GetWebInstanceByWebTag webTag is null");
        return {};
    }
    return g_nwebMap[webTag];
}
