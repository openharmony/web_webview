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

#include "ohos_nweb/bridge/ark_web_nweb_impl.h"

#include "ohos_nweb/bridge/ark_web_accessibility_event_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_accessibility_node_info_impl.h"
#include "ohos_nweb/bridge/ark_web_array_buffer_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_pdfconfig_args_wrapper.h"
#include "ohos_nweb/bridge/ark_web_bool_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_cache_options_wrapper.h"
#include "ohos_nweb/bridge/ark_web_core_struct_utils.h"
#include "ohos_nweb/bridge/ark_web_create_native_media_player_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_download_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_drag_data_impl.h"
#include "ohos_nweb/bridge/ark_web_drag_event_wrapper.h"
#include "ohos_nweb/bridge/ark_web_find_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_handler_wrapper.h"
#include "ohos_nweb/bridge/ark_web_history_list_impl.h"
#include "ohos_nweb/bridge/ark_web_hit_test_result_impl.h"
#include "ohos_nweb/bridge/ark_web_js_proxy_method_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_result_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_keyboard_event_wrapper.h"
#include "ohos_nweb/bridge/ark_web_message_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_mouse_event_wrapper.h"
#include "ohos_nweb/bridge/ark_web_preference_impl.h"
#include "ohos_nweb/bridge/ark_web_release_surface_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_rom_value_wrapper.h"
#include "ohos_nweb/bridge/ark_web_screen_lock_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_snapshot_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_spanstring_convert_html_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_string_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_stylus_touch_point_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_system_configuration_wrapper.h"
#include "ohos_nweb/bridge/ark_web_print_document_adapter_adapter_impl.h"
#include "ohos_nweb/ctocpp/ark_web_js_proxy_callback_vector_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_rom_value_vector_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_stylus_touch_point_info_vector_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_touch_point_info_vector_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_value_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebBlurReason = OHOS::NWeb::BlurReason;
using ArkWebFocusReason = OHOS::NWeb::FocusReason;
using ArkWebImageColorType = OHOS::NWeb::ImageColorType;
using ArkWebImageAlphaType = OHOS::NWeb::ImageAlphaType;
using ArkWebNestedScrollMode = OHOS::NWeb::NestedScrollMode;
using ArkPixelUnit = OHOS::NWeb::PixelUnit;

ArkWebNWebImpl::ArkWebNWebImpl(std::shared_ptr<OHOS::NWeb::NWeb> nweb_nweb) : nweb_nweb_(nweb_nweb) {}

void ArkWebNWebImpl::Resize(uint32_t width, uint32_t height, bool is_keyboard)
{
    nweb_nweb_->Resize(width, height, is_keyboard);
}

void ArkWebNWebImpl::OnPause()
{
    nweb_nweb_->OnPause();
}

void ArkWebNWebImpl::OnContinue()
{
    nweb_nweb_->OnContinue();
}

void ArkWebNWebImpl::OnDestroy()
{
    nweb_nweb_->OnDestroy();
}

void ArkWebNWebImpl::OnFocus(const int32_t& focus_reason)
{
    nweb_nweb_->OnFocus(static_cast<ArkWebFocusReason>(focus_reason));
}

void ArkWebNWebImpl::OnBlur(const int32_t& blur_reason)
{
    nweb_nweb_->OnBlur(static_cast<ArkWebBlurReason>(blur_reason));
}

void ArkWebNWebImpl::OnTouchPress(int32_t id, double x, double y, bool from_overlay)
{
    nweb_nweb_->OnTouchPress(id, x, y, from_overlay);
}

void ArkWebNWebImpl::OnTouchRelease(int32_t id, double x, double y, bool from_overlay)
{
    nweb_nweb_->OnTouchRelease(id, x, y, from_overlay);
}

void ArkWebNWebImpl::OnTouchMove(int32_t id, double x, double y, bool from_overlay)
{
    nweb_nweb_->OnTouchMove(id, x, y, from_overlay);
}

void ArkWebNWebImpl::OnTouchMove(const ArkWebTouchPointInfoVector& touch_point_infos, bool from_overlay)
{
    nweb_nweb_->OnTouchMove(ArkWebTouchPointInfoVectorStructToClass(touch_point_infos), from_overlay);
}

void ArkWebNWebImpl::OnTouchCancel()
{
    nweb_nweb_->OnTouchCancel();
}

void ArkWebNWebImpl::OnNavigateBack()
{
    nweb_nweb_->OnNavigateBack();
}

bool ArkWebNWebImpl::SendKeyEvent(int32_t key_code, int32_t key_action)
{
    return nweb_nweb_->SendKeyEvent(key_code, key_action);
}

void ArkWebNWebImpl::SendMouseWheelEvent(double x, double y, double delta_x, double delta_y)
{
    nweb_nweb_->SendMouseWheelEvent(x, y, delta_x, delta_y);
}

void ArkWebNWebImpl::SendMouseEvent(int x, int y, int button, int action, int count)
{
    nweb_nweb_->SendMouseEvent(x, y, button, action, count);
}

int ArkWebNWebImpl::Load(const ArkWebString& url)
{
    return nweb_nweb_->Load(ArkWebStringStructToClass(url));
}

bool ArkWebNWebImpl::IsNavigatebackwardAllowed()
{
    return nweb_nweb_->IsNavigatebackwardAllowed();
}

bool ArkWebNWebImpl::IsNavigateForwardAllowed()
{
    return nweb_nweb_->IsNavigateForwardAllowed();
}

bool ArkWebNWebImpl::CanNavigateBackOrForward(int num_steps)
{
    return nweb_nweb_->CanNavigateBackOrForward(num_steps);
}

void ArkWebNWebImpl::NavigateBack()
{
    nweb_nweb_->NavigateBack();
}

void ArkWebNWebImpl::NavigateForward()
{
    nweb_nweb_->NavigateForward();
}

void ArkWebNWebImpl::NavigateBackOrForward(int step)
{
    nweb_nweb_->NavigateBackOrForward(step);
}

void ArkWebNWebImpl::DeleteNavigateHistory()
{
    nweb_nweb_->DeleteNavigateHistory();
}

void ArkWebNWebImpl::Reload()
{
    nweb_nweb_->Reload();
}

int ArkWebNWebImpl::Zoom(float zoom_factor)
{
    return nweb_nweb_->Zoom(zoom_factor);
}

int ArkWebNWebImpl::ZoomIn()
{
    return nweb_nweb_->ZoomIn();
}

int ArkWebNWebImpl::ZoomOut()
{
    return nweb_nweb_->ZoomOut();
}

void ArkWebNWebImpl::Stop()
{
    nweb_nweb_->Stop();
}

void ArkWebNWebImpl::ExecuteJavaScript(const ArkWebString& code)
{
    nweb_nweb_->ExecuteJavaScript(ArkWebStringStructToClass(code));
}

void ArkWebNWebImpl::ExecuteJavaScript(
    const ArkWebString& code, ArkWebRefPtr<ArkWebMessageValueCallback> callback, bool extention)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->ExecuteJavaScript(ArkWebStringStructToClass(code), nullptr, extention);
        return;
    }

    nweb_nweb_->ExecuteJavaScript(
        ArkWebStringStructToClass(code), std::make_shared<ArkWebMessageValueCallbackWrapper>(callback), extention);
}

ArkWebRefPtr<ArkWebPreference> ArkWebNWebImpl::GetPreference()
{
    std::shared_ptr<OHOS::NWeb::NWebPreference> nweb_preference = nweb_nweb_->GetPreference();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_preference)) {
        return nullptr;
    }

    return new ArkWebPreferenceImpl(nweb_preference);
}

unsigned int ArkWebNWebImpl::GetWebId()
{
    return nweb_nweb_->GetWebId();
}

