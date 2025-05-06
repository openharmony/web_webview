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

#include "ohos_nweb/bridge/ark_web_nweb_wrapper.h"

#include "ohos_nweb/bridge/ark_web_accessibility_event_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_accessibility_node_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_array_buffer_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_bool_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_cache_options_impl.h"
#include "ohos_nweb/bridge/ark_web_create_native_media_player_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_download_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_drag_data_wrapper.h"
#include "ohos_nweb/bridge/ark_web_drag_event_impl.h"
#include "ohos_nweb/bridge/ark_web_find_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_handler_impl.h"
#include "ohos_nweb/bridge/ark_web_history_list_wrapper.h"
#include "ohos_nweb/bridge/ark_web_hit_test_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_result_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_keyboard_event_impl.h"
#include "ohos_nweb/bridge/ark_web_message_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_mouse_event_impl.h"
#include "ohos_nweb/bridge/ark_web_pdfconfig_args_impl.h"
#include "ohos_nweb/bridge/ark_web_preference_wrapper.h"
#include "ohos_nweb/bridge/ark_web_release_surface_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_screen_lock_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_spanstring_convert_html_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_string_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_system_configuration_impl.h"
#include "ohos_nweb/bridge/ark_web_view_struct_utils.h"
#include "ohos_nweb/cpptoc/ark_web_js_proxy_callback_vector_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_touch_point_info_vector_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_value_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNWebWrapper::ArkWebNWebWrapper(ArkWebRefPtr<ArkWebNWeb> ark_web_nweb) : ark_web_nweb_(ark_web_nweb) {}

void ArkWebNWebWrapper::Resize(uint32_t width, uint32_t height, bool is_keyboard)
{
    ark_web_nweb_->Resize(width, height, is_keyboard);
}

void ArkWebNWebWrapper::OnPause()
{
    ark_web_nweb_->OnPause();
}

void ArkWebNWebWrapper::OnContinue()
{
    ark_web_nweb_->OnContinue();
}

void ArkWebNWebWrapper::OnDestroy()
{
    ark_web_nweb_->OnDestroy();
}

void ArkWebNWebWrapper::OnFocus(const ArkWebFocusReason& focus_reason)
{
    ark_web_nweb_->OnFocus(static_cast<int32_t>(focus_reason));
}

void ArkWebNWebWrapper::OnBlur(const ArkWebBlurReason& blur_reason)
{
    ark_web_nweb_->OnBlur(static_cast<int32_t>(blur_reason));
}

