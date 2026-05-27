# 如何添加胶水层接口

本指南说明 ArkWeb 胶水层（`ohos_interface`）的双向接口规则。重点是方向判断、代码生成和约束，不重复 C++ 语法示例。

## 核心概念

胶水层定义在 `ohos_interface/include/`，通过纯虚接口将系统侧代码与 Chromium 引擎隔离。

| 方向 | 含义 | 调用方 | 是否允许新增接口文件 | 文件命名 |
|---|---|---|---|---|
| `ohos_nweb` | 系统侧 → ArkWebCore | `ohos_nweb/` | 允许 | `nweb_*.h` |
| `ohos_adapter` | ArkWebCore → 系统侧 | ArkWebCore.hap | 不允许 | `*_adapter.h` |

重要约束：`ohos_adapter` 方向接口不允许新增，只能在现有接口中扩展方法。这是版本兼容性要求。

## 代码生成架构

胶水层桥接代码由 `prepare.sh` 生成，不要手改生成物。

```text
ohos_interface/include/
        ↓ prepare.sh translate
ohos_glue/
        ↓
bridge/   系统侧桥接
cpptoc/   C++ 接口包装为 C 结构体
ctocpp/   C 结构体回调解包为 C++ 接口
```

涉及 bridge、cpptoc、ctocpp 时，先找到源接口定义和生成规则，再修改 BUILD 输出声明。

## 添加 nweb 方向接口

适用于为应用层提供新的 WebView 能力。

1. 在 `ohos_interface/include/ohos_nweb/` 下新增接口头文件。
2. 文件名使用 `nweb_<feature>.h`，类名使用 `NWeb<Feature>`，导出宏使用 `NWEB_EXPORT`。
3. 需要从主 `NWeb` 对象暴露能力时，在 `ohos_interface/include/ohos_nweb/nweb.h` 中添加获取方法。
4. 更新 `ohos_interface/BUILD.gn` 中对应 prepare target 的输出声明。
5. 在 `ohos_nweb/src/` 或对应模块实现接口。
6. 按需添加 NAPI、ANI、CJ 或 Native 绑定。
7. 添加必要测试。

参考现有接口文件时，优先看同类 `nweb_*.h` 和对应 `ohos_nweb/src/*_impl.*`。

## 扩展 adapter 方向接口

适用于 ArkWebCore 调用 OpenHarmony 系统服务。

1. 只能在已有 `ohos_interface/include/ohos_adapter/*_adapter.h` 中添加方法。
2. 不要新增 adapter 接口文件。
3. 在 `ohos_adapter/` 对应实现类中补实现。
4. 同步更新测试 mock，避免单元测试依赖真实系统服务。
5. 更新 `ohos_glue/BUILD.gn` 中 adapter 方向 prepare target 的输出声明。

如果需要全新的系统能力，先确认是否能归入已有 adapter；确实无法归入时，需要先评估接口兼容策略，而不是直接新增文件。

## 命名速查

| 场景 | 文件 | 类名 | 桥接命名 |
|---|---|---|---|
| nweb 新接口 | `nweb_<feature>.h` | `NWeb<Feature>` | `ArkWeb<Feature>Wrapper` |
| adapter 扩展 | `*_adapter.h` | `*Adapter` | `Ark*AdapterImpl` |
| nweb 回调 | `nweb_*_callback.h` | `NWeb*Callback` | `ArkWeb*CallbackImpl` / `ArkWeb*CallbackCpptoc` |

## 检查清单

- [ ] 已确认方向：`ohos_nweb` 还是 `ohos_adapter`。
- [ ] `ohos_adapter` 方向没有新增接口文件。
- [ ] 接口类只定义行为，不放数据成员。
- [ ] nweb 方向头文件使用 `NWEB_EXPORT`。
- [ ] 实现类放在 `ohos_nweb/src/` 或 `ohos_adapter/` 对应位置。
- [ ] 相关 BUILD prepare 输出声明已更新。
- [ ] Mock 实现已同步。
- [ ] 需要多语言 API 时，NAPI/ANI/CJ/Native 绑定已补齐。
- [ ] 提交使用 `git commit -s`。

## 相关文档

- [HOW_TO_ADD_PARAM_CONFIG.md](HOW_TO_ADD_PARAM_CONFIG.md)
- [HOW_TO_ADD_XML_CONFIG.md](HOW_TO_ADD_XML_CONFIG.md)
- [HOW_TO_ADD_BUILD_FEATURE.md](HOW_TO_ADD_BUILD_FEATURE.md)
