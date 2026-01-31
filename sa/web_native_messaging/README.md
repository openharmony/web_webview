# WebNativeMessagingService

## 概述

`WebNativeMessagingService` 是 OpenHarmony 系统中 ArkWeb WebView 组件的**Web 原生消息传递系统服务**（System Ability）。该服务提供了 Web 应用（运行在 WebView 中）与本地扩展（Extension）之间的双向通信能力。

**SA ID**: `8610` (`SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID`)
**进程名**: `web_native_messaging_service`
**库文件**: `libweb_native_messaging_service.z.so`

### 核心职责

1. **建立连接** - 在 Web 应用和 Native Extension 之间建立 IPC 通信连接
2. **Ability 启动** - 启动和管理 Native Extension Ability
3. **消息路由** - 在 Web 和 Extension 之间路由消息
4. **生命周期管理** - 管理连接的建立、断开和清理
5. **延迟退出** - 支持按需启动和空闲延迟退出

## 目录结构

```
sa/web_native_messaging/
├── service/                                   # 服务端实现
│   ├── web_native_messaging_service.h/cpp    # 主服务类
│   ├── web_native_messaging_manager.h/cpp    # 连接管理器
│   ├── extension_ipc_connection.h/cpp        # Extension IPC 连接
│   ├── connect_native_request.h/cpp          # 连接请求
│   ├── service_event_handler.h/cpp           # 事件处理器
│   ├── service_delay_exit_task.h/cpp         # 延迟退出任务
│   └── web_extension_connection_callback_proxy.h/cpp  # 回调代理
│
├── client/                                    # 客户端实现
│   ├── web_native_messaging_client.h/cpp     # 客户端主类
│   ├── web_native_messaging_load_callback.h/cpp  # SA 加载回调
│   └── web_extension_connection_callback.h/cpp  # 连接回调
│
├── common/                                    # 公共代码
│   ├── web_native_messaging_common.h/cpp     # 公共定义
│   ├── connection_native_info_parcel.h/cpp   # 连接信息序列化
│   ├── web_native_messaging_log.h            # 日志定义
│   └── i_web_extension_connection_callback.h # 回调接口
│
├── IWebNativeMessagingService.idl            # IDL 接口定义
├── BUILD.gn                                  # 构建配置
├── 8610.json                                 # SA 配置文件（SA Profile）
├── web_native_messaging_service.cfg          # 服务启动配置
├── libweb_native_messaging_service.map       # 符号可见性配置
└── README.md                                 # 本文档
```

## 工作原理

### 系统架构

```
┌──────────────────────────────────────────────────────────────┐
│                   Web Application (WebView)                  │
│                 (ArkTS/JS in Renderer Process)              │
└─────────────────────────┬────────────────────────────────────┘
                          │ NAPI Interface
                          ▼
┌──────────────────────────────────────────────────────────────┐
│              WebNativeMessagingClient (lib)                  │
│           - ConnectWebNativeMessagingExtension()             │
│           - DisconnectWebNativeMessagingExtension()          │
└─────────────────────────┬────────────────────────────────────┘
                          │ IPC (Binder)
                          ▼
┌──────────────────────────────────────────────────────────────┐
│         WebNativeMessagingService (SA 8610)                  │
│  ┌────────────────────────────────────────────────────┐     │
│  │  WebNativeMessagingManager                          │     │
│  │  - 管理多个 ExtensionIpcConnection                 │     │
│  └────────────────────────────────────────────────────┘     │
│  ┌────────────────────────────────────────────────────┐     │
│  │  ExtensionIpcConnection (per Extension)              │     │
│  │  - OnAbilityConnectDone()                           │     │
│  │  - OnAbilityDisconnectDone()                        │     │
│  │  - ConnectNative()                                  │     │
│  └────────────────────────────────────────────────────┘     │
└─────────────────────────┬────────────────────────────────────┘
                          │ Ability Connection
                          ▼
┌──────────────────────────────────────────────────────────────┐
│           Native Extension (UI Extension)                    │
│           (Type: WebNativeMessagingExtension)                │
│  - 接收来自 Web 的消息                                        │
│  - 处理消息并返回响应                                         │
└──────────────────────────────────────────────────────────────┘
```

### 核心工作流程

#### 1. 建立连接流程

```
Web App → Client → Service → Extension
```

**详细步骤**：

