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

#include "surface_control.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include "surface_buffer.h"
#include "surface/window.h"
#include "native_window.h"
#include "rs_trace.h"
#include "nweb_log.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context_manager.h"
#include "ui/rs_ui_director.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace NWeb {
namespace {
#define CHECK_NULL_POINTER(ptr) \
    do { \
        if (!(ptr)) { \
            WVLOG_E(#ptr " is null pointer"); \
            return; \
        } \
    } while (0)

#define API_COMPATIBLE_VERSION 18 // compatible with RS version 1.8, set by surface node
class RSSurfaceNodeReleaseBufferWoker {
public:
    static RSSurfaceNodeReleaseBufferWoker& GetInstance();

    void RegisterNode(std::shared_ptr<Rosen::RSSurfaceNode> nodePtr);
    void UnRegisterNode(std::shared_ptr<Rosen::RSSurfaceNode> nodePtr);
private:
    RSSurfaceNodeReleaseBufferWoker();
    ~RSSurfaceNodeReleaseBufferWoker();

    void RegisterReleaseBufferListener();
    void UnRegisterReleaseBufferListener();
    std::shared_ptr<Rosen::RSSurfaceNode> LocalGetNodeById(NodeId id);
    std::mutex mutex;
    std::unordered_map<NodeId, std::weak_ptr<Rosen::RSSurfaceNode>> surfaceNodeMap_;
};

std::shared_ptr<Rosen::RSSurfaceNode> CreateSurfaceNode(
    const char* name, std::shared_ptr<Rosen::RSUIContext> uiContext, RSSurfaceNodeType type, bool isWindow)
{
    RSSurfaceNodeConfig config = {
        .SurfaceNodeName = name,
        .isSync = true,
    };
    auto surfaceNode = Rosen::RSSurfaceNode::Create(config, type, isWindow, false, uiContext);
    if (surfaceNode) {
        surfaceNode->SetApiCompatibleVersion(API_COMPATIBLE_VERSION);
        surfaceNode->SetHardwareEnabled(true);
        surfaceNode->SetFrameGravity(Gravity::TOP_LEFT);
        WVLOG_I("CreateSurfaceNode success. name: %{public}s", name);
        return surfaceNode;
    }

    WVLOG_E("CreateSurfaceNode fail: %{public}s", name);
    return nullptr;
}

uint32_t ToARGB(float red, float green, float blue, float alpha)
{
    red = std::clamp(red, 0.0f, 1.0f);
    green = std::clamp(green, 0.0f, 1.0f);
    blue = std::clamp(blue, 0.0f, 1.0f);
    alpha = std::clamp(alpha, 0.0f, 1.0f);

    constexpr uint8_t channelMax = 255;
    constexpr uint8_t alphaShift = 24;
    constexpr uint8_t redShift = 16;
    constexpr uint8_t greenShift = 8;
    constexpr uint8_t blueShift = 0;

    return static_cast<uint32_t>(alpha * channelMax) << alphaShift |
           static_cast<uint32_t>(red * channelMax) << redShift |
           static_cast<uint32_t>(green * channelMax) << greenShift |
           static_cast<uint32_t>(blue * channelMax) << blueShift;
}

class ScopedTransaction {
public:
    ScopedTransaction()
    {
        auto uiContext = SurfaceControl::GetRSUIContext();
        if (uiContext && uiContext->GetRSTransaction()) {
            transaction_ = uiContext->GetRSTransaction();
            if (transaction_) {
                transaction_->Begin();
            } else {
                WVLOG_E("GetRSTransaction failed.");
            }
        } else {
            WVLOG_E("UIContext is nullptr or GetRSTransaction failed.");
        }
    }

    ~ScopedTransaction()
    {
        if (transaction_) {
            transaction_->Commit();
        }
    }

private:
    std::shared_ptr<RSTransactionHandler> transaction_;
};
} // namespace

std::shared_ptr<OHOS::Rosen::RSUIContext> SurfaceControl::GetRSUIContext()
{
    std::lock_guard<std::mutex> lock(rsUiContextMutex_);
    if (SurfaceControl::rsUIDirector_ == nullptr && SurfaceControl::connectToRender_ != nullptr) {
        RS_TRACE_NAME("Create rsUIDirector");
        SurfaceControl::rsUIDirector_ = OHOS::Rosen::RSUIDirector::Create(SurfaceControl::connectToRender_);
        RS_TRACE_NAME_FMT("Create rsUIDirector: %s", (SurfaceControl::rsUIDirector_ != nullptr ? "ok" : "fail"));
    }
    return SurfaceControl::rsUIDirector_ ? SurfaceControl::rsUIDirector_->GetRSUIContext() : nullptr;
}

void SurfaceControl::SetConnectToRenderObject(OHOS::sptr<OHOS::IRemoteObject> connectToRender)
{
    std::lock_guard<std::mutex> lock(rsUiContextMutex_);
    SurfaceControl::connectToRender_ = connectToRender;
}

sptr<SurfaceControl> SurfaceControl::CreateFromWindow(NativeWindow* window, const char* name)
{
    if (window == nullptr) {
        WVLOG_E("CreateFromWindow failed, window is nullptr");
        return nullptr;
    }
    OHOS::sptr<OHOS::Surface> surface = window->surface;
    if (surface == nullptr) {
        WVLOG_E("CreateFromWindow failed, surface is nullptr");
        return nullptr;
    }

    std::string fetchedRSHandle = surface->GetUserData("delegate_connect_to_render");
    sptr<IRemoteObject> connectToRender = nullptr;
    if (!fetchedRSHandle.empty()) {
        uint64_t handle = static_cast<uint64_t>(std::stoull(fetchedRSHandle));
        if (handle != 0) {
            IRemoteObject* rawPtr = reinterpret_cast<IRemoteObject*>(handle);
            connectToRender = sptr<IRemoteObject>(rawPtr);
            rawPtr->DecStrongRef(nullptr);
        }
    }

    std::string fetchedNodeId = surface->GetUserData("delegate_node_id");
    uint64_t nodeId = 0;
    if (!fetchedNodeId.empty()) {
        nodeId = std::stoull(fetchedNodeId);
    }

    WVLOG_I("CreateFromWindow nodeId: %{public}" PRIu64 ", connectToRender: %{public}p",
        nodeId, connectToRender.GetRefPtr());
    if (nodeId == 0) {
        WVLOG_E("node id is invalid");
        return nullptr;
    }
    SurfaceControl::SetConnectToRenderObject(connectToRender);
    ScopedTransaction scopedTransaction;

    auto uiContext = SurfaceControl::GetRSUIContext();
    if (!uiContext) {
        WVLOG_E("UIContext is nullptr.");
        return nullptr;
    }

    auto parentNode = uiContext->GetNodeMap().GetNode(nodeId);
    if (!parentNode) {
        WVLOG_E("ParentNode is nullptr, reCreate in nodeId=[%{public}" PRIu64 "]", nodeId);
        parentNode = Rosen::RSProxyNode::Create(nodeId, "root_proxy", uiContext);
    }

    if (!parentNode) {
        WVLOG_E("Recreate parent failed.");
        return nullptr;
    }

    auto surfaceNode = CreateSurfaceNode(name, uiContext, RSSurfaceNodeType::SELF_DRAWING_NODE, true);
    if (!surfaceNode) {
        WVLOG_E("CreateSurfaceNode failed");
        return nullptr;
    }
    parentNode->OHOS::Rosen::RSNode::AddChild(surfaceNode, -1);
    WVLOG_I("CreateFromWindow success, nodeId: %{public}" PRIu64, nodeId);
    return new SurfaceControl(std::move(surfaceNode), std::move(parentNode));
}

sptr<SurfaceControl> SurfaceControl::Create(const char* name)
{
    ScopedTransaction scopedTransaction;
    auto uiContext = SurfaceControl::GetRSUIContext();
    if (!uiContext) {
        WVLOG_E("UIContext is nullptr.");
        return nullptr;
    }
    auto surfaceNode = CreateSurfaceNode(name, uiContext, RSSurfaceNodeType::SELF_DRAWING_NODE, false);
    return new SurfaceControl(std::move(surfaceNode), std::shared_ptr<OHOS::Rosen::RSNode>());
}

SurfaceControl::SurfaceControl(
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode, std::shared_ptr<OHOS::Rosen::RSNode> parentNode)
    : surfaceNode_(std::move(surfaceNode)), parentNode_(std::move(parentNode))
{
    if (surfaceNode_) {
        surfaceNode_->SetPivot(0, 0);
        RSSurfaceNodeReleaseBufferWoker::GetInstance().RegisterNode(surfaceNode_);
    } else {
        WVLOG_E("Constructor surfaceNode is nullptr");
    }

    if (!parentNode_) {
        WVLOG_W("Constructor parentNode is nullptr");
    }
}

SurfaceControl::~SurfaceControl()
{
    RSSurfaceNodeReleaseBufferWoker::GetInstance().UnRegisterNode(surfaceNode_);
    if (parentNode_) {
        parentNode_->OHOS::Rosen::RSNode::RemoveChild(surfaceNode_);
    }
}

void SurfaceControl::SetParent(SurfaceControl* newParent)
{
    if (parentNode_) {
        parentNode_->OHOS::Rosen::RSNode::RemoveChild(surfaceNode_);
    }
    if (newParent && newParent->surfaceNode_) {
        // -1 means index.
        newParent->surfaceNode_->OHOS::Rosen::RSNode::AddChild(surfaceNode_, -1);
        parentNode_ = newParent->surfaceNode_;
    } else {
        WVLOG_W("NewParent is nullptr or surfaceNode_ is nullptr");
    }
}

void SurfaceControl::SetVisibility(bool visibility)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetVisible(visibility);
}

