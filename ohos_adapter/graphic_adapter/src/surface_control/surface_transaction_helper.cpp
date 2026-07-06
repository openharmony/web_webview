/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "common/rs_optional_trace.h"
#include "surface_transaction_helper.h"

namespace OHOS {
namespace NWeb {
SurfaceTransactionCallBackHelper& SurfaceTransactionCallBackHelper::GetInstance()
{
    static SurfaceTransactionCallBackHelper instance;
    return instance;
}

void SurfaceTransactionCallBackHelper::DfxInfo()
{
    if (!Rosen::RSSystemProperties::GetDebugTraceEnabled()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    RS_OPTIONAL_TRACE_NAME_FMT("callbackMap size= %u", callbackMap_.size());
    for (auto& iterMap : callbackMap_) {
        RS_OPTIONAL_TRACE_NAME_FMT("srcId=%llu", iterMap.first);
        for (auto& iter : iterMap.second) {
            RS_OPTIONAL_TRACE_NAME_FMT("seqnum=%llu", iter.first);
        }
    }
}

void SurfaceTransactionCallBackHelper::RegisterCallBack(
    const uint64_t& srcId, const uint64_t& seq, SurfaceTransaction::OnCompleteCallback cb)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = callbackMap_.find(srcId);
    if (iter != callbackMap_.end()) {
        iter->second[seq] = cb;
    } else {
        callbackMap_[srcId][seq] = cb;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("RegisterCallBack: srcId=%llu, seq=%llu", srcId, seq);
}

void SurfaceTransactionCallBackHelper::UnRegisterCallBack(const uint64_t& srcId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callbackMap_.erase(srcId);
}

SurfaceTransaction::OnCompleteCallback SurfaceTransactionCallBackHelper::GetCallBack(const uint64_t& srcId,
    const uint64_t& seq)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = callbackMap_.find(srcId);
    if (iter == callbackMap_.end()) {
        return nullptr;
    }

    if (iter->second.find(seq) == iter->second.end()) {
        return nullptr;
    }

    SurfaceTransaction::OnCompleteCallback cb = iter->second[seq];
    iter->second.erase(seq);
    return cb;
}
} // namespace NWeb
} // namespace OHOS