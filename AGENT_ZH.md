# AGENT.md

## 项目概述

本代码仓是 **OpenHarmony 系统 ArkWeb 组件**的核心实现，提供 WebView 组件能力。ArkWeb是基于Chromium和CEF(Chromium Embedded Framework) 构建的WebView Native引擎。

## 构建和测试命令

### 构建系统
本项目使用 **GN (Generate Ninja)** + **Ninja** 构建系统。

### 完整构建命令
```bash
# 标准完整构建（在 OpenHarmony 代码仓根目录执行）
./build.sh --product-name <产品名> --build-target webview
./build.sh --product-name rk3568 --build-target webview  # 示例：RK3568 产品

# 只构建特定模块
./build.sh --product-name <产品名> --build-target //base/web/webview/ohos_nweb:libnweb
./build.sh --product-name <产品名> --build-target //base/web/webview/ohos_nweb:nweb_hap
```

### 单元测试命令
```bash
# 运行所有单元测试
./build.sh --product-name <产品名> --build-target webview_test

# 运行特定单元测试
./build.sh --product-name <产品名> --build-target //base/web/webview/test/unittest/nweb_helper_test:unittest
./build.sh --product-name <产品名> --build-target //base/web/webview/test/unittest/napi_web_cookie_manager_test:unittest

# 安装并运行测试（需要先构建）
# 测试输出路径: out/<产品名>/tests/webview/
```

### 模糊测试命令
```bash
# 构建模糊测试
./build.sh --product-name <产品名> --build-target //base/web/webview/test/fuzztest/load_fuzzer:fuzztest
./build.sh --product-name <产品名> --build-target //base/web/webview/test/fuzztest/executejavascript_fuzzer:fuzztest

# 运行模糊测试（需要使用 OpenHarmony 测试框架）
# 测试二进制文件位于: out/<产品名>/tests/fuzztest/
```

## 核心架构

### 整体分层架构

```
应用层 (ArkTS/JS/CJ/C++)
  ↓ interfaces/ (NAPI/Native/CJ/ANI)
  ↓
ohos_nweb/ 核心引擎层
  ↓ ohos_interface/include/ohos_nweb/ (胶水层: 53+ 接口)
  ↓
ArkWebCore.hap (Chromium 引擎)
  ↓ ohos_interface/include/ohos_adapter/ (胶水层: 66+ 接口)
  ↓
ohos_adapter/ 系统适配层 (40+ 适配器)
  ↓
OpenHarmony 系统服务层
```