void SurfaceControl::SetZOrder(int32_t zOrder)
{
    CHECK_NULL_POINTER(surfaceNode_);
    if (auto parent = surfaceNode_->GetParent()) {
        parent->MoveChild(surfaceNode_, zOrder);
    }
}

void SurfaceControl::SetBuffer(sptr<SurfaceBuffer> buffer, UniqueFd fenceFd, const BufferReleaseCallback& callback)
{
    CHECK_NULL_POINTER(surfaceNode_);
    CHECK_NULL_POINTER(buffer);
    RS_TRACE_NAME_FMT("SurfaceControl:%s, node:[name: %s, id: %" PRIu64 "], bufferSeqNum=%u",
        __func__, surfaceNode_->GetName().c_str(), surfaceNode_->GetId(), buffer->GetSeqNum());
}

void SurfaceControl::SetSrcRect(float x, float y, float w, float h)
{
    CHECK_NULL_POINTER(surfaceNode_);
    RS_TRACE_NAME_FMT("SurfaceControl:%s, node:[name: %s, id: %" PRIu64 "], SrcRect[x:%.2f, y:%.2f, w:%.2f, h:%.2f]",
        __func__, surfaceNode_->GetName().c_str(), surfaceNode_->GetId(), x, y, w, h);
}

void SurfaceControl::SetDisplayRect(float x, float y, float w, float h)
{
    CHECK_NULL_POINTER(surfaceNode_);
    RS_TRACE_NAME_FMT("SurfaceControl:%s, node:[name: %s, id: %" PRIu64 "], "
        "DisplayRect[x:%.2f, y:%.2f, w:%.2f, h:%.2f]", __func__, surfaceNode_->GetName().c_str(),
        surfaceNode_->GetId(), x, y, w, h);
    surfaceNode_->SetBounds(x, y, w, h);
}

