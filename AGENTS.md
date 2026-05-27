# ArkWeb 组件指引

## 项目定位

本仓库对应 OpenHarmony `base/web/webview`，是 ArkWeb 组件的核心实现。ArkWeb 是基于 Chromium 和 CEF 构建的 WebView Native 引擎。上层应用通过 ArkTS/JS 调用 `<Web>` 组件，最终由本仓库的 Native 引擎承载渲染和交互。

## 代码地图

### 目录与分层

```
应用层 (ArkTS/JS/CJ/C++)
  ↓ interfaces/ (NAPI/Native/CJ/ANI 多语言绑定)
ohos_nweb/ 核心引擎层（WebView 生命周期、页面加载、Surface 管理）
  ↓ ohos_interface/include/ohos_nweb/ (nweb 方向：系统→内核，允许新增)
ArkWebCore.hap (Chromium 引擎，由 chromium_arkweb 仓库编译)
  ↓ ohos_interface/include/ohos_adapter/ (adapter 方向：内核→系统，禁止新增)
ohos_adapter/ 系统适配层（40+ 适配器，解耦 OpenHarmony 系统服务）
  ↓
OpenHarmony 系统服务层
```

### 目录职责

| 目录 | 职责 | 常见操作 |
|------|------|----------|
| `ohos_nweb/` | 核心引擎：NWebHelper、SurfaceAdapter、引擎初始化、WebView 创建与销毁、页面导航 | 新增 WebView 功能、修改引擎行为、处理生命周期 |
| `ohos_adapter/` | 系统适配：40+ 适配器将 OpenHarmony 系统服务调用解耦（图形、输入、音频、剪贴板、网络等） | 新增系统服务对接、修改适配器实现 |
| `ohos_interface/` | 胶水层接口定义：双向解耦的纯虚接口。nweb 方向 68+ 接口（✅ 允许新增），adapter 方向 53+ 接口（❌ 禁止新增） | 定义新接口、扩展现有接口方法 |
| `ohos_glue/` | 桥接代码：`prepare.sh` 自动生成的 bridge/cpptoc/ctocpp 三层桥接 | ⚠️ 生成物不要手改，修改接口后重新跑 `prepare.sh` |
| `interfaces/` | 多语言绑定：NAPI(ArkTS)、Native(C/C++ NDK)、ANI(ArkTS Static)、CJ(Cangjie FFI) | 新增 Public API、修改接口签名 |
| `arkweb_utils/` | 工具库：配置解析、通用功能 | 新增通用工具函数 |
| `sa/` | 系统服务：app_fwk_update、web_native_messaging | SA 生命周期管理 |
| `test/` | 测试：unittest(gtest)、fuzztest(LLVM Fuzzer) | 新增测试用例 |

## 知识路由

遇到问题先定位场景，再读对应文档。文档包含完整的领域概念和操作指引。

### 按场景路由

| 你要做什么 / 遇到什么问题 | 先读 |
|---|---|
| 新增胶水层接口、理解 nweb/adapter 双向解耦、代码生成桥接机制 | `docs/knowledge/HOW_TO_ADD_GLUE_LAYER_INTERFACE.md` |
| 添加运行时可修改的配置参数（推荐方式） | `docs/knowledge/HOW_TO_ADD_PARAM_CONFIG.md` |
| 添加编译时配置项（需重编）、XML 配置 | `docs/knowledge/HOW_TO_ADD_XML_CONFIG.md` |
| 编译特性开关、条件编译、可选功能的自动检测与 Mock | `docs/knowledge/HOW_TO_ADD_BUILD_FEATURE.md` |

### 领域词汇速查

遇到以下术语时，应理解其在本仓库中的含义：

- **NWeb**：ArkWeb 的核心引擎类，`ohos_nweb/` 中的 `NWebHelper` 管理其生命周期
- **ArkWebCore.hap**：Chromium 引擎预编译包，由 `chromium_arkweb` 仓库编译，本仓库通过胶水层加载
- **胶水层（Glue Layer）**：`ohos_interface/` 中定义的纯虚接口，通过 `prepare.sh` 生成 `ohos_glue/` 中的桥接代码
- **nweb 方向 / adapter 方向**：胶水层的两个数据流方向。nweb = 系统侧→内核，adapter = 内核→系统侧
- **bridge / cpptoc / ctocpp**：桥接代码的三层结构。bridge 封装调用，cpptoc 将 C++ 回调转 C 结构体，ctocpp 反向解包
- **NAPI / ANI / CJ / Native**：四种多语言绑定方式，共享 `nativecommon` 组件
- **TransferReferencedCount**：多语言接口共享的引用计数管理机制
- **PARAM 参数**：运行时可修改的配置，通过 `param get/set web.*` 访问
- **web_config.xml**：编译时 XML 配置，控制 renderer/GPU/Utility 进程等
- **prepare.sh**：胶水层代码生成脚本，修改接口后必须重新运行
- **crashpad**：崩溃收集机制，产出位于 `/data/log/crash/`
- **SA（System Ability）**：OpenHarmony 系统能力，本仓库有 app_fwk_update 和 web_native_messaging

