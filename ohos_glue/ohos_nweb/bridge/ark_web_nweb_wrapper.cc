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
#include "base/bridge/ark_web_bridge_macros.h"
#include "ohos_nweb/bridge/ark_web_accessibility_event_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_accessibility_node_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_bool_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_download_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_drag_data_wrapper.h"
#include "ohos_nweb/bridge/ark_web_find_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_handler_impl.h"
#include "ohos_nweb/bridge/ark_web_history_list_wrapper.h"
#include "ohos_nweb/bridge/ark_web_hit_test_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_result_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_message_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_preference_wrapper.h"
#include "ohos_nweb/bridge/ark_web_release_surface_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_screen_lock_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_string_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_view_struct_utils.h"
#include "ohos_nweb/cpptoc/ark_web_js_proxy_callback_vector_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_touch_point_info_vector_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_value_vector_cpptoc.h"

namespace OHOS::ArkWeb {

ArkWebNWebWrapper::ArkWebNWebWrapper(ArkWebRefPtr<ArkWebNWeb> ark_web_nweb)
    : ark_web_nweb_(ark_web_nweb) {
}

void ArkWebNWebWrapper::Resize(uint32_t width, uint32_t height,
                               bool is_keyboard) {
  ark_web_nweb_->Resize(width, height, is_keyboard);
}

void ArkWebNWebWrapper::OnPause() {
  ark_web_nweb_->OnPause();
}

void ArkWebNWebWrapper::OnContinue() {
  ark_web_nweb_->OnContinue();
}

void ArkWebNWebWrapper::OnDestroy() {
  ark_web_nweb_->OnDestroy();
}

void ArkWebNWebWrapper::OnFocus(const ArkWebFocusReason &focus_reason) {
  ark_web_nweb_->OnFocus(static_cast<int32_t>(focus_reason));
}

void ArkWebNWebWrapper::OnBlur(const ArkWebBlurReason &blur_reason) {
  ark_web_nweb_->OnBlur(static_cast<int32_t>(blur_reason));
}

void ArkWebNWebWrapper::OnTouchPress(int32_t id, double x, double y,
                                     bool from_overlay) {
  ark_web_nweb_->OnTouchPress(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::OnTouchRelease(int32_t id, double x, double y,
                                       bool from_overlay) {
  ark_web_nweb_->OnTouchRelease(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::OnTouchMove(int32_t id, double x, double y,
                                    bool from_overlay) {
  ark_web_nweb_->OnTouchMove(id, x, y, from_overlay);
}

void ArkWebNWebWrapper::OnTouchMove(
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebTouchPointInfo>>
        &touch_point_infos,
    bool from_overlay) {
  ArkWebTouchPointInfoVector st_touch_point_infos =
      ArkWebTouchPointInfoVectorClassToStruct(touch_point_infos);

  ark_web_nweb_->OnTouchMove(st_touch_point_infos, from_overlay);

  ArkWebTouchPointInfoVectorStructRelease(st_touch_point_infos);
}

void ArkWebNWebWrapper::OnTouchCancel() {
  ark_web_nweb_->OnTouchCancel();
}

void ArkWebNWebWrapper::OnNavigateBack() {
  ark_web_nweb_->OnNavigateBack();
}

bool ArkWebNWebWrapper::SendKeyEvent(int32_t key_code, int32_t key_action) {
  return ark_web_nweb_->SendKeyEvent(key_code, key_action);
}

void ArkWebNWebWrapper::SendMouseWheelEvent(double x, double y, double delta_x,
                                            double delta_y) {
  ark_web_nweb_->SendMouseWheelEvent(x, y, delta_x, delta_y);
}

void ArkWebNWebWrapper::SendMouseEvent(int x, int y, int button, int action,
                                       int count) {
  ark_web_nweb_->SendMouseEvent(x, y, button, action, count);
}

int ArkWebNWebWrapper::Load(const std::string &url) {
  ArkWebString stUrl = ArkWebStringClassToStruct(url);

  int code = ark_web_nweb_->Load(stUrl);

  ArkWebStringStructRelease(stUrl);
  return code;
}

bool ArkWebNWebWrapper::IsNavigatebackwardAllowed() {
  return ark_web_nweb_->IsNavigatebackwardAllowed();
}

bool ArkWebNWebWrapper::IsNavigateForwardAllowed() {
  return ark_web_nweb_->IsNavigateForwardAllowed();
}

bool ArkWebNWebWrapper::CanNavigateBackOrForward(int num_steps) {
  return ark_web_nweb_->CanNavigateBackOrForward(num_steps);
}

void ArkWebNWebWrapper::NavigateBack() {
  ark_web_nweb_->NavigateBack();
}

void ArkWebNWebWrapper::NavigateForward() {
  ark_web_nweb_->NavigateForward();
}

void ArkWebNWebWrapper::NavigateBackOrForward(int step) {
  ark_web_nweb_->NavigateBackOrForward(step);
}

void ArkWebNWebWrapper::DeleteNavigateHistory() {
  ark_web_nweb_->DeleteNavigateHistory();
}

void ArkWebNWebWrapper::Reload() {
  ark_web_nweb_->Reload();
}

int ArkWebNWebWrapper::Zoom(float zoom_factor) {
  return ark_web_nweb_->Zoom(zoom_factor);
}

int ArkWebNWebWrapper::ZoomIn() {
  return ark_web_nweb_->ZoomIn();
}

int ArkWebNWebWrapper::ZoomOut() {
  return ark_web_nweb_->ZoomOut();
}

void ArkWebNWebWrapper::Stop() {
  ark_web_nweb_->Stop();
}

void ArkWebNWebWrapper::ExecuteJavaScript(const std::string &code) {
  ArkWebString stCode = ArkWebStringClassToStruct(code);

  ark_web_nweb_->ExecuteJavaScript(stCode);

  ArkWebStringStructRelease(stCode);
}

void ArkWebNWebWrapper::ExecuteJavaScript(
    const std::string &code,
    std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback,
    bool extention) {
  ArkWebString stCode = ArkWebStringClassToStruct(code);

  if (CHECK_SHARED_PTR_IS_NULL(callback)) {
    ark_web_nweb_->ExecuteJavaScript(stCode, nullptr, extention);
  } else {
    ark_web_nweb_->ExecuteJavaScript(
        stCode, new ArkWebMessageValueCallbackImpl(callback), extention);
  }

  ArkWebStringStructRelease(stCode);
}

std::shared_ptr<OHOS::NWeb::NWebPreference> ArkWebNWebWrapper::GetPreference() {
  ArkWebRefPtr<ArkWebPreference> ark_web_preference =
      ark_web_nweb_->GetPreference();
  if (CHECK_REF_PTR_IS_NULL(ark_web_preference)) {
    return nullptr;
  }

  return std::make_shared<ArkWebPreferenceWrapper>(ark_web_preference);
}

unsigned int ArkWebNWebWrapper::GetWebId() {
  return ark_web_nweb_->GetWebId();
}

std::shared_ptr<OHOS::NWeb::HitTestResult>
ArkWebNWebWrapper::GetHitTestResult() {
  ArkWebRefPtr<ArkWebHitTestResult> ark_web_hit_test_result =
      ark_web_nweb_->GetHitTestResult();
  if (CHECK_REF_PTR_IS_NULL(ark_web_hit_test_result)) {
    return nullptr;
  }

  return std::make_shared<ArkWebHitTestResultWrapper>(ark_web_hit_test_result);
}

void ArkWebNWebWrapper::PutBackgroundColor(int color) {
  ark_web_nweb_->PutBackgroundColor(color);
}

void ArkWebNWebWrapper::InitialScale(float scale) {
  ark_web_nweb_->InitialScale(scale);
}

void ArkWebNWebWrapper::PutDownloadCallback(
    std::shared_ptr<OHOS::NWeb::NWebDownloadCallback> download_listener) {
  if (CHECK_SHARED_PTR_IS_NULL(download_listener)) {
    ark_web_nweb_->PutDownloadCallback(nullptr);
    return;
  }

  ark_web_nweb_->PutDownloadCallback(
      new ArkWebDownloadCallbackImpl(download_listener));
}

void ArkWebNWebWrapper::PutAccessibilityEventCallback(
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
        accessibility_event_listener) {
  if (CHECK_SHARED_PTR_IS_NULL(accessibility_event_listener)) {
    ark_web_nweb_->PutAccessibilityEventCallback(nullptr);
    return;
  }

  ark_web_nweb_->PutAccessibilityEventCallback(
      new ArkWebAccessibilityEventCallbackImpl(accessibility_event_listener));
}

void ArkWebNWebWrapper::PutAccessibilityIdGenerator(
    const AccessibilityIdGenerateFunc accessibility_id_generator) {
  ark_web_nweb_->PutAccessibilityIdGenerator(accessibility_id_generator);
}

void ArkWebNWebWrapper::SetNWebHandler(
    std::shared_ptr<OHOS::NWeb::NWebHandler> handler) {
  if (CHECK_SHARED_PTR_IS_NULL(handler)) {
    ark_web_nweb_->SetNWebHandler(nullptr);
    return;
  }

  ark_web_nweb_->SetNWebHandler(new ArkWebHandlerImpl(handler));
}

std::string ArkWebNWebWrapper::Title() {
  return ArkWebStringStructToClass(ark_web_nweb_->Title());
}

int ArkWebNWebWrapper::PageLoadProgress() {
  return ark_web_nweb_->PageLoadProgress();
}

int ArkWebNWebWrapper::ContentHeight() {
  return ark_web_nweb_->ContentHeight();
}

float ArkWebNWebWrapper::Scale() {
  return ark_web_nweb_->Scale();
}

int ArkWebNWebWrapper::Load(
    const std::string &url,
    const std::map<std::string, std::string> &additional_http_headers) {
  ArkWebString stUrl = ArkWebStringClassToStruct(url);
  ArkWebStringMap stHeaders =
      ArkWebStringMapClassToStruct(additional_http_headers);

  int code = ark_web_nweb_->Load(stUrl, stHeaders);

  ArkWebStringStructRelease(stUrl);
  ArkWebStringMapStructRelease(stHeaders);
  return code;
}

int ArkWebNWebWrapper::LoadWithDataAndBaseUrl(const std::string &base_url,
                                              const std::string &data,
                                              const std::string &mime_type,
                                              const std::string &encoding,
                                              const std::string &history_url) {
  ArkWebString stData = ArkWebStringClassToStruct(data);
  ArkWebString stBaseUrl = ArkWebStringClassToStruct(base_url);
  ArkWebString stEncoding = ArkWebStringClassToStruct(encoding);
  ArkWebString stMimeType = ArkWebStringClassToStruct(mime_type);
  ArkWebString stHistoryUrl = ArkWebStringClassToStruct(history_url);

  int code = ark_web_nweb_->LoadWithDataAndBaseUrl(
      stBaseUrl, stData, stMimeType, stEncoding, stHistoryUrl);

  ArkWebStringStructRelease(stData);
  ArkWebStringStructRelease(stBaseUrl);
  ArkWebStringStructRelease(stEncoding);
  ArkWebStringStructRelease(stMimeType);
  ArkWebStringStructRelease(stHistoryUrl);
  return code;
}

int ArkWebNWebWrapper::LoadWithData(const std::string &data,
                                    const std::string &mime_type,
                                    const std::string &encoding) {
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
    const std::string &object_name, const std::vector<std::string> &method_list,
    const int32_t object_id) {
  ArkWebString stName = ArkWebStringClassToStruct(object_name);
  ArkWebStringVector stMethods = ArkWebStringVectorClassToStruct(method_list);

  ark_web_nweb_->RegisterArkJSfunction(stName, stMethods, object_id);

  ArkWebStringStructRelease(stName);
  ArkWebStringVectorStructRelease(stMethods);
}

void ArkWebNWebWrapper::UnregisterArkJSfunction(
    const std::string &object_name,
    const std::vector<std::string> &method_list) {
  ArkWebString stName = ArkWebStringClassToStruct(object_name);
  ArkWebStringVector stMethods = ArkWebStringVectorClassToStruct(method_list);

  ark_web_nweb_->UnregisterArkJSfunction(stName, stMethods);

  ArkWebStringStructRelease(stName);
  ArkWebStringVectorStructRelease(stMethods);
}

void ArkWebNWebWrapper::SetNWebJavaScriptResultCallBack(
    std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBack> callback) {
  if (CHECK_SHARED_PTR_IS_NULL(callback)) {
    ark_web_nweb_->SetNWebJavaScriptResultCallBack(nullptr);
    return;
  }

  ark_web_nweb_->SetNWebJavaScriptResultCallBack(
      new ArkWebJsResultCallbackImpl(callback));
}

void ArkWebNWebWrapper::PutFindCallback(
    std::shared_ptr<OHOS::NWeb::NWebFindCallback> find_listener) {
  if (CHECK_SHARED_PTR_IS_NULL(find_listener)) {
    ark_web_nweb_->PutFindCallback(nullptr);
    return;
  }

  ark_web_nweb_->PutFindCallback(new ArkWebFindCallbackImpl(find_listener));
}

void ArkWebNWebWrapper::FindAllAsync(const std::string &search_str) {
  ArkWebString stSearchStr = ArkWebStringClassToStruct(search_str);

  ark_web_nweb_->FindAllAsync(stSearchStr);

  ArkWebStringStructRelease(stSearchStr);
}

void ArkWebNWebWrapper::ClearMatches() {
  ark_web_nweb_->ClearMatches();
}

void ArkWebNWebWrapper::FindNext(const bool forward) {
  ark_web_nweb_->FindNext(forward);
}

void ArkWebNWebWrapper::StoreWebArchive(
    const std::string &base_name, bool auto_name,
    std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback) {
  ArkWebString stName = ArkWebStringClassToStruct(base_name);

  if (CHECK_SHARED_PTR_IS_NULL(callback)) {
    ark_web_nweb_->StoreWebArchive(stName, auto_name, nullptr);
  } else {
    ark_web_nweb_->StoreWebArchive(stName, auto_name,
                                   new ArkWebStringValueCallbackImpl(callback));
  }

  ArkWebStringStructRelease(stName);
}

std::vector<std::string> ArkWebNWebWrapper::CreateWebMessagePorts() {
  ArkWebStringVector stPorts = ark_web_nweb_->CreateWebMessagePorts();

  std::vector<std::string> objPorts = ArkWebStringVectorStructToClass(stPorts);

  ArkWebStringVectorStructRelease(stPorts);
  return objPorts;
}

void ArkWebNWebWrapper::PostWebMessage(const std::string &message,
                                       const std::vector<std::string> &ports,
                                       const std::string &target_uri) {
  ArkWebString stMessage = ArkWebStringClassToStruct(message);
  ArkWebString stTargetUrl = ArkWebStringClassToStruct(target_uri);
  ArkWebStringVector stPorts = ArkWebStringVectorClassToStruct(ports);

  ark_web_nweb_->PostWebMessage(stMessage, stPorts, stTargetUrl);

  ArkWebStringStructRelease(stMessage);
  ArkWebStringStructRelease(stTargetUrl);
  ArkWebStringVectorStructRelease(stPorts);
}

void ArkWebNWebWrapper::ClosePort(const std::string &port_handle) {
  ArkWebString stPort = ArkWebStringClassToStruct(port_handle);

  ark_web_nweb_->ClosePort(stPort);

  ArkWebStringStructRelease(stPort);
}

void ArkWebNWebWrapper::PostPortMessage(
    const std::string &port_handle,
    std::shared_ptr<OHOS::NWeb::NWebMessage> data) {
  ArkWebString stPort = ArkWebStringClassToStruct(port_handle);

  ArkWebMessage ark_web_message;
  ark_web_message.nweb_message = data;
  ark_web_nweb_->PostPortMessage(stPort, ark_web_message);

  ArkWebStringStructRelease(stPort);
}

void ArkWebNWebWrapper::SetPortMessageCallback(
    const std::string &port_handle,
    std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback) {
  ArkWebString stPort = ArkWebStringClassToStruct(port_handle);

  if (CHECK_SHARED_PTR_IS_NULL(callback)) {
    ark_web_nweb_->SetPortMessageCallback(stPort, nullptr);
  } else {
    ark_web_nweb_->SetPortMessageCallback(
        stPort, new ArkWebMessageValueCallbackImpl(callback));
  }

  ArkWebStringStructRelease(stPort);
}

void ArkWebNWebWrapper::SendDragEvent(const OHOS::NWeb::DragEvent &drag_event) {
  ArkWebDragEvent ark_web_drag_event = ArkWebDragEventClassToStruct(drag_event);
  ark_web_nweb_->SendDragEvent(ark_web_drag_event);
}

void ArkWebNWebWrapper::ClearSslCache() {
  ark_web_nweb_->ClearSslCache();
}

std::string ArkWebNWebWrapper::GetUrl() {
  ArkWebString stUrl = ark_web_nweb_->GetUrl();

  std::string objUrl = ArkWebStringStructToClass(stUrl);
  ArkWebStringStructRelease(stUrl);
  return objUrl;
}

void ArkWebNWebWrapper::ClearClientAuthenticationCache() {
  ark_web_nweb_->ClearClientAuthenticationCache();
}

void ArkWebNWebWrapper::UpdateLocale(const std::string &language,
                                     const std::string &region) {
  ArkWebString stRegion = ArkWebStringClassToStruct(region);
  ArkWebString stLanguage = ArkWebStringClassToStruct(language);

  ark_web_nweb_->UpdateLocale(stLanguage, stRegion);

  ArkWebStringStructRelease(stRegion);
  ArkWebStringStructRelease(stLanguage);
}

const std::string ArkWebNWebWrapper::GetOriginalUrl() {
  ArkWebString stUrl = ark_web_nweb_->GetOriginalUrl();

  std::string objUrl = ArkWebStringStructToClass(stUrl);
  ArkWebStringStructRelease(stUrl);
  return objUrl;
}

bool ArkWebNWebWrapper::GetFavicon(const void **data, size_t &width,
                                   size_t &height,
                                   ArkWebImageColorType &color_type,
                                   ArkWebImageAlphaType &alpha_type) {
  int enum_color_type =
      static_cast<int>(ArkWebImageColorType::COLOR_TYPE_UNKNOWN);
  int enum_alpha_type =
      static_cast<int>(ArkWebImageAlphaType::ALPHA_TYPE_UNKNOWN);
  bool result = ark_web_nweb_->GetFavicon(data, width, height, enum_color_type,
                                          enum_alpha_type);
  color_type = static_cast<ArkWebImageColorType>(enum_color_type);
  alpha_type = static_cast<ArkWebImageAlphaType>(enum_alpha_type);
  return result;
}

void ArkWebNWebWrapper::PutNetworkAvailable(bool available) {
  ark_web_nweb_->PutNetworkAvailable(available);
}

void ArkWebNWebWrapper::HasImages(
    std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) {
  if (CHECK_SHARED_PTR_IS_NULL(callback)) {
    ark_web_nweb_->HasImages(nullptr);
    return;
  }

  ark_web_nweb_->HasImages(new ArkWebBoolValueCallbackImpl(callback));
}

void ArkWebNWebWrapper::RemoveCache(bool include_disk_files) {
  ark_web_nweb_->RemoveCache(include_disk_files);
}

std::shared_ptr<OHOS::NWeb::NWebHistoryList>
ArkWebNWebWrapper::GetHistoryList() {
  ArkWebRefPtr<ArkWebHistoryList> ark_web_history_list =
      ark_web_nweb_->GetHistoryList();
  if (CHECK_REF_PTR_IS_NULL(ark_web_history_list)) {
    return nullptr;
  }

  return std::make_shared<ArkWebHistoryListWrapper>(ark_web_history_list);
}

void ArkWebNWebWrapper::PutReleaseSurfaceCallback(
    std::shared_ptr<OHOS::NWeb::NWebReleaseSurfaceCallback>
        release_surface_listener) {
  if (CHECK_SHARED_PTR_IS_NULL(release_surface_listener)) {
    ark_web_nweb_->PutReleaseSurfaceCallback(nullptr);
    return;
  }

  ark_web_nweb_->PutReleaseSurfaceCallback(
      new ArkWebReleaseSurfaceCallbackImpl(release_surface_listener));
}

std::vector<uint8_t> ArkWebNWebWrapper::SerializeWebState() {
  ArkWebUint8Vector stState = ark_web_nweb_->SerializeWebState();

  std::vector<uint8_t> objState =
      ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(stState);
  ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stState);
  return objState;
}

bool ArkWebNWebWrapper::RestoreWebState(const std::vector<uint8_t> &state) {
  ArkWebUint8Vector stState =
      ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(state);

  bool flag = ark_web_nweb_->RestoreWebState(stState);

  ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stState);
  return flag;
}

void ArkWebNWebWrapper::PageUp(bool top) {
  ark_web_nweb_->PageUp(top);
}

void ArkWebNWebWrapper::PageDown(bool bottom) {
  ark_web_nweb_->PageDown(bottom);
}

void ArkWebNWebWrapper::ScrollTo(float x, float y) {
  ark_web_nweb_->ScrollTo(x, y);
}

void ArkWebNWebWrapper::ScrollBy(float delta_x, float delta_y) {
  ark_web_nweb_->ScrollBy(delta_x, delta_y);
}

void ArkWebNWebWrapper::SlideScroll(float vx, float vy) {
  ark_web_nweb_->SlideScroll(vx, vy);
}

bool ArkWebNWebWrapper::GetCertChainDerData(
    std::vector<std::string> &cert_chain_data, bool is_single_cert) {
  ArkWebStringVector cert_chain_data_vector;
  bool result = ark_web_nweb_->GetCertChainDerData(cert_chain_data_vector,
                                                   is_single_cert);
  cert_chain_data = ArkWebStringVectorStructToClass(cert_chain_data_vector);

  ArkWebStringVectorStructRelease(cert_chain_data_vector);
  return result;
}

void ArkWebNWebWrapper::SetScreenOffSet(double x, double y) {
  ark_web_nweb_->SetScreenOffSet(x, y);
}

void ArkWebNWebWrapper::SetAudioMuted(bool muted) {
  ark_web_nweb_->SetAudioMuted(muted);
}

void ArkWebNWebWrapper::SetShouldFrameSubmissionBeforeDraw(bool should) {
  ark_web_nweb_->SetShouldFrameSubmissionBeforeDraw(should);
}

void ArkWebNWebWrapper::NotifyPopupWindowResult(bool result) {
  ark_web_nweb_->NotifyPopupWindowResult(result);
}

void ArkWebNWebWrapper::SetAudioResumeInterval(int32_t resume_interval) {
  ark_web_nweb_->SetAudioResumeInterval(resume_interval);
}

void ArkWebNWebWrapper::SetAudioExclusive(bool audio_exclusive) {
  ark_web_nweb_->SetAudioExclusive(audio_exclusive);
}

void ArkWebNWebWrapper::RegisterScreenLockFunction(
    int32_t window_id,
    std::shared_ptr<OHOS::NWeb::NWebScreenLockCallback> callback) {
  if (CHECK_SHARED_PTR_IS_NULL(callback)) {
    ark_web_nweb_->RegisterScreenLockFunction(window_id, nullptr);
    return;
  }

  ark_web_nweb_->RegisterScreenLockFunction(
      window_id, new ArkWebScreenLockCallbackImpl(callback));
}

void ArkWebNWebWrapper::UnRegisterScreenLockFunction(int32_t window_id) {
  ark_web_nweb_->UnRegisterScreenLockFunction(window_id);
}

void ArkWebNWebWrapper::NotifyMemoryLevel(int32_t level) {
  ark_web_nweb_->NotifyMemoryLevel(level);
}

void ArkWebNWebWrapper::OnWebviewHide() {
  ark_web_nweb_->OnWebviewHide();
}

void ArkWebNWebWrapper::OnWebviewShow() {
  ark_web_nweb_->OnWebviewShow();
}

std::shared_ptr<OHOS::NWeb::NWebDragData>
ArkWebNWebWrapper::GetOrCreateDragData() {
  ArkWebRefPtr<ArkWebDragData> ark_web_drag_data =
      ark_web_nweb_->GetOrCreateDragData();
  if (CHECK_REF_PTR_IS_NULL(ark_web_drag_data)) {
    return nullptr;
  }

  return std::make_shared<ArkWebDragDataWrapper>(ark_web_drag_data);
}

void ArkWebNWebWrapper::PrefetchPage(
    const std::string &url,
    const std::map<std::string, std::string> &additional_http_headers) {
  ArkWebString stUrl = ArkWebStringClassToStruct(url);
  ArkWebStringMap stHeaders =
      ArkWebStringMapClassToStruct(additional_http_headers);

  ark_web_nweb_->PrefetchPage(stUrl, stHeaders);

  ArkWebStringStructRelease(stUrl);
  ArkWebStringMapStructRelease(stHeaders);
}

void ArkWebNWebWrapper::SetWindowId(uint32_t window_id) {
  ark_web_nweb_->SetWindowId(window_id);
}

void ArkWebNWebWrapper::OnOccluded() {
  ark_web_nweb_->OnOccluded();
}

void ArkWebNWebWrapper::OnUnoccluded() {
  ark_web_nweb_->OnUnoccluded();
}

void ArkWebNWebWrapper::SetToken(void *token) {
  ark_web_nweb_->SetToken(token);
}

void ArkWebNWebWrapper::SetNestedScrollMode(
    const ArkWebNestedScrollMode &nested_scroll_mode) {
  ark_web_nweb_->SetNestedScrollMode(static_cast<int32_t>(nested_scroll_mode));
}

void ArkWebNWebWrapper::SetEnableLowerFrameRate(bool enabled) {
  ark_web_nweb_->SetEnableLowerFrameRate(enabled);
}

void ArkWebNWebWrapper::SetVirtualKeyBoardArg(int32_t width, int32_t height,
                                              double keyboard) {
  ark_web_nweb_->SetVirtualKeyBoardArg(width, height, keyboard);
}

bool ArkWebNWebWrapper::ShouldVirtualKeyboardOverlay() {
  return ark_web_nweb_->ShouldVirtualKeyboardOverlay();
}

void ArkWebNWebWrapper::SetDrawRect(int32_t x, int32_t y, int32_t width,
                                    int32_t height) {
  ark_web_nweb_->SetDrawRect(x, y, width, height);
}

void ArkWebNWebWrapper::SetDrawMode(int32_t mode) {
  ark_web_nweb_->SetDrawMode(mode);
}

void *
ArkWebNWebWrapper::CreateWebPrintDocumentAdapter(const std::string &job_name) {
  ArkWebString stObjName = ArkWebStringClassToStruct(job_name);

  void *adapter = ark_web_nweb_->CreateWebPrintDocumentAdapter(stObjName);

  ArkWebStringStructRelease(stObjName);
  return adapter;
}

int ArkWebNWebWrapper::PostUrl(const std::string &url,
                               const std::vector<char> &post_data) {
  ArkWebString stUrl = ArkWebStringClassToStruct(url);
  ArkWebCharVector stPostData =
      ArkWebBasicVectorClassToStruct<char, ArkWebCharVector>(post_data);

  int code = ark_web_nweb_->PostUrl(stUrl, stPostData);

  ArkWebStringStructRelease(stUrl);
  ArkWebBasicVectorStructRelease<ArkWebCharVector>(stPostData);
  return code;
}

void ArkWebNWebWrapper::JavaScriptOnDocumentStart(
    const std::map<std::string, std::vector<std::string>> &script_items) {
  ArkWebStringVectorMap stScriptItems =
      ArkWebStringVectorMapClassToStruct(script_items);

  ark_web_nweb_->JavaScriptOnDocumentStart(stScriptItems);

  ArkWebStringVectorMapStructRelease(stScriptItems);
}

void ArkWebNWebWrapper::ExecuteAction(int64_t accessibility_id,
                                      uint32_t action) {
  ark_web_nweb_->ExecuteAction(accessibility_id, action);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo>
ArkWebNWebWrapper::GetFocusedAccessibilityNodeInfo(
    int64_t accessibility_id, bool is_accessibility_focus) {
  ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info =
      ark_web_nweb_->GetFocusedAccessibilityNodeInfo(accessibility_id,
                                                     is_accessibility_focus);
  if (CHECK_REF_PTR_IS_NULL(ark_web_accessibility_node_info)) {
    return nullptr;
  }

  return std::make_shared<ArkWebAccessibilityNodeInfoWrapper>(
      ark_web_accessibility_node_info);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo>
ArkWebNWebWrapper::GetAccessibilityNodeInfoById(int64_t accessibility_id) {
  ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info =
      ark_web_nweb_->GetAccessibilityNodeInfoById(accessibility_id);
  if (CHECK_REF_PTR_IS_NULL(ark_web_accessibility_node_info)) {
    return nullptr;
  }

  return std::make_shared<ArkWebAccessibilityNodeInfoWrapper>(
      ark_web_accessibility_node_info);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo>
ArkWebNWebWrapper::GetAccessibilityNodeInfoByFocusMove(int64_t accessibility_id,
                                                       int32_t direction) {
  ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info =
      ark_web_nweb_->GetAccessibilityNodeInfoByFocusMove(accessibility_id,
                                                         direction);
  if (CHECK_REF_PTR_IS_NULL(ark_web_accessibility_node_info)) {
    return nullptr;
  }

  return std::make_shared<ArkWebAccessibilityNodeInfoWrapper>(
      ark_web_accessibility_node_info);
}

void ArkWebNWebWrapper::SetAccessibilityState(bool state) {
  ark_web_nweb_->SetAccessibilityState(state);
}

bool ArkWebNWebWrapper::NeedSoftKeyboard() {
  return ark_web_nweb_->NeedSoftKeyboard();
}

bool ArkWebNWebWrapper::Discard() {
  return ark_web_nweb_->Discard();
}

bool ArkWebNWebWrapper::Restore() {
  return ark_web_nweb_->Restore();
}

int ArkWebNWebWrapper::GetSecurityLevel() {
  return ark_web_nweb_->GetSecurityLevel();
}

void ArkWebNWebWrapper::CallH5Function(
    int32_t routingId, int32_t h5ObjectId, const std::string &h5MethodName,
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>> &args) {
  ArkWebString stName = ArkWebStringClassToStruct(h5MethodName);
  ArkWebValueVector stArgs = ArkWebValueVectorClassToStruct(args);

  ark_web_nweb_->CallH5Function(routingId, h5ObjectId, stName, stArgs);

  ArkWebStringStructRelease(stName);
  ArkWebValueVectorStructRelease(stArgs);
}

bool ArkWebNWebWrapper::IsIncognitoMode() {
  return ark_web_nweb_->IsIncognitoMode();
}

void ArkWebNWebWrapper::RegisterNativeArkJSFunction(
    const char *objName,
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback>>
        &callbacks) {
  ArkWebJsProxyCallbackVector stJsProxyCallbackVector =
      ArkWebJsProxyCallbackVectorClassToStruct(callbacks);

  ark_web_nweb_->RegisterNativeArkJSFunction(objName, stJsProxyCallbackVector);

  ArkWebJsProxyCallbackVectorStructRelease(stJsProxyCallbackVector);
}

void ArkWebNWebWrapper::UnRegisterNativeArkJSFunction(const char *objName) {
  ark_web_nweb_->UnRegisterNativeArkJSFunction(objName);
}

void ArkWebNWebWrapper::RegisterNativeValideCallback(
    const char *webName, const NativeArkWebOnValidCallback callback) {
  ark_web_nweb_->RegisterNativeValideCallback(webName, callback);
}

void ArkWebNWebWrapper::RegisterNativeDestroyCallback(
    const char *webName, const NativeArkWebOnValidCallback callback) {
  ark_web_nweb_->RegisterNativeDestroyCallback(webName, callback);
}

void ArkWebNWebWrapper::JavaScriptOnDocumentEnd(
    const std::map<std::string, std::vector<std::string>> &script_items) {
  ArkWebStringVectorMap stScriptItems =
      ArkWebStringVectorMapClassToStruct(script_items);

  ark_web_nweb_->JavaScriptOnDocumentEnd(stScriptItems);

  ArkWebStringVectorMapStructRelease(stScriptItems);
}

void ArkWebNWebWrapper::EnableSafeBrowsing(bool enable) {
  ark_web_nweb_->EnableSafeBrowsing(enable);
}

bool ArkWebNWebWrapper::IsSafeBrowsingEnabled() {
  return ark_web_nweb_->IsSafeBrowsingEnabled();
}

void ArkWebNWebWrapper::SetPrintBackground(bool enable) {
  ark_web_nweb_->SetPrintBackground(enable);
}

bool ArkWebNWebWrapper::GetPrintBackground() {
  return ark_web_nweb_->GetPrintBackground();
}

} // namespace OHOS::ArkWeb