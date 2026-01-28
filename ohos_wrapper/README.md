# ohos_wrapper

## 概述

`ohos_wrapper` 是 ArkWeb WebView 组件的**轻量级封装层**，提供 C 风格的接口来访问 OpenHarmony 的特定系统服务。该层主要用于简化 WebView 引擎与某些系统服务之间的交互，特别是那些需要跨语言边界的场景。

**核心库**: `libnweb_ohos_wrapper.so`
**设计模式**: Wrapper（包装器）/ Facade（外观）模式
**语言接口**: C (extern "C")
**编译条件**: `webview_location_enable = true`

### 核心职责

1. **C 接口封装** - 将 C++ 的 OpenHarmony API 封装为 C 接口
2. **简化调用** - 提供更简洁的 API，隐藏复杂的系统服务细节
3. **跨语言支持** - 支持从 C/C++ 之外的调用方（如 WebView 引擎）
4. **按需编译** - 支持功能裁剪，当前仅包含位置服务

## 目录结构

```
ohos_wrapper/
├── BUILD.gn                      # 构建配置
├── src/                          # 源代码目录
│   └── nweb_location_wrapper.cpp  # 位置服务 Wrapper 实现
└── README.md                     # 本文档
```

**注意**：ohos_wrapper 是一个非常精简的模块，目前只包含位置服务的封装。未来可能会扩展其他功能。

## 架构设计

### Wrapper 层定位

```
┌─────────────────────────────────────────────────────┐
│         WebView 引擎 (Chromium/CEF)                   │
│              (C++ / C 代码)                          │
└────────────────────┬────────────────────────────────┘
                     │ 调用 C 接口
                     ▼
┌─────────────────────────────────────────────────────┐
│           ohos_wrapper (C 接口层)                    │
│  ┌──────────────────────────────────────────────┐   │
│  │  extern "C" OHOS_NWEB_EXPORT                  │   │
│  │  - IsLocationEnable()                         │   │
│  │  - StartLocating()                            │   │
│  │  - StopLocating()                             │   │
│  │  - EnableAbility()                            │   │
│  └──────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────┘
                     │ 调用 C++ API
                     ▼
┌─────────────────────────────────────────────────────┐
│         OpenHarmony Location Service                 │
│              (Locator SDK)                          │
│  ┌──────────────────────────────────────────────┐   │
│  │  Locator::GetInstance()                      │   │
│  │  - IsLocationEnabledV9()                     │   │
│  │  - StartLocatingV9()                         │   │
│  │  - StopLocatingV9()                          │   │
│  │  - EnableAbilityV9()                         │   │
│  └──────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────┘
```

### 与其他层的关系

```
┌──────────────────────────────────────────────────────┐
│                   接口层 (interfaces/)                │
│  - NAPI 接口 (ArkTS/JS)                               │
│  - Native 接口 (C++)                                   │
└────────────────┬─────────────────────────────────────┘
                 │
                 ├───────────────────┬─────────────────┐
                 ▼                   ▼                 ▼
        ┌──────────────┐    ┌──────────────┐   ┌──────────────┐
        │ ohos_wrapper │    │ ohos_adapter │   │ ohos_glue   │
        │   (C 接口)    │    │  (C++ 接口)  │   │  (桥接代码) │
        └──────────────┘    └──────────────┘   └──────────────┘
                 │                   │
                 └───────────────────┴─────────────────┘
                                 ▼
                    ┌──────────────────────┐
                    │ OpenHarmony 系统    │
                    │      服务           │
                    └──────────────────────┘
```

**对比说明**：
- **ohos_wrapper**: 提供 C 接口，简化调用，用于特定场景
- **ohos_adapter**: 提供 C++ 接口，完整的适配器模式，用于系统解耦
- **ohos_glue**: 提供 Glue 代码，用于接口与实现的桥接

## 核心功能

### 位置服务封装

`ohos_wrapper` 目前只包含位置服务的封装，提供 4 个 C 接口函数：

#### 1. IsLocationEnable

**功能**: 检查位置服务是否启用

**C 接口**:
```c
extern "C" OHOS_NWEB_EXPORT bool IsLocationEnable(bool& isEnabled)
```

