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

#include "ohos_nweb/bridge/ark_web_handler_impl.h"

#include "ohos_nweb/bridge/ark_web_access_request_wrapper.h"
#include "ohos_nweb/bridge/ark_web_app_link_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_console_log_wrapper.h"
#include "ohos_nweb/bridge/ark_web_context_menu_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_context_menu_params_wrapper.h"
#include "ohos_nweb/bridge/ark_web_controller_handler_wrapper.h"
#include "ohos_nweb/bridge/ark_web_cursor_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_custom_keyboard_handler_wrapper.h"
#include "ohos_nweb/bridge/ark_web_data_resubmission_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_date_time_chooser_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_date_time_chooser_wrapper.h"
#include "ohos_nweb/bridge/ark_web_drag_data_wrapper.h"
#include "ohos_nweb/bridge/ark_web_file_selector_params_wrapper.h"
#include "ohos_nweb/bridge/ark_web_first_meaningful_paint_details_wrapper.h"
#include "ohos_nweb/bridge/ark_web_full_screen_exit_handler_wrapper.h"
#include "ohos_nweb/bridge/ark_web_geo_location_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_image_options_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_all_ssl_error_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_dialog_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_http_auth_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_ssl_error_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_js_ssl_select_cert_result_wrapper.h"
#include "ohos_nweb/bridge/ark_web_key_event_wrapper.h"
#include "ohos_nweb/bridge/ark_web_largest_contentful_paint_details_wrapper.h"
#include "ohos_nweb/bridge/ark_web_load_committed_details_wrapper.h"
#include "ohos_nweb/bridge/ark_web_native_embed_data_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_native_embed_touch_event_wrapper.h"
#include "ohos_nweb/bridge/ark_web_native_embed_mouse_event_wrapper.h"
#include "ohos_nweb/bridge/ark_web_native_embed_param_data_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_nweb_wrapper.h"
#include "ohos_nweb/bridge/ark_web_quick_menu_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_quick_menu_params_wrapper.h"
#include "ohos_nweb/bridge/ark_web_screen_capture_access_request_wrapper.h"
#include "ohos_nweb/bridge/ark_web_select_popup_menu_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_select_popup_menu_param_wrapper.h"
#include "ohos_nweb/bridge/ark_web_string_vector_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_touch_handle_hot_zone_wrapper.h"
#include "ohos_nweb/bridge/ark_web_touch_handle_state_wrapper.h"
#include "ohos_nweb/bridge/ark_web_url_resource_error_wrapper.h"
#include "ohos_nweb/bridge/ark_web_url_resource_request_wrapper.h"
#include "ohos_nweb/bridge/ark_web_url_resource_response_wrapper.h"
#include "ohos_nweb/bridge/ark_web_view_struct_utils.h"
#include "ohos_nweb/ctocpp/ark_web_date_time_suggestion_vector_ctocpp.h"
#include "ohos_nweb/bridge/ark_web_native_message_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_runtime_connect_info_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebSslError = OHOS::NWeb::SslError;
using ArkWebCursorType = OHOS::NWeb::CursorType;
using ArkWebActivityType = OHOS::NWeb::ActivityType;
using ArkWebRenderExitReason = OHOS::NWeb::RenderExitReason;
using ArkWebDragOperation = OHOS::NWeb::NWebDragData::DragOperation;
using ArkWebRenderProcessNotRespondingReason = OHOS::NWeb::RenderProcessNotRespondingReason;
using ArkWebViewportFit = OHOS::NWeb::ViewportFit;
using ArkWebFocusSource = OHOS::NWeb::NWebFocusSource;

ArkWebHandlerImpl::ArkWebHandlerImpl(std::shared_ptr<OHOS::NWeb::NWebHandler> nweb_handler)
    : nweb_handler_(nweb_handler)
{}

void ArkWebHandlerImpl::SetNWeb(ArkWebRefPtr<ArkWebNWeb> nweb)
{
    if (CHECK_REF_PTR_IS_NULL(nweb)) {
        nweb_handler_->SetNWeb(nullptr);
        return;
    }

    nweb_handler_->SetNWeb(std::make_shared<ArkWebNWebWrapper>(nweb));
}

bool ArkWebHandlerImpl::OnFocus()
{
    return nweb_handler_->OnFocus();
}

void ArkWebHandlerImpl::OnMessage(const ArkWebString& param)
{
    nweb_handler_->OnMessage(ArkWebStringStructToClass(param));
}

void ArkWebHandlerImpl::OnResource(const ArkWebString& url)
{
    nweb_handler_->OnResource(ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnPageIcon(const void* data, size_t width, size_t height, int color_type, int alpha_type)
{
    nweb_handler_->OnPageIcon(data, width, height, static_cast<ArkWebImageColorType>(color_type),
        static_cast<ArkWebImageAlphaType>(alpha_type));
}

void ArkWebHandlerImpl::OnPageTitle(const ArkWebString& title)
{
    nweb_handler_->OnPageTitle(ArkWebStringStructToClass(title));
}

void ArkWebHandlerImpl::OnProxyDied()
{
    nweb_handler_->OnProxyDied();
}

void ArkWebHandlerImpl::OnHttpError(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceResponse> response)
{
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> nweb_request = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(request)) {
        nweb_request = std::make_shared<ArkWebUrlResourceRequestWrapper>(request);
    }

    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> nweb_response = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(response)) {
        nweb_response = std::make_shared<ArkWebUrlResourceResponseWrapper>(response);
    }

    nweb_handler_->OnHttpError(nweb_request, nweb_response);
}

bool ArkWebHandlerImpl::OnConsoleLog(ArkWebRefPtr<ArkWebConsoleLog> console_log)
{
    if (CHECK_REF_PTR_IS_NULL(console_log)) {
        return nweb_handler_->OnConsoleLog(nullptr);
    }

    return nweb_handler_->OnConsoleLog(std::make_shared<ArkWebConsoleLogWrapper>(console_log));
}

