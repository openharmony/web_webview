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

#ifndef SURFACE_CONTROL_SURFACE_CONTROL_H_
#define SURFACE_CONTROL_SURFACE_CONTROL_H_

#include <atomic>
#include <memory>
#include <refbase.h>
#include <vector>
#include <mutex>
#include "commonlibrary/c_utils/base/include/unique_fd.h"
#include "surface_type.h"
#include "common/rs_common_def.h"
#include "surface_buffer.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

struct NativeWindow;
namespace OHOS {
class SurfaceBuffer;
} // namespace OHOS

namespace OHOS::NWeb {
class RSNode;
class RSProxyNode;
class RSSurfaceNode;
class RSUIContext;

class RSC_EXPORT SurfaceControl : public RefBase {
public:
    using BufferReleaseCallback = std::function<void(int releaseFenceFd)>;

    static sptr<SurfaceControl> CreateFromWindow(NativeWindow* window, const char* name);
    static sptr<SurfaceControl> Create(const char* name);
    static std::shared_ptr<OHOS::Rosen::RSUIContext> GetRSUIContext();

    void SetParent(SurfaceControl* newParent);
    void SetVisibility(bool visibility);
    void SetZOrder(int32_t zOrder);
    void SetBuffer(sptr<SurfaceBuffer> buffer, UniqueFd fenceFd, const BufferReleaseCallback& callback);
    void SetBounds(float x, float y, float w, float h);
    void SetFrameGravity(int32_t gravity);
    void SetPivot(float x, float y);
    void SetBufferTransform(GraphicTransformType transform);
    void SetTranslate(float translateX, float translateY, float translateZ);
    void SetDamageRegion(std::vector<Rect> rects);
    void SetBufferAlpha(float alpha);
    void SetForegroundColor(float red, float green, float blue, float alpha);
    void SetBackgroundColor(float red, float green, float blue, float alpha);
    void SetBorderWidth(float left, float top, float right, float bottom);
    void SetBorderColor(float red, float green, float blue, float alpha);
    void SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
    void SetName(std::string name);
    void SetHardwareEnableHint(bool enable);
    void SetSrcRect(float x, float y, float w, float h);
    void SetDisplayRect(float x, float y, float w, float h);
    bool IsRootSurface() const;
    void ClearBufferQueueCache(bool cleanAll);

private:
    SurfaceControl(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode,
        std::shared_ptr<OHOS::Rosen::RSNode> parentNode);
    ~SurfaceControl() override;

    SurfaceControl(const SurfaceControl&) = delete;
    SurfaceControl& operator=(const SurfaceControl&) = delete;
    SurfaceControl(SurfaceControl&&) = delete;
    SurfaceControl& operator=(SurfaceControl&&) = delete;

    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<OHOS::Rosen::RSNode> parentNode_ = nullptr;

    static inline std::mutex rsUiContextMutex_;
    static inline OHOS::sptr<OHOS::IRemoteObject> connectToRender_ = nullptr;
    static inline std::shared_ptr<OHOS::Rosen::RSUIDirector> rsUIDirector_ = nullptr;
    static void SetConnectToRenderObject(OHOS::sptr<OHOS::IRemoteObject> connectToRender);
};
} // namespace OHOS::NWeb

#endif // SURFACE_CONTROL_SURFACE_CONTROL_H_