ArkWebRefPtr<ArkWebHitTestResult> ArkWebNWebImpl::GetHitTestResult()
{
    std::shared_ptr<OHOS::NWeb::HitTestResult> nweb_hit_test_result = nweb_nweb_->GetHitTestResult();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_hit_test_result)) {
        return nullptr;
    }

    return new ArkWebHitTestResultImpl(nweb_hit_test_result);
}

void ArkWebNWebImpl::PutBackgroundColor(int color)
{
    nweb_nweb_->PutBackgroundColor(color);
}

void ArkWebNWebImpl::InitialScale(float scale)
{
    nweb_nweb_->InitialScale(scale);
}

void ArkWebNWebImpl::PutDownloadCallback(ArkWebRefPtr<ArkWebDownloadCallback> download_listener)
{
    if (CHECK_REF_PTR_IS_NULL(download_listener)) {
        nweb_nweb_->PutDownloadCallback(nullptr);
        return;
    }

    nweb_nweb_->PutDownloadCallback(std::make_shared<ArkWebDownloadCallbackWrapper>(download_listener));
}

void ArkWebNWebImpl::PutAccessibilityEventCallback(
    ArkWebRefPtr<ArkWebAccessibilityEventCallback> accessibility_event_listener)
{
    if (CHECK_REF_PTR_IS_NULL(accessibility_event_listener)) {
        nweb_nweb_->PutAccessibilityEventCallback(nullptr);
        return;
    }

    nweb_nweb_->PutAccessibilityEventCallback(
        std::make_shared<ArkWebAccessibilityEventCallbackWrapper>(accessibility_event_listener));
}

void ArkWebNWebImpl::PutAccessibilityIdGenerator(AccessibilityIdGenerateFunc accessibility_id_generator)
{
    nweb_nweb_->PutAccessibilityIdGenerator(accessibility_id_generator);
}

void ArkWebNWebImpl::SetNWebHandler(ArkWebRefPtr<ArkWebHandler> handler)
{
    if (CHECK_REF_PTR_IS_NULL(handler)) {
        nweb_nweb_->SetNWebHandler(nullptr);
        return;
    }

    nweb_nweb_->SetNWebHandler(std::make_shared<ArkWebHandlerWrapper>(handler));
}

ArkWebString ArkWebNWebImpl::Title()
{
    return ArkWebStringClassToStruct(nweb_nweb_->Title());
}

int ArkWebNWebImpl::PageLoadProgress()
{
    return nweb_nweb_->PageLoadProgress();
}

int ArkWebNWebImpl::ContentHeight()
{
    return nweb_nweb_->ContentHeight();
}

float ArkWebNWebImpl::Scale()
{
    return nweb_nweb_->Scale();
}

int ArkWebNWebImpl::Load(const ArkWebString& url, const ArkWebStringMap& additional_http_headers)
{
    return nweb_nweb_->Load(ArkWebStringStructToClass(url), ArkWebStringMapStructToClass(additional_http_headers));
}

int ArkWebNWebImpl::LoadWithDataAndBaseUrl(const ArkWebString& base_url, const ArkWebString& data,
    const ArkWebString& mime_type, const ArkWebString& encoding, const ArkWebString& history_url)
{
    return nweb_nweb_->LoadWithDataAndBaseUrl(ArkWebStringStructToClass(base_url), ArkWebStringStructToClass(data),
        ArkWebStringStructToClass(mime_type), ArkWebStringStructToClass(encoding),
        ArkWebStringStructToClass(history_url));
}

int ArkWebNWebImpl::LoadWithData(const ArkWebString& data, const ArkWebString& mime_type, const ArkWebString& encoding)
{
    return nweb_nweb_->LoadWithData(
        ArkWebStringStructToClass(data), ArkWebStringStructToClass(mime_type), ArkWebStringStructToClass(encoding));
}

void ArkWebNWebImpl::RegisterArkJSfunction(
    const ArkWebString& object_name, const ArkWebStringVector& method_list, const int32_t object_id)
{
    nweb_nweb_->RegisterArkJSfunction(
        ArkWebStringStructToClass(object_name), ArkWebStringVectorStructToClass(method_list), object_id);
}

void ArkWebNWebImpl::UnregisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list)
{
    nweb_nweb_->UnregisterArkJSfunction(
        ArkWebStringStructToClass(object_name), ArkWebStringVectorStructToClass(method_list));
}

void ArkWebNWebImpl::SetNWebJavaScriptResultCallBack(ArkWebRefPtr<ArkWebJsResultCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->SetNWebJavaScriptResultCallBack(nullptr);
        return;
    }

    nweb_nweb_->SetNWebJavaScriptResultCallBack(std::make_shared<ArkWebJsResultCallbackWrapper>(callback));
}

void ArkWebNWebImpl::PutFindCallback(ArkWebRefPtr<ArkWebFindCallback> find_listener)
{
    if (CHECK_REF_PTR_IS_NULL(find_listener)) {
        nweb_nweb_->PutFindCallback(nullptr);
        return;
    }

    nweb_nweb_->PutFindCallback(std::make_shared<ArkWebFindCallbackWrapper>(find_listener));
}

void ArkWebNWebImpl::FindAllAsync(const ArkWebString& search_str)
{
    nweb_nweb_->FindAllAsync(ArkWebStringStructToClass(search_str));
}

void ArkWebNWebImpl::ClearMatches()
{
    nweb_nweb_->ClearMatches();
}

void ArkWebNWebImpl::FindNext(const bool forward)
{
    nweb_nweb_->FindNext(forward);
}

void ArkWebNWebImpl::StoreWebArchive(
    const ArkWebString& base_name, bool auto_name, ArkWebRefPtr<ArkWebStringValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->StoreWebArchive(ArkWebStringStructToClass(base_name), auto_name, nullptr);
        return;
    }

    nweb_nweb_->StoreWebArchive(
        ArkWebStringStructToClass(base_name), auto_name, std::make_shared<ArkWebStringValueCallbackWrapper>(callback));
}

ArkWebStringVector ArkWebNWebImpl::CreateWebMessagePorts()
{
    return ArkWebStringVectorClassToStruct(nweb_nweb_->CreateWebMessagePorts());
}

void ArkWebNWebImpl::PostWebMessage(
    const ArkWebString& message, const ArkWebStringVector& ports, const ArkWebString& target_uri)
{
    nweb_nweb_->PostWebMessage(ArkWebStringStructToClass(message), ArkWebStringVectorStructToClass(ports),
        ArkWebStringStructToClass(target_uri));
}

void ArkWebNWebImpl::ClosePort(const ArkWebString& port_handle)
{
    nweb_nweb_->ClosePort(ArkWebStringStructToClass(port_handle));
}

void ArkWebNWebImpl::PostPortMessage(const ArkWebString& port_handle, const ArkWebMessage& data)
{
    nweb_nweb_->PostPortMessage(ArkWebStringStructToClass(port_handle), data.nweb_message);
}

void ArkWebNWebImpl::SetPortMessageCallback(
    const ArkWebString& port_handle, ArkWebRefPtr<ArkWebMessageValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->SetPortMessageCallback(ArkWebStringStructToClass(port_handle), nullptr);
        return;
    }

    nweb_nweb_->SetPortMessageCallback(
        ArkWebStringStructToClass(port_handle), std::make_shared<ArkWebMessageValueCallbackWrapper>(callback));
}

void ArkWebNWebImpl::SendDragEvent(ArkWebRefPtr<ArkWebDragEvent> drag_event)
{
    if (CHECK_REF_PTR_IS_NULL(drag_event)) {
        nweb_nweb_->SendDragEvent(nullptr);
        return;
    }

    nweb_nweb_->SendDragEvent(std::make_shared<ArkWebDragEventWrapper>(drag_event));
}

