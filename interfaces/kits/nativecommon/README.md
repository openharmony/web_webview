# interfaces/kits/nativecommon - ArkWeb 通用组件库

本目录提供 ArkWeb 接口层 (NAPI 和 Native) 的通用组件和数据结构实现,作为接口层与核心引擎层之间的桥梁。

## 概述

### 核心功能

nativecommon 提供以下能力:
- **通用数据结构** - 封装核心引擎数据结构为接口层可用格式
- **引用计数管理** - 跨语言边界的对象生命周期管理
- **错误处理** - 统一的错误码和错误消息系统
- **类型转换** - Native 类型与 NAPI 类型的适配层
- **共享基础库** - NAPI 和 Native 接口共用的基础组件

### 设计目标

- **代码复用** - 避免在 NAPI 和 Native 中重复实现相同逻辑
- **类型安全** - 提供类型安全的封装
- **内存安全** - 通过引用计数防止内存泄漏和悬空指针
- **解耦** - 隔离接口层与核心引擎层的直接依赖

## 架构设计

### 位置和作用

```
interfaces/kits/napi/ (NAPI 接口)
  ↓ 依赖
interfaces/kits/nativecommon/ (通用组件) ← 当前目录
  ↓ 依赖
ohos_nweb/ (核心引擎)

interfaces/native/ (Native 接口)
  ↓ 依赖
interfaces/kits/nativecommon/ (通用组件) ← 当前目录
```

### 核心组件分类

| 组件类别 | 文件 | 功能 |
|---------|------|------|
| **数据结构封装** | `web_history_list.*` | 历史记录列表 |
| **数据结构封装** | `web_message_port.*` | 消息端口 |
| **数据结构封装** | `web_resource_handler.*` | 资源处理器 |
| **数据结构封装** | `web_scheme_handler_request.*` | 自定义协议请求 |
| **数据结构封装** | `web_scheme_handler_response.*` | 自定义协议响应 |
| **元数据封装** | `nweb_user_agent_brand_version_impl.*` | User-Agent 品牌 |
| **元数据封装** | `nweb_user_agent_metadata_impl.*` | User-Agent 元数据 |
| **媒体处理** | `napi_native_mediaplayer_handler_impl.*` | 媒体播放器 |
| **工具类** | `web_errors.*` | 错误处理 |
| **工具类** | `transfer_referenced_count.h` | 引用计数 |

## 核心组件详解

### 1. 引用计数管理 (transfer_referenced_count.h)

**作用**: 为跨语言传输的对象提供引用计数管理,防止对象过早释放或内存泄漏。

```cpp
class TransferReferencedCount {
public:
    TransferReferencedCount() = default;
    ~TransferReferencedCount() = default;

    // 增加引用计数
    void IncRefCount() { referencedCount_++; }

    // 减少引用计数,返回剩余计数
    int32_t DecRefCount() {
        referencedCount_--;
        return referencedCount_;
    }

    // 获取当前引用计数
    int32_t RefCount() { return referencedCount_; }

private:
    int32_t referencedCount_ = 0;
};
```

**使用场景**:
- NAPI 对象传递到 JavaScript
- Native 接口对象返回给 C++ 应用
- 对象在多个上下文中共享

**使用示例**:

```cpp
// 1. 创建对象并增加引用
WebHistoryList* historyList = new WebHistoryList(sptrHistoryList);
historyList->IncRefCount();

// 2. 包装为 NAPI 对象
napi_wrap(env, jsObject, historyList, [](napi_env env, void* data, void* hint) {
    WebHistoryList* obj = static_cast<WebHistoryList*>(data);
    if (obj->DecRefCount() == 0) {
        delete obj;  // 引用计数为 0 时释放
    }
}, nullptr, nullptr);

// 3. 在其他地方使用时增加引用
historyList->IncRefCount();

// 4. 使用完毕减少引用
if (historyList->DecRefCount() == 0) {
    delete historyList;
}
```

### 2. 错误处理系统 (web_errors.h/cpp)

**作用**: 提供统一的错误码定义和错误消息格式化功能。

#### 错误码定义