void ArkWebHandlerImpl::OnRouterPush(const ArkWebString& param)
{
    nweb_handler_->OnRouterPush(ArkWebStringStructToClass(param));
}

void ArkWebHandlerImpl::OnPageLoadEnd(int http_status_code, const ArkWebString& url)
{
    nweb_handler_->OnPageLoadEnd(http_status_code, ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnPageLoadBegin(const ArkWebString& url)
{
    nweb_handler_->OnPageLoadBegin(ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnPageLoadError(int error_code, const ArkWebString& description, const ArkWebString& url)
{
    nweb_handler_->OnPageLoadError(error_code, ArkWebStringStructToClass(description), ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnDesktopIconUrl(const ArkWebString& icon_url, bool precomposed)
{
    nweb_handler_->OnDesktopIconUrl(ArkWebStringStructToClass(icon_url), precomposed);
}

void ArkWebHandlerImpl::OnLoadingProgress(int new_progress)
{
    nweb_handler_->OnLoadingProgress(new_progress);
}

void ArkWebHandlerImpl::OnGeolocationShow(const ArkWebString& origin, ArkWebRefPtr<ArkWebGeoLocationCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_handler_->OnGeolocationShow(ArkWebStringStructToClass(origin), nullptr);
        return;
    }

    nweb_handler_->OnGeolocationShow(
        ArkWebStringStructToClass(origin), std::make_shared<ArkWebGeoLocationCallbackWrapper>(callback));
}

void ArkWebHandlerImpl::OnGeolocationHide()
{
    nweb_handler_->OnGeolocationHide();
}

bool ArkWebHandlerImpl::OnFileSelectorShow(
    ArkWebRefPtr<ArkWebStringVectorValueCallback> callback, ArkWebRefPtr<ArkWebFileSelectorParams> params)
{
    std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> nweb_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_callback = std::make_shared<ArkWebStringVectorValueCallbackWrapper>(callback);
    }

    std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> nweb_params = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(params)) {
        nweb_params = std::make_shared<ArkWebFileSelectorParamsWrapper>(params);
    }

    return nweb_handler_->OnFileSelectorShow(nweb_callback, nweb_params);
}

void ArkWebHandlerImpl::OnResourceLoadError(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceError> error)
{
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> nweb_request = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(request)) {
        nweb_request = std::make_shared<ArkWebUrlResourceRequestWrapper>(request);
    }

    std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> nweb_error = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(error)) {
        nweb_error = std::make_shared<ArkWebUrlResourceErrorWrapper>(error);
    }

    nweb_handler_->OnResourceLoadError(nweb_request, nweb_error);
}

void ArkWebHandlerImpl::OnPermissionRequest(ArkWebRefPtr<ArkWebAccessRequest> request)
{
    if (CHECK_REF_PTR_IS_NULL(request)) {
        nweb_handler_->OnPermissionRequest(nullptr);
        return;
    }

    nweb_handler_->OnPermissionRequest(std::make_shared<ArkWebAccessRequestWrapper>(request));
}

void ArkWebHandlerImpl::OnQuickMenuDismissed()
{
    nweb_handler_->OnQuickMenuDismissed();
}

void ArkWebHandlerImpl::OnContextMenuDismissed()
{
    nweb_handler_->OnContextMenuDismissed();
}

void ArkWebHandlerImpl::OnTouchSelectionChanged(ArkWebRefPtr<ArkWebTouchHandleState> insert_handle,
    ArkWebRefPtr<ArkWebTouchHandleState> start_selection_handle,
    ArkWebRefPtr<ArkWebTouchHandleState> end_selection_handle)
{
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_insert_handle = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(insert_handle)) {
        nweb_insert_handle = std::make_shared<ArkWebTouchHandleStateWrapper>(insert_handle);
    }

    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_start_selection_handle = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(start_selection_handle)) {
        nweb_start_selection_handle = std::make_shared<ArkWebTouchHandleStateWrapper>(start_selection_handle);
    }

    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_end_selection_handle = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(end_selection_handle)) {
        nweb_end_selection_handle = std::make_shared<ArkWebTouchHandleStateWrapper>(end_selection_handle);
    }

    nweb_handler_->OnTouchSelectionChanged(nweb_insert_handle, nweb_start_selection_handle, nweb_end_selection_handle);
}

bool ArkWebHandlerImpl::OnHandleInterceptRequest(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceResponse> response)
{
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> nweb_request = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(request)) {
        nweb_request = std::make_shared<ArkWebUrlResourceRequestWrapper>(request);
    }

    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> nweb_reponse = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(response)) {
        nweb_reponse = std::make_shared<ArkWebUrlResourceResponseWrapper>(response);
    }

    return nweb_handler_->OnHandleInterceptRequest(nweb_request, nweb_reponse);
}

void ArkWebHandlerImpl::OnRefreshAccessedHistory(const ArkWebString& url, bool is_reload)
{
    nweb_handler_->OnRefreshAccessedHistory(ArkWebStringStructToClass(url), is_reload);
}

void ArkWebHandlerImpl::OnPermissionRequestCanceled(ArkWebRefPtr<ArkWebAccessRequest> request)
{
    if (CHECK_REF_PTR_IS_NULL(request)) {
        nweb_handler_->OnPermissionRequestCanceled(nullptr);
        return;
    }

    nweb_handler_->OnPermissionRequestCanceled(std::make_shared<ArkWebAccessRequestWrapper>(request));
}

bool ArkWebHandlerImpl::OnHandleInterceptUrlLoading(ArkWebRefPtr<ArkWebUrlResourceRequest> request)
{
    if (CHECK_REF_PTR_IS_NULL(request)) {
        return nweb_handler_->OnHandleInterceptUrlLoading(nullptr);
    }

    return nweb_handler_->OnHandleInterceptUrlLoading(std::make_shared<ArkWebUrlResourceRequestWrapper>(request));
}

