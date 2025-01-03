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

#include "ohos_nweb/bridge/ark_web_handler_wrapper.h"

#include "ohos_nweb/bridge/ark_web_access_request_impl.h"
#include "ohos_nweb/bridge/ark_web_applink_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_console_log_impl.h"
#include "ohos_nweb/bridge/ark_web_context_menu_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_context_menu_params_impl.h"
#include "ohos_nweb/bridge/ark_web_controller_handler_impl.h"
#include "ohos_nweb/bridge/ark_web_core_struct_utils.h"
#include "ohos_nweb/bridge/ark_web_cursor_info_impl.h"
#include "ohos_nweb/bridge/ark_web_custom_keyboard_handler_impl.h"
#include "ohos_nweb/bridge/ark_web_data_resubmission_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_date_time_chooser_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_date_time_chooser_impl.h"
#include "ohos_nweb/bridge/ark_web_drag_data_impl.h"
#include "ohos_nweb/bridge/ark_web_file_selector_params_impl.h"
#include "ohos_nweb/bridge/ark_web_first_meaningful_paint_details_impl.h"
#include "ohos_nweb/bridge/ark_web_full_screen_exit_handler_impl.h"
#include "ohos_nweb/bridge/ark_web_geo_location_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_image_options_impl.h"
#include "ohos_nweb/bridge/ark_web_js_dialog_result_impl.h"
#include "ohos_nweb/bridge/ark_web_js_http_auth_result_impl.h"
#include "ohos_nweb/bridge/ark_web_js_ssl_error_result_impl.h"
#include "ohos_nweb/bridge/ark_web_js_ssl_select_cert_result_impl.h"
#include "ohos_nweb/bridge/ark_web_key_event_impl.h"
#include "ohos_nweb/bridge/ark_web_largest_contentful_paint_details_impl.h"
#include "ohos_nweb/bridge/ark_web_load_committed_details_impl.h"
#include "ohos_nweb/bridge/ark_web_native_embed_data_info_impl.h"
#include "ohos_nweb/bridge/ark_web_native_embed_touch_event_impl.h"
#include "ohos_nweb/bridge/ark_web_nweb_impl.h"
#include "ohos_nweb/bridge/ark_web_quick_menu_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_quick_menu_params_impl.h"
#include "ohos_nweb/bridge/ark_web_screen_capture_access_request_impl.h"
#include "ohos_nweb/bridge/ark_web_select_popup_menu_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_select_popup_menu_param_impl.h"
#include "ohos_nweb/bridge/ark_web_string_vector_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_touch_handle_hot_zone_impl.h"
#include "ohos_nweb/bridge/ark_web_touch_handle_state_impl.h"
#include "ohos_nweb/bridge/ark_web_url_resource_error_impl.h"
#include "ohos_nweb/bridge/ark_web_url_resource_request_impl.h"
#include "ohos_nweb/bridge/ark_web_url_resource_response_impl.h"
#include "ohos_nweb/cpptoc/ark_web_date_time_suggestion_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebHandlerWrapper::ArkWebHandlerWrapper(ArkWebRefPtr<ArkWebHandler> ark_web_handler)
    : ark_web_handler_(ark_web_handler)
{}

void ArkWebHandlerWrapper::SetNWeb(std::shared_ptr<OHOS::NWeb::NWeb> nweb)
{
    if (CHECK_SHARED_PTR_IS_NULL(nweb)) {
        ark_web_handler_->SetNWeb(nullptr);
        return;
    }

    ark_web_handler_->SetNWeb(new ArkWebNWebImpl(nweb));
}

bool ArkWebHandlerWrapper::OnFocus()
{
    return ark_web_handler_->OnFocus();
}

void ArkWebHandlerWrapper::OnMessage(const std::string& param)
{
    ArkWebString stParam = ArkWebStringClassToStruct(param);

    ark_web_handler_->OnMessage(stParam);

    ArkWebStringStructRelease(stParam);
}

void ArkWebHandlerWrapper::OnResource(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_handler_->OnResource(stUrl);

    ArkWebStringStructRelease(stUrl);
}

void ArkWebHandlerWrapper::OnPageIcon(
    const void* data, size_t width, size_t height, ArkWebImageColorType color_type, ArkWebImageAlphaType alpha_type)
{
    ark_web_handler_->OnPageIcon(data, width, height, static_cast<int>(color_type), static_cast<int>(alpha_type));
}

void ArkWebHandlerWrapper::OnPageTitle(const std::string& title)
{
    ArkWebString stTitle = ArkWebStringClassToStruct(title);

    ark_web_handler_->OnPageTitle(stTitle);

    ArkWebStringStructRelease(stTitle);
}

void ArkWebHandlerWrapper::OnProxyDied()
{
    ark_web_handler_->OnProxyDied();
}

