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

#ifndef NWEB_WEB_MESSAGE_H
#define NWEB_WEB_MESSAGE_H

#include <vector>

#include "nweb_export.h"
#include "nweb_value.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebMessage : public NWebValue {
public:
    explicit NWebMessage(NWebValue::Type type) : NWebValue(type) {}

    ~NWebMessage() = default;

    std::vector<uint8_t> GetBinary() { return binary_data_; }

    void SetBinary(std::vector<uint8_t>& binary_data) {
        binary_data_.reserve(binary_data.size());
        binary_data_ = binary_data;
    }

private:
    std::vector<uint8_t> binary_data_;
};
} // namespace OHOS::NWeb

#endif  // NWEB_WEB_MESSAGE_H
