/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
#ifndef OHOS_NWEB_ANI_FULL_SCREEN_VIDEO_OVERLAY_HANDLER_H
#define OHOS_NWEB_ANI_FULL_SCREEN_VIDEO_OVERLAY_HANDLER_H

#include <ani.h>

namespace OHOS {
namespace NWeb {
ani_status StsFullScreenVideoOverlayHandlerInnerInit(ani_env* env);
ani_status StsOverlayCleanerInit(ani_env* env);
} // namespace NWeb
} // namespace OHOS

#endif // OHOS_NWEB_ANI_FULL_SCREEN_VIDEO_OVERLAY_HANDLER_H
