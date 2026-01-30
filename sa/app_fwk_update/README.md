# AppFwkUpdateService

## 概述

`AppFwkUpdateService` 是 OpenHarmony 系统中 ArkWeb WebView 组件的**应用框架更新系统服务**（System Ability）。该服务负责监听和管理 ArkWebCore HAP 包的安装、更新和卸载事件，并在检测到变化时通知相关组件（如 AppSpawn 进程）进行相应的处理。

**SA ID**: `8350` (`SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID`)
**进程名**: `app_fwk_update_service`
**库文件**: `libapp_fwk_update_service.z.so`

### 核心职责

1. **监听 ArkWebCore 包变化** - 通过公共事件监听 HAP 包的安装、更新、卸载
2. **验证包安装** - 验证新的 ArkWebCore 包是否正确安装
3. **通知框架更新** - 在 BMS (Bundle Manager Service) 启动后通知框架
4. **与 AppSpawn 通信** - 通过 AppSpawn 客户端发送消息，触发 Web 库的加载/卸载
5. **按需启动** - 支持按需启动（On-Demand），节省系统资源

## 目录结构

```
sa/app_fwk_update/
├── include/
│   ├── app_fwk_update_service.h           # 服务主类头文件
│   ├── app_fwk_update_client.h            # 客户端接口头文件
│   └── app_fwk_update_load_callback.h     # SA 加载回调头文件
├── src/
│   ├── app_fwk_update_service.cpp         # 服务实现
│   ├── app_fwk_update_client.cpp          # 客户端实现
│   └── app_fwk_update_load_callback.cpp   # SA 加载回调实现
├── IAppFwkUpdateService.idl               # IDL 接口定义
├── BUILD.gn                               # 构建配置
├── 8350.json                              # SA 配置文件（SA Profile）
├── app_fwk_update_service.cfg             # 服务启动配置
├── libapp_fwk_update_service.map          # 符号可见性配置
└── README.md                              # 本文档
```

## 工作原理

### 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                    Common Event Service                     │
│               (usual.event.PACKAGE_CHANGED)                  │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────┐
│              AppFwkUpdateService (SA 8350)                   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  PackageChangedReceiver (订阅公共事件)                 │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  VerifyPackageInstall() (验证包安装)                   │   │
│  │  NotifyFWKAfterBmsStart() (通知框架)                  │   │
│  │  NotifyArkWebInstallSuccess() (通知安装成功)          │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────┬───────────────────────────────────────┘
                      │ AppSpawn Client
                      ▼
┌─────────────────────────────────────────────────────────────┐
│                   AppSpawn Service                          │
│  - MSG_LOAD_WEBLIB_IN_APPSPAWN    (加载 Web 库)             │
│  - MSG_UNLOAD_WEBLIB_IN_APPSPAWN  (卸载 Web 库)            │
│  - MSG_UPDATE_MOUNT_POINTS        (更新挂载点)              │
└─────────────────────────────────────────────────────────────┘
```

### 核心工作流程

#### 1. 服务启动（按需启动）

服务采用**按需启动**模式，只有在接收到相关公共事件时才会启动：

**触发事件**（定义在 `8350.json`）：
- `usual.event.PACKAGE_CHANGED` - 包变化事件
- `usual.event.PACKAGE_ADDED` - 包添加事件

**启动流程**：
```
1. Common Event Service 接收到 PACKAGE_CHANGED/ADDED 事件
2. SAMGR (System Ability Manager) 检查事件中的 bundleName
3. 如果 bundleName 为 "com.ohos.nweb" 或 "com.ohos.arkwebcore"
4. 启动 app_fwk_update_service 进程
5. 调用 AppFwkUpdateService::OnStart() -> Init()
6. 服务发布到 SAMGR
7. 订阅 COMMON_EVENT_SERVICE
8. 延迟 60 秒后卸载任务（如果空闲）
```

#### 2. 包变化监听

**PackageChangedReceiver** 订阅并处理包变化事件：

```cpp
void PackageChangedReceiver::OnReceiveEvent(const CommonEventData& data) {
    std::string action = data.GetWant().GetAction();
    std::string bundleName = data.GetWant().GetBundle();

    // 只处理 ArkWeb 相关的包
    if (action == COMMON_EVENT_PACKAGE_CHANGED &&
        (bundleName == "com.ohros.nweb" || bundleName == "com.ohos.arkwebcore")) {
        // 从 BundleManager 获取 HAP 路径
        // 触发 OnPackageChangedEvent(bundleName, hapPath)
    }
}
```

#### 3. 验证包安装流程

当检测到 ArkWebCore 包变化时，执行 `VerifyPackageInstall()`：

```
VerifyPackageInstall(bundleName, hapPath, success)
    │
    ├─ 1. 检查调用者 UID（必须为 FOUNDATION_UID: 5523）
    │
    ├─ 2. 发送 MSG_UNLOAD_WEBLIB_IN_APPSPAWN 到 AppSpawn
    │      └─ 通知 AppSpawn 卸载旧的 Web 库
    │
    ├─ 3. 检查 HAP 路径是否变化
    │      └─ 如果路径未变，直接返回成功
    │
    ├─ 4. 设置新的安装路径
    │      └─ SetParameter("persist.arkwebcore.install_path", hapPath)
    │
    ├─ 5. 设置包名
    │      └─ SetParameter("persist.arkwebcore.package_name", bundleName)
    │
    └─ 6. 发送 MSG_UPDATE_MOUNT_POINTS 到 AppSpawn
           └─ 通知 AppSpawn 更新挂载点
