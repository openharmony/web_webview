/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef NWEN_SECURITY_OPTIONS_IMPL_H_
#define NWEN_SECURITY_OPTIONS_IMPL_H_

#include "nweb.h"

namespace OHOS::NWeb {

class NWebSecurityOptionsImpl : public NWebSecurityOptions {
public:
    NWebSecurityOptionsImpl(bool disableJIT, bool disableWasm, bool disableWebGL,
        bool disablePDF, bool disableMathML, bool disableSW, bool disableUDP)
        : disable_jit_(disableJIT), disable_wasm_(disableWasm), disable_webgl_(disableWebGL),
          disable_pdf_(disablePDF), disable_mathml_(disableMathML),
          disable_service_worker_(disableSW), disable_udp_(disableUDP) {}

    ~NWebSecurityOptionsImpl() override = default;

    bool GetDisableJITCompilation() override { return disable_jit_; }
    bool GetDisableWebAssembly() override { return disable_wasm_; }
    bool GetDisableWebGL() override { return disable_webgl_; }
    bool GetDisablePDFViewer() override { return disable_pdf_; }
    bool GetDisableMathML() override { return disable_mathml_; }
    bool GetDisableServiceWorker() override { return disable_service_worker_; }
    bool GetDisableNonProxyUDP() override { return disable_udp_; }

private:
    bool disable_jit_ = false;
    bool disable_wasm_ = false;
    bool disable_webgl_ = false;
    bool disable_pdf_ = false;
    bool disable_mathml_ = false;
    bool disable_service_worker_ = false;
    bool disable_udp_ = false;
};

} // namespace OHOS::NWeb

#endif // NWEN_SECURITY_OPTIONS_IMPL_H_
