# interfaces/kits/cj - ArkWeb CJ FFI 接口

本目录提供 ArkWeb 的 CJ (Cangjie) FFI (Foreign Function Interface) 接口实现,通过 C FFI 为跨语言调用提供标准化的绑定机制,支持 Rust、Python 等语言调用 WebView 功能。

## 概述

### 核心功能

CJ FFI 接口提供了以下能力:
- **C FFI 绑定** - 标准的 C 外部函数接口
- **跨语言支持** - 支持 Rust、Python、Node.js 等多种语言
- **类型兼容** - 提供与 FFI 兼容的数据类型
- **轻量级** - 最小化运行时开销
- **ABI 稳定** - 保证跨语言调用的二进制兼容性

### 适用场景

- ✅ Rust 应用集成 WebView
- ✅ Python 脚本调用 WebView 功能
- ✅ 跨语言混合开发
- ✅ 需要标准 FFI 接口的场景
- ✅ 多语言统一接口需求

### CJ vs 其他接口

| 特性 | CJ FFI | NAPI | ANI | Native |
|------|--------|------|-----|--------|
| **主要语言** | Rust, Python, C++ | ArkTS | ArkTS | C/C++ |
| **绑定方式** | C FFI | NAPI | ANI | C API |
| **性能** | 高 (接近 Native) | 中 | 高 | 最高 |
| **跨语言** | 优秀 (多语言) | 仅 ArkTS | 仅 ArkTS | 仅 C/C++ |
| **复杂度** | 低 | 中 | 中 | 低 |
| **类型安全** | 编译时 | 运行时 | 编译时 | 编译时 |
| **API 稳定性** | ABI 稳定 | API 稳定 | API 稳定 | ABI 稳定 |

## 架构设计

### 整体架构

```
Rust/Python/C++ 应用
  ↓ FFI 调用
CJ FFI 接口层 (interfaces/kits/cj/)
  ↓ C 函数调用
ohos_nweb/ 核心引擎层
  ↓ ohos_interface/include/ohos_nweb/
ArkWebCore.hap
```

**架构说明**:
- **CJ FFI 层**: 提供标准 C FFI 函数,使用 C 兼容类型
- **核心引擎层**: 提供 WebView 核心功能
- **无需 nativecommon**: 直接使用 C 基本类型和结构体

### 目录结构

```
interfaces/kits/cj/
├── BUILD.gn                    # 构建配置
├── include/                    # 公共头文件 (导出)
│   ├── webview_ffi.h           # WebView FFI 声明
│   ├── web_cookie_manager_ffi.h    # Cookie FFI
│   ├── web_data_base_ffi.h         # 数据库 FFI
│   ├── web_storage_ffi.h           # 存储 FFI
│   ├── web_download_ffi.h          # 下载 FFI
│   ├── webview_adsblock_ffi.h      # 广告拦截 FFI
│   ├── webview_media_ffi.h         # 媒体 FFI
│   ├── webview_message_ffi.h       # 消息 FFI
│   ├── webview_scheme_ffi.h        # 自定义协议 FFI
│   └── ...                        # 更多 FFI 头文件
└── src/                        # 实现文件
    ├── webview_ffi.cpp          # WebView FFI 实现
    ├── web_cookie_manager_ffi.cpp
    ├── web_data_base_ffi.cpp
    ├── web_storage_ffi.cpp
    ├── web_download_ffi.cpp
    ├── webview_controller_impl.cpp  # 控制器实现
    ├── web_runtime_delegate.cpp     # 运行时代理
    └── ...                        # 更多实现文件
```

### 核心组件

| 组件 | FFI 头文件 | 说明 |
|------|----------|------|
| **WebviewController** | `webview_ffi.h` | WebView 控制器核心 |
| **WebCookieManager** | `web_cookie_manager_ffi.h` | Cookie 管理 |
| **WebDataBase** | `web_data_base_ffi.h` | 数据库管理 |
| **WebStorage** | `web_storage_ffi.h` | 存储 |
| **WebDownload** | `web_download_ffi.h` | 下载管理 |
| **WebAdsBlock** | `webview_adsblock_ffi.h` | 广告拦截 |
| **WebMedia** | `webview_media_ffi.h` | 媒体播放 |
| **WebMessage** | `webview_message_ffi.h` | 消息通信 |
| **WebScheme** | `webview_scheme_ffi.h` | 自定义协议 |