```

**关键系统参数**：
- `persist.arkwebcore.install_path` - ArkWebCore 安装路径
- `persist.arkwebcore.package_name` - ArkWebCore 包名
- `web.engine.install.completed` - Web 引擎安装完成标志

#### 4. 通知框架更新

`NotifyFWKAfterBmsStart()` 在 BMS 启动后被调用：

```
NotifyFWKAfterBmsStart()
    │
    ├─ 1. 读取 persist.arkwebcore.package_name
    │
    ├─ 2. 发送 MSG_LOAD_WEBLIB_IN_APPSPAWN 到 AppSpawn
    │      └─ 通知 AppSpawn 加载 Web 库
    │
    └─ 3. 设置 web.engine.install.completed = "true"
           └─ 通知其他组件 Web 引擎已就绪
```

#### 5. AppSpawn 消息类型

通过 `SendAppSpawnMessage()` 发送三种消息：

| 消息类型 | 值 | 说明 |
|---------|---|------|
| `MSG_LOAD_WEBLIB_IN_APPSPAWN` | 20 | 加载 Web 库到 AppSpawn |
| `MSG_UNLOAD_WEBLIB_IN_APPSPAWN` | 21 | 从 AppSpawn 卸载 Web 库 |
| `MSG_UPDATE_MOUNT_POINTS` | 22 | 更新挂载点 |

## IDL 接口定义

### IAppFwkUpdateService.idl

```idl
interface OHOS.NWeb.IAppFwkUpdateService {
    // 验证包安装（同步调用）
    void VerifyPackageInstall(
        [in] String bundleName,   // 包名
        [in] String hapPath,      // HAP 文件路径
        [out] int success         // 0=成功, -1=失败
    );

    // BMS 启动后通知框架（单向调用）
    [oneway] void NotifyFWKAfterBmsStart();

    // 通知 ArkWeb 安装成功
    void NotifyArkWebInstallSuccess();
}
```

### 接口实现

所有接口都在 `AppFwkUpdateService` 中实现：

```cpp
class AppFwkUpdateService : public SystemAbility,
                            public AppFwkUpdateServiceStub {
public:
    ErrCode VerifyPackageInstall(
        const std::string& bundleName,
        const std::string& hapPath,
        int32_t& success) override;

    ErrCode NotifyFWKAfterBmsStart() override;

    ErrCode NotifyArkWebInstallSuccess() override;

    // ...
};
```

## 客户端使用

### AppFwkUpdateClient

客户端通过单例模式访问服务：

```cpp
#include "app_fwk_update_client.h"

using namespace OHOS::NWeb;

// 获取客户端实例
AppFwkUpdateClient& client = AppFwkUpdateClient::GetInstance();

