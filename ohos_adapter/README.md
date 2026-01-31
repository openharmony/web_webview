# ohos_adapter

## 概述

`ohos_adapter`是 ArkWeb WebView 组件的**OpenHarmony 系统适配层**，提供了一套统一的接口来访问 OpenHarmony 的各种系统服务。该层是 WebView 引擎与 OpenHarmony 系统之间的桥梁，实现了引擎与系统服务的解耦。

**核心库**: `libnweb_ohos_adapter.so`
**设计模式**: 适配器模式 (Adapter Pattern)
**API 标签**: `innerapi_tags: [ "platformsdk" ]`

### 核心职责

1. **接口统一** - 将 OpenHarmony 各种子系统的 API 封装为统一接口
2. **依赖解耦** - 隔离 WebView 引擎与具体系统实现的依赖
3. **可测试性** - 支持 Mock 实现，便于单元测试
4. **按需编译** - 支持功能裁剪，减少不必要的依赖

## 目录结构

```
ohos_adapter/
├── BUILD.gn                          # 主构建配置
├── interfaces/                       # 适配器接口定义目录
│
├── graphic_adapter/                  # 图形显示适配器
│   ├── include/
│   │   ├── surface_adapter_impl.h      # Surface 适配
│   │   ├── vsync_adapter_impl.h        # 垂直同步适配
│   │   ├── native_image_adapter_impl.h # 原生图像适配
│   │   └── window_adapter_impl.h       # 窗口适配
│   └── src/
│
├── hiviewdfx_adapter/                # DFX (Design for X) 适配器
│   ├── include/
│   │   ├── hisysevent_adapter_impl.h  # 系统事件适配
│   │   ├── hitrace_adapter_impl.h     # 链路追踪适配
│   │   └── hilog_adapter.cpp          # 日志适配
│   └── src/
│
├── multimodalinput_adapter/          # 多模态输入适配器
│   └── include/mmi_adapter_impl.h     # MMI 适配
│
├── power_mgr_adapter/                # 电源管理适配器
│   └── include/power_mgr_client_adapter_impl.h
│
├── audio_adapter/                    # 音频适配器（条件编译）
│   ├── include/
│   │   ├── audio_renderer_adapter_impl.h
│   │   ├── audio_capturer_adapter_impl.h
│   │   ├── audio_system_manager_adapter_impl.h
│   │   └── audio_device_desc_adapter_impl.h
│   └── src/
│
├── location_adapter/                 # 位置服务适配器（条件编译）
│   ├── include/
│   │   ├── location_proxy_adapter_impl.h
│   │   ├── location_callback_adapter_impl.h
│   │   └── location_instance_impl.h
│   └── src/
│
├── camera_adapter/                   # 相机适配器（条件编译）
│   ├── include/
│   │   ├── camera_manager_adapter_impl.h
│   │   ├── format_adapter_impl.h
│   │   └── video_device_descriptor_adapter_impl.h
│   └── src/
│
├── media_adapter/                    # 媒体适配器（条件编译）
│   ├── include/
│   │   ├── media_codec_list_adapter_impl.h
│   │   ├── media_codec_decoder_adapter_impl.h
│   │   ├── codec_format_adapter_impl.h
│   │   └── ohos_buffer_adapter_impl.h
│   └── src/
│
├── sensor_adapter/                   # 传感器适配器（条件编译）
│   └── include/sensor_adapter_impl.h
│
├── pasteboard_adapter/               # 剪贴板适配器
│   └── include/pasteboard_client_adapter_impl.h
│
├── access_token_adapter/             # 访问令牌适配器
│   └── include/access_token_adapter_impl.h
│
├── inputmethodframework_adapter/     # 输入法框架适配器
│   └── include/imf_adapter_impl.h
│
├── cert_mgr_adapter/                 # 证书管理适配器
│   └── include/cert_mgr_adapter_impl.h
│
├── battery_mgr_adapter/              # 电池管理适配器（条件编译）
│   └── include/battery_mgr_client_adapter_impl.h
│
├── screenlock_mgr_adapter/           # 屏幕锁定管理适配器
│   └── include/screenlock_manager_adapter_impl.h
│
├── print_manager_adapter/            # 打印管理适配器（条件编译）
│   └── include/print_manager_adapter_impl.h
│
├── flowbuffer_adapter/               # 流缓冲区适配器
│   └── include/flowbuffer_adapter_impl.h
│
├── drawing_text_adapter/             # 文本绘制适配器
│   └── include/ohos_drawing_text_adapter_impl.h
│
├── net_connect_adapter/              # 网络连接适配器（条件编译）
│   └── include/
│
├── net_proxy_adapter/                # 网络代理适配器
│   └── include/net_proxy_adapter_impl.h
│
├── datashare_adapter/                # 数据共享适配器
│   └── include/datashare_adapter_impl.h
│
├── distributeddatamgr_adapter/       # 分布式数据管理适配器
│   └── webdatabase/
│       ├── include/
│       │   ├── ohos_web_data_base_adapter_impl.h
│       │   └── ohos_web_permission_data_base_adapter_impl.h
│       └── src/
│
├── keystore_adapter/                 # 密钥存储适配器
│   └── include/keystore_adapter_impl.h
│
├── ohos_native_buffer_adapter/       # Native Buffer 适配器
│   └── include/ohos_native_buffer_adapter_impl.h
│
├── ohos_image_adapter/               # 图像解码适配器
│   └── include/ohos_image_decoder_adapter_impl.h
│
├── ohos_resource_adapter/            # 资源适配器
│   └── include/ohos_resource_adapter_impl.h
│
├── event_handler_adapter/            # 事件处理器适配器
│   └── include/event_handler_adapter_impl.h
│
├── aafwk_adapter/                    # Ability 框架适配器
│   └── include/
│       ├── aafwk_app_mgr_client_adapter_impl.h
│       ├── aafwk_browser_client_adapter_impl.h
│       ├── aafwk_browser_host_impl.h
│       └── aafwk_render_scheduler_impl.h
│
├── background_task_adapter/          # 后台任务适配器
│   └── include/background_task_impl.h
│
├── date_time_format_adapter/         # 日期时间格式化适配器
│   └── include/date_time_format_adapter_impl.h
│
├── display_manager_adapter/          # 显示管理器适配器
│   └── include/display_manager_adapter_impl.h
│
├── enterprise_device_management_adapter/  # 企业设备管理适配器（条件编译）
│   └── include/enterprise_device_management_adapter_impl.h
│
├── migration_manager_adapter/        # 迁移管理适配器
│   └── include/migration_manager_adapter_impl.h
│
├── res_sched_adapter/                # 资源调度适配器
│   └── include/res_sched_client_adapter_impl.h
│
├── screen_capture_adapter/           # 屏幕捕获适配器（条件编译）
│   └── include/screen_capture_adapter_impl.h
│
├── soc_perf_adapter/                 # SoC 性能适配器（条件编译）
│   └── include/soc_perf_client_adapter_impl.h
│
├── system_properties_adapter/        # 系统属性适配器
│   └── include/system_properties_adapter_impl.h
│
├── ohos_adapter_helper/              # 适配器辅助工具
│   └── include/ohos_adapter_helper_impl.h
│
├── ohos_init_web_adapter/            # Web 初始化适配器
│   └── include/ohos_init_web_adapter_impl.h
│
└── README.md                         # 本文档
```