void ArkWebNWebImpl::ClearSslCache()
{
    nweb_nweb_->ClearSslCache();
}

ArkWebString ArkWebNWebImpl::GetUrl()
{
    return ArkWebStringClassToStruct(nweb_nweb_->GetUrl());
}

void ArkWebNWebImpl::ClearClientAuthenticationCache()
{
    nweb_nweb_->ClearClientAuthenticationCache();
}

void ArkWebNWebImpl::UpdateLocale(const ArkWebString& language, const ArkWebString& region)
{
    nweb_nweb_->UpdateLocale(ArkWebStringStructToClass(language), ArkWebStringStructToClass(region));
}

const ArkWebString ArkWebNWebImpl::GetOriginalUrl()
{
    return ArkWebStringClassToStruct(nweb_nweb_->GetOriginalUrl());
}

bool ArkWebNWebImpl::GetFavicon(const void** data, size_t& width, size_t& height, int& color_type, int& alpha_type)
{
    ArkWebImageColorType enum_color_type = ArkWebImageColorType::COLOR_TYPE_UNKNOWN;
    ArkWebImageAlphaType enum_alpha_type = ArkWebImageAlphaType::ALPHA_TYPE_UNKNOWN;
    bool result = nweb_nweb_->GetFavicon(data, width, height, enum_color_type, enum_alpha_type);
    color_type = static_cast<int>(enum_color_type);
    alpha_type = static_cast<int>(enum_alpha_type);
    return result;
}

void ArkWebNWebImpl::PutNetworkAvailable(bool available)
{
    nweb_nweb_->PutNetworkAvailable(available);
}

void ArkWebNWebImpl::HasImages(ArkWebRefPtr<ArkWebBoolValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->HasImages(nullptr);
        return;
    }

    nweb_nweb_->HasImages(std::make_shared<ArkWebBoolValueCallbackWrapper>(callback));
}

void ArkWebNWebImpl::RemoveCache(bool include_disk_files)
{
    nweb_nweb_->RemoveCache(include_disk_files);
}

ArkWebRefPtr<ArkWebHistoryList> ArkWebNWebImpl::GetHistoryList()
{
    std::shared_ptr<OHOS::NWeb::NWebHistoryList> nweb_history_list = nweb_nweb_->GetHistoryList();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_history_list)) {
        return nullptr;
    }

    return new ArkWebHistoryListImpl(nweb_history_list);
}

void ArkWebNWebImpl::PutReleaseSurfaceCallback(ArkWebRefPtr<ArkWebReleaseSurfaceCallback> release_surface_listener)
{
    if (CHECK_REF_PTR_IS_NULL(release_surface_listener)) {
        nweb_nweb_->PutReleaseSurfaceCallback(nullptr);
        return;
    }

    nweb_nweb_->PutReleaseSurfaceCallback(
        std::make_shared<ArkWebReleaseSurfaceCallbackWrapper>(release_surface_listener));
}

ArkWebUint8Vector ArkWebNWebImpl::SerializeWebState()
{
    return ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(nweb_nweb_->SerializeWebState());
}

bool ArkWebNWebImpl::RestoreWebState(const ArkWebUint8Vector& state)
{
    return nweb_nweb_->RestoreWebState(ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(state));
}

void ArkWebNWebImpl::PageUp(bool top)
{
    nweb_nweb_->PageUp(top);
}

void ArkWebNWebImpl::PageDown(bool bottom)
{
    nweb_nweb_->PageDown(bottom);
}

void ArkWebNWebImpl::ScrollTo(float x, float y)
{
    nweb_nweb_->ScrollTo(x, y);
}

void ArkWebNWebImpl::ScrollBy(float delta_x, float delta_y)
{
    nweb_nweb_->ScrollBy(delta_x, delta_y);
}

void ArkWebNWebImpl::SlideScroll(float vx, float vy)
{
    nweb_nweb_->SlideScroll(vx, vy);
}

bool ArkWebNWebImpl::GetCertChainDerData(ArkWebStringVector& cert_chain_data, bool is_single_cert)
{
    std::vector<std::string> cert_chain_data_vector;
    bool result = nweb_nweb_->GetCertChainDerData(cert_chain_data_vector, is_single_cert);
    cert_chain_data = ArkWebStringVectorClassToStruct(cert_chain_data_vector);
    return result;
}

void ArkWebNWebImpl::SetScreenOffSet(double x, double y)
{
    nweb_nweb_->SetScreenOffSet(x, y);
}

void ArkWebNWebImpl::SetAudioMuted(bool muted)
{
    nweb_nweb_->SetAudioMuted(muted);
}

void ArkWebNWebImpl::SetShouldFrameSubmissionBeforeDraw(bool should)
{
    nweb_nweb_->SetShouldFrameSubmissionBeforeDraw(should);
}

void ArkWebNWebImpl::NotifyPopupWindowResult(bool result)
{
    nweb_nweb_->NotifyPopupWindowResult(result);
}

void ArkWebNWebImpl::SetAudioResumeInterval(int32_t resume_interval)
{
    nweb_nweb_->SetAudioResumeInterval(resume_interval);
}

void ArkWebNWebImpl::SetAudioExclusive(bool audio_exclusive)
{
    nweb_nweb_->SetAudioExclusive(audio_exclusive);
}

void ArkWebNWebImpl::RegisterScreenLockFunction(int32_t window_id, ArkWebRefPtr<ArkWebScreenLockCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->RegisterScreenLockFunction(window_id, nullptr);
        return;
    }

    nweb_nweb_->RegisterScreenLockFunction(window_id, std::make_shared<ArkWebScreenLockCallbackWrapper>(callback));
}

void ArkWebNWebImpl::UnRegisterScreenLockFunction(int32_t window_id)
{
    nweb_nweb_->UnRegisterScreenLockFunction(window_id);
}

void ArkWebNWebImpl::NotifyMemoryLevel(int32_t level)
{
    nweb_nweb_->NotifyMemoryLevel(level);
}

void ArkWebNWebImpl::OnWebviewHide()
{
    nweb_nweb_->OnWebviewHide();
}

void ArkWebNWebImpl::OnWebviewShow()
{
    nweb_nweb_->OnWebviewShow();
}

ArkWebRefPtr<ArkWebDragData> ArkWebNWebImpl::GetOrCreateDragData()
{
    std::shared_ptr<OHOS::NWeb::NWebDragData> nweb_drag_data = nweb_nweb_->GetOrCreateDragData();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_drag_data)) {
        return nullptr;
    }

    return new ArkWebDragDataImpl(nweb_drag_data);
}

void ArkWebNWebImpl::PrefetchPage(const ArkWebString& url, const ArkWebStringMap& additional_http_headers)
{
    nweb_nweb_->PrefetchPage(ArkWebStringStructToClass(url), ArkWebStringMapStructToClass(additional_http_headers));
}

void ArkWebNWebImpl::SetWindowId(uint32_t window_id)
{
    nweb_nweb_->SetWindowId(window_id);
}

void ArkWebNWebImpl::OnOccluded()
{
    nweb_nweb_->OnOccluded();
}

void ArkWebNWebImpl::OnUnoccluded()
{
    nweb_nweb_->OnUnoccluded();
}

void ArkWebNWebImpl::SetToken(void* token)
{
    nweb_nweb_->SetToken(token);
}

void ArkWebNWebImpl::SetNestedScrollMode(const int32_t& nested_scroll_mode)
{
    nweb_nweb_->SetNestedScrollMode(static_cast<ArkWebNestedScrollMode>(nested_scroll_mode));
}

void ArkWebNWebImpl::SetEnableLowerFrameRate(bool enabled)
{
    nweb_nweb_->SetEnableLowerFrameRate(enabled);
}

