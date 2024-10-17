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

#ifndef NWEB_FIRST_MEANINGFUL_PAINT_DETAILS_H
#define NWEB_FIRST_MEANINGFUL_PAINT_DETAILS_H

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebFirstMeaningfulPaintDetails {
public:
    NWebFirstMeaningfulPaintDetails() = default;

    virtual ~NWebFirstMeaningfulPaintDetails() = default;

    /**
     * @brief Get start time of navigation.
     *
     * @retval Start time of navigation.
     */
    virtual int64_t GetNavigationStartTime() = 0;

    /**
     * @brief Get paint time of first meaningful content.
     *
     * @retval Paint time of first meaningful content.
     */
    virtual int64_t GetFirstMeaningfulPaintTime() = 0;
};

} // namespace OHOS::NWeb

#endif // NWEB_FIRST_MEANINGFUL_PAINT_DETAILS_H