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

#ifndef NWEB_ANI_WEBVIEW_AI_PAGE_COMMAND_CALLBACK_H
#define NWEB_ANI_WEBVIEW_AI_PAGE_COMMAND_CALLBACK_H

#include <memory>
#include <string>

#include "ani.h"
#include "event_handler.h"
#include "nweb_value_callback.h"

namespace OHOS::NWeb {

class AniWebviewAIPageCommandCallback : public std::enable_shared_from_this<AniWebviewAIPageCommandCallback>,
                                        public NWebStringValueCallback {
public:
    AniWebviewAIPageCommandCallback(ani_env* env, ani_resolver resolver);
    ~AniWebviewAIPageCommandCallback() = default;

    void OnReceiveValue(const std::string& result) override;

private:
    void ResolvePromise(const std::string& result);

    ani_vm* vm_ = nullptr;
    ani_resolver resolver_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
};

} // namespace OHOS::NWeb

#endif // NWEB_ANI_WEBVIEW_AI_PAGE_COMMAND_CALLBACK_H
