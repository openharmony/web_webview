/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_NATIVE_COMMON_WEB_MESSAGE_PORT_H
#define OHOS_NWEB_NATIVE_COMMON_WEB_MESSAGE_PORT_H
#include "nweb.h"
#include "web_errors.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "transfer_referenced_count.h"

namespace OHOS {
namespace NWeb {

class WebMessagePort : public TransferReferencedCount {
public:
    WebMessagePort(int32_t nwebId, std::string& port, bool isExtentionType);

    ~WebMessagePort() = default;

    ErrCode ClosePort();

    ErrCode PostPortMessage(std::shared_ptr<NWebMessage> data);

    ErrCode SetPortMessageCallback(std::shared_ptr<NWebMessageValueCallback> callback);

    std::string GetPortHandle() const;

    bool IsExtentionType()
    {
        return isExtentionType_;
    }

private:
    int32_t nwebId_ = -1;
    std::string portHandle_;
    bool isExtentionType_;
};

} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_NATIVE_COMMON_WEB_MESSAGE_PORT_H