# arkweb_utils

## 概述

`arkweb_utils` 是 ArkWeb WebView 组件的**工具库**，提供引擎版本管理、库加载、路径解析、配置管理等核心工具功能。该库被编译为 `libarkweb_utils.so`，作为平台 SDK (`innerapi_tags: [ "platformsdk" ]`) 供其他模块使用。

**⚠️ 重要约束：本库不得引入额外的依赖关系！**

### 核心职责

1. **Web 引擎版本管理** - 管理多个 WebView 引擎版本（Legacy M114、Evergreen M132）的切换
2. **动态库加载与预加载** - 负责 `libarkweb_engine.so` 和适配器库的加载、预加载优化
3. **路径解析与管理** - 提供各种运行时路径的获取和解析
4. **应用信息管理** - 从命令行和配置中提取和管理应用信息（BundleName、ApiVersion、AppVersion）
5. **Shared RELRO 支持** - 提供只读重定位地址空间的优化支持（仅 arm64）

## 目录结构

```
arkweb_utils/
├── arkweb_utils.h                 # 主头文件，定义所有公共接口
├── arkweb_utils.cpp               # 主实现文件
├── arkweb_preload_common.h        # 预加载相关接口
├── arkweb_preload_common.cpp      # 预加载实现
├── BUILD.gn                       # 构建配置
└── README.md                      # 本文档
```

## 核心功能

### 1. Web 引擎版本管理

#### 引擎版本枚举
```cpp
enum class ArkWebEngineVersion {
    SYSTEM_DEFAULT = 0,   // 系统默认
    M114 = 1,             // Legacy 引擎 (基于 Chromium 114)
    M132 = 2,             // Evergreen 引擎 (基于 Chromium 132)
    PLAYGROUND = 99998,   // 测试环境
    SYSTEM_EVERGREEN = 99999  // 系统级 Evergreen
};

enum class ArkWebEngineType {
    LEGACY = M114,        // 传统引擎
    EVERGREEN = M132,     // 常青引擎
    PLAYGROUND = PLAYGROUND
};
```

#### 主要接口
```cpp
// 获取当前活跃的引擎版本
ArkWebEngineVersion getActiveWebEngineVersion();

// 设置活跃的引擎版本
void setActiveWebEngineVersion(ArkWebEngineVersion version);

// 检查是否为 Evergreen 引擎
bool IsActiveWebEngineEvergreen();

// 获取引擎类型
ArkWebEngineType getActiveWebEngineType();
```

#### 版本选择逻辑
引擎版本由以下因素决定（优先级从高到低）：
1. **应用强制指定** - 从命令行参数 `#--appEngineVersion=` 读取
2. **系统参数** - `web.engine.enforce` (强制)、`web.engine.default` (默认)
3. **Legacy 应用名单** - 配置文件中的 `web.engine.legacyApp` 数组
4. **云端配置** - 系统版本文件 `/system/etc/ArkWebSafeBrowsing/generic/version.txt`
5. **更新版本文件** - `/data/service/el1/public/update/param_service/install/system/etc/ArkWebSafeBrowsing/generic/version.txt`

### 2. 应用信息管理

#### 应用信息提取
从渲染进程命令行参数中提取应用信息：

```cpp
// 命令行参数格式：
// #--appEngineVersion=1
// #--appBundleName=com.example.app
// #--appApiVersion=9
// #--appVersion=1.0.0

// 更新应用信息
void UpdateAppInfoFromCmdline(std::string& renderCmd);

// 提取并移除参数
std::string ExtractAndRemoveParam(std::string& renderCmd, const std::string& prefix);

// 获取应用信息
std::string GetBundleName();
std::string GetApiVersion();
std::string GetAppVersion();
```

#### 内部设置接口（供内部模块使用）
```cpp
void SetActiveWebEngineVersionInner(ArkWebEngineVersion version);
void SetBundleNameInner(const std::string& bundleName);
void SetApiVersionInner(const std::string& apiVersion);
void SetAppVersionInner(const std::string& appVersion);
```

### 3. 路径管理

#### 核心路径常量

