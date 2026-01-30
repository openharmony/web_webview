# interfaces/kits/ani - ArkWeb ANI (Ark Native Interface™) 接口

本目录提供 ArkWeb 的 ANI (Ark Native Interface™) 接口实现,为 ArkTS 应用提供高性能的 C++ 绑定机制,是继 NAPI 之后的下一代跨语言接口方案。

## 概述

### 核心功能

ANI 接口提供了以下能力:
- **高性能绑定** - 比 NAPI 更高效的 ArkTS ↔ C++ 绑定
- **类型安全** - 编译时类型检查,减少运行时错误
- **零拷贝优化** - 最小化数据拷贝,提升性能
- **异步支持** - 原生支持 Promise 和异步回调
- **面向对象** - 完整的类和方法绑定支持

### 适用场景

- ✅ 需要 NAPI 接口但性能要求更高的场景
- ✅ ArkTS 应用中需要调用 C++ 功能
- ✅ 复杂数据结构的高效传递
- ✅ 新项目或可迁移到 ANI 的现有项目

### ANI vs NAPI

| 特性 | ANI | NAPI |
|------|-----|------|
| **性能** | 更高 (优化过的绑定机制) | 标准 |
| **类型安全** | 编译时检查 | 运行时检查 |
| **API 稳定性** | 较新,持续演进 | 成熟稳定 |
| **学习曲线** | 稍陡 | 平缓 |
| **文档和生态** | 发展中 | 完善 |
| **使用场景** | 性能关键路径 | 通用场景 |

## 架构设计

### 整体架构

```
ArkTS/ETS 应用层
  ↓
ANI 绑定层 (interfaces/kits/ani/)
  ↓ ani_ref/ani_value
nativecommon 通用组件层 (interfaces/kits/nativecommon/)
  ↓ 引用计数管理、数据结构转换
ohos_nweb/ 核心引擎层
  ↓ ohos_interface/include/ohos_nweb/
ArkWebCore.hap
```

**架构说明**:
- **ANI 绑定层**: 实现高性能的 ArkTS ↔ C++ 绑定
- **nativecommon 层**: 提供通用组件和数据结构封装 (与 NAPI 共享)
- **核心引擎层**: 提供 WebView 核心功能

### 目录结构

```
interfaces/kits/ani/
├── BUILD.gn                    # 构建配置
├── webview/                    # WebView 主要实现
│   ├── BUILD.gn                # webview ANI 库构建配置
│   ├── ets/                    # ArkTS/ETS 类型定义
│   │   ├── @ohos.web.webview.ets
│   │   ├── @ohos.web.permissionRequest.ets
│   │   └── @ohos.web.fileSelector.ets
│   ├── native/                 # C++ 实现文件 (共享)
│   │   ├── common/             # 通用工具类
│   │   ├── webviewcontroller/  # 控制器实现
│   │   └── webfunction/        # 功能函数
│   └── src/                    # ANI 特定实现
│       ├── common/             # ANI 工具类
│       ├── webviewcontroller/  # ANI 控制器绑定
│       ├── webcookiemanager/   # Cookie 管理
│       ├── webdatabase/        # 数据库
│       ├── webstorage/         # 存储
│       ├── proxycontroller/    # 代理控制
│       └── webadsblockmanager/ # 广告拦截
└── webnativemessagingextension/ # 原生消息扩展
    ├── ability/                # 能力相关
    ├── manager/                # 管理器
    └── ets/                    # ETS 定义
```

### 核心模块

| 模块 | 说明 | 主要类 |
|------|------|--------|
| **webviewcontroller** | WebView 控制器核心 | WebviewController, WebMessagePort |
| **webcookiemanager** | Cookie 管理 | WebCookieManager |
| **webdatabase** | 数据库管理 | WebDataBase, GeolocationPermission |
| **webstorage** | 存储 | WebStorage |
| **proxycontroller** | 代理控制 | ProxyController, ProxyConfig |
| **webadsblockmanager** | 广告拦截 | WebAdsBlockManager |
| **webnativemessagingextension** | 原生消息扩展 | WebNativeMessagingExtension |

## ANI 绑定机制

### 1. ANI API 基础

**头文件**: `ani.h`

