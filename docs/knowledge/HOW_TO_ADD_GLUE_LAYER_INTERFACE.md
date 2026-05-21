# 如何添加胶水层接口

本指南说明如何在 ArkWeb 胶水层（ohos_interface）中添加新接口，包括 nweb 方向（系统侧 → 内核）和 adapter 方向（内核 → 系统侧）。

## 概述

### 什么是胶水层？

胶水层是 ArkWeb 实现双向解耦的核心机制，定义在 `ohos_interface/include/` 下，通过纯虚接口（C++ abstract class）将系统侧代码与 Chromium 引擎隔离。

```
ohos_interface/include/
├── ohos_nweb/       ← 系统侧 → 内核方向（68 个接口）
└── ohos_adapter/    ← 内核 → 系统侧方向（53 个接口）
```

### 两个方向的区别

| | ohos_nweb 方向 | ohos_adapter 方向 |
|---|---|---|
| **数据流向** | 系统侧 → 内核 | 内核 → 系统侧 |
| **谁调用** | ohos_nweb/ 调用 | ArkWebCore.hap 调用 |
| **接口数量** | 68+ | 53+ |
| **允许新增** | ✅ 是 | ❌ 否 |
| **文件命名** | `nweb_*.h` | `*_adapter.h` |

> ⚠️ **重要约束**：ohos_adapter 方向接口不允许新增，只能在现有接口中扩展。这是版本兼容性要求。

## 代码生成架构

胶水层代码不是手写的，而是通过 `prepare.sh` 脚本自动生成桥接代码：

```
ohos_interface/include/ohos_nweb/nweb.h     (接口定义)
         ↓ prepare.sh translate
ohos_glue/ → gen/ohos_nweb/
    ├── bridge/     ark_web_nweb_wrapper.cpp/h        (系统侧桥接)
    ├── cpptoc/     ark_web_nweb_cpptoc.cpp/h         (C++ → C 回调包装)
    └── ctocpp/     ark_web_nweb_ctocpp.cpp/h         (C 回调 → C++ 解包)
```

### 三层桥接

| 层 | 目录 | 作用 |
|----|------|------|
| **bridge** | `bridge/` | 系统侧实现，将 nweb 接口调用转发到 Chromium 引擎 |
| **cpptoc** | `cpptoc/` | 将 C++ 回调/接口封装为 C 结构体，供 Chromium 侧调用 |
| **ctocpp** | `ctocpp/` | 将 C 结构体回调解包为 C++ 接口调用，供系统侧使用 |

### 输出库

| 库 | 输出名 | 说明 |
|----|--------|------|
| `ohos_base_glue_source` | `arkweb_glue_base` | 基础类型桥接 |
| `ohos_nweb_glue_source` | `arkweb_core_loader_glue` | nweb 方向桥接 |
| `ohos_adapter_glue_source` | — | adapter 方向桥接 |

## 添加 nweb 方向接口

适用于：为应用层提供新的 WebView 功能 API。

### 步骤 1：定义接口类

在 `ohos_interface/include/ohos_nweb/` 下新建头文件：

```cpp
// ohos_interface/include/ohos_nweb/nweb_new_feature.h
#ifndef NWEB_NEW_FEATURE_H
#define NWEB_NEW_FEATURE_H

#include "nweb_export.h"

namespace OHOS::NWeb {

class NWEB_EXPORT NWebNewFeature {
public:
    NWebNewFeature() = default;
    virtual ~NWebNewFeature() = default;

    // 纯虚接口定义
    virtual void DoSomething(const std::string& param) = 0;
    virtual std::string GetResult() = 0;
};

} // namespace OHOS::NWeb

#endif
```

**命名规范**：
- 文件名：`nweb_<feature_name>.h`
- 类名：`NWeb<FeatureName>`（PascalCase）
- 导出宏：`NWEB_EXPORT`

### 步骤 2：在 NWeb 类中暴露接口

修改 `ohos_interface/include/ohos_nweb/nweb.h`，在 `NWeb` 类中添加获取新接口的方法：

```cpp
#include "nweb_new_feature.h"

class NWEB_EXPORT NWeb {
public:
    // ... 已有方法 ...

    // 新增：获取新功能接口
    virtual NWebNewFeature* GetNewFeature() = 0;
};
```

### 步骤 3：更新构建配置

修改 `ohos_interface/BUILD.gn`，在 `webview_glue_nweb_prepare` 中添加新接口文件的输出声明。

### 步骤 4：实现接口

在 `ohos_nweb/src/` 下新建实现类：

