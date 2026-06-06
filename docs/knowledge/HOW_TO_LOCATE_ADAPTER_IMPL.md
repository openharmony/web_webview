# 如何定位适配器的真实实现

ArkWeb 通过适配器对接系统能力。每个适配器只有一份运行时生效的实现，位于本仓库 `ohos_adapter/` 或 chromium_arkweb 仓库 `ohos_adapter_ndk/` 之一。具体走哪条路径，由 chromium_arkweb 仓库中的分派决策文件决定。

## 前提：理解两套实现体系

| | 本仓库 `ohos_adapter/` (Ark 侧) | chromium_arkweb 仓库 `ohos_adapter_ndk/` (NDK 侧) |
|-|----------------------------------|---------------------------------------------|
| API 级别 | OpenHarmony 内部 C++ SDK | OpenHarmony 公开 C NDK API (`OH_*`) |
| 编译产物 | `libnweb_ohos_adapter.so` | 编入 `libarkweb_engine.so` |
| 调用方式 | 经 CAPI 桥接跨 .so | 进程内直接调用 |
| 本仓库视角 | **实现在本仓库** | **实现不在本仓库** |

## 定位规则（三步判定法）

**Step 1** — 打开 chromium_arkweb 仓库中的分派决策文件：

```
<project_root>/src/arkweb/ohos_adapter_ndk/ohos_adapter_helper_decouple.cpp
```

**Step 2** — 搜索目标适配器的工厂方法（如 `CreateXxxAdapter`、`GetXxxInstance`）

**Step 3** — 根据代码模式判断真实实现位置：

```cpp
// 模式 A：直接实例化 → NDK 直连，实现不在本仓库
return std::make_unique<NWeb::XxxAdapterImpl>();
return XxxAdapterImpl::GetInstance();
// → 真实代码在 chromium_arkweb 仓库 src/arkweb/ohos_adapter_ndk/<模块名>/
// → 本仓库 ohos_adapter/ 下的同名文件不参与运行时

// 模式 B：经 ctocpp_ 桥接 → Ark 桥接，实现在本仓库
ArkWebRefPtr<ArkXxxAdapter> adapter = ctocpp_->CreateXxxAdapter();
return std::make_unique<ArkXxxAdapterWrapper>(adapter);
// → 真实代码在本仓库 ohos_adapter/<模块名>/src/

// 模式 C：返回 nullptr → 当前未实现
return nullptr;
```

## 迁移方向

整体趋势是从模式 B（Ark 桥接）向模式 A（NDK 直连）迁移。当一个适配器完成迁移后：
- chromium_arkweb 仓库 `ohos_adapter_ndk/` 下新增 NDK 实现
- `ohos_adapter_helper_decouple.cpp` 工厂方法从 `ctocpp_` 桥接改为直接实例化
- `build/prepare.sh` 中添加 `sed` 命令排除旧胶水层 wrapper
- 本仓库中的同名适配器文件保留但不再被运行时调用