## 架构设计

### 适配器模式

`ohos_adapter` 采用经典的**适配器模式**：

```
┌─────────────────────────────────────────────────────┐
│              WebView 引擎 (Chromium/CEF)              │
└────────────────────┬────────────────────────────────┘
                     │ 调用适配器接口
                     ▼
┌─────────────────────────────────────────────────────┐
│              适配器接口层 (Interface Layer)           │
│  ┌──────────────┬──────────────┬──────────────┐      │
│  │ VSyncAdapter │ SurfaceAdapter│ ...Adapter   │      │
│  └──────────────┴──────────────┴──────────────┘      │
└────────────────────┬────────────────────────────────┘
                     │ 实现
                     ▼
┌─────────────────────────────────────────────────────┐
│           适配器实现层 (Impl Layer)                   │
│  ┌──────────────────┬──────────────────┐             │
│  │ VSyncAdapterImpl │ SurfaceAdapterImpl│ ...         │
│  └──────────────────┴──────────────────┘             │
└────────────────────┬────────────────────────────────┘
                     │ 调用 OpenHarmony API
                     ▼
┌─────────────────────────────────────────────────────┐
│          OpenHarmony 系统服务 (System Services)       │
│  ┌──────────┬──────────┬──────────┬──────────┐        │
│  │ Surface  │  VSync   │ Audio    │ Location │        │
│  └──────────┴──────────┴──────────┴──────────┘        │
└─────────────────────────────────────────────────────┘
```