void ArkWebNWebWrapper::OnTouchPress(int32_t id, double x, double y, bool from_overlay)
{
    ark_web_nweb_->OnTouchPress(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::OnTouchRelease(int32_t id, double x, double y, bool from_overlay)
{
    ark_web_nweb_->OnTouchRelease(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::OnTouchMove(int32_t id, double x, double y, bool from_overlay)
{
    ark_web_nweb_->OnTouchMove(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::OnTouchMove(
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebTouchPointInfo>>& touch_point_infos, bool from_overlay)
{
    ArkWebTouchPointInfoVector st_touch_point_infos = ArkWebTouchPointInfoVectorClassToStruct(touch_point_infos);

    ark_web_nweb_->OnTouchMove(st_touch_point_infos, from_overlay);

    ArkWebTouchPointInfoVectorStructRelease(st_touch_point_infos);
}

void ArkWebNWebWrapper::OnTouchCancel()
{
    ark_web_nweb_->OnTouchCancel();
}

void ArkWebNWebWrapper::OnNavigateBack()
{
    ark_web_nweb_->OnNavigateBack();
}

bool ArkWebNWebWrapper::SendKeyEvent(int32_t key_code, int32_t key_action)
{
    return ark_web_nweb_->SendKeyEvent(key_code, key_action);
}

void ArkWebNWebWrapper::SendMouseWheelEvent(double x, double y, double delta_x, double delta_y)
{
    ark_web_nweb_->SendMouseWheelEvent(x, y, delta_x, delta_y);
}

void ArkWebNWebWrapper::SendMouseEvent(int x, int y, int button, int action, int count)
{
    ark_web_nweb_->SendMouseEvent(x, y, button, action, count);
}

int ArkWebNWebWrapper::Load(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    int code = ark_web_nweb_->Load(stUrl);

    ArkWebStringStructRelease(stUrl);
    return code;
}

bool ArkWebNWebWrapper::IsNavigatebackwardAllowed()
{
    return ark_web_nweb_->IsNavigatebackwardAllowed();
}

bool ArkWebNWebWrapper::IsNavigateForwardAllowed()
{
    return ark_web_nweb_->IsNavigateForwardAllowed();
}

bool ArkWebNWebWrapper::CanNavigateBackOrForward(int num_steps)
{
    return ark_web_nweb_->CanNavigateBackOrForward(num_steps);
}

void ArkWebNWebWrapper::NavigateBack()
{
    ark_web_nweb_->NavigateBack();
}

void ArkWebNWebWrapper::NavigateForward()
{
    ark_web_nweb_->NavigateForward();
}

void ArkWebNWebWrapper::NavigateBackOrForward(int step)
{
    ark_web_nweb_->NavigateBackOrForward(step);
}

void ArkWebNWebWrapper::DeleteNavigateHistory()
{
    ark_web_nweb_->DeleteNavigateHistory();
}

void ArkWebNWebWrapper::Reload()
{
    ark_web_nweb_->Reload();
}

int ArkWebNWebWrapper::Zoom(float zoom_factor)
{
    return ark_web_nweb_->Zoom(zoom_factor);
}

int ArkWebNWebWrapper::ZoomIn()
{
    return ark_web_nweb_->ZoomIn();
}

int ArkWebNWebWrapper::ZoomOut()
{
    return ark_web_nweb_->ZoomOut();
}

void ArkWebNWebWrapper::Stop()
{
    ark_web_nweb_->Stop();
}

void ArkWebNWebWrapper::ExecuteJavaScript(const std::string& code)
{
    ArkWebString stCode = ArkWebStringClassToStruct(code);

    ark_web_nweb_->ExecuteJavaScript(stCode);

    ArkWebStringStructRelease(stCode);
}

void ArkWebNWebWrapper::ExecuteJavaScript(
    const std::string& code, std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback, bool extention)
{
    ArkWebString stCode = ArkWebStringClassToStruct(code);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->ExecuteJavaScript(stCode, nullptr, extention);
    } else {
        ark_web_nweb_->ExecuteJavaScript(stCode, new ArkWebMessageValueCallbackImpl(callback), extention);
    }

    ArkWebStringStructRelease(stCode);
}

std::shared_ptr<OHOS::NWeb::NWebPreference> ArkWebNWebWrapper::GetPreference()
{
    ArkWebRefPtr<ArkWebPreference> ark_web_preference = ark_web_nweb_->GetPreference();
    if (CHECK_REF_PTR_IS_NULL(ark_web_preference)) {
        return nullptr;
    }

    return std::make_shared<ArkWebPreferenceWrapper>(ark_web_preference);
}

unsigned int ArkWebNWebWrapper::GetWebId()
{
    return ark_web_nweb_->GetWebId();
}

std::shared_ptr<OHOS::NWeb::HitTestResult> ArkWebNWebWrapper::GetHitTestResult()
{
    ArkWebRefPtr<ArkWebHitTestResult> ark_web_hit_test_result = ark_web_nweb_->GetHitTestResult();
    if (CHECK_REF_PTR_IS_NULL(ark_web_hit_test_result)) {
        return nullptr;
    }

    return std::make_shared<ArkWebHitTestResultWrapper>(ark_web_hit_test_result);
}

void ArkWebNWebWrapper::PutBackgroundColor(int color)
{
    ark_web_nweb_->PutBackgroundColor(color);
}

void ArkWebNWebWrapper::InitialScale(float scale)
{
    ark_web_nweb_->InitialScale(scale);
}

void ArkWebNWebWrapper::PutDownloadCallback(std::shared_ptr<OHOS::NWeb::NWebDownloadCallback> download_listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(download_listener)) {
        ark_web_nweb_->PutDownloadCallback(nullptr);
        return;
    }

    ark_web_nweb_->PutDownloadCallback(new ArkWebDownloadCallbackImpl(download_listener));
}

void ArkWebNWebWrapper::PutAccessibilityEventCallback(
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback> accessibility_event_listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(accessibility_event_listener)) {
        ark_web_nweb_->PutAccessibilityEventCallback(nullptr);
        return;
    }

    ark_web_nweb_->PutAccessibilityEventCallback(
        new ArkWebAccessibilityEventCallbackImpl(accessibility_event_listener));
}

void ArkWebNWebWrapper::PutAccessibilityIdGenerator(const AccessibilityIdGenerateFunc accessibility_id_generator)
{
    ark_web_nweb_->PutAccessibilityIdGenerator(accessibility_id_generator);
}

void ArkWebNWebWrapper::SetNWebHandler(std::shared_ptr<OHOS::NWeb::NWebHandler> handler)
{
    if (CHECK_SHARED_PTR_IS_NULL(handler)) {
        ark_web_nweb_->SetNWebHandler(nullptr);
        return;
    }

    ark_web_nweb_->SetNWebHandler(new ArkWebHandlerImpl(handler));
}

std::string ArkWebNWebWrapper::Title()
{
    return ArkWebStringStructToClass(ark_web_nweb_->Title());
}

int ArkWebNWebWrapper::PageLoadProgress()
{
    return ark_web_nweb_->PageLoadProgress();
}

int ArkWebNWebWrapper::ContentHeight()
{
    return ark_web_nweb_->ContentHeight();
}

float ArkWebNWebWrapper::Scale()
{
    return ark_web_nweb_->Scale();
}

int ArkWebNWebWrapper::Load(const std::string& url, const std::map<std::string, std::string>& additional_http_headers)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebStringMap stHeaders = ArkWebStringMapClassToStruct(additional_http_headers);

    int code = ark_web_nweb_->Load(stUrl, stHeaders);

    ArkWebStringStructRelease(stUrl);
    ArkWebStringMapStructRelease(stHeaders);
    return code;
}

int ArkWebNWebWrapper::LoadWithDataAndBaseUrl(const std::string& base_url, const std::string& data,
    const std::string& mime_type, const std::string& encoding, const std::string& history_url)
{
    ArkWebString stData = ArkWebStringClassToStruct(data);
    ArkWebString stBaseUrl = ArkWebStringClassToStruct(base_url);
    ArkWebString stEncoding = ArkWebStringClassToStruct(encoding);
    ArkWebString stMimeType = ArkWebStringClassToStruct(mime_type);
    ArkWebString stHistoryUrl = ArkWebStringClassToStruct(history_url);

    int code = ark_web_nweb_->LoadWithDataAndBaseUrl(stBaseUrl, stData, stMimeType, stEncoding, stHistoryUrl);

    ArkWebStringStructRelease(stData);
    ArkWebStringStructRelease(stBaseUrl);
    ArkWebStringStructRelease(stEncoding);
    ArkWebStringStructRelease(stMimeType);
    ArkWebStringStructRelease(stHistoryUrl);
    return code;
}

int ArkWebNWebWrapper::LoadWithData(const std::string& data, const std::string& mime_type, const std::string& encoding)
{
    ArkWebString stData = ArkWebStringClassToStruct(data);
    ArkWebString stEncoding = ArkWebStringClassToStruct(encoding);
    ArkWebString stMimeType = ArkWebStringClassToStruct(mime_type);

    int code = ark_web_nweb_->LoadWithData(stData, stMimeType, stEncoding);

    ArkWebStringStructRelease(stData);
    ArkWebStringStructRelease(stEncoding);
    ArkWebStringStructRelease(stMimeType);
    return code;
}

void ArkWebNWebWrapper::RegisterArkJSfunction(
    const std::string& object_name, const std::vector<std::string>& method_list, const int32_t object_id)
{
    ArkWebString stName = ArkWebStringClassToStruct(object_name);
    ArkWebStringVector stMethods = ArkWebStringVectorClassToStruct(method_list);

    ark_web_nweb_->RegisterArkJSfunction(stName, stMethods, object_id);

    ArkWebStringStructRelease(stName);
    ArkWebStringVectorStructRelease(stMethods);
}

void ArkWebNWebWrapper::UnregisterArkJSfunction(
    const std::string& object_name, const std::vector<std::string>& method_list)
{
    ArkWebString stName = ArkWebStringClassToStruct(object_name);
    ArkWebStringVector stMethods = ArkWebStringVectorClassToStruct(method_list);

    ark_web_nweb_->UnregisterArkJSfunction(stName, stMethods);

    ArkWebStringStructRelease(stName);
    ArkWebStringVectorStructRelease(stMethods);
}

void ArkWebNWebWrapper::SetNWebJavaScriptResultCallBack(
    std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBack> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->SetNWebJavaScriptResultCallBack(nullptr);
        return;
    }

    ark_web_nweb_->SetNWebJavaScriptResultCallBack(new ArkWebJsResultCallbackImpl(callback));
}

void ArkWebNWebWrapper::PutFindCallback(std::shared_ptr<OHOS::NWeb::NWebFindCallback> find_listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(find_listener)) {
        ark_web_nweb_->PutFindCallback(nullptr);
        return;
    }

    ark_web_nweb_->PutFindCallback(new ArkWebFindCallbackImpl(find_listener));
}

void ArkWebNWebWrapper::FindAllAsync(const std::string& search_str)
{
    ArkWebString stSearchStr = ArkWebStringClassToStruct(search_str);

    ark_web_nweb_->FindAllAsync(stSearchStr);

    ArkWebStringStructRelease(stSearchStr);
}

void ArkWebNWebWrapper::ClearMatches()
{
    ark_web_nweb_->ClearMatches();
}

