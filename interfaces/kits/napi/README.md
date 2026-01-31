# interfaces/kits/napi - ArkWeb NAPI (ArkTS) 接口

本目录提供 ArkWeb 的 NAPI (Native API) 接口实现,允许 ArkTS应用直接使用 WebView 功能。

## 概述

### 核心功能

NAPI 接口提供了以下能力:
- **ArkTS/JS 绑定** - 为ArkTS应用提供 WebView API
- **面向对象设计** - 将 C++ 对象映射为 JS 类
- **异步回调支持** - 支持 Promise 和 Callback 两种异步模式
- **类型安全** - 通过 NAPI 提供类型检查和转换
- **事件机制** - 支持事件监听和派发

### 适用场景

- ✅ ArkTS 应用开发 (OpenHarmony主要开发语言)
- ✅ 需要与 ArkUI 框架集成的场景
- ✅ 跨语言调用 (ArkTS → C++)

## 架构设计

### 整体架构

```
ArkTS 应用层
  ↓
NAPI 绑定层 (interfaces/kits/napi/)
  ↓ napi_wrap/napi_unwrap
nativecommon 通用组件层 (interfaces/kits/nativecommon/)
  ↓ 引用计数管理、数据结构转换、错误处理
ohos_nweb/ 核心引擎层
  ↓ ohos_interface/include/ohos_nweb/
ArkWebCore.hap
```

**架构说明**:
- **NAPI 绑定层**: 实现 ArkTS 与 C++ 的绑定
- **nativecommon 层**: 提供通用组件和数据结构封装,管理对象生命周期
- **核心引擎层**: 提供 WebView 核心功能

### 核心模块

| 模块 | 说明 | 主要类 | 使用 nativecommon |
|------|------|--------|------------------|
| **webviewcontroller** | WebView 控制器核心 | WebviewController, WebMessagePort | ✅ WebHistoryList, WebMessagePort |
| **webcookiemanager** | Cookie 管理 | WebCookieManager | ❌ |
| **webdatabase** | 数据库管理 | WebDataBase, WebStorage | ❌ |
| **webasynccontroller** | 异步控制器 | WebAsyncController | ❌ |
| **proxycontroller** | 代理控制 | ProxyController, ProxyConfig | ❌ |
| **webadsblockmanager** | 广告拦截 | WebAdsBlockManager | ❌ |
| **web_native_messaging** | 原生消息扩展 | WebNativeMessagingExtension | ❌ |

**说明**: 标记 ✅ 的模块使用 nativecommon 提供的数据结构和工具类。

### NAPI 绑定机制

#### 1. 类绑定流程

```cpp
// 1. 定义 JS 类名和构造函数
const std::string WEBVIEW_CONTROLLER_CLASS_NAME = "WebviewController";

// 2. Init() 函数中定义类属性和方法
napi_value NapiWebviewController::Init(napi_env env, napi_value exports)
{
    // 定义类属性
    napi_property_descriptor properties[] = {
        // 声明式方法 (在 JS 中使用)
        DECLARE_NAPI_FUNCTION("loadUrl", LoadUrl),
        DECLARE_NAPI_FUNCTION("runJavaScript", RunJavaScript),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
        // ... 更多方法
    };

    // 定义类
    napi_value constructor = nullptr;
    napi_define_class(
        env,
        "WebviewController",        // JS 类名
        NAPI_AUTO_LENGTH,            // 类名长度
        JsConstructor,               // C++ 构造函数
        nullptr,                     // 用户数据
        sizeof(properties) / sizeof(properties[0]),
        properties,                  // 属性数组
        &constructor                 // 输出构造函数
    );

    // 导出类
    napi_set_named_property(env, exports, "WebviewController", constructor);
    return exports;
}
```

#### 2. 对象包装机制