## CJ FFI 绑定机制

### 1. FFI 函数声明

**头文件**: `webview_ffi.h`

```cpp
extern "C" {
    // FFI_EXPORT 宏导出函数符号
    FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char* url);
    FFI_EXPORT int32_t FfiOHOSWebviewCtlRefresh(int64_t id);
    FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetUrl(int64_t id);
    FFI_EXPORT int32_t FfiOHOSWebviewCtlRunJavaScript(
        int64_t id,
        char* cScript,
        void (*callbackRef)(RetDataCString infoRef)
    );
}
```

**关键特性**:
- `extern "C"`: 使用 C 链接约定,避免 name mangling
- `FFI_EXPORT`: 导出符号为公共 API
- 使用 C 基本类型 (`int64_t`, `char*`, `int32_t`)
- 使用函数指针作为回调

### 2. FFI 数据类型

**基本类型映射**:

| C 类型 | Rust 类型 | Python 类型 | 说明 |
|--------|----------|------------|------|
| `int32_t` | `i32` | `int` | 32位整数 |
| `int64_t` | `i64` | `int` | 64位整数 (对象 ID) |
| `float` | `f32` | `float` | 单精度浮点 |
| `bool` | `bool` | `bool` | 布尔值 |
| `char*` | `*const c_char` | `bytes` | C 字符串 |

**FFI 特定结构体**:

```cpp
// 字符串返回值
struct RetDataCString {
    int32_t code;         // 错误码
    char* data;           // 字符串数据
};

// 数组结构
template<typename T>
struct CArr {
    T* data;              // 数据指针
    int64_t size;         // 数组大小
};

// WebHeader 数组
struct ArrWebHeader {
    CArrCString keys;     // 键数组
    CArrCString values;   // 值数组
};

// 加载数据
struct LoadDatas {
    char* data;           // 数据内容
    char* mimeType;       // MIME 类型
    char* encoding;       // 编码
    char* baseUrl;        // 基础 URL
};
```

### 3. 函数实现模式

**基本函数实现** (无回调):

```cpp
// FFI 函数实现
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char* url)
{
    // 1. 参数验证
    if (!url) {
        WEBVIEWLOGE("url is nullptr");
        return NWebError::PARAM_CHECK_ERROR;
    }

    // 2. 获取控制器对象
    WebviewControllerImpl* controller = GetControllerById(id);
    if (!controller) {
        WEBVIEWLOGE("controller is nullptr, id: %{public}lld", id);
        return NWebError::INIT_ERROR;
    }

    // 3. URL 验证
    std::string urlStr(url);
    if (!CheckUrl(urlStr)) {
        return NWebError::INVALID_URL;
    }

    // 4. 调用核心引擎
    ErrCode errCode = controller->LoadUrl(urlStr);

    // 5. 返回错误码
    return errCode;
}
```

**异步函数实现** (带回调):

```cpp
// 回调函数类型
typedef void (*JavaScriptCallback)(RetDataCString infoRef);

// 异步 FFI 函数
FFI_EXPORT int32_t FfiOHOSWebviewCtlRunJavaScript(
    int64_t id,
    char* cScript,
    void (*callbackRef)(RetDataCString infoRef))
{
    // 1. 参数验证
    if (!cScript || !callbackRef) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    // 2. 获取控制器
    WebviewControllerImpl* controller = GetControllerById(id);
    if (!controller) {
        return NWebError::INIT_ERROR;
    }

    // 3. 转换脚本字符串
    std::string script(cScript);

    // 4. 创建 C++ 回调
    auto callback = std::make_shared<WebviewJavaScriptExecuteCallback>();

    // 5. 设置 FFI 回调包装器
    callback->SetCallback([callbackRef](const std::string& result) {
        // 创建返回数据
        RetDataCString ret = {
            .code = NWebError::NO_ERROR,
            .data = strdup(result.c_str())  // 复制字符串
        };

        // 调用 FFI 回调
        callbackRef(ret);
    });

    // 6. 执行 JavaScript
    ErrCode errCode = controller->RunJavaScript(script, callback);

    return errCode;
}
```