```
1. Web 应用调用 NAPI 接口
   ├─ chrome.runtime.sendNativeMessage()
   └─ 传递：extensionId, message

2. WebNativeMessagingClient.ConnectWebNativeMessagingExtension()
   ├─ 加载 SA 8610（按需启动服务）
   ├─ 获取 Service Proxy
   └─ 调用 ConnectWebNativeMessagingExtension()

3. WebNativeMessagingService.ConnectWebNativeMessagingExtension()
   ├─ 获取调用者信息（TokenId, Pid, BundleName）
   ├─ 验证权限和参数
   └─ 转发到 Manager

4. WebNativeMessagingManager.ConnectWebNativeMessagingExtension()
   ├─ 检查是否已存在 IPC 连接
   │   └─ 如果存在，复用连接
   ├─ 如果不存在，创建新连接
   │   ├─ 创建 ExtensionIpcConnection
   │   ├─ 调用 AbilityManager::ConnectAbility()
   │   └─ 等待 Extension 启动
   └─ 创建 ConnectionNativeRequest
       ├─ 包含连接信息（connectionId, bundleName 等）
       └─ 加入请求队列

5. ExtensionIpcConnection.OnAbilityConnectDone()
   ├─ Extension 启动完成
   ├─ 获取 Extension 的远程对象
   ├─ 处理队列中的请求
   └─ 调用 ConnectNative()

6. ExtensionIpcConnection.ConnectNative()
   ├─ 通过 Proxy 调用 Extension 的连接接口
   ├─ 传递连接信息（ConnectionNativeInfo）
   └─ 建立 Web ↔ Extension 的消息通道
```

#### 2. 断开连接流程

```
Web App → Client → Service → Extension
```

**详细步骤**：

```
1. Web 应用调用断开连接接口
   └─ 传递：connectionId

2. WebNativeMessagingClient.DisconnectWebNativeMessagingExtension()
   └─ 调用 Service 的 DisconnectWebNativeMessagingExtension()

3. WebNativeMessagingManager.DisconnectWebNativeMessagingExtension()
   ├─ 查找对应的 ConnectionNativeRequest
   ├─ 调用 ExtensionIpcConnection.DisconnectNative()
   ├─ 如果请求队列为空，断开 Ability 连接
   └─ 清理资源

4. ExtensionIpcConnection.DisconnectNative()
   ├─ 通过 Proxy 通知 Extension
   ├─ 传递 connectionId
   └─ 关闭消息通道

5. ExtensionIpcConnection.OnAbilityDisconnectDone()
   ├─ Ability 连接断开完成
   ├─ 清理 IPC 连接
   └─ 如果所有连接都关闭，触发延迟退出任务
```

#### 3. 启动 Ability 流程

```
Web App → Client → Service → AbilityManager → Extension
```

```
StartAbility(token, want, startOptions)
    │
    ├─ 1. 验证调用者身份和权限
    │
    ├─ 2. 调用 AbilityManager::StartAbility()
    │      └─ 启动指定的 Extension Ability
    │
    ├─ 3. 设置连接回调
    │      └─ OnAbilityConnectDone / OnAbilityDisconnectDone
    │
    └─ 4. 返回结果
```

#### 4. Extension 主动断开

```
Extension → Service → Client → Web App
```

```
1. Extension 进程崩溃或主动断开
   └─ 调用 StopNativeConnectionFromExtension()

2. WebNativeMessagingManager.StopNativeConnectionFromExtension()
   ├─ 查找对应的 ExtensionIpcConnection
   ├─ 清理该连接的所有请求
   ├─ 通知 Web 应用连接已断开
   └─ 如果无其他连接，断开 Ability 连接

3. 通过回调通知 Web 应用
   └─ WebExtensionConnectionCallback.OnDisconnect()
```

## IDL 接口定义

### IWebNativeMessagingService.idl

