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
 
import type UIAbilityContext from 'application.UIAbilityContext';
import type Want from '@ohos.app.ability.Want';

namespace webNativeMessagingExtensionManager {
    static {
        loadLibraryWithPermissionCheck("web_native_messaging_extension_manager_ani_kit.z", "@ohos.web.webNativeMessagingExtensionManager");
    }
    export interface ConnectionNativeInfo {
        connectionId: int;
        bundleName: string;
        extensionOrigin: string;
        extensionPid: int;
    }
  
    export class ConnectionNativeInfoInner implements ConnectionNativeInfo {
        connectionId: int = 0;
        bundleName: string = "";
        extensionOrigin: string = "";
        extensionPid: int  = 0;
    }
  
    export enum NmErrorCode {
        PERMISSION_DENY = 17100203,
        WANT_CONTENT_ERROR = 17100202,
        INNER_ERROR = 17100201
    }

    export interface WebExtensionConnectionCallback {
        onConnect(connection: ConnectionNativeInfo): void;
        onDisconnect(connection: ConnectionNativeInfo): void;
        onFailed(errorCode: NmErrorCode, errorMessage: string): void;
    }

    export class WebExtensionConnectionCallbackInner implements WebExtensionConnectionCallback {
        onConnect(connection: ConnectionNativeInfo): void {};
        onDisconnect(connection: ConnectionNativeInfo): void {};
        onFailed(errorCode: NmErrorCode, errorMessage: string): void {};
    }
    export native function connectNative(context: UIAbilityContext, want: Want, callback: WebExtensionConnectionCallback): int;
    export native function disconnectNativeSync(connectionId: int): void;
    export function disconnectNative(connectionId: int): Promise<void> {
        let pPromise = new Promise<void>((resolve: (v: PromiseLike<void>) => void,
                                      reject: (error: Error) => void): void => {
            let pPromise1 = taskpool.execute(():undefined => {
                disconnectNativeSync(connectionId);
                return undefined});
            pPromise1.then((e :Any): void=>{
                console.log("in Promise then. e is ", e)
                resolve(Promise.resolve());
            }, (err:Error): void => {
                reject(err);
            });
        });
        return pPromise;
    }
}
export default webNativeMessagingExtensionManager;
