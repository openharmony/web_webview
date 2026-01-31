# interfaces/native - ArkWeb Native C 接口

本目录提供 ArkWeb 的 Native C NDK 接口,允许 C/C++ 应用直接使用 WebView 功能,无需通过 ArkTS/JS。

## 概述

### 核心功能

Native 接口提供了以下能力:
- **C API 访问** - 直接使用 C/C++ 调用 WebView 功能
- **统一入口** - 通过 `OH_ArkWeb_GetNativeAPI()` 获取不同类型的 API
- **动态加载** - 运行时从 libnweb.so 加载符号
- **高性能** - 避免跨语言调用的性能开销

### 适用场景

- ✅ C/C++ 原生应用集成 WebView
- ✅ 高性能需求的场景
- ✅ 需要直接访问底层 API 的场景
- ✅ 游戏引擎集成 WebView
- ✅ 需要深度定制的应用

## 架构设计

### 统一入口模式

Native 接口采用"统一入口 + 类型分发"的设计模式:

```
C/C++ 应用
  ↓ OH_ArkWeb_GetNativeAPI(type)
interfaces/native/ (统一入口)
  ↓ arkweb_interface.cpp
  ↓ 根据 type 分发
LoadComponentAPI() / LoadControllerAPI() / ...
  ↓ 动态加载符号
NWebHelper::LoadFuncSymbol()
  ↓ 返回 API 接口指针
应用使用 API
```

**与 nativecommon 的关系**:
- Native 接口直接调用核心引擎 (ohos_nweb),不通过 nativecommon
- Native 接口返回的是 C 结构体和指针,不需要引用计数管理
- nativecommon 主要为 NAPI 接口服务,提供跨语言边界的对象管理

### API 类型

| API 类型 | 枚举值 | 功能说明 |
|---------|--------|----------|
| **Component** | `ARKWEB_NATIVE_COMPONENT` | 组件生命周期 API |
| **Controller** | `ARKWEB_NATIVE_CONTROLLER` | 控制器 API (页面操作、JS 执行等) |
| **WebMessagePort** | `ARKWEB_NATIVE_WEB_MESSAGE_PORT` | 消息端口 API |
| **WebMessage** | `ARKWEB_NATIVE_WEB_MESSAGE` | 消息 API |
| **CookieManager** | `ARKWEB_NATIVE_COOKIE_MANAGER` | Cookie 管理 API |
| **JavaScriptValue** | `ARKWEB_NATIVE_JAVASCRIPT_VALUE` | JS 值 API |

## 核心文件说明

### 头文件 (API 定义)

| 文件 | 功能 |
|------|------|
| `arkweb_interface.h` | 统一入口定义, `OH_ArkWeb_GetNativeAPI()` |
| `native_interface_arkweb.h` | 详细的 Native API 声明 |
| `arkweb_type.h` | 公共类型定义 (结构体、枚举、回调) |
| `arkweb_error_code.h` | 错误码定义 |
| `arkweb_net_error_list.h` | 网络错误列表 |
| `arkweb_scheme_handler.h` | 自定义协议处理器 |

### 实现文件

| 文件 | 功能 |
|------|------|
| `arkweb_interface.cpp` | 统一入口实现,动态加载分发 |
| `native_interface_arkweb.cpp` | Native API 实现 |
| `arkweb_scheme_handler.cpp` | 自定义协议处理实现 |
| `native_arkweb_utils.cpp` | 工具函数 |
| `native_javascript_execute_callback.cpp` | JS 执行回调 |

### 构建配置

| 文件 | 功能 |
|------|------|
| `BUILD.gn` | 构建配置,定义 libohweb.so |
| `libohweb.ndk.json` | NDK 描述文件 |

## 工作原理

### 1. 动态符号加载机制

**核心函数**: `LoadFunction()` (arkweb_interface.cpp:92)