void ArkWebNWebWrapper::FindNext(const bool forward)
{
    ark_web_nweb_->FindNext(forward);
}

void ArkWebNWebWrapper::StoreWebArchive(
    const std::string& base_name, bool auto_name, std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback)
{
    ArkWebString stName = ArkWebStringClassToStruct(base_name);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->StoreWebArchive(stName, auto_name, nullptr);
    } else {
        ark_web_nweb_->StoreWebArchive(stName, auto_name, new ArkWebStringValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stName);
}

std::vector<std::string> ArkWebNWebWrapper::CreateWebMessagePorts()
{
    ArkWebStringVector stPorts = ark_web_nweb_->CreateWebMessagePorts();

    std::vector<std::string> objPorts = ArkWebStringVectorStructToClass(stPorts);

    ArkWebStringVectorStructRelease(stPorts);
    return objPorts;
}

void ArkWebNWebWrapper::PostWebMessage(
    const std::string& message, const std::vector<std::string>& ports, const std::string& target_uri)
{
    ArkWebString stMessage = ArkWebStringClassToStruct(message);
    ArkWebString stTargetUrl = ArkWebStringClassToStruct(target_uri);
    ArkWebStringVector stPorts = ArkWebStringVectorClassToStruct(ports);

    ark_web_nweb_->PostWebMessage(stMessage, stPorts, stTargetUrl);

    ArkWebStringStructRelease(stMessage);
    ArkWebStringStructRelease(stTargetUrl);
    ArkWebStringVectorStructRelease(stPorts);
}

void ArkWebNWebWrapper::ClosePort(const std::string& port_handle)
{
    ArkWebString stPort = ArkWebStringClassToStruct(port_handle);

    ark_web_nweb_->ClosePort(stPort);

    ArkWebStringStructRelease(stPort);
}

void ArkWebNWebWrapper::PostPortMessage(const std::string& port_handle, std::shared_ptr<OHOS::NWeb::NWebMessage> data)
{
    ArkWebString stPort = ArkWebStringClassToStruct(port_handle);

    ArkWebMessage ark_web_message;
    ark_web_message.nweb_message = data;
    ark_web_nweb_->PostPortMessage(stPort, ark_web_message);

    ArkWebStringStructRelease(stPort);
}

void ArkWebNWebWrapper::SetPortMessageCallback(
    const std::string& port_handle, std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback)
{
    ArkWebString stPort = ArkWebStringClassToStruct(port_handle);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->SetPortMessageCallback(stPort, nullptr);
    } else {
        ark_web_nweb_->SetPortMessageCallback(stPort, new ArkWebMessageValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stPort);
}

void ArkWebNWebWrapper::SendDragEvent(std::shared_ptr<OHOS::NWeb::NWebDragEvent> drag_event)
{
    if (CHECK_SHARED_PTR_IS_NULL(drag_event)) {
        ark_web_nweb_->SendDragEvent(nullptr);
        return;
    }

    ark_web_nweb_->SendDragEvent(new ArkWebDragEventImpl(drag_event));
}

void ArkWebNWebWrapper::ClearSslCache()
{
    ark_web_nweb_->ClearSslCache();
}

std::string ArkWebNWebWrapper::GetUrl()
{
    ArkWebString stUrl = ark_web_nweb_->GetUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

void ArkWebNWebWrapper::ClearClientAuthenticationCache()
{
    ark_web_nweb_->ClearClientAuthenticationCache();
}

void ArkWebNWebWrapper::UpdateLocale(const std::string& language, const std::string& region)
{
    ArkWebString stRegion = ArkWebStringClassToStruct(region);
    ArkWebString stLanguage = ArkWebStringClassToStruct(language);

    ark_web_nweb_->UpdateLocale(stLanguage, stRegion);

    ArkWebStringStructRelease(stRegion);
    ArkWebStringStructRelease(stLanguage);
}

const std::string ArkWebNWebWrapper::GetOriginalUrl()
{
    ArkWebString stUrl = ark_web_nweb_->GetOriginalUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

bool ArkWebNWebWrapper::GetFavicon(const void** data, size_t& width, size_t& height, ArkWebImageColorType& color_type,
    ArkWebImageAlphaType& alpha_type)
{
    int enum_color_type = static_cast<int>(ArkWebImageColorType::COLOR_TYPE_UNKNOWN);
    int enum_alpha_type = static_cast<int>(ArkWebImageAlphaType::ALPHA_TYPE_UNKNOWN);
    bool result = ark_web_nweb_->GetFavicon(data, width, height, enum_color_type, enum_alpha_type);
    color_type = static_cast<ArkWebImageColorType>(enum_color_type);
    alpha_type = static_cast<ArkWebImageAlphaType>(enum_alpha_type);
    return result;
}

void ArkWebNWebWrapper::PutNetworkAvailable(bool available)
{
    ark_web_nweb_->PutNetworkAvailable(available);
}

void ArkWebNWebWrapper::HasImages(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->HasImages(nullptr);
        return;
    }

    ark_web_nweb_->HasImages(new ArkWebBoolValueCallbackImpl(callback));
}

void ArkWebNWebWrapper::RemoveCache(bool include_disk_files)
{
    ark_web_nweb_->RemoveCache(include_disk_files);
}

std::shared_ptr<OHOS::NWeb::NWebHistoryList> ArkWebNWebWrapper::GetHistoryList()
{
    ArkWebRefPtr<ArkWebHistoryList> ark_web_history_list = ark_web_nweb_->GetHistoryList();
    if (CHECK_REF_PTR_IS_NULL(ark_web_history_list)) {
        return nullptr;
    }

    return std::make_shared<ArkWebHistoryListWrapper>(ark_web_history_list);
}

void ArkWebNWebWrapper::PutReleaseSurfaceCallback(
    std::shared_ptr<OHOS::NWeb::NWebReleaseSurfaceCallback> release_surface_listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(release_surface_listener)) {
        ark_web_nweb_->PutReleaseSurfaceCallback(nullptr);
        return;
    }

    ark_web_nweb_->PutReleaseSurfaceCallback(new ArkWebReleaseSurfaceCallbackImpl(release_surface_listener));
}

std::vector<uint8_t> ArkWebNWebWrapper::SerializeWebState()
{
    ArkWebUint8Vector stState = ark_web_nweb_->SerializeWebState();

    std::vector<uint8_t> objState = ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(stState);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stState);
    return objState;
}

bool ArkWebNWebWrapper::RestoreWebState(const std::vector<uint8_t>& state)
{
    ArkWebUint8Vector stState = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(state);

    bool flag = ark_web_nweb_->RestoreWebState(stState);

    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stState);
    return flag;
}

void ArkWebNWebWrapper::PageUp(bool top)
{
    ark_web_nweb_->PageUp(top);
}

void ArkWebNWebWrapper::PageDown(bool bottom)
{
    ark_web_nweb_->PageDown(bottom);
}

void ArkWebNWebWrapper::ScrollTo(float x, float y)
{
    ark_web_nweb_->ScrollTo(x, y);
}