**ArkWebCore HAP 路径**（按架构区分）：
```cpp
// arm64
ARK_WEB_CORE_HAP_LIB_PATH = "/data/storage/el1/bundle/arkwebcore/libs/arm64"
ARK_WEB_CORE_LEGACY_HAP_LIB_PATH = "/data/storage/el1/bundle/arkwebcorelegacy/libs/arm64"

// arm
ARK_WEB_CORE_HAP_LIB_PATH = "/data/storage/el1/bundle/arkwebcore/libs/arm"
ARK_WEB_CORE_LEGACY_HAP_LIB_PATH = "/data/storage/el1/bundle/arkwebcorelegacy/libs/arm"

// x86_64
ARK_WEB_CORE_HAP_LIB_PATH = "/data/storage/el1/bundle/arkwebcore/libs/x86_64"
```

**系统预装 HAP 路径**：
```cpp
PRECONFIG_LEGACY_HAP_PATH = "/system/app/ArkWebCoreLegacy/ArkWebCoreLegacy.hap"
PRECONFIG_EVERGREEN_HAP_PATH = "/system/app/com.ohos.arkwebcore/ArkWebCore.hap"
PRECONFIG_EVERGREEN_WATCH_HAP_PATH = "/system/app/NWeb/NWeb.hap"
```

#### 路径获取接口
```cpp
// 获取 arkweb 库路径（沙箱内）
std::string GetArkwebLibPath();

// 获取 arkweb 命名空间
std::string GetArkwebNameSpace();

// 获取 arkweb 相对路径（用于 Bundle）
std::string GetArkwebRelativePathForBundle();

// 获取 arkweb 相对路径（用于 Mock）
std::string GetArkwebRelativePathForMock();

// 获取 arkweb 安装路径
std::string GetArkwebInstallPath();

// 获取 Bundle 安装路径（预加载使用）
std::string GetArkwebBundleInstallLibPath();

// 获取 OHOS 适配器 Glue 库路径
std::string GetOhosAdptGlueSrcLibPath();
```

### 4. 动态库加载与预加载

#### 预加载模式
```cpp
enum class RenderPreLoadMode {
    PRELOAD_NO = 0,         // 不预加载
    PRELOAD_PARTIAL = 1,    // 只预加载 libohos_adapter_glue_source.z.so
    PRELOAD_FULL = 2        // 预加载 libohos_adapter_glue_source.z.so 和 libarkweb_engine.so
};
```

#### 预加载策略
- **编译时强制** - 如果定义了 `PRELOAD_RENDER_LIB`，强制使用 `PRELOAD_FULL` 模式
- **系统参数** - `const.startup.nwebspawn.preloadMode`（0=不预加载，1=部分，2=完全）
- **内存限制** - RAM <= 8GB 时，不预加载（避免内存压力）

#### 预加载接口
```cpp
// 为 Browser 进程预加载
void PreloadArkWebLibForBrowser();

// 为 Render 进程预加载
void PreloadArkWebLibForRender();
```

**预加载流程**（Render 进程）：
1. 检查系统总内存
2. 读取预加载模式参数
3. 部分预加载：只加载 `libohos_adapter_glue_source.z.so`
4. 完全预加载：先加载适配器库，再加载 `libarkweb_engine.so`

#### 动态加载接口
```cpp
// 打开 arkweb 库
void DlopenArkWebLib();

// 关闭 arkweb 库
int DlcloseArkWebLib();

// 初始化 ArkWeb Bridge Helper
void* ArkWebBridgeHelperSharedInit(bool runMode, const std::string& mockBundlePath = "");
```

### 5. Shared RELRO 支持（仅 arm64）

Shared RELRO (Relocation Read-Only) 是一种内存优化技术，允许多个进程共享相同的只读重定位页面，减少内存占用。

#### RELRO 接口
```cpp
// 预留地址空间（512MB）
bool ReserveAddressSpace();

// 在子进程中创建 RELRO 文件
void CreateRelroFileInSubProc();

// 使用 RELRO 文件加载库
void* LoadWithRelroFile(const std::string& lib, Dl_namespace* dlns);
```

#### RELRO 路径
```cpp
SHARED_RELRO_DIR = "/data/service/el1/public/for-all-app/shared_relro"
NWEB_RELRO_PATH = SHARED_RELRO_DIR + "/libwebviewchromium64.relro"
RESERVED_VMA_SIZE = 512 * 1024 * 1024  // 512MB
```

**⚠️ 约束**：Shared RELRO 功能仅支持 arm64 架构，且在 ASAN 模式下禁用。

### 6. 宏定义辅助工具