```idl
sequenceable OHOS.IRemoteObject;
sequenceable OHOS.AAFwk.Want;
sequenceable OHOS.AAFwk.StartOptions;

interface OHOS.NWeb.IWebNativeMessagingService {
    // 连接 Native Messaging Extension
    void ConnectWebNativeMessagingExtension(
        [in] IRemoteObject token,                    // 调用者 Token
        [in] Want want,                             // Extension 信息（bundleName, abilityName）
        [in] IRemoteObject connectionCallback,      // 连接回调
        [in] int connectionId,                      // 连接 ID
        [out] int errorNum                          // 错误码
    );

    // 断开 Native Messaging Extension 连接
    void DisconnectWebNativeMessagingExtension(
        [in] int connectionId,                      // 连接 ID
        [out] int errorNum                          // 错误码
    );

    // 启动 Ability
    void StartAbility(
        [in] IRemoteObject token,                   // 调用者 Token
        [in] Want want,                             // Ability 信息
        [in] StartOptions options,                  // 启动选项
        [out] int errorNum                          // 错误码
    );

    // Extension 主动断开连接
    void StopNativeConnectionFromExtension(
        [in] int connectionId,                      // 连接 ID
        [out] int errorNum                          // 错误码
    );
};
```

### 错误码定义

```cpp
enum ConnectNativeRet : int32_t {
    SUCCESS = 0,                          // 成功
    PERMISSION_CHECK_ERROR = -1,          // 权限检查失败
    CONTEXT_ERROR = -2,                   // 上下文错误
    WANT_FORMAT_ERROR = -3,               // Want 格式错误
    CONNECTION_NOT_EXIST = -4,            // 连接不存在
    MEMORY_ERROR = -5,                    // 内存错误
    CALLBACK_ERROR = -6,                  // 回调错误
    IPC_ERROR = -7,                       // IPC 错误
    SERVICE_INIT_ERROR = -8,              // 服务初始化错误
    CONNECTION_ID_EXIST = -9,             // 连接 ID 已存在
    REQUEST_SIZE_TOO_LARGE = -10,         // 请求过大
    CONNECT_STATUS_ERROR = -11,           // 连接状态错误
    ABILITY_CONNECTION_ERROR = -12,       // Ability 连接错误
    SERVICE_DIED_ERROR = -13,             // 服务死亡
};
```

### 连接信息结构

```cpp
struct ConnectionNativeInfo {
    int32_t connectionId;         // 连接 ID
    std::string bundleName;       // Extension 的 Bundle 名称
    std::string extensionOrigin;  // Extension 来源
    int32_t extensionPid;         // Extension 进程 ID
};
```

## 核心组件

### 1. WebNativeMessagingService

**职责**：System Ability 主类，负责服务的生命周期管理。

```cpp
class WebNativeMessagingService : public SystemAbility,
                                  public WebNativeMessagingServiceStub {
    // IDL 接口实现
    ErrCode ConnectWebNativeMessagingExtension(...) override;
    ErrCode DisconnectWebNativeMessagingExtension(...) override;
    ErrCode StartAbility(...) override;
    ErrCode StopNativeConnectionFromExtension(...) override;

    // System Ability 生命周期
    void OnStart(const SystemAbilityOnDemandReason& startReason) override;
    void OnStop() override;

private:
    std::shared_ptr<WebNativeMessagingManager> manager_;
};
```

### 2. WebNativeMessagingManager

**职责**：管理所有 Extension IPC 连接，处理连接请求。

**核心数据结构**：
```cpp
// IPC 连接映射表
// Key: <AccessTokenID, BundleName>
// Value: ExtensionIpcConnection 的智能指针
using AbilityCallbackMapType =
    std::map<std::pair<AccessTokenID, std::string>, sptr<ExtensionIpcConnection>>;

AbilityCallbackMapType AbilityConnectMap_;  // IPC 连接映射表
std::mutex AbilityConnectMutex_;           // 保护映射表的互斥锁
```

**核心方法**：
```cpp
class WebNativeMessagingManager {
    // 连接 Extension
    void ConnectWebNativeMessagingExtension(...);

    // 断开 Extension
    void DisconnectWebNativeMessagingExtension(...);

    // 启动 Ability
    void StartAbility(...);

    // 查找或创建 IPC 连接
    sptr<ExtensionIpcConnection> LookUpOrNewIpcConnection(...);

    // 清理 Ability 连接
    void CleanAbilityConnection(AccessTokenID id, std::string& bundleName);

private:
    // 事件处理器（用于异步处理）
    std::shared_ptr<ServiceEventHandler> serviceHandler_;

    // 延迟退出任务
    std::shared_ptr<ServiceDelayExitTask> delayExitTask_;
};
```

### 3. ExtensionIpcConnection

**职责**：管理单个 Extension 的 IPC 连接，继承自 `AbilityConnection`。