### 4. 对象管理

**对象 ID 管理**:

```cpp
// 全局对象映射
static std::unordered_map<int64_t, WebviewControllerImpl*> g_controllerMap;
static std::atomic<int64_t> g_nextControllerId{1};

// 创建对象
FFI_EXPORT int64_t FfiOHOSWebviewCtlConstructor()
{
    // 1. 创建控制器对象
    WebviewControllerImpl* controller = new WebviewControllerImpl();

    // 2. 分配 ID
    int64_t id = g_nextControllerId++;

    // 3. 存储到映射表
    g_controllerMap[id] = controller;

    // 4. 返回 ID 给调用者
    return id;
}

// 获取对象
WebviewControllerImpl* GetControllerById(int64_t id)
{
    auto it = g_controllerMap.find(id);
    if (it != g_controllerMap.end()) {
        return it->second;
    }
    return nullptr;
}

// 销毁对象
FFI_EXPORT void FfiOHOSWebviewCtlDestructor(int64_t id)
{
    auto it = g_controllerMap.find(id);
    if (it != g_controllerMap.end()) {
        delete it->second;
        g_controllerMap.erase(it);
    }
}
```

## 核心模块详解

### 1. WebviewController FFI

**头文件**: `include/webview_ffi.h`
**实现文件**: `src/webview_ffi.cpp`

**主要功能**:
- **对象管理**: Constructor, Destructor
- **页面导航**: LoadUrl, LoadData, Refresh, Stop
- **页面控制**: Forward, Backward, Zoom, Scroll
- **JavaScript**: RunJavaScript, RegisterJavaScriptProxy
- **状态查询**: GetUrl, GetTitle, GetProgress
- **事件处理**: OnActive, OnInactive

**核心函数示例**:

```cpp
// 获取 URL (返回字符串结构)
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetUrl(int64_t id)
{
    RetDataCString ret = {
        .code = NWebError::NWEB_ERROR,
        .data = nullptr
    };

    WebviewControllerImpl* controller = GetControllerById(id);
    if (!controller) {
        ret.code = NWebError::INIT_ERROR;
        return ret;
    }

    std::string url = controller->GetUrl();
    ret.code = NWebError::NO_ERROR;
    ret.data = strdup(url.c_str());  // 复制字符串

    return ret;
}

// 设置自定义 UserAgent
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetCustomUserAgent(int64_t id, char* cUserAgent)
{
    if (!cUserAgent) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    WebviewControllerImpl* controller = GetControllerById(id);
    if (!controller) {
        return NWebError::INIT_ERROR;
    }

    std::string userAgent(cUserAgent);
    return controller->SetCustomUserAgent(userAgent);
}
```

### 2. WebCookieManager FFI

**头文件**: `include/web_cookie_manager_ffi.h`
**实现文件**: `src/web_cookie_manager_ffi.cpp`

**主要功能**:
- Cookie 获取: `FfiOHOSWebviewCookieFetchCookie`
- Cookie 设置: `FfiOHOSWebviewCookieConfigCookie`
- Cookie 清除: `FfiOHOSWebviewCookieClearAllCookies`

### 3. WebDataBase FFI

**头文件**: `include/web_data_base_ffi.h`
**实现文件**: `src/web_data_base_ffi.cpp`

**主要功能**:
- 数据库管理
- 地理位置权限管理

### 4. WebStorage FFI

**头文件**: `include/web_storage_ffi.h`
**实现文件**: `src/web_storage_ffi.cpp`

**主要功能**:
- LocalStorage 管理
- SessionStorage 管理

### 5. WebDownload FFI

**头文件**: `include/web_download_ffi.h`
**实现文件**: `src/web_download_ffi.cpp`

**主要功能**:
- 下载项管理
- 下载代理设置
- 下载控制 (开始、暂停、取消)

## 跨语言使用示例

### Rust 使用示例