```cpp
// ANI 环境和类型
typedef struct ani_env_* ani_env;
typedef void* ani_ref;
typedef uint32_t ani_boolean;
typedef int32_t ani_status;

// ANI 状态码
#define ANI_OK 0
#define ANI_ERROR -1
```

### 2. 类绑定流程

**初始化函数**: `StsWebviewControllerInit()` (ani_webview_controller.cpp:7328)

```cpp
ani_status StsWebviewControllerInit(ani_env *env)
{
    // 1. 查找 ArkTS 类
    ani_class webviewControllerCls = nullptr;
    ani_status status = env->FindClass(ANI_CLASS_WEBVIEW_CONTROLLER, &webviewControllerCls);

    // 2. 定义实例方法数组
    std::array instanceMethods = {
        ani_native_function { "<ctor>", "C{std.core.String}:", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "loadUrl", nullptr, reinterpret_cast<void*>(LoadUrl) },
        ani_native_function { "runJavaScript", nullptr, reinterpret_cast<void*>(RunJavaScript) },
        ani_native_function { "on", nullptr, reinterpret_cast<void*>(On) },
        ani_native_function { "off", nullptr, reinterpret_cast<void*>(Off) },
        // ... 更多方法
    };

    // 3. 绑定实例方法
    status = env->Class_BindNativeMethods(
        webviewControllerCls,
        instanceMethods.data(),
        instanceMethods.size()
    );

    // 4. 绑定静态方法 (如果有)
    status = env->Class_BindStaticNativeMethods(
        webviewControllerCls,
        staticMethods.data(),
        staticMethods.size()
    );

    return ANI_OK;
}
```

### 3. 方法签名

**方法签名格式**: `"参数签名:返回签名"`

**类型签名对照表**:

| ArkTS 类型 | ANI 签名 |
|-----------|---------|
| `string` | `C{std.core.String}` |
| `number` | `D` (double) 或 `I` (int) |
| `boolean` | `Z` |
| `void` | `:` |
| `Array<T>` | `[T` |
| `Object` | `L{fully.qualified.ClassName};` |

**示例**:
```cpp
// loadUrl(string): void
ani_native_function { "loadUrl", nullptr, LoadUrl }

// loadData(string, string, string, string): void
ani_native_function { "loadData", "C{std.core.String}C{std.core.String}C{std.core.String}C{std.core.String}:", LoadData }

// runJavaScript(string): Promise<string>
ani_native_function { "runJavaScript", "C{std.core.String}:D", RunJavaScript }
```

### 4. 对象包装机制

**基础对象包装**:
```cpp
// 1. 创建 C++ 对象
WebviewController* webviewController = new WebviewController();

// 2. 包装为 ani_ref
ani_ref aniObject = nullptr;
ani_status status = env->Object_New(ConstructorClass, &aniObject);

// 3. 设置对象属性
status = env->SetNativePointer(
    aniObject,
    webviewController,
    [](ani_env *env, void* data, void* hint) {
        // deleter: 对象被 GC 时调用
        delete static_cast<WebviewController*>(data);
    }
);
```

**nativecommon 对象包装** (带引用计数):
```cpp
// 1. 创建 nativecommon 对象
WebHistoryList* historyList = new WebHistoryList(sptrHistoryList);
historyList->IncRefCount();  // 增加引用计数

// 2. 包装为 ani_ref
ani_ref aniHistoryList = nullptr;
env->Object_New(HistoryListClass, &aniHistoryList);

// 3. 设置带引用计数的 deleter
env->SetNativePointer(
    aniHistoryList,
    historyList,
    [](ani_env *env, void* data, void* hint) {
        WebHistoryList* obj = static_cast<WebHistoryList*>(data);
        if (obj->DecRefCount() == 0) {
            delete obj;  // 引用计数为 0 时释放
        }
    }
);
```

### 5. 参数解析

**解析字符串参数**:
```cpp
ani_status LoadUrl(ani_env *env, ani_ref thisObj, ani_ref urlRef)
{
    // 1. 获取字符串长度
    ani_size length = 0;
    ani_status status = env->String_GetUtf8Length(urlRef, &length);

    // 2. 分配缓冲区
    std::vector<char> buffer(length + 1);
    ani_size copied = 0;

    // 3. 获取字符串内容
    status = env->String_GetUtf8Chars(urlRef, buffer.data(), buffer.size(), &copied);
    std::string url(buffer.data(), copied);

    // 4. 调用核心引擎
    webviewController->LoadUrl(url);

    return ANI_OK;
}
```