void ArkWebNWebImpl::SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard)
{
    nweb_nweb_->SetVirtualKeyBoardArg(width, height, keyboard);
}

bool ArkWebNWebImpl::ShouldVirtualKeyboardOverlay()
{
    return nweb_nweb_->ShouldVirtualKeyboardOverlay();
}

void ArkWebNWebImpl::SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
    nweb_nweb_->SetDrawRect(x, y, width, height);
}

void ArkWebNWebImpl::SetDrawMode(int32_t mode)
{
    nweb_nweb_->SetDrawMode(mode);
}

void* ArkWebNWebImpl::CreateWebPrintDocumentAdapter(const ArkWebString& job_name)
{
    return nweb_nweb_->CreateWebPrintDocumentAdapter(ArkWebStringStructToClass(job_name));
}

ArkWebRefPtr<ArkWebPrintDocumentAdapterAdapter> ArkWebNWebImpl::CreateWebPrintDocumentAdapterV2(
    const ArkWebString& job_name)
{
    std::unique_ptr<OHOS::NWeb::NWebPrintDocumentAdapterAdapter> adapter =
        nweb_nweb_->CreateWebPrintDocumentAdapterV2(ArkWebStringStructToClass(job_name));
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return new ArkWebPrintDocumentAdapterAdapterImpl(std::move(adapter));
}

int ArkWebNWebImpl::PostUrl(const ArkWebString& url, const ArkWebCharVector& post_data)
{
    return nweb_nweb_->PostUrl(
        ArkWebStringStructToClass(url), ArkWebBasicVectorStructToClass<char, ArkWebCharVector>(post_data));
}

void ArkWebNWebImpl::JavaScriptOnDocumentStart(const ArkWebStringVectorMap& script_items)
{
    nweb_nweb_->JavaScriptOnDocumentStart(ArkWebStringVectorMapStructToClass(script_items));
}

void ArkWebNWebImpl::JavaScriptOnDocumentStartByOrder(const ArkWebStringVectorMap& script_items,
    const ArkWebStringVector& script_items_by_order)
{
    nweb_nweb_->JavaScriptOnDocumentStartByOrder(ArkWebStringVectorMapStructToClass(script_items),
        ArkWebStringVectorStructToClass(script_items_by_order));
}

void ArkWebNWebImpl::JavaScriptOnDocumentEndByOrder(const ArkWebStringVectorMap& script_items,
    const ArkWebStringVector& script_items_by_order)
{
    nweb_nweb_->JavaScriptOnDocumentEndByOrder(ArkWebStringVectorMapStructToClass(script_items),
        ArkWebStringVectorStructToClass(script_items_by_order));
}

void ArkWebNWebImpl::JavaScriptOnHeadReadyByOrder(const ArkWebStringVectorMap& script_items,
    const ArkWebStringVector& script_items_by_order)
{
    nweb_nweb_->JavaScriptOnHeadReadyByOrder(ArkWebStringVectorMapStructToClass(script_items),
        ArkWebStringVectorStructToClass(script_items_by_order));
}

void ArkWebNWebImpl::ExecuteAction(int64_t accessibility_id, uint32_t action)
{
    nweb_nweb_->ExecuteAction(accessibility_id, action);
}

ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ArkWebNWebImpl::GetFocusedAccessibilityNodeInfo(
    int64_t accessibility_id, bool is_accessibility_focus)
{
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> nweb_accessibility_node_info =
        nweb_nweb_->GetFocusedAccessibilityNodeInfo(accessibility_id, is_accessibility_focus);
    if (CHECK_SHARED_PTR_IS_NULL(nweb_accessibility_node_info)) {
        return nullptr;
    }

    return new ArkWebAccessibilityNodeInfoImpl(nweb_accessibility_node_info);
}

ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ArkWebNWebImpl::GetAccessibilityNodeInfoById(int64_t accessibility_id)
{
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> nweb_accessibility_node_info =
        nweb_nweb_->GetAccessibilityNodeInfoById(accessibility_id);
    if (CHECK_SHARED_PTR_IS_NULL(nweb_accessibility_node_info)) {
        return nullptr;
    }

    return new ArkWebAccessibilityNodeInfoImpl(nweb_accessibility_node_info);
}

ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ArkWebNWebImpl::GetAccessibilityNodeInfoByFocusMove(
    int64_t accessibility_id, int32_t direction)
{
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> nweb_accessibility_node_info =
        nweb_nweb_->GetAccessibilityNodeInfoByFocusMove(accessibility_id, direction);
    if (CHECK_SHARED_PTR_IS_NULL(nweb_accessibility_node_info)) {
        return nullptr;
    }

    return new ArkWebAccessibilityNodeInfoImpl(nweb_accessibility_node_info);
}

void ArkWebNWebImpl::SetAccessibilityState(bool state)
{
    nweb_nweb_->SetAccessibilityState(state);
}

bool ArkWebNWebImpl::NeedSoftKeyboard()
{
    return nweb_nweb_->NeedSoftKeyboard();
}

bool ArkWebNWebImpl::Discard()
{
    return nweb_nweb_->Discard();
}

bool ArkWebNWebImpl::Restore()
{
    return nweb_nweb_->Restore();
}

int ArkWebNWebImpl::GetSecurityLevel()
{
    return nweb_nweb_->GetSecurityLevel();
}

void ArkWebNWebImpl::CallH5Function(
    int32_t routingId, int32_t h5ObjectId, const ArkWebString& h5MethodName, const ArkWebValueVector& args)
{
    nweb_nweb_->CallH5Function(
        routingId, h5ObjectId, ArkWebStringStructToClass(h5MethodName), ArkWebValueVectorStructToClass(args));
}

bool ArkWebNWebImpl::IsIncognitoMode()
{
    return nweb_nweb_->IsIncognitoMode();
}

void ArkWebNWebImpl::RegisterNativeArkJSFunction(const char* objName, const ArkWebJsProxyCallbackVector& callbacks)
{
    nweb_nweb_->RegisterNativeArkJSFunction(objName, ArkWebJsProxyCallbackVectorStructToClass(callbacks));
}

void ArkWebNWebImpl::UnRegisterNativeArkJSFunction(const char* objName)
{
    nweb_nweb_->UnRegisterNativeArkJSFunction(objName);
}

void ArkWebNWebImpl::RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback)
{
    nweb_nweb_->RegisterNativeValideCallback(webName, callback);
}

void ArkWebNWebImpl::RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnValidCallback callback)
{
    nweb_nweb_->RegisterNativeDestroyCallback(webName, callback);
}

void ArkWebNWebImpl::JavaScriptOnDocumentEnd(const ArkWebStringVectorMap& script_items)
{
    nweb_nweb_->JavaScriptOnDocumentEnd(ArkWebStringVectorMapStructToClass(script_items));
}

void ArkWebNWebImpl::EnableSafeBrowsing(bool enable)
{
    nweb_nweb_->EnableSafeBrowsing(enable);
}

bool ArkWebNWebImpl::IsSafeBrowsingEnabled()
{
    return nweb_nweb_->IsSafeBrowsingEnabled();
}

void ArkWebNWebImpl::SetPrintBackground(bool enable)
{
    nweb_nweb_->SetPrintBackground(enable);
}

bool ArkWebNWebImpl::GetPrintBackground()
{
    return nweb_nweb_->GetPrintBackground();
}

void ArkWebNWebImpl::CloseAllMediaPresentations()
{
    nweb_nweb_->CloseAllMediaPresentations();
}

void ArkWebNWebImpl::StopAllMedia()
{
    nweb_nweb_->StopAllMedia();
}

void ArkWebNWebImpl::ResumeAllMedia()
{
    nweb_nweb_->ResumeAllMedia();
}

