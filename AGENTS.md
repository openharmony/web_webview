# ArkWeb 组件指引

## 项目定位

本仓库对应 OpenHarmony `base/web/webview`，是 ArkWeb 组件的核心实现。ArkWeb 是基于 Chromium 和 CEF 构建的 WebView Native 引擎。

优先按这些目录定位问题：

- `ohos_nweb/`：核心引擎层，NWebHelper、SurfaceAdapter、引擎初始化、WebView 创建。
- `ohos_adapter/`：系统适配层，40+ 适配器对接 OpenHarmony 系统服务。
- `ohos_interface/`：胶水层接口定义，双向解耦的关键。`ohos_nweb/` 方向 53+ 接口（✅ 允许新增），`ohos_adapter/` 方向 66+ 接口（❌ 不允许新增）。
- `interfaces/`：多语言接口（NAPI、Native、CJ、ANI）。
- `ohos_glue/`：接口层与胶水层之间的桥接代码。
- `arkweb_utils/`：工具库，配置解析等通用功能。
- `sa/`：系统服务（app_fwk_update、web_native_messaging）。
- `test/`：测试代码（unittest、fuzztest）。

## 构建和验证

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。

```sh
# 完整构建
./build.sh --product-name rk3568 --build-target webview

# 构建特定模块
./build.sh --product-name rk3568 --build-target //base/web/webview/ohos_nweb:libnweb

# 单元测试
./build.sh --product-name rk3568 --build-target webview_test

# 模糊测试
./build.sh --product-name rk3568 --build-target //base/web/webview/test/fuzztest/load_fuzzer:fuzztest
```

提交使用 `git commit -s`，并保留 `Co-Authored-By: Agent`。

## 知识索引

稳定背景知识放在 `docs/knowledge/`。改动前按场景读取对应文件：

| 场景 | 先读 |
|------|------|
| 编译特性开关、条件编译、可选功能的自动检测与 Mock | `docs/knowledge/HOW_TO_ADD_BUILD_FEATURE.md` |
| PARAM 参数配置（运行时可修改，推荐） | `docs/knowledge/HOW_TO_ADD_PARAM_CONFIG.md` |
| 胶水层接口、双向解耦、nweb/adapter 方向、代码生成桥接 | `docs/knowledge/HOW_TO_ADD_GLUE_LAYER_INTERFACE.md` |
| XML 配置（编译时配置，需重编） | `docs/knowledge/HOW_TO_ADD_XML_CONFIG.md` |

各模块详细文档：

| 模块 | 文档 |
|------|------|
| ohos_nweb（核心引擎） | `ohos_nweb/README.md` |
| ohos_adapter（系统适配） | `ohos_adapter/README.md` |
| ohos_interface（胶水层） | `ohos_interface/README.md` |
| ohos_wrapper（封装层） | `ohos_wrapper/README.md` |
| arkweb_utils（工具库） | `arkweb_utils/README.md` |
| NAPI 接口 | `interfaces/kits/napi/README.md` |
| ANI 接口 | `interfaces/kits/ani/README.md` |
| CJ 接口 | `interfaces/kits/cj/README.md` |
| Native NDK 接口 | `interfaces/native/README.md` |
| nativecommon 通用组件 | `interfaces/kits/nativecommon/README.md` |
| 系统服务 | `sa/app_fwk_update/README.md`、`sa/web_native_messaging/README.md` |
| 测试 | `test/README.md` |

## 分层架构

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

## 常见开发场景

### 添加新的 Public API

1. `ohos_interface/include/ohos_nweb/` — 定义接口类
2. `ohos_nweb/` — 实现具体功能
3. `interfaces/kits/` — 多语言绑定（NAPI / Native / ANI / CJ）
4. `test/unittest/` — 单元测试
5. 更新 BUILD.gn 和 bundle.json

### 添加胶水层 API（ohos_adapter 方向）

⚠️ ohos_adapter 接口**不允许新增**，只能在现有接口中扩展功能。

### 添加配置项

推荐 PARAM 参数（运行时可修改），见 `docs/knowledge/HOW_TO_ADD_PARAM_CONFIG.md`。编译时配置用 XML，见 `docs/knowledge/HOW_TO_ADD_XML_CONFIG.md`。

### 添加测试用例

- **单元测试**：`test/unittest/`，gtest 框架，公共代码在 `test/unittest/common/`
- **模糊测试**：`test/fuzztest/`，LLVM Fuzzer 框架
- 详见 `test/README.md`

## 调试

```sh
hilog -T ArkWeb              # WebView 日志
hilog -T webadapter          # 适配层日志
param get web.*              # 查看运行时配置
cat /etc/web/web_config.xml  # 查看 XML 配置
ps -A | grep arkweb          # 检查进程
hidumper -s 8610 -a --memory # SA 内存分析
```

崩溃文件位置：`/data/log/crash/`，由 `arkweb_crashpad_handler` 生成。

## 项目约束

- 所有系统交互通过 `ohos_adapter/` 解耦，便于移植和测试。
- renderer/GPU/Utility 进程由 Chromium 管理，通过 `web_config.xml` 控制。
- ohos_adapter 方向接口不允许新增，功能只能在现有接口内扩展。
- 多语言接口（NAPI/ANI/CJ）共享 nativecommon 组件，使用 TransferReferencedCount 管理引用计数。
