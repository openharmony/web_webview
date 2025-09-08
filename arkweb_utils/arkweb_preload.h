/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WEB_WEBVIEW_ARKWEB_UTILS_ARKWEB_PRELOAD_H
#define WEB_WEBVIEW_ARKWEB_UTILS_ARKWEB_PRELOAD_H

#ifndef ARKWEB_EXPORT
#define ARKWEB_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

ARKWEB_EXPORT void ProcessSpawnDlopenMsg();

ARKWEB_EXPORT int ProcessSpawnDlcloseMsg();

#ifdef __cplusplus
}
#endif

#endif  // WEB_WEBVIEW_ARKWEB_UTILS_ARKWEB_PRELOAD_H