**解析整数参数**:
```cpp
ani_status SetWebId(ani_env *env, ani_ref thisObj, ani_int webId)
{
    WebviewController* controller = nullptr;
    env->GetNativePointer(thisObj, (void**)&controller);

    controller->SetWebId(webId);

    return ANI_OK;
}
```

**解析对象参数**:
```cpp
ani_status SetCustomUserAgent(ani_env *env, ani_ref thisObj, ani_ref userAgentRef)
{
    // 1. 检查是否为 null
    ani_boolean isNull = ANI_FALSE;
    env->Value_IsNull(userAgentRef, &isNull);

    if (isNull == ANI_TRUE) {
        // 处理 null 情况
        return ANI_OK;
    }

    // 2. 获取字符串
    ani_size length = 0;
    env->String_GetUtf8Length(userAgentRef, &length);
    std::vector<char> buffer(length + 1);
    ani_size copied = 0;
    env->String_GetUtf8Chars(userAgentRef, buffer.data(), buffer.size(), &copied);

    std::string userAgent(buffer.data(), copied);

    // 3. 调用核心引擎
    controller->SetCustomUserAgent(userAgent);

    return ANI_OK;
}
```

## 核心模块详解

### 1. WebviewController (核心控制器)

**文件**: `src/webviewcontroller/ani_webview_controller.cpp` (7568 行)

**主要功能**:
- 页面导航 (LoadUrl, Backward, Forward, Refresh)
- JavaScript 执行 (RunJavaScript)
- 消息通信 (CreateWebMessagePorts, PostMessage)
- 页面控制 (Zoom, Scroll, Stop)
- 状态查询 (GetTitle, GetUrl, GetProgress)
- 下载管理 (StartDownload, SetDownloadDelegate)

**核心方法示例**:

```cpp
ani_status LoadUrl(ani_env *env, ani_ref thisObj, ani_ref urlRef)
{
    // 1. 获取控制器对象
    WebviewController* webviewController = nullptr;
    env->GetNativePointer(thisObj, (void**)&webviewController);

    // 2. 解析 URL 字符串
    ani_size length = 0;
    env->String_GetUtf8Length(urlRef, &length);
    std::vector<char> buffer(length + 1);
    ani_size copied = 0;
    env->String_GetUtf8Chars(urlRef, buffer.data(), buffer.size(), &copied);
    std::string url(buffer.data(), copied);

    // 3. 调用核心引擎
    ErrCode errCode = webviewController->LoadUrl(url);

    // 4. 处理错误
    if (errCode != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrcode(env, errCode);
        return ANI_ERROR;
    }

    return ANI_OK;
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

### 2. 异步编程模型

**Promise 模式**:

```cpp
ani_status RunJavaScript(ani_env *env, ani_ref thisObj, ani_ref scriptRef, ani_ref promiseRef)
{
    // 1. 解析脚本
    std::string script;
    AniParseUtils::ParseString(env, scriptRef, script);

    // 2. 获取 Promise 对象
    ani_ref promiseThen = nullptr;
    ani_ref promiseCatch = nullptr;
    env->Promise_GetThen(promiseRef, &promiseThen);
    env->Promise_GetCatch(promiseRef, &promiseCatch);

    // 3. 创建回调实现
    auto callback = std::make_shared<AniNWebValueCallbackImpl>(
        env, promiseThen, promiseCatch);

    // 4. 调用核心引擎
    webviewController->RunJavaScript(script, callback);

    return ANI_OK;
}
```

**回调实现**:
```cpp
class AniNWebValueCallbackImpl : public NWebMessageValueCallback {
public:
    AniNWebValueCallbackImpl(ani_env *env, ani_ref promiseThen, ani_ref promiseCatch)
        : env_(env), promiseThen_(promiseThen), promiseCatch_(promiseCatch)
    {}

    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override
    {
        // 1. 在主线程执行
        uv_work_t* work = new uv_work_t;
        work->data = this;
        uv_queue_work(uv_default_loop(), work,
            [](uv_work_t *work) {},  // 工作函数
            [](uv_work_t *work, int status) {  // 完成回调
                AniNWebValueCallbackImpl* callback =
                    static_cast<AniNWebValueCallbackImpl*>(work->data);

                // 2. 创建结果对象
                ani_ref resultRef = nullptr;
                callback->CreateResultFromNWebMessage(result, &resultRef);

                // 3. 调用 Promise then
                ani_ref argv[] = { resultRef };
                callback->env_->Function_Call(
                    callback->promiseThen_,
                    nullptr,
                    1,
                    argv,
                    nullptr
                );
            }
        );
    }

private:
    ani_env *env_;
    ani_ref promiseThen_;
    ani_ref promiseCatch_;
};
```

### 3. 事件系统

**事件注册**:

```cpp
ani_status On(ani_env *env, ani_ref thisObj, ani_ref eventNameRef, ani_ref callbackRef)
{
    // 1. 解析事件名
    std::string eventName;
    AniParseUtils::ParseString(env, eventNameRef, eventName);

    // 2. 保存回调引用
    ani_ref callbackRefGlobal = nullptr;
    env->CreateGlobalReference(callbackRef, &callbackRefGlobal);

    // 3. 注册事件监听
    webviewController->On(eventName,
        [env, callbackRefGlobal](const Event& event) {
            // 4. 创建事件对象
            ani_ref eventObj = nullptr;
            CreateEventObject(env, event, &eventObj);

            // 5. 调用回调
            ani_ref argv[] = { eventObj };
            env->Function_Call(callbackRefGlobal, nullptr, 1, argv, nullptr);
        }
    );

    return ANI_OK;
}
```

## 数据类型转换

### 基本类型转换

| ArkTS 类型 | C++ 类型 | ANI API |
|-----------|---------|---------|
| `string` | `std::string` | `String_GetUtf8Chars()` |
| `number` | `double`, `int32_t` | `Value_GetDouble()`, `Value_GetInt()` |
| `boolean` | `bool` | `Value_GetBoolean()` |
| `undefined` | - | `Value_IsUndefined()` |
| `null` | - | `Value_IsNull()` |

**字符串转换**:
```cpp
// ArkTS → C++
std::string AniParseUtils::ParseString(ani_env *env, ani_ref stringRef)
{
    ani_size length = 0;
    env->String_GetUtf8Length(stringRef, &length);

    std::vector<char> buffer(length + 1);
    ani_size copied = 0;
    env->String_GetUtf8Chars(stringRef, buffer.data(), buffer.size(), &copied);

    return std::string(buffer.data(), copied);
}