bool ArkWebHandlerImpl::RunQuickMenu(
    ArkWebRefPtr<ArkWebQuickMenuParams> params, ArkWebRefPtr<ArkWebQuickMenuCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> nweb_params = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(params)) {
        nweb_params = std::make_shared<ArkWebQuickMenuParamsWrapper>(params);
    }

    std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> nweb_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_callback = std::make_shared<ArkWebQuickMenuCallbackWrapper>(callback);
    }

    return nweb_handler_->RunQuickMenu(nweb_params, nweb_callback);
}

bool ArkWebHandlerImpl::RunContextMenu(
    ArkWebRefPtr<ArkWebContextMenuParams> params, ArkWebRefPtr<ArkWebContextMenuCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebContextMenuParams> nweb_params = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(params)) {
        nweb_params = std::make_shared<ArkWebContextMenuParamsWrapper>(params);
    }

    std::shared_ptr<OHOS::NWeb::NWebContextMenuCallback> nweb_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_callback = std::make_shared<ArkWebContextMenuCallbackWrapper>(callback);
    }

    return nweb_handler_->RunContextMenu(nweb_params, nweb_callback);
}

void ArkWebHandlerImpl::UpdateDragCursor(unsigned char op)
{
    nweb_handler_->UpdateDragCursor(static_cast<ArkWebDragOperation>(op));
}

bool ArkWebHandlerImpl::FilterScrollEvent(const float x, const float y, const float x_velocity, const float y_velocity)
{
    return nweb_handler_->FilterScrollEvent(x, y, x_velocity, y_velocity);
}

ArkWebStringVector ArkWebHandlerImpl::VisitedUrlHistory()
{
    return ArkWebStringVectorClassToStruct(nweb_handler_->VisitedUrlHistory());
}

void ArkWebHandlerImpl::OnWindowNewByJS(
    const ArkWebString& target_url, bool is_alert, bool is_user_trigger, ArkWebRefPtr<ArkWebControllerHandler> handler)
{
    if (CHECK_REF_PTR_IS_NULL(handler)) {
        nweb_handler_->OnWindowNewByJS(ArkWebStringStructToClass(target_url), is_alert, is_user_trigger, nullptr);
        return;
    }

    nweb_handler_->OnWindowNewByJS(ArkWebStringStructToClass(target_url), is_alert, is_user_trigger,
        std::make_shared<ArkWebControllerHandlerWrapper>(handler));
}

void ArkWebHandlerImpl::OnWindowExitByJS()
{
    nweb_handler_->OnWindowExitByJS();
}

bool ArkWebHandlerImpl::OnAlertDialogByJS(
    const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnAlertDialogByJS(
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(message), nullptr);
    }

    return nweb_handler_->OnAlertDialogByJS(ArkWebStringStructToClass(url), ArkWebStringStructToClass(message),
        std::make_shared<ArkWebJsDialogResultWrapper>(result));
}

bool ArkWebHandlerImpl::OnBeforeUnloadByJS(
    const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnBeforeUnloadByJS(
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(message), nullptr);
    }

    return nweb_handler_->OnBeforeUnloadByJS(ArkWebStringStructToClass(url), ArkWebStringStructToClass(message),
        std::make_shared<ArkWebJsDialogResultWrapper>(result));
}

bool ArkWebHandlerImpl::OnPromptDialogByJS(const ArkWebString& url, const ArkWebString& message,
    const ArkWebString& default_value, ArkWebRefPtr<ArkWebJsDialogResult> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnPromptDialogByJS(ArkWebStringStructToClass(url), ArkWebStringStructToClass(message),
            ArkWebStringStructToClass(default_value), nullptr);
    }

    return nweb_handler_->OnPromptDialogByJS(ArkWebStringStructToClass(url), ArkWebStringStructToClass(message),
        ArkWebStringStructToClass(default_value), std::make_shared<ArkWebJsDialogResultWrapper>(result));
}

bool ArkWebHandlerImpl::OnConfirmDialogByJS(
    const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnConfirmDialogByJS(
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(message), nullptr);
    }

    return nweb_handler_->OnConfirmDialogByJS(ArkWebStringStructToClass(url), ArkWebStringStructToClass(message),
        std::make_shared<ArkWebJsDialogResultWrapper>(result));
}

bool ArkWebHandlerImpl::OnHttpAuthRequestByJS(
    ArkWebRefPtr<ArkWebJsHttpAuthResult> result, const ArkWebString& host, const ArkWebString& realm)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnHttpAuthRequestByJS(
            nullptr, ArkWebStringStructToClass(host), ArkWebStringStructToClass(realm));
    }

    return nweb_handler_->OnHttpAuthRequestByJS(std::make_shared<ArkWebJsHttpAuthResultWrapper>(result),
        ArkWebStringStructToClass(host), ArkWebStringStructToClass(realm));
}

bool ArkWebHandlerImpl::OnSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnSslErrorRequestByJS(nullptr, static_cast<ArkWebSslError>(error));
    }

    return nweb_handler_->OnSslErrorRequestByJS(
        std::make_shared<ArkWebJsSslErrorResultWrapper>(result), static_cast<ArkWebSslError>(error));
}

bool ArkWebHandlerImpl::OnSslSelectCertRequestByJS(ArkWebRefPtr<ArkWebJsSslSelectCertResult> result,
    const ArkWebString& host, int port, const ArkWebStringVector& key_types, const ArkWebStringVector& issuers)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnSslSelectCertRequestByJS(nullptr, ArkWebStringStructToClass(host), port,
            ArkWebStringVectorStructToClass(key_types), ArkWebStringVectorStructToClass(issuers));
    }

    return nweb_handler_->OnSslSelectCertRequestByJS(std::make_shared<ArkWebJsSslSelectCertResultWrapper>(result),
        ArkWebStringStructToClass(host), port, ArkWebStringVectorStructToClass(key_types),
        ArkWebStringVectorStructToClass(issuers));
}

