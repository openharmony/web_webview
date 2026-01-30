# 如何添加编译特性开关

本指南说明如何在ArkWeb项目中添加新的编译特性开关,用于控制可选功能的编译和运行时行为。

## 概述

### 什么是编译特性开关?

编译特性开关是 GN (Generate Ninja) 构建系统中的布尔变量,用于控制:

- **可选功能的编译** - 根据系统组件依赖自动启用/禁用功能
- **代码条件编译** - 通过 `#ifdef` 宏控制代码路径
- **依赖项链接** - 仅在启用时链接相关系统库
- **测试目标构建** - 仅在启用时构建相关测试

### 设计模式

```
config.gni 定义开关
    ↓
根据 global_parts_info 自动检测依赖
    ↓
BUILD.gn 使用开关控制编译
    ├─ 条件添加源文件 (真实现 / Mock实现)
    ├─ 条件添加依赖 (external_deps)
    └─ 条件定义宏 (defines)
    ↓
C++ 代码使用宏控制代码路径
    ↓
最终构建产物包含/不包含该功能
```

## 现有特性开关列表

| 开关名称 | 默认值 | 依赖的系统组件 | 功能说明 |
|---------|--------|--------------|----------|
| `webview_soc_perf_enable` | true | resourceschedule_soc_perf | SoC 性能优化 |
| `webview_audio_enable` | true | multimedia_audio_framework, multimedia_av_session | 音频播放和录制 |
| `webview_location_enable` | true | location_location | 地理位置服务 |
| `webview_media_player_enable` | true | multimedia_player_framework | 媒体播放器 |
| `webview_camera_enable` | true | multimedia_camera_framework, hdf_drivers_interface_camera | 相机访问 |
| `webview_telephony_enable` | true | telephony_cellular_data, telephony_core_service | 电话服务 |
| `webview_battery_manager_enable` | true | powermgr_battery_manager | 电池管理 |
| `webview_power_manager_enable` | true | powermgr_power_manager | 电源管理 |
| `webview_avcodec_enable` | true | multimedia_av_codec | 音视频编解码 |
| `webview_print_enable` | true | print_print_fwk | 打印功能 |
| `webview_enterprise_device_manager_enable` | true | customization_enterprise_device_management | 企业设备管理 |
| `webview_media_avsession_enable` | true | multimedia_av_session | 音视频会话管理 |
| `webview_sensors_sensor_enable` | true | sensors_sensor | 传感器访问 |
| `webview_enable_heif_decoder` | false | hdf_drivers_interface_display | HEIF 图像解码 |
| `webview_drm_enable` | true | multimedia_drm_framework | DRM 版权保护 |
| `webview_preload_render_lib` | true | - | 预加载渲染库 |

## 如何添加新的特性开关

### 步骤 1: 在 config.gni 中定义开关

**位置**: `config.gni`

```gni
declare_args() {
  # 1. 定义开关变量,默认设置为 true
  webview_bluetooth_enable = true

  # 2. 根据 global_parts_info 自动检测依赖
  # 如果系统不包含蓝牙组件,自动禁用该功能
  if (defined(global_parts_info) &&
      !defined(global_parts_info.communication_bluetooth)) {
    webview_bluetooth_enable = false
  }
}
```

**命名规范**:
- 格式: `webview_<功能>_enable`
- 全小写,使用下划线分隔
- 描述性名称,清晰表达功能

**自动检测逻辑**:
- 默认值设为 `true` (假设大多数产品包含该功能)
- 检查 `global_parts_info` 中是否存在对应组件
- 如果不存在,自动设置为 `false`

### 步骤 2: 在 BUILD.gn 中使用开关

#### 2.1 条件编译源文件

**位置**: `ohos_adapter/BUILD.gn` (或相关适配器的 BUILD.gn)