// C++ → ArkTS
ani_ref AniParseUtils::CreateString(ani_env *env, const std::string& str)
{
    ani_ref result = nullptr;
    env->String_NewUtf8(str.c_str(), str.length(), &result);
    return result;
}
```

**数字转换**:
```cpp
// ArkTS → C++
ani_status GetNumber(ani_env *env, ani_ref valueRef, double* result)
{
    ani_boolean isNumber = ANI_FALSE;
    env->Value_IsNumber(valueRef, &isNumber);

    if (!isNumber) {
        return ANI_ERROR;
    }

    return env->Value_GetDouble(valueRef, result);
}

// C++ → ArkTS
ani_ref CreateNumber(ani_env *env, double value)
{
    ani_ref result = nullptr;
    env->Number_New(value, &result);
    return result;
}
```

**布尔转换**:
```cpp
// ArkTS → C++
ani_status GetBoolean(ani_env *env, ani_ref valueRef, bool* result)
{
    ani_boolean value = ANI_FALSE;
    ani_status status = env->Value_GetBoolean(valueRef, &value);
    *result = (value == ANI_TRUE);
    return status;
}

// C++ → ArkTS
ani_ref CreateBoolean(ani_env *env, bool value)
{
    ani_ref result = nullptr;
    env->Boolean_New(value ? ANI_TRUE : ANI_FALSE, &result);
    return result;
}
```

## 错误处理

### 错误码定义

使用 nativecommon 的错误码系统 (与 NAPI 共享):

```cpp
namespace NWebError {
constexpr ErrCode NO_ERROR = 0;
constexpr ErrCode PARAM_CHECK_ERROR = 401;
constexpr ErrCode INVALID_URL = 17100002;
constexpr ErrCode INIT_ERROR = 17100001;
}
```

### 抛出错误

```cpp
// 使用 AniBusinessError 抛出错误
if (url.empty()) {
    AniBusinessError::ThrowErrorByErrcode(env, NWebError::INVALID_URL);
    return ANI_ERROR;
}

