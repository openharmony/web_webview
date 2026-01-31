# 如何添加 web_config.xml 配置项

本文档说明如何在 `ohos_nweb/etc/web_config.xml` 中添加新的配置项。

> **⚠️ 重要提示**:
>
> **推荐优先使用 [PARAM 参数配置](./HOW_TO_ADD_PARAM_CONFIG.md)**
>
> PARAM 参数相比 web_config.xml 具有以下优势:
> - ✅ **运行时可修改** - 无需重新编译,立即生效
> - ✅ **动态调优** - 支持运行时调整性能参数
> - ✅ **便于调试** - 快速切换配置进行测试
> - ✅ **持久化存储** - 系统重启后保持配置
>
> **何时使用 web_config.xml**:
> - ✅ 核心架构配置 (渲染进程数、多进程模式等)
> - ✅ 启动时必须确定的配置
> - ✅ 需要随代码版本控制的配置
> - ✅ 安全敏感的默认配置
>
> **何时使用 PARAM 参数**:
> - ✅ 性能调优 (缓存大小、线程数等)
> - ✅ 功能开关 (实验性功能)
> - ✅ 调试选项 (日志级别、trace 等)
> - ✅ 运行时动态配置
>
> 详见: [HOW_TO_ADD_PARAM_CONFIG.md](./HOW_TO_ADD_PARAM_CONFIG.md)

## 配置系统架构

### 核心组件

1. **web_config.xml** - 配置文件 (`ohos_nweb/etc/web_config.xml`)
2. **NWebConfigHelper** - 配置解析类 (`ohos_nweb/src/nweb_config_helper.cpp`)
3. **configMap** - 配置映射表 (定义在 `nweb_config_helper.cpp` 中)

### 配置流程

```
web_config.xml
    ↓ ParseConfig()
NWebConfigHelper::ReadConfig()
    ↓ 遍历 XML 节点
configMap 查找映射
    ↓ 转换为命令行参数
initArgs->AddArg()
    ↓
传递给 Chromium 引擎
```

## 添加配置项的步骤

### 步骤 1: 在 web_config.xml 中添加配置项

在 `ohos_nweb/etc/web_config.xml` 的相应分类下添加新配置:

```xml
<initConfig>
    <yourCategory>
        <yourConfigName>true</yourConfigName>  <!-- 或 false, 或其他值 -->
    </yourCategory>
</initConfig>
```

**分类建议**:
- `renderConfig` - 渲染相关配置
- `mediaConfig` - 媒体相关配置
- `settingConfig` - 设置和功能开关
- `performanceConfig` - 性能相关配置
- 或创建新的分类

### 步骤 2: 在 configMap 中添加映射

在 `ohos_nweb/src/nweb_config_helper.cpp` 的 `configMap` 中添加映射:

```cpp
const std::unordered_map<std::string_view, std::function<std::string(std::string&)>> configMap = {
    // ... 现有配置 ...

    { "yourCategory/yourConfigName",
        [](std::string& contentStr) {
            // 配置值转换逻辑
            return contentStr == "true" ? std::string("--your-command-line-arg") : std::string();
        } },
};
```

**关键点**:
- **键**: `"分类名/配置名"` (必须与 XML 中的路径完全一致)
- **值**: Lambda 函数,接收配置值字符串,返回命令行参数

### 步骤 3: 测试配置

1. 重新编译项目:
```bash
./build.sh --product-name <产品名> --build-target //base/web/webview/ohos_nweb:libnweb
```

2. 修改配置文件:
```bash
# 编辑 /system/etc/web/web_config.xml 或自定义路径
```

3. 查看日志验证:
```bash
hilog -T ArkWeb
```

## 配置类型示例

### 1. 布尔类型配置 (开关)

**XML**:
```xml
<settingConfig>
    <enableYourFeature>true</enableYourFeature>
</settingConfig>
```