void ArkWebNWebImpl::PauseAllMedia()
{
    nweb_nweb_->PauseAllMedia();
}

int ArkWebNWebImpl::GetMediaPlaybackState()
{
    return nweb_nweb_->GetMediaPlaybackState();
}

ArkWebString ArkWebNWebImpl::GetLastJavascriptProxyCallingFrameUrl()
{
    return ArkWebStringClassToStruct(nweb_nweb_->GetLastJavascriptProxyCallingFrameUrl());
}

void ArkWebNWebImpl::EnableIntelligentTrackingPrevention(bool enable)
{
    nweb_nweb_->EnableIntelligentTrackingPrevention(enable);
}

bool ArkWebNWebImpl::IsIntelligentTrackingPreventionEnabled()
{
    return nweb_nweb_->IsIntelligentTrackingPreventionEnabled();
}

void ArkWebNWebImpl::StartCamera()
{
    nweb_nweb_->StartCamera();
}

void ArkWebNWebImpl::StopCamera()
{
    nweb_nweb_->StopCamera();
}

void ArkWebNWebImpl::CloseCamera()
{
    nweb_nweb_->CloseCamera();
}

bool ArkWebNWebImpl::GetPendingSizeStatus()
{
    return nweb_nweb_->GetPendingSizeStatus();
}

void ArkWebNWebImpl::ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy)
{
    nweb_nweb_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
}

void ArkWebNWebImpl::ExecuteJavaScriptExt(
    const int fd, const size_t scriptLength, ArkWebRefPtr<ArkWebMessageValueCallback> callback, bool extention)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->ExecuteJavaScriptExt(fd, scriptLength, nullptr, extention);
        return;
    }

    nweb_nweb_->ExecuteJavaScriptExt(
        fd, scriptLength, std::make_shared<ArkWebMessageValueCallbackWrapper>(callback), extention);
}

void ArkWebNWebImpl::OnRenderToBackground()
{
    nweb_nweb_->OnRenderToBackground();
}

void ArkWebNWebImpl::OnRenderToForeground()
{
    nweb_nweb_->OnRenderToForeground();
}

void ArkWebNWebImpl::OnOnlineRenderToForeground()
{
    nweb_nweb_->OnOnlineRenderToForeground();
}

void ArkWebNWebImpl::PrecompileJavaScript(const ArkWebString& url, const ArkWebString& script,
    ArkWebRefPtr<ArkWebCacheOptions>& cacheOptions, ArkWebRefPtr<ArkWebMessageValueCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::CacheOptions> options = std::make_shared<ArkWebCacheOptionsWrapper>(cacheOptions);
    nweb_nweb_->PrecompileJavaScript(ArkWebStringStructToClass(url), ArkWebStringStructToClass(script), options,
        std::make_shared<ArkWebMessageValueCallbackWrapper>(callback));
}

void ArkWebNWebImpl::OnCreateNativeMediaPlayer(ArkWebRefPtr<ArkWebCreateNativeMediaPlayerCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->OnCreateNativeMediaPlayer(nullptr);
        return;
    }

    nweb_nweb_->OnCreateNativeMediaPlayer(std::make_shared<ArkWebCreateNativeMediaPlayerCallbackWrapper>(callback));
}

void ArkWebNWebImpl::DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width)
{
    nweb_nweb_->DragResize(width, height, pre_height, pre_width);
}

void ArkWebNWebImpl::OnTouchCancelById(int32_t id, double x, double y, bool from_overlay)
{
    nweb_nweb_->OnTouchCancelById(id, x, y, from_overlay);
}

int ArkWebNWebImpl::ScaleGestureChange(double scale, double centerX, double centerY)
{
    return nweb_nweb_->ScaleGestureChange(scale, centerX, centerY);
}

void ArkWebNWebImpl::InjectOfflineResource(const ArkWebString& url, const ArkWebString& origin,
    const ArkWebUint8Vector& resource, const ArkWebStringMap& responseHeaders, const int type)
{
    nweb_nweb_->InjectOfflineResource(ArkWebStringStructToClass(url), ArkWebStringStructToClass(origin),
        ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(resource),
        ArkWebStringMapStructToClass(responseHeaders), type);
}

bool ArkWebNWebImpl::TerminateRenderProcess()
{
    return nweb_nweb_->TerminateRenderProcess();
}

void ArkWebNWebImpl::SuggestionSelected(int32_t index)
{
    nweb_nweb_->SuggestionSelected(index);
}

void ArkWebNWebImpl::SendTouchpadFlingEvent(double x, double y, double vx, double vy)
{
    nweb_nweb_->SendTouchpadFlingEvent(x, y, vx, vy);
}

void ArkWebNWebImpl::RegisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list,
    const ArkWebStringVector& async_method_list, const int32_t object_id)
{
    nweb_nweb_->RegisterArkJSfunction(ArkWebStringStructToClass(object_name),
        ArkWebStringVectorStructToClass(method_list), ArkWebStringVectorStructToClass(async_method_list), object_id);
}

void ArkWebNWebImpl::SetFitContentMode(int32_t mode)
{
    nweb_nweb_->SetFitContentMode(mode);
}

ArkWebString ArkWebNWebImpl::GetSelectInfo()
{
    return ArkWebStringClassToStruct(nweb_nweb_->GetSelectInfo());
}

void ArkWebNWebImpl::OnSafeInsetsChange(int left, int top, int right, int bottom)
{
    nweb_nweb_->OnSafeInsetsChange(left, top, right, bottom);
}

void ArkWebNWebImpl::OnTextSelected()
{
    nweb_nweb_->OnTextSelected();
}

bool ArkWebNWebImpl::WebSendKeyEvent(int32_t key_code, int32_t key_action,
                                     const ArkWebInt32Vector& pressedCodes)
{
    return nweb_nweb_->WebSendKeyEvent(key_code, key_action,
        ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(pressedCodes));
}

void ArkWebNWebImpl::OnConfigurationUpdated(
    ArkWebRefPtr<ArkWebSystemConfiguration> configuration)
{
    if (CHECK_REF_PTR_IS_NULL(configuration)) {
        nweb_nweb_->OnConfigurationUpdated(nullptr);
        return;
    }
    nweb_nweb_->OnConfigurationUpdated(
        std::make_shared<ArkWebSystemConfigurationWrapper>(configuration));
}
void ArkWebNWebImpl::EnableAdsBlock(bool enable) {
    nweb_nweb_->EnableAdsBlock(enable);
}

bool ArkWebNWebImpl::IsAdsBlockEnabled() {
    return nweb_nweb_->IsAdsBlockEnabled();
}

bool ArkWebNWebImpl::IsAdsBlockEnabledForCurPage() {
    return nweb_nweb_->IsAdsBlockEnabledForCurPage();
}

void ArkWebNWebImpl::NotifyForNextTouchEvent()
{
    nweb_nweb_->NotifyForNextTouchEvent();
}

int ArkWebNWebImpl::SetUrlTrustList(const ArkWebString& urlTrustList)
{
    return nweb_nweb_->SetUrlTrustList(ArkWebStringStructToClass(urlTrustList));
}

int ArkWebNWebImpl::SetUrlTrustListWithErrMsg(const ArkWebString& urlTrustList, ArkWebString& detailErrMsg)
{
    std::string detailMsg;
    int res = nweb_nweb_->SetUrlTrustListWithErrMsg(ArkWebStringStructToClass(urlTrustList),
        detailMsg);
    detailErrMsg = ArkWebStringClassToStruct(detailMsg);
    return res;
}

void ArkWebNWebImpl::PutSpanstringConvertHtmlCallback(
    ArkWebRefPtr<ArkWebSpanstringConvertHtmlCallback> callback) {
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->PutSpanstringConvertHtmlCallback(nullptr);
        return;
    }
    nweb_nweb_->PutSpanstringConvertHtmlCallback(
        std::make_shared<ArkWebSpanstringConvertHtmlCallbackWrapper>(callback));
}

