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
#include "web_history_list.h"

namespace OHOS {
namespace NWeb {

std::shared_ptr<NWebHistoryItem> WebHistoryList::GetItem(int32_t index)
{
    if (!sptrHistoryList_) {
        return nullptr;
    }
    return sptrHistoryList_->GetItem(index);
}

int32_t WebHistoryList::GetListSize()
{
    int32_t listSize = 0;

    if (!sptrHistoryList_) {
        return listSize;
    }
    listSize = sptrHistoryList_->GetListSize();
    return listSize;
}

} // namespace NWeb
} // namespace OHOS