**configMap**:
```cpp
{ "settingConfig/enableYourFeature",
    [](std::string& contentStr) {
        return contentStr == "true"
            ? std::string("--enable-your-feature")
            : std::string();
    } },
```

### 2. 数值类型配置

**XML**:
```xml
<renderConfig>
    <yourParameter>42</yourParameter>
</renderConfig>
```

**configMap**:
```cpp
{ "renderConfig/yourParameter",
    [](std::string& contentStr) {
        return std::string("--your-parameter=") + contentStr;
    } },
```

### 3. 字符串类型配置

**XML**:
```xml
<userAgentConfig>
    <customValue>YourString</customValue>
</userAgentConfig>
```

**configMap**:
```cpp
{ "userAgentConfig/customValue",
    [](std::string& contentStr) {
        return std::string("--custom-value=") + contentStr;
    } },
```

### 4. 反向布尔配置 (false 启用)

**XML**:
```xml
<settingConfig>
    <disableYourFeature>true</disableYourFeature>
</settingConfig>
```

**configMap**:
```cpp
{ "settingConfig/disableYourFeature",
    [](std::string& contentStr) {
        return contentStr == "true"
            ? std::string("--disable-your-feature")
            : std::string();
    } },
```

## 现有配置分类参考

### renderConfig (渲染配置)
- `renderProcessCount` - 渲染进程数量限制

### mediaConfig (媒体配置)
- `backgroundMediaShouldSuspend` - 后台媒体暂停

### settingConfig (功能开关)
- `enablePrinting` - 打印功能
- `enableHttpCacheSimple` - HTTP 缓存简化模式
- `enableCalcTabletMode` - 平板模式计算
- `disableMobileStyleSheet` - 禁用移动端样式表

### performanceConfig (性能配置)
- `visibleAreaRatio` - 可见区域比例
- `maxFdNumber` - 最大文件描述符数量

### deleteArgsConfig (删除参数)
- `enableOopGpu` - 进程外 GPU

## 注意事项

### 1. 命名规范

**分类命名**:
- 使用驼峰命名法 (CamelCase)
- 以配置类型结尾: `Config`, `Setting`, `Option`
- 示例: `renderConfig`, `mediaConfig`, `settingConfig`

**配置项命名**:
- 使用驼峰命名法
- 布尔值: `enable*` (启用) 或 `disable*` (禁用)
- 参数值: 使用描述性名称
- 示例: `enablePrinting`, `renderProcessCount`, `visibleAreaRatio`

### 2. 命令行参数规范

**Chromium 命令行参数**:
- 通常以 `--` 开头
- 使用小写和连字符: `--enable-feature`
- 详见 Chromium 源码中的开关列表

**自定义前缀**:
- OpenHarmony 特定参数使用 `--ohos-` 前缀
- 示例: `--ohos-enable-printing`, `--ohos-user-agent-value`

### 3. 配置值类型

| XML 值类型 | C++ 类型 | 示例 |
|-----------|---------|------|
| `true`/`false` | `std::string` | `<enableXXX>true</enableXXX>` |
| 整数 | `std::string` | `<count>42</count>` |
| 浮点数 | `std::string` | `<ratio>0.5</ratio>` |
| 字符串 | `std::string` | `<value>custom</value>` |

### 4. 配置文件位置优先级

配置系统按以下优先级读取配置:
1. `/system/etc/web/web_config.xml` (系统默认)
2. 自定义配置路径 (通过配置策略)

## 完整示例

### 示例: 添加新的性能优化开关

#### 1. 修改 web_config.xml

```xml
<initConfig>
    <performanceConfig>
        <enableTurboMode>true</enableTurboMode>
    </performanceConfig>
</initConfig>
```

#### 2. 修改 nweb_config_helper.cpp

```cpp
{ "performanceConfig/enableTurboMode",
    [](std::string& contentStr) {
        return contentStr == "true"
            ? std::string("--enable-turbo-mode")
            : std::string();
    } },
```

