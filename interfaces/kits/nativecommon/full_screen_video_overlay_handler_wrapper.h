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

#ifndef BASE_WEB_WEBVIEW_INTERFACES_KITS_NATIVECOMMON_FULL_SCREEN_VIDEO_OVERLAY_HANDLER_WRAPPER_H
#define BASE_WEB_WEBVIEW_INTERFACES_KITS_NATIVECOMMON_FULL_SCREEN_VIDEO_OVERLAY_HANDLER_WRAPPER_H

#include <functional>
#include <memory>
#include <string>

#include "transfer_referenced_count.h"

namespace OHOS::NWeb {

// Callback fields are optional. Unset (default-constructed) std::function fields
// are safe to move/copy but must be checked for validity before invocation.
struct FullScreenVideoListenerCallbacks {
    std::function<void(int32_t status)> OnStatusChanged;
    std::function<void(bool muted)> OnMutedChanged;
    std::function<void(double playbackRate)> OnPlaybackRateChanged;
    std::function<void(double duration)> OnDurationChanged;
    std::function<void(double currentPlayTime)> OnTimeUpdate;
    std::function<void(double bufferedEndTime)> OnBufferedEndTimeChanged;
    std::function<void()> OnEnded;
    std::function<void(int32_t width, int32_t height)> OnVideoSizeChanged;
};

struct FullScreenVideoOverlayHandlerVTable {
    void (*destroy)(void* handlerPtr);
    void (*setVideoSurface)(void* handlerPtr, const char* surfaceId, size_t len);
    void (*requestMediaControl)(void* handlerPtr, int32_t action, const char* param, size_t len);
    void (*addListener)(void* handlerPtr, const FullScreenVideoListenerCallbacks* callbacks);
};

class FullScreenVideoOverlayHandlerWrapper : public TransferReferencedCount {
public:
    explicit FullScreenVideoOverlayHandlerWrapper(void* handlerPtr, FullScreenVideoOverlayHandlerVTable vtable);

    ~FullScreenVideoOverlayHandlerWrapper();

    void SetVideoSurface(const std::string& surfaceId);

    void RequestMediaControl(int32_t action, const std::string& param);

    void AddListener(FullScreenVideoListenerCallbacks callbacks);

    void* GetHandlerPtr() const;

private:
    void* handlerPtr_;
    FullScreenVideoOverlayHandlerVTable vtable_;
};

} // namespace OHOS::NWeb

#endif // BASE_WEB_WEBVIEW_INTERFACES_KITS_NATIVECOMMON_FULL_SCREEN_VIDEO_OVERLAY_HANDLER_WRAPPER_H