**参数**:
- `isEnabled`: 输出参数，位置服务是否启用

**返回值**:
- `true`: 调用成功
- `false`: 调用失败

**内部实现**:
```cpp
LocationErrCode ret = g_locatorProxy->IsLocationEnabledV9(isEnabled);
return ret == LocationErrCode::ERRCODE_SUCCESS;
```

#### 2. StartLocating

**功能**: 开始定位

**C 接口**:
```c
extern "C" OHOS_NWEB_EXPORT bool StartLocating(
    std::unique_ptr<RequestConfig>& requestConfig,
    OHOS::sptr<ILocatorCallback>& callback)
```

**参数**:
- `requestConfig`: 定位请求配置（场景、精度等）
- `callback`: 定位回调接口，用于接收位置更新

**返回值**:
- `true`: 开始定位成功
- `false`: 开始定位失败

**内部实现**:
```cpp
LocationErrCode ret = g_locatorProxy->StartLocatingV9(requestConfig, callback);
return ret == LocationErrCode::ERRCODE_SUCCESS;
```

#### 3. StopLocating

**功能**: 停止定位

**C 接口**:
```c
extern "C" OHOS_NWEB_EXPORT bool StopLocating(OHOS::sptr<ILocatorCallback>& callback)
```

**参数**:
- `callback`: 要停止的定位回调

**返回值**:
- `true`: 停止定位成功
- `false`: 停止定位失败

**内部实现**:
```cpp
LocationErrCode ret = g_locatorProxy->StopLocatingV9(callback);
return ret == LocationErrCode::ERRCODE_SUCCESS;
```

#### 4. EnableAbility

**功能**: 启用/禁用位置能力

**C 接口**:
```c
extern "C" OHOS_NWEB_EXPORT bool EnableAbility(bool enable)
```

**参数**:
- `enable`: true=启用，false=禁用

**返回值**:
- `true`: 设置成功
- `false`: 设置失败

**内部实现**:
```cpp
LocationErrCode ret = g_locatorProxy->EnableAbilityV9(enable);
return ret == LocationErrCode::ERRCODE_SUCCESS;
```

## 构建配置

### BUILD.gn 分析

```gn
ohos_shared_library("nweb_ohos_wrapper") {
  # 分支保护（arm64）
  if (target_cpu == "arm64") {
    branch_protector_ret = "pac_ret"
  }

  # 源文件列表（初始为空）
  sources = []

  # 编译选项
  cflags = [
    "-Wall",
    "-Werror",
  ]

  # 头文件路径
  include_dirs = [
    "${target_gen_dir}/../ohos_nweb/include",  # nweb 头文件
    "include",                                   # wrapper 本地头文件
  ]

  # 外部依赖（基础）
  external_deps = [
    "c_utils:utils",      # 工具库
    "hilog:libhilog",     # 日志系统
    "ipc:ipc_core",       # IPC 通信
  ]

  # 内部依赖
  deps = [
    "../ohos_interface:webview_interface_prepare",  # WebView 接口准备
  ]

  # 条件编译：位置服务
  if (webview_location_enable) {
    sources += [ "src/nweb_location_wrapper.cpp" ]

    external_deps += [
      "location:lbsservice_common",  # 位置服务基础库
      "location:locator_sdk",       # Locator SDK
    ]
  }

  part_name = "webview"
  subsystem_name = "web"
}
```

### 条件编译

**启用位置服务**:
```gni
webview_location_enable = true
```

**禁用位置服务**:
```gni
webview_location_enable = false  # 默认值
```

**效果**:
- 启用：编译 `nweb_location_wrapper.cpp`，依赖 `locator_sdk`
- 禁用：不编译任何源文件，库为空

### 输出库

编译后生成 `libnweb_ohos_wrapper.so`，但只有启用位置服务时才有实际内容。

## 工作原理

### 全局 Locator 代理

```cpp
// nweb_location_wrapper.cpp
namespace OHOS::NWeb {
    // 全局 Locator 代理实例
    auto g_locatorProxy = Locator::GetInstance();
}
```

**说明**:
- 使用 OpenHarmony Location SDK 的单例模式
- 全局变量，在库加载时初始化
- 所有定位相关操作都通过这个代理进行