```rust
// FFI 声明
#[link(name = "cj_webview_ffi")]
extern "C" {
    fn FfiOHOSWebviewCtlConstructor() -> i64;
    fn FfiOHOSWebviewCtlLoadUrl(id: i64, url: *const c_char) -> i32;
    fn FfiOHOSWebviewCtlRefresh(id: i64) -> i32;
    fn FfiOHOSWebviewCtlGetUrl(id: i64) -> RetDataCString;
}

// Rust 包装器
struct WebviewController {
    id: i64,
}

impl WebviewController {
    fn new() -> Self {
        unsafe {
            let id = FfiOHOSWebviewCtlConstructor();
            WebviewController { id }
        }
    }

    fn load_url(&self, url: &str) -> Result<(), i32> {
        unsafe {
            let c_url = CString::new(url).unwrap();
            let result = FfiOHOSWebviewCtlLoadUrl(self.id, c_url.as_ptr());
            if result != 0 {
                Err(result)
            } else {
                Ok(())
            }
        }
    }

    fn refresh(&self) -> Result<(), i32> {
        unsafe {
            let result = FfiOHOSWebviewCtlRefresh(self.id);
            if result != 0 {
                Err(result)
            } else {
                Ok(())
            }
        }
    }

    fn get_url(&self) -> Result<String, i32> {
        unsafe {
            let ret = FfiOHOSWebviewCtlGetUrl(self.id);
            if ret.code != 0 {
                return Err(ret.code);
            }

            if ret.data.is_null() {
                return Ok(String::new());
            }

            let url = CStr::from_ptr(ret.data)
                .to_string_lossy()
                .into_owned();

            // 释放字符串 (需要提供释放函数)
            // FfiOHOSWebviewFreeCString(ret.data);

            Ok(url)
        }
    }
}

// 使用
fn main() {
    let controller = WebviewController::new();
    controller.load_url("https://www.example.com").unwrap();
    controller.refresh().unwrap();

    match controller.get_url() {
        Ok(url) => println!("Current URL: {}", url),
        Err(e) => eprintln!("Error: {}", e),
    }
}
```

### Python 使用示例 (使用 ctypes)

```python
import ctypes
from ctypes import c_int64, c_int32, c_char_p, c_bool, POINTER

# 加载库
lib = ctypes.CDLL("libcj_webview_ffi.so")

# 定义函数签名
lib.FfiOHOSWebviewCtlConstructor.restype = c_int64
lib.FfiOHOSWebviewCtlLoadUrl.argtypes = [c_int64, c_char_p]
lib.FfiOHOSWebviewCtlLoadUrl.restype = c_int32

lib.FfiOHOSWebviewCtlGetUrl.argtypes = [c_int64]
lib.FfiOHOSWebviewCtlGetUrl.restype = RetDataCString

# 定义返回结构
class RetDataCString(ctypes.Structure):
    _fields_ = [
        ("code", c_int32),
        ("data", c_char_p),
    ]

# Python 包装器
class WebviewController:
    def __init__(self):
        self.id = lib.FfiOHOSWebviewCtlConstructor()

    def load_url(self, url):
        result = lib.FfiOHOSWebviewCtlLoadUrl(self.id, url.encode('utf-8'))
        if result != 0:
            raise Exception(f"LoadUrl failed with error: {result}")

    def get_url(self):
        ret = lib.FfiOHOSWebviewCtlGetUrl(self.id)
        if ret.code != 0:
            raise Exception(f"GetUrl failed with error: {ret.code}")

        if ret.data:
            url = ctypes.string_at(ret.data).decode('utf-8')
            # 需要提供释放函数
            # lib.FfiOHOSWebviewFreeCString(ret.data)
            return url
        return ""

# 使用
controller = WebviewController()
controller.load_url("https://www.example.com")
print(f"Current URL: {controller.get_url()}")
```

### C++ 使用示例

