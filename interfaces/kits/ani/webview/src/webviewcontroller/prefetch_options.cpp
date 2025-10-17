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

#include "prefetch_options.h"

#include <securec.h>

#include "nweb_log.h"
#include "business_error.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
PrefetchOptions::PrefetchOptions(ani_env* env)
    : env_(env)
{
    WVLOG_D("PrefetchOptions::PrefetchOptions");
}

PrefetchOptions::~PrefetchOptions()
{
    WVLOG_I("PrefetchOptions::~PrefetchOptions");
    (void)env_;
}

} // namespace NWeb
} // namespace OHOS