void ArkWebNWebWrapper::ScrollBy(float delta_x, float delta_y)
{
    ark_web_nweb_->ScrollBy(delta_x, delta_y);
}

void ArkWebNWebWrapper::SlideScroll(float vx, float vy)
{
    ark_web_nweb_->SlideScroll(vx, vy);
}

bool ArkWebNWebWrapper::GetCertChainDerData(std::vector<std::string>& cert_chain_data, bool is_single_cert)
{
    ArkWebStringVector cert_chain_data_vector;
    bool result = ark_web_nweb_->GetCertChainDerData(cert_chain_data_vector, is_single_cert);
    cert_chain_data = ArkWebStringVectorStructToClass(cert_chain_data_vector);

    ArkWebStringVectorStructRelease(cert_chain_data_vector);
    return result;
}

void ArkWebNWebWrapper::SetScreenOffSet(double x, double y)
{
    ark_web_nweb_->SetScreenOffSet(x, y);
}

void ArkWebNWebWrapper::SetAudioMuted(bool muted)
{
    ark_web_nweb_->SetAudioMuted(muted);
}

void ArkWebNWebWrapper::SetShouldFrameSubmissionBeforeDraw(bool should)
{
    ark_web_nweb_->SetShouldFrameSubmissionBeforeDraw(should);
}

void ArkWebNWebWrapper::NotifyPopupWindowResult(bool result)
{
    ark_web_nweb_->NotifyPopupWindowResult(result);
}

void ArkWebNWebWrapper::SetAudioResumeInterval(int32_t resume_interval)
{
    ark_web_nweb_->SetAudioResumeInterval(resume_interval);
}

void ArkWebNWebWrapper::SetAudioExclusive(bool audio_exclusive)
{
    ark_web_nweb_->SetAudioExclusive(audio_exclusive);
}

void ArkWebNWebWrapper::RegisterScreenLockFunction(
    int32_t window_id, std::shared_ptr<OHOS::NWeb::NWebScreenLockCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->RegisterScreenLockFunction(window_id, nullptr);
        return;
    }

    ark_web_nweb_->RegisterScreenLockFunction(window_id, new ArkWebScreenLockCallbackImpl(callback));
}

void ArkWebNWebWrapper::UnRegisterScreenLockFunction(int32_t window_id)
{
    ark_web_nweb_->UnRegisterScreenLockFunction(window_id);
}

void ArkWebNWebWrapper::NotifyMemoryLevel(int32_t level)
{
    ark_web_nweb_->NotifyMemoryLevel(level);
}

void ArkWebNWebWrapper::OnWebviewHide()
{
    ark_web_nweb_->OnWebviewHide();
}

void ArkWebNWebWrapper::OnWebviewShow()
{
    ark_web_nweb_->OnWebviewShow();
}

std::shared_ptr<OHOS::NWeb::NWebDragData> ArkWebNWebWrapper::GetOrCreateDragData()
{
    ArkWebRefPtr<ArkWebDragData> ark_web_drag_data = ark_web_nweb_->GetOrCreateDragData();
    if (CHECK_REF_PTR_IS_NULL(ark_web_drag_data)) {
        return nullptr;
    }

    return std::make_shared<ArkWebDragDataWrapper>(ark_web_drag_data);
}

void ArkWebNWebWrapper::PrefetchPage(
    const std::string& url, const std::map<std::string, std::string>& additional_http_headers)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebStringMap stHeaders = ArkWebStringMapClassToStruct(additional_http_headers);

    ark_web_nweb_->PrefetchPage(stUrl, stHeaders);

    ArkWebStringStructRelease(stUrl);
    ArkWebStringMapStructRelease(stHeaders);
}

void ArkWebNWebWrapper::SetWindowId(uint32_t window_id)
{
    ark_web_nweb_->SetWindowId(window_id);
}

void ArkWebNWebWrapper::OnOccluded()
{
    ark_web_nweb_->OnOccluded();
}

void ArkWebNWebWrapper::OnUnoccluded()
{
    ark_web_nweb_->OnUnoccluded();
}

void ArkWebNWebWrapper::SetToken(void* token)
{
    ark_web_nweb_->SetToken(token);
}

void ArkWebNWebWrapper::SetNestedScrollMode(const ArkWebNestedScrollMode& nested_scroll_mode)
{
    ark_web_nweb_->SetNestedScrollMode(static_cast<int32_t>(nested_scroll_mode));
}

void ArkWebNWebWrapper::SetEnableLowerFrameRate(bool enabled)
{
    ark_web_nweb_->SetEnableLowerFrameRate(enabled);
}

void ArkWebNWebWrapper::SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard)
{
    ark_web_nweb_->SetVirtualKeyBoardArg(width, height, keyboard);
}

bool ArkWebNWebWrapper::ShouldVirtualKeyboardOverlay()
{
    return ark_web_nweb_->ShouldVirtualKeyboardOverlay();
}

void ArkWebNWebWrapper::SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
    ark_web_nweb_->SetDrawRect(x, y, width, height);
}

void ArkWebNWebWrapper::SetDrawMode(int32_t mode)
{
    ark_web_nweb_->SetDrawMode(mode);
}

void* ArkWebNWebWrapper::CreateWebPrintDocumentAdapter(const std::string& job_name)
{
    ArkWebString stObjName = ArkWebStringClassToStruct(job_name);

    void* adapter = ark_web_nweb_->CreateWebPrintDocumentAdapter(stObjName);

    ArkWebStringStructRelease(stObjName);
    return adapter;
}

int ArkWebNWebWrapper::PostUrl(const std::string& url, const std::vector<char>& post_data)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebCharVector stPostData = ArkWebBasicVectorClassToStruct<char, ArkWebCharVector>(post_data);

    int code = ark_web_nweb_->PostUrl(stUrl, stPostData);

    ArkWebStringStructRelease(stUrl);
    ArkWebBasicVectorStructRelease<ArkWebCharVector>(stPostData);
    return code;
}

void ArkWebNWebWrapper::JavaScriptOnDocumentStart(const std::map<std::string, std::vector<std::string>>& script_items)
{
    ArkWebStringVectorMap stScriptItems = ArkWebStringVectorMapClassToStruct(script_items);

    ark_web_nweb_->JavaScriptOnDocumentStart(stScriptItems);

    ArkWebStringVectorMapStructRelease(stScriptItems);
}

void ArkWebNWebWrapper::JavaScriptOnDocumentStartByOrder(
    const std::map<std::string, std::vector<std::string>>& script_items,
    const std::vector<std::string>& script_items_by_order)
{
    ArkWebStringVectorMap stScriptItems = ArkWebStringVectorMapClassToStruct(script_items);
    ArkWebStringVector stscript_items_by_order = ArkWebStringVectorClassToStruct(script_items_by_order);

    ark_web_nweb_->JavaScriptOnDocumentStartByOrder(stScriptItems, stscript_items_by_order);

    ArkWebStringVectorMapStructRelease(stScriptItems);
    ArkWebStringVectorStructRelease(stscript_items_by_order);
}