// 抛出自定义错误消息
ani_ref errorRef = nullptr;
env->Error_New("Invalid URL format", &errorRef);
env->Throw(errorRef);
```

### ArkTS 错误处理

```typescript
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

## 构建和集成

### 构建配置 (BUILD.gn)

```gni
ohos_shared_library("webview_ani") {
    shlib_type = "ani"  # ANI 类型库

    sources = [
        "./src/common/ani_business_error.cpp",
        "./src/common/ani_parse_utils.cpp",
        "./src/webviewcontroller/ani_webview_controller.cpp",
        "./src/webviewcontroller/ani_web_scheme_handler.cpp",
        # ... 更多源文件
    ]

    include_dirs = [
        "./src/common",
        "./src/webviewcontroller",
        "../../nativecommon",  # 使用 nativecommon
        # ... 更多头文件路径
    ]

    deps = [
        "../../../ohos_nweb:libnweb",
        "../../native:ohweb",
    ]

    external_deps = [
        "hilog:libhilog",
        "ability_runtime:app_context",
    ]

    part_name = "webview"
    subsystem_name = "web"
}
```

### 构建命令

```bash
# 构建 ANI 模块
./build.sh --product-name <产品名> \
    --build-target //base/web/webview/interfaces/kits/ani:webview_ani

# 输出
# libwebview_ani.so (安装到 /usr/lib/module/web/)
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
- `libwebview_common.so` (nativecommon) - 通用组件库
- `libhilog.so` - 日志系统

**系统依赖**:
- OpenHarmony SDK API 12+
- ArkTS 编译器
- ANI 运行时
- ACE (ArkUI) 引擎

## 与 nativecommon 的关系

### 共享 nativecommon 组件

ANI 接口与 NAPI 接口共享 nativecommon 层:

**共享的组件**:
- `WebHistoryList` - 历史记录列表
- `WebMessagePort` - 消息端口
- `WebSchemeHandlerRequest` - 自定义协议请求
- `WebSchemeHandlerResponse` - 自定义协议响应
- `NWebUserAgentMetadataImpl` - User-Agent 元数据
- `TransferReferencedCount` - 引用计数基类
- `NWebError` - 错误码系统

**使用方式**:
```cpp
// ANI 中使用 nativecommon 对象
WebHistoryList* historyList = new WebHistoryList(nwebHistoryList);
historyList->IncRefCount();

// 包装为 ani_ref
env->SetNativePointer(
    aniHistoryList,
    historyList,
    [](ani_env *env, void* data, void* hint) {
        WebHistoryList* obj = static_cast<WebHistoryList*>(data);
        if (obj->DecRefCount() == 0) {
            delete obj;
        }
    }
);
```

### ANI vs NAPI 架构对比

```
NAPI 路径:
ArkTS → NAPI (napi_wrap) → nativecommon → ohos_nweb → ArkWebCore.hap

ANI 路径:
ArkTS → ANI (ani_ref) → nativecommon → ohos_nweb → ArkWebCore.hap
```

**相似之处**:
- 都使用 nativecommon 进行数据结构封装
- 都使用引用计数管理对象生命周期
- 都调用相同的核心引擎 (ohos_nweb)

**差异之处**:
- ANI 使用 `ani_ref` 代替 `napi_value`
- ANI 使用不同的 API (`ani_env` 代替 `napi_env`)
- ANI 的绑定机制性能更高

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

### 2. 避免不必要的字符串拷贝

```cpp
// ❌ 多次拷贝
std::string url;
env->String_GetUtf8Chars(urlRef, buffer, size, &copied);
url = std::string(buffer, copied);
controller->LoadUrl(url);

// ✅ 直接使用
ani_size length = 0;
env->String_GetUtf8Length(urlRef, &length);
std::vector<char> buffer(length + 1);
ani_size copied = 0;
env->String_GetUtf8Chars(urlRef, buffer.data(), buffer.size(), &copied);
controller->LoadUrl(std::string(buffer.data(), copied));
```

### 3. 使用 Promise 并行

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

## 调试技巧

### 日志查看

```bash
# 查看 ANI 日志
hilog -T ArkWeb

