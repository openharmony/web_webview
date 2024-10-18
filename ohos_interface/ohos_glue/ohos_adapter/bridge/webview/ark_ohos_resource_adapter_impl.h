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

#ifndef ARK_OHOS_RESOURCE_ADAPTER_IMPL_H
#define ARK_OHOS_RESOURCE_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_resource_adapter.h"
#include "ohos_resource_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosResourceAdapterImpl : public ArkOhosResourceAdapter {
public:
    explicit ArkOhosResourceAdapterImpl(std::shared_ptr<OHOS::NWeb::OhosResourceAdapter>);

    bool GetRawFileData(const ArkWebString& rawFile, size_t& len, uint8_t** dest, bool isSys) override;

    ArkWebRefPtr<ArkOhosFileMapper> GetRawFileMapper(const ArkWebString& rawFile, bool isSys) override;

    bool IsRawFileExist(const ArkWebString& rawFile, bool isSys) override;

    bool GetRawFileLastModTime(const ArkWebString& rawFile, uint16_t& date, uint16_t& time, bool isSys) override;

    bool GetRawFileLastModTime(const ArkWebString& rawFile, time_t& time, bool isSys) override;

    ArkWebString GetSystemLanguage() override;

private:
    std::shared_ptr<OHOS::NWeb::OhosResourceAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkOhosResourceAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_RESOURCE_ADAPTER_IMPL_H