### 设计优势

1. **解耦**: WebView 引擎不需要直接依赖 OpenHarmony 系统服务
2. **可测试**: 可以提供 Mock 实现，便于单元测试
3. **可维护**: 系统服务 API 变化时，只需修改适配器实现
4. **可移植**: 理论上可以移植到其他操作系统
5. **可裁剪**: 通过条件编译控制功能，减少不必要的依赖

### 命名规范

- **接口**: `XxxAdapter` (如 `VSyncAdapter`)
- **实现**: `XxxAdapterImpl` (如 `VSyncAdapterImpl`)
- **Mock**: `XxxAdapterMock` (用于测试)

## 核心适配器

### 1. graphic_adapter - 图形显示适配器

**功能**: 提供图形渲染相关的系统能力

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `SurfaceAdapter` | Surface 管理（生产者/消费者） | `libsurface.so` |
| `VSyncAdapter` | 垂直同步信号 | `VSyncReceiver` |
| `NativeImageAdapter` | 原生图像处理 | `libnative_image.so` |
| `WindowAdapter` | 窗口管理 | `libwm.so` |

**典型使用**:
```cpp
// 创建 Surface
auto surfaceAdapter = SurfaceAdapterImpl::Create();
surfaceAdapter->Init();

// 请求 VSync
auto vsyncAdapter = std::make_shared<VSyncAdapterImpl>();
vsyncAdapter->RequestFrame(callback);
```

### 2. hiviewdfx_adapter - DFX 适配器

**功能**: 提供日志、事件、追踪等 DFx 能力

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `HiSysEventAdapter` | 系统事件上报 | `libhisysevent.so` |
| `HiTraceAdapter` | 链路追踪 | `libhitrace_meter.so` |
| `HiLogAdapter` | 日志输出 | `libhilog.so` |

**典型使用**:
```cpp
// 上报系统事件
auto& eventAdapter = HiSysEventAdapterImpl::GetInstance();
eventAdapter.Write("WEBVIEW_ERROR", EventType::FAULT,
    std::make_tuple("errorCode", -1));

// 输出日志
HiLogAdapter::WriteLog(LOG_ERROR, "WebView load failed");
```

### 3. multimodalinput_adapter - 多模态输入适配器

**功能**: 处理各种输入设备事件

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `MMIAdapter` | 多模态输入设备管理 | `libmmi-client.so` |

**典型使用**:
```cpp
auto mmiAdapter = std::make_shared<MMIAdapterImpl>();
mmiAdapter->RegisterDevListener(callback);
```

### 4. audio_adapter - 音频适配器（条件编译）

**功能**: 提供音频播放和捕获能力

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `AudioRendererAdapter` | 音频渲染（播放） | `libaudio_client.so` |
| `AudioCapturerAdapter` | 音频捕获（录音） | `libaudio_client.so` |
| `AudioSystemManagerAdapter` | 音频系统管理 | `libaudio_client.so` |

**编译条件**: `webview_audio_enable = true`

**典型使用**:
```cpp
auto renderer = std::make_shared<AudioRendererAdapterImpl>();
renderer->CreateRenderer(AudioRendererType::PLAYBACK);
renderer->Start();
```

### 5. location_adapter - 位置服务适配器（条件编译）

**功能**: 提供地理位置服务

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `LocationProxyAdapter` | 位置服务代理 | `liblocator_sdk.so` |
| `LocationCallbackAdapter` | 位置回调 | `liblocator_sdk.so` |

**编译条件**: `webview_location_enable = true`

**典型使用**:
```cpp
auto locationAdapter = std::make_shared<LocationProxyAdapterImpl>();
locationAdapter->RequestLocationUpdate(callback);
```