void ArkWebHandlerWrapper::OnHttpError(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response)
{
    ArkWebRefPtr<ArkWebUrlResourceRequest> ark_web_request = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(request)) {
        ark_web_request = new ArkWebUrlResourceRequestImpl(request);
    }

    ArkWebRefPtr<ArkWebUrlResourceResponse> ark_web_response = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(response)) {
        ark_web_response = new ArkWebUrlResourceResponseImpl(response);
    }

    ark_web_handler_->OnHttpError(ark_web_request, ark_web_response);
}

bool ArkWebHandlerWrapper::OnConsoleLog(std::shared_ptr<OHOS::NWeb::NWebConsoleLog> console_log)
{
    if (CHECK_SHARED_PTR_IS_NULL(console_log)) {
        return ark_web_handler_->OnConsoleLog(nullptr);
    }

    return ark_web_handler_->OnConsoleLog(new ArkWebConsoleLogImpl(console_log));
}

void ArkWebHandlerWrapper::OnRouterPush(const std::string& param)
{
    ArkWebString stParam = ArkWebStringClassToStruct(param);

    ark_web_handler_->OnRouterPush(stParam);

    ArkWebStringStructRelease(stParam);
}

void ArkWebHandlerWrapper::OnPageLoadEnd(int http_status_code, const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_handler_->OnPageLoadEnd(http_status_code, stUrl);

    ArkWebStringStructRelease(stUrl);
}

void ArkWebHandlerWrapper::OnPageLoadBegin(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_handler_->OnPageLoadBegin(stUrl);

    ArkWebStringStructRelease(stUrl);
}

void ArkWebHandlerWrapper::OnPageLoadError(int error_code, const std::string& description, const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stDescription = ArkWebStringClassToStruct(description);

    ark_web_handler_->OnPageLoadError(error_code, stDescription, stUrl);

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stDescription);
}

void ArkWebHandlerWrapper::OnDesktopIconUrl(const std::string& icon_url, bool precomposed)
{
    ArkWebString stIconUrl = ArkWebStringClassToStruct(icon_url);

    ark_web_handler_->OnDesktopIconUrl(stIconUrl, precomposed);

    ArkWebStringStructRelease(stIconUrl);
}

void ArkWebHandlerWrapper::OnLoadingProgress(int new_progress)
{
    ark_web_handler_->OnLoadingProgress(new_progress);
}

void ArkWebHandlerWrapper::OnGeolocationShow(
    const std::string& origin, std::shared_ptr<OHOS::NWeb::NWebGeolocationCallbackInterface> callback)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_handler_->OnGeolocationShow(stOrigin, nullptr);
    } else {
        ark_web_handler_->OnGeolocationShow(stOrigin, new ArkWebGeoLocationCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stOrigin);
}

void ArkWebHandlerWrapper::OnGeolocationHide()
{
    ark_web_handler_->OnGeolocationHide();
}

bool ArkWebHandlerWrapper::OnFileSelectorShow(std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> callback,
    std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> params)
{
    ArkWebRefPtr<ArkWebStringVectorValueCallback> ark_web_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_callback = new ArkWebStringVectorValueCallbackImpl(callback);
    }

    ArkWebRefPtr<ArkWebFileSelectorParams> ark_web_params = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(params)) {
        ark_web_params = new ArkWebFileSelectorParamsImpl(params);
    }

    return ark_web_handler_->OnFileSelectorShow(ark_web_callback, ark_web_params);
}

void ArkWebHandlerWrapper::OnResourceLoadError(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> error)
{
    ArkWebRefPtr<ArkWebUrlResourceRequest> ark_web_request = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(request)) {
        ark_web_request = new ArkWebUrlResourceRequestImpl(request);
    }

    ArkWebRefPtr<ArkWebUrlResourceError> ark_web_error = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(error)) {
        ark_web_error = new ArkWebUrlResourceErrorImpl(error);
    }

    ark_web_handler_->OnResourceLoadError(ark_web_request, ark_web_error);
}

void ArkWebHandlerWrapper::OnPermissionRequest(std::shared_ptr<OHOS::NWeb::NWebAccessRequest> request)
{
    if (CHECK_SHARED_PTR_IS_NULL(request)) {
        ark_web_handler_->OnPermissionRequest(nullptr);
        return;
    }

    ark_web_handler_->OnPermissionRequest(new ArkWebAccessRequestImpl(request));
}

void ArkWebHandlerWrapper::OnQuickMenuDismissed()
{
    ark_web_handler_->OnQuickMenuDismissed();
}

void ArkWebHandlerWrapper::OnContextMenuDismissed()
{
    ark_web_handler_->OnContextMenuDismissed();
}

void ArkWebHandlerWrapper::OnTouchSelectionChanged(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insert_handle,
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> start_selection_handle,
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> end_selection_handle)
{
    ArkWebRefPtr<ArkWebTouchHandleState> ark_web_insert_handle = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(insert_handle)) {
        ark_web_insert_handle = new ArkWebTouchHandleStateImpl(insert_handle);
    }

    ArkWebRefPtr<ArkWebTouchHandleState> ark_web_start_selection_handle = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(start_selection_handle)) {
        ark_web_start_selection_handle = new ArkWebTouchHandleStateImpl(start_selection_handle);
    }

    ArkWebRefPtr<ArkWebTouchHandleState> ark_web_end_selection_handle = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(end_selection_handle)) {
        ark_web_end_selection_handle = new ArkWebTouchHandleStateImpl(end_selection_handle);
    }

    ark_web_handler_->OnTouchSelectionChanged(
        ark_web_insert_handle, ark_web_start_selection_handle, ark_web_end_selection_handle);
}