void ArkWebNWebWrapper::JavaScriptOnDocumentEndByOrder(
    const std::map<std::string, std::vector<std::string>>& script_items,
    const std::vector<std::string>& script_items_by_order)
{
    ArkWebStringVectorMap stScriptItems = ArkWebStringVectorMapClassToStruct(script_items);
    ArkWebStringVector stscript_items_by_order = ArkWebStringVectorClassToStruct(script_items_by_order);

    ark_web_nweb_->JavaScriptOnDocumentEndByOrder(stScriptItems, stscript_items_by_order);

    ArkWebStringVectorMapStructRelease(stScriptItems);
    ArkWebStringVectorStructRelease(stscript_items_by_order);
}

void ArkWebNWebWrapper::JavaScriptOnHeadReadyByOrder(
    const std::map<std::string, std::vector<std::string>>& script_items,
    const std::vector<std::string>& script_items_by_order)
{
    ArkWebStringVectorMap stScriptItems = ArkWebStringVectorMapClassToStruct(script_items);
    ArkWebStringVector stscript_items_by_order = ArkWebStringVectorClassToStruct(script_items_by_order);

    ark_web_nweb_->JavaScriptOnHeadReadyByOrder(stScriptItems, stscript_items_by_order);

    ArkWebStringVectorMapStructRelease(stScriptItems);
    ArkWebStringVectorStructRelease(stscript_items_by_order);
}

void ArkWebNWebWrapper::ExecuteAction(int64_t accessibility_id, uint32_t action)
{
    ark_web_nweb_->ExecuteAction(accessibility_id, action);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> ArkWebNWebWrapper::GetFocusedAccessibilityNodeInfo(
    int64_t accessibility_id, bool is_accessibility_focus)
{
    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info =
        ark_web_nweb_->GetFocusedAccessibilityNodeInfo(accessibility_id, is_accessibility_focus);
    if (CHECK_REF_PTR_IS_NULL(ark_web_accessibility_node_info)) {
        return nullptr;
    }

    return std::make_shared<ArkWebAccessibilityNodeInfoWrapper>(ark_web_accessibility_node_info);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> ArkWebNWebWrapper::GetAccessibilityNodeInfoById(
    int64_t accessibility_id)
{
    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info =
        ark_web_nweb_->GetAccessibilityNodeInfoById(accessibility_id);
    if (CHECK_REF_PTR_IS_NULL(ark_web_accessibility_node_info)) {
        return nullptr;
    }

    return std::make_shared<ArkWebAccessibilityNodeInfoWrapper>(ark_web_accessibility_node_info);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> ArkWebNWebWrapper::GetAccessibilityNodeInfoByFocusMove(
    int64_t accessibility_id, int32_t direction)
{
    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info =
        ark_web_nweb_->GetAccessibilityNodeInfoByFocusMove(accessibility_id, direction);
    if (CHECK_REF_PTR_IS_NULL(ark_web_accessibility_node_info)) {
        return nullptr;
    }

    return std::make_shared<ArkWebAccessibilityNodeInfoWrapper>(ark_web_accessibility_node_info);
}

void ArkWebNWebWrapper::SetAccessibilityState(bool state)
{
    ark_web_nweb_->SetAccessibilityState(state);
}

bool ArkWebNWebWrapper::NeedSoftKeyboard()
{
    return ark_web_nweb_->NeedSoftKeyboard();
}

bool ArkWebNWebWrapper::Discard()
{
    return ark_web_nweb_->Discard();
}

bool ArkWebNWebWrapper::Restore()
{
    return ark_web_nweb_->Restore();
}

int ArkWebNWebWrapper::GetSecurityLevel()
{
    return ark_web_nweb_->GetSecurityLevel();
}

void ArkWebNWebWrapper::CallH5Function(int32_t routingId, int32_t h5ObjectId, const std::string& h5MethodName,
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>>& args)
{
    ArkWebString stName = ArkWebStringClassToStruct(h5MethodName);
    ArkWebValueVector stArgs = ArkWebValueVectorClassToStruct(args);

    ark_web_nweb_->CallH5Function(routingId, h5ObjectId, stName, stArgs);

    ArkWebStringStructRelease(stName);
    ArkWebValueVectorStructRelease(stArgs);
}

bool ArkWebNWebWrapper::IsIncognitoMode()
{
    return ark_web_nweb_->IsIncognitoMode();
}

void ArkWebNWebWrapper::RegisterNativeArkJSFunction(
    const char* objName, const std::vector<std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback>>& callbacks)
{
    ArkWebJsProxyCallbackVector stJsProxyCallbackVector = ArkWebJsProxyCallbackVectorClassToStruct(callbacks);

    ark_web_nweb_->RegisterNativeArkJSFunction(objName, stJsProxyCallbackVector);

    ArkWebJsProxyCallbackVectorStructRelease(stJsProxyCallbackVector);
}

void ArkWebNWebWrapper::UnRegisterNativeArkJSFunction(const char* objName)
{
    ark_web_nweb_->UnRegisterNativeArkJSFunction(objName);
}

void ArkWebNWebWrapper::RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback)
{
    ark_web_nweb_->RegisterNativeValideCallback(webName, callback);
}

void ArkWebNWebWrapper::RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnValidCallback callback)
{
    ark_web_nweb_->RegisterNativeDestroyCallback(webName, callback);
}

void ArkWebNWebWrapper::JavaScriptOnDocumentEnd(const std::map<std::string, std::vector<std::string>>& script_items)
{
    ArkWebStringVectorMap stScriptItems = ArkWebStringVectorMapClassToStruct(script_items);

    ark_web_nweb_->JavaScriptOnDocumentEnd(stScriptItems);

    ArkWebStringVectorMapStructRelease(stScriptItems);
}

void ArkWebNWebWrapper::EnableSafeBrowsing(bool enable)
{
    ark_web_nweb_->EnableSafeBrowsing(enable);
}

bool ArkWebNWebWrapper::IsSafeBrowsingEnabled()
{
    return ark_web_nweb_->IsSafeBrowsingEnabled();
}

void ArkWebNWebWrapper::SetPrintBackground(bool enable)
{
    ark_web_nweb_->SetPrintBackground(enable);
}

bool ArkWebNWebWrapper::GetPrintBackground()
{
    return ark_web_nweb_->GetPrintBackground();
}

void ArkWebNWebWrapper::CloseAllMediaPresentations()
{
    ark_web_nweb_->CloseAllMediaPresentations();
}

void ArkWebNWebWrapper::StopAllMedia()
{
    ark_web_nweb_->StopAllMedia();
}

void ArkWebNWebWrapper::ResumeAllMedia()
{
    ark_web_nweb_->ResumeAllMedia();
}

void ArkWebNWebWrapper::PauseAllMedia()
{
    ark_web_nweb_->PauseAllMedia();
}

int ArkWebNWebWrapper::GetMediaPlaybackState()
{
    return ark_web_nweb_->GetMediaPlaybackState();
}

void ArkWebNWebWrapper::EnableIntelligentTrackingPrevention(bool enable)
{
    ark_web_nweb_->EnableIntelligentTrackingPrevention(enable);
}

bool ArkWebNWebWrapper::IsIntelligentTrackingPreventionEnabled() const
{
    return ark_web_nweb_->IsIntelligentTrackingPreventionEnabled();
}

void ArkWebNWebWrapper::StartCamera()
{
    ark_web_nweb_->StartCamera();
}

void ArkWebNWebWrapper::StopCamera()
{
    ark_web_nweb_->StopCamera();
}

void ArkWebNWebWrapper::CloseCamera()
{
    ark_web_nweb_->CloseCamera();
}

std::string ArkWebNWebWrapper::GetLastJavascriptProxyCallingFrameUrl()
{
    return ArkWebStringStructToClass(ark_web_nweb_->GetLastJavascriptProxyCallingFrameUrl());
}

bool ArkWebNWebWrapper::GetPendingSizeStatus()
{
    return ark_web_nweb_->GetPendingSizeStatus();
}

void ArkWebNWebWrapper::ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy)
{
    ark_web_nweb_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
}