```cpp
namespace NWebError {
constexpr ErrCode NO_ERROR = 0;                       // 成功
constexpr ErrCode PARAM_CHECK_ERROR = 401;            // 参数检查错误
constexpr ErrCode INIT_ERROR = 17100001;              // 初始化错误
constexpr ErrCode INVALID_URL = 17100002;             // 无效 URL
constexpr ErrCode INVALID_RESOURCE = 17100003;        // 无效资源
constexpr ErrCode FUNCTION_NOT_ENABLE = 17100004;     // 功能未启用
constexpr ErrCode INVALID_COOKIE_VALUE = 17100005;    // 无效 Cookie 值
constexpr ErrCode CAN_NOT_POST_MESSAGE = 17100010;    // 无法发送消息
constexpr ErrCode INVALID_ORIGIN = 17100011;          // 无效源
constexpr ErrCode NWEB_ERROR = -1;                    // 一般错误

// 获取错误消息
std::string GetErrMsgByErrCode(ErrCode code);

// 格式化错误消息
std::string FormatString(const char *errorMsgTemplate, ...);
}
```

#### 错误消息模板

```cpp
namespace ParamCheckErrorMsgTemplate {
    extern const char* TYPE_ERROR;              // 类型错误
    extern const char* TYPE_ALL_STRING;         // 所有参数必须是字符串
    extern const char* TYPE_ALL_INT;            // 所有参数必须是整数
    extern const char* PARAM_TYEPS_ERROR;       // 参数类型错误
    extern const char* PARAM_NUMBERS_ERROR_ONE; // 参数数量错误 (1 个)
    extern const char* PARAM_NUMBERS_ERROR_TWO; // 参数数量错误 (2 个)
    extern const char* PARAM_NOT_NULL;          // 参数不能为空
    extern const char* PARAM_TYPE_INVALID;      // 参数类型无效
}
```

#### 使用示例

```cpp
// 1. 返回错误码
if (url.empty()) {
    return NWebError::INVALID_URL;
}

// 2. 获取错误消息
std::string errMsg = NWebError::GetErrMsgByErrCode(NWebError::INVALID_URL);

// 3. 格式化自定义错误消息
std::string formattedMsg = NWebError::FormatString(
    "Invalid URL: %s, length: %d",
    url.c_str(),
    url.length()
);

// 4. 在 NAPI 中抛出错误
BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
```

### 3. WebHistoryList (web_history_list.*)

**作用**: 封装历史记录列表,提供索引访问和列表大小查询。

```cpp
class WebHistoryList : public TransferReferencedCount {
public:
    explicit WebHistoryList(std::shared_ptr<NWebHistoryList> sptrHistoryList);

    // 获取当前历史记录索引
    int32_t GetCurrentIndex();

    // 获取指定索引的历史记录项
    std::shared_ptr<NWebHistoryItem> GetItem(int32_t index);

    // 获取历史记录列表大小
    int32_t GetListSize();

private:
    std::shared_ptr<NWebHistoryList> sptrHistoryList_;
};
```

**使用场景**:
- NAPI: 实现返回给 JavaScript 的 WebHistoryList 对象
- Native: 实现返回给 C++ 应用的历史记录列表

**数据流**:
```
ohos_nweb (核心引擎)
  ↓ std::shared_ptr<NWebHistoryList>
nativecommon (封装层)
  ↓ WebHistoryList
NAPI/Native (接口层)
  ↓ napi_value / C++ 对象
应用层
```

### 4. WebMessagePort (web_message_port.*)

**作用**: 封装消息端口,提供跨上下文的消息通信能力。

```cpp
class WebMessagePort : public TransferReferencedCount {
public:
    WebMessagePort(int32_t nwebId, std::string& port, bool isExtentionType);

    // 关闭端口
    ErrCode ClosePort();

    // 发送消息
    ErrCode PostPortMessage(std::shared_ptr<NWebMessage> data,
                            std::shared_ptr<NWebRomValue> value);

    // 设置消息回调
    ErrCode SetPortMessageCallback(std::shared_ptr<NWebMessageValueCallback> callback);

    // 获取端口句柄
    std::string GetPortHandle() const;

    // 是否为扩展类型
    bool IsExtentionType() { return isExtentionType_; }

private:
    int32_t nwebId_;
    std::string portHandle_;
    bool isExtentionType_;
};
```

**关键特性**:
- **扩展类型支持**: `isExtentionType_` 标识是否支持扩展消息格式
- **句柄管理**: 通过 `portHandle_` 字符串标识端口
- **回调机制**: 支持设置消息接收回调