bool ArkWebHandlerWrapper::OnHandleInterceptRequest(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response)
{
    ArkWebRefPtr<ArkWebUrlResourceRequest> ark_web_request = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(request)) {
        ark_web_request = new ArkWebUrlResourceRequestImpl(request);
    }

    ArkWebRefPtr<ArkWebUrlResourceResponse> ark_web_response = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(response)) {
        ark_web_response = new ArkWebUrlResourceResponseImpl(response);
    }

    return ark_web_handler_->OnHandleInterceptRequest(ark_web_request, ark_web_response);
}

void ArkWebHandlerWrapper::OnRefreshAccessedHistory(const std::string& url, bool is_reload)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_handler_->OnRefreshAccessedHistory(stUrl, is_reload);

    ArkWebStringStructRelease(stUrl);
}

void ArkWebHandlerWrapper::OnPermissionRequestCanceled(std::shared_ptr<OHOS::NWeb::NWebAccessRequest> request)
{
    if (CHECK_SHARED_PTR_IS_NULL(request)) {
        ark_web_handler_->OnPermissionRequestCanceled(nullptr);
        return;
    }

    ark_web_handler_->OnPermissionRequestCanceled(new ArkWebAccessRequestImpl(request));
}

bool ArkWebHandlerWrapper::OnHandleInterceptUrlLoading(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request)
{
    if (CHECK_SHARED_PTR_IS_NULL(request)) {
        return ark_web_handler_->OnHandleInterceptUrlLoading(nullptr);
    }

    return ark_web_handler_->OnHandleInterceptUrlLoading(new ArkWebUrlResourceRequestImpl(request));
}

bool ArkWebHandlerWrapper::RunQuickMenu(std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
    std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> callback)
{
    ArkWebRefPtr<ArkWebQuickMenuParams> ark_web_params = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(params)) {
        ark_web_params = new ArkWebQuickMenuParamsImpl(params);
    }

    ArkWebRefPtr<ArkWebQuickMenuCallback> ark_web_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_callback = new ArkWebQuickMenuCallbackImpl(callback);
    }

    return ark_web_handler_->RunQuickMenu(ark_web_params, ark_web_callback);
}

bool ArkWebHandlerWrapper::RunContextMenu(std::shared_ptr<OHOS::NWeb::NWebContextMenuParams> params,
    std::shared_ptr<OHOS::NWeb::NWebContextMenuCallback> callback)
{
    ArkWebRefPtr<ArkWebContextMenuParams> ark_web_params = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(params)) {
        ark_web_params = new ArkWebContextMenuParamsImpl(params);
    }

    ArkWebRefPtr<ArkWebContextMenuCallback> ark_web_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_callback = new ArkWebContextMenuCallbackImpl(callback);
    }

    return ark_web_handler_->RunContextMenu(ark_web_params, ark_web_callback);
}

void ArkWebHandlerWrapper::UpdateDragCursor(ArkWebDragOperation op)
{
    ark_web_handler_->UpdateDragCursor(static_cast<unsigned char>(op));
}

bool ArkWebHandlerWrapper::FilterScrollEvent(
    const float x, const float y, const float x_velocity, const float y_velocity)
{
    return ark_web_handler_->FilterScrollEvent(x, y, x_velocity, y_velocity);
}

std::vector<std::string> ArkWebHandlerWrapper::VisitedUrlHistory()
{
    ArkWebStringVector stUrlHistory = ark_web_handler_->VisitedUrlHistory();

    std::vector<std::string> objUrlHistory = ArkWebStringVectorStructToClass(stUrlHistory);
    ArkWebStringVectorStructRelease(stUrlHistory);
    return objUrlHistory;
}

void ArkWebHandlerWrapper::OnWindowNewByJS(const std::string& target_url, bool is_alert, bool is_user_trigger,
    std::shared_ptr<OHOS::NWeb::NWebControllerHandler> handler)
{
    ArkWebString stTargetUrl = ArkWebStringClassToStruct(target_url);

    if (CHECK_SHARED_PTR_IS_NULL(handler)) {
        ark_web_handler_->OnWindowNewByJS(stTargetUrl, is_alert, is_user_trigger, nullptr);
    } else {
        ark_web_handler_->OnWindowNewByJS(
            stTargetUrl, is_alert, is_user_trigger, new ArkWebControllerHandlerImpl(handler));
    }

    ArkWebStringStructRelease(stTargetUrl);
}

void ArkWebHandlerWrapper::OnWindowExitByJS()
{
    ark_web_handler_->OnWindowExitByJS();
}

