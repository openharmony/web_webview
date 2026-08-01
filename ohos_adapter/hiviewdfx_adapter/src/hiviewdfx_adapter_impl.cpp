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

#include "hiviewdfx_adapter_impl.h"

#if defined(hidebug_async_stack)
#include "async_stack.h"
#endif

namespace OHOS::NWeb {
HiViewDfxAdapterImpl& HiViewDfxAdapterImpl::GetInstance()
{
    static HiViewDfxAdapterImpl instance;
    return instance;
}

void HiViewDfxAdapterImpl::DfxSetSubmitterStackId(uint64_t ctx)
{
#if defined(hidebug_async_stack)
    ::DfxSetSubmitterStackId(ctx);
#else
    (void)ctx;
#endif
}

void HiViewDfxAdapterImpl::DfxPopSubmitterStackId(uint64_t ctx)
{
#if defined(hidebug_async_stack)
    ::DfxPopSubmitterStackId(ctx);
#else
    (void)ctx;
#endif
}

void HiViewDfxAdapterImpl::ReleaseAsyncContext(uint64_t ctx)
{
#if defined(hidebug_async_stack)
    ::ReleaseAsyncContext(ctx);
#else
    (void)ctx;
#endif
}

uint64_t HiViewDfxAdapterImpl::DfxCollectAsyncStack()
{
#if defined(hidebug_async_stack)
    return ::DfxCollectAsyncStack(ASYNC_TYPE_ARKWEB);
#else
    return 0;
#endif
}
} // namespace OHOS::NWeb
