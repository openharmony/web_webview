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

#ifndef OHOS_ABILITY_RUNTIME_IBROWSER_H
#define OHOS_ABILITY_RUNTIME_IBROWSER_H

#include "iremote_broker.h"
#include "irender_scheduler.h"
#include "surface.h"

namespace OHOS{
namespace NWeb{
/**
 * @class IBrowser
 * Ipc interface of render process to app mgr service
*/
class IBrowser : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.Browser");

    virtual sptr<IRemoteObject> QueryRenderSurface(int32_t surface_id) = 0;

    virtual void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role) = 0;

    virtual void PassSurface(sptr<Surface> surface, int64_t surface_id) = 0;

    virtual void DestroyRenderSurface(int32_t surface_id) = 0;

    enum class Message {
        QUERY_RENDER_SURFACE = 1,
        REPORT_THREAD = 2,
        PASS_SURFACE = 3,
        DESTROY_RENDER_SURFACE = 4,
    };
};
} // namespace NWeb
} // namespace OHOS
#endif // OHOS_ABILITY_RUNTIME_IBROWSER_H