bool ArkWebHandlerWrapper::OnAlertDialogByJS(
    const std::string& url, const std::string& message, std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stMessage = ArkWebStringClassToStruct(message);

    bool flag = false;
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        flag = ark_web_handler_->OnAlertDialogByJS(stUrl, stMessage, nullptr);
    } else {
        flag = ark_web_handler_->OnAlertDialogByJS(stUrl, stMessage, new ArkWebJsDialogResultImpl(result));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stMessage);
    return flag;
}

bool ArkWebHandlerWrapper::OnBeforeUnloadByJS(
    const std::string& url, const std::string& message, std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stMessage = ArkWebStringClassToStruct(message);

    bool flag = false;
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        flag = ark_web_handler_->OnBeforeUnloadByJS(stUrl, stMessage, nullptr);
    } else {
        flag = ark_web_handler_->OnBeforeUnloadByJS(stUrl, stMessage, new ArkWebJsDialogResultImpl(result));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stMessage);
    return flag;
}

bool ArkWebHandlerWrapper::OnPromptDialogByJS(const std::string& url, const std::string& message,
    const std::string& default_value, std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stMessage = ArkWebStringClassToStruct(message);
    ArkWebString stDefaultValue = ArkWebStringClassToStruct(default_value);

    bool flag = false;
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        flag = ark_web_handler_->OnPromptDialogByJS(stUrl, stMessage, stDefaultValue, nullptr);
    } else {
        flag = ark_web_handler_->OnPromptDialogByJS(
            stUrl, stMessage, stDefaultValue, new ArkWebJsDialogResultImpl(result));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stMessage);
    ArkWebStringStructRelease(stDefaultValue);
    return flag;
}

bool ArkWebHandlerWrapper::OnConfirmDialogByJS(
    const std::string& url, const std::string& message, std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stMessage = ArkWebStringClassToStruct(message);

    bool flag = false;
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        flag = ark_web_handler_->OnConfirmDialogByJS(stUrl, stMessage, nullptr);
    } else {
        flag = ark_web_handler_->OnConfirmDialogByJS(stUrl, stMessage, new ArkWebJsDialogResultImpl(result));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stMessage);
    return flag;
}

bool ArkWebHandlerWrapper::OnHttpAuthRequestByJS(
    std::shared_ptr<OHOS::NWeb::NWebJSHttpAuthResult> result, const std::string& host, const std::string& realm)
{
    ArkWebString stHost = ArkWebStringClassToStruct(host);
    ArkWebString stRealm = ArkWebStringClassToStruct(realm);

    bool flag = false;
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        flag = ark_web_handler_->OnHttpAuthRequestByJS(nullptr, stHost, stRealm);
    } else {
        flag = ark_web_handler_->OnHttpAuthRequestByJS(new ArkWebJsHttpAuthResultImpl(result), stHost, stRealm);
    }

    ArkWebStringStructRelease(stHost);
    ArkWebStringStructRelease(stRealm);
    return flag;
}

bool ArkWebHandlerWrapper::OnSslErrorRequestByJS(
    std::shared_ptr<OHOS::NWeb::NWebJSSslErrorResult> result, ArkWebSslError error)
{
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        return ark_web_handler_->OnSslErrorRequestByJS(nullptr, static_cast<int>(error));
    }

    return ark_web_handler_->OnSslErrorRequestByJS(new ArkWebJsSslErrorResultImpl(result), static_cast<int>(error));
}

bool ArkWebHandlerWrapper::OnSslSelectCertRequestByJS(std::shared_ptr<OHOS::NWeb::NWebJSSslSelectCertResult> result,
    const std::string& host, int port, const std::vector<std::string>& key_types,
    const std::vector<std::string>& issuers)
{
    ArkWebString stHost = ArkWebStringClassToStruct(host);
    ArkWebStringVector stIssuers = ArkWebStringVectorClassToStruct(issuers);
    ArkWebStringVector stKeyTypes = ArkWebStringVectorClassToStruct(key_types);

    bool flag = false;
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        flag = ark_web_handler_->OnSslSelectCertRequestByJS(nullptr, stHost, port, stKeyTypes, stIssuers);
    } else {
        flag = ark_web_handler_->OnSslSelectCertRequestByJS(
            new ArkWebJsSslSelectCertResultImpl(result), stHost, port, stKeyTypes, stIssuers);
    }

    ArkWebStringStructRelease(stHost);
    ArkWebStringVectorStructRelease(stIssuers);
    ArkWebStringVectorStructRelease(stKeyTypes);
    return flag;
}

void ArkWebHandlerWrapper::OnScroll(double x_offset, double y_offset)
{
    ark_web_handler_->OnScroll(x_offset, y_offset);
}

void ArkWebHandlerWrapper::OnOverScroll(float x_offset, float y_offset)
{
    ark_web_handler_->OnOverScroll(x_offset, y_offset);
}

void ArkWebHandlerWrapper::OnScrollState(bool scroll_state)
{
    ark_web_handler_->OnScrollState(scroll_state);
}

void ArkWebHandlerWrapper::OnPageVisible(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_handler_->OnPageVisible(stUrl);

    ArkWebStringStructRelease(stUrl);
}