void ArkWebNWebWrapper::ExecuteJavaScriptExt(const int fd, const size_t scriptLength,
    std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback, bool extention)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->ExecuteJavaScriptExt(fd, scriptLength, nullptr, extention);
    } else {
        ark_web_nweb_->ExecuteJavaScriptExt(fd, scriptLength, new ArkWebMessageValueCallbackImpl(callback), extention);
    }
}

void ArkWebNWebWrapper::OnRenderToBackground()
{
    ark_web_nweb_->OnRenderToBackground();
}

void ArkWebNWebWrapper::OnRenderToForeground()
{
    ark_web_nweb_->OnRenderToForeground();
}

void ArkWebNWebWrapper::PrecompileJavaScript(const std::string& url, const std::string& script,
    std::shared_ptr<OHOS::NWeb::CacheOptions>& cacheOptions,
    std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback)
{
    ArkWebString ark_url = ArkWebStringClassToStruct(url);
    ArkWebString ark_script = ArkWebStringClassToStruct(script);
    ArkWebRefPtr<ArkWebCacheOptions> options = new ArkWebCacheOptionsImpl(cacheOptions);
    ark_web_nweb_->PrecompileJavaScript(ark_url, ark_script, options, new ArkWebMessageValueCallbackImpl(callback));
    ArkWebStringStructRelease(ark_url);
    ArkWebStringStructRelease(ark_script);
}

void ArkWebNWebWrapper::OnCreateNativeMediaPlayer(
    std::shared_ptr<OHOS::NWeb::NWebCreateNativeMediaPlayerCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return;
    }

    ark_web_nweb_->OnCreateNativeMediaPlayer(new ArkWebCreateNativeMediaPlayerCallbackImpl(callback));
}

void ArkWebNWebWrapper::SendTouchpadFlingEvent(double x, double y, double vx, double vy)
{
    ark_web_nweb_->SendTouchpadFlingEvent(x, y, vx, vy);
}

void ArkWebNWebWrapper::DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width)
{
    ark_web_nweb_->DragResize(width, height, pre_height, pre_width);
}

