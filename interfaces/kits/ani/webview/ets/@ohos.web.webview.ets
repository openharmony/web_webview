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

class Cleaner {
    private ptr: long = 0
    constructor(ptr: long) {
        this.ptr = ptr
    }
    native clean(): void
}

function CleanerCallback(cleaner: Cleaner): void {
    cleaner.clean()
}

let destroyRegister = new FinalizationRegistry<Cleaner>(CleanerCallback)
let unregisterToken = new object()

export default namespace webview {
    export interface RequestInfo {
        url: string;
        method: string;
        formData: string;
    }

    export interface WebHeader {
        headerKey: string;
        headerValue: string;
    }

    export class WebviewController {
        static {
            loadLibrary("webview_ani.z");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr)
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor(webTag?: string);
        native _setNWebId(nwebId: number): void;
        native _setHapPath(hapPath: string): void;

        native static initializeWebEngine(): void;
        native loadUrl(url: string, headers?: Array<WebHeader>): void;

        native onActive(): void;
        native onInactive(): void;
        native getWebId(): number;
    }
}