bool ArkWebHandlerWrapper::OnPreKeyEvent(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event)
{
    if (CHECK_SHARED_PTR_IS_NULL(event)) {
        return ark_web_handler_->OnPreKeyEvent(nullptr);
    }

    return ark_web_handler_->OnPreKeyEvent(new ArkWebKeyEventImpl(event));
}

void ArkWebHandlerWrapper::OnScaleChanged(float old_scale_factor, float new_scale_factor)
{
    ark_web_handler_->OnScaleChanged(old_scale_factor, new_scale_factor);
}

bool ArkWebHandlerWrapper::OnCursorChange(
    const ArkWebCursorType& type, std::shared_ptr<OHOS::NWeb::NWebCursorInfo> info)
{
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return ark_web_handler_->OnCursorChange(static_cast<int32_t>(type), nullptr);
    }

    return ark_web_handler_->OnCursorChange(static_cast<int32_t>(type), new ArkWebCursorInfoImpl(info));
}

void ArkWebHandlerWrapper::OnRenderExited(ArkWebRenderExitReason reason)
{
    ark_web_handler_->OnRenderExited(static_cast<int>(reason));
}

void ArkWebHandlerWrapper::OnResizeNotWork()
{
    ark_web_handler_->OnResizeNotWork();
}

void ArkWebHandlerWrapper::OnFullScreenExit()
{
    ark_web_handler_->OnFullScreenExit();
}

void ArkWebHandlerWrapper::OnFullScreenEnter(std::shared_ptr<OHOS::NWeb::NWebFullScreenExitHandler> handler)
{
    if (CHECK_SHARED_PTR_IS_NULL(handler)) {
        ark_web_handler_->OnFullScreenEnter(nullptr);
        return;
    }

    ark_web_handler_->OnFullScreenEnter(new ArkWebFullScreenExitHandlerImpl(handler));
}

bool ArkWebHandlerWrapper::OnDragAndDropData(
    const void* data, size_t len, std::shared_ptr<OHOS::NWeb::NWebImageOptions> opt)
{
    if (CHECK_SHARED_PTR_IS_NULL(opt)) {
        return ark_web_handler_->OnDragAndDropData(data, len, nullptr);
    }

    return ark_web_handler_->OnDragAndDropData(data, len, new ArkWebImageOptionsImpl(opt));
}

void ArkWebHandlerWrapper::OnSelectPopupMenu(std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuParam> params,
    std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuCallback> callback)
{
    ArkWebRefPtr<ArkWebSelectPopupMenuParam> ark_web_params = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(params)) {
        ark_web_params = new ArkWebSelectPopupMenuParamImpl(params);
    }

    ArkWebRefPtr<ArkWebSelectPopupMenuCallback> ark_web_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_callback = new ArkWebSelectPopupMenuCallbackImpl(callback);
    }

    ark_web_handler_->OnSelectPopupMenu(ark_web_params, ark_web_callback);
}

void ArkWebHandlerWrapper::OnDataResubmission(std::shared_ptr<OHOS::NWeb::NWebDataResubmissionCallback> handler)
{
    if (CHECK_SHARED_PTR_IS_NULL(handler)) {
        ark_web_handler_->OnDataResubmission(nullptr);
        return;
    }

    ark_web_handler_->OnDataResubmission(new ArkWebDataResubmissionCallbackImpl(handler));
}

void ArkWebHandlerWrapper::OnRootLayerChanged(int width, int height)
{
    ark_web_handler_->OnRootLayerChanged(width, height);
}

void ArkWebHandlerWrapper::OnAudioStateChanged(bool playing)
{
    ark_web_handler_->OnAudioStateChanged(playing);
}

void ArkWebHandlerWrapper::OnOverScrollFlingEnd()
{
    ark_web_handler_->OnOverScrollFlingEnd();
}

bool ArkWebHandlerWrapper::OnUnProcessedKeyEvent(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event)
{
    if (CHECK_SHARED_PTR_IS_NULL(event)) {
        return ark_web_handler_->OnUnProcessedKeyEvent(nullptr);
    }

    return ark_web_handler_->OnUnProcessedKeyEvent(new ArkWebKeyEventImpl(event));
}

bool ArkWebHandlerWrapper::OnDragAndDropDataUdmf(std::shared_ptr<OHOS::NWeb::NWebDragData> drag_data)
{
    if (CHECK_SHARED_PTR_IS_NULL(drag_data)) {
        return ark_web_handler_->OnDragAndDropDataUdmf(nullptr);
    }

    return ark_web_handler_->OnDragAndDropDataUdmf(new ArkWebDragDataImpl(drag_data));
}

void ArkWebHandlerWrapper::OnFirstContentfulPaint(int64_t navigation_start_tick, int64_t first_contentful_paint_ms)
{
    ark_web_handler_->OnFirstContentfulPaint(navigation_start_tick, first_contentful_paint_ms);
}

