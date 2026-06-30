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
#ifndef SURFACE_CONTROL_SURFACE_TRANSACTION_HELPER_H_
#define SURFACE_CONTROL_SURFACE_TRANSACTION_HELPER_H_

#include <mutex>
#include <map>
#include "surface_transaction.h"

namespace OHOS {
namespace NWeb {
class SurfaceTransactionCallBackHelper {
public:
    static SurfaceTransactionCallBackHelper& GetInstance();

    void RegisterCallBack(const uint64_t& srcId, const uint64_t& seq, SurfaceTransaction::OnCompleteCallback cb);
    void UnRegisterCallBack(const uint64_t& srcId);
    SurfaceTransaction::OnCompleteCallback GetCallBack(const uint64_t& srcId, const uint64_t& seq);
    void DfxInfo();
private:
    std::mutex mutex_;
    std::map<uint64_t, std::map<uint64_t, SurfaceTransaction::OnCompleteCallback>> callbackMap_;
};
} // namespace NWeb
} // namespace OHOS

#endif // SURFACE_CONTROL_SURFACE_TRANSACTION_HELPER_H_