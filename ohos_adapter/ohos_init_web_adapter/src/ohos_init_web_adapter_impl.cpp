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

#include "ohos_init_web_adapter_impl.h"

#include "nweb_log.h"

namespace OHOS::NWeb {
WebRunInitedCallback* OhosInitWebAdapterImpl::webRunInitedCallback_ = nullptr;

WebRunInitedCallback* OhosInitWebAdapterImpl::GetRunWebInitedCallback()
{
    return OhosInitWebAdapterImpl::webRunInitedCallback_;
}

void OhosInitWebAdapterImpl::SetRunWebInitedCallback(WebRunInitedCallback* callback)
{
    OhosInitWebAdapterImpl::webRunInitedCallback_ = callback;
}
}  // namespace OHOS::NWeb