void ArkWebHandlerImpl::OnScroll(double x_offset, double y_offset)
{
    nweb_handler_->OnScroll(x_offset, y_offset);
}

void ArkWebHandlerImpl::OnOverScroll(float x_offset, float y_offset)
{
    nweb_handler_->OnOverScroll(x_offset, y_offset);
}

void ArkWebHandlerImpl::OnScrollState(bool scroll_state)
{
    nweb_handler_->OnScrollState(scroll_state);
}

void ArkWebHandlerImpl::OnPageVisible(const ArkWebString& url)
{
    nweb_handler_->OnPageVisible(ArkWebStringStructToClass(url));
}

bool ArkWebHandlerImpl::OnPreKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event)
{
    if (CHECK_REF_PTR_IS_NULL(event)) {
        return nweb_handler_->OnPreKeyEvent(nullptr);
    }

    return nweb_handler_->OnPreKeyEvent(std::make_shared<ArkWebKeyEventWrapper>(event));
}

void ArkWebHandlerImpl::OnScaleChanged(float old_scale_factor, float new_scale_factor)
{
    nweb_handler_->OnScaleChanged(old_scale_factor, new_scale_factor);
}

bool ArkWebHandlerImpl::OnCursorChange(const int32_t& type, ArkWebRefPtr<ArkWebCursorInfo> info)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return nweb_handler_->OnCursorChange(static_cast<ArkWebCursorType>(type), nullptr);
    }

    return nweb_handler_->OnCursorChange(
        static_cast<ArkWebCursorType>(type), std::make_shared<ArkWebCursorInfoWrapper>(info));
}

void ArkWebHandlerImpl::OnRenderExited(int reason)
{
    nweb_handler_->OnRenderExited(static_cast<ArkWebRenderExitReason>(reason));
}

void ArkWebHandlerImpl::OnResizeNotWork()
{
    nweb_handler_->OnResizeNotWork();
}

void ArkWebHandlerImpl::OnFullScreenExit()
{
    nweb_handler_->OnFullScreenExit();
}

void ArkWebHandlerImpl::OnFullScreenEnter(ArkWebRefPtr<ArkWebFullScreenExitHandler> handler)
{
    if (CHECK_REF_PTR_IS_NULL(handler)) {
        nweb_handler_->OnFullScreenEnter(nullptr);
        return;
    }

    nweb_handler_->OnFullScreenEnter(std::make_shared<ArkWebFullScreenExitHandlerWrapper>(handler));
}

void ArkWebHandlerImpl::OnFullScreenEnterWithVideoSize(
    ArkWebRefPtr<ArkWebFullScreenExitHandler> handler, int video_natural_width, int video_natural_height)
{
    if (CHECK_REF_PTR_IS_NULL(handler)) {
        nweb_handler_->OnFullScreenEnterWithVideoSize(nullptr, video_natural_width, video_natural_height);
        return;
    }

    nweb_handler_->OnFullScreenEnterWithVideoSize(
        std::make_shared<ArkWebFullScreenExitHandlerWrapper>(handler), video_natural_width, video_natural_height);
}

bool ArkWebHandlerImpl::OnDragAndDropData(const void* data, size_t len, ArkWebRefPtr<ArkWebImageOptions> opt)
{
    if (CHECK_REF_PTR_IS_NULL(opt)) {
        return nweb_handler_->OnDragAndDropData(data, len, nullptr);
    }

    return nweb_handler_->OnDragAndDropData(data, len, std::make_shared<ArkWebImageOptionsWrapper>(opt));
}

void ArkWebHandlerImpl::OnSelectPopupMenu(
    ArkWebRefPtr<ArkWebSelectPopupMenuParam> params, ArkWebRefPtr<ArkWebSelectPopupMenuCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuParam> nweb_params = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(params)) {
        nweb_params = std::make_shared<ArkWebSelectPopupMenuParamWrapper>(params);
    }

    std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuCallback> nweb_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_callback = std::make_shared<ArkWebSelectPopupMenuCallbackWrapper>(callback);
    }

    nweb_handler_->OnSelectPopupMenu(nweb_params, nweb_callback);
}

void ArkWebHandlerImpl::OnDataResubmission(ArkWebRefPtr<ArkWebDataResubmissionCallback> handler)
{
    if (CHECK_REF_PTR_IS_NULL(handler)) {
        nweb_handler_->OnDataResubmission(nullptr);
        return;
    }

    nweb_handler_->OnDataResubmission(std::make_shared<ArkWebDataResubmissionCallbackWrapper>(handler));
}

void ArkWebHandlerImpl::OnRootLayerChanged(int width, int height)
{
    nweb_handler_->OnRootLayerChanged(width, height);
}

void ArkWebHandlerImpl::OnAudioStateChanged(bool playing)
{
    nweb_handler_->OnAudioStateChanged(playing);
}

void ArkWebHandlerImpl::OnOverScrollFlingEnd()
{
    nweb_handler_->OnOverScrollFlingEnd();
}

bool ArkWebHandlerImpl::OnUnProcessedKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event)
{
    if (CHECK_REF_PTR_IS_NULL(event)) {
        return nweb_handler_->OnUnProcessedKeyEvent(nullptr);
    }

    return nweb_handler_->OnUnProcessedKeyEvent(std::make_shared<ArkWebKeyEventWrapper>(event));
}

bool ArkWebHandlerImpl::OnDragAndDropDataUdmf(ArkWebRefPtr<ArkWebDragData> drag_data)
{
    if (CHECK_REF_PTR_IS_NULL(drag_data)) {
        return nweb_handler_->OnDragAndDropDataUdmf(nullptr);
    }

    return nweb_handler_->OnDragAndDropDataUdmf(std::make_shared<ArkWebDragDataWrapper>(drag_data));
}

