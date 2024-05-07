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

#include "ohos_nweb/ctocpp/ark_web_nweb_ctocpp.h"

#include "ohos_nweb/cpptoc/ark_web_accessibility_event_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_bool_value_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_cache_options_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_create_native_media_player_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_download_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_drag_event_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_find_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_handler_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_js_result_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_message_value_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_release_surface_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_screen_lock_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_string_value_callback_cpptoc.h"
#include "ohos_nweb/ctocpp/ark_web_accessibility_node_info_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_drag_data_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_history_list_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_hit_test_result_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_preference_ctocpp.h"

#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::Resize(uint32_t width, uint32_t height, bool is_keyboard)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, resize, );

    // Execute
    _struct->resize(_struct, width, height, is_keyboard);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnPause()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_pause, );

    // Execute
    _struct->on_pause(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnContinue()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_continue, );

    // Execute
    _struct->on_continue(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnDestroy()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_destroy, );

    // Execute
    _struct->on_destroy(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnFocus(const int32_t& focus_reason)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_focus, );

    // Execute
    _struct->on_focus(_struct, &focus_reason);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnBlur(const int32_t& blur_reason)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_blur, );

    // Execute
    _struct->on_blur(_struct, &blur_reason);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnTouchPress(int32_t id, double x, double y, bool from_overlay)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_press, );

    // Execute
    _struct->on_touch_press(_struct, id, x, y, from_overlay);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnTouchRelease(int32_t id, double x, double y, bool from_overlay)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_release, );

    // Execute
    _struct->on_touch_release(_struct, id, x, y, from_overlay);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnTouchMove(int32_t id, double x, double y, bool from_overlay)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_move1, );

    // Execute
    _struct->on_touch_move1(_struct, id, x, y, from_overlay);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnTouchMove(const ArkWebTouchPointInfoVector& touch_point_infos, bool from_overlay)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_move2, );

    // Execute
    _struct->on_touch_move2(_struct, &touch_point_infos, from_overlay);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnTouchCancel()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_cancel, );

    // Execute
    _struct->on_touch_cancel(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnNavigateBack()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_navigate_back, );

    // Execute
    _struct->on_navigate_back(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::SendKeyEvent(int32_t key_code, int32_t key_action)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, send_key_event, false);

    // Execute
    return _struct->send_key_event(_struct, key_code, key_action);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SendMouseWheelEvent(double x, double y, double delta_x, double delta_y)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, send_mouse_wheel_event, );

    // Execute
    _struct->send_mouse_wheel_event(_struct, x, y, delta_x, delta_y);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SendMouseEvent(int x, int y, int button, int action, int count)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, send_mouse_event, );

    // Execute
    _struct->send_mouse_event(_struct, x, y, button, action, count);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::Load(const ArkWebString& url)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, load1, 0);

    // Execute
    return _struct->load1(_struct, &url);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::IsNavigatebackwardAllowed()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_navigatebackward_allowed, false);

    // Execute
    return _struct->is_navigatebackward_allowed(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::IsNavigateForwardAllowed()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_navigate_forward_allowed, false);

    // Execute
    return _struct->is_navigate_forward_allowed(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::CanNavigateBackOrForward(int num_steps)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, can_navigate_back_or_forward, false);

    // Execute
    return _struct->can_navigate_back_or_forward(_struct, num_steps);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::NavigateBack()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, navigate_back, );

    // Execute
    _struct->navigate_back(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::NavigateForward()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, navigate_forward, );

    // Execute
    _struct->navigate_forward(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::NavigateBackOrForward(int step)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, navigate_back_or_forward, );

    // Execute
    _struct->navigate_back_or_forward(_struct, step);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::DeleteNavigateHistory()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, delete_navigate_history, );

    // Execute
    _struct->delete_navigate_history(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::Reload()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, reload, );

    // Execute
    _struct->reload(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::Zoom(float zoom_factor)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, zoom, 0);

    // Execute
    return _struct->zoom(_struct, zoom_factor);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::ZoomIn()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, zoom_in, 0);

    // Execute
    return _struct->zoom_in(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::ZoomOut()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, zoom_out, 0);

    // Execute
    return _struct->zoom_out(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::Stop()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop, );

    // Execute
    _struct->stop(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ExecuteJavaScript(const ArkWebString& code)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, execute_java_script1, );

    // Execute
    _struct->execute_java_script1(_struct, &code);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ExecuteJavaScript(
    const ArkWebString& code, ArkWebRefPtr<ArkWebMessageValueCallback> callback, bool extention)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, execute_java_script2, );

    // Execute
    _struct->execute_java_script2(_struct, &code, ArkWebMessageValueCallbackCppToC::Invert(callback), extention);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebPreference> ArkWebNWebCToCpp::GetPreference()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_preference, nullptr);

    // Execute
    ark_web_preference_t* _retval = _struct->get_preference(_struct);

    // Return type: refptr_same
    return ArkWebPreferenceCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
