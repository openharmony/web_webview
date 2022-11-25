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

#ifndef NWEB_WINDOW_ADAPTER_H
#define NWEB_WINDOW_ADAPTER_H

#include <window.h>

#include <memory>
#include <unordered_map>
#include <vsync_receiver.h>

#include "nweb.h"
#include "nweb_adapter_common.h"

namespace OHOS::NWeb {
class NWebWindowAdapter {
public:
    static NWebWindowAdapter &Instance();
    NWebCreateInfo GetCreateInfo(Rosen::Window *window, const  NWebInitArgs &initArgs);
    void RegistEventCb(Rosen::Window *window, std::shared_ptr<NWeb> nweb);
    void RequestVsync(Rosen::Window *window, std::shared_ptr<NWeb> nweb);

private:
    void GetSize(Rosen::Window *window, NWebCreateInfo &createInfo);
    void GetRenderInterface(Rosen::Window *window, NWebCreateInfo &createInfo);
    void VsyncCb(Rosen::Window *window, std::shared_ptr<NWeb> nweb);

private:
    std::unordered_map<Rosen::Window *, NWebWindowInfo> windowInfoMap_;
    std::shared_ptr<Rosen::VSyncReceiver> receiver_ = nullptr;
};
} // namespace OHOS

#endif // NWEB_WINDOW_ADAPTER_H