## 专家经验与约束

### 硬性约束（不可违反）

- **ohos_adapter 方向接口禁止新增文件**：只能在现有接口中扩展方法，这是版本兼容性硬性要求
- **所有系统交互必须通过 `ohos_adapter/` 解耦**：禁止在 `ohos_nweb/` 中直接调用 OpenHarmony 系统服务 API
- **ohos_glue/ 生成物禁止手改**：桥接代码由 `prepare.sh` 生成，修改接口后重跑生成
- **Public API 兼容性**：修改 `interfaces/` 中的接口签名前必须评估向后兼容性，破坏性变更需升版并走正式废弃流程
- **DCO 签名**：提交必须使用 `git commit -s`

### 变更前确认（Ask Before）

- 修改 `interfaces/` 中任何已有 API 的签名或语义
- 涉及安全/权限/信任相关的行为变更
- 涉及协议兼容性或持久化数据格式变更
- 新增或修改第三方依赖、License
- 删除或重命名 Public API
- 修改 `web_config.xml` 的默认值

### 反模式（不要这样做）

- ❌ 在 `ohos_nweb/` 中直接调用 OHOS 系统服务（应通过 adapter 解耦）
- ❌ 手动编辑 `ohos_glue/` 中的生成代码（应修改接口后跑 `prepare.sh`）
- ❌ 在 `ohos_interface/include/ohos_adapter/` 下新增接口文件（应扩展现有接口）
- ❌ 不经兼容性评估修改 Public API 签名（应先评估影响范围）
- ❌ 绕过 `ohos_adapter/` 在核心引擎中硬编码系统服务调用（应走适配层）

### 依赖关系边界

```
┌─────────────────────────────────────────────────────┐
│  interfaces/ (NAPI/ANI/CJ/Native)                   │
│    可依赖: ohos_nweb/, ohos_interface/              │
│    不可依赖: ohos_adapter/, ohos_glue/              │
├─────────────────────────────────────────────────────┤
│  ohos_nweb/ (核心引擎)                               │
│    可依赖: ohos_interface/, arkweb_utils/           │
│    不可直接依赖: ohos_adapter/ (通过胶水层)          │
├─────────────────────────────────────────────────────┤
│  ohos_interface/ (胶水层接口，纯虚类)                │
│    不依赖本仓库任何模块（仅 C++ 标准库和基础类型）    │
├─────────────────────────────────────────────────────┤
│  ohos_adapter/ (系统适配)                            │
│    可依赖: ohos_interface/, OpenHarmony SDK         │
│    不可依赖: ohos_nweb/, interfaces/                │
├─────────────────────────────────────────────────────┤
│  ohos_glue/ (桥接，自动生成)                         │
│    可依赖: ohos_interface/                          │
│    不可手改                                         │
└─────────────────────────────────────────────────────┘
```

## 构建与验证

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。

### 最小验证命令

```sh
# 编译验证（确认编译通过）
./build.sh --product-name rk3568 --build-target webview

# 编译特定模块（更快）
./build.sh --product-name rk3568 --build-target //base/web/webview/ohos_nweb:libnweb
./build.sh --product-name rk3568 --build-target //base/web/webview/ohos_adapter:webview_adapter
```

### 测试命令

```sh
# 单元测试
./build.sh --product-name rk3568 --build-target webview_test

# 模糊测试
./build.sh --product-name rk3568 --build-target //base/web/webview/test/fuzztest/load_fuzzer:fuzztest
```

### 验证清单（按变更类型）

| 变更类型 | 最小验证 |
|---|---|
| 修改内部实现（ohos_nweb/ohos_adapter） | 编译通过 + 相关模块单测 |
| 新增/修改胶水层接口 | 编译通过 + prepare.sh 重新生成 + 全量单测 |
| 新增/修改 Public API（interfaces/） | 编译通过 + 全量单测 + NAPI/ANI/CJ 各语言绑定编译 + 兼容性评估 |
| 修改配置相关（PARAM/XML） | 编译通过 + 配置加载单测 |

### 调试命令

```sh
hilog -T ArkWeb              # WebView 日志
hilog -T webadapter          # 适配层日志
param get web.*              # 查看运行时 PARAM 配置
cat /etc/web/web_config.xml  # 查看 XML 配置
ps -A | grep arkweb          # 检查进程
hidumper -s 8610 -a --memory # SA 内存分析
```

崩溃文件位置：`/data/log/crash/`，由 `arkweb_crashpad_handler` 生成。

### 完成标准

变更完成后应确认：
1. ✅ 编译无错误无新增警告
2. ✅ 相关单测通过
3. ✅ 如涉及接口变更，`prepare.sh` 已重新执行
4. ✅ 如涉及 Public API，已评估兼容性影响
5. ✅ 提交带 DCO 签名（`git commit -s`）

如果无法在本地运行构建（如无 OH 源码环境），应在 PR 描述中明确标注验证范围和限制。