**基础对象包装** (直接 NAPI 对象):
```cpp
// C++ 对象包装到 JS 对象
WebviewController* webviewController = new WebviewController();
napi_wrap(
    env,
    thisVar,                         // JS 对象
    webviewController,               // C++ 对象指针
    nullptr,                         // 对象回收提示
    nullptr,                         // 环境回收提示
    nullptr                          // 包装数据
);

// 从 JS 对象解包 C++ 对象
WebviewController* webviewController = nullptr;
napi_unwrap(
    env,
    thisVar,                         // JS 对象
    (void**)&webviewController       // 输出 C++ 对象指针
);
```

**nativecommon 对象包装** (带引用计数):
```cpp
// 1. 创建 nativecommon 对象 (继承 TransferReferencedCount)
WebHistoryList* historyList = new WebHistoryList(sptrHistoryList);
historyList->IncRefCount();  // 增加引用计数

// 2. 包装到 JS 对象,并设置 deleter
napi_wrap(
    env,
    jsObject,                       // JS 对象
    historyList,                    // nativecommon 对象指针
    [](napi_env env, void* data, void* hint) {  // deleter
        // 对象被 GC 时调用
        WebHistoryList* obj = static_cast<WebHistoryList*>(data);
        if (obj->DecRefCount() == 0) {  // 减少引用计数
            delete obj;  // 引用计数为 0 时释放
        }
    },
    nullptr,
    nullptr
);

// 3. 在其他地方使用时需要增加引用
historyList->IncRefCount();
```

**引用计数管理** (来自 nativecommon):
- `TransferReferencedCount` 基类提供引用计数功能
- `IncRefCount()`: 对象传递时增加引用
- `DecRefCount()`: 对象释放时减少引用,返回剩余计数
- 计数为 0 时自动释放对象,防止内存泄漏和悬空指针

#### 3. 模块注册机制

**模块注册入口**: `napi_webview_native_module.cpp`

```cpp
// 1. 导出所有类
static napi_value WebViewExport(napi_env env, napi_value exports)
{
    NapiWebDataBase::Init(env, exports);           // WebDataBase
    NapiWebStorage::Init(env, exports);            // WebStorage
    NapiWebviewController::Init(env, exports);     // WebviewController
    NapiWebCookieManager::Init(env, exports);      // WebCookieManager
    NapiProxyController::Init(env, exports);       // ProxyController
    // ... 更多类
    return exports;
}

// 2. 定义模块
static napi_module _module = {
    .nm_version = 1,
    .nm_register_func = WebViewExport,             // 注册函数
    .nm_modname = "web.webview",                   // 模块名
};

// 3. 自动注册 (构造函数属性)
extern "C" __attribute__((constructor)) void Register()
{
    napi_module_register(&_module);
}
```

### JS 层导出机制

**文件**: `js/webview_export.js`

```javascript
// 导入 C++ NAPI 模块
let webview = requireInternal('web.webview');

// 在 JS 层扩展功能
export class WebviewController {
    // JS 层的方法会调用 C++ 层的实现
    loadUrl(url) {
        // 参数验证
        // 调用 C++ 实现
    }
}

// 导出模块
export default {
    WebviewController,
    WebCookieManager,
    // ... 其他类
};
```

## 核心模块详解

### 1. WebviewController (核心控制器)

**文件**: `webviewcontroller/napi_webview_controller.cpp` (347,820 行)

**主要功能**:
- 页面导航 (LoadUrl, Backward, Forward, Refresh)
- JavaScript 执行 (RunJavaScript)
- 消息通信 (CreateWebMessagePorts, PostMessage)
- 页面控制 (Zoom, Scroll, Stop)
- 状态查询 (GetTitle, GetUrl, GetProgress)

**核心方法示例**:

```cpp
// LoadUrl 实现
static napi_value LoadUrl(napi_env env, napi_callback_info info)
{
    // 1. 获取 JS 对象
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    // 2. 解包 C++ 对象
    WebviewController* webviewController = nullptr;
    napi_unwrap(env, thisVar, (void**)&webviewController);

    // 3. 解析参数
    std::string url;
    NapiParseUtils::ParseString(env, argv[0], url);

    // 4. 调用核心引擎
    ErrCode errCode = webviewController->LoadUrl(url, httpHeaders);

    // 5. 返回结果
    return nullptr;
}
```

