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

#ifndef NWEB_HANDLER_IMPL_TEST_FOR_INTERCEPT_H
#define NWEB_HANDLER_IMPL_TEST_FOR_INTERCEPT_H

#include "nweb_handler.h"

namespace OHOS::NWeb {
class NWebHandlerImplTestForIntercept : public NWebHandler {
public:
    NWebHandlerImplTestForIntercept() = default;
    ~NWebHandlerImplTestForIntercept() = default;
    void SetNWeb(std::shared_ptr<NWeb> nweb) override;
    std::shared_ptr<NWebUrlResourceResponse> OnHandleInterceptRequest(
        std::shared_ptr<NWebUrlResourceRequest> request) override;
private:
    std::weak_ptr<NWeb> nwebweak_;
};
} // namespace OHOS::NWeb

#endif // NWEB_HANDLER_IMPL_TEST_FOR_INTERCEPT_H