```cpp
#include "webview_ffi.h"
#include <iostream>

int main()
{
    // 1. 创建控制器
    int64_t id = FfiOHOSWebviewCtlConstructor();

    // 2. 加载 URL
    int32_t result = FfiOHOSWebviewCtlLoadUrl(id, "https://www.example.com");
    if (result != 0) {
        std::cerr << "LoadUrl failed: " << result << std::endl;
        return 1;
    }

    // 3. 获取当前 URL
    RetDataCString ret = FfiOHOSWebviewCtlGetUrl(id);
    if (ret.code == 0 && ret.data) {
        std::cout << "Current URL: " << ret.data << std::endl;
        free(ret.data);  // 释放字符串
    }

    // 4. 刷新页面
    result = FfiOHOSWebviewCtlRefresh(id);
    if (result != 0) {
        std::cerr << "Refresh failed: " << result << std::endl;
    }

    // 5. 销毁控制器
    // FfiOHOSWebviewCtlDestructor(id);

    return 0;
}
```

## 数据类型转换

### 字符串处理

**C → 跨语言**:
```cpp
// 返回字符串 (使用 strdup 复制)
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetTitle(int64_t id)
{
    RetDataCString ret = { .code = NWebError::NWEB_ERROR, .data = nullptr };

    std::string title = controller->GetTitle();
    ret.code = NWebError::NO_ERROR;
    ret.data = strdup(title.c_str());  // 复制到堆内存

    return ret;
}
```

**跨语言 → C**:
```cpp
// 接收字符串 (假设有效)
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char* url)
{
    if (!url) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    // 创建 std::string (复制)
    std::string urlStr(url);

    return controller->LoadUrl(urlStr);
}
```

### 数组处理

**传递数组到 FFI**:
```cpp
// 字符串数组结构
struct CArrString {
    char** data;
    int64_t size;
};

// 使用示例
FFI_EXPORT int32_t FfiOHOSWebviewCtlRegisterJavaScriptProxy(
    int64_t id,
    CArrI64 cFuncIds,
    const char* cName,
    CArrString cMethodList)
{
    // 1. 转换方法列表
    std::vector<std::string> methods;
    for (int64_t i = 0; i < cMethodList.size; i++) {
        if (cMethodList.data[i]) {
            methods.push_back(std::string(cMethodList.data[i]));
        }
    }

    // 2. 调用核心引擎
    return controller->RegisterJavaScriptProxy(funcIds, cName, methods);
}
```

## 错误处理

### 错误码定义

使用 nativecommon 的错误码系统:

```cpp
namespace NWebError {
constexpr ErrCode NO_ERROR = 0;
constexpr ErrCode PARAM_CHECK_ERROR = 401;
constexpr ErrCode INVALID_URL = 17100002;
constexpr ErrCode INIT_ERROR = 17100001;
}
```

### 返回错误

```cpp
// 方式 1: 返回错误码
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char* url)
{
    if (!url) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    // ...
    return NWebError::NO_ERROR;
}

// 方式 2: 返回结构体包含错误码
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetUrl(int64_t id)
{
    RetDataCString ret = {
        .code = NWebError::NWEB_ERROR,
        .data = nullptr
    };

    auto controller = GetControllerById(id);
    if (!controller) {
        ret.code = NWebError::INIT_ERROR;
        return ret;
    }

    std::string url = controller->GetUrl();
    ret.code = NWebError::NO_ERROR;
    ret.data = strdup(url.c_str());

    return ret;
}
```

### 错误处理示例 (Rust)

```rust
fn load_url(&self, url: &str) -> Result<(), i32> {
    unsafe {
        let c_url = CString::new(url).unwrap();
        let result = FfiOHOSWebviewCtlLoadUrl(self.id, c_url.as_ptr());

        match result {
            0 => Ok(()),
            401 => Err(401),  // PARAM_CHECK_ERROR
            17100002 => Err(17100002),  // INVALID_URL
            code => Err(code),
        }
    }
}
```

## 内存管理

### 内存分配策略

**基本原则**:
- **返回的字符串**: 使用 `strdup()` 复制到堆内存
- **接收的字符串**: 假设有效,立即复制到 `std::string`
- **回调数据**: 调用者负责释放

**释放机制**:

