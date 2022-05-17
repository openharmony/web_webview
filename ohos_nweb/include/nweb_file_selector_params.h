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

#ifndef NWEB_FILE_SELECTOR_PARAMS_H
#define NWEB_FILE_SELECTOR_PARAMS_H

#include <string>
#include <vector>
#include "nweb_export.h"

namespace OHOS::NWeb {
using AcceptTypeList = std::vector<std::string>;
class OHOS_NWEB_EXPORT NWebFileSelectorParams {
public:
    enum FileSelectorMode{
        FILE_OPEN_MODE = 0,
        FILE_OPEN_MULTIPLE_MODE,
        FILE_OPEN_FOLDER_MODE,
        FILE_SAVE_MODE
    };

    NWebFileSelectorParams() = default;

    virtual ~NWebFileSelectorParams() = default;

    virtual const std::string Title() const = 0;

    virtual FileSelectorMode Mode() const = 0;

    virtual const std::string DefaultFilename() const = 0;

    virtual const AcceptTypeList AcceptType() const = 0;

    virtual bool IsCapture() const = 0;
};
}

#endif