bool ArkWebNWebImpl::WebPageSnapshot(const char* id,
                                     int type,
                                     int width,
                                     int height,
                                     const WebSnapshotCallback callback)
{
    return nweb_nweb_->WebPageSnapshot(id, static_cast<ArkPixelUnit>(type), width,
                                       height, callback);
}

void ArkWebNWebImpl::SetPathAllowingUniversalAccess(const ArkWebStringVector& dirs,
                                                    const ArkWebStringVector& moduleName,
                                                    ArkWebString& errorPath)
{
    std::string errPath;
    nweb_nweb_->SetPathAllowingUniversalAccess(ArkWebStringVectorStructToClass(dirs),
        ArkWebStringVectorStructToClass(moduleName), errPath);
    errorPath = ArkWebStringClassToStruct(errPath);
}

void ArkWebNWebImpl::PerformAction(int64_t accessibility_id, uint32_t action,
    const ArkWebStringMap& actionArguments)
{
    nweb_nweb_->PerformAction(accessibility_id, action, ArkWebStringMapStructToClass(actionArguments));
}

void ArkWebNWebImpl::ScrollToWithAnime(float x, float y, int32_t duration)
{
    nweb_nweb_->ScrollToWithAnime(x, y, duration);
}

void ArkWebNWebImpl::ScrollByWithAnime(float delta_x, float delta_y, int32_t duration)
{
    nweb_nweb_->ScrollByWithAnime(delta_x, delta_y, duration);
}

void ArkWebNWebImpl::WebSendMouseWheelEvent(double x,
                                            double y,
                                            double delta_x,
                                            double delta_y,
                                            const ArkWebInt32Vector& pressedCodes)
{
    nweb_nweb_->WebSendMouseWheelEvent(x, y, delta_x, delta_y,
        ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(pressedCodes));
}

void ArkWebNWebImpl::WebSendTouchpadFlingEvent(double x,
                                               double y,
                                               double vx,
                                               double vy,
                                               const ArkWebInt32Vector& pressedCodes)
{
    nweb_nweb_->WebSendTouchpadFlingEvent(x, y, vx, vy,
        ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(pressedCodes));
}

void ArkWebNWebImpl::SendAccessibilityHoverEvent(int32_t x, int32_t y)
{
    nweb_nweb_->SendAccessibilityHoverEvent(x, y);
}

void ArkWebNWebImpl::RegisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list,
    const ArkWebStringVector& async_method_list, const int32_t object_id, const ArkWebString& permission)
{
    nweb_nweb_->RegisterArkJSfunction(ArkWebStringStructToClass(object_name),
        ArkWebStringVectorStructToClass(method_list), ArkWebStringVectorStructToClass(async_method_list), object_id,
        ArkWebStringStructToClass(permission));
}

void ArkWebNWebImpl::ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard)
{
    nweb_nweb_->ResizeVisibleViewport(width, height, isKeyboard);
}

void ArkWebNWebImpl::SetBackForwardCacheOptions(int32_t size, int32_t timeToLive)
{
    nweb_nweb_->SetBackForwardCacheOptions(size, timeToLive);
}

void ArkWebNWebImpl::SetAutofillCallback(ArkWebRefPtr<ArkWebMessageValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->SetAutofillCallback(nullptr);
        return;
    }

    nweb_nweb_->SetAutofillCallback(std::make_shared<ArkWebMessageValueCallbackWrapper>(callback));
}

void ArkWebNWebImpl::FillAutofillData(const ArkWebMessage& data)
{
    nweb_nweb_->FillAutofillData(data.nweb_message);
}

void ArkWebNWebImpl::OnAutofillCancel(const ArkWebString& fillContent)
{
    nweb_nweb_->OnAutofillCancel(ArkWebStringStructToClass(fillContent));
}

void ArkWebNWebImpl::GetScrollOffset(float* offset_x, float* offset_y)
{
    nweb_nweb_->GetScrollOffset(offset_x, offset_y);
}

void ArkWebNWebImpl::ExecuteCreatePDFExt(
    ArkWebRefPtr<ArkWebPDFConfigArgs> pdfConfig, ArkWebRefPtr<ArkWebArrayBufferValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_nweb_->ExecuteCreatePDFExt(
            std::make_shared<ArkWebPDFConfigArgsWrapper>(CHECK_REF_PTR_IS_NULL(pdfConfig) ? nullptr : pdfConfig),
            nullptr);
        return;
    }

    nweb_nweb_->ExecuteCreatePDFExt(
        std::make_shared<ArkWebPDFConfigArgsWrapper>(CHECK_REF_PTR_IS_NULL(pdfConfig) ? nullptr : pdfConfig),
        std::make_shared<ArkWebArrayBufferValueCallbackWrapper>(callback));
}

bool ArkWebNWebImpl::ScrollByWithResult(float delta_x, float delta_y)
{
    return nweb_nweb_->ScrollByWithResult(delta_x, delta_y);
}

void ArkWebNWebImpl::SetPopupSurface(void* popupSurface)
{
    nweb_nweb_->SetPopupSurface(popupSurface);
}

void ArkWebNWebImpl::OnDestroyImageAnalyzerOverlay()
{
    nweb_nweb_->OnDestroyImageAnalyzerOverlay();
}

void ArkWebNWebImpl::WebSendMouseEvent(ArkWebRefPtr<ArkWebMouseEvent> mouseEvent)
{
    if (CHECK_REF_PTR_IS_NULL(mouseEvent)) {
        nweb_nweb_->WebSendMouseEvent(nullptr);
        return;
    }

    nweb_nweb_->WebSendMouseEvent(std::make_shared<ArkWebMouseEventWrapper>(mouseEvent));
}

bool ArkWebNWebImpl::GetAccessibilityVisible(int64_t accessibility_id)
{
    return nweb_nweb_->GetAccessibilityVisible(accessibility_id);
}

void ArkWebNWebImpl::SetTransformHint(uint32_t rotation)
{
    nweb_nweb_->SetTransformHint(rotation);
}

void ArkWebNWebImpl::WebComponentsBlur()
{
    nweb_nweb_->WebComponentsBlur();
}

float ArkWebNWebImpl::DumpGpuInfo()
{
    return nweb_nweb_->DumpGpuInfo();
}

int ArkWebNWebImpl::ScaleGestureChangeV2(int type, double scale, double originScale, double centerX, double centerY)
{
    return nweb_nweb_->ScaleGestureChangeV2(type, scale, originScale, centerX, centerY);
}

bool ArkWebNWebImpl::SendKeyboardEvent(ArkWebRefPtr<ArkWebKeyboardEvent> keyboardEvent)
{
    if (CHECK_REF_PTR_IS_NULL(keyboardEvent)) {
        nweb_nweb_->SendKeyboardEvent(nullptr);
        return false;
    }

    return nweb_nweb_->SendKeyboardEvent(std::make_shared<ArkWebKeyboardEventWrapper>(keyboardEvent));
}

bool ArkWebNWebImpl::PerformActionV2(int64_t accessibility_id, uint32_t action,
    const ArkWebStringMap& actionArguments)
{
    return nweb_nweb_->PerformActionV2(accessibility_id, action, ArkWebStringMapStructToClass(actionArguments));
}

bool ArkWebNWebImpl::IsActivePolicyDisable()
{
    return nweb_nweb_->IsActivePolicyDisable();
}

void ArkWebNWebImpl::PutOptimizeParserBudgetEnabled(bool enable)
{
    nweb_nweb_->PutOptimizeParserBudgetEnabled(enable);
}