```cpp
class ExtensionIpcConnection : public AbilityRuntime::AbilityConnection {
public:
    // Ability 连接回调
    void OnAbilityConnectDone(
        const ElementName& element,
        const sptr<IRemoteObject>& remoteObject,
        int resultCode) override;

    void OnAbilityDisconnectDone(
        const ElementName& element,
        int resultCode) override;

    // 连接/断开 Native Messaging
    int32_t ConnectNative(std::shared_ptr<ConnectionNativeRequest> request);
    int32_t DisconnectNative(int32_t connectionId, bool& resDeleted);

    // 处理调用者死亡
    void HandleCallerDeath();

private:
    AccessTokenID callerTokenId_;        // 调用者的 AccessToken ID
    std::string targetBundleName_;      // 目标 Bundle 名称
    std::string targetAbilityName_;     // 目标 Ability 名称
    sptr<IRemoteObject> token_;         // 调用者 Token
    int32_t callerUserId_;              // 调用者 User ID

    // 请求队列（同一个 Extension 可能有多个连接）
    std::map<int32_t, std::shared_ptr<ConnectionNativeRequest>> requestMap_;
    std::mutex requestMapMutex_;

    wptr<ExtensionIpcConnection> wpThis_;          // 自身的弱引用
    wptr<IWebNativeMessagingManager> wpManager_;   // Manager 的弱引用
};
```

**请求队列管理**：
- 一个 Extension 可以同时服务多个 Web 应用
- 每个连接有唯一的 `connectionId`
- 所有请求共享同一个 Ability 连接
- 当请求队列为空时，断开 Ability 连接

### 4. ConnectionNativeRequest

**职责**：封装单次连接请求的所有信息。

```cpp
struct ConnectNativeParams {
    AccessTokenID callerTokenId;           // 调用者 Token ID
    int32_t callerPid;                     // 调用者进程 ID
    int32_t callerConnectId;               // 调用者侧的连接 ID
    int32_t innerConnectId;                // 服务侧的连接 ID
    std::string callerBundleName;          // 调用者的 Bundle 名称
    sptr<IRemoteObject> connectionCallback; // 连接回调
    sptr<IRemoteObject> token;             // 调用者 Token
    int32_t callerUserId;                  // 调用者 User ID
};

class ConnectionNativeRequest {
public:
    ConnectionNativeRequest(const ConnectNativeParams& params, const Want& want);

    // 获取连接参数
    const ConnectNativeParams& GetParams() const;

    // 获取 Extension 信息
    const std::string& GetExtensionBundleName() const;
    const std::string& GetExtensionAbilityName() const;

private:
    ConnectNativeParams params_;
    AAFwk::Want want_;
};
```

### 5. ServiceEventHandler

**职责**：异步处理连接请求和回调。

```cpp
class ServiceEventHandler : public AppExecFwk::EventHandler {
public:
    // 处理连接任务
    void ProcessConnectTask(const std::function<void()>& task);

    // 处理断开任务
    void ProcessDisconnectTask(const std::function<void()>& task);

    // 处理失败任务
    void ProcessFailedTask(const std::function<void()>& task);
};
```

### 6. ServiceDelayExitTask

**职责**：实现空闲延迟退出机制。

```cpp
class ServiceDelayExitTask {
public:
    // 启动延迟退出任务
    void StartDelayExitTask();

    // 重置延迟退出任务（有新活动时）
    void ResetDelayExitTask();

    // 停止延迟退出任务
    void StopDelayExitTask();

private:
    // 延迟时间（默认 30 秒）
    static constexpr int32_t DELAY_EXIT_TIME = 30000;

    std::shared_ptr<AppExecFwk::EventHandler> handler_;
};
```

## 客户端使用

### WebNativeMessagingClient

客户端通过单例模式访问服务：

```cpp
#include "web_native_messaging_client.h"

using namespace OHOS::NWeb;

// 获取客户端实例
WebNativeMessagingClient& client = WebNativeMessagingClient::GetInstance();

// 连接 Extension
int result = client.ConnectWebNativeMessagingExtension(
    token,                  // 调用者 Token
    want,                   // Extension 信息（bundleName, abilityName）
    connectionCallback,     // 连接回调
    connectionId            // 连接 ID
);

// 断开 Extension
result = client.DisconnectWebNativeMessagingExtension(connectionId);

// 启动 Ability
result = client.StartAbility(token, want, startOptions);

// Extension 主动断开
result = client.StopNativeConnectionFromExtension(connectionId);
```