```cpp
template<typename Fn>
static void LoadFunction(const char* functionName, Fn* fnOut)
{
    void* fn = OHOS::NWeb::NWebHelper::Instance().LoadFuncSymbol(functionName);
    if (!fn) {
        WVLOG_E("%{public}s not found.", functionName);
        return;
    }
    *fnOut = reinterpret_cast<Fn*>(fn);
}
```

**工作流程**:
1. 调用 `NWebHelper::LoadFuncSymbol()` 从 libnweb.so 加载符号
2. 将符号转换为函数指针
3. 返回给应用使用

### 2. API 分发机制

**核心函数**: `OH_ArkWeb_GetNativeAPI()` (arkweb_interface.cpp:279)

```cpp
ArkWeb_AnyNativeAPI* OH_ArkWeb_GetNativeAPI(ArkWeb_NativeAPIVariantKind type)
{
    switch (type) {
        case ARKWEB_NATIVE_COMPONENT:
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_ComponentImpl);
        case ARKWEB_NATIVE_CONTROLLER:
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_ControllerImpl);
        // ... 其他类型
    }
}
```

**工作流程**:
1. 应用调用 `OH_ArkWeb_GetNativeAPI(type)` 指定 API 类型
2. 根据类型调用对应的 `LoadXXXAPI()` 函数
3. 加载函数指针到全局变量
4. 返回 API 接口指针

### 3. 组件 API 加载

**示例**: `LoadComponentAPI()` 加载组件相关函数

```cpp
#define ARKWEB_NATIVE_FOR_EACH_COMPONENT_API_FN(DO)           \
    DO(onControllerAttached, OH_ArkWeb_OnControllerAttached); \
    DO(onPageBegin, OH_ArkWeb_OnPageBegin);                   \
    DO(onPageEnd, OH_ArkWeb_OnPageEnd);                       \
    DO(onDestroy, OH_ArkWeb_OnDestroy)
```

**加载过程**:
1. 调用 `LoadFunction()` 加载每个符号
2. 将函数指针存储到 `g_ComponentImpl` 结构
3. 返回给应用使用

## API 分类详解

### 1. Component API

**功能**: 组件生命周期管理

**主要函数**:
- `OH_ArkWeb_OnControllerAttached()` - 控制器附加
- `OH_ArkWeb_OnPageBegin()` - 页面开始加载
- `OH_ArkWeb_OnPageEnd()` - 页面加载完成
- `OH_ArkWeb_OnDestroy()` - 组件销毁

**使用场景**: 监听 WebView 生命周期事件

### 2. Controller API

**功能**: WebView 页面操作

**主要函数**:
- `OH_ArkWeb_RunJavaScript()` - 执行 JavaScript 代码
- `OH_ArkWeb_RegisterJavaScriptProxy()` - 注册 JS 对象
- `OH_ArkWeb_Refresh()` - 刷新页面
- `OH_ArkWeb_CreateWebMessagePorts()` - 创建消息端口
- `OH_ArkWeb_PostWebMessage()` - 发送消息

**使用场景**: 控制 WebView 行为,与页面交互

### 3. WebMessagePort API

**功能**: 消息端口管理

**主要函数**:
- `OH_WebMessage_PostMessage()` - 发送消息
- `OH_WebMessage_Close()` - 关闭端口
- `OH_WebMessage_SetMessageEventHandler()` - 设置消息处理器

**使用场景**: 双向通信

### 4. CookieManager API

**功能**: Cookie 管理

**主要函数**:
- `OH_CookieManager_FetchCookieSync()` - 获取 Cookie
- `OH_CookieManager_ConfigCookieSync()` - 配置 Cookie
- `OH_CookieManager_ClearAllCookiesSync()` - 清除所有 Cookie

**使用场景**: Cookie 管理

### 5. JavaScriptValue API

**功能**: JavaScript 值操作

**主要函数**:
- `OH_JavaScript_CreateJavaScriptValue()` - 创建 JS 值对象

**使用场景**: JS 值类型转换

## 使用示例

### 示例 1: 获取 Controller API