```gni
ohos_shared_library("nweb_ohos_adapter") {
  sources = [
    # ... 通用源文件
  ]

  # 条件添加蓝牙适配器实现
  if (webview_bluetooth_enable) {
    sources += [
      "bluetooth_adapter/src/bluetooth_adapter_impl.cpp",
      "bluetooth_adapter/src/bluetooth_device_adapter_impl.cpp",
      "bluetooth_adapter/src/bluetooth_socket_adapter_impl.cpp",
    ]
    external_deps += [
      "bluetooth:bluetooth_framework",
    ]
    defines += [ "NWEB_BLUETOOTH_ENABLE" ]
  } else {
    # 使用 Mock 实现替代,避免链接失败
    sources += [
      "bluetooth_adapter/src/bluetooth_adapter_mock.cpp",
      "bluetooth_adapter/src/bluetooth_device_adapter_mock.cpp",
      "bluetooth_adapter/src/bluetooth_socket_adapter_mock.cpp",
    ]
  }
}
```

**关键点**:
- **真实现**: 当功能启用时,使用实际的系统 API
- **Mock 实现**: 当功能禁用时,使用空实现或返回默认值
- **依赖库**: 仅在启用时添加系统库依赖
- **宏定义**: 定义 `NWEB_<功能>_ENABLE` 宏供 C++ 代码使用

#### 2.2 在测试中使用开关

**位置**: `test/unittest/<适配器名>_test/BUILD.gn`

```gni
import("../../../config.gni")

config("module_private_config") {
  if (webview_bluetooth_enable) {
    defines = [ "NWEB_BLUETOOTH_ENABLE" ]
  }
}

ohos_unittest("nweb_bluetooth_adapter_test") {
  sources = [ "bluetooth_adapter_impl_test.cpp" ]

  configs = [ ":module_private_config" ]

  deps = [ "$webview_path/ohos_adapter:nweb_ohos_adapter" ]

  external_deps = [
    "c_utils:utils",
    "googletest:gtest_main",
    "hilog:libhilog",
  ]

  # 仅在功能启用时添加蓝牙依赖
  if (webview_bluetooth_enable) {
    external_deps += [
      "bluetooth:bluetooth_framework",
    ]
  }
}

# 条件添加测试目标
group("unittest") {
  testonly = true
  deps = []
  if (webview_bluetooth_enable) {
    deps += [ ":nweb_bluetooth_adapter_test" ]
  }
}
```

### 步骤 3: 在 C++ 代码中使用宏

#### 3.1 头文件中条件编译

**位置**: `ohos_adapter/bluetooth_adapter/include/bluetooth_adapter.h`

```cpp
#ifndef OHOS_ADAPTER_BLUETOOTH_ADAPTER_H
#define OHOS_ADAPTER_BLUETOOTH_ADAPTER_H

#include "refbase.h"

namespace OHOS::NWeb {

class BluetoothAdapter : public RefBase {
public:
    // 通用方法 (所有平台都支持)
    virtual bool IsAvailable() = 0;

#ifdef NWEB_BLUETOOTH_ENABLE
    // 仅在启用时编译的 API
    virtual void EnableBluetooth() = 0;
    virtual void DisableBluetooth() = 0;
    virtual bool IsEnabled() const = 0;
    virtual std::vector<std::string> GetConnectedDevices() = 0;
#endif

    virtual ~BluetoothAdapter() = default;
};

} // namespace OHOS::NWeb
#endif // OHOS_ADAPTER_BLUETOOTH_ADAPTER_H
```

#### 3.2 实现文件中提供实现

**真实现** (当功能启用时):
**位置**: `ohos_adapter/bluetooth_adapter/src/bluetooth_adapter_impl.cpp`