#### 版本检查宏
```cpp
// 如果引擎版本低于 minVersion，则返回
RETURN_IF_UNSUPPORTED_ENGINE(minVersion, funName)

// 检查是否从 M114 调用
IS_CALLING_FROM_M114()

// 如果从 M114 调用，则返回
RETURN_IF_CALLING_FROM_M114()
```

这些宏用于在新引擎中屏蔽旧版本不支持的功能。

### 7. 配置文件管理

#### ArkWebCoreCfg.json
配置文件路径：
```
/data/service/el1/public/update/param_service/install/system/etc/ArkWebSafeBrowsing/generic/ArkWebCoreCfg.json
```

#### 支持的配置项
```json
{
  "web.engine.default": 1,        // 默认引擎类型 (1=Legacy, 2=Evergreen)
  "web.engine.enforce": 2,        // 强制使用引擎类型
  "web.engine.legacyApp": [       // 需要使用 Legacy 引擎的应用名单
    "com.example.app1",
    "com.example.app2"
  ]
}
```

#### 版本文件
- 系统版本：`/system/etc/ArkWebSafeBrowsing/generic/version.txt`
- 更新版本：`/data/service/el1/public/update/param_service/install/system/etc/ArkWebSafeBrowsing/generic/version.txt`

## 依赖关系

### 外部依赖（BUILD.gn）
```gn
external_deps = [
    "init:libbegetutil",              # 参数获取
    "hilog:libhilog",                 # 日志系统
    "jsoncpp:jsoncpp",                # JSON 解析
    "bundle_framework:appexecfwk_base",   # Bundle 框架基础
    "selinux_adapter:librestorecon",      # SELinux 上下文恢复
    "ability_runtime:app_context",        # 应用上下文
    "bundle_framework:appexecfwk_core"    # Bundle 框架核心
]
```

### 内部依赖
```gn
include_dirs = [
    "../ohos_nweb/include/",              # Nweb 公共头文件
    "../ohos_adapter/hiviewdfx_adapter/include/",  # HiLog 日志适配
]
```

### ⚠️ 依赖约束

**重要**：`libarkweb_utils.so` 作为一个**基础工具库**，有以下约束：

1. **不得引入额外的系统依赖**
   - 只能依赖已经在 `BUILD.gn` 中声明的组件
   - 新增依赖需要谨慎评估，避免循环依赖

2. **不得依赖 WebView 核心模块**
   - 不能依赖 `ohos_nweb` 的实现部分
   - 只能使用其头文件定义的接口

3. **保持轻量级**
   - 避免引入大型框架
   - 避免重量级模板和复杂的第三方库

4. **线程安全**
   - 全局变量使用 `std::mutex` 保护（如 `g_appInfoMutex`）
   - 避免在多线程环境下出现竞态条件

## 编译配置

### 构建目标
```gn
ohos_shared_library("libarkweb_utils") {
  output_name = "arkweb_utils"
  sources = [
    "arkweb_utils.cpp",
    "arkweb_preload_common.cpp"
  ]
  ...
}
```

### 支持的架构
- `arm64` - 64位 ARM（完整支持，包括 Shared RELRO）
- `arm` - 32位 ARM
- `x86_64` - 64位 x86（部分支持）

### 编译宏
```cpp
webview_arm64          // ARM64 架构
webview_arm            // ARM32 架构
webview_x86_64         // x86_64 架构
IS_ASAN                // ASAN 模式
ASAN_DETECTOR          // ASAN 检测器
PRELOAD_RENDER_LIB     // 强制预加载渲染库
```

## 使用示例

### 示例 1: 获取当前引擎版本并加载库
```cpp
#include "arkweb_utils.h"

using namespace OHOS::ArkWeb;

void InitWebEngine() {
    // 获取当前引擎版本
    ArkWebEngineVersion version = getActiveWebEngineVersion();

    // 检查是否为 Evergreen
    if (IsActiveWebEngineEvergreen()) {
        // 使用 Evergreen 特性
    }

    // 加载 arkweb 库
    DlopenArkWebLib();
}
```

### 示例 2: 从命令行提取应用信息
```cpp
#include "arkweb_utils.h"

using namespace OHOS::ArkWeb;

void ProcessRenderCmd(std::string& renderCmd) {
    // 更新应用信息（会从命令行提取并移除相关参数）
    UpdateAppInfoFromCmdline(renderCmd);

    // 获取应用信息
    std::string bundleName = GetBundleName();
    std::string apiVersion = GetApiVersion();

    // 使用应用信息选择引擎
    SelectWebcoreBeforeProcessRun(bundleName);
}
```