bool ArkWebNWebImpl::GetAccessibilityNodeRectById(
    int64_t accessibilityId, int32_t* width, int32_t* height, int32_t* offsetX, int32_t* offsetY)
{
    return nweb_nweb_->GetAccessibilityNodeRectById(accessibilityId, width, height, offsetX, offsetY);
}

ArkWebRefPtr<ArkWebHitTestResult> ArkWebNWebImpl::GetLastHitTestResult()
{
    std::shared_ptr<OHOS::NWeb::HitTestResult> nweb_last_hit_test_result = nweb_nweb_->GetLastHitTestResult();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_last_hit_test_result)) {
        return nullptr;
    }

    return new ArkWebHitTestResultImpl(nweb_last_hit_test_result);
}

ArkWebString ArkWebNWebImpl::GetCurrentLanguage()
{
    return ArkWebStringClassToStruct(nweb_nweb_->GetCurrentLanguage());
}
bool ArkWebNWebImpl::WebSendMouseWheelEventV2(
        double x, double y, double delta_x, double delta_y, const ArkWebInt32Vector &pressedCodes, int32_t source)
{
    return nweb_nweb_->WebSendMouseWheelEventV2(
        x, y, delta_x, delta_y, ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(pressedCodes), source);
}

bool ArkWebNWebImpl::IsNWebEx()
{
    return nweb_nweb_->IsNWebEx();
}

void ArkWebNWebImpl::SetEnableHalfFrameRate(bool enabled)
{
    nweb_nweb_->SetEnableHalfFrameRate(enabled);
}

void ArkWebNWebImpl::MaximizeResize()
{
    nweb_nweb_->MaximizeResize();
}

void ArkWebNWebImpl::OnDragAttach()
{
    nweb_nweb_->OnDragAttach();
}

bool ArkWebNWebImpl::SetFocusByPosition(float x, float y)
{
    return nweb_nweb_->SetFocusByPosition(x, y);
}

void ArkWebNWebImpl::SetSurfaceDensity(const double& density)
{
    nweb_nweb_->SetSurfaceDensity(density);
}

void ArkWebNWebImpl::SetBorderRadiusFromWeb(double borderRadiusTopLeft, double borderRadiusTopRight,
    double borderRadiusBottomLeft, double borderRadiusBottomRight)
{
    nweb_nweb_->SetBorderRadiusFromWeb(
        borderRadiusTopLeft, borderRadiusTopRight, borderRadiusBottomLeft, borderRadiusBottomRight);
}

void ArkWebNWebImpl::SetNativeInnerWeb(bool isInnerWeb)
{
    nweb_nweb_->SetNativeInnerWeb(isInnerWeb);
}

void ArkWebNWebImpl::SendAccessibilityHoverEventV2(int32_t x, int32_t y, bool isHoverEnter)
{
    nweb_nweb_->SendAccessibilityHoverEventV2(x, y, isHoverEnter);
}

void ArkWebNWebImpl::OnBrowserForeground()
{
    nweb_nweb_->OnBrowserForeground();
}

void ArkWebNWebImpl::OnBrowserBackground()
{
    nweb_nweb_->OnBrowserBackground();
}

void ArkWebNWebImpl::RegisterNativeJavaScriptProxy(const ArkWebString& objName,
    const ArkWebStringVector& methodName, ArkWebRefPtr<ArkWebJsProxyMethod> data,
    bool isAsync, const ArkWebString& permission)
{
    if (CHECK_REF_PTR_IS_NULL(data)) {
        nweb_nweb_->RegisterNativeJavaScriptProxy(ArkWebStringStructToClass(objName),
                                                  ArkWebStringVectorStructToClass(methodName),
                                                  nullptr,
                                                  isAsync,
                                                  ArkWebStringStructToClass(permission));
        return;
    }
    nweb_nweb_->RegisterNativeJavaScriptProxy(ArkWebStringStructToClass(objName),
                                              ArkWebStringVectorStructToClass(methodName),
                                              std::make_shared<ArkWebJsProxyMethodWrapper>(data),
                                              isAsync,
                                              ArkWebStringStructToClass(permission));
}

void ArkWebNWebImpl::SetFocusWindowId(uint32_t focus_window_id)
{
    nweb_nweb_->SetFocusWindowId(focus_window_id);
}

void ArkWebNWebImpl::RunDataDetectorJS()
{
    nweb_nweb_->RunDataDetectorJS();
}

void ArkWebNWebImpl::SetDataDetectorEnable(bool enable)
{
    nweb_nweb_->SetDataDetectorEnable(enable);
}

void ArkWebNWebImpl::OnDataDetectorSelectText()
{
    nweb_nweb_->OnDataDetectorSelectText();
}

void ArkWebNWebImpl::OnDataDetectorCopy(const ArkWebStringVector& recordMix)
{
    nweb_nweb_->OnDataDetectorCopy(ArkWebStringVectorStructToClass(recordMix));
}

void ArkWebNWebImpl::SetPipNativeWindow(int delegate_id,
                                        int child_id,
                                        int frame_routing_id,
                                        void* window)
{
    nweb_nweb_->SetPipNativeWindow(delegate_id, child_id,
                                   frame_routing_id, window);
}

void ArkWebNWebImpl::SendPipEvent(int delegate_id,
                                  int child_id,
                                  int frame_routing_id,
                                  int event)
{
    nweb_nweb_->SendPipEvent(delegate_id, child_id,
                             frame_routing_id,event);
}

void ArkWebNWebImpl::SetBlanklessLoadingKey(const ArkWebString& key)
{
    nweb_nweb_->SetBlanklessLoadingKey(ArkWebStringStructToClass(key));
}

void ArkWebNWebImpl::SetPrivacyStatus(bool isPrivate)
{
    nweb_nweb_->SetPrivacyStatus(isPrivate);
}

int ArkWebNWebImpl::GetSelectStartIndex()
{
    return nweb_nweb_->GetSelectStartIndex();
}

int ArkWebNWebImpl::GetSelectEndIndex()
{
    return nweb_nweb_->GetSelectEndIndex();
}

ArkWebString ArkWebNWebImpl::GetAllTextInfo()
{
    return ArkWebStringClassToStruct(nweb_nweb_->GetAllTextInfo());
}

void ArkWebNWebImpl::SetAudioSessionType(int32_t audio_session_type)
{
    nweb_nweb_->SetAudioSessionType(audio_session_type);
}

int64_t ArkWebNWebImpl::GetWebAccessibilityIdByHtmlElementId(const ArkWebString& htmlElementId)
{
    return nweb_nweb_->GetWebAccessibilityIdByHtmlElementId(ArkWebStringStructToClass(htmlElementId));
}

int32_t ArkWebNWebImpl::GetBlanklessInfoWithKey(const ArkWebString& key, double* similarity, int32_t* loadingTime)
{
    return nweb_nweb_->GetBlanklessInfoWithKey(ArkWebStringStructToClass(key), similarity, loadingTime);
}

int32_t ArkWebNWebImpl::SetBlanklessLoadingWithKey(const ArkWebString& key, bool isStart)
{
    return nweb_nweb_->SetBlanklessLoadingWithKey(ArkWebStringStructToClass(key), isStart);
}

void ArkWebNWebImpl::UpdateSingleHandleVisible(bool isVisible)
{
    nweb_nweb_->UpdateSingleHandleVisible(isVisible);
}

void ArkWebNWebImpl::SetTouchHandleExistState(bool touchHandleExist)
{
    nweb_nweb_->SetTouchHandleExistState(touchHandleExist);
}

void ArkWebNWebImpl::AvoidVisibleViewportBottom(int32_t avoidHeight)
{
    nweb_nweb_->AvoidVisibleViewportBottom(avoidHeight);
}

int32_t ArkWebNWebImpl::GetVisibleViewportAvoidHeight()
{
    return nweb_nweb_->GetVisibleViewportAvoidHeight();
}

