# web_webview
- [简介](#简介)
- [软件架构](#软件架构)
- [目录](#目录)
- [使用说明](#使用说明)
- [相关仓](#相关仓)
## 简介
nwebview是OpenHarmony web组件的Native引擎，基于Chromium和CEF构建。
## 软件架构
软件架构说明
```
    -----------------------
    |       web组件         |
    -----------------------
    |      nwebview        |
    -----------------------
    |        CEF           |
    -----------------------
    |      Chromium        |
    -----------------------
    |  OpenHarmony基础库    |
    -----------------------
```
* web组件：OpenHarmony的UI组件。
* nwebview：基于CEF构建的OpenHarmony web组件的Native引擎。
* CEF：CEF全称Chromium Embedded Framework，是一个基于Google Chromium 的开源项目。
* Chromium： Chromium是一个由Google主导开发的网页浏览器。以BSD许可证等多重自由版权发行并开放源代码。
 ## 目录
```
.
├── ohos_nweb            # openharmony适配代码
│   ├── include
│   ├── prebuilts        # NWeb.hap，基于third_party_chromium源码仓构建
│   └── src
└── test                 # nwebview测试代码
```
## 使用说明
**此代码仓是openharmony web组件的Native引擎，此处仅提供基于Native接口的测试使用说明。**

1. 测试代码默认不编译，使用测试代码时，需要在代码仓根目录bundle.json中增加nweb_test目标：
```
"sub_component": [
    "//base/web/webview/ohos_nweb:libnweb",
    "//base/web/webview/ohos_nweb:nweb_hap",
    "//base/web/webview/test:nweb_test"
],
```
2. 编译
```
./build.sh --product-name rk3568 --ccache --build-target nweb_test
```
3. 运行，将nweb_test push到RK3568中，连接wifi保证网络可用，设置日期保证开发板与当前系统时间一致
```
nweb_test 1 --width=720 --height=1184 --add-args=--for-test
```
以上命令会默认加载百度首页，--url=xxx.com命令行参数可以设置你想加载的网页。

## 相关仓

[ace_ace_engine](https://gitee.com/openharmony/ace_ace_engine)

[third_party_cef](https://gitee.com/openharmony/third_party_cef)

**[web_webview](https://gitee.com/openharmony/web_webview)**

[third_party_chromium](https://gitee.com/openharmony/third_party_chromium)