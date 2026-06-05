/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "full_screen_video_overlay_handler_wrapper.h"

#include "nweb_log.h"

namespace OHOS::NWeb {

FullScreenVideoOverlayHandlerWrapper::FullScreenVideoOverlayHandlerWrapper(
    void* handlerPtr, FullScreenVideoOverlayHandlerVTable vtable)
    : handlerPtr_(handlerPtr), vtable_(vtable)
{
    if (!vtable_.destroy || !vtable_.setVideoSurface ||
        !vtable_.requestMediaControl || !vtable_.addListener) {
        WVLOG_E("FullScreenVideoOverlayHandlerVTable has null function pointers");
        if (vtable_.destroy && handlerPtr_) {
            vtable_.destroy(handlerPtr_);
        }
        handlerPtr_ = nullptr;
    }
}

FullScreenVideoOverlayHandlerWrapper::~FullScreenVideoOverlayHandlerWrapper()
{
    if (handlerPtr_ && vtable_.destroy) {
        vtable_.destroy(handlerPtr_);
        handlerPtr_ = nullptr;
    }
}

void FullScreenVideoOverlayHandlerWrapper::SetVideoSurface(const std::string& surfaceId)
{
    if (handlerPtr_ && vtable_.setVideoSurface) {
        vtable_.setVideoSurface(handlerPtr_, surfaceId.c_str(), surfaceId.size());
    }
}

void FullScreenVideoOverlayHandlerWrapper::RequestMediaControl(int32_t action, const std::string& param)
{
    if (handlerPtr_ && vtable_.requestMediaControl) {
        vtable_.requestMediaControl(handlerPtr_, action, param.c_str(), param.size());
    }
}

void FullScreenVideoOverlayHandlerWrapper::AddListener(FullScreenVideoListenerCallbacks callbacks)
{
    if (handlerPtr_ && vtable_.addListener) {
        vtable_.addListener(handlerPtr_, &callbacks);
    }
}

void* FullScreenVideoOverlayHandlerWrapper::GetHandlerPtr() const
{
    return handlerPtr_;
}

} // namespace OHOS::NWeb