unsigned int ArkWebNWebCToCpp::GetWebId()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_web_id, 0);

    // Execute
    return _struct->get_web_id(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebHitTestResult> ArkWebNWebCToCpp::GetHitTestResult()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_hit_test_result, nullptr);

    // Execute
    ark_web_hit_test_result_t* _retval = _struct->get_hit_test_result(_struct);

    // Return type: refptr_same
    return ArkWebHitTestResultCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutBackgroundColor(int color)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_background_color, );

    // Execute
    _struct->put_background_color(_struct, color);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::InitialScale(float scale)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, initial_scale, );

    // Execute
    _struct->initial_scale(_struct, scale);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutDownloadCallback(ArkWebRefPtr<ArkWebDownloadCallback> download_listener)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_download_callback, );

    // Execute
    _struct->put_download_callback(_struct, ArkWebDownloadCallbackCppToC::Invert(download_listener));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutAccessibilityEventCallback(
    ArkWebRefPtr<ArkWebAccessibilityEventCallback> accessibility_event_listener)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_accessibility_event_callback, );

    // Execute
    _struct->put_accessibility_event_callback(
        _struct, ArkWebAccessibilityEventCallbackCppToC::Invert(accessibility_event_listener));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutAccessibilityIdGenerator(const AccessibilityIdGenerateFunc accessibility_id_generator)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_accessibility_id_generator, );

    // Execute
    _struct->put_accessibility_id_generator(_struct, accessibility_id_generator);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetNWebHandler(ArkWebRefPtr<ArkWebHandler> handler)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_nweb_handler, );

    // Execute
    _struct->set_nweb_handler(_struct, ArkWebHandlerCppToC::Invert(handler));
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkWebNWebCToCpp::Title()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, title, ark_web_string_default);

    // Execute
    return _struct->title(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::PageLoadProgress()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, page_load_progress, 0);

    // Execute
    return _struct->page_load_progress(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::ContentHeight()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, content_height, 0);

    // Execute
    return _struct->content_height(_struct);
}

