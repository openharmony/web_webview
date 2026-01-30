/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RES_SCHED_CLIENT_ADAPTER_H
#define RES_SCHED_CLIENT_ADAPTER_H

#include <cstdint>
#include <unistd.h>

namespace OHOS::NWeb {

enum class ResSchedTypeAdapter : int32_t {
    RES_TYPE_KEY_THREAD = 0,
    RES_TYPE_WEB_STATUS_CHANGE,
    RES_TYPE_WEB_SCENE,
    RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE,
    RES_TYPE_WEBVIEW_SCREEN_CAPTURE,
    RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE,
    RES_TYPE_WEB_SUBWIN_CALL_STATUS_CHANGE,
};

enum class ResSchedStatusAdapter : int32_t {
    THREAD_CREATED = 0,
    THREAD_DESTROYED,
    WEB_ACTIVE,
    WEB_INACTIVE,
    WEB_SCENE_ENTER,
    WEB_SCENE_EXIT,
    AUDIO_STATUS_START,
    AUDIO_STATUS_STOP,
    SCREEN_CAPTURE_START,
    SCREEN_CAPTURE_STOP,
    VIDEO_PLAYING_START,
    VIDEO_PLAYING_STOP,
    WEB_SUBWIN_CALL_START,
    WEB_SUBWIN_CALL_STOP,
};

enum class ResSchedRoleAdapter : int32_t {
    USER_INTERACT = 0,
    NORMAL_DISPLAY,
    IMPORTANT_DISPLAY,
    NORMAL_AUDIO,
    IMPORTANT_AUDIO,
    IMAGE_DECODE,
};

enum class ResSchedSceneAdapter : int32_t {
    LOAD_URL = 0,
    CLICK,
    SLIDE,
    RESIZE,
    VISIBLE,
    KEYBOARD_CLICK,
    KEY_TASK,
    IMAGE_DECODE,
};

class ResSchedClientAdapter {
public:
    ResSchedClientAdapter() = default;

    virtual ~ResSchedClientAdapter() = default;

    static bool ReportKeyThread(
        ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid, ResSchedRoleAdapter roleAdapter);
    static bool ReportWindowStatus(
        ResSchedStatusAdapter statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId = -1);
    static bool ReportScene(
        ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter, int32_t nwebId = -1);
    static bool ReportAudioData(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid);
    static void ReportWindowId(int32_t windowId, int32_t nwebId = -1);
    static void ReportNWebInit(ResSchedStatusAdapter statusAdapter, int32_t nwebId);
    static void ReportRenderProcessStatus(ResSchedStatusAdapter statusAdapter, pid_t pid);
    static bool ReportScreenCapture(ResSchedStatusAdapter statusAdapter, pid_t pid);
    static bool ReportVideoPlaying(ResSchedStatusAdapter statusAdapter, pid_t pid);
    static void ReportProcessInUse(pid_t pid);
    static void ReportSiteIsolationMode(bool mode);
    static bool ReportSubwindowCall(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid);
};

} // namespace OHOS::NWeb

#endif // RES_SCHED_CLIENT_ADAPTER_H