### C 接口封装

```cpp
extern "C" OHOS_NWEB_EXPORT bool StartLocating(
    std::unique_ptr<RequestConfig>& requestConfig,
    OHOS::sptr<ILocatorCallback>& callback)
{
    // 1. 参数检查
    if (!g_locatorProxy || !requestConfig || !callback) {
        WVLOG_E("g_locatorProxy is nullptr");
        return false;
    }

    // 2. 调用 OpenHarmony C++ API
    LocationErrCode ret = g_locatorProxy->StartLocatingV9(
        requestConfig, callback);

    // 3. 错误处理
    if (ret != LocationErrCode::ERRCODE_SUCCESS) {
        WVLOG_E("StartLocating failed, errcode:%{public}d", ret);
        return false;
    }

    // 4. 返回成功
    return true;
}
```

**封装要点**:
1. **extern "C"**: 使用 C 链接约定，避免 C++ 名称修饰
2. **OHOS_NWEB_EXPORT**: 导出符号，使库外部可见
3. **参数验证**: 检查指针和引用的有效性
4. **错误转换**: 将 `LocationErrCode` 转换为 `bool`
5. **日志记录**: 使用 `WVLOG_E` 记录错误信息

### 错误处理

所有 C 接口都遵循统一的错误处理模式：

```cpp
// 模式
bool FunctionName(...) {
    // 1. 检查全局代理
    if (!g_locatorProxy) {
        WVLOG_E("g_locatorProxy is nullptr");
        return false;
    }

    // 2. 调用系统 API
    LocationErrCode ret = g_locatorProxy->SomeFunction(...);

    // 3. 检查返回码
    if (ret != LocationErrCode::ERRCODE_SUCCESS) {
        WVLOG_E("Function failed, errcode:%{public}d", ret);
        return false;
    }

    // 4. 返回成功
    return true;
}
```

**错误码映射**:
- `LocationErrCode::ERRCODE_SUCCESS` → `true`
- 其他任何错误码 → `false`

## 使用场景

### 场景 1: 检查位置服务状态

```cpp
#include "nweb_export.h"

// 检查位置服务是否启用
bool isEnabled = false;
bool success = IsLocationEnable(isEnabled);

if (success) {
    if (isEnabled) {
        // 位置服务已启用
        printf("Location service is enabled\n");
    } else {
        // 位置服务未启用
        printf("Location service is disabled\n");
    }
} else {
    // 检查失败
    printf("Failed to check location service status\n");
}
```

### 场景 2: 开始和停止定位

```cpp
#include "locator.h"
#include "nweb_export.h"

// 1. 创建定位配置
auto requestConfig = std::make_unique<RequestConfig>();
requestConfig->SetScenario(Scenario::UNSET);
requestConfig->SetPriority(LocationRequestPriority::ACCURACY);

// 2. 创建定位回调
class MyLocatorCallback : public ILocatorCallback {
public:
    void OnLocationReport(const std::unique_ptr<Location>& location) override {
        // 接收位置更新
        printf("Location: lat=%f, lon=%f\n",
            location->GetLatitude(),
            location->GetLongitude());
    }

    void OnLocatingStatusChange(int status) override {
        // 定位状态变化
        printf("Status changed: %d\n", status);
    }

    void OnErrorReport(int errorCode) override {
        // 错误报告
        printf("Error occurred: %d\n", errorCode);
    }
};

auto callback = sptr<ILocatorCallback>(new MyLocatorCallback());

// 3. 开始定位
bool success = StartLocating(requestConfig, callback);
if (!success) {
    printf("Failed to start locating\n");
    return;
}

// 4. 停止定位
success = StopLocating(callback);
if (!success) {
    printf("Failed to stop locating\n");
}
```

### 场景 3: 启用/禁用位置能力

```cpp
#include "nweb_export.h"

// 启用位置服务
bool success = EnableAbility(true);
if (success) {
    printf("Location ability enabled\n");
} else {
    printf("Failed to enable location ability\n");
}

// 禁用位置服务
success = EnableAbility(false);
if (success) {
    printf("Location ability disabled\n");
} else {
    printf("Failed to disable location ability\n");
}
```