void SurfaceControl::SetBounds(float x, float y, float w, float h)
{
    CHECK_NULL_POINTER(surfaceNode_);
    if (surfaceNode_->GetName().find("delegate_container") == std::string::npos) {
        RS_TRACE_NAME_FMT("disable SetBounds for delegate_container, nodeId=%llu, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]",
            surfaceNode_->GetId(), x, y, w, h);
        return;
    }
    const float epsilon = 1e-9;
    if (w <= epsilon || h <= epsilon) {
        WVLOG_E("Invalid bounds, x: %.2f, y: %.2f, w: %.2f, h: %.2f", x, y, w, h);
        return;
    }
    surfaceNode_->SetBounds(x, y, w, h);
}

void SurfaceControl::SetFrameGravity(int32_t gravity)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetFrameGravity(static_cast<OHOS::Rosen::Gravity>(gravity));
}

void SurfaceControl::SetPivot(float x, float y)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetPivot(x, y);
}

void SurfaceControl::SetBufferTransform(GraphicTransformType transform)
{
    CHECK_NULL_POINTER(surfaceNode_);
}

void SurfaceControl::SetTranslate(float translateX, float translateY, float translateZ)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetTranslate(translateX, translateY, translateZ);
}

void SurfaceControl::SetDamageRegion(std::vector<Rect> rects)
{
    CHECK_NULL_POINTER(surfaceNode_);
}

void SurfaceControl::SetBufferAlpha(float alpha)
{
    CHECK_NULL_POINTER(surfaceNode_);
    const float epsilon = 1e-9;
    if (alpha < -epsilon) {
        WVLOG_E("Invalid alpha: %.2f", alpha);
        return;
    }
    surfaceNode_->SetAlpha(alpha);
}