```cpp
#include "arkweb_interface.h"

// 1. 获取 Controller API
ArkWeb_ControllerAPI* controller = nullptr;
{
    ArkWeb_AnyNativeAPI* api = OH_ArkWeb_GetNativeAPI(ARKWEB_NATIVE_CONTROLLER);
    if (api && api->size == sizeof(ArkWeb_ControllerAPI)) {
        controller = reinterpret_cast<ArkWeb_ControllerAPI*>(api);
    }
}

if (!controller) {
    // 错误处理
    return;
}

// 2. 使用 API 执行 JavaScript
controller->runJavaScript(webId, "console.log('Hello from C++')", callback);
```

### 示例 2: 注册 JS 代理

```cpp
// 定义 JS 回调函数
void OnJavaScriptResult(const char* result) {
    printf("JS Result: %s\n", result);
}

// 定义 JS 代理回调
char* OnJavaScriptProxy(const char** argv, int argc) {
    // 处理 JS 调用
    return strdup("result from C++");
}

// 注册 JS 代理
controller->registerJavaScriptProxy(
    webId,
    "jsObjectName",
    OnJavaScriptProxy,
    OnJavaScriptResult
);
```

### 示例 3: WebMessage 通信

```cpp
// 1. 创建 WebMessagePort
ArkWeb_WebMessagePortAPI* portAPI = nullptr;
{
    ArkWeb_AnyNativeAPI* api = OH_ArkWeb_GetNativeAPI(ARKWEB_NATIVE_WEB_MESSAGE_PORT);
    portAPI = reinterpret_cast<ArkWeb_WebMessagePortAPI*>(api);
}

// 2. 创建消息端口
int32_t portId = portAPI->createWebMessagePorts(webId, mainPort);

// 3. 设置消息处理器
portAPI->setMessageEventHandler(portId, [](int32_t port, ArkWeb_WebMessagePtr msg) {
    // 处理接收到的消息
});

// 4. 发送消息
ArkWeb_WebMessagePtr message = createMessage("Hello from C++");
portAPI->postMessage(portId, message);
```

## 构建和集成

### 构建

```bash
# 构建 Native 接口库
./build.sh --product-name <产品名> --build-target //base/web/webview/interfaces/native:ohweb
```

**输出**:
- `libohweb.so` - Native 接口库 (安装到 `/usr/lib/ndk/`)

### CMake 集成示例

```cmake
# 查找 ArkWeb Native 库
find_library(ohweb libohweb.so HINTS ${CMAKE_INSTALL_PREFIX}/usr/lib/ndk)

# 链接库
target_link_libraries(your_app ${ohweb})

# 包含头文件
target_include_directories(your_app PRIVATE
    ${CMAKE_INSTALL_PREFIX}/usr/include/web/
)
```

### 依赖关系

**依赖库**:
- `libnweb.so` - 核心 WebView 引擎
- `libarkweb_utils.so` - 工具库
- `libace_napi.z.so` - NAPI 支持
- `libhilog.so` - 日志系统
- `libeventhandler.so` - 事件处理

**系统依赖**:
- OpenHarmony NDK R11+
- `SystemCapability.Web.Webview.Core`

## API 版本管理

### API 类型枚举 (ArkWeb_NativeAPIVariantKind)

| API 类型 | 引入版本 | 说明 |
|---------|---------|------|
| `ARKWEB_NATIVE_COMPONENT` | API 12 | 组件 API |
| `ARKWEB_NATIVE_CONTROLLER` | API 12 | 控制器 API |
| `ARKWEB_NATIVE_WEB_MESSAGE_PORT` | API 12 | 消息端口 API |
| `ARKWEB_NATIVE_WEB_MESSAGE` | API 12 | 消息 API |
| `ARKWEB_NATIVE_COOKIE_MANAGER` | API 12 | Cookie 管理 API |
| `ARKWEB_NATIVE_JAVASCRIPT_VALUE` | API 16 | JS 值 API |

### 向后兼容性