### 连接回调

```cpp
class WebExtensionConnectionCallback : public WebExtensionConnectionCallbackStub {
public:
    // 连接成功
    void OnConnect(const ConnectionNativeInfo& info) override {
        // Extension 已连接，可以开始发送消息
    }

    // 连接断开
    void OnDisconnect() override {
        // 连接已断开，清理资源
    }
};
```

### Want 配置

```cpp
AAFwk::Want want;
want.SetBundleName("com.example.app");              // Extension 的 Bundle 名称
want.SetAbilityName("WebNativeMessagingExtension"); // Extension 的 Ability 名称
want.SetModuleName("entry");                        // Module 名称
```

## 服务配置

### SA Profile (8610.json)

```json
{
    "process": "web_native_messaging_service",
    "systemability": [{
        "name": 8610,                                   // SA ID
        "libpath": "libweb_native_messaging_service.z.so",
        "run-on-create": false,                        // 不在创建时运行（按需启动）
        "distributed": false,                          // 非分布式
        "dump-level": 1                                // Dump 信息级别
    }]
}
```

### 服务启动配置 (web_native_messaging_service.cfg)

```json
{
    "services": [{
        "name": "web_native_messaging_service",
        "path": ["/system/bin/sa_main",
                 "/system/profile/web_native_messaging_service.json"],
        "ondemand": true,                              // 按需启动
        "uid": "web_native_messaging_service",
        "gid": ["web_native_messaging_service"],
        "permission": [
            "ohos.permission.START_ABILITIES_FROM_BACKGROUND",  // 后台启动 Ability
            "ohos.permission.GET_BUNDLE_INFO"                    // 获取 Bundle 信息
        ],
        "apl": "system_basic",                       // Ability 权限级别
        "secon": "u:r:web_native_messaging_service:s0"           // SELinux 上下文
    }]
}
```

## 依赖关系

### 外部依赖（BUILD.gn）

#### 服务依赖
```gn
external_deps = [
    "ability_base:want",                    # Want 数据结构
    "ability_base:zuri",                    # URI 处理
    "ability_runtime:ability_manager",      # Ability 管理
    "ability_runtime:ability_connect_callback_stub",  # Ability 连接回调
    "ability_runtime:ability_context_native",        # Ability 上下文
    "ability_runtime:ability_start_options",         # Ability 启动选项
    "ability_runtime:napi_common",          # NAPI 公共部分
    "access_token:libaccesstoken_sdk",      # AccessToken 管理
    "bundle_framework:appexecfwk_base",     # Bundle 框架基础
    "bundle_framework:appexecfwk_core",     # Bundle 框架核心
    "c_utils:utils",                        # 工具库
    "eventhandler:libeventhandler",         # 事件处理器
    "hilog:libhilog",                       # 日志系统
    "hitrace:hitrace_meter",                # 链路追踪
    "ipc:ipc_core",                         # IPC 核心
    "safwk:system_ability_fwk",             # System Ability 框架
    "samgr:samgr_proxy",                    # SAMGR 代理
]
```

#### 客户端依赖 (web_native_messaging_kit)
```gn
external_deps = [
    "ability_base:want",
    "ability_base:zuri",
    "ability_runtime:ability_manager",
    "ability_runtime:ability_start_options",
    "ability_runtime:napi_common",
    "c_utils:utils",
    "hilog:libhilog",
    "hitrace:hitrace_meter",
    "ipc:ipc_core",
    "samgr:samgr_proxy",
]
```

**注**：客户端依赖较服务端少，因为不需要 Ability 连接相关功能。

### 输出库

1. **`libweb_native_messaging_service.z.so`** - System Ability 服务库
   - 被系统服务进程加载
   - 实现 IDL Stub 接口

2. **`libweb_native_messaging_kit.so`** - 客户端 SDK 库
   - 被应用进程加载
   - 实现 IDL Proxy 接口
   - 标记为 `innerapi_tags: [ "platformsdk" ]`

## 使用场景

### 场景 1: Web 应用与 Native Extension 通信