void SurfaceControl::SetForegroundColor(float red, float green, float blue, float alpha)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetForegroundColor(ToARGB(red, green, blue, alpha));
}

void SurfaceControl::SetBackgroundColor(float red, float green, float blue, float alpha)
{
    CHECK_NULL_POINTER(surfaceNode_);
    RS_TRACE_NAME_FMT("SurfaceControl::SetBackgroundColor: [r:%f, g:%f, b:%f, a:%f], nodeId=%llu",
        red, green, blue, alpha, surfaceNode_->GetId());
    surfaceNode_->SetBackgroundColor(ToARGB(red, green, blue, alpha));
}

void SurfaceControl::SetBorderWidth(float left, float top, float right, float bottom)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetBorderWidth(left, top, right, bottom);
}

void SurfaceControl::SetBorderColor(float red, float green, float blue, float alpha)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetBorderColor(ToARGB(red, green, blue, alpha));
}

void SurfaceControl::SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetBorderStyle(left, top, right, bottom);
}

void SurfaceControl::SetName(std::string name)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetNodeName(name);
}

void SurfaceControl::SetHardwareEnableHint(bool enable)
{
    CHECK_NULL_POINTER(surfaceNode_);
    surfaceNode_->SetHardwareEnableHint(enable);
}

bool SurfaceControl::IsRootSurface() const
{
    return parentNode_ != nullptr;
}

void SurfaceControl::ClearBufferQueueCache(bool cleanAll)
{
    CHECK_NULL_POINTER(surfaceNode_);
    WVLOG_I("SurfaceControl::ClearBufferQueueCache: %{public}d", cleanAll);
}

RSSurfaceNodeReleaseBufferWoker::RSSurfaceNodeReleaseBufferWoker()
{
    RS_TRACE_NAME("webview RSSurfaceNodeReleaseBufferWoker::RSSurfaceNodeReleaseBufferWoker");
    RegisterReleaseBufferListener();
}

RSSurfaceNodeReleaseBufferWoker::~RSSurfaceNodeReleaseBufferWoker()
{
    RS_TRACE_NAME("webview RSSurfaceNodeReleaseBufferWoker::~RSSurfaceNodeReleaseBufferWoker");
    UnRegisterReleaseBufferListener();
}

RSSurfaceNodeReleaseBufferWoker& RSSurfaceNodeReleaseBufferWoker::GetInstance()
{
    static RSSurfaceNodeReleaseBufferWoker instance;
    return instance;
}

void RSSurfaceNodeReleaseBufferWoker::RegisterNode(std::shared_ptr<Rosen::RSSurfaceNode> nodePtr)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (!nodePtr) {
        return;
    }
    NodeId id = nodePtr->GetId();
    auto itr = surfaceNodeMap_.find(id);
    if (itr != surfaceNodeMap_.end()) {
        return;
    }
    Rosen::RSSurfaceNode::WeakPtr ptr(nodePtr);
    surfaceNodeMap_.emplace(id, ptr);
    RS_TRACE_NAME_FMT("webview add surfaceNodeMap_ size=%u, id=%llu", surfaceNodeMap_.size(), id);
    return;
}

void RSSurfaceNodeReleaseBufferWoker::UnRegisterNode(std::shared_ptr<Rosen::RSSurfaceNode> nodePtr)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (!nodePtr) {
        return;
    }
    NodeId id = nodePtr->GetId();
    auto itr = surfaceNodeMap_.find(id);
    bool findit = false;
    if (itr != surfaceNodeMap_.end()) {
        findit = true;
        surfaceNodeMap_.erase(itr);
    }
    RS_TRACE_NAME_FMT("webview UnregisterNode surfaceNodeMap_ size=%u, findit=%d, nodeId=%llu",
        surfaceNodeMap_.size(), findit, id);
}

std::shared_ptr<Rosen::RSSurfaceNode> RSSurfaceNodeReleaseBufferWoker::LocalGetNodeById(NodeId id)
{
    std::lock_guard<std::mutex> lock(mutex);
    RS_TRACE_NAME_FMT("webview find surfaceNodeMap_ size=%u", surfaceNodeMap_.size());
    auto itr = surfaceNodeMap_.find(id);
    if (itr != surfaceNodeMap_.end()) {
        return itr->second.lock();
    }
    return nullptr;
}

void RSSurfaceNodeReleaseBufferWoker::RegisterReleaseBufferListener()
{
}

void RSSurfaceNodeReleaseBufferWoker::UnRegisterReleaseBufferListener()
{
}
} // namespace NWeb
} // namespace OHOS