- **size 字段**: 每个 API 结构体都有 `size` 字段用于版本检查
- **动态加载**: 运行时加载符号,避免编译时依赖
- **版本检查**: 使用前检查 `api->size` 是否匹配

```cpp
if (api && api->size == sizeof(ArkWeb_ControllerAPI)) {
    // 版本匹配,可以使用
    controller = reinterpret_cast<ArkWeb_ControllerAPI*>(api);
}
```

## 错误处理

### 错误码

定义在 `arkweb_error_code.h`:

```cpp
typedef enum {
    ARKWEB_SUCCESS = 0,                    // 成功
    ARKWEB_ERROR = -1,                      // 一般错误
    ARKWEB_ERROR_INVALID_ARGUMENT = -2,    // 无效参数
    // ... 更多错误码
} ArkWeb_ErrorCode;
```

### 错误检查

```cpp
// 1. 检查 API 获取是否成功
ArkWeb_AnyNativeAPI* api = OH_ArkWeb_GetNativeAPI(ARKWEB_NATIVE_CONTROLLER);
if (!api) {
    // 处理错误
    return;
}

// 2. 检查版本是否匹配
if (api->size != sizeof(ArkWeb_ControllerAPI)) {
    // 版本不匹配
    return;
}

// 3. 使用回调错误码
void OnCookieSaved(ArkWeb_ErrorCode errorCode) {
    if (errorCode == ARKWEB_SUCCESS) {
        // 保存成功
    } else {
        // 处理错误
    }
}
```

## 性能考虑

### 优势

1. **零拷贝** - C/C++ 直接调用,无需跨语言边界
2. **类型安全** - 编译时类型检查
3. **低开销** - 避免 ArkTS ↔ C/C++ 转换
4. **高性能** - 适合频繁调用的场景

### 注意事项

1. **符号加载** - 首次调用有加载开销
2. **版本兼容** - 需要检查 API 版本
3. **生命周期** - 确保 API 在使用期间有效

## 调试

### 查看日志

```bash
# 查看 Native 接口日志
hilog -T ArkWeb

# 查看符号加载日志
hilog | grep "not found"
```

### 常见问题

**Q: `OH_ArkWeb_GetNativeAPI` 返回 nullptr**

可能原因:
1. libnweb.so 未加载
2. API 类型不支持
3. 符号未找到

解决方法:
```bash
# 检查 libnweb.so 是否存在
ls -l /usr/lib/ndk/libnweb.so

# 检查日志
hilog -T ArkWeb
```

**Q: 函数调用崩溃**

可能原因:
1. API 版本不匹配
2. 空指针未检查
3. 生命周期问题

解决方法:
```cpp
// 始终检查 API 版本
if (api->size != sizeof(ArkWeb_XXXAPI)) {
    return;
}

// 检查返回值
if (controller->runJavaScript(...) != ARKWEB_SUCCESS) {
    // 处理错误
}
```

## 限制和约束

### 功能限制

- ❌ 不支持所有 NAPI 功能 (部分功能仅支持 ArkTS)
- ❌ 需要 C/C++ 环境和 NDK
- ❌ 版本兼容性需手动管理

### 使用限制

- 必须链接 libohweb.so
- 依赖 libnweb.so (核心引擎)
- 需要正确的系统能力声明

## 最佳实践

### 1. API 版本检查

```cpp
ArkWeb_AnyNativeAPI* api = OH_ArkWeb_GetNativeAPI(type);
if (!api || api->size != expected_size) {
    WVLOG_E("API version mismatch or not loaded");
    return ERROR;
}
```

### 2. 错误处理

```cpp
// 检查所有返回值
ArkWeb_ErrorCode ret = controller->runJavaScript(...);
if (ret != ARKWEB_SUCCESS) {
    WVLOG_E("runJavaScript failed: %{public}d", ret);
    return;
}
```

### 3. 生命周期管理