// 验证包安装
int result = client.VerifyPackageInstall(
    "com.ohos.nweb",
    "/data/storage/el1/bundle/arkwebcore/entry.hap"
);

// 通知框架 BMS 已启动
int ret = client.NotifyFWKAfterBmsStart();

// 通知 ArkWeb 安装成功
client.NotifyArkWebInstallSuccess();
```

### 客户端工作原理

1. **SA 加载**：
   - 调用 `LoadFwkService()` 请求 SAMGR 加载 SA 8350
   - 超时时间：4 秒
   - 使用 `AppFwkUpdateLoadCallback` 处理加载结果

2. **Proxy 获取**：
   - 通过 `iface_cast<IAppFwkUpdateService>` 获取服务代理
   - 支持服务死亡监听（Death Recipient）

3. **重试机制**：
   - 连接失败时自动重试
   - 加载超时后返回错误

## 服务配置

### SA Profile (8350.json)

```json
{
    "process": "app_fwk_update_service",
    "systemability": [{
        "name": 8350,                                    // SA ID
        "libpath": "libapp_fwk_update_service.z.so",
        "run-on-create": false,                         // 不在创建时运行
        "auto-restart": true,                           // 自动重启
        "distributed": false,                           // 非分布式
        "dump-level": 1,
        "start-on-demand": {                            // 按需启动配置
            "commonevent": [
                { "name": "usual.event.PACKAGE_CHANGED" },
                { "name": "usual.event.PACKAGE_ADDED" }
            ]
        }
    }]
}
```

### 服务启动配置 (app_fwk_update_service.cfg)

```json
{
    "services": [{
        "name": "app_fwk_update_service",
        "path": ["/system/bin/sa_main",
                 "/system/profile/app_fwk_update_service.json"],
        "ondemand": true,                               // 按需启动
        "uid": "app_fwk_update",
        "gid": ["app_fwk_update"],
        "permission": [
            "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
            "ohos.permission.GET_BUNDLE_INFO"
        ],
        "start-mode": "condition",                      // 条件启动模式
        "secon": "u:r:app_fwk_update_service:s0"        // SELinux 上下文
    }]
}
```

## 关键常量

### Bundle 名称集合
```cpp
const std::set<std::string> ARK_WEB_DEFAULT_BUNDLE_NAME_SET = {
    "com.ohros.nweb",          // 旧包名
    "com.ohos.arkwebcore"      // 新包名
};
```

### UID
```cpp
const int FOUNDATION_UID = 5523;  // Foundation 进程的 UID
```

### 系统参数
```cpp
const std::string PERSIST_ARKWEBCORE_PACKAGE_NAME = "persist.arkwebcore.package_name";
const std::string WEB_ENGINE_INSTALL_COMPLETED = "web.engine.install.completed";
const std::string PERSIST_ARKWEBCORE_INSTALL_PATH = "persist.arkwebcore.install_path";
```

### 重试和超时
```cpp
const int RETRY_COUNT = 2;               // AppSpawn 消息重试次数
const int LOAD_SA_TIMEOUT_MS = 4000;     // SA 加载超时（4秒）
const int32_t TASK_DELAY_TIME = 60000;   // 延迟卸载任务时间（60秒）
```

## 依赖关系

### 外部依赖（BUILD.gn）

```gn
external_deps = [
    "ability_base:want",                    # Want 数据结构
    "ability_base:zuri",                    # URI 处理
    "ability_runtime:ability_manager",      # Ability 管理
    "ability_runtime:napi_common",          # NAPI 公共部分
    "appspawn:appspawn_client",             # AppSpawn 客户端
    "bundle_framework:appexecfwk_base",     # Bundle 框架基础
    "bundle_framework:appexecfwk_core",     # Bundle 框架核心
    "c_utils:utils",                        # 工具库
    "common_event_service:cesfwk_core",     # 公共事件服务核心
    "common_event_service:cesfwk_innerkits",# 公共事件内部 kit
    "eventhandler:libeventhandler",         # 事件处理器
    "hilog:libhilog",                       # 日志系统
    "hitrace:hitrace_meter",                # 链路追踪
    "init:libbeget_proxy",                  # Init 代理
    "init:libbegetutil",                    # Init 工具
    "ipc:ipc_core",                         # IPC 核心
    "safwk:system_ability_fwk",             # System Ability 框架
    "samgr:samgr_proxy",                    # SAMGR 代理
]
```

### 内部依赖
```gn
include_dirs = [
    "include",                               # 本服务头文件
    "${target_gen_dir}/../../ohos_nweb/include",
    "../../ohos_nweb/include",              # Nweb 头文件
    "${target_gen_dir}",                     # IDL 生成目录
]
```

### 权限要求

- `ohos.permission.GET_BUNDLE_INFO_PRIVILEGED` - 获取 Bundle 信息特权
- `ohos.permission.GET_BUNDLE_INFO` - 获取 Bundle 信息

## 使用场景

### 场景 1: ArkWebCore OTA 更新

```
1. 系统通过 OTA 更新新的 ArkWebCore.hap
2. BMS (Bundle Manager Service) 安装新的 HAP 包
3. Common Event Service 发送 PACKAGE_CHANGED 事件
4. AppFwkUpdateService 按需启动
5. PackageChangedReceiver 接收事件
6. 调用 VerifyPackageInstall()
7. 通知 AppSpawn 卸载旧的 Web 库
8. 设置新的安装路径和包名
9. 通知 AppSpawn 更新挂载点
10. 设置 web.engine.install.completed = true
11. 其他组件检测到参数变化，重新加载 Web 库
```

### 场景 2: 系统启动后 BMS 初始化完成

```
1. BMS 启动并完成初始化
2. 调用 NotifyFWKAfterBmsStart()
3. 读取 persist.arkwebcore.package_name
4. 通知 AppSpawn 加载 Web 库
5. 设置 web.engine.install.completed = true
6. WebView 组件可以正常使用
```

### 场景 3: ArkWebCore 安装成功通知

```
1. ArkWebCore 安装成功
2. 调用 NotifyArkWebInstallSuccess()
3. 检查预加载模式和启动完成状态
4. 如果需要预加载且启动已完成，设置安装完成标志
5. 触发后续的预加载流程
```

## 日志与调试

### 日志标签
```cpp
HILOG_TAG="app_fwk_update_service"
```

### 关键日志点

1. **服务启动**：
   ```
   AppFwkUpdateService reasonName: commonevent
   AppFwkUpdateService bundleName: com.ohros.nweb
   Service init success.
   ```

2. **包变化事件**：
   ```
   packagechangeReceiver OnReceiveEvent, ret = PACKAGE_CHANGED.
   PackagechangeReceiver hapPath is /data/storage/.../entry.hap.
   ```

3. **AppSpawn 通信**：
   ```
   Send appspawn message start,uid = 1037.
   Send appspawn message success.
   ```

4. **参数设置**：
   ```
   SetWebInstallPath res = 1.
   SetWebCorePackageName res = 1.
   ```

### 调试建议

1. **查看服务状态**：
   ```bash
   # 检查服务是否运行
   ps -A | grep app_fwk_update_service

   # 查看 SA 状态
   hidumper -s 8350
   ```

2. **查看系统参数**：
   ```bash
   # 查看包名
   param get persist.arkwebcore.package_name

   # 查看安装路径
   param get persist.arkwebcore.install_path

   # 查看安装完成标志
   param get web.engine.install.completed
   ```

3. **手动触发服务**：
   ```bash
   # 发送公共事件（需要 root 权限）
   # 模拟包安装事件
   ```

4. **查看日志**：
   ```bash
   hilog -T app_fwk_update_service
   ```

## 安全性

### UID 校验

`VerifyPackageInstall()` 会检查调用者的 UID：

```cpp
if (IPCSkeleton::GetCallingUid() != FOUNDATION_UID) {
    return ERR_INVALID_VALUE;
}
```

只有 Foundation 进程（UID 5523）才能调用此接口，防止恶意应用篡改 Web 引擎。

### 权限要求

服务需要以下权限才能正常工作：
- `GET_BUNDLE_INFO_PRIVILEGED` - 获取包信息特权
- `GET_BUNDLE_INFO` - 获取包信息

### SELinux 策略

服务运行在专门的 SELinux 域：
```
secon: u:r:app_fwk_update_service:s0
```

## 性能优化

### 按需启动

服务采用按需启动模式，节省系统资源：
- 不在系统启动时运行
- 只在接收到相关公共事件时启动
- 空闲 60 秒后自动卸载

### 延迟卸载

```cpp
void PostDelayUnloadTask() {
    // 延迟 60 秒后卸载服务
    // 如果期间有新的请求，取消卸载任务
}
```

### 重试机制

AppSpawn 消息发送失败时自动重试（最多 2 次）：
```cpp
do {
    ret = AppSpawnClientSendMsg(clientHandle, reqHandle, &result);
} while (++retryCount < RETRY_COUNT && ret != 0);
```

## 故障处理

### 服务启动失败

**症状**：服务无法启动或注册失败

**排查**：
1. 检查 `8350.json` 配置是否正确
2. 检查 `app_fwk_update_service.cfg` 配置
3. 查看日志：`hilog -T app_fwk_update_service`
4. 检查权限是否正确授予

### AppSpawn 通信失败

**症状**：`SendAppSpawnMessage` 返回错误

**排查**：
1. 检查 AppSpawn 服务是否运行：`ps -A | grep appspawn`
2. 检查消息类型是否正确
3. 查看系统日志确认错误码
4. 检查包名是否为有效的 ArkWeb 包

### 包变化事件未触发

**症状**：更新 HAP 后服务无响应

**排查**：
1. 确认公共事件是否发送：使用 `dump broadcaster`
2. 检查 bundleName 是否匹配：必须是 `com.ohros.nweb` 或 `com.ohos.arkwebcore`
3. 查看服务是否按需启动失败
4. 检查 Common Event Service 是否正常

### 参数未设置成功

**症状**：`SetParameter` 返回错误

**排查**：
1. 检查参数名是否正确
2. 检查是否有设置参数的权限
3. 查看系统参数服务状态：`param get`
4. 确认参数值是否合法

## 常见问题

### Q1: 为什么要使用按需启动？

**A**: ArkWebCore 更新不频繁，按需启动可以：
- 节省系统内存和 CPU 资源
- 减少系统启动时间
- 只在需要时才运行服务

### Q2: 为什么需要与 AppSpawn 通信？

**A**: AppSpawn 负责应用进程的孵化，需要：
- 在 AppSpawn 进程中预加载 Web 库
- 更新 Web 库的挂载点
- 确保新启动的应用使用最新的 Web 引擎

### Q3: VerifyPackageInstall 为什么只允许 Foundation 调用？

**A**: 这是安全考虑，防止恶意应用：
- 伪造包更新事件
- 强制卸载 Web 库导致系统异常
- 篡改 Web 引擎路径

### Q4: 服务为什么会在 60 秒后卸载？

**A**: 这是资源优化策略：
- 包更新事件是瞬时的
- 更新完成后服务不再需要
- 延迟卸载避免频繁启动/停止

### Q5: 如何手动触发服务加载 Web 库？

**A**: 有两种方式：
1. 通过客户端调用：`AppFwkUpdateClient::GetInstance().NotifyFWKAfterBmsStart()`
2. 设置系统参数：`param set web.engine.install.completed true`

## 维护建议

### 代码规范
1. 保持 IDL 接口稳定，避免频繁变更
2. 新增接口需要向后兼容
3. 使用 `WVLOG_I/WVLOG_E` 记录关键操作

### 测试要点
1. 单元测试：`test/unittest/app_fwk_update_client_test`
2. 服务启动/停止测试
3. 包更新场景测试
4. AppSpawn 通信测试
5. 并发调用测试

### 版本演进
1. 新增功能要考虑按需启动的影响
2. 避免长时间阻塞操作
3. 新增公共事件订阅需要更新 `8350.json`

## 相关文档

- [主 README.md](../../README.md) - WebView 组件总览
- [AGENT.md](../../AGENT.md) - 代码仓指南
- [arkweb_utils/README.md](../../arkweb_utils/README.md) - 工具库文档
- [System Ability 指南](https://gitee.com/openharmony/docs) - OpenHarmony SA 开发指南

## 许可证

Apache License 2.0