void ArkWebHandlerImpl::OnFirstContentfulPaint(int64_t navigation_start_tick, int64_t first_contentful_paint_ms)
{
    nweb_handler_->OnFirstContentfulPaint(navigation_start_tick, first_contentful_paint_ms);
}

void ArkWebHandlerImpl::OnDateTimeChooserPopup(ArkWebRefPtr<ArkWebDateTimeChooser> chooser,
    const ArkWebDateTimeSuggestionVector& suggestions, ArkWebRefPtr<ArkWebDateTimeChooserCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebDateTimeChooser> nweb_date_time_chooser = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(chooser)) {
        nweb_date_time_chooser = std::make_shared<ArkWebDateTimeChooserWrapper>(chooser);
    }

    std::shared_ptr<OHOS::NWeb::NWebDateTimeChooserCallback> nweb_date_time_chooser_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_date_time_chooser_callback = std::make_shared<ArkWebDateTimeChooserCallbackWrapper>(callback);
    }

    nweb_handler_->OnDateTimeChooserPopup(nweb_date_time_chooser,
        ArkWebDateTimeSuggestionVectorStructToClass(suggestions), nweb_date_time_chooser_callback);
}

void ArkWebHandlerImpl::OnDateTimeChooserClose()
{
    nweb_handler_->OnDateTimeChooserClose();
}

void ArkWebHandlerImpl::OnScreenCaptureRequest(ArkWebRefPtr<ArkWebScreenCaptureAccessRequest> request)
{
    if (CHECK_REF_PTR_IS_NULL(request)) {
        nweb_handler_->OnScreenCaptureRequest(nullptr);
        return;
    }

    nweb_handler_->OnScreenCaptureRequest(std::make_shared<ArkWebScreenCaptureAccessRequestWrapper>(request));
}

void ArkWebHandlerImpl::OnActivityStateChanged(int state, int type)
{
    nweb_handler_->OnActivityStateChanged(state, static_cast<ArkWebActivityType>(type));
}

void ArkWebHandlerImpl::OnGetTouchHandleHotZone(ArkWebRefPtr<ArkWebTouchHandleHotZone> hot_zone)
{
    if (CHECK_REF_PTR_IS_NULL(hot_zone)) {
        nweb_handler_->OnGetTouchHandleHotZone(nullptr);
        return;
    }

    nweb_handler_->OnGetTouchHandleHotZone(std::make_shared<ArkWebTouchHandleHotZoneWrapper>(hot_zone));
}

void ArkWebHandlerImpl::OnCompleteSwapWithNewSize()
{
    nweb_handler_->OnCompleteSwapWithNewSize();
}

void ArkWebHandlerImpl::OnOverScrollFlingVelocity(float x_velocity, float y_velocity, bool is_fling)
{
    nweb_handler_->OnOverScrollFlingVelocity(x_velocity, y_velocity, is_fling);
}

void ArkWebHandlerImpl::OnNavigationEntryCommitted(ArkWebRefPtr<ArkWebLoadCommittedDetails> details)
{
    if (CHECK_REF_PTR_IS_NULL(details)) {
        nweb_handler_->OnScreenCaptureRequest(nullptr);
        return;
    }

    nweb_handler_->OnNavigationEntryCommitted(std::make_shared<ArkWebLoadCommittedDetailsWrapper>(details));
}

void ArkWebHandlerImpl::OnNativeEmbedLifecycleChange(ArkWebRefPtr<ArkWebNativeEmbedDataInfo> data_info)
{
    if (CHECK_REF_PTR_IS_NULL(data_info)) {
        nweb_handler_->OnNativeEmbedLifecycleChange(nullptr);
        return;
    }

    nweb_handler_->OnNativeEmbedLifecycleChange(std::make_shared<ArkWebNativeEmbedDataInfoWrapper>(data_info));
}

void ArkWebHandlerImpl::OnNativeEmbedGestureEvent(ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> touch_event)
{
    if (CHECK_REF_PTR_IS_NULL(touch_event)) {
        nweb_handler_->OnNativeEmbedGestureEvent(nullptr);
        return;
    }

    nweb_handler_->OnNativeEmbedGestureEvent(std::make_shared<ArkWebNativeEmbedTouchEventWrapper>(touch_event));
}

void ArkWebHandlerImpl::OnSafeBrowsingCheckResult(int threat_type)
{
    nweb_handler_->OnSafeBrowsingCheckResult(threat_type);
}

void ArkWebHandlerImpl::OnFirstMeaningfulPaint(ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> details)
{
    if (CHECK_REF_PTR_IS_NULL(details)) {
        return;
    }

    nweb_handler_->OnFirstMeaningfulPaint(std::make_shared<ArkWebFirstMeaningfulPaintDetailsWrapper>(details));
}

void ArkWebHandlerImpl::OnLargestContentfulPaint(ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> details)
{
    if (CHECK_REF_PTR_IS_NULL(details)) {
        return;
    }

    nweb_handler_->OnLargestContentfulPaint(std::make_shared<ArkWebLargestContentfulPaintDetailsWrapper>(details));
}

void ArkWebHandlerImpl::OnIntelligentTrackingPreventionResult(
    const ArkWebString& website_host, const ArkWebString& tracker_host)
{
    nweb_handler_->OnIntelligentTrackingPreventionResult(
        ArkWebStringStructToClass(website_host), ArkWebStringStructToClass(tracker_host));
}

bool ArkWebHandlerImpl::OnHandleOverrideUrlLoading(ArkWebRefPtr<ArkWebUrlResourceRequest> request)
{
    if (CHECK_REF_PTR_IS_NULL(request)) {
        return nweb_handler_->OnHandleOverrideUrlLoading(nullptr);
    }

    return nweb_handler_->OnHandleOverrideUrlLoading(std::make_shared<ArkWebUrlResourceRequestWrapper>(request));
}

