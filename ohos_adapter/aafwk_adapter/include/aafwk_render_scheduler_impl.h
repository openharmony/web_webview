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

#ifndef AAFWK_RENDER_SCHEDULER_IMPL_H
#define AAFWK_RENDER_SCHEDULER_IMPL_H

#include <memory>
#include "aafwk_render_scheduler_host_adapter.h"
#include "render_scheduler_host.h"

namespace OHOS::NWeb {
class AafwkRenderSchedulerImpl : public OHOS::AppExecFwk::RenderSchedulerHost {
public:
    explicit AafwkRenderSchedulerImpl(std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter);

    ~AafwkRenderSchedulerImpl() override = default;

    void NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd) override;

private:
    std::shared_ptr<AafwkRenderSchedulerHostAdapter> renderSchedulerHostAdapter_;
};
}  // namespace content

#endif  // AAFWK_RENDER_SCHEDULER_IMPL_H