```cpp
// ohos_nweb/src/nweb_new_feature_impl.h
#include "nweb_new_feature.h"

namespace OHOS::NWeb {
class NWebNewFeatureImpl : public NWebNewFeature {
public:
    void DoSomething(const std::string& param) override;
    std::string GetResult() override;
};
}
```

### 步骤 5：多语言绑定

根据目标语言添加对应的绑定层：

| 语言 | 路径 | 参考 |
|------|------|------|
| ArkTS (NAPI) | `interfaces/kits/napi/` | [interfaces/kits/napi/README.md](../../interfaces/kits/napi/README.md) |
| C/C++ (Native) | `interfaces/native/` | [interfaces/native/README.md](../../interfaces/native/README.md) |
| ArkTS Static (ANI) | `interfaces/kits/ani/` | [interfaces/kits/ani/README.md](../../interfaces/kits/ani/README.md) |
| CJ FFI | `interfaces/kits/cj/` | [interfaces/kits/cj/README.md](../../interfaces/kits/cj/README.md) |

### 步骤 6：构建和测试

```bash
# 构建
./build.sh --product-name rk3568 --build-target webview

# 单元测试
./build.sh --product-name rk3568 --build-target //base/web/webview/test/unittest/<module>:unittest
```

## 扩展 adapter 方向接口

> ⚠️ **不允许新增接口文件**，只能在已有接口中扩展方法。

### 步骤 1：在现有接口中添加方法

例如在 `ohos_interface/include/ohos_adapter/graphic_adapter.h` 中添加新方法：

```cpp
class GraphicAdapter {
public:
    // 已有方法...
    virtual void ExistingMethod() = 0;

    // 新增方法（必须保持纯虚，有默认行为的提供空实现）
    virtual void NewMethod(int param) = 0;
};
```

### 步骤 2：实现新方法

在 `ohos_adapter/` 对应的实现类中添加具体实现：

```cpp
// ohos_adapter/graphic_adapter_impl.cpp
void GraphicAdapterImpl::NewMethod(int param) {
    // 调用 OpenHarmony 系统服务
}
```

### 步骤 3：更新 Mock 实现

在测试 mock 中同步添加空实现：

```cpp
// test/unittest/common/mock/graphic_adapter_mock.h
MOCK_METHOD1(NewMethod, void(int));
```

### 步骤 4：更新构建配置

adapter 方向的桥接代码也是自动生成的，需要在 `ohos_glue/BUILD.gn` 的 `ohos_glue_adapter_prepare` outputs 中添加对应文件。

## 命名速查

| 场景 | 文件 | 类名 | 桥接类名 |
|------|------|------|----------|
| nweb 新接口 | `nweb_<feature>.h` | `NWeb<Feature>` | `ArkWeb<Feature>Wrapper` (bridge) |
| adapter 扩展 | `*_adapter.h` | `*Adapter` | `Ark*AdapterImpl` (bridge) |
| nweb 回调 | `nweb_*_callback.h` | `NWeb*Callback` | `ArkWeb*CallbackImpl` (bridge) + `ArkWeb*CallbackCpptoc` (cpptoc) |

## 检查清单

添加新接口前确认：

- [ ] 确认方向：nweb（系统→内核）还是 adapter（内核→系统）
- [ ] adapter 方向：**不允许新增接口文件**，只能扩展现有接口
- [ ] 接口类使用纯虚函数，不含数据成员
- [ ] 头文件包含 `NWEB_EXPORT` 宏（nweb）或无导出宏（adapter）
- [ ] 实现类放在 `ohos_nweb/src/` 或 `ohos_adapter/` 对应位置
- [ ] 更新 `BUILD.gn` 构建配置
- [ ] 同步更新 Mock 实现（`test/unittest/common/`）
- [ ] 添加对应语言的绑定（NAPI/ANI/CJ/Native）
- [ ] `git commit -s` 签名提交

## 相关文档

- [ohos_nweb/README.md](../../ohos_nweb/README.md) — 核心引擎层文档
- [ohos_adapter/README.md](../../ohos_adapter/README.md) — 系统适配层文档
- [HOW_TO_ADD_PARAM_CONFIG.md](HOW_TO_ADD_PARAM_CONFIG.md) — PARAM 参数配置（推荐）
- [HOW_TO_ADD_XML_CONFIG.md](HOW_TO_ADD_XML_CONFIG.md) — XML 配置
- [HOW_TO_ADD_BUILD_FEATURE.md](HOW_TO_ADD_BUILD_FEATURE.md) — 编译特性开关
- [AGENTS.md](../../AGENTS.md) — 代码仓指引