bool ArkWebHandlerImpl::OnAllSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsAllSslErrorResult> result, int error,
    const ArkWebString& url, const ArkWebString& originalUrl, const ArkWebString& referrer, bool isFatalError,
    bool isMainFrame)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnAllSslErrorRequestByJS(nullptr, static_cast<ArkWebSslError>(error),
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(originalUrl), ArkWebStringStructToClass(referrer),
            isFatalError, isMainFrame);
    }

    return nweb_handler_->OnAllSslErrorRequestByJS(std::make_shared<ArkWebJsAllSslErrorResultWrapper>(result),
        static_cast<ArkWebSslError>(error), ArkWebStringStructToClass(url), ArkWebStringStructToClass(originalUrl),
        ArkWebStringStructToClass(referrer), isFatalError, isMainFrame);
}

void ArkWebHandlerImpl::OnTooltip(const ArkWebString& tooltip)
{
    nweb_handler_->OnTooltip(ArkWebStringStructToClass(tooltip));
}

void ArkWebHandlerImpl::ReleaseResizeHold()
{
    nweb_handler_->ReleaseResizeHold();
}

ArkWebCharVector ArkWebHandlerImpl::GetWordSelection(const ArkWebString& text, int8_t offset)
{
    if (!nweb_handler_) {
        return ark_web_char_vector_default;
    }
    std::vector<int8_t> vec = nweb_handler_->GetWordSelection(ArkWebStringStructToClass(text), offset);
    std::vector<char> result(vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        result[i] = vec[i];
    }
    ArkWebCharVector ark_result = ArkWebBasicVectorClassToStruct<char, ArkWebCharVector>(result);
    return ark_result;
}

void ArkWebHandlerImpl::UpdateClippedSelectionBounds(int x, int y, int w, int h)
{
    nweb_handler_->UpdateClippedSelectionBounds(x, y, w, h);
}

bool ArkWebHandlerImpl::OnOpenAppLink(const ArkWebString& url, ArkWebRefPtr<ArkWebAppLinkCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return nweb_handler_->OnOpenAppLink(ArkWebStringStructToClass(url), nullptr);
    }

    return nweb_handler_->OnOpenAppLink(
        ArkWebStringStructToClass(url), std::make_shared<ArkWebAppLinkCallbackWrapper>(callback));
}
void ArkWebHandlerImpl::OnRenderProcessNotResponding(const ArkWebString& js_stack, int pid, int reason)
{
    nweb_handler_->OnRenderProcessNotResponding(
        ArkWebStringStructToClass(js_stack), pid, static_cast<ArkWebRenderProcessNotRespondingReason>(reason));
}

void ArkWebHandlerImpl::OnRenderProcessResponding()
{
    nweb_handler_->OnRenderProcessResponding();
}

void ArkWebHandlerImpl::OnShowAutofillPopup(
    const float offsetX, const float offsetY, const ArkWebStringVector& menu_items)
{
    nweb_handler_->OnShowAutofillPopup(offsetX, offsetY, ArkWebStringVectorStructToClass(menu_items));
}

void ArkWebHandlerImpl::OnHideAutofillPopup()
{
    nweb_handler_->OnHideAutofillPopup();
}

void ArkWebHandlerImpl::OnViewportFitChange(int viewportFit)
{
    nweb_handler_->OnViewportFitChange(static_cast<ArkWebViewportFit>(viewportFit));
}

void ArkWebHandlerImpl::CreateOverlay(void* data, size_t len, int width, int height, int offset_x, int offset_y,
    int rect_width, int rect_height, int point_x, int point_y)
{
    nweb_handler_->CreateOverlay(
        data, len, width, height, offset_x, offset_y, rect_width, rect_height, point_x, point_y);
}

void ArkWebHandlerImpl::OnOverlayStateChanged(int offset_x, int offset_y, int rect_width, int rect_height)
{
    nweb_handler_->OnOverlayStateChanged(offset_x, offset_y, rect_width, rect_height);
}

bool ArkWebHandlerImpl::OnFocus(int source)
{
    return nweb_handler_->OnFocus(static_cast<ArkWebFocusSource>(source));
}

bool ArkWebHandlerImpl::OnOverScroll(float xOffset, float yOffset, float xVelocity, float yVelocity)
{
    return nweb_handler_->OnOverScroll(xOffset, yOffset, xVelocity, yVelocity);
}

void ArkWebHandlerImpl::KeyboardReDispatch(ArkWebRefPtr<ArkWebKeyEvent> event, bool isUsed)
{
    if (CHECK_REF_PTR_IS_NULL(event)) {
        nweb_handler_->KeyboardReDispatch(nullptr, isUsed);
    }

    nweb_handler_->KeyboardReDispatch(std::make_shared<ArkWebKeyEventWrapper>(event), isUsed);
}

void ArkWebHandlerImpl::OnInterceptKeyboardAttach(ArkWebRefPtr<ArkWebCustomKeyboardHandler> keyboardHandler,
    const ArkWebStringMap& attributes, bool& useSystemKeyboard, int32_t& enterKeyType)
{
    if (CHECK_REF_PTR_IS_NULL(keyboardHandler)) {
        return nweb_handler_->OnInterceptKeyboardAttach(
            nullptr, ArkWebStringMapStructToClass(attributes), useSystemKeyboard, enterKeyType);
    }

    nweb_handler_->OnInterceptKeyboardAttach(std::make_shared<ArkWebCustomKeyboardHandlerWrapper>(keyboardHandler),
        ArkWebStringMapStructToClass(attributes), useSystemKeyboard, enterKeyType);
}

void ArkWebHandlerImpl::OnCustomKeyboardAttach()
{
    nweb_handler_->OnCustomKeyboardAttach();
}

