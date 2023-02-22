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

#ifndef DATASHARE_ADAPTER_IMPL_H
#define DATASHARE_ADAPTER_IMPL_H

#include "datashare_adapter.h"

namespace OHOS::NWeb {

class DatashareAdapterImpl : public DatashareAdapter {
public:
    static DatashareAdapterImpl& GetInstance();

    ~DatashareAdapterImpl() override = default;

    int OpenDataShareUriForRead(const std::string& uriStr) const override;
    std::string GetFileDisplayName(const std::string& uriStr) override;

private:
    DatashareAdapterImpl() = default;

    DatashareAdapterImpl(const DatashareAdapterImpl& other) = delete;

    DatashareAdapterImpl& operator=(const DatashareAdapterImpl&) = delete;
};

}  // namespace OHOS::NWeb

#endif  // DATASHARE_ADAPTER_IMPL_H