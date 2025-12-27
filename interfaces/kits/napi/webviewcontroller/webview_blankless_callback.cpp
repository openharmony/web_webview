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

#include "webview_blankless_callback.h"

#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
void WebviewBlanklessCallback::OnReceiveValue(const std::string& key, int32_t state,
    int64_t timestamp, const std::string& reason)
{
    if (!env_ || !callbackRef_) {
        return;
    }

    napi_value callback = nullptr;
    if (napi_get_reference_value(env_, callbackRef_, &callback) != napi_ok) {
        return;
    }

    napi_value setResult[INTEGER_ONE] = {0};
    napi_create_object(env_, &setResult[INTEGER_ZERO]);

    napi_value napiKey = nullptr;
    napi_create_string_utf8(env_, key.c_str(), key.length(), &napiKey);
    napi_set_named_property(env_, setResult[INTEGER_ZERO], "key", napiKey);

    napi_value napiState = nullptr;
    napi_create_int32(env_, state, &napiState);
    napi_set_named_property(env_, setResult[INTEGER_ZERO], "state", napiState);

    napi_value napiTime = nullptr;
    napi_create_int64(env_, timestamp, &napiTime);
    napi_set_named_property(env_, setResult[INTEGER_ZERO], "timestamp", napiTime);

    napi_value napiReason = nullptr;
    napi_create_string_utf8(env_, reason.c_str(), reason.length(), &napiReason);
    napi_set_named_property(env_, setResult[INTEGER_ZERO], "reason", napiReason);

    napi_value callbackResult = nullptr;
    napi_call_function(env_, nullptr, callback, INTEGER_ONE, setResult, &callbackResult);

    if (state == 1 || state == 2) { // 1 is failed, 2 is frame interpolation removed.
        napi_delete_reference(env_, callbackRef_);
    }
}

} // namespace OHOS::NWeb