## 与 ohos_adapter 的区别

| 特性 | ohos_wrapper | ohos_adapter |
|------|-------------|--------------|
| **接口类型** | C (extern "C") | C++ (类和虚函数) |
| **封装粒度** | 细粒度（函数级） | 粗粒度（类级） |
| **设计模式** | Wrapper / Facade | Adapter Pattern |
| **用途** | 简化特定功能调用 | 系统级解耦 |
| **功能范围** | 位置服务（当前） | 40+ 个系统服务 |
| **可测试性** | 有限（无 Mock） | 强（支持 Mock） |
| **依赖关系** | 直接依赖系统 SDK | 通过适配器接口 |
| **扩展性** | 低（手写每个函数） | 高（添加新适配器） |

**何时使用 ohos_wrapper**:
- 需要 C 接口（如跨语言调用）
- 功能简单，不需要完整的适配器模式
- 临时性的快速封装

**何时使用 ohos_adapter**:
- 需要 C++ 接口
- 需要完整的系统解耦
- 需要可测试性（Mock）
- 需要可移植性

## 依赖关系

### 外部依赖

```gn
external_deps = [
    "c_utils:utils",              # 工具库
    "hilog:libhilog",             # 日志系统
    "ipc:ipc_core",               # IPC 通信
    "location:lbsservice_common", # 位置服务基础（条件）
    "location:locator_sdk",       # Locator SDK（条件）
]
```

### 内部依赖

```gn
deps = [
    "../ohos_interface:webview_interface_prepare",  # WebView 接口
]
```

### 间接依赖

通过 `ohos_interface` 间接依赖：
- `ohos_nweb/include`: Nweb 公共头文件
- `ohos_adapter/include`: 适配器接口

## 日志与调试

### 日志标签

使用 `nweb_log.h` 中定义的宏：
```cpp
WVLOG_E("Error message");  // 错误日志
WVLOG_I("Info message");   // 信息日志
WVLOG_D("Debug message");  // 调试日志
```

### 关键日志点

```cpp
// Locator 代理为空
WVLOG_E("g_locatorProxy is nullptr");

// 开始定位失败
WVLOG_E("StartLocating failed, errcode:%{public}d", ret);

// 停止定位失败
WVLOG_E("StopLocating failed, errcode:%{public}d", ret);

// 启用能力失败
WVLOG_E("EnableAbility failed, errcode:%{public}d", ret);
```

### 调试建议

1. **检查库是否编译**：
   ```bash
   # 查看库文件是否存在
   ls -l out/<product>/libnweb_ohos_wrapper.so
   
   # 查看符号表
   nm -D libnweb_ohos_wrapper.so | grep IsLocationEnable
   ```

2. **检查位置服务是否启用**：
   ```bash
   # 查看系统日志
   hilog -T Locator
   
   # 查看日志输出
   hilog -T webadapter
   ```

3. **测试接口调用**：
   ```cpp
   // 编写测试程序
   bool isEnabled = false;
   bool ret = IsLocationEnable(isEnabled);
   printf("IsLocationEnable: ret=%d, enabled=%d\n", ret, isEnabled);
   ```

## 未来扩展

### 可能的扩展方向

虽然当前 `ohos_wrapper` 只包含位置服务，但未来可以扩展到其他功能：

#### 1. 其他 Wrapper 函数

```cpp
// 音频 Wrapper
extern "C" OHOS_NWEB_EXPORT bool StartAudioPlayback();
extern "C" OHOS_NWEB_EXPORT bool StopAudioPlayback();

// 相机 Wrapper
extern "C" OHOS_NWEB_EXPORT bool OpenCamera();
extern "C" OHOS_NWEB_EXPORT bool CloseCamera();

// 传感器 Wrapper
extern "C" OHOS_NWEB_EXPORT bool StartSensor(int type);
extern "C" OHOS_NWEB_EXPORT bool StopSensor(int type);
```

#### 2. 扩展步骤

1. **创建新源文件**:
   ```
   ohos_wrapper/src/nweb_audio_wrapper.cpp
   ohos_wrapper/src/nweb_camera_wrapper.cpp
   ```