void ArkWebNWebWrapper::OnTouchCancelById(int32_t id, double x, double y, bool from_overlay)
{
    ark_web_nweb_->OnTouchCancelById(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::InjectOfflineResource(const std::string& url, const std::string& origin,
    const std::vector<uint8_t>& resource, const std::map<std::string, std::string>& responseHeaders, const int type)
{
    ArkWebString arkUrl = ArkWebStringClassToStruct(url);
    ArkWebString arkOrigin = ArkWebStringClassToStruct(origin);

    auto responseHeadersMap = ArkWebStringMapClassToStruct(responseHeaders);

    ArkWebUint8Vector arkResource = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(resource);

    ark_web_nweb_->InjectOfflineResource(arkUrl, arkOrigin, arkResource, responseHeadersMap, type);

    ArkWebStringStructRelease(arkUrl);
    ArkWebStringStructRelease(arkOrigin);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(arkResource);
    ArkWebStringMapStructRelease(responseHeadersMap);
}

void ArkWebNWebWrapper::RegisterArkJSfunction(const std::string& object_name,
    const std::vector<std::string>& method_list, const std::vector<std::string>& async_method_list,
    const int32_t object_id)
{
    ArkWebString stName = ArkWebStringClassToStruct(object_name);
    ArkWebStringVector stMethods = ArkWebStringVectorClassToStruct(method_list);
    ArkWebStringVector stAsyncMethods = ArkWebStringVectorClassToStruct(async_method_list);

    ark_web_nweb_->RegisterArkJSfunction(stName, stMethods, stAsyncMethods, object_id);

    ArkWebStringStructRelease(stName);
    ArkWebStringVectorStructRelease(stMethods);
    ArkWebStringVectorStructRelease(stAsyncMethods);
}

void ArkWebNWebWrapper::SetFitContentMode(int32_t mode)
{
    ark_web_nweb_->SetFitContentMode(mode);
}

bool ArkWebNWebWrapper::TerminateRenderProcess()
{
    return ark_web_nweb_->TerminateRenderProcess();
}

std::string ArkWebNWebWrapper::GetSelectInfo()
{
    return ArkWebStringStructToClass(ark_web_nweb_->GetSelectInfo());
}

void ArkWebNWebWrapper::OnOnlineRenderToForeground()
{
    ark_web_nweb_->OnOnlineRenderToForeground();
}

int ArkWebNWebWrapper::ScaleGestureChange(double scale, double centerX, double centerY)
{
    return ark_web_nweb_->ScaleGestureChange(scale, centerX, centerY);
}

void ArkWebNWebWrapper::SuggestionSelected(int32_t index)
{
    ark_web_nweb_->SuggestionSelected(index);
}

void ArkWebNWebWrapper::OnSafeInsetsChange(int left, int top, int right, int bottom)
{
    ark_web_nweb_->OnSafeInsetsChange(left, top, right, bottom);
}

void ArkWebNWebWrapper::OnTextSelected()
{
    ark_web_nweb_->OnTextSelected();
}

void ArkWebNWebWrapper::NotifyForNextTouchEvent()
{
    ark_web_nweb_->NotifyForNextTouchEvent();
}

void ArkWebNWebWrapper::EnableAdsBlock(bool enable)
{
    ark_web_nweb_->EnableAdsBlock(enable);
}

bool ArkWebNWebWrapper::IsAdsBlockEnabled()
{
    return ark_web_nweb_->IsAdsBlockEnabled();
}

bool ArkWebNWebWrapper::IsAdsBlockEnabledForCurPage()
{
    return ark_web_nweb_->IsAdsBlockEnabledForCurPage();
}

bool ArkWebNWebWrapper::WebPageSnapshot(const char* id,
                                        ArkPixelUnit type,
                                        int width,
                                        int height,
                                        const WebSnapshotCallback callback)
{
    return ark_web_nweb_->WebPageSnapshot(id, static_cast<int>(type), width, height, callback);
}

void ArkWebNWebWrapper::OnConfigurationUpdated(std::shared_ptr<OHOS::NWeb::NWebSystemConfiguration> configuration)
{
    if (CHECK_SHARED_PTR_IS_NULL(configuration)) {
        ark_web_nweb_->OnConfigurationUpdated(nullptr);
        return;
    }

    ark_web_nweb_->OnConfigurationUpdated(new ArkWebSystemConfigurationImpl(configuration));
}

int ArkWebNWebWrapper::SetUrlTrustListWithErrMsg(
    const std::string& urlTrustList, std::string& detailErrMsg)
{
    ArkWebString stUrlTrustList = ArkWebStringClassToStruct(urlTrustList);
    ArkWebString stDetailErrMsg = ark_web_string_default;
    int res = ark_web_nweb_->SetUrlTrustListWithErrMsg(stUrlTrustList, stDetailErrMsg);
    detailErrMsg = ArkWebStringStructToClass(stDetailErrMsg);
    ArkWebStringStructRelease(stUrlTrustList);
    ArkWebStringStructRelease(stDetailErrMsg);
    return res;
}

void ArkWebNWebWrapper::PutSpanstringConvertHtmlCallback(
    std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->PutSpanstringConvertHtmlCallback(nullptr);
        return;
    }

    ark_web_nweb_->PutSpanstringConvertHtmlCallback(
        new ArkWebSpanstringConvertHtmlCallbackImpl(callback));
}

int ArkWebNWebWrapper::SetUrlTrustList(const std::string& urlTrustList)
{
    ArkWebString stUrlTrustList = ArkWebStringClassToStruct(urlTrustList);
    int res = ark_web_nweb_->SetUrlTrustList(stUrlTrustList);
    ArkWebStringStructRelease(stUrlTrustList);
    return res;
}

bool ArkWebNWebWrapper::WebSendKeyEvent(int32_t key_code, int32_t key_action, const std::vector<int32_t>& pressedCodes)
{
    ArkWebInt32Vector pCodes = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(pressedCodes);

    bool result = ark_web_nweb_->WebSendKeyEvent(key_code, key_action, pCodes);

    ArkWebBasicVectorStructRelease<ArkWebInt32Vector>(pCodes);
    return result;
}

void ArkWebNWebWrapper::SetPathAllowingUniversalAccess(
    const std::vector<std::string>& dirs,
    const std::vector<std::string>& moduleName,
    std::string& errorPath)
{
    ArkWebStringVector stDirs = ArkWebStringVectorClassToStruct(dirs);
    ArkWebStringVector stModuleName = ArkWebStringVectorClassToStruct(moduleName);
    ArkWebString stErrorPath = ark_web_string_default;

    ark_web_nweb_->SetPathAllowingUniversalAccess(stDirs, stModuleName, stErrorPath);

    errorPath = ArkWebStringStructToClass(stErrorPath);
    ArkWebStringVectorStructRelease(stDirs);
    ArkWebStringVectorStructRelease(stModuleName);
    ArkWebStringStructRelease(stErrorPath);
}

void ArkWebNWebWrapper::WebSendMouseWheelEvent(double x,
                                               double y,
                                               double delta_x,
                                               double delta_y,
                                               const std::vector<int32_t>& pressedCodes)
{
    ArkWebInt32Vector pCodes = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(pressedCodes);

    ark_web_nweb_->WebSendMouseWheelEvent(x, y, delta_x, delta_y, pCodes);

    ArkWebBasicVectorStructRelease<ArkWebInt32Vector>(pCodes);
}

void ArkWebNWebWrapper::WebSendTouchpadFlingEvent(double x,
                                                  double y,
                                                  double vx,
                                                  double vy,
                                                  const std::vector<int32_t>& pressedCodes)
{
    ArkWebInt32Vector pCodes = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(pressedCodes);

    ark_web_nweb_->WebSendTouchpadFlingEvent(x, y, vx, vy, pCodes);

    ArkWebBasicVectorStructRelease<ArkWebInt32Vector>(pCodes);
}

void ArkWebNWebWrapper::ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard)
{
    ark_web_nweb_->ResizeVisibleViewport(width, height, isKeyboard);
}

void ArkWebNWebWrapper::SetBackForwardCacheOptions(int32_t size, int32_t timeToLive)
{
    ark_web_nweb_->SetBackForwardCacheOptions(size, timeToLive);
}

void ArkWebNWebWrapper::RegisterArkJSfunction(const std::string& object_name,
    const std::vector<std::string>& method_list, const std::vector<std::string>& async_method_list,
    const int32_t object_id,
    const std::string& permission)
{
    ArkWebString stName = ArkWebStringClassToStruct(object_name);
    ArkWebStringVector stMethods = ArkWebStringVectorClassToStruct(method_list);
    ArkWebStringVector stAsyncMethods = ArkWebStringVectorClassToStruct(async_method_list);
    ArkWebString stPermission = ArkWebStringClassToStruct(permission);

    ark_web_nweb_->RegisterArkJSfunction(stName, stMethods, stAsyncMethods, object_id, stPermission);

    ArkWebStringStructRelease(stName);
    ArkWebStringVectorStructRelease(stMethods);
    ArkWebStringVectorStructRelease(stAsyncMethods);
    ArkWebStringStructRelease(stPermission);
}

void ArkWebNWebWrapper::SetAutofillCallback(std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->SetAutofillCallback(nullptr);
    } else {
        ark_web_nweb_->SetAutofillCallback(new ArkWebMessageValueCallbackImpl(callback));
    }
}

void ArkWebNWebWrapper::FillAutofillData(std::shared_ptr<OHOS::NWeb::NWebMessage> data)
{
    ArkWebMessage ark_web_message;
    ark_web_message.nweb_message = data;
    ark_web_nweb_->FillAutofillData(ark_web_message);
}

void ArkWebNWebWrapper::OnAutofillCancel(const std::string& fillContent)
{
    ArkWebString stFillContent = ArkWebStringClassToStruct(fillContent);

    ark_web_nweb_->OnAutofillCancel(stFillContent);

    ArkWebStringStructRelease(stFillContent);
}

void ArkWebNWebWrapper::PerformAction(int64_t accessibility_id, uint32_t action,
    const std::map<std::string, std::string>& actionArguments)
{
    ArkWebStringMap stArguments = ArkWebStringMapClassToStruct(actionArguments);
    ark_web_nweb_->PerformAction(accessibility_id, action, stArguments);

    ArkWebStringMapStructRelease(stArguments);
}

void ArkWebNWebWrapper::SendAccessibilityHoverEvent(int32_t x, int32_t y)
{
    ark_web_nweb_->SendAccessibilityHoverEvent(x, y);
}

