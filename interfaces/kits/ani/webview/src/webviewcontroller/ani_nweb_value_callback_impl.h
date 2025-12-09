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

#ifndef OHOS_NWEB_VALUE_CALLBACKIMPL_H
#define OHOS_NWEB_VALUE_CALLBACKIMPL_H

#include <ani.h>
#include <string>

#include "event_handler.h"
#include "nweb_value_callback.h"
#include "nweb_web_message.h"
#include "nweb_message_ext.h"

namespace OHOS {
namespace NWeb {
class NWebValueCallbackImpl : public NWebMessageValueCallback {
public:
    explicit NWebValueCallbackImpl(ani_vm* vm, ani_fn_object callback, bool extention);
    ~NWebValueCallbackImpl();
    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;
    void OnReceiveValueV2(std::shared_ptr<NWebHapValue> value) override;

private:
    ani_vm* vm_ = nullptr;
    ani_ref callback_ = nullptr;
    bool extention_;
    std::condition_variable condition_;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    void WebMessageCallback(ani_env* env, std::shared_ptr<NWebMessage> result);
    void WebMessageOnReceiveValueCallback(std::shared_ptr<NWebMessage> result);
};
} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_VALUE_CALLBACKIMPL_H