```cpp
#include "bluetooth_adapter.h"
#include "bluetooth_standard.h" // 系统蓝牙 API

namespace OHOS::NWeb {

class BluetoothAdapterImpl : public BluetoothAdapter {
public:
    bool IsAvailable() override {
#ifdef NWEB_BLUETOOTH_ENABLE
        return bluetoothSystemManager != nullptr;
#else
        return false; // Mock 实现始终返回不可用
#endif
    }

#ifdef NWEB_BLUETOOTH_ENABLE
    void EnableBluetooth() override {
        if (bluetoothSystemManager) {
            bluetoothSystemManager->Enable();
        }
    }

    void DisableBluetooth() override {
        if (bluetoothSystemManager) {
            bluetoothSystemManager->Disable();
        }
    }

    bool IsEnabled() const override {
        return bluetoothSystemManager ?
               bluetoothSystemManager->IsEnabled() : false;
    }

    std::vector<std::string> GetConnectedDevices() override {
        // 调用系统蓝牙 API 获取设备列表
        // ...
    }
#endif

private:
#ifdef NWEB_BLUETOOTH_ENABLE
    std::shared_ptr<BluetoothSystemManager> bluetoothSystemManager_;
#endif
};

} // namespace OHOS::NWeb
```

**Mock 实现** (当功能禁用时):
**位置**: `ohos_adapter/bluetooth_adapter/src/bluetooth_adapter_mock.cpp`

```cpp
#include "bluetooth_adapter.h"

namespace OHOS::NWeb {

// Mock 实现 - 所有方法返回默认值
class BluetoothAdapterMock : public BluetoothAdapter {
public:
    bool IsAvailable() override {
        return false; // Mock 始终返回不可用
    }

    // 其他方法不需要实现,因为头文件中已用 #ifdef 保护
};

} // namespace OHOS::NWeb
```

#### 3.3 在核心引擎中使用

**位置**: `ohos_nweb/src/nweb_bluetooth_manager.cpp`

```cpp
#include "ohos_adapter/bluetooth_adapter.h"

void NWebBluetoothManager::Initialize()
{
    auto bluetoothAdapter = BluetoothAdapterManager::GetInstance().GetAdapter();

    // 检查蓝牙功能是否可用
    if (!bluetoothAdapter || !bluetoothAdapter->IsAvailable()) {
        WVLOG_I("Bluetooth is not available on this device");
        return;
    }

#ifdef NWEB_BLUETOOTH_ENABLE
    // 仅在功能启用时执行
    if (bluetoothAdapter->IsEnabled()) {
        auto devices = bluetoothAdapter->GetConnectedDevices();
        // 处理已连接设备
    }
#else
    WVLOG_W("Bluetooth feature is disabled in build");
#endif
}
```

### 步骤 4: 在 ohos_wrapper 中使用 (可选)

如果需要在 wrapper 层暴露该功能:

**位置**: `ohos_wrapper/BUILD.gn`

```gni
import("../config.gni")

ohos_shared_library("nweb_ohos_wrapper") {
  sources = []

  if (webview_bluetooth_enable) {
    sources += [ "src/nweb_bluetooth_wrapper.cpp" ]
    external_deps += [
      "bluetooth:bluetooth_framework",
    ]
  }
}
```

### 步骤 5: 验证和测试

#### 5.1 编译测试

```bash
# 1. 功能启用时编译 (包含蓝牙组件)
./build.sh --product-name rk3568 --build-target //base/web/webview:webview

# 检查编译日志,确认蓝牙相关代码被编译
grep -i bluetooth build.log

# 2. 功能禁用时编译 (不包含蓝牙组件)
# 方法 1: 修改产品配置移除蓝牙组件
# 方法 2: 临时强制禁用
# 在 vendor/<product>/config.json 中设置:
# {
#   "webview_bluetooth_enable": false
# }

# 验证 Mock 实现被编译
grep -i mock build.log
```

#### 5.3 运行时验证

