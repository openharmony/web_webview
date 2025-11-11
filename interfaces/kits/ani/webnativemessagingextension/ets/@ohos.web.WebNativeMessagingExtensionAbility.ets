/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import ExtensionAbility from '@ohos.app.ability.ExtensionAbility';
import type WebNativeMessagingExtensionContext from './@ohos.web.WebNativeMessagingExtensionContext';

export interface ConnectionInfo {
    connectionId: int;
    bundleName: string;
    extensionOrigin: string;
    fdRead: int;
    fdWrite: int;
}

export class ConnectionInfoInner implements ConnectionInfo {
    connectionId: int = 0;
    bundleName: string = "";
    extensionOrigin: string = "";
    fdRead: int = 0;
    fdWrite: int = 0;
}

export default class WebNativeMessagingExtensionAbility extends ExtensionAbility {
    static {
        loadLibraryWithPermissionCheck("web_native_messaging_extension_context_ani_kit.z", "@ohos.web.WebNativeMessagingExtensionAbility");
    }

    context!: WebNativeMessagingExtensionContext;

    native onConnectNative(info: ConnectionInfo): void;
    native onDisconnectNative(info: ConnectionInfo): void;
    native onDestroy(): void;
}