### 示例 3: 预加载优化
```cpp
#include "arkweb_preload_common.h"

using namespace OHOS::ArkWeb;

void RenderProcessInit() {
    // 预加载 ArkWeb 库（会根据内存和配置决定是否加载）
    PreloadArkWebLibForRender();
}
```

### 示例 4: 版本兼容性检查
```cpp
#include "arkweb_utils.h"

void NewFeatureOnlySupportedInM132() {
    // 如果引擎版本低于 M132，直接返回
    RETURN_IF_UNSUPPORTED_ENGINE(ArkWebEngineVersion::M132, __func__);

    // M132+ 的新特性实现
    // ...
}
```

## 日志与调试

### 日志标签
使用 `nweb_log.h` 中定义的宏：
- `WVLOG_I` - 信息日志
- `WVLOG_E` - 错误日志
- `WVLOG_D` - 调试日志

### 关键日志点
1. 引擎版本选择
2. 动态库加载成功/失败
3. 预加载模式决策
4. 配置文件解析
5. 应用信息提取

### 调试建议
1. 使用 `hilog -T ArkWeb` 查看 WebView 相关日志
2. 检查系统参数：`param get web.engine.default`
3. 检查配置文件：`cat /data/service/el1/public/.../ArkWebCoreCfg.json`
4. 检查版本文件：`cat /system/etc/ArkWebSafeBrowsing/generic/version.txt`

## 性能优化

### 预加载策略
- **大内存设备** (>8GB RAM): 使用 `PRELOAD_PARTIAL` 或 `PRELOAD_FULL`
- **小内存设备** (<=8GB RAM): 使用 `PRELOAD_NO`，避免内存压力

### Shared RELRO
- 多进程共享 512MB 地址空间
- 减少实际物理内存占用
- 仅支持 arm64 架构

### 路径缓存
- 应用信息提取后会缓存
- 引擎版本决策后会缓存
- 避免重复的参数读取

## 常见问题

### Q1: 为什么有些接口名带 `Inner` 后缀？
**A**: `SetBundleNameInner` 等接口是供内部模块使用的，不应该直接调用。应使用 `UpdateAppInfoFromCmdline` 统一提取和设置。

### Q2: 预加载的库什么时候被加载？
**A**:
- Browser 进程：在 `PreloadArkWebLibForBrowser()` 中加载
- Render 进程：在 `PreloadArkWebLibForRender()` 中加载
- 加载时机：进程初始化早期，在 WebView 实例创建之前

### Q3: Shared RELRO 失败会影响功能吗？
**A**: 不会。Shared RELRO 是纯优化功能，失败后会回退到常规加载方式。

### Q4: 如何切换到 Legacy 引擎？
**A**: 有三种方式：
1. 在应用命令行添加：`#--appEngineVersion=1`
2. 修改配置文件：设置 `web.engine.default = 1`
3. 将应用加入 `web.engine.legacyApp` 数组

### Q5: x86_64 为什么不支持某些功能？
**A**: x86_64 主要用于模拟器和开发调试，部分功能（如 Shared RELRO）仅在真实设备架构（arm/arm64）上启用。

## 维护建议

### 代码规范
1. 保持接口的 `ARKWEB_EXPORT` 宏
2. 使用 `std::mutex` 保护全局状态
3. 避免在头文件中暴露实现细节
4. 新增功能要考虑多版本兼容性

### 测试要点
1. 单元测试：`test/unittest/arkweb_utils_test`
2. 模糊测试：`test/fuzztest/arkwebutils_fuzzer`
3. 版本切换测试：Legacy ↔ Evergreen
4. 预加载功能测试
5. 路径解析测试（多架构）

### 版本演进
- 新增功能要使用 `RETURN_IF_UNSUPPORTED_ENGINE` 宏保护
- 保持向后兼容性
- 避免破坏现有的版本选择逻辑

## 相关文档

- [主 README.md](../README.md) - WebView 组件总览
- [CLAUDE.md](../CLAUDE.md) - 代码仓指南
- [ohos_nweb/README.md](../ohos_nweb/README.md) - 核心引擎文档

## 许可证

Apache License 2.0