void ArkWebHandlerWrapper::OnDateTimeChooserPopup(std::shared_ptr<OHOS::NWeb::NWebDateTimeChooser> chooser,
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebDateTimeSuggestion>>& suggestions,
    std::shared_ptr<OHOS::NWeb::NWebDateTimeChooserCallback> callback)
{
    ArkWebDateTimeSuggestionVector st_date_time_suggestion_vector =
        ArkWebDateTimeSuggestionVectorClassToStruct(suggestions);

    ArkWebRefPtr<ArkWebDateTimeChooser> ark_web_date_time_chooser = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(chooser)) {
        ark_web_date_time_chooser = new ArkWebDateTimeChooserImpl(chooser);
    }

    ArkWebRefPtr<ArkWebDateTimeChooserCallback> ark_web_date_time_chooser_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_date_time_chooser_callback = new ArkWebDateTimeChooserCallbackImpl(callback);
    }

    ark_web_handler_->OnDateTimeChooserPopup(
        ark_web_date_time_chooser, st_date_time_suggestion_vector, ark_web_date_time_chooser_callback);

    ArkWebDateTimeSuggestionVectorStructRelease(st_date_time_suggestion_vector);
}

void ArkWebHandlerWrapper::OnDateTimeChooserClose()
{
    ark_web_handler_->OnDateTimeChooserClose();
}

void ArkWebHandlerWrapper::OnScreenCaptureRequest(std::shared_ptr<OHOS::NWeb::NWebScreenCaptureAccessRequest> request)
{
    if (CHECK_SHARED_PTR_IS_NULL(request)) {
        ark_web_handler_->OnScreenCaptureRequest(nullptr);
        return;
    }

    ark_web_handler_->OnScreenCaptureRequest(new ArkWebScreenCaptureAccessRequestImpl(request));
}

void ArkWebHandlerWrapper::OnActivityStateChanged(int state, ArkWebActivityType type)
{
    ark_web_handler_->OnActivityStateChanged(state, static_cast<int>(type));
}

void ArkWebHandlerWrapper::OnGetTouchHandleHotZone(std::shared_ptr<OHOS::NWeb::NWebTouchHandleHotZone> hot_zone)
{
    if (CHECK_SHARED_PTR_IS_NULL(hot_zone)) {
        ark_web_handler_->OnGetTouchHandleHotZone(nullptr);
        return;
    }

    ark_web_handler_->OnGetTouchHandleHotZone(new ArkWebTouchHandleHotZoneImpl(hot_zone));
}

void ArkWebHandlerWrapper::OnCompleteSwapWithNewSize()
{
    ark_web_handler_->OnCompleteSwapWithNewSize();
}

void ArkWebHandlerWrapper::OnOverScrollFlingVelocity(float x_velocity, float y_velocity, bool is_fling)
{
    ark_web_handler_->OnOverScrollFlingVelocity(x_velocity, y_velocity, is_fling);
}

void ArkWebHandlerWrapper::OnNavigationEntryCommitted(std::shared_ptr<OHOS::NWeb::NWebLoadCommittedDetails> details)
{
    if (CHECK_SHARED_PTR_IS_NULL(details)) {
        ark_web_handler_->OnNavigationEntryCommitted(nullptr);
        return;
    }

    ark_web_handler_->OnNavigationEntryCommitted(new ArkWebLoadCommittedDetailsImpl(details));
}

void ArkWebHandlerWrapper::OnNativeEmbedLifecycleChange(std::shared_ptr<OHOS::NWeb::NWebNativeEmbedDataInfo> dataInfo)
{
    if (CHECK_SHARED_PTR_IS_NULL(dataInfo)) {
        ark_web_handler_->OnNativeEmbedLifecycleChange(nullptr);
        return;
    }

    ark_web_handler_->OnNativeEmbedLifecycleChange(new ArkWebNativeEmbedDataInfoImpl(dataInfo));
}

void ArkWebHandlerWrapper::OnNativeEmbedGestureEvent(std::shared_ptr<OHOS::NWeb::NWebNativeEmbedTouchEvent> touchEvent)
{
    if (CHECK_SHARED_PTR_IS_NULL(touchEvent)) {
        ark_web_handler_->OnNativeEmbedGestureEvent(nullptr);
        return;
    }

    ark_web_handler_->OnNativeEmbedGestureEvent(new ArkWebNativeEmbedTouchEventImpl(touchEvent));
}

void ArkWebHandlerWrapper::OnSafeBrowsingCheckResult(int threat_type)
{
    ark_web_handler_->OnSafeBrowsingCheckResult(threat_type);
}

void ArkWebHandlerWrapper::OnFullScreenEnterWithVideoSize(
    std::shared_ptr<OHOS::NWeb::NWebFullScreenExitHandler> handler, int video_natural_width, int video_natural_height)
{
    if (CHECK_SHARED_PTR_IS_NULL(handler)) {
        ark_web_handler_->OnFullScreenEnterWithVideoSize(nullptr, video_natural_width, video_natural_height);
        return;
    }

    ark_web_handler_->OnFullScreenEnterWithVideoSize(
        new ArkWebFullScreenExitHandlerImpl(handler), video_natural_width, video_natural_height);
}