ARK_WEB_NO_SANITIZE
float ArkWebNWebCToCpp::Scale()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, scale, 0);

    // Execute
    return _struct->scale(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::Load(const ArkWebString& url, const ArkWebStringMap& additional_http_headers)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, load2, 0);

    // Execute
    return _struct->load2(_struct, &url, &additional_http_headers);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::LoadWithDataAndBaseUrl(const ArkWebString& base_url, const ArkWebString& data,
    const ArkWebString& mime_type, const ArkWebString& encoding, const ArkWebString& history_url)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, load_with_data_and_base_url, 0);

    // Execute
    return _struct->load_with_data_and_base_url(_struct, &base_url, &data, &mime_type, &encoding, &history_url);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::LoadWithData(
    const ArkWebString& data, const ArkWebString& mime_type, const ArkWebString& encoding)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, load_with_data, 0);

    // Execute
    return _struct->load_with_data(_struct, &data, &mime_type, &encoding);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RegisterArkJSfunction(
    const ArkWebString& object_name, const ArkWebStringVector& method_list, const int32_t object_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_ark_jsfunction1, );

    // Execute
    _struct->register_ark_jsfunction1(_struct, &object_name, &method_list, object_id);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::UnregisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unregister_ark_jsfunction, );

    // Execute
    _struct->unregister_ark_jsfunction(_struct, &object_name, &method_list);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetNWebJavaScriptResultCallBack(ArkWebRefPtr<ArkWebJsResultCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_nweb_java_script_result_call_back, );

    // Execute
    _struct->set_nweb_java_script_result_call_back(_struct, ArkWebJsResultCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutFindCallback(ArkWebRefPtr<ArkWebFindCallback> find_listener)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_find_callback, );

    // Execute
    _struct->put_find_callback(_struct, ArkWebFindCallbackCppToC::Invert(find_listener));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::FindAllAsync(const ArkWebString& search_str)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, find_all_async, );

    // Execute
    _struct->find_all_async(_struct, &search_str);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ClearMatches()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, clear_matches, );

    // Execute
    _struct->clear_matches(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::FindNext(const bool forward)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, find_next, );

    // Execute
    _struct->find_next(_struct, forward);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::StoreWebArchive(
    const ArkWebString& base_name, bool auto_name, ArkWebRefPtr<ArkWebStringValueCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, store_web_archive, );

    // Execute
    _struct->store_web_archive(_struct, &base_name, auto_name, ArkWebStringValueCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
ArkWebStringVector ArkWebNWebCToCpp::CreateWebMessagePorts()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_vector_default);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_web_message_ports, ark_web_string_vector_default);

    // Execute
    return _struct->create_web_message_ports(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PostWebMessage(
    const ArkWebString& message, const ArkWebStringVector& ports, const ArkWebString& target_uri)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, post_web_message, );

    // Execute
    _struct->post_web_message(_struct, &message, &ports, &target_uri);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ClosePort(const ArkWebString& port_handle)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, close_port, );

    // Execute
    _struct->close_port(_struct, &port_handle);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PostPortMessage(const ArkWebString& port_handle, const ArkWebMessage& data)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, post_port_message, );

    // Execute
    _struct->post_port_message(_struct, &port_handle, &data);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetPortMessageCallback(
    const ArkWebString& port_handle, ArkWebRefPtr<ArkWebMessageValueCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_port_message_callback, );

    // Execute
    _struct->set_port_message_callback(_struct, &port_handle, ArkWebMessageValueCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SendDragEvent(ArkWebRefPtr<ArkWebDragEvent> drag_event)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, send_drag_event, );

    // Execute
    _struct->send_drag_event(_struct, ArkWebDragEventCppToC::Invert(drag_event));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ClearSslCache()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, clear_ssl_cache, );

    // Execute
    _struct->clear_ssl_cache(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkWebNWebCToCpp::GetUrl()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_url, ark_web_string_default);

    // Execute
    return _struct->get_url(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ClearClientAuthenticationCache()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, clear_client_authentication_cache, );

    // Execute
    _struct->clear_client_authentication_cache(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::UpdateLocale(const ArkWebString& language, const ArkWebString& region)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, update_locale, );

    // Execute
    _struct->update_locale(_struct, &language, &region);
}