**消息流**:
```
应用层 (JavaScript/C++)
  ↓ 发送消息
WebMessagePort::PostPortMessage()
  ↓ 调用核心引擎
ohos_nweb::NWeb::PostWebMessage()
  ↓ 传递给
ArkWebCore.hap (Chromium)
  ↓ 接收消息
ArkWebCore.hap
  ↓ 触发回调
ohos_nweb callback
  ↓ 传递给
WebMessagePort::SetPortMessageCallback()
  ↓ 返回给
应用层
```

### 5. WebSchemeHandlerRequest (web_scheme_handler_request.*)

**作用**: 封装自定义协议请求,为应用提供处理自定义协议的能力。

```cpp
class WebSchemeHandlerRequest : public RefBase {
public:
    explicit WebSchemeHandlerRequest(const ArkWeb_ResourceRequest* request);

    // 获取请求 URL
    char* GetRequestUrl();

    // 获取请求方法 (GET/POST/等)
    char* GetMethod();

    // 获取 Referrer
    char* GetReferrer();

    // 是否为重定向
    bool IsRedirect();

    // 是否为主框架
    bool IsMainFrame();

    // 是否有用户手势
    bool HasGesture();

    // 获取请求头列表
    const WebHeaderList& GetHeader();

    // 获取 HTTP Body 流
    ArkWeb_HttpBodyStream* GetHttpBodyStream();

    // 获取资源类型
    int32_t GetRequestResourceType();

    // 获取框架 URL
    char* GetFrameUrl();

private:
    char* url_;
    char* method_;
    char* referrer_;
    bool isRedirect_;
    bool isMainFrame_;
    bool hasGesture_;
    WebHeaderList headerList_;
    ArkWeb_HttpBodyStream* stream_;
    int32_t requestResourceType_;
    char* frameUrl_;
};
```

**使用场景**:
- 自定义协议处理 (如 `app://`, `custom://`)
- 资源拦截和修改
- 本地资源加载

**使用流程**:
```cpp
// 1. 应用注册自定义协议处理器
controller->SetWebSchemeHandler("app", handler);

// 2. WebView 发起自定义协议请求
// 例如: <img src="app://assets/logo.png">

// 3. 核心引擎创建 WebSchemeHandlerRequest
WebSchemeHandlerRequest request(rawRequest);

// 4. 应用处理器接收请求
char* url = request.GetRequestUrl();          // "app://assets/logo.png"
char* method = request.GetMethod();          // "GET"
bool isMain = request.IsMainFrame();          // false (图片资源)

// 5. 应用读取请求并返回响应
// 读取请求体、处理请求头、生成响应数据
```

### 6. WebSchemeHandlerResponse (web_scheme_handler_response.*)

**作用**: 封装自定义协议响应,应用通过此类返回响应数据。

```cpp
class WebSchemeHandlerResponse : public RefBase {
public:
    WebSchemeHandlerResponse();

    // 设置响应状态码
    void SetStatusCode(int32_t statusCode);

    // 设置响应状态文本
    void SetStatusText(const char* statusText);

    // 设置响应头
    void SetResponseHeader(const char* key, const char* value);

    // 设置响应数据
    void SetResponseData(const char* data, size_t dataLength);

    // 获取响应状态码
    int32_t GetStatusCode() const;

    // 获取响应状态文本
    const char* GetStatusText() const;

    // 获取响应头
    const WebHeaderList& GetHeader() const;

    // 获取响应数据
    const char* GetResponseData() const;

    // 获取响应数据长度
    size_t GetResponseDataLength() const;
};
```

**使用示例**:
```cpp
// 1. 创建响应对象
WebSchemeHandlerResponse response;

// 2. 设置状态码和状态文本
response.SetStatusCode(200);
response.SetStatusText("OK");

// 3. 设置响应头
response.SetResponseHeader("Content-Type", "image/png");
response.SetResponseHeader("Cache-Control", "max-age=3600");

// 4. 设置响应数据
std::vector<uint8_t> imageData = ReadImageFile("logo.png");
response.SetResponseData(
    reinterpret_cast<const char*>(imageData.data()),
    imageData.size()
);

// 5. 返回响应
// 核心引擎将响应数据发送给 WebView 渲染
```

### 7. User-Agent 元数据封装

#### nweb_user_agent_brand_version_impl.*