### 6. camera_adapter - 相机适配器（条件编译）

**功能**: 提供相机访问能力（用于 WebRTC）

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `CameraManagerAdapter` | 相机管理器 | `camera_framework` |
| `FormatAdapter` | 相机格式 | `camera_framework` |
| `VideoDeviceDescriptorAdapter` | 视频设备描述符 | `camera_framework` |

**编译条件**: `webview_camera_enable = true`

### 7. media_adapter - 媒体适配器（条件编译）

**功能**: 提供音视频编解码能力

| 适配器 | 功能 | OpenHarmony API |
|--------|------|-----------------|
| `MediaCodecDecoderAdapter` | 媒体解码器 | `libnative_media_vdec.so` |
| `MediaCodecEncoderAdapter` | 媒体编码器 | `libnative_media_venc.so` |
| `CodecFormatAdapter` | 编解码格式 | `av_codec_client` |
| `OhosBufferAdapter` | 媒体缓冲区 | `libnative_buffer.so` |

**编译条件**: `webview_avcodec_enable = true`

### 8. 其他重要适配器

| 适配器 | 功能 | 依赖库 |
|--------|------|--------|
| `pasteboard_adapter` | 剪贴板操作 | `pasteboard_client` |
| `access_token_adapter` | 访问令牌权限校验 | `libaccesstoken_sdk` |
| `inputmethodframework_adapter` | 输入法框架 | `inputmethod_client` |
| `cert_mgr_adapter` | 证书管理 | `cert_manager_sdk` |
| `battery_mgr_adapter` | 电池状态监听 | `batterysrv_client` |
| `power_mgr_adapter` | 电源管理 | `powermgr_client` |
| `print_manager_adapter` | 打印服务 | `print_client` |
| `screenlock_mgr_adapter` | 屏幕锁定管理 | `screenlock_client` |
| `sensor_adapter` | 传感器访问 | `sensor_interface_native` |
| `datashare_adapter` | 数据共享 | `datashare_consumer` |
| `event_handler_adapter` | 事件处理器 | `libeventhandler` |
| `aafwk_adapter` | Ability 框架 | `ability_manager` |

## 构建配置

### 主构建目标

```gn
ohos_shared_library("nweb_ohos_adapter") {
  # 输出库名
  # 默认依赖所有适配器实现

  sources = [
    # 核心适配器（无条件编译）
    "graphic_adapter/src/*.cpp",
    "hiviewdfx_adapter/src/*.cpp",
    "multimodalinput_adapter/src/*.cpp",
    # ... 其他核心适配器
  ]

  # 条件编译适配器
  if (webview_audio_enable) {
    sources += [ "audio_adapter/src/*.cpp" ]
  }

  if (webview_location_enable) {
    sources += [ "location_adapter/src/*.cpp" ]
  }

  if (webview_camera_enable) {
    sources += [ "camera_adapter/src/*.cpp" ]
  }

  # ... 更多条件编译
}
```

### 条件编译选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `webview_audio_enable` | false | 启用音频功能 |
| `webview_location_enable` | false | 启用位置服务 |
| `webview_camera_enable` | false | 启用相机功能 |
| `webview_avcodec_enable` | false | 启用音视频编解码 |
| `webview_media_player_enable` | false | 启用媒体播放器 |
| `webview_soc_perf_enable` | false | 启用 SoC 性能优化 |
| `webview_telephony_enable` | false | 启用电话功能 |
| `webview_battery_manager_enable` | false | 启用电池管理 |
| `webview_power_manager_enable` | false | 启用电源管理 |
| `webview_print_enable` | false | 启用打印功能 |
| `webview_sensors_sensor_enable` | false | 启用传感器 |

### 依赖的 OpenHarmony 组件

适配器层依赖 **40+ 个** OpenHarmony 系统组件，包括：

#### 核心框架
- `ability_runtime`: Ability 运行时
- `bundle_framework`: Bundle 框架
- `eventhandler`: 事件处理器
- `ipc`: IPC 通信

#### 图形显示
- `graphic_2d`: 2D 图形库
- `graphic_surface`: Surface 管理
- `window_manager`: 窗口管理器
- `image_framework`: 图像框架

