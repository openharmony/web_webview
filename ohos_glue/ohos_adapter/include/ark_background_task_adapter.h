/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_BACKGROUND_TASK_ADAPTER_H
#define ARK_BACKGROUND_TASK_ADAPTER_H

#include "include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--web engine(source=library)--*/
class ArkBackgroundTaskAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkBackgroundTaskAdapter() = default;

    virtual ~ArkBackgroundTaskAdapter() = default;

    /*--web engine()--*/
    static bool RequestBackgroundRunning(bool running, int32_t bgMode);
};
} // namespace OHOS::ArkWeb

#endif // ARK_BACKGROUND_TASK_ADAPTER_H