**ArkTS 使用示例**:

```typescript
import webview from '@ohos.web.webview';

let controller = new webview.WebviewController();
controller.loadUrl('https://www.example.com');

// 执行 JavaScript
controller.runJavaScript('console.log("Hello from ArkTS")')
    .then(result => {
        console.log('JS Result: ' + result);
    });

// 监听事件
controller.on('pageBegin', (event) => {
    console.log('Page started loading: ' + event.url);
});
```

### 2. WebMessagePort (消息端口)

**文件**: `webviewcontroller/napi_webview_controller.h` (NapiWebMessagePort 类)

**主要功能**:
- 端口创建 (CreateWebMessagePorts)
- 消息发送 (PostMessageEvent)
- 消息接收 (OnMessageEvent)
- 端口关闭 (Close)

**使用流程**:

```cpp
// 1. 创建消息端口对
static napi_value CreateWebMessagePorts(napi_env env, napi_callback_info info)
{
    // 调用核心引擎创建端口
    std::vector<std::shared_ptr<WebMessagePort>> ports;
    webviewController->CreateWebMessagePorts(ports);

    // 包装为 JS 对象
    for (auto& port : ports) {
        napi_value portObj = CreatePortObject(env, port);
        // ... 添加到数组
    }

    return portArray;
}
```

### 3. WebCookieManager (Cookie 管理)

**文件**: `webcookiemanager/napi_web_cookie_manager.cpp`

**主要功能**:
- Cookie 获取 (FetchCookie)
- Cookie 设置 (ConfigCookie)
- Cookie 清除 (ClearAllCookies)

### 4. ProxyController (代理控制器)

**文件**: `proxycontroller/napi_proxy_controller.cpp`

**主要功能**:
- 代理规则设置
- 代理配置管理
- PAC (Proxy Auto-Config) 支持

## NAPI 数据类型转换

### 基本类型转换

| JS 类型 | C++ 类型 | 转换函数 |
|---------|---------|---------|
| `string` | `std::string` | `napi_get_value_string_utf8()` |
| `number` | `int32_t`, `double` | `napi_get_value_int32()`, `napi_get_value_double()` |
| `boolean` | `bool` | `napi_get_value_bool()` |
| `object` | 自定义结构 | `napi_get_named_property()` |
| `array` | `std::vector<T>` | 自定义解析函数 |

### 复杂类型转换示例

```cpp
// 字符串转换
std::string url;
NapiParseUtils::ParseString(env, argv[0], url);

// 布尔数组转换
std::vector<bool> boolArray;
NapiParseUtils::ParseBooleanArray(env, argv[0], boolArray);

// 字符串数组转换
std::vector<std::string> stringArray;
NapiParseUtils::ParseStringArray(env, argv[0], stringArray);

// 对象解析
napi_value configObj = argv[0];
int32_t timeout = 0;
napi_value timeoutValue;
napi_get_named_property(env, configObj, "timeout", &timeoutValue);
napi_get_value_int32(env, timeoutValue, &timeout);
```

### 回调函数处理

```cpp
// 1. 保存 JS 回调
napi_value callback = argv[1];
napi_ref callbackRef;
napi_create_reference(env, callback, 1, &callbackRef);

// 2. 异步调用回调 (使用 uv_async)
struct CallbackData {
    napi_env env;
    napi_ref callbackRef;
    std::string result;
};

uv_work_t* work = new uv_work_t;
CallbackData* callbackData = new CallbackData{env, callbackRef, result};
work->data = callbackData;

uv_queue_work(uv_default_loop(), work,
    [](uv_work_t *work) {},  // 工作函数
    [](uv_work_t *work, int status) {  // 完成回调
        CallbackData* data = static_cast<CallbackData*>(work->data);
        napi_value callback, result;
        napi_get_reference_value(data->env, data->callbackRef, &callback);
        napi_create_string_utf8(data->env, data->result.c_str(),
                               data->result.length(), &result);
        napi_call_function(data->env, nullptr, callback, 1, &result, nullptr);
    }
);
```