**核心创新**: `ohos_interface/include/` 胶水层实现双向解耦
- **ohos_nweb/** (系统侧 → 内核, 53+ 接口): ✅ 允许新增
- **ohos_adapter/** (内核 → 系统侧, 66+ 接口): ❌ 不允许新增

详见: [ohos_interface/README.md](./ohos_interface/README.md)

### 核心组件说明

| 组件 | 说明 | 详细文档 |
|------|------|----------|
| **ohos_nweb/** | 核心引擎层,提供 NWebHelper、SurfaceAdapter 等 | [ohos_nweb/README.md](./ohos_nweb/README.md) |
| **ohos_adapter/** | 系统适配层,40+ 适配器对接 OpenHarmony 系统 | [ohos_adapter/README.md](./ohos_adapter/README.md) |
| **ohos_interface/** | 胶水层接口定义,双向解耦的关键 | [ohos_interface/README.md](./ohos_interface/README.md) |
| **ohos_wrapper/** | 封装层,依赖 OpenHarmony 系统 | [ohos_wrapper/README.md](./ohos_wrapper/README.md) |
| **arkweb_utils/** | 工具库,配置解析等通用功能 | [arkweb_utils/README.md](./arkweb_utils/README.md) |
| **interfaces/** | 多语言接口 (NAPI, Native, CJ, ANI) | NAPI: [interfaces/kits/napi/README.md](interfaces/kits/napi/README.md)<br/>ANI: [interfaces/kits/ani/README.md](interfaces/kits/ani/README.md)<br/>CJ API: [interfaces/kits/cj/README.md](interfaces/kits/cj/README.md)<br/>Native API: [interfaces/native/README.md](interfaces/native/README.md) |
| **sa/** | 系统服务 (app_fwk_update, web_native_messaging) | [sa/app_fwk_update/README.md](./sa/app_fwk_update/README.md)<br/>[sa/web_native_messaging/README.md](./sa/web_native_messaging/README.md) |
| **test/** | 测试代码 (unittest, fuzztest) | [test/README.md](test/README.md) |

## 开发工作流

### 常见开发场景

#### 1. 添加新的 Public API (应用层接口)

为 ArkTS/JS/C++/Rust/Python 等语言提供新的 WebView 功能 API,包括以下步骤:

1. **[胶水层接口定义](ohos_interface/README.md)** (`ohos_interface/include/ohos_nweb/`) - 定义接口类
2. **[核心引擎实现](ohos_nweb/README.md)** (`ohos_nweb/`) - 实现具体功能
3. **多语言绑定**:
   - **[NAPI 绑定](interfaces/kits/napi/README.md)** (`interfaces/kits/napi/`) - ArkTS接口
   - **[Native 绑定](interfaces/native/README.md)** (`interfaces/native/`) - C/C++ NDK 接口
   - **[ANI 绑定](interfaces/kits/ani/README.md)** (`interfaces/kits/ani/`) - 高性能 ArkTS Static 接口
   - **[CJ FFI 绑定](interfaces/kits/cj/README.md)** (`interfaces/kits/cj/`) - CJ 接口
4. **测试** (`test/unittest/`) - 单元测试
5. **构建配置** - 更新 BUILD.gn 和 bundle.json

详见: **[interfaces/kits/napi/README.md](./interfaces/kits/napi/README.md)**

#### 2. 添加胶水层 API (ohos_adapter 方向)

为 ArkWebCore.hap 访问 OpenHarmony 系统服务添加适配器接口:

1. **胶水层接口定义** (`ohos_interface/include/ohos_adapter/`) - 定义接口类
2. **适配器实现** (`ohos_adapter/`) - 实现系统服务调用
3. **核心引擎集成** (`ohos_nweb/`) - 使用适配器接口

⚠️ **重要约束**: ohos_adapter 接口**不允许新增**,只能在现有接口中扩展功能

#### 3. 添加配置项

ArkWeb 支持两种配置方式,**推荐使用 PARAM 参数**:

##### 3.1 PARAM 参数配置 (运行时配置) ✅ 推荐

- **文件位置**: `ohos_nweb/etc/para/web.para`
- **权限配置**: `ohos_nweb/etc/para/web.para.dac`
- **代码读取**: 使用 `OHOS::system::GetBoolParameter()` 等函数
- **运行时修改**: `param set web.performance.turbo true`
- **优势**: 运行时可修改、立即生效、便于调试

详见: **[ohos_nweb/HOW_TO_ADD_PARAM_CONFIG.md](./ohos_nweb/HOW_TO_ADD_PARAM_CONFIG.md)**

##### 3.2 XML 配置 (编译时配置)

- **文件位置**: `ohos_nweb/etc/web_config.xml`
- **配置映射**: `ohos_nweb/src/nweb_config_helper.cpp` 中的 configMap
- **适用场景**: 核心架构配置、启动时必须确定的配置
- **劣势**: 需要重新编译、灵活性低

详见: **[ohos_nweb/HOW_TO_ADD_XML_CONFIG.md](./ohos_nweb/HOW_TO_ADD_XML_CONFIG.md)**

#### 4. 添加 nativecommon 通用组件

为 NAPI 和 ANI 接口添加共享的数据结构封装:

1. **定义数据结构** (`interfaces/kits/nativecommon/`) - 继承 TransferReferencedCount
2. **NAPI 使用** - 使用 napi_wrap 包装,自动管理引用计数
3. **ANI 使用** - 使用 SetNativePointer 包装,自动管理引用计数

详见: **[interfaces/kits/nativecommon/README.md](./interfaces/kits/nativecommon/README.md)**

#### 5. 添加条件编译特性

为可选功能添加编译时开关,根据产品配置自动包含/排除功能:

1. **定义开关** (`config.gni`) - 添加 `webview_<功能>_enable` 变量
2. **自动检测** - 根据 `global_parts_info` 检测系统依赖
3. **BUILD.gn 使用** - 条件添加源文件、依赖、宏定义
4. **代码中使用** - 使用 `#ifdef` 控制代码路径
5. **提供 Mock 实现** - 为禁用状态提供空实现

详见: **[HOW_TO_ADD_BUILD_FEATURE.md](./HOW_TO_ADD_BUILD_FEATURE.md)**

#### 6. 添加测试用例

为新功能添加单元测试和模糊测试:

1. **单元测试** (`test/unittest/`) - 使用 gtest 框架
   - 创建测试目录: `test/unittest/<模块名>_test/`
   - 编写测试类继承 `::testing::Test`
   - 实现 `SetUp()` 和 `TearDown()`
   - 使用 `TEST_F()` 或 `HWTEST_F()` 编写测试用例
   - 编写 BUILD.gn 配置测试目标

2. **模糊测试** (`test/fuzztest/`) - 使用 LLVM Fuzzer 框架
   - 创建 Fuzzer 目录: `test/fuzztest/<功能名>_fuzzer/`
   - 实现 `LLVMFuzzerTestOneInput()` 函数
   - 使用 `FuzzedDataProvider` 解析随机输入
   - 配置 project.xml 设置运行参数
   - 编写 BUILD.gn 配置 Fuzzer 目标

3. **公共代码** (`test/unittest/common/`) - 复用测试辅助代码
   - `nweb_create_window.h/cpp` - 窗口创建辅助函数
   - 测试固件 (Test Fixture)
   - Mock 类和辅助函数

详见: **[test/README.md](./test/README.md)**

## 调试指南

### 日志查看
```bash
# 查看 WebView 相关日志
hilog -T ArkWeb
hilog -T webadapter

# 查看配置
cat /etc/web/web_config.xml
param get web.*

# 检查进程
ps -A | grep arkweb

# SA内存分析
hidumper -s 8610 -a --memory
```

### 问题定位

**编译错误**:
- 检查 `bundle.json` 中是否声明了所需依赖
- 检查 `BUILD.gn` 中的 `deps` 和 `external_deps`
- 确认目标架构 (arm64/arm/x86_64) 的预编译文件存在

**运行时崩溃**:
- 使用 `hilog` 查看日志：`hilog -T ArkWeb`
- 检查 `arkweb_crashpad_handler` 是否正常工作
- 查看 `ohos_nweb/etc/web_config.xml` 配置是否正确
- 崩溃文件位置: `/data/log/crash/`

**功能异常**:
- 使用 `hiviewdfx_adapter` 的 HiTrace 进行链路追踪
- 检查 `hiviewdfx_adapter` 的 HiSysEvent 事件上报
- 在 `ohos_nweb/src/nweb_hisysevent.cpp`中查看事件定义

## 重要提示

- **适配器模式**: 所有系统交互通过 `ohos_adapter/`解耦,便于移植和测试
- **多进程架构**: renderer/GPU/Utility 进程由 Chromium 管理,通过 `web_config.xml` 控制
- **崩溃处理**: `arkweb_crashpad_handler` 捕获崩溃,生成 minidump 到 `/data/log/crash/`