2. **实现 C 接口**:
   ```cpp
   extern "C" OHOS_NWEB_EXPORT bool StartAudioPlayback() {
       // 调用 OpenHarmony Audio API
       // 返回结果
   }
   ```

3. **更新 BUILD.gn**:
   ```gn
   if (webview_audio_enable) {
       sources += [ "src/nweb_audio_wrapper.cpp" ]
       external_deps += [ "audio_framework:audio_client" ]
   }
   ```

### 扩展建议

1. **保持简单**: Wrapper 应该是薄层，避免复杂逻辑
2. **统一接口**: 遵循现有的命名和错误处理模式
3. **文档完善**: 为每个新函数添加注释和使用示例
4. **测试覆盖**: 添加单元测试验证功能

## 常见问题

### Q1: 为什么需要 ohos_wrapper，而不是直接使用 ohos_adapter？

**A**:
- **ohos_wrapper** 提供 C 接口，适合从 C 代码调用
- **ohos_adapter** 提供 C++ 接口，适合 C++ 代码
- **ohos_wrapper** 更轻量，没有适配器的开销
- **ohos_wrapper** 可以作为临时封装，快速集成功能

### Q2: 为什么 ohos_wrapper 只包含位置服务？

**A**:
- 位置服务是 Web Geolocation API 的基础
- 位置服务的 C++ 接口相对复杂，需要简化
- 其他功能通过 ohos_adapter 已经足够
- 未来可以根据需要扩展

### Q3: ohos_wrapper 的性能开销如何？

**A**:
- **非常小**: 只是一层薄薄的函数转发
- **无虚函数**: 直接调用系统 API
- **无额外对象**: 使用全局单例
- **内联优化**: 编译器可以内联调用

### Q4: 如何在 WebView 引擎中使用这些接口？

**A**:
```cpp
// 1. 包含头文件
#include "nweb_export.h"

// 2. 链接库
// 在 BUILD.gn 中添加：
// libs = [ "libnweb_ohos_wrapper.so" ]

// 3. 调用接口
bool isEnabled = false;
bool ret = IsLocationEnable(isEnabled);
```

### Q5: ohos_wrapper 是否支持多线程？

**A**:
- **是**: OpenHarmony Location SDK 本身是线程安全的
- **全局代理**: `g_locatorProxy` 是只读的，可以安全并发访问
- **回调**: `ILocatorCallback` 会在独立线程调用
- **注意**: 回调中的操作需要考虑线程安全

## 维护建议

### 添加新功能

1. **评估必要性**:
   - 是否真的需要 Wrapper？
   - 能否通过 ohos_adapter 实现？
   - 是否有跨语言需求？

2. **实现步骤**:
   - 创建新的源文件（如 `nweb_xxx_wrapper.cpp`）
   - 实现 `extern "C"` 接口
   - 添加错误处理和日志
   - 更新 BUILD.gn
   - 编写单元测试
   - 更新文档

3. **代码规范**:
   - 使用 `extern "C"` 导出
   - 使用 `OHOS_NWEB_EXPORT` 宏
   - 统一的错误处理模式
   - 完善的日志记录

### 测试要点

1. **单元测试**:
   ```cpp
   TEST(LocationWrapperTest, IsLocationEnable) {
       bool isEnabled = false;
       bool ret = IsLocationEnable(isEnabled);
       EXPECT_TRUE(ret);
   }
   ```

2. **集成测试**:
   - 测试开始/停止定位的完整流程
   - 测试错误情况（如服务未启动）
   - 测试并发调用

3. **性能测试**:
   - 测试函数调用的开销
   - 测试多次定位的性能

### 文档维护

- 为每个新函数添加注释
- 更新 README.md
- 提供使用示例
- 记录已知限制

## 相关文档

- [主 README.md](../README.md) - WebView 组件总览
- [AGENT.md](../AGENT.md) - 代码仓指南
- [ohos_adapter/README.md](../ohos_adapter/README.md) - 适配器层文档
- [ohos_nweb/README.md](../ohos_nweb/README.md) - 核心引擎文档
- [OpenHarmony Location SDK](https://gitee.com/openharmony/location) - 位置服务文档

## 许可证

Apache License 2.0
