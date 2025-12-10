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

#ifndef NWEB_ANI_WEB_COOKIE_MANAGER_H
#define NWEB_ANI_WEB_COOKIE_MANAGER_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <uv.h>

#include "event_handler.h"
#include "nweb_value_callback.h"

namespace OHOS {
namespace NWeb {
class NWebConfigCookieCallbackImpl : public std::enable_shared_from_this<NWebConfigCookieCallbackImpl>,
                                     public NWebLongValueCallback {
public:
    NWebConfigCookieCallbackImpl(ani_env* env, ani_ref callback, ani_resolver resolver);
    ~NWebConfigCookieCallbackImpl();

    void OnReceiveValue(long result) override;

    ani_env* GetEnv()
    {
        ani_env* env = nullptr;
        if (vm_) {
            vm_->GetEnv(ANI_VERSION_1, &env);
        }
        return env;
    }

    ani_ref GetCallBack()
    {
        return callback_;
    }

    ani_resolver GetResolver()
    {
        return resolver_;
    }
private:
    ani_vm* vm_;
    ani_ref callback_;
    ani_resolver resolver_;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
};

} // namespace NWeb
} // namespace OHOS

#endif // NWEB_ANI_WEB_COOKIE_MANAGER_H