**作用**: 封装 User-Agent 品牌版本信息。

```cpp
class NWebUserAgentBrandVersionImpl : public NWebUserAgentBrandVersion {
public:
    NWebUserAgentBrandVersionImpl(
        const std::string& majorVersion,
        const std::string& minorVersion);

    // 获取主版本号
    std::string GetMajorVersion() const override;

    // 获取次版本号
    std::string GetMinorVersion() const override;

private:
    std::string majorVersion_;
    std::string minorVersion_;
};
```

#### nweb_user_agent_metadata_impl.*

**作用**: 封装 User-Agent 元数据,支持更精细的 User-Agent 控制。

```cpp
class NWebUserAgentMetadataImpl : public NWebUserAgentMetadata {
public:
    NWebUserAgentMetadataImpl();

    // 设置品牌版本
    void SetBrowsersVersion(
        const std::string& brand,
        const std::string& version) override;

    // 设置架构
    void SetArchitecture(const std::string& architecture) override;

    // 设置设备模型
    void SetDeviceModel(const std::string& model) override;

    // 设置操作系统
    void SetOperatingSystem(const std::string& os) override;

    // 获取品牌版本列表
    const std::vector<std::pair<std::string, std::string>>&
    GetBrowsersVersion() const override;

    // 获取架构
    const std::string& GetArchitecture() const override;

    // 获取设备模型
    const std::string& GetDeviceModel() const override;

    // 获取操作系统
    const std::string& GetOperatingSystem() const override;

private:
    std::vector<std::pair<std::string, std::string>> browsersVersion_;
    std::string architecture_;
    std::string deviceModel_;
    std::string operatingSystem_;
};
```

**使用场景**:
- 自定义 User-Agent 字符串
- 模拟不同浏览器
- 适配特定网站

**使用示例**:
```cpp
// 1. 创建 User-Agent 元数据
auto metadata = std::make_shared<NWebUserAgentMetadataImpl>();

// 2. 设置浏览器品牌和版本
metadata->SetBrowsersVersion("Chrome", "120.0.6099.109");
metadata->SetBrowsersVersion("Edg", "120.0.2210.61");

// 3. 设置架构和设备
metadata->SetArchitecture("arm64");
metadata->SetDeviceModel("Huawei Mate 60");
metadata->SetOperatingSystem("HarmonyOS 4.0");

// 4. 应用到控制器
controller->SetUserAgentMetadata(metadata);

// 5. 生成的 User-Agent 类似:
// Mozilla/5.0 (HarmonyOS 4.0; Huawei Mate 60) AppleWebKit/537.36
// (KHTML, like Gecko) Chrome/120.0.6099.109 Safari/537.36 Edg/120.0.2210.61
```

### 8. 媒体播放器处理 (napi_native_mediaplayer_handler_impl.*)

**作用**: 封装媒体播放器处理器,支持 WebView 中的音视频播放控制。

```cpp
class NapiNativeMediaPlayerHandlerImpl : public NapiNativeMediaPlayerHandler {
public:
    NapiNativeMediaPlayerHandlerImpl(int32_t nwebId, const std::string& src);

    // 播放
    void Play() override;

    // 暂停
    void Pause() override;

    // 跳转到指定位置
    void SeekTo(double time) override;

    // 设置播放速度
    void SetPlaybackSpeed(double speed) override;

    // 获取当前播放位置
    double CurrentTime() override;

    // 获取媒体时长
    double Duration() override;

    // 设置音量
    void SetVolume(double volume) override;

private:
    int32_t nwebId_;
    std::string src_;
};
```

## 数据流转过程

### NAPI 接口数据流

```
ArkTS/JavaScript 应用
  ↓ (napi_value)
NAPI 层 (interfaces/kits/napi/)
  ↓ 解析参数
nativecommon 层 (当前目录)
  ↓ 转换为核心引擎类型
ohos_nweb/ (核心引擎)
  ↓ 调用 Chromium
ArkWebCore.hap
```

### 示例: 历史记录查询

**JavaScript 调用**:
```javascript
let history = controller.getBackForwardEntries();
let currentIndex = history.currentIndex;
let item = history.getItem(0);
```

**数据流**:

