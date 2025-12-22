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

import ExtensionContext from 'application.ExtensionContext';
import Want from '@ohos.app.ability.Want';
import StartOptions from '@ohos.app.ability.StartOptions';

class Cleaner {
    public ptr: long = 0;

    constructor(ptr: long) {
        this.ptr = ptr;
    }

    native clean(): void;
}

export function callback(cleaner: Cleaner): void {
    cleaner.clean();
}

let destroyRegister = new FinalizationRegistry<Cleaner>(callback);
let unregisterToken = new object();

export default class WebNativeMessagingExtensionContext extends ExtensionContext {
    static {
        loadLibraryWithPermissionCheck("web_native_messaging_extension_context_ani_kit.z", "@ohos.web.WebNativeMessagingExtensionContext");
    }

    constructor(context: long) {
        if (this.nativeEtsContext == 0) {
            this.nativeEtsContext = context;
        }
        this.registerCleaner(this.nativeEtsContext);
    }

    registerCleaner(ptr: long): void {
        this.cleaner = new Cleaner(ptr);
        destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    unregisterCleaner(): void {
        destroyRegister.unregister(unregisterToken);
    }

    nativeEtsContext: long = 0;
    private cleaner: Cleaner | null = null;

    native startAbilitySync(want: Want, options?: StartOptions): void;
    startAbility(want: Want, options?: StartOptions): Promise<void> {
        let pPromise = new Promise<void>((resolve: (v: PromiseLike<void>) => void,
                                      reject: (error: Error) => void): void => {
            let pPromise1 = taskpool.execute(():undefined => {
                this.startAbilitySync(want, options);
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

    native terminateSelfSync(): void;
    terminateSelf(): Promise<void> {
        let pPromise = new Promise<void>((resolve: (v: PromiseLike<void>) => void,
                                      reject: (error: Error) => void): void => {
            let pPromise1 = taskpool.execute(():undefined => {
                this.terminateSelfSync();
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