```cpp
// 1. 字符串释放 (需要导出)
FFI_EXPORT void FfiOHOSWebviewFreeCString(char* str)
{
    if (str) {
        free(str);
    }
}

// 2. 数组释放
FFI_EXPORT void FfiOHOSWebviewFreeCStringArray(CArrCString* arr)
{
    if (arr) {
        for (int64_t i = 0; i < arr->size; i++) {
            if (arr->data[i]) {
                free(arr->data[i]);
            }
        }
        free(arr->data);
    }
}

// 3. 对象释放
FFI_EXPORT void FfiOHOSWebviewCtlDestructor(int64_t id)
{
    auto it = g_controllerMap.find(id);
    if (it != g_controllerMap.end()) {
        delete it->second;
        g_controllerMap.erase(it);
    }
}
```

### Rust 内存管理

```rust
impl Drop for WebviewController {
    fn drop(&mut self) {
        unsafe {
            FfiOHOSWebviewCtlDestructor(self.id);
        }
    }
}

// 字符串自动释放
struct CStringWrapper {
    ptr: *const c_char,
}

impl CStringWrapper {
    fn new(ptr: *const c_char) -> Self {
        CStringWrapper { ptr }
    }

    fn as_str(&self) -> &str {
        unsafe {
            CStr::from_ptr(self.ptr)
                .to_str()
                .unwrap_or("")
        }
    }
}

impl Drop for CStringWrapper {
    fn drop(&mut self) {
        unsafe {
            FfiOHOSWebviewFreeCString(self.ptr as *mut c_char);
        }
    }
}
```

## 构建和集成

### 构建配置 (BUILD.gn)

```gni
ohos_shared_library("cj_webview_ffi") {
    sources = [
        "src/webview_ffi.cpp",
        "src/web_cookie_manager_ffi.cpp",
        "src/web_data_base_ffi.cpp",
        # ... 更多源文件
    ]

    include_dirs = [
        "include",
        # ... 更多头文件路径
    ]

    deps = [
        "../../../ohos_nweb:libnweb",
        "../../native:ohweb",
    ]

    external_deps = [
        "hilog:libhilog",
        "napi:cj_bind_ffi",
        "napi:cj_bind_native",
    ]

    part_name = "webview"
    subsystem_name = "web"
}
```

### 构建命令

```bash
# 构建 CJ FFI 库
./build.sh --product-name <产品名> \
    --build-target //base/web/webview/interfaces/kits/cj:cj_webview_ffi

# 输出
# libcj_webview_ffi.so (安装到 /usr/lib/)
```

### 集成到 Rust 项目

**Cargo.toml**:
```toml
[package]
name = "arkweb_ffi"
version = "0.1.0"
edition = "2021"

[dependencies]
libc = "0.2"

[build-dependencies]
cc = "1.0"
```

**build.rs**:
```rust
fn main() {
    println!("cargo:rustc-link-search=/usr/lib");
    println!("cargo:rustc-link-lib=cj_webview_ffi");
    println!("cargo:rerun-if-changed=build.rs");
}
```

## 与 nativecommon 的关系

### 不使用 nativecommon

CJ FFI 接口 **不使用** nativecommon 层:

**原因**:
1. **C 兼容性**: nativecommon 使用 C++ 特性 (shared_ptr, 引用计数),不兼容纯 C
2. **轻量级**: FFI 需要最小化依赖
3. **多语言支持**: C++ 对象难以直接映射到其他语言

**替代方案**:
- **对象 ID 管理**: 使用 `int64_t ID` 代替对象指针
- **手动内存管理**: 使用 `strdup()` 和 `free()`
- **C 结构体**: 使用 FFI 兼容的结构体

**对比**:

```
NAPI/ANI 路径:
应用 → NAPI/ANI → nativecommon (shared_ptr + 引用计数) → ohos_nweb

CJ FFI 路径:
应用 → CJ FFI (C 函数) → ohos_nweb (直接调用)
```

## 性能考虑

### 优势

1. **零拷贝潜力**: 可以实现零拷贝的数据传递
2. **编译优化**: 编译器可以优化跨语言调用
3. **低开销**: 最小运行时开销

### 劣势

1. **字符串拷贝**: `strdup()` 需要复制字符串
2. **手动管理**: 需要手动释放内存
3. **类型不安全**: C FFI 缺乏类型检查

### 优化建议