## 异步编程模型

### Promise 模式

```cpp
static napi_value RunJavaScript(napi_env env, napi_callback_info info)
{
    // 1. 创建 Promise
    napi_value promise;
    napi_deferred deferred;
    napi_create_promise(env, &deferred, &promise);

    // 2. 创建回调实现
    auto callback = std::make_shared<NWebValueCallbackImpl>(
        env, deferred, true);

    // 3. 调用核心引擎
    webviewController->RunJavaScript(script, callback);

    // 4. 返回 Promise
    return promise;
}
```

### Callback 模式

```cpp
static napi_value OnPageBegin(napi_env env, napi_callback_info info)
{
    // 1. 获取回调函数
    napi_value callback = argv[0];
    napi_ref callbackRef;
    napi_create_reference(env, callback, 1, &callbackRef);

    // 2. 设置事件监听
    webviewController->SetPageBeginHandler(
        [env, callbackRef](const std::string& url) {
            napi_value callback, jsUrl;
            napi_get_reference_value(env, callbackRef, &callback);
            napi_create_string_utf8(env, url.c_str(), url.length(), &jsUrl);
            napi_call_function(env, nullptr, callback, 1, &jsUrl, nullptr);
        }
    );

    return nullptr;
}
```

## 错误处理机制

### 错误码定义

```cpp
// business_error.h
enum ErrorCode {
    NO_ERROR = 0,
    PARAM_CHECK_ERROR = 401,
    UNKNOWN_ERROR = -1,
    NAPI_ERRNO_MAX = 1000000
};
```

### 错误抛出

```cpp
// 参数检查错误
if (url.empty() || url.length() > URL_MAXIMUM) {
    BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
    return nullptr;
}

// 自定义错误消息
napi_throw_error(env, nullptr, "Invalid URL format");

// 类型错误
napi_throw_type_error(env, nullptr, "Expected string parameter");
```

### JS 层错误处理

```javascript
try {
    controller.loadUrl(invalidUrl);
} catch (error) {
    console.error('Error code: ' + error.code);
    console.error('Error message: ' + error.message);
}

// Promise 错误处理
controller.runJavaScript(script)
    .then(result => console.log(result))
    .catch(error => console.error(error.code, error.message));
```

## 事件系统

### 事件注册机制

```cpp
// on() 方法实现
static napi_value On(napi_env env, napi_callback_info info)
{
    // 1. 解析事件名和回调
    std::string eventName;
    NapiParseUtils::ParseString(env, argv[0], eventName);

    napi_value callback = argv[1];
    napi_ref callbackRef;
    napi_create_reference(env, callback, 1, &callbackRef);

    // 2. 注册事件监听
    webviewController->On(eventName, [env, callbackRef](const Event& event) {
        // 触发回调
    });

    return nullptr;
}
```

### 主要事件类型

| 事件名 | 说明 | 触发时机 |
|--------|------|---------|
| `onPageBegin` | 页面开始加载 | LoadUrl 调用后 |
| `onPageEnd` | 页面加载完成 | 页面内容加载完毕 |
| `onErrorReceive` | 加载错误发生 | 网络错误、资源加载失败 |
| `onHttpErrorReceive` | HTTP 错误 | HTTP 4xx/5xx 错误 |
| `onControllerAttached` | 控制器附加 | WebviewController 绑定到 Web 组件 |
| `onRenderProcessGone` | 渲染进程终止 | 渲染进程崩溃 |

## 构建和集成

### 构建配置

