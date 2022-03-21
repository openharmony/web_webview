# web_webview
## Introduction
nwebview is the native engine of the OpenHarmony web component and is built based on Chromium and the Chromium Embedded Framework (CEF).
## Software Architecture
Below is the software architecture.
```
    ------------------------------
    |       Web component         |
    ------------------------------
    |      nwebview               |
    ------------------------------
    |        CEF                  |
    ------------------------------
    |      Chromium               |
    ------------------------------
    |  OpenHarmony base library   |
    ------------------------------
```
* Web component: UI component in OpenHarmony.
* nwebview: native engine of the OpenHarmony web component, which is built based on the Chromium Embedded Framework (CEF).
* CEF: stands for Chromium Embedded Framework. It is an open-source project based on Google Chromium.
* Chromium: an open-source web browser principally developed by Google and released under the BSD license and other permissive open-source licenses.
 ## Directory Structure
```
.
├── ohos_nweb            # OpenHarmony adaptation code
│   ├── include
│   ├── prebuilts        # NWeb.hap, built based on the third_party_chromium source code repository
│   └── src
└── test                 # nwebview test code
```
## Usage
** This code repository is the native engine of the OpenHarmony web component. This topic describes only how to use the test code based on the native interfaces.**

1. By default, the test code is not compiled. Therefore, to use the test code, you need to add the **nweb_test** target to the **bundle.json** file in the root directory of the code repository.
```
"sub_component": [
    "//base/web/webview/ohos_nweb:libnweb",
    "//base/web/webview/ohos_nweb:nweb_hap",
    "//base/web/webview/test:nweb_test"
],
```
2. Build source code.
```
./build.sh --product-name rk3568 --ccache --build-target nweb_test
```
3. Push **nweb_test** to RK3568, connect to a Wi-Fi network, and make sure the time on RK3568 is the same as the current system time.
```
nweb_test 1 --width=720 --height=1184 --add-args=--for-test
```
The preceding command loads the Baidu home page by default. To load another page, run **--url=xxx.com**.

## Repositories Involved

[ace_ace_engine](https://gitee.com/openharmony/ace_ace_engine)

[third_party_cef](https://gitee.com/openharmony/third_party_cef)

**[web_webview](https://gitee.com/openharmony/web_webview)**

[third_party_chromium](https://gitee.com/openharmony/third_party_chromium)