```rust
// ❌ 多次调用
for _ in 0..1000 {
    controller.run_javascript("setValue(1)");
}

// ✅ 批量操作
controller.run_javascript("
    for (let i = 0; i < 1000; i++) {
        setValue(i);
    }
");
```

## 调试技巧

### 日志查看

```bash
# 查看 CJ FFI 日志
hilog -T WebView

# 查看 FFI 调用日志
hilog -T WebView | grep "FFI"
```

### 常见问题

**Q: 对象 ID 无效**

可能原因:
1. 对象已销毁
2. ID 类型错误 (int32 vs int64)
3. 未调用 Constructor

解决方法:
```cpp
// 始终检查 ID 有效性
WebviewControllerImpl* controller = GetControllerById(id);
if (!controller) {
    WEBVIEWLOGE("Invalid controller ID: %{public}lld", id);
    return NWebError::INIT_ERROR;
}
```

**Q: 字符串乱码**

可能原因:
1. 编码不匹配
2. 字符串未正确终止
3. 内存已释放

解决方法:
```cpp
// 确保 UTF-8 编码
std::string urlStr(url);  // 假设是 UTF-8

// 复制字符串
ret.data = strdup(urlStr.c_str());

// 使用后释放
free(ret.data);
```

**Q: 内存泄漏**

可能原因:
1. 未释放返回的字符串
2. 未销毁对象
3. 循环引用

解决方法:
```cpp
// 1. 提供释放函数
FFI_EXPORT void FfiOHOSWebviewFreeCString(char* str);

// 2. 调用者负责释放
let ret = FfiOHOSWebviewCtlGetUrl(id);
FfiOHOSWebviewFreeCString(ret.data);

// 3. 使用 RAII (Rust)
impl Drop for CStringWrapper {
    fn drop(&mut self) {
        FfiOHOSWebviewFreeCString(self.ptr);
    }
}
```

## 最佳实践

### 1. 参数验证

```cpp
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char* url)
{
    // 1. 检查指针
    if (!url) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    // 2. 检查对象
    WebviewControllerImpl* controller = GetControllerById(id);
    if (!controller) {
        return NWebError::INIT_ERROR;
    }

    // 3. 验证参数
    std::string urlStr(url);
    if (!CheckUrl(urlStr)) {
        return NWebError::INVALID_URL;
    }

    // ... 处理逻辑
}
```

### 2. 内存安全

```cpp
// 1. 使用 strdup 复制返回值
ret.data = strdup(string.c_str());

// 2. 导出释放函数
FFI_EXPORT void FfiOHOSWebviewFreeCString(char* str)
{
    if (str) {
        free(str);
    }
}

// 3. 文档化内存所有权
/**
 * @return 返回的字符串必须使用 FfiOHOSWebviewFreeCString() 释放
 */
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetUrl(int64_t id);
```

### 3. 错误处理

```cpp
// 1. 统一错误码
namespace NWebError {
constexpr ErrCode NO_ERROR = 0;
constexpr ErrCode PARAM_CHECK_ERROR = 401;
}

// 2. 返回详细错误
RetDataCString ret = {
    .code = NWebError::INVALID_URL,
    .data = nullptr
};

// 3. 调用者检查错误
if (ret.code != 0) {
    // 处理错误
}
```

### 4. 线程安全

```cpp
// 1. 使用互斥锁保护全局状态
std::mutex g_controllerMapMutex;

WebviewControllerImpl* GetControllerById(int64_t id)
{
    std::lock_guard<std::mutex> lock(g_controllerMapMutex);
    auto it = g_controllerMap.find(id);
    return (it != g_controllerMap.end()) ? it->second : nullptr;
}

// 2. 避免在回调中持有锁
```

## 相关文档

- **[../../native/README.md](../../native/README.md)** - Native C++ 接口文档
- **[../napi/README.md](../napi/README.md)** - NAPI 接口文档
- **[../ani/README.md](../ani/README.md)** - ANI 接口文档
- **[../nativecommon/README.md](../nativecommon/README.md)** - nativecommon 通用组件库文档
- **[../../../ohos_nweb/README.md](../../../ohos_nweb/README.md)** - 核心引擎文档
- **[../../../AGENT.md](../../../AGENT.md)** - 代码仓指南