void ArkWebHandlerImpl::OnCustomKeyboardClose()
{
    nweb_handler_->OnCustomKeyboardClose();
}

void ArkWebHandlerImpl::OnAdsBlocked(const ArkWebString& url, const ArkWebStringVector& adsBlocked)
{
    nweb_handler_->OnAdsBlocked(ArkWebStringStructToClass(url), ArkWebStringVectorStructToClass(adsBlocked));
}

void ArkWebHandlerImpl::OnCursorUpdate(double x, double y, double width, double height)
{
    nweb_handler_->OnCursorUpdate(x, y, width, height);
}

void ArkWebHandlerImpl::ReportDynamicFrameLossEvent(const ArkWebString& sceneId, bool isStart)
{
    nweb_handler_->ReportDynamicFrameLossEvent(ArkWebStringStructToClass(sceneId), isStart);
}

void ArkWebHandlerImpl::HideHandleAndQuickMenuIfNecessary(bool hide)
{
    nweb_handler_->HideHandleAndQuickMenuIfNecessary(hide);
}

void ArkWebHandlerImpl::ChangeVisibilityOfQuickMenu()
{
    nweb_handler_->ChangeVisibilityOfQuickMenu();
}

void ArkWebHandlerImpl::StartVibraFeedback(const ArkWebString& vibratorType)
{
    nweb_handler_->StartVibraFeedback(ArkWebStringStructToClass(vibratorType));
}

void ArkWebHandlerImpl::OnPopupSize(int x, int y, int width, int height)
{
    nweb_handler_->OnPopupSize(x, y, width, height);
}

void ArkWebHandlerImpl::OnPopupShow(bool show)
{
    nweb_handler_->OnPopupShow(show);
}

void ArkWebHandlerImpl::OnNativeEmbedVisibilityChange(const ArkWebString& embed_id, bool visibility)
{
    nweb_handler_->OnNativeEmbedVisibilityChange(ArkWebStringStructToClass(embed_id), visibility);
}

bool ArkWebHandlerImpl::CloseImageOverlaySelection()
{
    return nweb_handler_->CloseImageOverlaySelection();
}

bool ArkWebHandlerImpl::OnSslErrorRequestByJSV2(ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error,
    const ArkWebStringVector& certChainData)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnSslErrorRequestByJSV2(nullptr, static_cast<ArkWebSslError>(error),
            ArkWebStringVectorStructToClass(certChainData));
    }

    return nweb_handler_->OnSslErrorRequestByJSV2(
        std::make_shared<ArkWebJsSslErrorResultWrapper>(result), static_cast<ArkWebSslError>(error),
            ArkWebStringVectorStructToClass(certChainData));
}

void ArkWebHandlerImpl::OnAccessibilityEvent(int64_t accessibilityId, int32_t eventType)
{
    nweb_handler_->OnAccessibilityEvent(accessibilityId, eventType);
}

bool ArkWebHandlerImpl::IsCurrentFocus()
{
    return nweb_handler_->IsCurrentFocus();
}

void ArkWebHandlerImpl::GetVisibleRectToWeb(int& visibleX, int& visibleY, int& visibleWidth, int& visibleHeight)
{
    nweb_handler_->GetVisibleRectToWeb(visibleX, visibleY, visibleWidth, visibleHeight);
}

void ArkWebHandlerImpl::OnScrollStart(const float x, const float y)
{
    nweb_handler_->OnScrollStart(x, y);
}

void ArkWebHandlerImpl::OnShowAutofillPopupV2(
    const float offsetX, const float offsetY, const float height, const float width,
    const ArkWebStringVector& menu_items)
{
    nweb_handler_->OnShowAutofillPopupV2(offsetX, offsetY, height, width, ArkWebStringVectorStructToClass(menu_items));
}

void ArkWebHandlerImpl::RestoreRenderFit()
{
    nweb_handler_->RestoreRenderFit();
}

void ArkWebHandlerImpl::OnAccessibilityEventV2(int64_t accessibilityId, int32_t eventType, const ArkWebString& argument)
{
    nweb_handler_->OnAccessibilityEventV2(accessibilityId, eventType, ArkWebStringStructToClass(argument));
}

bool ArkWebHandlerImpl::OnNestedScroll(float& x, float& y, float& xVelocity, float& yVelocity, bool& isAvailable)
{
    return nweb_handler_->OnNestedScroll(x, y, xVelocity, yVelocity, isAvailable);
}

void ArkWebHandlerImpl::EnableSecurityLayer(bool isNeedSecurityLayer)
{
    nweb_handler_->EnableSecurityLayer(isNeedSecurityLayer);
}

bool ArkWebHandlerImpl::ChangeVisibilityOfQuickMenuV2()
{
    return nweb_handler_->ChangeVisibilityOfQuickMenuV2();
}

void ArkWebHandlerImpl::OnPip(int status,
                              int delegate_id,
                              int child_id,
                              int frame_routing_id,
                              int width,
                              int height)
{
    nweb_handler_->OnPip(status, delegate_id, child_id,
                         frame_routing_id, width, height);
}

bool ArkWebHandlerImpl::OnBeforeUnloadByJSV2(
    const ArkWebString& url, const ArkWebString& message, bool isReload, ArkWebRefPtr<ArkWebJsDialogResult> result)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnBeforeUnloadByJSV2(
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(message), isReload, nullptr);
    }

    return nweb_handler_->OnBeforeUnloadByJSV2(ArkWebStringStructToClass(url), ArkWebStringStructToClass(message),
        isReload, std::make_shared<ArkWebJsDialogResultWrapper>(result));
}

void ArkWebHandlerImpl::OnNativeEmbedMouseEvent(ArkWebRefPtr<ArkWebNativeEmbedMouseEvent> mouse_event)
{
    if (CHECK_REF_PTR_IS_NULL(mouse_event)) {
        nweb_handler_->OnNativeEmbedMouseEvent(nullptr);
        return;
    }

    nweb_handler_->OnNativeEmbedMouseEvent(std::make_shared<ArkWebNativeEmbedMouseEventWrapper>(mouse_event));
}