```bash
# 1. 安装到设备
hdc install ArkWebCore.hap

# 2. 查看日志确认功能状态
hilog -T ArkWeb | grep -i bluetooth

# 预期输出 (启用):
# I ArkWeb: Bluetooth adapter initialized
# I ArkWeb: Bluetooth is available

# 预期输出 (禁用):
# W ArkWeb: Bluetooth is not available on this device
```

#### 5.4 单元测试

```bash
# 运行适配器测试
./runtest --part-name webview --test-case nweb_bluetooth_adapter_test

# 测试应覆盖:
# 1. 功能启用时的正常流程
# 2. 功能禁用时的 Mock 行为
# 3. 边界条件 (设备不支持蓝牙等)
```

## 高级用法

### 1. 功能互斥

某些功能可能互斥,不能同时启用:

```gni
declare_args() {
  webview_gpu_renderer_a = true
  webview_gpu_renderer_b = false

  # 确保 renderer_b 和 renderer_a 不同时启用
  assert(!webview_gpu_renderer_a || !webview_gpu_renderer_b,
         "GPU renderer A and B cannot be enabled at the same time")
}
```

### 2. 功能依赖

某些功能依赖其他功能:

```gni
declare_args() {
  webview_advanced_audio_enable = false

  # 高级音频功能依赖基础音频功能
  if (webview_advanced_audio_enable && !webview_audio_enable) {
    webview_advanced_audio_enable = false
  }
}
```

### 3. 平台相关

某些功能仅特定平台支持:

```gni
declare_args() {
  webview_custom_feature = false

  # 仅在特定产品上启用
  if (target_product == "custom_device") {
    webview_custom_feature = true
  }
}
```

### 4. 组合条件

多个条件的组合判断:

```gni
# 在 BUILD.gn 中
if (webview_bluetooth_enable && webview_audio_enable) {
  # 蓝牙音频相关功能
  sources += [
    "bluetooth_audio_adapter/src/impl.cpp",
  ]
  defines += [
    "NWEB_BLUETOOTH_AUDIO_ENABLE",
  ]
}
```

## 最佳实践

### ✅ 推荐做法

1. **提供 Mock 实现**
   - 始终为禁用功能提供 Mock 实现
   - Mock 实现应返回安全的默认值
   - 避免空指针或链接错误

2. **清晰的命名**
   - 使用描述性的功能名称
   - 遵循命名规范 `webview_<功能>_enable`
   - 宏定义使用 `NWEB_<功能>_ENABLE`

3. **自动检测依赖**
   - 利用 `global_parts_info` 自动检测
   - 减少手动配置,避免错误

4. **完整的测试覆盖**
   - 为启用和禁用状态都编写测试
   - 测试 Mock 实现的行为
   - 测试功能边界条件

5. **文档更新**
   - 在相关 README.md 中说明新功能
   - 更新 AGENT.md 中的开关列表
   - 添加使用示例

### ❌ 避免做法

1. **硬编码平台判断**
   ```gni
   # ❌ 不推荐
   if (target_cpu == "arm64") {
     webview_feature_enable = true
   }
   
   # ✅ 推荐
   if (defined(global_parts_info.xxx)) {
     webview_feature_enable = true
   }
   ```

2. **忘记更新测试**
   - 添加功能后忘记更新测试 BUILD.gn
   - 导致功能禁用时测试失败

3. **Mock 实现崩溃**
   ```cpp
   // ❌ 不推荐: 可能崩溃
   void* GetAdapter() {
       return nullptr; // 调用者未检查空指针
   }
   
   // ✅ 推荐: 返回安全的空实现
   std::shared_ptr<Adapter> GetAdapter() {
       return std::make_shared<MockAdapter>();
   }
   ```

4. **过度使用条件编译**
   - 仅在必要时使用 `#ifdef`
   - 考虑使用运行时检测替代

## 调试技巧

### 1. 查看开关当前值

```bash
# 查看构建时的 GN 变量值
gn args out/rk3568 --list=webview_*_enable

# 或查看生成的 build.ninja
grep "webview_bluetooth_enable" out/rk3568/build.ninja
```

