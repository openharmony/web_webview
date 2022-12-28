/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_HISTORY_LIST_H
#define NWEB_HISTORY_LIST_H

#include <memory>
#include <string>
#include "nweb_export.h"

namespace OHOS::NWeb {
enum class ImageColorType;
enum class ImageAlphaType;
class OHOS_NWEB_EXPORT NWebHistoryItem {
public:
    virtual ~NWebHistoryItem() = default;

    virtual std::string GetHistoryRawUrl() = 0;

    virtual std::string GetHistoryTitle() = 0;

    virtual std::string GetHistoryUrl() = 0;

    virtual bool GetFavicon(void** data, int& width, int& height,
        ImageColorType& colorType, ImageAlphaType& alphaType) = 0;
};

class OHOS_NWEB_EXPORT NWebHistoryList {
public:
    virtual ~NWebHistoryList() = default;

    virtual int32_t GetCurrentIndex() = 0;

    virtual std::shared_ptr<NWebHistoryItem> GetItem(int32_t index) = 0;

    virtual int32_t GetListSize() = 0;
};
}
#endif