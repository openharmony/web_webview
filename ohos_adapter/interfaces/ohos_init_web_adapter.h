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

#ifndef OHOS_INIT_WEB_ADAPTER_H
#define OHOS_INIT_WEB_ADAPTER_H

#include <functional>
#include <string>
#include <sys/types.h>

namespace OHOS::NWeb {
class WebRunInitedCallback {
public:
    WebRunInitedCallback() = default;

    virtual ~WebRunInitedCallback() = default;

    virtual void RunInitedCallback() = 0;
};

class OhosInitWebAdapter {
public:
    OhosInitWebAdapter() = default;
    virtual ~OhosInitWebAdapter() = default;

    virtual WebRunInitedCallback* GetRunWebInitedCallback() = 0;

    virtual void SetRunWebInitedCallback(WebRunInitedCallback* &&callback) = 0;
};
}  // namespace OHOS::NWeb

#endif  // OHOS_INIT_WEB_ADAPTER_H