```cpp
// 在合适时机获取 API
void InitWebView() {
    controllerApi = GetControllerAPI();
}

// 在合适时机释放
void DestroyWebView() {
    controllerApi = nullptr;
}
```

### 4. 多线程安全

- API 调用不是线程安全的
- 需要在主线程或使用互斥锁

## 与 nativecommon 的关系

### 架构差异

Native 接口和 NAPI 接口在架构上的主要差异:

| 方面 | Native 接口 | NAPI 接口 |
|------|------------|----------|
| **调用路径** | 应用 → interfaces/native → ohos_nweb | 应用 → NAPI → nativecommon → ohos_nweb |
| **数据类型** | C 结构体、指针 | C++ 对象 (带引用计数) |
| **内存管理** | 应用手动管理 | 引用计数自动管理 |
| **性能** | 高 (零拷贝) | 中 (有跨语言开销) |
| **对象生命周期** | 应用控制 | 引用计数控制 |

### 为何不使用 nativecommon

Native 接口不使用 nativecommon 的原因:

1. **C API 设计**: Native 接口是纯 C API,使用结构体和函数指针
2. **性能考虑**: 避免额外的封装层,保持高性能
3. **简单性**: C/C++ 应用可以直接管理内存,不需要引用计数
4. **兼容性**: NDK 接口需要保持稳定性,减少依赖

### 数据结构对比

**Native 接口** (直接使用核心引擎类型):
```cpp
// 核心引擎定义的结构体
typedef struct ArkWeb_ControllerAPI {
    int32_t (*runJavaScript)(int32_t webId, const char* script, ...);
    // ...
} ArkWeb_ControllerAPI;

// 直接返回给应用使用
ArkWeb_ControllerAPI* controller = OH_ArkWeb_GetNativeAPI(ARKWEB_NATIVE_CONTROLLER);
controller->runJavaScript(webId, "console.log('Hello')");
```

**NAPI 接口** (使用 nativecommon 封装):
```cpp
// nativecommon 封装类
class WebHistoryList : public TransferReferencedCount {
    std::shared_ptr<NWebHistoryList> sptrHistoryList_;
    // ...
};

// 包装为 JS 对象,带引用计数
WebHistoryList* historyList = new WebHistoryList(nwebHistoryList);
historyList->IncRefCount();
napi_wrap(env, jsObject, historyList, deleter, ...);
```

### 何时选择哪种接口

**选择 Native 接口**:
- ✅ C/C++ 原生应用
- ✅ 游戏引擎集成
- ✅ 需要极致性能
- ✅ 希望直接控制内存

**选择 NAPI 接口**:
- ✅ ArkTS 应用开发
- ✅ 需要与 ArkUI 框架集成
- ✅ 不想手动管理内存
- ✅ 使用 JavaScript/TypeScript

### 共享的核心组件

虽然 Native 接口不使用 nativecommon,但两者共享:

1. **核心引擎**: 都调用 `ohos_nweb/` 提供的功能
2. **错误码**: 都使用 `arkweb_error_code.h` 中定义的错误码
3. **配置系统**: 都受 `web_config.xml` 和 param 参数影响

**架构对比图**:
```
Native 接口路径:
C/C++ 应用 → interfaces/native → ohos_nweb → ArkWebCore.hap

NAPI 接口路径:
ArkTS 应用 → interfaces/napi → nativecommon → ohos_nweb → ArkWebCore.hap
```

### 相关文档

详见: **[../kits/nativecommon/README.md](../kits/nativecommon/README.md)** - nativecommon 通用组件库文档

## 相关文档

- **[../kits/nativecommon/README.md](../kits/nativecommon/README.md)** - nativecommon 通用组件库文档
- **[../kits/napi/README.md](../kits/napi/README.md)** - NAPI 接口文档
- **[../../ohos_nweb/README.md](../../ohos_nweb/README.md)** - 核心 WebView 引擎
- **[../ohos_interface/README.md](../ohos_interface/README.md)** - 接口层总览
- **[../../AGENT.md](../../AGENT.md)** - 代码仓指南
