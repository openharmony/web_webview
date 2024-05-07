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

#ifndef ARK_WEB_NWEB_CAPI_H_
#define ARK_WEB_NWEB_CAPI_H_
#pragma once

#include "ohos_nweb/capi/ark_web_accessibility_event_callback_capi.h"
#include "ohos_nweb/capi/ark_web_accessibility_node_info_capi.h"
#include "ohos_nweb/capi/ark_web_bool_value_callback_capi.h"
#include "ohos_nweb/capi/ark_web_cache_options_capi.h"
#include "ohos_nweb/capi/ark_web_create_native_media_player_callback_capi.h"
#include "ohos_nweb/capi/ark_web_download_callback_capi.h"
#include "ohos_nweb/capi/ark_web_drag_data_capi.h"
#include "ohos_nweb/capi/ark_web_drag_event_capi.h"
#include "ohos_nweb/capi/ark_web_find_callback_capi.h"
#include "ohos_nweb/capi/ark_web_history_list_capi.h"
#include "ohos_nweb/capi/ark_web_hit_test_result_capi.h"
#include "ohos_nweb/capi/ark_web_js_result_callback_capi.h"
#include "ohos_nweb/capi/ark_web_message_value_callback_capi.h"
#include "ohos_nweb/capi/ark_web_preference_capi.h"
#include "ohos_nweb/capi/ark_web_release_surface_callback_capi.h"
#include "ohos_nweb/capi/ark_web_screen_lock_callback_capi.h"
#include "ohos_nweb/capi/ark_web_string_value_callback_capi.h"
#include "ohos_nweb/include/ark_web_js_proxy_callback_vector.h"
#include "ohos_nweb/include/ark_web_touch_point_info_vector.h"

#include "base/capi/ark_web_base_ref_counted_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_handler_t ark_web_handler_t;