```
1. Web 应用（如浏览器）加载页面
2. 页面中的 JS 调用 chrome.runtime.sendNativeMessage()
3. NAPI 接口被调用
4. WebNativeMessagingClient.ConnectWebNativeMessagingExtension()
5. 服务启动 Extension（如果未运行）
6. 建立 IPC 连接
7. Web 可以通过连接发送消息到 Extension
8. Extension 处理消息并返回响应
9. Web 接收响应并更新页面
```

### 场景 2: 多个 Web 应用共享同一个 Extension

```
App1 → WebNativeMessagingClient → Service → Extension
                                              ↑
App2 → WebNativeMessagingClient ───────────────┘

1. App1 首次连接，创建 ExtensionIpcConnection
2. App2 连接同一个 Extension
3. 服务检测到 IPC 连接已存在，复用连接
4. 两个应用的请求通过同一个 Ability 连接路由
5. 通过 connectionId 区分不同应用的请求
```

### 场景 3: Extension 崩溃后的重连

```
1. Extension 进程崩溃
2. AbilityConnection 触发 OnAbilityDisconnectDone()
3. 服务清理该 Extension 的所有连接
4. 通知所有连接的 Web 应用（通过回调）
5. Web 应用可以尝试重新连接
6. 服务重新启动 Extension
7. 建立新的连接
```

### 场景 4: 按需启动和延迟退出

```
1. 首次连接请求到达
2. SAMGR 启动 web_native_messaging_service 进程
3. 服务初始化，创建 WebNativeMessagingManager
4. 处理连接请求
5. 所有连接断开后
6. 启动延迟退出任务（30 秒）
7. 如果 30 秒内有新连接，取消退出
8. 如果 30 秒内无新连接，服务退出
```

## 日志与调试

### 日志标签
```cpp
HILOG_TAG="web_native_messaging_service"
```

### 宏定义
```cpp
WNMLOG_I("Info log");   // 信息日志
WNMLOG_D("Debug log");  // 调试日志
WNMLOG_E("Error log");  // 错误日志
WNMLOG_W("Warn log");   // 警告日志
```

### 关键日志点

1. **服务启动**：
   ```
   Service on start.
   Service init success.
   ```

2. **连接建立**：
   ```
   ConnectWebNativeMessagingExtension, connectionId: %{public}d
   OnAbilityConnectDone, resultCode: %{public}d
   ConnectNative success, connectionId: %{public}d
   ```

3. **连接断开**：
   ```
   DisconnectWebNativeMessagingExtension, connectionId: %{public}d
   OnAbilityDisconnectDone, resultCode: %{public}d
   ```

4. **Extension 连接管理**：
   ```
   LookUpOrNewIpcConnection, create new connection
   IsIpcConnectExist: false
   CleanAbilityConnection, bundleName: %{public}s
   ```

### 调试建议

1. **查看服务状态**：
   ```bash
   # 检查服务是否运行
   ps -A | grep web_native_messaging_service

   # 查看 SA 状态
   hidumper -s 8610
   ```

2. **查看连接信息**：
   ```bash
   # Dump 所有连接
   hidumper -s 8610 -a -connection
   ```

3. **查看日志**：
   ```bash
   # 查看服务日志
   hilog -T web_native_messaging_service
   ```

4. **跟踪 Ability 连接**：
   ```bash
   # 查看 Ability Manager 日志
   hilog -T AbilityManager
   ```

## 安全性

### 权限检查

服务需要以下权限：
- `START_ABILITIES_FROM_BACKGROUND` - 后台启动 Ability
- `GET_BUNDLE_INFO` - 获取 Bundle 信息

### AccessToken 验证

服务通过 `AccessTokenID` 识别调用者：
```cpp
AccessTokenID callerTokenId = IPCSkeleton::GetCallingTokenID();
```

每个 IPC 连接都与调用者的 `AccessTokenID` 绑定，防止跨应用访问。

### BundleName 验证

服务从 `AccessTokenID` 反查 `BundleName`，确保调用者身份真实：
```cpp
bool GetPidExtensionBundleName(int32_t pid, std::string& bundleName);
```

### SELinux 策略

服务运行在专门的 SELinux 域：
```
secon: u:r:web_native_messaging_service:s0
apl: system_basic
```

## 性能优化

### 连接复用

多个 Web 应用连接同一个 Extension 时，复用同一个 Ability 连接：
- 减少 IPC 开销
- 降低内存占用
- 提高 Extension 启动速度

### 请求队列