ARK_WEB_NO_SANITIZE
const ArkWebString ArkWebNWebCToCpp::GetOriginalUrl()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_original_url, ark_web_string_default);

    // Execute
    return _struct->get_original_url(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::GetFavicon(const void** data, size_t& width, size_t& height, int& color_type, int& alpha_type)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_favicon, false);

    // Execute
    return _struct->get_favicon(_struct, data, &width, &height, &color_type, &alpha_type);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutNetworkAvailable(bool available)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_network_available, );

    // Execute
    _struct->put_network_available(_struct, available);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::HasImages(ArkWebRefPtr<ArkWebBoolValueCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, has_images, );

    // Execute
    _struct->has_images(_struct, ArkWebBoolValueCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RemoveCache(bool include_disk_files)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, remove_cache, );

    // Execute
    _struct->remove_cache(_struct, include_disk_files);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebHistoryList> ArkWebNWebCToCpp::GetHistoryList()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_history_list, nullptr);

    // Execute
    ark_web_history_list_t* _retval = _struct->get_history_list(_struct);

    // Return type: refptr_same
    return ArkWebHistoryListCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PutReleaseSurfaceCallback(ArkWebRefPtr<ArkWebReleaseSurfaceCallback> release_surface_listener)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, put_release_surface_callback, );

    // Execute
    _struct->put_release_surface_callback(
        _struct, ArkWebReleaseSurfaceCallbackCppToC::Invert(release_surface_listener));
}

ARK_WEB_NO_SANITIZE
ArkWebUint8Vector ArkWebNWebCToCpp::SerializeWebState()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_uint8_vector_default);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, serialize_web_state, ark_web_uint8_vector_default);

    // Execute
    return _struct->serialize_web_state(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::RestoreWebState(const ArkWebUint8Vector& state)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, restore_web_state, false);

    // Execute
    return _struct->restore_web_state(_struct, &state);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PageUp(bool top)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, page_up, );

    // Execute
    _struct->page_up(_struct, top);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PageDown(bool bottom)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, page_down, );

    // Execute
    _struct->page_down(_struct, bottom);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ScrollTo(float x, float y)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, scroll_to, );

    // Execute
    _struct->scroll_to(_struct, x, y);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ScrollBy(float delta_x, float delta_y)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, scroll_by, );

    // Execute
    _struct->scroll_by(_struct, delta_x, delta_y);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SlideScroll(float vx, float vy)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, slide_scroll, );

    // Execute
    _struct->slide_scroll(_struct, vx, vy);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::GetCertChainDerData(ArkWebStringVector& cert_chain_data, bool is_single_cert)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_cert_chain_der_data, false);

    // Execute
    return _struct->get_cert_chain_der_data(_struct, &cert_chain_data, is_single_cert);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetScreenOffSet(double x, double y)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_screen_off_set, );

    // Execute
    _struct->set_screen_off_set(_struct, x, y);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetAudioMuted(bool muted)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_audio_muted, );

    // Execute
    _struct->set_audio_muted(_struct, muted);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetShouldFrameSubmissionBeforeDraw(bool should)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_should_frame_submission_before_draw, );

    // Execute
    _struct->set_should_frame_submission_before_draw(_struct, should);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::NotifyPopupWindowResult(bool result)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, notify_popup_window_result, );

    // Execute
    _struct->notify_popup_window_result(_struct, result);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetAudioResumeInterval(int32_t resume_interval)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_audio_resume_interval, );

    // Execute
    _struct->set_audio_resume_interval(_struct, resume_interval);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetAudioExclusive(bool audio_exclusive)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_audio_exclusive, );

    // Execute
    _struct->set_audio_exclusive(_struct, audio_exclusive);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RegisterScreenLockFunction(int32_t window_id, ArkWebRefPtr<ArkWebScreenLockCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_screen_lock_function, );

    // Execute
    _struct->register_screen_lock_function(_struct, window_id, ArkWebScreenLockCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::UnRegisterScreenLockFunction(int32_t window_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, un_register_screen_lock_function, );

    // Execute
    _struct->un_register_screen_lock_function(_struct, window_id);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::NotifyMemoryLevel(int32_t level)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, notify_memory_level, );

    // Execute
    _struct->notify_memory_level(_struct, level);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnWebviewHide()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_webview_hide, );

    // Execute
    _struct->on_webview_hide(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnWebviewShow()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_webview_show, );

    // Execute
    _struct->on_webview_show(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebDragData> ArkWebNWebCToCpp::GetOrCreateDragData()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_or_create_drag_data, nullptr);

    // Execute
    ark_web_drag_data_t* _retval = _struct->get_or_create_drag_data(_struct);

    // Return type: refptr_same
    return ArkWebDragDataCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PrefetchPage(const ArkWebString& url, const ArkWebStringMap& additional_http_headers)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, prefetch_page, );

    // Execute
    _struct->prefetch_page(_struct, &url, &additional_http_headers);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetWindowId(uint32_t window_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_window_id, );

    // Execute
    _struct->set_window_id(_struct, window_id);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnOccluded()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_occluded, );

    // Execute
    _struct->on_occluded(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnUnoccluded()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_unoccluded, );

    // Execute
    _struct->on_unoccluded(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetToken(void* token)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_token, );

    // Execute
    _struct->set_token(_struct, token);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetNestedScrollMode(const int32_t& nested_scroll_mode)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_nested_scroll_mode, );

    // Execute
    _struct->set_nested_scroll_mode(_struct, &nested_scroll_mode);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetEnableLowerFrameRate(bool enabled)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_enable_lower_frame_rate, );

    // Execute
    _struct->set_enable_lower_frame_rate(_struct, enabled);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_virtual_key_board_arg, );

    // Execute
    _struct->set_virtual_key_board_arg(_struct, width, height, keyboard);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::ShouldVirtualKeyboardOverlay()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, should_virtual_keyboard_overlay, false);

    // Execute
    return _struct->should_virtual_keyboard_overlay(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_draw_rect, );

    // Execute
    _struct->set_draw_rect(_struct, x, y, width, height);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetDrawMode(int32_t mode)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_draw_mode, );

    // Execute
    _struct->set_draw_mode(_struct, mode);
}