bool ArkWebHandlerWrapper::OnHandleOverrideUrlLoading(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request)
{
    if (CHECK_SHARED_PTR_IS_NULL(request)) {
        return ark_web_handler_->OnHandleOverrideUrlLoading(nullptr);
    }

    return ark_web_handler_->OnHandleOverrideUrlLoading(new ArkWebUrlResourceRequestImpl(request));
}

void ArkWebHandlerWrapper::OnIntelligentTrackingPreventionResult(
    const std::string& website_host, const std::string& tracker_host)
{
    ArkWebString stWebsiteHost = ArkWebStringClassToStruct(website_host);
    ArkWebString stTrackerHost = ArkWebStringClassToStruct(tracker_host);

    ark_web_handler_->OnIntelligentTrackingPreventionResult(stWebsiteHost, stTrackerHost);

    ArkWebStringStructRelease(stWebsiteHost);
    ArkWebStringStructRelease(stTrackerHost);
}

void ArkWebHandlerWrapper::OnFirstMeaningfulPaint(std::shared_ptr<OHOS::NWeb::NWebFirstMeaningfulPaintDetails> details)
{
    if (CHECK_SHARED_PTR_IS_NULL(details)) {
        ark_web_handler_->OnFirstMeaningfulPaint(nullptr);
        return;
    }

    ark_web_handler_->OnFirstMeaningfulPaint(new ArkWebFirstMeaningfulPaintDetailsImpl(details));
}

void ArkWebHandlerWrapper::OnLargestContentfulPaint(
    std::shared_ptr<OHOS::NWeb::NWebLargestContentfulPaintDetails> details)
{
    if (CHECK_SHARED_PTR_IS_NULL(details)) {
        ark_web_handler_->OnLargestContentfulPaint(nullptr);
        return;
    }

    ark_web_handler_->OnLargestContentfulPaint(new ArkWebLargestContentfulPaintDetailsImpl(details));
}

bool ArkWebHandlerWrapper::OnAllSslErrorRequestByJS(std::shared_ptr<OHOS::NWeb::NWebJSAllSslErrorResult> result,
    ArkWebSslError error, const std::string& url, const std::string& originalUrl, const std::string& referrer,
    bool isFatalError, bool isMainFrame)
{
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        return ark_web_handler_->OnAllSslErrorRequestByJS(nullptr, static_cast<int>(error),
            ArkWebStringClassToStruct(url), ArkWebStringClassToStruct(originalUrl), ArkWebStringClassToStruct(referrer),
            isFatalError, isMainFrame);
    }

    return ark_web_handler_->OnAllSslErrorRequestByJS(new ArkWebJsAllSslErrorResultImpl(result),
        static_cast<int>(error), ArkWebStringClassToStruct(url), ArkWebStringClassToStruct(originalUrl),
        ArkWebStringClassToStruct(referrer), isFatalError, isMainFrame);
}

void ArkWebHandlerWrapper::OnTooltip(const std::string& tooltip)
{
    ArkWebString stTooltip = ArkWebStringClassToStruct(tooltip);

    ark_web_handler_->OnTooltip(stTooltip);

    ArkWebStringStructRelease(stTooltip);
}

void ArkWebHandlerWrapper::ReleaseResizeHold()
{
    ark_web_handler_->ReleaseResizeHold();
}

std::vector<int8_t> ArkWebHandlerWrapper::GetWordSelection(const std::string& text, int8_t offset)
{
    ArkWebString ark_text = ArkWebStringClassToStruct(text);
    ArkWebCharVector ark_result = ark_web_handler_->GetWordSelection(ark_text, offset);
    std::vector<int8_t> result = ArkWebBasicVectorStructToClass<int8_t, ArkWebCharVector>(ark_result);

    ArkWebBasicVectorStructRelease(ark_result);
    ArkWebStringStructRelease(ark_text);
    return result;
}

void ArkWebHandlerWrapper::UpdateClippedSelectionBounds(int x, int y, int w, int h)
{
    ark_web_handler_->UpdateClippedSelectionBounds(x, y, w, h);
}

void ArkWebHandlerWrapper::OnRenderProcessNotResponding(
    const std::string& js_stack, int pid, ArkWebRenderProcessNotRespondingReason reason)
{
    ArkWebString js_stack_str = ArkWebStringClassToStruct(js_stack);
    ark_web_handler_->OnRenderProcessNotResponding(js_stack_str, pid, static_cast<int>(reason));
}

void ArkWebHandlerWrapper::OnRenderProcessResponding()
{
    ark_web_handler_->OnRenderProcessResponding();
}

bool ArkWebHandlerWrapper::OnOpenAppLink(
    const std::string& url, std::shared_ptr<OHOS::NWeb::NWebAppLinkCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return false;
    }

    return ark_web_handler_->OnOpenAppLink(ArkWebStringClassToStruct(url), new ArkWebAppLinkCallbackImpl(callback));
}