#### 媒体能力（条件编译）
- `audio_framework`: 音频框架
- `av_codec`: 音视频编解码
- `player_framework`: 播放器框架
- `camera_framework`: 相机框架
- `media_foundation`: 媒体基础库

#### 系统服务
- `hilog`: 日志系统
- `hisysevent`: 系统事件
- `hitrace`: 链路追踪
- `power_manager`: 电源管理
- `battery_manager`: 电池管理
- `sensor`: 传感器服务
- `location`: 位置服务
- `input`: 输入管理
- `pasteboard`: 剪贴板
- `print_fwk`: 打印框架
- `certificate_manager`: 证书管理
- `access_token`: 访问令牌

### 输出库

编译后生成 `libnweb_ohos_adapter.so`，标记为 `platformsdk`，可被其他模块使用。

## 工作原理

### 接口定义

适配器接口定义在 `interfaces/` 目录下，例如：

```cpp
// interfaces/graphic_adapter.h
class VSyncAdapter {
public:
    virtual ~VSyncAdapter() = default;

    virtual void RequestFrame(
        const std::function<void(int64_t, int64_t)>& callback) = 0;

    virtual void Destroy() = 0;
};
```

### 实现类

实现类继承接口并调用 OpenHarmony API：

```cpp
// graphic_adapter/include/vsync_adapter_impl.h
class VSyncAdapterImpl : public VSyncAdapter {
public:
    VSyncAdapterImpl();
    ~VSyncAdapterImpl() override;

    void RequestFrame(
        const std::function<void(int64_t, int64_t)>& callback) override;

    void Destroy() override;

private:
    sptr<VSyncReceiver> vsyncReceiver_;  // OpenHarmony VSync 接收器
};
```

### Mock 实现

用于单元测试的 Mock 实现：

```cpp
// audio_adapter/src/audio_renderer_adapter_mock.cpp
class AudioRendererAdapterMock : public AudioRendererAdapter {
public:
    int32_t CreateRenderer(AudioRendererType audioRendererType) override {
        // Mock 实现，不调用真实系统 API
        return 0;
    }
};
```

### 调用流程

```
WebView 引擎
    │
    ├─ 调用 VSyncAdapter::RequestFrame()
    │
    ├─ ↓
    │
    VSyncAdapterImpl::RequestFrame()
    │
    ├─ 调用 vsyncReceiver_->RequestNextVSync()
    │
    ├─ ↓
    │
    OpenHarmony VSync Service
    │
    ├─ 返回 VSync 信号
    │
    └─ 回调 WebView 引擎
```

## 使用场景

### 场景 1: Surface 管理

```cpp
// 1. 创建生产者 Surface
auto producerAdapter = SurfaceAdapterImpl::CreateSurface();
auto producer = std::make_shared<SurfaceAdapterImpl>(producerAdapter);

// 2. 创建消费者 Surface
auto consumerAdapter = SurfaceAdapterImpl::CreateSurface();
auto consumer = std::make_shared<SurfaceAdapterImpl>(consumerAdapter);

// 3. 连接生产者和消费者
producer->LinkToConsumer(consumer);

// 4. 请求 Buffer
auto buffer = producer->RequestBuffer(width, height);

// 5. 渲染并 Flush
producer->FlushBuffer(buffer);
```

### 场景 2: VSync 同步

```cpp
// 1. 创建 VSync 适配器
auto vsyncAdapter = std::make_shared<VSyncAdapterImpl>();

// 2. 请求 VSync 信号
vsyncAdapter->RequestFrame([](int64_t timestamp, int64_t period) {
    // 在 VSync 回调中进行渲染
    RenderFrame();
});

// 3. 渲染完成后不再需要时销毁
vsyncAdapter->Destroy();
```

### 场景 3: 系统事件上报

```cpp
// 1. 获取 HiSysEvent 适配器单例
auto& eventAdapter = HiSysEventAdapterImpl::GetInstance();

// 2. 上报错误事件
eventAdapter.Write(
    "WEBVIEW_CRASH",                                    // 事件名
    EventType::FAULT,                                   // 事件类型
    std::make_tuple(
        "errorCode", -1,                                // 错误码
        "errorMsg", "Renderer process crashed",        // 错误信息
        "bundleName", "com.example.app"                // Bundle 名称
    )
);
```