ARK_WEB_NO_SANITIZE
void* ArkWebNWebCToCpp::CreateWebPrintDocumentAdapter(const ArkWebString& job_name)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_web_print_document_adapter, nullptr);

    // Execute
    return _struct->create_web_print_document_adapter(_struct, &job_name);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::PostUrl(const ArkWebString& url, const ArkWebCharVector& post_data)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, post_url, 0);

    // Execute
    return _struct->post_url(_struct, &url, &post_data);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::JavaScriptOnDocumentStart(const ArkWebStringVectorMap& script_items)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, java_script_on_document_start, );

    // Execute
    _struct->java_script_on_document_start(_struct, &script_items);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ExecuteAction(int64_t accessibility_id, uint32_t action)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, execute_action, );

    // Execute
    _struct->execute_action(_struct, accessibility_id, action);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ArkWebNWebCToCpp::GetFocusedAccessibilityNodeInfo(
    int64_t accessibility_id, bool is_accessibility_focus)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_focused_accessibility_node_info, nullptr);

    // Execute
    ark_web_accessibility_node_info_t* _retval =
        _struct->get_focused_accessibility_node_info(_struct, accessibility_id, is_accessibility_focus);

    // Return type: refptr_same
    return ArkWebAccessibilityNodeInfoCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ArkWebNWebCToCpp::GetAccessibilityNodeInfoById(int64_t accessibility_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_accessibility_node_info_by_id, nullptr);

    // Execute
    ark_web_accessibility_node_info_t* _retval = _struct->get_accessibility_node_info_by_id(_struct, accessibility_id);

    // Return type: refptr_same
    return ArkWebAccessibilityNodeInfoCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ArkWebNWebCToCpp::GetAccessibilityNodeInfoByFocusMove(
    int64_t accessibility_id, int32_t direction)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_accessibility_node_info_by_focus_move, nullptr);

    // Execute
    ark_web_accessibility_node_info_t* _retval =
        _struct->get_accessibility_node_info_by_focus_move(_struct, accessibility_id, direction);

    // Return type: refptr_same
    return ArkWebAccessibilityNodeInfoCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetAccessibilityState(bool state)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_accessibility_state, );

    // Execute
    _struct->set_accessibility_state(_struct, state);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::NeedSoftKeyboard()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, need_soft_keyboard, false);

    // Execute
    return _struct->need_soft_keyboard(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::Discard()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, discard, false);

    // Execute
    return _struct->discard(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::Restore()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, restore, false);

    // Execute
    return _struct->restore(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::GetSecurityLevel()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_security_level, 0);

    // Execute
    return _struct->get_security_level(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::CallH5Function(
    int32_t routingId, int32_t h5ObjectId, const ArkWebString& h5MethodName, const ArkWebValueVector& args)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, call_h5function, );

    // Execute
    _struct->call_h5function(_struct, routingId, h5ObjectId, &h5MethodName, &args);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::IsIncognitoMode()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_incognito_mode, false);

    // Execute
    return _struct->is_incognito_mode(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RegisterNativeArkJSFunction(const char* objName, const ArkWebJsProxyCallbackVector& callbacks)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_native_ark_jsfunction, );

    // Execute
    _struct->register_native_ark_jsfunction(_struct, objName, &callbacks);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::UnRegisterNativeArkJSFunction(const char* objName)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, un_register_native_ark_jsfunction, );

    // Execute
    _struct->un_register_native_ark_jsfunction(_struct, objName);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_native_valide_callback, );

    // Execute
    _struct->register_native_valide_callback(_struct, webName, callback);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnValidCallback callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_native_destroy_callback, );

    // Execute
    _struct->register_native_destroy_callback(_struct, webName, callback);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::JavaScriptOnDocumentEnd(const ArkWebStringVectorMap& script_items)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, java_script_on_document_end, );

    // Execute
    _struct->java_script_on_document_end(_struct, &script_items);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::EnableSafeBrowsing(bool enable)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, enable_safe_browsing, );

    // Execute
    _struct->enable_safe_browsing(_struct, enable);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::IsSafeBrowsingEnabled()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_safe_browsing_enabled, false);

    // Execute
    return _struct->is_safe_browsing_enabled(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SetPrintBackground(bool enable)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_print_background, );

    // Execute
    _struct->set_print_background(_struct, enable);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::GetPrintBackground()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_print_background, false);

    // Execute
    return _struct->get_print_background(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::CloseAllMediaPresentations()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, close_all_media_presentations, );

    // Execute
    _struct->close_all_media_presentations(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::StopAllMedia()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_all_media, );

    // Execute
    _struct->stop_all_media(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ResumeAllMedia()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, resume_all_media, );

    // Execute
    _struct->resume_all_media(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PauseAllMedia()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, pause_all_media, );

    // Execute
    _struct->pause_all_media(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::GetMediaPlaybackState()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_media_playback_state, 0);

    // Execute
    return _struct->get_media_playback_state(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::EnableIntelligentTrackingPrevention(bool enable)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, enable_intelligent_tracking_prevention, );

    // Execute
    _struct->enable_intelligent_tracking_prevention(_struct, enable);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::IsIntelligentTrackingPreventionEnabled()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_intelligent_tracking_prevention_enabled, false);

    // Execute
    return _struct->is_intelligent_tracking_prevention_enabled(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::StartCamera()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_camera, );

    // Execute
    _struct->start_camera(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::StopCamera()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_camera, );

    // Execute
    _struct->stop_camera(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::CloseCamera()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, close_camera, );

    // Execute
    _struct->close_camera(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkWebNWebCToCpp::GetLastJavascriptProxyCallingFrameUrl()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_last_javascript_proxy_calling_frame_url, ark_web_string_default);

    // Execute
    return _struct->get_last_javascript_proxy_calling_frame_url(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebNWebCToCpp::GetPendingSizeStatus()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_pending_size_status, false);

    // Execute
    return _struct->get_pending_size_status(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, scroll_by_ref_screen, );

    // Execute
    _struct->scroll_by_ref_screen(_struct, delta_x, delta_y, vx, vy);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::ExecuteJavaScriptExt(
    const int fd, const size_t scriptLength, ArkWebRefPtr<ArkWebMessageValueCallback> callback, bool extention)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, execute_java_script_ext, );

    // Execute
    _struct->execute_java_script_ext(
        _struct, fd, scriptLength, ArkWebMessageValueCallbackCppToC::Invert(callback), extention);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnRenderToBackground()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_render_to_background, );

    // Execute
    _struct->on_render_to_background(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnRenderToForeground()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_render_to_foreground, );

    // Execute
    _struct->on_render_to_foreground(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::PrecompileJavaScript(const ArkWebString& url, const ArkWebString& script,
    ArkWebRefPtr<ArkWebCacheOptions>& cacheOptions, ArkWebRefPtr<ArkWebMessageValueCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, precompile_java_script, );

    // Translate param: cacheOptions; type: refptr_diff_byref
    ark_web_cache_options_t* cacheOptionsStruct = NULL;
    if (cacheOptions.get()) {
        cacheOptionsStruct = ArkWebCacheOptionsCppToC::Invert(cacheOptions);
    }
    ark_web_cache_options_t* cacheOptionsOrig = cacheOptionsStruct;

    // Execute
    _struct->precompile_java_script(
        _struct, &url, &script, &cacheOptionsStruct, ArkWebMessageValueCallbackCppToC::Invert(callback));

    // Restore param:cacheOptions; type: refptr_diff_byref
    if (cacheOptionsStruct) {
        if (cacheOptionsStruct != cacheOptionsOrig) {
            cacheOptions = ArkWebCacheOptionsCppToC::Revert(cacheOptionsStruct);
        }
    } else {
        cacheOptions = nullptr;
    }
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnCreateNativeMediaPlayer(ArkWebRefPtr<ArkWebCreateNativeMediaPlayerCallback> callback)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_create_native_media_player, );

    // Execute
    _struct->on_create_native_media_player(_struct, ArkWebCreateNativeMediaPlayerCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, drag_resize, );

    // Execute
    _struct->drag_resize(_struct, width, height, pre_height, pre_width);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::OnTouchCancelById(int32_t id, double x, double y, bool from_overlay)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_cancel_by_id, );

    // Execute
    _struct->on_touch_cancel_by_id(_struct, id, x, y, from_overlay);
}