**BUILD.gn**:
```gni
ohos_shared_library("webview_napi") {
    sources = [
        "webviewcontroller/napi_webview_controller.cpp",
        "webcookiemanager/napi_web_cookie_manager.cpp",
        # ... 更多源文件
    ]

    deps = [
        "../../../ohos_nweb:libnweb",
        "../native:ohweb",
    ]

    external_deps = [
        "napi:ace_napi",
        "hilog:libhilog",
    ]

    relative_install_dir = "module/web"
    part_name = "webview"
}
```

### 构建命令

```bash
# 构建 NAPI 模块
./build.sh --product-name <产品名> --build-target //base/web/webview/interfaces/kits/napi:webview_napi

# 输出
# libwebview_napi.so (安装到 /usr/lib/module/web/)
```

### ArkTS 集成示例

```typescript
// import.ets
import webview from '@ohos.web.webview';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src: 'https://www.example.com', controller: this.controller })
        .onPageBegin(() => {
          console.log('Page started loading');
        })
        .onPageEnd(() => {
          console.log('Page loaded');
        });
    }
  }
}
```

### 依赖关系

**依赖库**:
- `libnweb.so` - 核心 WebView 引擎
- `libohweb.so` - Native 接口库
- `libace_napi.z.so` - NAPI 框架
- `libhilog.so` - 日志系统

**系统依赖**:
- OpenHarmony SDK API 9+
- ArkTS 编译器
- ACE (ArkUI) 引擎

## 与 nativecommon 的关系

### nativecommon 的作用

`nativecommon` 是 NAPI 层和核心引擎层之间的桥梁,提供以下关键功能:

#### 1. 数据结构封装

nativecommon 将核心引擎的数据结构适配为 NAPI 可用的格式:

```cpp
// 核心引擎类型
std::shared_ptr<NWebHistoryList>

// nativecommon 封装
WebHistoryList (继承 TransferReferencedCount)

// NAPI 层使用
napi_wrap(env, jsObject, historyList, ...)
```

**常见封装类型**:
- `WebHistoryList` - 历史记录列表
- `WebMessagePort` - 消息端口
- `WebSchemeHandlerRequest` - 自定义协议请求
- `WebSchemeHandlerResponse` - 自定义协议响应
- `NWebUserAgentMetadataImpl` - User-Agent 元数据

#### 2. 引用计数管理

为跨语言传输的对象提供生命周期管理:

```cpp
class TransferReferencedCount {
    void IncRefCount();   // napi_wrap 时调用
    int32_t DecRefCount(); // deleter 中调用
    int32_t RefCount();    // 查询当前计数
};
```

**防止的问题**:
- ❌ 内存泄漏 (对象未释放)
- ❌ 悬空指针 (对象提前释放)
- ❌ 重复释放 (double free)

#### 3. 错误处理系统

统一的错误码定义和错误消息格式化:

```cpp
// 使用 nativecommon 错误码
if (url.empty()) {
    return NWebError::INVALID_URL;  // 17100002
}

// 获取错误消息
std::string errMsg = NWebError::GetErrMsgByErrCode(NWebError::INVALID_URL);

// 在 NAPI 中抛出错误
BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
```

**常用错误码**:
- `NO_ERROR = 0` - 成功
- `PARAM_CHECK_ERROR = 401` - 参数错误
- `INVALID_URL = 17100002` - 无效 URL
- `INIT_ERROR = 17100001` - 初始化错误

### 数据流转示例

以 `getBackForwardEntries()` 为例:

```javascript
// 1. ArkTS 调用
let history = controller.getBackForwardEntries();
```