### 场景 4: 音频播放

```cpp
// 1. 创建音频渲染器
auto renderer = std::make_shared<AudioRendererAdapterImpl>();

// 2. 初始化渲染器
renderer->CreateRenderer(AudioRendererType::PLAYBACK);
renderer->SetRendererCallback(callback);

// 3. 设置音频参数
renderer->SetRendererSampleRate(48000);
renderer->SetRendererChannel(2);

// 4. 开始播放
renderer->Start();

// 5. 写入音频数据
renderer->Enqueue(buffer, size);

// 6. 停止播放
renderer->Stop();
```

## 条件编译说明

### 为什么需要条件编译？

1. **减少依赖** - 不是所有设备都需要所有功能
2. **减小体积** - 裁剪不必要的适配器，减少库体积
3. **降低内存** - 减少系统服务依赖，降低运行时内存占用
4. **安全合规** - 某些功能可能需要特殊的权限或产品形态

### 如何启用功能？

在 `config.gni` 或编译参数中设置：

```gni
# 启用音频功能
webview_audio_enable = true

# 启用位置服务
webview_location_enable = true

# 启用相机功能
webview_camera_enable = true

# 启用音视频编解码
webview_avcodec_enable = true
```

### Mock 实现的作用

当功能未启用时，使用 Mock 实现：

```cpp
// audio_adapter/src/audio_renderer_adapter_mock.cpp
class AudioRendererAdapterMock : public AudioRendererAdapter {
public:
    int32_t CreateRenderer(AudioRendererType audioRendererType) override {
        // 返回成功，但不提供真实功能
        return 0;
    }

    int32_t Start() override {
        // 不执行任何操作
        return 0;
    }
};
```

**作用**：

- 保持接口一致性
- 避免 WebView 引擎编译错误
- 运行时优雅降级（功能不可用）

## 接口示例

### SurfaceAdapter 接口

```cpp
class SurfaceAdapter {
public:
    static sptr<Surface> CreateSurface();

    virtual int32_t SetDefaultWidthAndHeight(int32_t width, int32_t height) = 0;
    virtual int32_t GetDefaultWidth() = 0;
    virtual int32_t GetDefaultHeight() = 0;

    virtual sptr<SurfaceBuffer> RequestBuffer(int32_t& fencingFd) = 0;
    virtual int32_t FlushBuffer(sptr<SurfaceBuffer> buffer) = 0;

    virtual int32_t LinkToConsumer(sptr<SurfaceAdapter> &consumer) = 0;
};
```

### VSyncAdapter 接口

```cpp
class VSyncAdapter {
public:
    virtual void RequestFrame(
        const std::function<void(int64_t timestamp, int64_t period)>& callback
    ) = 0;

    virtual void Destroy() = 0;
};
```

### HiSysEventAdapter 接口

```cpp
class HiSysEventAdapter {
public:
    virtual int Write(
        const std::string& eventName,
        EventType type,
        const std::tuple<const std::string, const std::string>& data
    ) = 0;

    // ... 多个重载版本，支持不同数量的参数
};
```

## 依赖关系

### 内部依赖

```gn
deps = [
    "../ohos_nweb:web_configs",           # WebView 配置
    "//base/web/webview/arkweb_utils:libarkweb_utils",  # 工具库
    "../ohos_glue:ohos_base_glue_source",  # Glue 代码
]
```

### 外部依赖（部分）

```gn
external_deps = [
    # 核心框架
    "ability_runtime:ability_manager",
    "bundle_framework:appexecfwk_base",
    "eventhandler:libeventhandler",
    "ipc:ipc_core",

    # 图形显示
    "graphic_2d:libcomposer",
    "graphic_surface:surface",
    "window_manager:libwm",

    # DFX
    "hilog:libhilog",
    "hisysevent:libhisysevent",
    "hitrace:hitrace_meter",

    # 媒体（条件编译）
    "audio_framework:audio_client",
    "av_codec:av_codec_client",
    "camera_framework:camera_framework",

    # 其他
    "pasteboard:pasteboard_client",
    "access_token:libaccesstoken_sdk",
    "cert_manager:cert_manager_sdk",

    # ... 共 40+ 个依赖
]
```