ARK_WEB_NO_SANITIZE
int ArkWebNWebCToCpp::ScaleGestureChange(double scale, double centerX, double centerY)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, scale_gesture_change, 0);

    // Execute
    return _struct->scale_gesture_change(_struct, scale, centerX, centerY);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::InjectOfflineResource(const ArkWebString& url, const ArkWebString& origin,
    const ArkWebUint8Vector& resource, const ArkWebStringMap& responseHeaders, const int type)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, inject_offline_resource, );

    // Execute
    _struct->inject_offline_resource(_struct, &url, &origin, &resource, &responseHeaders, type);
}

bool ArkWebNWebCToCpp::TerminateRenderProcess()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, terminate_render_process, false);

    // Execute
    return _struct->terminate_render_process(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SuggestionSelected(int32_t index)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, suggestion_selected, );

    // Execute
    _struct->suggestion_selected(_struct, index);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::RegisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list,
    const ArkWebStringVector& async_method_list, const int32_t object_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_nweb_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_ark_jsfunction2, );

    // Execute
    _struct->register_ark_jsfunction2(_struct, &object_name, &method_list, &async_method_list, object_id);
}

ARK_WEB_NO_SANITIZE
void ArkWebNWebCToCpp::SendTouchpadFlingEvent(double x, double y, double vx, double vy)
{
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_nweb_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, send_touchpad_fling_event, );

  // Execute
  _struct->send_touchpad_fling_event(_struct,
      x,
      y,
      vx,
      vy);
}

ArkWebNWebCToCpp::ArkWebNWebCToCpp() {}

ArkWebNWebCToCpp::~ArkWebNWebCToCpp() {}

template<>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkWebNWebCToCpp, ArkWebNWeb, ark_web_nweb_t>::kBridgeType = ARK_WEB_NWEB;

} // namespace OHOS::ArkWeb