void ArkWebHandlerWrapper::OnShowAutofillPopup(
    const float offsetX, const float offsetY, const std::vector<std::string>& menu_items)
{
    ArkWebStringVector stMenuItems = ArkWebStringVectorClassToStruct(menu_items);
    ark_web_handler_->OnShowAutofillPopup(offsetX, offsetY, stMenuItems);
}

void ArkWebHandlerWrapper::OnHideAutofillPopup()
{
    ark_web_handler_->OnHideAutofillPopup();
}

bool ArkWebHandlerWrapper::OnFocus(ArkWebFocusSource source)
{
    return ark_web_handler_->OnFocus(static_cast<int>(source));
}

bool ArkWebHandlerWrapper::OnOverScroll(float xOffset, float yOffset, float xVelocity, float yVelocity)
{
    return ark_web_handler_->OnOverScroll(xOffset, yOffset, xVelocity, yVelocity);
}

void ArkWebHandlerWrapper::CreateOverlay(void* data, size_t len, int width, int height, int offset_x, int offset_y,
    int rect_width, int rect_height, int point_x, int point_y)
{
    ark_web_handler_->CreateOverlay(
        data, len, width, height, offset_x, offset_y, rect_width, rect_height, point_x, point_y);
}

void ArkWebHandlerWrapper::OnOverlayStateChanged(int offset_x, int offset_y, int rect_width, int rect_height)
{
    ark_web_handler_->OnOverlayStateChanged(offset_x, offset_y, rect_width, rect_height);
}

void ArkWebHandlerWrapper::OnViewportFitChange(ArkWebViewportFit viewportFit)
{
    ark_web_handler_->OnViewportFitChange(static_cast<int>(viewportFit));
}

void ArkWebHandlerWrapper::OnAdsBlocked(const std::string& url, const std::vector<std::string>& adsBlocked)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebStringVector stAdsBlocked = ArkWebStringVectorClassToStruct(adsBlocked);

    ark_web_handler_->OnAdsBlocked(stUrl, stAdsBlocked);

    ArkWebStringVectorStructRelease(stAdsBlocked);
    ArkWebStringStructRelease(stUrl);
}

void ArkWebHandlerWrapper::OnInterceptKeyboardAttach(
    const std::shared_ptr<OHOS::NWeb::NWebCustomKeyboardHandler> keyboardHandler,
    const std::map<std::string, std::string> &attributes, bool &useSystemKeyboard, int32_t &enterKeyType)
{
    if (CHECK_SHARED_PTR_IS_NULL(keyboardHandler)) {
        return;
    }

    ark_web_handler_->OnInterceptKeyboardAttach(new ArkWebCustomKeyboardHandlerImpl(keyboardHandler),
        ArkWebStringMapClassToStruct(attributes), useSystemKeyboard, enterKeyType);
}

void ArkWebHandlerWrapper::OnCustomKeyboardAttach()
{
    ark_web_handler_->OnCustomKeyboardAttach();
}

void ArkWebHandlerWrapper::OnCustomKeyboardClose()
{
    ark_web_handler_->OnCustomKeyboardClose();
}

void ArkWebHandlerWrapper::KeyboardReDispatch(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event, bool isUsed)
{
    if (CHECK_SHARED_PTR_IS_NULL(event)) {
        ark_web_handler_->KeyboardReDispatch(nullptr, isUsed);
        return;
    }

    ark_web_handler_->KeyboardReDispatch(new ArkWebKeyEventImpl(event), isUsed);
}

void ArkWebHandlerWrapper::HideHandleAndQuickMenuIfNecessary(bool hide)
{
    ark_web_handler_->HideHandleAndQuickMenuIfNecessary(hide);
}

void ArkWebHandlerWrapper::OnCursorUpdate(double x, double y, double width, double height)
{
    ark_web_handler_->OnCursorUpdate(x, y, width, height);
}


 void ArkWebHandlerWrapper::ChangeVisibilityOfQuickMenu()
 {
     ark_web_handler_->ChangeVisibilityOfQuickMenu();
 }
 
 void ArkWebHandlerWrapper::StartVibraFeedback(const std::string& vibratorType)
 {
     ArkWebString stVibratorType = ArkWebStringClassToStruct(vibratorType);

     ark_web_handler_->StartVibraFeedback(stVibratorType);

     ArkWebStringStructRelease(stVibratorType);
 }
 
 void ArkWebHandlerWrapper::OnNativeEmbedVisibilityChange(const std::string& embed_id, bool visibility)
 {
     ArkWebString stEmbedId = ArkWebStringClassToStruct(embed_id);

     ark_web_handler_->OnNativeEmbedVisibilityChange(stEmbedId, visibility);
 }

 bool ArkWebHandlerWrapper::CloseImageOverlaySelection()
 {
    return ark_web_handler_->CloseImageOverlaySelection();
 }

void ArkWebHandlerWrapper::GetVisibleRectToWeb(int& visibleX, int& visibleY, int& visibleWidth, int& visibleHeight)
{
    ark_web_handler_->GetVisibleRectToWeb(visibleX, visibleY, visibleWidth, visibleHeight);
}
} // namespace OHOS::ArkWeb