#### 3. 重新编译并测试

```bash
# 编译
./build.sh --product-name rk3568 --build-target //base/web/webview/ohos_nweb:libnweb

# 查看日志确认参数生效
hilog -T ArkWeb | grep "turbo"
```

## 高级配置

### 复杂逻辑配置

如果配置转换需要复杂逻辑:

```cpp
{ "settingConfig/complexConfig",
    [](std::string& contentStr) {
        if (contentStr == "auto") {
            return std::string("--complex-mode=auto");
        } else if (contentStr == "manual") {
            return std::string("--complex-mode=manual");
        } else {
            int value = std::stoi(contentStr);
            return std::string("--complex-mode=") + std::to_string(value);
        }
    } },
```

### 条件组合配置

如果多个配置项需要组合使用:

```cpp
// 分别添加到 configMap
{ "category/config1", [](std::string& s) { /* ... */ } },
{ "category/config2", [](std::string& s) { /* ... */ } },

// 在 ReadConfig 中会自动添加到 initArgs
```

## 调试技巧

### 1. 查看配置解析日志

```bash
hilog -T ArkWeb
```

常见日志:
- `read config from init node` - 从 initConfig 节点读取
- `read config from root node` - 从根节点读取
- `not found for web_config` - 未找到 configMap 映射

### 2. 验证命令行参数

在 `nweb_helper.cpp` 中添加日志:
```cpp
WVLOG_I("Adding arg: %{public}s", param.c_str());
```

### 3. 检查配置文件路径

```cpp
// 在 ParseConfig 中查看实际路径
WVLOG_D("web config file path:%{public}s", cfgFilePath.c_str());
```

## 相关文件

- `ohos_nweb/etc/web_config.xml` - 配置文件
- `ohos_nweb/include/nweb_config_helper.h` - 配置辅助类头文件
- `ohos_nweb/src/nweb_config_helper.cpp` - 配置解析实现
- `ohos_nweb/src/nweb_helper.cpp` - NWebHelper 实现

## 配置方式对比

### web_config.xml vs PARAM 参数

| 特性 | web_config.xml | PARAM 参数 |
|------|---------------|-----------|
| **修改时机** | 编译时 | 运行时 |
| **生效方式** | 重新编译构建 | 立即生效或重启进程 |
| **适用场景** | 核心架构配置 | 性能调优、调试 |
| **灵活性** | 低 (需重新编译) | 高 (动态修改) |
| **持久化** | 随代码版本控制 | 系统参数存储 |
| **调试效率** | 低 (编译周期长) | 高 (立即生效) |

### 推荐配置方式选择

**使用 web_config.xml (本文档)**:
- ✅ 渲染进程数量 (`renderProcessCount`)
- ✅ 多进程模式 (`enableSingleRenderProcess`)
- ✅ 核心功能开关 (`enablePrinting`)
- ✅ 启动时必须确定的架构配置
- ✅ 安全敏感的默认配置

**使用 PARAM 参数**:
- ✅ 性能调优 (缓存大小、线程数等)
- ✅ 功能开关 (实验性功能、优化选项)
- ✅ 调试选项 (日志级别、trace 开关)
- ✅ 运行时动态配置
- ✅ 需要频繁调整的参数

**混合使用**:
- web_config.xml 设置默认值
- PARAM 参数运行时覆盖
- 通过 `WriteConfigValueToSysPara()` 将 XML 配置写入参数

详见: [HOW_TO_ADD_PARAM_CONFIG.md](./HOW_TO_ADD_PARAM_CONFIG.md)

## 参考资料

- [HOW_TO_ADD_PARAM_CONFIG.md](./HOW_TO_ADD_PARAM_CONFIG.md) - PARAM 参数配置指南 (推荐)
- [主 README](../README.md) - 项目概述
- [AGENT.md](../AGENT.md) - 代码仓指南
- Chromium 命令行参数列表: `chrome_switches.cc` (Chromium 源码)
