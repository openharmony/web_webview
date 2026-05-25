# ArkWeb 组件指引

## 项目定位

本仓库对应 OpenHarmony `base/web/webview`，是 ArkWeb 组件的核心实现。ArkWeb 是基于 Chromium 和 CEF 构建的 WebView Native 引擎。

## 快速路由

- `ohos_nweb/`：核心引擎层，包含 NWebHelper、SurfaceAdapter、引擎初始化和 WebView 创建。
- `ohos_adapter/`：系统适配层，所有 OpenHarmony 系统服务调用应通过这里解耦。
- `ohos_interface/`：胶水层接口定义；`ohos_nweb/` 方向允许新增接口，`ohos_adapter/` 方向不允许新增接口。
- `interfaces/`：多语言接口层，包含 NAPI、Native、CJ、ANI。
- `ohos_glue/`：接口层与胶水层之间的桥接代码，生成物不要手改。
- `arkweb_utils/`：工具库和通用配置解析。
- `sa/`：系统服务能力，如 app_fwk_update、web_native_messaging。
- `test/`：单元测试和模糊测试。

构建入口：从 OpenHarmony 源码根执行 `./build.sh --product-name <product> --build-target <target>`。

提交使用 `git commit -s`，并保留 `Co-Authored-By: Agent`。

## 知识索引

稳定背景知识放在 `docs/knowledge/`。改动前按场景读取对应文件：

| 场景 | 先读 |
|---|---|
| 编译特性开关、条件编译、可选功能的自动检测与 Mock | `docs/knowledge/HOW_TO_ADD_BUILD_FEATURE.md` |
| PARAM 参数配置（运行时可修改，推荐） | `docs/knowledge/HOW_TO_ADD_PARAM_CONFIG.md` |
| 胶水层接口、双向解耦、nweb/adapter 方向、代码生成桥接 | `docs/knowledge/HOW_TO_ADD_GLUE_LAYER_INTERFACE.md` |
| XML 配置（编译时配置，需重编） | `docs/knowledge/HOW_TO_ADD_XML_CONFIG.md` |

## 分层架构

```text
应用层 (ArkTS/JS/CJ/C++)
  ↓ interfaces/ (NAPI/Native/CJ/ANI)
ohos_nweb/ 核心引擎层
  ↓ ohos_interface/include/ohos_nweb/
ArkWebCore.hap (Chromium 引擎)
  ↓ ohos_interface/include/ohos_adapter/
ohos_adapter/ 系统适配层
  ↓
OpenHarmony 系统服务层
```

## 常见场景

- 添加胶水层 API：见 `docs/knowledge/HOW_TO_ADD_GLUE_LAYER_INTERFACE.md`。
- 添加配置项：优先 PARAM，见 `docs/knowledge/HOW_TO_ADD_PARAM_CONFIG.md`；编译时配置用 XML，见 `docs/knowledge/HOW_TO_ADD_XML_CONFIG.md`。
- 添加测试：单元测试在 `test/unittest/`，模糊测试在 `test/fuzztest/`。

## 项目约束

- 所有系统交互通过 `ohos_adapter/` 解耦，便于移植和测试。
- renderer/GPU/Utility 进程由 Chromium 管理，通过 `web_config.xml` 控制。
- `ohos_adapter` 方向接口不允许新增，功能只能在现有接口内扩展。
- 多语言接口（NAPI/ANI/CJ）共享 nativecommon 组件，使用 `TransferReferencedCount` 管理引用计数。