```cpp
// 2. NAPI 层 (napi_webview_controller.cpp)
static napi_value GetBackForwardEntries(napi_env env, napi_callback_info info)
{
    // 解包控制器对象
    WebviewController* webviewController = nullptr;
    napi_unwrap(env, thisVar, (void**)&webviewController);

    // 调用核心引擎,获取 std::shared_ptr<NWebHistoryList>
    std::shared_ptr<NWebHistoryList> nwebHistoryList =
        webviewController->GetBackForwardEntries();

    // 创建 nativecommon 封装对象
    WebHistoryList* historyList = new WebHistoryList(nwebHistoryList);
    historyList->IncRefCount();  // 增加引用计数

    // 包装为 JS 对象
    napi_value jsHistoryList;
    napi_create_object(env, &jsHistoryList);
    napi_wrap(env, jsHistoryList, historyList,
        [](napi_env env, void* data, void* hint) {
            WebHistoryList* obj = static_cast<WebHistoryList*>(data);
            if (obj->DecRefCount() == 0) {
                delete obj;
            }
        }, nullptr, nullptr);

    return jsHistoryList;
}
```

```cpp
// 3. nativecommon 层 (web_history_list.cpp)
int32_t WebHistoryList::GetCurrentIndex()
{
    if (!sptrHistoryList_) {  // 检查 shared_ptr
        return -1;
    }
    return sptrHistoryList_->GetCurrentIndex();
}
```

```cpp
// 4. 核心引擎层 (ohos_nweb)
std::shared_ptr<NWebHistoryList> WebviewController::GetBackForwardEntries()
{
    if (!nweb_) {
        return nullptr;
    }
    return nweb_->GetBackForwardEntries();
}
```

### 何时使用 nativecommon

**需要使用 nativecommon 的场景**:
- ✅ 返回复杂数据结构 (历史记录、消息端口等)
- ✅ 需要跨语言边界传递对象
- ✅ 需要引用计数管理生命周期
- ✅ 使用统一错误码系统

**不需要使用 nativecommon 的场景**:
- ❌ 简单类型转换 (string, number, boolean)
- ❌ 直接调用核心引擎 API
- ❌ 不需要共享的临时对象

### 相关文档

详见: **[../nativecommon/README.md](../nativecommon/README.md)** - nativecommon 通用组件库完整文档

## 与 Native 接口对比

| 特性 | NAPI 接口 | Native 接口 |
|------|-----------|------------|
| **语言** | ArkTS | C++ |
| **使用场景** | UI 应用开发 | 高性能场景、游戏引擎 |
| **类型安全** | 运行时检查 | 编译时检查 |
| **性能** | 有跨语言开销 | 零拷贝,高性能 |
| **易用性** | 高 (符合 JS 开发习惯) | 中 (需要 C++ 开发能力) |
| **异步支持** | Promise/Callback | 回调函数 |
| **API 丰富度** | 完整 | 完整 |

**选择建议**:
- ✅ **使用 NAPI 接口**: ArkTS 应用、UI 开发、常规业务逻辑
- ✅ **使用 Native 接口**: C++ 应用、游戏引擎、高性能场景

## 性能优化

### 1. 减少跨语言调用

```typescript
// ❌ 频繁调用 (性能差)
for (let i = 0; i < 1000; i++) {
    controller.runJavaScript(`setValue(${i})`);
}

// ✅ 批量调用 (性能好)
controller.runJavaScript(`
    for (let i = 0; i < 1000; i++) {
        setValue(i);
    }
`);
```

### 2. 使用 Promise 并行

```typescript
// ❌ 串行执行
await controller.runJavaScript(script1);
await controller.runJavaScript(script2);

// ✅ 并行执行
await Promise.all([
    controller.runJavaScript(script1),
    controller.runJavaScript(script2)
]);
```

### 3. 缓存对象引用

```typescript
// ✅ 复用 controller 对象
class MyComponent {
    private controller = new webview.WebviewController();

    aboutToAppear() {
        this.controller.loadUrl(url);
    }
}
```

## 调试技巧

### 日志查看

```bash
# 查看 NAPI 日志
hilog -T ArkWeb

# 查看 JS 层日志
console.log('Debug info');
```

### 常见问题

**Q: NAPI 模块加载失败**

可能原因:
1. libwebview_napi.so 未安装
2. 依赖库缺失
3. SDK 版本不匹配

