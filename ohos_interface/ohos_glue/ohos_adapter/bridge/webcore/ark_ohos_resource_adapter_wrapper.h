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

#ifndef ARK_OHOS_RESOURCE_ADAPTER_WRAPPER_H
#define ARK_OHOS_RESOURCE_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_ohos_resource_adapter.h"
#include "ohos_resource_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosResourceAdapterWrapper : public OHOS::NWeb::OhosResourceAdapter {
public:
    explicit ArkOhosResourceAdapterWrapper(ArkWebRefPtr<ArkOhosResourceAdapter>);

    bool GetRawFileData(const std::string& rawFile, size_t& len, uint8_t** dest, bool isSys = false) override;

    std::shared_ptr<NWeb::OhosFileMapper> GetRawFileMapper(const std::string& rawFile, bool isSys = false) override;

    bool IsRawFileExist(const std::string& rawFile, bool isSys = false) override;

    bool GetRawFileLastModTime(const std::string& rawFile, uint16_t& date, uint16_t& time, bool isSys = false) override;

    bool GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys = false) override;

    std::string GetSystemLanguage() override;

private:
    ArkWebRefPtr<ArkOhosResourceAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // OHOS_RESOURCE_ADAPTER_H
