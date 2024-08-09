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

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class NWebMessage;
class NWebWebStorageOrigin;

class OHOS_NWEB_EXPORT NWebBoolValueCallback {
public:
    virtual ~NWebBoolValueCallback() = default;

    virtual void OnReceiveValue(bool value) = 0;
};

class OHOS_NWEB_EXPORT NWebLongValueCallback {
public:
    virtual ~NWebLongValueCallback() = default;

    virtual void OnReceiveValue(long value) = 0;
};

class OHOS_NWEB_EXPORT NWebStringValueCallback {
public:
    virtual ~NWebStringValueCallback() = default;

    virtual void OnReceiveValue(const std::string& value) = 0;
};

class OHOS_NWEB_EXPORT NWebStringVectorValueCallback {
public:
    virtual ~NWebStringVectorValueCallback() = default;

    virtual void OnReceiveValue(const std::vector<std::string>& value) = 0;
};

class OHOS_NWEB_EXPORT NWebMessageValueCallback {
public:
    virtual ~NWebMessageValueCallback() = default;

    virtual void OnReceiveValue(std::shared_ptr<NWebMessage> value) = 0;
};

class OHOS_NWEB_EXPORT NWebWebStorageOriginVectorValueCallback {
public:
    virtual ~NWebWebStorageOriginVectorValueCallback() = default;

    virtual void OnReceiveValue(const std::vector<std::shared_ptr<NWebWebStorageOrigin>>& value) = 0;
};

class OHOS_NWEB_EXPORT NWebArrayBufferValueCallback {
public:
    virtual ~NWebArrayBufferValueCallback() = default;

    virtual void OnReceiveValue(const char* value, const long size) = 0;
};
} // namespace OHOS::NWeb

#endif // NWebValueCallback