typedef struct _ark_web_nweb_t {
    /**
     * @brief Base structure.
     */
    ark_web_base_ref_counted_t base;

    void(ARK_WEB_CALLBACK* resize)(struct _ark_web_nweb_t* self, uint32_t width, uint32_t height, bool is_keyboard);

    void(ARK_WEB_CALLBACK* on_pause)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_continue)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_destroy)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_focus)(struct _ark_web_nweb_t* self, const int32_t* focus_reason);

    void(ARK_WEB_CALLBACK* on_blur)(struct _ark_web_nweb_t* self, const int32_t* blur_reason);

    void(ARK_WEB_CALLBACK* on_touch_press)(
        struct _ark_web_nweb_t* self, int32_t id, double x, double y, bool from_overlay);

    void(ARK_WEB_CALLBACK* on_touch_release)(
        struct _ark_web_nweb_t* self, int32_t id, double x, double y, bool from_overlay);

    void(ARK_WEB_CALLBACK* on_touch_move1)(
        struct _ark_web_nweb_t* self, int32_t id, double x, double y, bool from_overlay);

    void(ARK_WEB_CALLBACK* on_touch_move2)(
        struct _ark_web_nweb_t* self, const ArkWebTouchPointInfoVector* touch_point_infos, bool from_overlay);

    void(ARK_WEB_CALLBACK* on_touch_cancel)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_navigate_back)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* send_key_event)(struct _ark_web_nweb_t* self, int32_t key_code, int32_t key_action);

    void(ARK_WEB_CALLBACK* send_mouse_wheel_event)(
        struct _ark_web_nweb_t* self, double x, double y, double delta_x, double delta_y);

    void(ARK_WEB_CALLBACK* send_mouse_event)(
        struct _ark_web_nweb_t* self, int x, int y, int button, int action, int count);

    int(ARK_WEB_CALLBACK* load1)(struct _ark_web_nweb_t* self, const ArkWebString* url);

    bool(ARK_WEB_CALLBACK* is_navigatebackward_allowed)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* is_navigate_forward_allowed)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* can_navigate_back_or_forward)(struct _ark_web_nweb_t* self, int num_steps);

    void(ARK_WEB_CALLBACK* navigate_back)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* navigate_forward)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* navigate_back_or_forward)(struct _ark_web_nweb_t* self, int step);

    void(ARK_WEB_CALLBACK* delete_navigate_history)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* reload)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* zoom)(struct _ark_web_nweb_t* self, float zoom_factor);

    int(ARK_WEB_CALLBACK* zoom_in)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* zoom_out)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* stop)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* execute_java_script1)(struct _ark_web_nweb_t* self, const ArkWebString* code);

    void(ARK_WEB_CALLBACK* execute_java_script2)(struct _ark_web_nweb_t* self, const ArkWebString* code,
        ark_web_message_value_callback_t* callback, bool extention);

    ark_web_preference_t*(ARK_WEB_CALLBACK* get_preference)(struct _ark_web_nweb_t* self);

    unsigned int(ARK_WEB_CALLBACK* get_web_id)(struct _ark_web_nweb_t* self);

    ark_web_hit_test_result_t*(ARK_WEB_CALLBACK* get_hit_test_result)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* put_background_color)(struct _ark_web_nweb_t* self, int color);

    void(ARK_WEB_CALLBACK* initial_scale)(struct _ark_web_nweb_t* self, float scale);

    void(ARK_WEB_CALLBACK* put_download_callback)(
        struct _ark_web_nweb_t* self, ark_web_download_callback_t* download_listener);

    void(ARK_WEB_CALLBACK* put_accessibility_event_callback)(
        struct _ark_web_nweb_t* self, ark_web_accessibility_event_callback_t* accessibility_event_listener);

    void(ARK_WEB_CALLBACK* put_accessibility_id_generator)(
        struct _ark_web_nweb_t* self, const AccessibilityIdGenerateFunc accessibility_id_generator);

    void(ARK_WEB_CALLBACK* set_nweb_handler)(struct _ark_web_nweb_t* self, ark_web_handler_t* handler);

    ArkWebString(ARK_WEB_CALLBACK* title)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* page_load_progress)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* content_height)(struct _ark_web_nweb_t* self);

    float(ARK_WEB_CALLBACK* scale)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* load2)(
        struct _ark_web_nweb_t* self, const ArkWebString* url, const ArkWebStringMap* additional_http_headers);

    int(ARK_WEB_CALLBACK* load_with_data_and_base_url)(struct _ark_web_nweb_t* self, const ArkWebString* base_url,
        const ArkWebString* data, const ArkWebString* mime_type, const ArkWebString* encoding,
        const ArkWebString* history_url);

    int(ARK_WEB_CALLBACK* load_with_data)(struct _ark_web_nweb_t* self, const ArkWebString* data,
        const ArkWebString* mime_type, const ArkWebString* encoding);

    void(ARK_WEB_CALLBACK* register_ark_jsfunction1)(struct _ark_web_nweb_t* self, const ArkWebString* object_name,
        const ArkWebStringVector* method_list, const int32_t object_id);

    void(ARK_WEB_CALLBACK* unregister_ark_jsfunction)(
        struct _ark_web_nweb_t* self, const ArkWebString* object_name, const ArkWebStringVector* method_list);

    void(ARK_WEB_CALLBACK* set_nweb_java_script_result_call_back)(
        struct _ark_web_nweb_t* self, ark_web_js_result_callback_t* callback);

    void(ARK_WEB_CALLBACK* put_find_callback)(struct _ark_web_nweb_t* self, ark_web_find_callback_t* find_listener);

    void(ARK_WEB_CALLBACK* find_all_async)(struct _ark_web_nweb_t* self, const ArkWebString* search_str);

    void(ARK_WEB_CALLBACK* clear_matches)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* find_next)(struct _ark_web_nweb_t* self, const bool forward);

    void(ARK_WEB_CALLBACK* store_web_archive)(struct _ark_web_nweb_t* self, const ArkWebString* base_name,
        bool auto_name, ark_web_string_value_callback_t* callback);

    ArkWebStringVector(ARK_WEB_CALLBACK* create_web_message_ports)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* post_web_message)(struct _ark_web_nweb_t* self, const ArkWebString* message,
        const ArkWebStringVector* ports, const ArkWebString* target_uri);

    void(ARK_WEB_CALLBACK* close_port)(struct _ark_web_nweb_t* self, const ArkWebString* port_handle);

    void(ARK_WEB_CALLBACK* post_port_message)(
        struct _ark_web_nweb_t* self, const ArkWebString* port_handle, const ArkWebMessage* data);

    void(ARK_WEB_CALLBACK* set_port_message_callback)(
        struct _ark_web_nweb_t* self, const ArkWebString* port_handle, ark_web_message_value_callback_t* callback);

    void(ARK_WEB_CALLBACK* send_drag_event)(struct _ark_web_nweb_t* self, ark_web_drag_event_t* drag_event);

    void(ARK_WEB_CALLBACK* clear_ssl_cache)(struct _ark_web_nweb_t* self);

    ArkWebString(ARK_WEB_CALLBACK* get_url)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* clear_client_authentication_cache)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* update_locale)(
        struct _ark_web_nweb_t* self, const ArkWebString* language, const ArkWebString* region);

    const ArkWebString(ARK_WEB_CALLBACK* get_original_url)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* get_favicon)(struct _ark_web_nweb_t* self, const void** data, size_t* width, size_t* height,
        int* color_type, int* alpha_type);

    void(ARK_WEB_CALLBACK* put_network_available)(struct _ark_web_nweb_t* self, bool available);

    void(ARK_WEB_CALLBACK* has_images)(struct _ark_web_nweb_t* self, ark_web_bool_value_callback_t* callback);

    void(ARK_WEB_CALLBACK* remove_cache)(struct _ark_web_nweb_t* self, bool include_disk_files);

    ark_web_history_list_t*(ARK_WEB_CALLBACK* get_history_list)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* put_release_surface_callback)(
        struct _ark_web_nweb_t* self, ark_web_release_surface_callback_t* release_surface_listener);

    ArkWebUint8Vector(ARK_WEB_CALLBACK* serialize_web_state)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* restore_web_state)(struct _ark_web_nweb_t* self, const ArkWebUint8Vector* state);

    void(ARK_WEB_CALLBACK* page_up)(struct _ark_web_nweb_t* self, bool top);

    void(ARK_WEB_CALLBACK* page_down)(struct _ark_web_nweb_t* self, bool bottom);

    void(ARK_WEB_CALLBACK* scroll_to)(struct _ark_web_nweb_t* self, float x, float y);

    void(ARK_WEB_CALLBACK* scroll_by)(struct _ark_web_nweb_t* self, float delta_x, float delta_y);

    void(ARK_WEB_CALLBACK* slide_scroll)(struct _ark_web_nweb_t* self, float vx, float vy);

    bool(ARK_WEB_CALLBACK* get_cert_chain_der_data)(
        struct _ark_web_nweb_t* self, ArkWebStringVector* cert_chain_data, bool is_single_cert);

    void(ARK_WEB_CALLBACK* set_screen_off_set)(struct _ark_web_nweb_t* self, double x, double y);

    void(ARK_WEB_CALLBACK* set_audio_muted)(struct _ark_web_nweb_t* self, bool muted);

    void(ARK_WEB_CALLBACK* set_should_frame_submission_before_draw)(struct _ark_web_nweb_t* self, bool should);

    void(ARK_WEB_CALLBACK* notify_popup_window_result)(struct _ark_web_nweb_t* self, bool result);

    void(ARK_WEB_CALLBACK* set_audio_resume_interval)(struct _ark_web_nweb_t* self, int32_t resume_interval);

    void(ARK_WEB_CALLBACK* set_audio_exclusive)(struct _ark_web_nweb_t* self, bool audio_exclusive);

    void(ARK_WEB_CALLBACK* register_screen_lock_function)(
        struct _ark_web_nweb_t* self, int32_t window_id, ark_web_screen_lock_callback_t* callback);

    void(ARK_WEB_CALLBACK* un_register_screen_lock_function)(struct _ark_web_nweb_t* self, int32_t window_id);

    void(ARK_WEB_CALLBACK* notify_memory_level)(struct _ark_web_nweb_t* self, int32_t level);

    void(ARK_WEB_CALLBACK* on_webview_hide)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_webview_show)(struct _ark_web_nweb_t* self);

    ark_web_drag_data_t*(ARK_WEB_CALLBACK* get_or_create_drag_data)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* prefetch_page)(
        struct _ark_web_nweb_t* self, const ArkWebString* url, const ArkWebStringMap* additional_http_headers);

    void(ARK_WEB_CALLBACK* set_window_id)(struct _ark_web_nweb_t* self, uint32_t window_id);

    void(ARK_WEB_CALLBACK* on_occluded)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_unoccluded)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* set_token)(struct _ark_web_nweb_t* self, void* token);

    void(ARK_WEB_CALLBACK* set_nested_scroll_mode)(struct _ark_web_nweb_t* self, const int32_t* nested_scroll_mode);

    void(ARK_WEB_CALLBACK* set_enable_lower_frame_rate)(struct _ark_web_nweb_t* self, bool enabled);

    void(ARK_WEB_CALLBACK* set_virtual_key_board_arg)(
        struct _ark_web_nweb_t* self, int32_t width, int32_t height, double keyboard);

    bool(ARK_WEB_CALLBACK* should_virtual_keyboard_overlay)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* set_draw_rect)(
        struct _ark_web_nweb_t* self, int32_t x, int32_t y, int32_t width, int32_t height);

    void(ARK_WEB_CALLBACK* set_draw_mode)(struct _ark_web_nweb_t* self, int32_t mode);

    void*(ARK_WEB_CALLBACK* create_web_print_document_adapter)(
        struct _ark_web_nweb_t* self, const ArkWebString* job_name);

    int(ARK_WEB_CALLBACK* post_url)(
        struct _ark_web_nweb_t* self, const ArkWebString* url, const ArkWebCharVector* post_data);

    void(ARK_WEB_CALLBACK* java_script_on_document_start)(
        struct _ark_web_nweb_t* self, const ArkWebStringVectorMap* script_items);

    void(ARK_WEB_CALLBACK* execute_action)(struct _ark_web_nweb_t* self, int64_t accessibility_id, uint32_t action);

    ark_web_accessibility_node_info_t*(ARK_WEB_CALLBACK* get_focused_accessibility_node_info)(
        struct _ark_web_nweb_t* self, int64_t accessibility_id, bool is_accessibility_focus);

    ark_web_accessibility_node_info_t*(ARK_WEB_CALLBACK* get_accessibility_node_info_by_id)(
        struct _ark_web_nweb_t* self, int64_t accessibility_id);

    ark_web_accessibility_node_info_t*(ARK_WEB_CALLBACK* get_accessibility_node_info_by_focus_move)(
        struct _ark_web_nweb_t* self, int64_t accessibility_id, int32_t direction);

    void(ARK_WEB_CALLBACK* set_accessibility_state)(struct _ark_web_nweb_t* self, bool state);

    bool(ARK_WEB_CALLBACK* need_soft_keyboard)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* discard)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* restore)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* get_security_level)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* call_h5function)(struct _ark_web_nweb_t* self, int32_t routingId, int32_t h5ObjectId,
        const ArkWebString* h5MethodName, const ArkWebValueVector* args);

    bool(ARK_WEB_CALLBACK* is_incognito_mode)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* register_native_ark_jsfunction)(
        struct _ark_web_nweb_t* self, const char* objName, const ArkWebJsProxyCallbackVector* callbacks);

    void(ARK_WEB_CALLBACK* un_register_native_ark_jsfunction)(struct _ark_web_nweb_t* self, const char* objName);

    void(ARK_WEB_CALLBACK* register_native_valide_callback)(
        struct _ark_web_nweb_t* self, const char* webName, const NativeArkWebOnValidCallback callback);

    void(ARK_WEB_CALLBACK* register_native_destroy_callback)(
        struct _ark_web_nweb_t* self, const char* webName, const NativeArkWebOnValidCallback callback);

    void(ARK_WEB_CALLBACK* java_script_on_document_end)(
        struct _ark_web_nweb_t* self, const ArkWebStringVectorMap* script_items);

    void(ARK_WEB_CALLBACK* enable_safe_browsing)(struct _ark_web_nweb_t* self, bool enable);

    bool(ARK_WEB_CALLBACK* is_safe_browsing_enabled)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* set_print_background)(struct _ark_web_nweb_t* self, bool enable);

    bool(ARK_WEB_CALLBACK* get_print_background)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* close_all_media_presentations)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* stop_all_media)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* resume_all_media)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* pause_all_media)(struct _ark_web_nweb_t* self);

    int(ARK_WEB_CALLBACK* get_media_playback_state)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* enable_intelligent_tracking_prevention)(struct _ark_web_nweb_t* self, bool enable);

    bool(ARK_WEB_CALLBACK* is_intelligent_tracking_prevention_enabled)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* start_camera)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* stop_camera)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* close_camera)(struct _ark_web_nweb_t* self);

    ArkWebString(ARK_WEB_CALLBACK* get_last_javascript_proxy_calling_frame_url)(struct _ark_web_nweb_t* self);

    bool(ARK_WEB_CALLBACK* get_pending_size_status)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* scroll_by_ref_screen)(
        struct _ark_web_nweb_t* self, float delta_x, float delta_y, float vx, float vy);

    void(ARK_WEB_CALLBACK* execute_java_script_ext)(struct _ark_web_nweb_t* self, const int fd,
        const size_t scriptLength, ark_web_message_value_callback_t* callback, bool extention);

    void(ARK_WEB_CALLBACK* on_render_to_background)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* on_render_to_foreground)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* precompile_java_script)(struct _ark_web_nweb_t* self, const ArkWebString* url,
        const ArkWebString* script, ark_web_cache_options_t** cacheOptions, ark_web_message_value_callback_t* callback);

    void(ARK_WEB_CALLBACK* on_create_native_media_player)(
        struct _ark_web_nweb_t* self, ark_web_create_native_media_player_callback_t* callback);

    void(ARK_WEB_CALLBACK* drag_resize)(
        struct _ark_web_nweb_t* self, uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width);

    void(ARK_WEB_CALLBACK* on_touch_cancel_by_id)(
        struct _ark_web_nweb_t* self, int32_t id, double x, double y, bool from_overlay);

    int(ARK_WEB_CALLBACK* scale_gesture_change)(
        struct _ark_web_nweb_t* self, double scale, double centerX, double centerY);

    void(ARK_WEB_CALLBACK* inject_offline_resource)(struct _ark_web_nweb_t* self, const ArkWebString* url,
        const ArkWebString* origin, const ArkWebUint8Vector* resource, const ArkWebStringMap* responseHeaders,
        const int type);

    bool(ARK_WEB_CALLBACK* terminate_render_process)(struct _ark_web_nweb_t* self);

    void(ARK_WEB_CALLBACK* suggestion_selected)(struct _ark_web_nweb_t* self, int32_t index);

    void(ARK_WEB_CALLBACK* register_ark_jsfunction2)(struct _ark_web_nweb_t* self, const ArkWebString* object_name,
        const ArkWebStringVector* method_list, const ArkWebStringVector* async_method_list, const int32_t object_id);

    void(ARK_WEB_CALLBACK* send_touchpad_fling_event)(struct _ark_web_nweb_t* self,
                                                      double x, double y, double vx, double vy);
} ark_web_nweb_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_NWEB_CAPI_H_