void ArkWebHandlerImpl::OnActivateContentByJS()
{
    nweb_handler_->OnActivateContentByJS();
}

void ArkWebHandlerImpl::OnLoadStarted(const ArkWebString& url)
{
    nweb_handler_->OnLoadStarted(ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnLoadFinished(const ArkWebString& url)
{
    nweb_handler_->OnLoadFinished(ArkWebStringStructToClass(url));
}

bool ArkWebHandlerImpl::OnAllSslErrorRequestByJSV2(ArkWebRefPtr<ArkWebJsAllSslErrorResult> result, int error,
    const ArkWebString& url, const ArkWebString& originalUrl, const ArkWebString& referrer, bool isFatalError,
    bool isMainFrame, const ArkWebStringVector& certChainData)
{
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nweb_handler_->OnAllSslErrorRequestByJSV2(nullptr, static_cast<ArkWebSslError>(error),
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(originalUrl), ArkWebStringStructToClass(referrer),
            isFatalError, isMainFrame, ArkWebStringVectorStructToClass(certChainData));
    }

    return nweb_handler_->OnAllSslErrorRequestByJSV2(std::make_shared<ArkWebJsAllSslErrorResultWrapper>(result),
        static_cast<ArkWebSslError>(error), ArkWebStringStructToClass(url), ArkWebStringStructToClass(originalUrl),
        ArkWebStringStructToClass(referrer), isFatalError, isMainFrame, ArkWebStringVectorStructToClass(certChainData));
}

void ArkWebHandlerImpl::ShowMagnifier()
{
    nweb_handler_->ShowMagnifier();
}

void ArkWebHandlerImpl::HideMagnifier()
{
    nweb_handler_->HideMagnifier();
}

void ArkWebHandlerImpl::OnPageTitleV2(const ArkWebString& title, bool isRealTitle)
{
    nweb_handler_->OnPageTitleV2(ArkWebStringStructToClass(title), isRealTitle);
}

void ArkWebHandlerImpl::OnInsertBlanklessFrame(const ArkWebString& pathToFrame)
{
    nweb_handler_->OnInsertBlanklessFrame(ArkWebStringStructToClass(pathToFrame));
}

void ArkWebHandlerImpl::OnRemoveBlanklessFrame(int delayTime)
{
    nweb_handler_->OnRemoveBlanklessFrame(delayTime);
}

ArkWebString ArkWebHandlerImpl::OnHandleOverrideErrorPage(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request,
    ArkWebRefPtr<ArkWebUrlResourceError> error)
{
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> nweb_request = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(request)) {
        nweb_request = std::make_shared<ArkWebUrlResourceRequestWrapper>(request);
    }

    std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> nweb_error = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(error)) {
        nweb_error = std::make_shared<ArkWebUrlResourceErrorWrapper>(error);
    }

    return ArkWebStringClassToStruct(nweb_handler_->OnHandleOverrideErrorPage(nweb_request, nweb_error));
}

void ArkWebHandlerImpl::OnPdfScrollAtBottom(const ArkWebString& url)
{
    nweb_handler_->OnPdfScrollAtBottom(ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnPdfLoadEvent(int32_t result, const ArkWebString& url)
{
    nweb_handler_->OnPdfLoadEvent(result, ArkWebStringStructToClass(url));
}

void ArkWebHandlerImpl::OnTakeFocus(ArkWebRefPtr<ArkWebKeyEvent> event)
{
    if (CHECK_REF_PTR_IS_NULL(event)) {
        nweb_handler_->OnTakeFocus(nullptr);
        return;
    }

    nweb_handler_->OnTakeFocus(std::make_shared<ArkWebKeyEventWrapper>(event));
}

void ArkWebHandlerImpl::OnInsertBlanklessFrameWithSize(const ArkWebString& pathToFrame,
                                                       uint32_t width,
                                                       uint32_t height)
{
    nweb_handler_->OnInsertBlanklessFrameWithSize(ArkWebStringStructToClass(pathToFrame), width, height);
}

void ArkWebHandlerImpl::SetImeShow(bool visible)
{
    nweb_handler_->SetImeShow(visible);
}

bool ArkWebHandlerImpl::IsShowHandle()
{
    return nweb_handler_->IsShowHandle();
}

void ArkWebHandlerImpl::OnNativeEmbedObjectParamChange(ArkWebRefPtr<ArkWebNativeEmbedParamDataInfo> data_info)
{
    if (CHECK_REF_PTR_IS_NULL(data_info)) {
        nweb_handler_->OnNativeEmbedObjectParamChange(nullptr);
        return;
    }

    nweb_handler_->OnNativeEmbedObjectParamChange(std::make_shared<ArkWebNativeEmbedParamDataInfoWrapper>(data_info));
}

void ArkWebHandlerImpl::OnExtensionDisconnect(int32_t connectId)
{
    nweb_handler_->OnExtensionDisconnect(connectId);
}

ArkWebString ArkWebHandlerImpl::OnWebNativeMessage(
    ArkWebRefPtr<ArkWebRuntimeConnectInfo> info, ArkWebRefPtr<ArkWebNativeMessageCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebRuntimeConnectInfo> nweb_connect_info = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(info)) {
        nweb_connect_info = std::make_shared<ArkWebRuntimeConnectInfoWrapper>(info);
    }
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return ArkWebStringClassToStruct(nweb_handler_->OnWebNativeMessage(nweb_connect_info, nullptr));
    }
    return ArkWebStringClassToStruct(nweb_handler_->OnWebNativeMessage(
        nweb_connect_info, std::make_shared<ArkWebNativeMessageCallbackWrapper>(callback)));
}
} // namespace OHOS::ArkWeb
