# 如何使用 Param 参数进行 ArkWeb 功能配置

本文档说明如何使用 OpenHarmony 的 param (系统参数) 进行 ArkWeb 功能的动态配置。

> **✅ 推荐配置方式**:
>
> PARAM 参数是 ArkWeb 的**推荐配置方式**,相比 web_config.xml 具有以下优势:
> - ✅ **运行时可修改** - 无需重新编译,立即生效
> - ✅ **动态调优** - 支持运行时调整性能参数
> - ✅ **便于调试** - 快速切换配置进行测试
> - ✅ **持久化存储** - 系统重启后保持配置
> - ✅ **灵活性高** - 适合频繁调整的参数
>
> **适用场景**:
> - ✅ 性能调优 (缓存大小、线程数、超时时间等)
> - ✅ 功能开关 (实验性功能、优化选项)
> - ✅ 调试选项 (日志级别、trace 开关、dump 等)
> - ✅ 运行时动态配置
> - ✅ 需要现场调整的参数
>
> **不适用场景** (请使用 [web_config.xml](./HOW_TO_ADD_XML_CONFIG.md)):
> - ❌ 核心架构配置 (渲染进程数、多进程模式)
> - ❌ 启动时必须确定的配置
> - ❌ 安全敏感的默认配置
>
> 详见: [配置方式对比](#配置方式对比)

## Param 参数系统概述

### 什么是 Param 参数

Param 是 OpenHarmony 的系统参数管理机制，提供:
- **运行时可修改** - 无需重新编译和重启
- **持久化存储** - 系统重启后保持配置
- **访问控制** - 通过 `.dac` 文件管理权限
- **分类管理** - 支持 `persist.` 持久化参数和普通参数

### Param vs web_config.xml

| 特性 | web_config.xml | Param 参数 |
|------|---------------|-----------|
| 修改时机 | 编译时 | 运行时 |
| 生效方式 | 重新编译构建 | 立即生效或重启进程 |
| 使用场景 | 核心功能开关 | 调试、优化、动态调优 |
| 权限管理 | 文件权限 | DAC 权限控制 |

## Param 参数架构

### 核心文件

```
ohos_nweb/
├── etc/para/
│   ├── web.para           # 参数定义文件 (参数名=默认值)
│   └── web.para.dac       # 权限控制文件 (用户:组:权限)
└── src/
    ├── nweb_config_helper.cpp  # 参数写入 (SetParameter)
    └── nweb_helper.cpp         # 参数读取 (GetParameter)
```

### 参数流转过程

```
web_config.xml (定义)
    ↓ ReadConfig()
WriteConfigValueToSysPara()
    ↓ OHOS::system::SetParameter()
系统参数存储 (/etc/param/)
    ↓ OHOS::system::GetParameter()
运行时读取使用
```

## Param 参数类型

### 1. 持久化参数 (persist.)

以 `persist.` 开头,系统重启后保持:

```bash
# 定义
persist.arkwebcore.install_path = /system/app/com.ohos.arkwebcore/ArkWebCore.hap
persist.arkwebcore.package_name = com.ohos.arkwebcore
```

**用途**:
- 安装路径配置
- 包名配置
- 需要持久化的设置

### 2. 普通参数 (web.*, web.debug.*)

运行时参数,可能因系统更新重置:

```bash
# 性能优化
web.optimization=true

# 调试开关
web.debug.devtools = false
web.debug.trace = false
web.debug.dump.on = false

# 功能开关
web.ohos.vulkan = None
web.ohos.dvsync = false
web.oop.gpu = None

# TCP 参数
web.TCPConnectedSocket.initialCongestionWindowSize = -1
```

**用途**:
- 功能开关
- 性能调优
- 调试选项

### 3. 动态写入参数

由 `web_config.xml` 配置项自动写入的参数:

```bash
# 由 flowBufferConfig/maxFdNumber 写入
web.flowbuffer.maxfd

# 由 TCPConnectedSocketConfig/initialCongestionWindowSize 写入
web.TCPConnectedSocket.initialCongestionWindowSize
```

## 添加 Param 参数的步骤

### 步骤 1: 在 web.para 中定义参数默认值

编辑 `ohos_nweb/etc/para/web.para`:

```bash
# 格式: 参数名 = 默认值
web.your.newparam = defaultValue

# 布尔类型
web.your.feature.enable = true

# 数值类型
web.your.feature.timeout = 5000

# 字符串类型
web.your.feature.mode = auto

# 持久化参数
persist.arkwebcore.your.config = /path/to/config
```

### 步骤 2: 在 web.para.dac 中设置权限

编辑 `ohos_nweb/etc/para/web.para.dac`:

```bash
# 格式: 参数名 = 用户:组:权限 (八进制)
web.your.newparam = root:root:0644

# 可读写 (shell 可修改)
web.your.feature.debug = root:shell:0775

# 只读
web.your.feature.readonly = root:root:0444

# 系统服务专用
web.engine.install.completed = app_fwk_update:app_fwk_update:0775
```

**权限说明**:
- `0644` (rw-r--r--): root 可读写,其他用户只读
- `0775` (rwxrwxr-x): 所有者和组可读写执行,其他用户可读执行
- `0444` (r--r--r--): 所有用户只读

### 步骤 3: 在代码中读取参数

#### 读取字符串参数

```cpp
#include "parameters.h"

// 在 nweb_helper.cpp 或其他文件中
std::string configValue = OHOS::system::GetParameter("web.your.newparam", "defaultValue");

if (!configValue.empty()) {
    // 使用配置值
    initArgs->AddArg(std::string("--your-param=").append(configValue));
}
```

#### 读取布尔参数

```cpp
#include "parameters.h"

bool isEnabled = OHOS::system::GetBoolParameter("web.your.feature.enable", false);

if (isEnabled) {
    // 启用功能
    initArgs->AddArg("--enable-your-feature");
}
```

#### 读取整数参数

```cpp
#include "parameters.h"

int timeout = OHOS::system::GetIntParameter("web.your.feature.timeout", 5000);

// 使用参数值
std::string paramValue = std::string("--timeout=") + std::to_string(timeout);
initArgs->AddArg(paramValue);
```

### 步骤 4: 在代码中写入参数 (可选)

如果需要从 web_config.xml 写入参数,修改 `nweb_config_helper.cpp`:

```cpp
void NWebConfigHelper::WriteConfigValueToSysPara(const std::string &configName, const std::string &value)
{
    // 现有配置...
    if (configName == "flowBufferConfig/maxFdNumber") {
        OHOS::system::SetParameter("web.flowbuffer.maxfd", value);
    } else if (configName == "TCPConnectedSocketConfig/initialCongestionWindowSize") {
        OHOS::system::SetParameter("web.TCPConnectedSocket.initialCongestionWindowSize", value);
    }
    // 添加新配置
    else if (configName == "yourCategory/yourConfig") {
        OHOS::system::SetParameter("web.your.newparam", value);
    }
}
```

### 步骤 5: 重新编译和安装

```bash
# 编译
./build.sh --product-name <产品名> --build-target //base/web/webview/ohos_nweb:libnweb

# 安装到设备
# 参数文件会安装到 /etc/param/ 目录
```

## Param 参数使用场景

### 场景 1: 动态功能开关

**web.para**:
```bash
# 实验性功能开关
web.experimental.feature_x = false
```

**代码**:
```cpp
bool enableFeatureX = OHOS::system::GetBoolParameter("web.experimental.feature_x", false);
if (enableFeatureX) {
    initArgs->AddArg("--enable-feature-x");
}
```

**运行时修改**:
```bash
# 启用功能
param set web.experimental.feature_x true

# 查看当前值
param get web.experimental.feature_x
```

### 场景 2: 性能调优

**web.para**:
```bash
# 缓存大小
web.cache.size.mb = 100

# 线程数
web.worker.thread.count = 4
```

**代码**:
```cpp
int cacheSize = OHOS::system::GetIntParameter("web.cache.size.mb", 50);
initArgs->AddArg(std::string("--cache-size-mb=") + std::to_string(cacheSize));
```

### 场景 3: 调试选项

**web.para**:
```bash
# 调试开关
web.debug.devtools = false
web.debug.trace = false
web.debug.dump.on = false
```

**代码**:
```cpp
bool enableDevTools = OHOS::system::GetBoolParameter("web.debug.devtools", false);
if (enableDevTools) {
    initArgs->AddArg("--remote-debugging-port=9222");
}
```

### 场景 4: 路径配置

**web.para**:
```bash
# 持久化路径配置
persist.arkwebcore.install_path = /system/app/com.ohos.arkwebcore/ArkWebCore.hap
persist.arkwebcore.package_name = com.ohos.arkwebcore
```

**代码** (nweb_helper.cpp:727):
```cpp
std::string arkWebInstallPath = OHOS::system::GetParameter("persist.arkwebcore.install_path", "");
if (!arkWebInstallPath.empty()) {
    initArgs->AddArg(std::string("--arkwebcore-install-path=").append(arkWebInstallPath));
}
```

## Param 参数命名规范

### 命名模式

| 模式 | 用途 | 示例 |
|------|------|------|
| `web.*` | 普通 web 参数 | `web.optimization` |
| `web.debug.*` | 调试相关 | `web.debug.trace` |
| `web.ohos.*` | OHOS 特定功能 | `web.ohos.vulkan` |
| `persist.arkwebcore.*` | 持久化配置 | `persist.arkwebcore.install_path` |

### 命名建议

1. **使用小写和点分隔**
   - ✅ `web.feature.enable`
   - ❌ `web_feature_enable`

2. **按功能分组**
   - ✅ `web.cache.size`, `web.cache.mode`
   - ❌ `web.cacheSize`, `web.webCacheMode`

3. **布尔值使用 enable/disable 前缀**
   - ✅ `web.feature.enable`
   - ✅ `web.optimization.disable`

4. **持久化参数使用 persist. 前缀**
   - ✅ `persist.arkwebcore.config`
   - ❌ `arkwebcore.persist.config`

## Param 参数管理命令

### 查看参数值

```bash
# 查看单个参数
param get web.optimization

# 查看所有 web 参数
param get web.*

# 查看持久化参数
param get persist.arkwebcore.*
```

### 设置参数值

```bash
# 设置布尔值
param set web.optimization true

# 设置字符串值
param set web.debug.mode verbose

# 设置数值
param set web.cache.size 100
```

### 持久化参数

```bash
# 持久化参数会在系统重启后保持
param set persist.arkwebcore.config /new/path
```

### 权限检查

```bash
# 查看参数权限
ls -l /etc/param/web.para.dac

# 检查是否可写
param set web.test.value 0
# 如果失败,检查权限或使用 root
```

## Param 参数优先级

```
代码默认值
    ↓
web.para 默认值
    ↓
param set 运行时设置 (最高优先级)
```

**示例**:
```cpp
// 代码默认值: 5000
int timeout = OHOS::system::GetIntParameter("web.timeout", 5000);

// web.para: web.timeout = 3000
// 结果: 3000

// param set web.timeout 10000
// 结果: 10000 (最高优先级)
```

## 注意事项

### 1. 权限管理

- 使用 `web.para.dac` 严格控制权限
- 调试参数可设为 `root:shell:0775` 允许 shell 修改
- 核心参数设为 `root:root:0644` 只允许 root 修改

### 2. 参数值类型

| 参数类型 | 默认值表示 | 示例 |
|---------|-----------|------|
| 布尔 | `true`/`false` | `web.optimization=true` |
| 整数 | 数字 | `web.timeout=5000` |
| 字符串 | 字符串 | `web.mode=auto` |
| 空/无 | `None`/`none` | `web.debug.mode=None` |

### 3. 性能考虑

- 频繁读取参数可能影响性能
- 建议在初始化时读取一次,缓存到内存
- 避免在热路径中调用 `GetParameter`

### 4. 兼容性

- 新增参数需提供合理的默认值
- 考虑旧版本系统的兼容性
- 参数命名避免与系统参数冲突

### 5. 调试

```bash
# 查看参数是否生效
param get web.your.param

# 查看日志
hilog -T ArkWeb | grep "your.param"

# 检查权限
ls -l /etc/param/
```

## 完整示例

### 示例: 添加自定义缓存配置

#### 1. 定义参数 (web.para)

```bash
# 缓存大小 (MB)
web.cache.size = 100

# 缓存模式
web.cache.mode = simple

# 是否启用缓存
web.cache.enable = true
```

#### 2. 设置权限 (web.para.dac)

```bash
web.cache.size = root:root:0644
web.cache.mode = root:root:0644
web.cache.enable = root:shell:0775
```

#### 3. 代码实现 (nweb_helper.cpp)

```cpp
#include "parameters.h"

// 在初始化函数中
bool enableCache = OHOS::system::GetBoolParameter("web.cache.enable", true);
if (enableCache) {
    // 获取缓存大小
    int cacheSize = OHOS::system::GetIntParameter("web.cache.size", 100);
    initArgs->AddArg(std::string("--cache-size-mb=") + std::to_string(cacheSize));

    // 获取缓存模式
    std::string cacheMode = OHOS::system::GetParameter("web.cache.mode", "simple");
    if (cacheMode == "simple") {
        initArgs->AddArg("--enable-http-cache-simple");
    }
}
```

#### 4. 运行时修改

```bash
# 修改缓存大小
param set web.cache.size 200

# 修改缓存模式
param set web.cache.mode advanced

# 查看当前值
param get web.cache.*
```

#### 5. 验证

```bash
# 查看日志确认参数生效
hilog -T ArkWeb | grep "cache"

# 检查缓存行为
```

## Param 参数调试

### 1. 查看参数读取日志

```cpp
// 添加调试日志
WVLOG_I("Reading param: web.your.param, value: %{public}s", value.c_str());
```

### 2. 验证参数文件

```bash
# 检查参数文件是否安装
ls -l /etc/param/web.para

# 查看参数文件内容
cat /etc/param/web.para | grep your.param

# 检查权限
cat /etc/param/web.para.dac | grep your.param
```

### 3. 参数读取失败排查

```bash
# 1. 检查参数是否定义
param get web.your.param

# 2. 检查权限
ls -l /etc/param/web.para.dac

# 3. 检查代码中参数名是否正确 (区分大小写)

# 4. 查看系统日志
hilog -T ArkWeb
hilog -T PARAM
```

## 相关文件

### 配置文件
- `ohos_nweb/etc/para/web.para` - 参数定义
- `ohos_nweb/etc/para/web.para.dac` - 权限控制
- `ohos_nweb/BUILD.gn` - 构建配置 (安装参数文件)

### 源代码
- `ohos_nweb/src/nweb_config_helper.cpp` - 参数写入
- `ohos_nweb/src/nweb_helper.cpp` - 参数读取
- `ohos_nweb/include/nweb_config_helper.h` - 配置辅助类头文件

### 相关文档
- [HOW_TO_ADD_XML_CONFIG.md](./HOW_TO_ADD_XML_CONFIG.md) - web_config.xml 配置指南
- [README.md](./README.md) - 项目概述
- [AGENT.md](../AGENT.md) - 代码仓指南

## 配置方式对比

### PARAM 参数 vs web_config.xml

| 特性 | PARAM 参数 | web_config.xml |
|------|-----------|---------------|
| **修改时机** | 运行时 | 编译时 |
| **生效方式** | 立即生效或重启进程 | 重新编译构建 |
| **灵活性** | 高 (动态修改) | 低 (需重新编译) |
| **持久化** | 系统参数存储 | 随代码版本控制 |
| **调试效率** | 高 (立即生效) | 低 (编译周期长) |
| **适用场景** | 性能调优、调试 | 核心架构配置 |

### 推荐配置方式选择

**使用 PARAM 参数 (本文档)**:
- ✅ 性能调优 (缓存大小、线程数、超时时间等)
- ✅ 功能开关 (实验性功能、优化选项)
- ✅ 调试选项 (日志级别、trace 开关、dump 等)
- ✅ 运行时动态配置
- ✅ 需要频繁调整的参数
- ✅ 需要现场调整的配置

**使用 web_config.xml**:
- ✅ 核心架构配置 (渲染进程数、多进程模式)
- ✅ 启动时必须确定的配置
- ✅ 安全敏感的默认配置
- ✅ 需要随代码版本控制的配置
- ✅ 影响引擎架构的配置

**混合使用最佳实践**:
1. **web_config.xml** 设置核心架构默认值
2. **PARAM 参数** 用于运行时调优和调试
3. 通过 `WriteConfigValueToSysPara()` 将特定 XML 配置写入参数
4. PARAM 参数可覆盖 web_config.xml 的设置

详见: [HOW_TO_ADD_XML_CONFIG.md](./HOW_TO_ADD_XML_CONFIG.md)

## 参考资料

- OpenHarmony 参数系统文档
- `parameters.h` - 参数 API 头文件
- OpenHarmony 系统参数规范