## 维护指南

### 添加新的适配器

1. **创建目录结构**：
   ```
   ohos_adapter/new_feature_adapter/
   ├── include/
   │   └── new_feature_adapter_impl.h
   └── src/
       └── new_feature_adapter_impl.cpp
   ```

2. **定义接口**（在 `interfaces/` 目录）：
   ```cpp
   // interfaces/new_feature_adapter.h
   class NewFeatureAdapter {
   public:
       virtual ~NewFeatureAdapter() = default;
       virtual int DoSomething() = 0;
   };
   ```

3. **实现适配器**：
   ```cpp
   // new_feature_adapter/include/new_feature_adapter_impl.h
   class NewFeatureAdapterImpl : public NewFeatureAdapter {
   public:
       int DoSomething() override;
   };
   ```

4. **更新 BUILD.gn**：
   ```gn
   sources += [
       "new_feature_adapter/src/new_feature_adapter_impl.cpp",
   ]
   
   include_dirs += [
       "new_feature_adapter/include",
   ]
   
   external_deps += [
       "new_feature:feature_client",
   ]
   ```

5. **添加 Mock 实现**（如果支持条件编译）：
   ```cpp
   // new_feature_adapter/src/new_feature_adapter_mock.cpp
   class NewFeatureAdapterMock : public NewFeatureAdapter {
   public:
       int DoSomething() override { return 0; }
   };
   ```

### 编写单元测试

1. **使用 Mock 实现**：
   ```cpp
   TEST(AdapterTest, UseMockAdapter) {
       auto mockAdapter = std::make_shared<NewFeatureAdapterMock>();
       int result = mockAdapter->DoSomething();
       EXPECT_EQ(result, 0);
   }
   ```

2. **测试真实实现**：
   ```cpp
   TEST(AdapterTest, UseRealAdapter) {
       auto adapter = std::make_shared<NewFeatureAdapterImpl>();
       int result = adapter->DoSomething();
       EXPECT_GT(result, 0);
   }
   ```

### 接口变更注意事项

1. **向后兼容** - 新增接口要提供默认实现
2. **Mock 同步** - 接口变更时同步更新 Mock 实现
3. **文档更新** - 更新接口文档和使用示例
4. **测试覆盖** - 新接口要添加单元测试

## 常见问题

### Q1: 为什么需要适配器层？

**A**:
- **解耦合**: WebView 引擎（基于 Chromium）不需要直接依赖 OpenHarmony API
- **可测试**: 提供 Mock 实现，便于单元测试和集成测试
- **可移植**: 理论上可以移植到其他操作系统
- **可维护**: 系统服务 API 变化时，只需修改适配器实现

### Q2: Mock 实现和真实实现有什么区别？

**A**:
- **Mock 实现**: 不调用真实系统 API，只返回默认值，用于编译和测试
- **真实实现**: 调用 OpenHarmony 系统服务，提供真实功能

### Q3: 如何选择启用哪些功能？

**A**: 根据设备需求和产品配置：
- **基础功能**: graphic, hiviewdfx, multimodalinput（默认启用）
- **媒体功能**: audio, location, camera（按需启用）
- **高级功能**: avcodec, print, sensor（按需启用）

### Q4: 适配器性能开销大吗？

**A**: 开销很小：
- **虚函数调用**: 现代编译器优化后开销可忽略
- **直接转发**: 大多数适配器直接转发到系统 API
- **内联优化**: 关键路径可以被内联

### Q5: 如何调试适配器问题？

**A**:
1. 查看 `hilog -T webadapter` 日志
3. 检查适配器返回值和错误码
4. 使用 Mock 实现隔离问题

## 相关文档

- [主 README.md](../README.md) - WebView 组件总览
- [AGENT.md](../AGENT.md) - 代码仓指南
- [ohos_nweb/README.md](../ohos_nweb/README.md) - 核心引擎文档
- [ohos_glue/](../ohos_glue/) - Glue 代码层
