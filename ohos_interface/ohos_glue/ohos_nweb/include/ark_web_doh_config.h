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

#ifndef ARK_WEB_DOH_CONFIG_H_
#define ARK_WEB_DOH_CONFIG_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebDohConfig : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetMode() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetConfig() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DOH_CONFIG_H_
