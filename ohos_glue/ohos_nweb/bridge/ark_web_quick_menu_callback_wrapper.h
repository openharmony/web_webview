/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_QUICK_MENU_CALLBACK_WRAPPER_H_
#define ARK_WEB_QUICK_MENU_CALLBACK_WRAPPER_H_
#pragma once

#include "base/include/ark_web_types.h"
#include "include/nweb_context_menu_params.h"
#include "ohos_nweb/include/ark_web_quick_menu_callback.h"

namespace OHOS::ArkWeb {

using ArkWebMenuEventFlags = OHOS::NWeb::MenuEventFlags;

class ArkWebQuickMenuCallbackWrapper
    : public OHOS::NWeb::NWebQuickMenuCallback {
public:
  ArkWebQuickMenuCallbackWrapper(
      ArkWebRefPtr<ArkWebQuickMenuCallback> ark_web_quick_menu_callback);
  ~ArkWebQuickMenuCallbackWrapper() = default;

  void Cancel() override;

  void Continue(int32_t command_id, ArkWebMenuEventFlags flag) override;

private:
  ArkWebRefPtr<ArkWebQuickMenuCallback> ark_web_quick_menu_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_QUICK_MENU_CALLBACK_WRAPPER_H_