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

#ifndef ARK_WEB_NATIVE_EMBED_DATA_INFO_H_
#define ARK_WEB_NATIVE_EMBED_DATA_INFO_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "ohos_nweb/include/ark_web_native_embed_info.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkWebNativeEmbedDataInfo : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual int GetStatus() = 0;

  /*--ark web()--*/
  virtual ArkWebString GetEmbedId() = 0;

  /*--ark web()--*/
  virtual ArkWebString GetSurfaceId() = 0;

  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebNativeEmbedInfo> GetNativeEmbedInfo() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_DATA_INFO_H_