同一个 Extension 的多个连接请求通过队列管理：
- 异步处理，避免阻塞
- 支持并发连接

### 延迟退出

服务空闲 30 秒后自动退出：
- 节省系统资源
- 按需启动，快速响应

### 事件处理

使用 `EventHandler` 异步处理连接请求：
- 避免 IPC 线程阻塞
- 提高并发处理能力

## 故障处理

### Extension 启动失败

**症状**：`OnAbilityConnectDone` 返回错误码

**排查**：
1. 检查 Extension 是否存在
2. 检查 `want` 配置是否正确（bundleName, abilityName）
3. 查看 Ability Manager 日志
4. 检查 Extension 权限配置

### 连接超时

**症状**：长时间无回调

**排查**：
1. 检查服务是否正常运行
2. 检查 Ability Manager 是否正常
3. 查看 Extension 进程日志
4. 检查网络和 IPC 状态

### 内存泄漏

**症状**：长时间运行后内存持续增长

**排查**：
1. 使用 `hidumper -s 8610 -a --memory` 查看内存使用
2. 检查请求队列是否正常清理
3. 检查 IPC 连接是否正常释放
4. 使用 ASan 检测内存问题

### 服务频繁重启

**症状**：服务进程反复启动退出

**排查**：
1. 检查延迟退出时间是否过短
2. 查看是否有频繁的连接/断开
3. 检查 Extension 是否频繁崩溃
4. 查看系统日志中的崩溃信息

## 常见问题

### Q1: 为什么要使用 Ability 连接而不是直接 IPC？

**A**: Ability 连接提供：
- **生命周期管理**：Ability 框架管理 Extension 的启动和停止
- **权限隔离**：每个 Ability 运行在独立的沙箱中
- **系统级支持**：Ability Manager 处理复杂的连接逻辑
- **跨进程通信**：使用 Binder 机制，安全可靠

### Q2: 一个 Extension 能同时服务多个 Web 应用吗？

**A**: 可以。服务通过以下机制支持：
- **连接复用**：多个 Web 应用共享同一个 Ability 连接
- **请求队列**：每个连接有唯一的 `connectionId`
- **消息路由**：服务根据 `connectionId` 路由消息

### Q3: Extension 崩溃后如何处理？

**A**: 服务会：
1. 触发 `OnAbilityDisconnectDone()` 回调
2. 清理该 Extension 的所有连接
3. 通知所有连接的 Web 应用
4. Web 应用可以重新连接
5. 服务重新启动 Extension

### Q4: 为什么需要延迟退出机制？

**A**: 延迟退出用于：
- **节省资源**：空闲时释放内存和 CPU
- **快速响应**：避免频繁启停
- **平衡性能**：30 秒是一个合理的折中

### Q5: 如何监控连接状态？

**A**: 有多种方式：
1. 使用 `hidumper -s 8610` 查看所有连接
2. 查看日志输出
3. 实现连接回调监听状态变化
4. 使用系统监控工具

## 维护建议

### 代码规范
1. 保持 IDL 接口稳定，避免频繁变更
2. 新增功能要向后兼容
3. 使用 `WNMLOG_*` 宏记录关键操作
4. 所有 IPC 调用都要进行权限检查

### 测试要点
1. 单元测试：`test/unittest/web_native_messaging_client_test`
2. 服务启动/停止测试
3. 连接/断开流程测试
4. 多连接并发测试
5. Extension 崩溃恢复测试
6. 内存泄漏测试

### 性能考虑
1. 避免在 IPC 线程中执行耗时操作
2. 使用 `EventHandler` 异步处理
3. 及时清理不再使用的资源
4. 控制请求队列大小

### 版本演进
1. 新增接口要考虑兼容性
2. 避免破坏现有的连接流程
3. 新增错误码要明确定义
4. 更新文档和测试用例

## 相关文档

- [主 README.md](../../README.md) - WebView 组件总览
- [AGENT.md](../../AGENT.md) - 代码仓指南
- [WebNativeMessagingExtension](../../interfaces/kits/napi/web_native_messaging_extension/README.md) - NAPI 接口文档
- [System Ability 指南](https://gitee.com/openharmony/docs) - OpenHarmony SA 开发指南
- [Ability 连接](https://gitee.com/openharmony/docs) - Ability Connection 文档

## 许可证

Apache License 2.0