# 查看 ANI 绑定日志
hilog -T ArkWeb | grep "ANI"
```

### 常见问题

**Q: ANI 模块加载失败**

可能原因:
1. libwebview_ani.so 未安装
2. 依赖库缺失
3. SDK 版本不匹配 (需要 API 12+)

解决方法:
```bash
# 检查库文件是否存在
ls -l /usr/lib/module/web/libwebview_ani.so

# 检查依赖
ldd /usr/lib/module/web/libwebview_ani.so
```

**Q: 对象方法调用崩溃**

可能原因:
1. GetNativePointer 失败
2. C++ 对象已释放
3. 类型转换错误

解决方法:
```cpp
// 始终检查 GetNativePointer 结果
WebviewController* controller = nullptr;
ani_status status = env->GetNativePointer(thisObj, (void**)&controller);
if (status != ANI_OK || !controller) {
    env->ThrowError("Invalid controller object");
    return ANI_ERROR;
}
```

**Q: 回调函数未执行**

可能原因:
1. 未正确保存 ani_ref 全局引用
2. 线程问题 (回调在错误线程执行)
3. 环境已销毁

解决方法:
```cpp
// 使用 CreateGlobalReference 保存回调
ani_ref callbackRefGlobal = nullptr;
env->CreateGlobalReference(callbackRef, &callbackRefGlobal);

// 使用 uv_async 确保在主线程执行
uv_async_t* async = new uv_async_t;
async->data = callbackData;
uv_async_init(uv_default_loop(), async, AsyncCallback);
uv_async_send(async);
```

## 最佳实践

### 1. 参数验证

```cpp
ani_status LoadUrl(ani_env *env, ani_ref thisObj, ani_ref urlRef)
{
    // 1. 检查 URL 是否为 null
    ani_boolean isNull = ANI_FALSE;
    env->Value_IsNull(urlRef, &isNull);
    if (isNull == ANI_TRUE) {
        AniBusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return ANI_ERROR;
    }

    // 2. 检查 URL 是否为字符串
    ani_boolean isString = ANI_FALSE;
    env->Value_IsString(urlRef, &isString);
    if (isString == ANI_FALSE) {
        AniBusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return ANI_ERROR;
    }

    // 3. 解析并验证 URL
    std::string url;
    AniParseUtils::ParseString(env, urlRef, url);
    if (url.empty() || url.length() > URL_MAXIMUM) {
        AniBusinessError::ThrowErrorByErrcode(env, NWebError::INVALID_URL);
        return ANI_ERROR;
    }

    // ... 处理逻辑
}
```

### 2. 内存管理

```cpp
// 1. 使用智能指针管理回调
auto callback = std::make_shared<AniNWebValueCallbackImpl>(env, promiseThen, promiseCatch);

// 2. 在 lambda 中捕获智能指针
webviewController->RunJavaScript(script, [callback](const std::string& result) {
    callback->OnReceiveValue(result);
});

// 3. deleter 中释放资源
env->SetNativePointer(
    aniObject,
    data,
    [](ani_env *env, void* data, void* hint) {
        delete static_cast<MyData*>(data);
    }
);
```

### 3. 异常处理

```cpp
ani_status RunJavaScript(ani_env *env, ani_ref thisObj, ani_ref scriptRef, ani_ref promiseRef)
{
    try {
        // 解析参数
        std::string script;
        AniParseUtils::ParseString(env, scriptRef, script);

        if (script.empty()) {
            AniBusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
            return ANI_ERROR;
        }

        // 调用核心引擎
        auto callback = std::make_shared<AniNWebValueCallbackImpl>(env, promiseThen, promiseCatch);
        webviewController->RunJavaScript(script, callback);

        return ANI_OK;

    } catch (const std::exception& e) {
        ani_ref errorRef = nullptr;
        env->Error_New(e.what(), &errorRef);
        env->Throw(errorRef);
        return ANI_ERROR;
    }
}
```

## 相关文档

- **[../nativecommon/README.md](../nativecommon/README.md)** - nativecommon 通用组件库文档
- **[../napi/README.md](../napi/README.md)** - NAPI 接口文档
- **[../../native/README.md](../../native/README.md)** - Native C++ 接口文档
- **[../../../ohos_nweb/README.md](../../../ohos_nweb/README.md)** - 核心引擎文档
- **[../../../AGENT.md](../../../AGENT.md)** - 代码仓指南
