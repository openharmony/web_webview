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

#ifndef VALUE_CALLBACK_H
#define VALUE_CALLBACK_H

#include "nweb_export.h"

namespace OHOS::NWeb {
template<typename T>
class OHOS_NWEB_EXPORT NWebValueCallback {
public:
    NWebValueCallback() = default;

    virtual ~NWebValueCallback() = default;

    virtual void OnReceiveValue(T value) = 0;
};
} // namespace OHOS::NWeb

#endif  // VALUE_CALLBACK_H