解决方法:
```bash
# 检查库文件是否存在
ls -l /usr/lib/module/web/libwebview_napi.so

# 检查依赖
ldd /usr/lib/module/web/libwebview_napi.so
```

**Q: 对象方法调用崩溃**

可能原因:
1. napi_unwrap 失败
2. C++ 对象已释放
3. 类型转换错误

解决方法:
```cpp
// 始终检查解包结果
WebviewController* controller = nullptr;
napi_status status = napi_unwrap(env, thisVar, (void**)&controller);
if (status != napi_ok || !controller) {
    napi_throw_error(env, nullptr, "Invalid controller object");
    return nullptr;
}
```

**Q: 回调函数未执行**

可能原因:
1. 未正确保存 napi_ref
2. 线程问题
3. 环境已销毁

解决方法:
```cpp
// 使用 uv_async 确保在主线程执行
uv_async_t* async = new uv_async_t;
async->data = callbackData;
uv_async_init(uv_default_loop(), async, AsyncCallback);
uv_async_send(async);
```

## 最佳实践

### 1. 参数验证

```cpp
static napi_value LoadUrl(napi_env env, napi_callback_info info)
{
    // 1. 检查参数数量
    if (argc < 1) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    // 2. 检查参数类型
    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_string) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    // 3. 验证参数值
    std::string url;
    NapiParseUtils::ParseString(env, argv[0], url);
    if (url.empty() || url.length() > URL_MAXIMUM) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    // ... 处理逻辑
}
```

### 2. 内存管理

```cpp
// JsConstructor 中分配内存
static napi_value JsConstructor(napi_env env, napi_callback_info info)
{
    WebviewController* controller = new WebviewController();

    // 使用 napi_wrap 自动管理内存
    // 当 JS 对象被 GC 时,自动调用 deleter
    napi_wrap(
        env,
        thisVar,
        controller,
        [](napi_env env, void* data, void* hint) {
            // deleter: 对象销毁时调用
            delete static_cast<WebviewController*>(data);
        },
        nullptr,
        nullptr
    );

    return thisVar;
}
```

### 3. 异常处理

```cpp
static napi_value RunJavaScript(napi_env env, napi_callback_info info)
{
    try {
        // 处理逻辑
        std::string script;
        NapiParseUtils::ParseString(env, argv[0], script);

        if (script.empty()) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return nullptr;
        }

        // 调用核心引擎
        auto result = webviewController->RunJavaScript(script);

        // 返回结果
        return CreateResult(env, result);

    } catch (const std::exception& e) {
        napi_throw_error(env, nullptr, e.what());
        return nullptr;
    }
}
```

### 4. 生命周期管理

```typescript
// ArkTS 中正确管理生命周期
@Entry
@Component
struct WebComponent {
    controller: webview.WebviewController = new webview.WebviewController();

    aboutToAppear() {
        // 组件出现时创建
    }

    aboutToDisappear() {
        // 组件销毁时清理
        // 注意: controller 对象会被 JS 引擎自动 GC
    }

    build() {
        Web({ src: 'https://example.com', controller: this.controller })
    }
}
```

## 相关文档

- **[../nativecommon/README.md](../nativecommon/README.md)** - nativecommon 通用组件库文档
- **[../../native/README.md](../../native/README.md)** - Native C++ 接口文档
- **[../../../ohos_nweb/README.md](../../../ohos_nweb/README.md)** - 核心引擎文档
- **[../../../AGENT.md](../../../AGENT.md)** - 代码仓指南
- **[../../../ohos_nweb/HOW_TO_ADD_XML_CONFIG.md](../../../ohos_nweb/HOW_TO_ADD_XML_CONFIG.md)** - XML 配置指南
- **[../../../ohos_nweb/HOW_TO_ADD_PARAM_CONFIG.md](../../../ohos_nweb/HOW_TO_ADD_PARAM_CONFIG.md)** - PARAM 配置指南