void ArkWebNWebWrapper::GetScrollOffset(float* offset_x, float* offset_y)
{
    ark_web_nweb_->GetScrollOffset(offset_x, offset_y);
}

void ArkWebNWebWrapper::ExecuteCreatePDFExt(std::shared_ptr<OHOS::NWeb::NWebPDFConfigArgs> pdfConfig,
    std::shared_ptr<OHOS::NWeb::NWebArrayBufferValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_nweb_->ExecuteCreatePDFExt(
            new ArkWebPDFConfigArgsImpl(CHECK_SHARED_PTR_IS_NULL(pdfConfig) ? nullptr : pdfConfig), nullptr);
    } else {
        ark_web_nweb_->ExecuteCreatePDFExt(
            new ArkWebPDFConfigArgsImpl(CHECK_SHARED_PTR_IS_NULL(pdfConfig) ? nullptr : pdfConfig),
            new ArkWebArrayBufferValueCallbackImpl(callback));
    }
}

bool ArkWebNWebWrapper::ScrollByWithResult(float delta_x, float delta_y)
{
    return ark_web_nweb_->ScrollByWithResult(delta_x, delta_y);
}

void ArkWebNWebWrapper::OnDestroyImageAnalyzerOverlay()
{
    ark_web_nweb_->OnDestroyImageAnalyzerOverlay();
}

void ArkWebNWebWrapper::ScrollToWithAnime(float x, float y, int32_t duration)
{
    ark_web_nweb_->ScrollToWithAnime(x, y, duration);
}

void ArkWebNWebWrapper::ScrollByWithAnime(float delta_x, float delta_y, int32_t duration)
{
    ark_web_nweb_->ScrollByWithAnime(delta_x, delta_y, duration);
}

void ArkWebNWebWrapper::SetPopupSurface(void* popupSurface)
{
    ark_web_nweb_->SetPopupSurface(popupSurface);
}

void ArkWebNWebWrapper::WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent)
{
    if (CHECK_SHARED_PTR_IS_NULL(mouseEvent)) {
        ark_web_nweb_->WebSendMouseEvent(nullptr);
        return;
    }

    ark_web_nweb_->WebSendMouseEvent(new ArkWebMouseEventImpl(mouseEvent));
}

bool ArkWebNWebWrapper::GetAccessibilityVisible(int64_t accessibility_id)
{
    return ark_web_nweb_->GetAccessibilityVisible(accessibility_id);
}

void ArkWebNWebWrapper::SetTransformHint(uint32_t rotation)
{
    return ark_web_nweb_->SetTransformHint(rotation);
}

void ArkWebNWebWrapper::WebComponentsBlur()
{
    ark_web_nweb_->WebComponentsBlur();
}

float ArkWebNWebWrapper::DumpGpuInfo()
{
    return ark_web_nweb_->DumpGpuInfo();
}

int ArkWebNWebWrapper::ScaleGestureChangeV2(int type,
                                            double scale,
                                            double originScale,
                                            double centerX,
                                            double centerY)
{
    return ark_web_nweb_->ScaleGestureChangeV2(type, scale, originScale, centerX, centerY);
}

bool ArkWebNWebWrapper::SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent)
{
    if (CHECK_SHARED_PTR_IS_NULL(keyboardEvent)) {
        ark_web_nweb_->SendKeyboardEvent(nullptr);
        return false;
    }

    return ark_web_nweb_->SendKeyboardEvent(new ArkWebKeyboardEventImpl(keyboardEvent));
}

bool ArkWebNWebWrapper::PerformActionV2(int64_t accessibility_id, uint32_t action,
    const std::map<std::string, std::string>& actionArguments)
{
    ArkWebStringMap stArguments = ArkWebStringMapClassToStruct(actionArguments);
    bool res = ark_web_nweb_->PerformActionV2(accessibility_id, action, stArguments);

    ArkWebStringMapStructRelease(stArguments);
    return res;
}

bool ArkWebNWebWrapper::IsActivePolicyDisable()
{
    return ark_web_nweb_->IsActivePolicyDisable();
}

void ArkWebNWebWrapper::PutOptimizeParserBudgetEnabled(bool enable)
{
    ark_web_nweb_->PutOptimizeParserBudgetEnabled(enable);
}

bool ArkWebNWebWrapper::GetAccessibilityNodeRectById(
    int64_t accessibilityId, int32_t* width, int32_t* height, int32_t* offsetX, int32_t* offsetY)
{
    return ark_web_nweb_->GetAccessibilityNodeRectById(accessibilityId, width, height, offsetX, offsetY);
}

std::shared_ptr<OHOS::NWeb::HitTestResult> ArkWebNWebWrapper::GetLastHitTestResult()
{
    ArkWebRefPtr<ArkWebHitTestResult> ark_web_last_hit_test_result = ark_web_nweb_->GetLastHitTestResult();
    if (CHECK_REF_PTR_IS_NULL(ark_web_last_hit_test_result)) {
        return nullptr;
    }

    return std::make_shared<ArkWebHitTestResultWrapper>(ark_web_last_hit_test_result);
}

std::string ArkWebNWebWrapper::GetCurrentLanguage()
{
    return ArkWebStringStructToClass(ark_web_nweb_->GetCurrentLanguage());
}
bool ArkWebNWebWrapper::WebSendMouseWheelEventV2(
        double x, double y, double delta_x, double delta_y, const std::vector<int32_t> &pressedCodes, int32_t source)
{
    ArkWebInt32Vector pCodes = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(pressedCodes);

    bool result = ark_web_nweb_->WebSendMouseWheelEventV2(x, y, delta_x, delta_y, pCodes, source);

    ArkWebBasicVectorStructRelease<ArkWebInt32Vector>(pCodes);
    return result;
}

bool ArkWebNWebWrapper::IsNWebEx()
{
    return ark_web_nweb_->IsNWebEx();
}

void ArkWebNWebWrapper::SetEnableHalfFrameRate(bool enabled)
{
    ark_web_nweb_->SetEnableHalfFrameRate(enabled);
}

void ArkWebNWebWrapper::MaximizeResize()
{
    ark_web_nweb_->MaximizeResize();
}

void ArkWebNWebWrapper::OnDragAttach()
{
    ark_web_nweb_->OnDragAttach();
}

bool ArkWebNWebWrapper::SetFocusByPosition(float x, float y)
{
    return ark_web_nweb_->SetFocusByPosition(x, y);
}

void ArkWebNWebWrapper::SetSurfaceDensity(const double& density)
{
    ark_web_nweb_->SetSurfaceDensity(density);
}

int ArkWebNWebWrapper::GetSelectStartIndex()
{
    return ark_web_nweb_->GetSelectStartIndex();
}

int ArkWebNWebWrapper::GetSelectEndIndex()
{
    return ark_web_nweb_->GetSelectEndIndex();
}

std::string ArkWebNWebWrapper::GetAllTextInfo()
{
    return ArkWebStringStructToClass(ark_web_nweb_->GetAllTextInfo());
}
} // namespace OHOS::ArkWeb
