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

#ifndef ARK_AI_ENGINE_ADAPTER_H
#define ARK_AI_ENGINE_ADAPTER_H

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=library)--*/
class ArkAiEngineAdapter : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual ArkWebCharVector GetWordSelection(const ArkWebString &text, int8_t offset) = 0;

protected:
  /*--ark web()--*/
  ArkAiEngineAdapter() = default;
  /*--ark web()--*/
  virtual ~ArkAiEngineAdapter() = default;
};
} // namespace OHOS::ArkWeb

#endif // ARK_AI_ENGINE_ADAPTER_H