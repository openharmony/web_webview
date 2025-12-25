/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_BLANKLESS_CALLBACK_IMPL_H_
#define ARK_WEB_BLANKLESS_CALLBACK_IMPL_H_
#pragma once

#include "include/nweb_blankless_callback.h"
#include "ohos_nweb/include/ark_web_blankless_callback.h"

namespace OHOS::ArkWeb {

class ArkWebBlanklessCallbackImpl : public ArkWebBlanklessCallback {
    IMPLEMENT_REFCOUNTING(ArkWebBlanklessCallbackImpl);

public:
    ArkWebBlanklessCallbackImpl(std::shared_ptr<OHOS::NWeb::NWebBlanklessCallback> nweb_blankless_callback);
    ~ArkWebBlanklessCallbackImpl() = default;

    void OnReceiveValue(const ArkWebString& key, int32_t state,
        int64_t timestamp, const ArkWebString& reason) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebBlanklessCallback> nweb_blankless_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_BLANKLESS_CALLBACK_IMPL_H_