1. **NAPI 层** (napi_webview_controller.cpp):
```cpp
static napi_value GetBackForwardEntries(napi_env env, napi_callback_info info)
{
    // 解包 C++ 控制器对象
    WebviewController* webviewController = nullptr;
    napi_unwrap(env, thisVar, (void**)&webviewController);

    // 调用核心引擎
    std::shared_ptr<NWebHistoryList> nwebHistoryList =
        webviewController->GetBackForwardEntries();

    // 创建 nativecommon 封装对象
    WebHistoryList* historyList = new WebHistoryList(nwebHistoryList);
    historyList->IncRefCount();  // 增加引用计数

    // 包装为 JS 对象
    napi_value jsHistoryList;
    napi_create_object(env, &jsHistoryList);
    napi_wrap(env, jsHistoryList, historyList, /* deleter */);

    return jsHistoryList;
}
```

2. **nativecommon 层** (web_history_list.cpp):
```cpp
int32_t WebHistoryList::GetCurrentIndex()
{
    if (!sptrHistoryList_) {
        return -1;
    }
    return sptrHistoryList_->GetCurrentIndex();
}

std::shared_ptr<NWebHistoryItem> WebHistoryList::GetItem(int32_t index)
{
    if (!sptrHistoryList_) {
        return nullptr;
    }
    return sptrHistoryList_->GetItem(index);
}
```

3. **核心引擎层** (ohos_nweb):
```cpp
std::shared_ptr<NWebHistoryList> WebviewController::GetBackForwardEntries()
{
    if (!nweb_) {
        return nullptr;
    }
    return nweb_->GetBackForwardEntries();
}
```

### Native 接口数据流

```
C++ 应用
  ↓ 调用 C API
Native 接口 (interfaces/native/)
  ↓ 转换参数
nativecommon 层 (当前目录)
  ↓ 转换为核心引擎类型
ohos_nweb/ (核心引擎)
```

## 构建和集成

### 构建配置 (BUILD.gn)

```gni
ohos_shared_library("webview_common") {
  sources = [
    "web_history_list.cpp",
    "web_message_port.cpp",
    "web_errors.cpp",
    # ... 其他源文件
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
# 构建 nativecommon 库
./build.sh --product-name <产品名> \
    --build-target //base/web/webview/interfaces/kits/nativecommon:webview_common

# 输出
# libwebview_common.so (安装到 /usr/lib/)
```

### 依赖关系

**被依赖方**:
- `interfaces/kits/napi:webview_napi` (NAPI 接口库)
- `interfaces/native:ohweb` (Native 接口库)

**依赖方**:
- `ohos_nweb:libnweb` (核心引擎)
- `ohos_adapter:nweb_ohos_adapter` (适配器)

## 设计模式和原则

### 1. 适配器模式

**作用**: 将核心引擎的数据结构适配为接口层可用的格式。

```cpp
// 核心引擎类型
std::shared_ptr<NWebHistoryList>

// 适配为接口层类型
WebHistoryList (继承 TransferReferencedCount)
```

### 2. 引用计数模式

**作用**: 管理跨语言边界的对象生命周期。

```cpp
class WebHistoryList : public TransferReferencedCount {
    // 每次 napi_wrap 时 IncRefCount()
    // 每次 deleter 调用时 DecRefCount()
    // 计数为 0 时释放对象
};
```

### 3. 工厂模式 (间接)

**作用**: NAPI 层的构造函数创建 nativecommon 对象。

```cpp
static napi_value GetBackForwardEntries(napi_env env, napi_callback_info info)
{
    // 创建 WebHistoryList 对象
    WebHistoryList* historyList = new WebHistoryList(nwebHistoryList);
    // ...
}
```

### 4. RAII (资源获取即初始化)

**作用**: 使用智能指针自动管理资源。

```cpp
// 使用 shared_ptr 自动管理生命周期
std::shared_ptr<NWebHistoryList> sptrHistoryList_;
std::shared_ptr<NWebMessage> data_;
```

## 与其他组件的关系

### 与 NAPI 层的关系

```
NAPI 层 (interfaces/kits/napi/)
  - 创建 nativecommon 对象
  - 包装为 napi_value
  - 通过 napi_wrap 绑定生命周期
  - 在 deleter 中调用 DecRefCount()
```

### 与 Native 层的关系

```
Native 层 (interfaces/native/)
  - 创建 nativecommon 对象
  - 返回指针给 C++ 应用
  - 文档化引用计数管理规则
```