### 2. 追踪宏定义

```bash
# 查看编译命令中的宏定义
grep -r "NWEB_BLUETOOTH_ENABLE" .out/

# 在代码中打印
#ifdef NWEB_BLUETOOTH_ENABLE
#warning "Bluetooth is enabled"
#endif
```

### 3. 强制启用/禁用

临时测试时可以覆盖自动检测:

```bash
# 在命令行指定参数
./build.sh --product-name rk3568 \
  --build-target //base/web/webview:webview \
  --gn-args="webview_bluetooth_enable=true"
```

## 常见问题

### Q1: 为什么编译时找不到系统头文件?

**原因**: 功能开关启用,但产品配置中不包含对应组件。

**解决**:
1. 检查 `vendor/<product>/config.json` 是否包含该组件
2. 或临时禁用该功能开关
3. 确保 Mock 实现存在

### Q2: Mock 实现应该返回什么?

**原则**: 返回安全的默认值,不影响核心功能。

**示例**:
```cpp
// 蓝牙适配器 Mock
bool IsAvailable() { return false; }  // 不可用
std::vector<std::string> GetDevices() { return {}; }  // 空列表
void Enable() {}  // 空操作
```

### Q3: 如何处理多个相关的开关?

**方法**: 使用组合条件或依赖关系。

```gni
# 方法 1: 组合条件
if (webview_audio_enable && webview_bluetooth_enable) {
  # 蓝牙音频功能
}

# 方法 2: 依赖关系
declare_args() {
  webview_bluetooth_audio_enable = false
  # 蓝牙音频依赖基础音频和蓝牙
  if (webview_audio_enable && webview_bluetooth_enable) {
    webview_bluetooth_audio_enable = true
  }
}
```

### Q4: 能否运行时检测而不是编译时?

**答案**: 可以,但编译时开关更高效。

**编译时开关优势**:
- 减小二进制体积
- 避免运行时检测开销
- 编译时保证依赖可用

**运行时检测适用**:
- 功能可能动态安装/卸载
- 需要向后兼容旧版本
- 不确定系统是否支持

## 相关文档

- **[config.gni](./config.gni)** - 所有特性开关定义
- **[ohos_adapter/README.md](./ohos_adapter/README.md)** - 适配器层详细文档
- **[ohos_wrapper/README.md](./ohos_wrapper/README.md)** - 封装层文档
- **[AGENT.md](./AGENT.md)** - 项目概述和开发指南
- **[BUILD.gn 语法](https://gn.googlesource.com/gn/+/main/docs/reference.md)** - GN 构建系统文档

## 示例总结

添加蓝牙功能支持的完整示例:

```gni
# config.gni
declare_args() {
  webview_bluetooth_enable = true
  if (defined(global_parts_info) &&
      !defined(global_parts_info.communication_bluetooth)) {
    webview_bluetooth_enable = false
  }
}
```

```gni
# ohos_adapter/BUILD.gn
if (webview_bluetooth_enable) {
  sources += [ "bluetooth_adapter/src/impl.cpp" ]
  external_deps += [ "bluetooth:bluetooth_framework" ]
  defines += [ "NWEB_BLUETOOTH_ENABLE" ]
} else {
  sources += [ "bluetooth_adapter/src/mock.cpp" ]
}
```

```cpp
// ohos_adapter/bluetooth_adapter/include/bluetooth_adapter.h
class BluetoothAdapter {
public:
    virtual bool IsAvailable() = 0;
#ifdef NWEB_BLUETOOTH_ENABLE
    virtual void Enable() = 0;
#endif
};
```

这样的设计确保:
- ✅ 编译时根据产品配置自动包含/排除功能
- ✅ 不影响其他功能的正常使用
- ✅ Mock 实现保证不包含该功能的产品也能编译通过
- ✅ 清晰的代码结构和命名规范