bool ArkWebNWebImpl::TriggerBlanklessForUrl(const ArkWebString& url)
{
    return nweb_nweb_->TriggerBlanklessForUrl(ArkWebStringStructToClass(url));
}

void ArkWebNWebImpl::SetVisibility(bool isVisible)
{
    nweb_nweb_->SetVisibility(isVisible);
}

void ArkWebNWebImpl::SetViewportScaleState()
{
    nweb_nweb_->SetViewportScaleState();
}

void ArkWebNWebImpl::GetPageOffset(float* offset_x, float* offset_y)
{
    nweb_nweb_->GetPageOffset(offset_x, offset_y);
}

void ArkWebNWebImpl::SetErrorPageEnabled(bool enable)
{
    nweb_nweb_->SetErrorPageEnabled(enable);
}

bool ArkWebNWebImpl::GetErrorPageEnabled()
{
    return nweb_nweb_->GetErrorPageEnabled();
}

int32_t ArkWebNWebImpl::GetWebDestroyMode()
{
    return static_cast<int32_t>(nweb_nweb_->GetWebDestroyMode());
}

void ArkWebNWebImpl::CallH5FunctionV2(
    int32_t routing_id, int32_t h5_object_id, const ArkWebString& h5_method_name, const ArkWebRomValueVector& args)
{
    nweb_nweb_->CallH5FunctionV2(
        routing_id, h5_object_id, ArkWebStringStructToClass(h5_method_name), ArkWebRomValueVectorStructToClass(args));
}

void ArkWebNWebImpl::PostPortMessageV2(const ArkWebString& portHandle, ArkWebRefPtr<ArkWebRomValue> data)
{
    if (CHECK_REF_PTR_IS_NULL(data)) {
        nweb_nweb_->PostPortMessageV2(ArkWebStringStructToClass(portHandle), nullptr);
        return;
    }

    nweb_nweb_->PostPortMessageV2(ArkWebStringStructToClass(portHandle), std::make_shared<ArkWebRomValueWrapper>(data));
}

void ArkWebNWebImpl::FillAutofillDataV2(ArkWebRefPtr<ArkWebRomValue> data)
{
    if (CHECK_REF_PTR_IS_NULL(data)) {
        nweb_nweb_->FillAutofillDataV2(nullptr);
        return;
    }

    nweb_nweb_->FillAutofillDataV2(std::make_shared<ArkWebRomValueWrapper>(data));
}

void ArkWebNWebImpl::OnStylusTouchPress(
    ArkWebRefPtr<ArkWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay)
{
    if (CHECK_REF_PTR_IS_NULL(stylus_touch_point_info)) {
        nweb_nweb_->OnStylusTouchPress(nullptr, from_overlay);
        return;
    }

    nweb_nweb_->OnStylusTouchPress(
        std::make_shared<ArkWebStylusTouchPointInfoWrapper>(stylus_touch_point_info), from_overlay);
}

void ArkWebNWebImpl::OnStylusTouchRelease(
    ArkWebRefPtr<ArkWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay)
{
    if (CHECK_REF_PTR_IS_NULL(stylus_touch_point_info)) {
        nweb_nweb_->OnStylusTouchRelease(nullptr, from_overlay);
        return;
    }

    nweb_nweb_->OnStylusTouchRelease(
        std::make_shared<ArkWebStylusTouchPointInfoWrapper>(stylus_touch_point_info), from_overlay);
}

void ArkWebNWebImpl::OnStylusTouchMove(
    const ArkWebStylusTouchPointInfoVector& stylus_touch_point_infos, bool from_overlay)
{
    nweb_nweb_->OnStylusTouchMove(
        ArkWebStylusTouchPointInfoVectorStructToClass(stylus_touch_point_infos), from_overlay);
}

void ArkWebNWebImpl::RecordBlanklessFrameSize(uint32_t width, uint32_t height)
{
    nweb_nweb_->RecordBlanklessFrameSize(width, height);
}

void ArkWebNWebImpl::PrefetchPageV2(const ArkWebString& url, const ArkWebStringMap& additional_http_headers,
    int32_t minTimeBetweenPrefetchesMs, bool ignoreCacheControlNoStore)
{
    nweb_nweb_->PrefetchPageV2(ArkWebStringStructToClass(url), ArkWebStringMapStructToClass(additional_http_headers),
        minTimeBetweenPrefetchesMs, ignoreCacheControlNoStore);
}

void ArkWebNWebImpl::SetForceEnableZoom(bool forceEnableZoom)
{
    WVLOG_E("forceEnableZoom = %{public}d", forceEnableZoom);
    nweb_nweb_->SetForceEnableZoom(forceEnableZoom);
}

void ArkWebNWebImpl::SetBlankScreenDetectionConfig(bool enable, const ArkWebDoubleVector& detectionTiming,
    const ArkWebInt32Vector& detectionMethods, int32_t contentfulNodesCountThreshold)
{
    nweb_nweb_->SetBlankScreenDetectionConfig(enable,
        ArkWebBasicVectorStructToClass<double, ArkWebDoubleVector>(detectionTiming),
        ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(detectionMethods), contentfulNodesCountThreshold);
}

bool ArkWebNWebImpl::WebPageSnapshotV2(const char* id,
                                       int type,
                                       int width,
                                       int height,
                                       ArkWebRefPtr<ArkWebSnapshotCallback> callback)
{
    return nweb_nweb_->WebPageSnapshotV2(id, static_cast<ArkPixelUnit>(type), width, height,
                                         std::make_shared<ArkWebSnapshotCallbackWrapper>(callback));
}

void ArkWebNWebImpl::StopFling()
{
    WVLOG_D("ArkWebNWebImpl::StopFling");
    nweb_nweb_->StopFling();
}

void ArkWebNWebImpl::ResumeMicrophone()
{
    nweb_nweb_->ResumeMicrophone();
}

void ArkWebNWebImpl::StopMicrophone()
{
    nweb_nweb_->StopMicrophone();
}

void ArkWebNWebImpl::PauseMicrophone()
{
    nweb_nweb_->PauseMicrophone();
}

void ArkWebNWebImpl::JavaScriptOnDocumentStartByOrderV2(const ArkWebStringVectorMap& script_items,
    const ArkWebPairStringVectorMap& script_regex_items, const ArkWebStringVector& script_items_by_order)
{
    nweb_nweb_->JavaScriptOnDocumentStartByOrderV2(ArkWebStringVectorMapStructToClass(script_items),
        ArkWebPairStringVectorMapStructToClass(script_regex_items),
        ArkWebStringVectorStructToClass(script_items_by_order));
}

void ArkWebNWebImpl::JavaScriptOnDocumentEndByOrderV2(const ArkWebStringVectorMap& script_items,
    const ArkWebPairStringVectorMap& script_regex_items, const ArkWebStringVector& script_items_by_order)
{
    nweb_nweb_->JavaScriptOnDocumentEndByOrderV2(ArkWebStringVectorMapStructToClass(script_items),
        ArkWebPairStringVectorMapStructToClass(script_regex_items),
        ArkWebStringVectorStructToClass(script_items_by_order));
}

void ArkWebNWebImpl::JavaScriptOnHeadReadyByOrderV2(const ArkWebStringVectorMap& script_items,
    const ArkWebPairStringVectorMap& script_regex_items, const ArkWebStringVector& script_items_by_order)
{
    nweb_nweb_->JavaScriptOnHeadReadyByOrderV2(ArkWebStringVectorMapStructToClass(script_items),
        ArkWebPairStringVectorMapStructToClass(script_regex_items),
        ArkWebStringVectorStructToClass(script_items_by_order));
}
} // namespace OHOS::ArkWeb