### 与核心引擎层的关系

```
核心引擎层 (ohos_nweb/)
  - 提供 std::shared_ptr<T> 类型
  - nativecommon 持有 shared_ptr 引用
  - 通过 shared_ptr 访问核心功能
```

## 最佳实践

### 1. 引用计数管理

```cpp
// ✅ 正确: 创建时增加引用
WebHistoryList* list = new WebHistoryList(nwebList);
list->IncRefCount();
napi_wrap(env, jsObj, list, [](napi_env env, void* data, void* hint) {
    WebHistoryList* obj = static_cast<WebHistoryList*>(data);
    if (obj->DecRefCount() == 0) {
        delete obj;
    }
}, nullptr, nullptr);

// ❌ 错误: 忘记增加引用
WebHistoryList* list = new WebHistoryList(nwebList);
napi_wrap(env, jsObj, list, [](napi_env env, void* data, void* hint) {
    // 引用计数为 0,导致提前释放
    delete static_cast<WebHistoryList*>(data);
}, nullptr, nullptr);
```

### 2. 错误处理

```cpp
// ✅ 正确: 统一使用错误码
if (index < 0 || index >= GetListSize()) {
    return NWebError::PARAM_CHECK_ERROR;
}

// ✅ 正确: 使用格式化错误消息
std::string errMsg = NWebError::FormatString(
    "Index %d out of range [0, %d)",
    index,
    GetListSize()
);

// ❌ 错误: 硬编码错误消息
return "Index out of range";
```

### 3. 空指针检查

```cpp
// ✅ 正确: 始终检查 shared_ptr
int32_t WebHistoryList::GetCurrentIndex()
{
    if (!sptrHistoryList_) {
        return -1;  // 返回默认值
    }
    return sptrHistoryList_->GetCurrentIndex();
}

// ❌ 错误: 未检查空指针
int32_t WebHistoryList::GetCurrentIndex()
{
    return sptrHistoryList_->GetCurrentIndex();  // 可能崩溃
}
```

### 4. 字符串管理

```cpp
// ✅ 正确: 返回 const char* 避免拷贝
const char* WebSchemeHandlerRequest::GetRequestUrl() const
{
    return url_;  // 内部管理的字符串
}

// ❌ 错误: 返回临时指针
char* WebSchemeHandlerRequest::GetRequestUrl()
{
    std::string temp = "http://example.com";
    return temp.c_str();  // 临时对象,悬空指针
}
```

## 调试技巧

### 查看日志

```bash
# 查看 nativecommon 日志
hilog -T ArkWeb

# 查看引用计数
# 在代码中添加调试日志
WVLOG_I("Ref count after wrap: %{public}d", obj->RefCount());
```

### 常见问题

**Q: 内存泄漏**

可能原因:
1. 引用计数不平衡 (IncRefCount 多于 DecRefCount)
2. shared_ptr 循环引用
3. napi_wrap deleter 未调用

解决方法:
```cpp
// 检查引用计数
WVLOG_I("Ref count: %{public}d", obj->RefCount());

// 使用 weak_ptr 打破循环引用
std::weak_ptr<NWebHistoryList> weakList = sptrHistoryList_;
```

**Q: 崩溃或访问违例**

可能原因:
1. 未检查 shared_ptr 是否为空
2. 对象已被释放 (引用计数为 0)
3. 跨线程访问

解决方法:
```cpp
// 始终检查空指针
if (!sptrHistoryList_) {
    WVLOG_E("HistoryList is null");
    return nullptr;
}

// 使用互斥锁保护
std::lock_guard<std::mutex> lock(mutex_);
```

**Q: 类型转换错误**

可能原因:
1. 错误的 napi_unwrap 类型
2. 错误的 static_cast 类型

解决方法:
```cpp
// 使用 RTTI 检查类型
WebHistoryList* list = dynamic_cast<WebHistoryList*>(data);
if (!list) {
    WVLOG_E("Invalid type cast");
    return;
}
```

## 相关文档

- **[../napi/README.md](../napi/README.md)** - NAPI 接口文档
- **[../native/README.md](../native/README.md)** - Native 接口文档
- **[../../../ohos_nweb/README.md](../../../ohos_nweb/README.md)** - 核心引擎文档
- **[../../../AGENT.md](../../../AGENT.md)** - 代码仓指南
