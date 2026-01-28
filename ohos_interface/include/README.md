# ohos_interface/include

## 概述

本目录包含 ArkWeb WebView 组件的所有公共接口定义,分为两大类:

1. **ohos_adapter** - ArkWebCore.hap 内核调用系统侧服务的接口
2. **ohos_nweb** - 系统侧应用调用 ArkWebCore.hap 内核功能的接口

## 目录结构

```
include/
├── ohos_adapter/              # 适配器接口 (66 个头文件)
│   ├── graphic_adapter.h
│   ├── location_adapter.h
│   └── ...
│
└── ohos_nweb/                 # NWeb 接口 (53 个头文件)
    ├── nweb.h
    ├── nweb_helper.h
    └── ...
```

## ohos_adapter 接口（内核 → 系统侧）

**说明**: 这些接口定义了 ArkWebCore.hap 内核如何访问 OpenHarmony 系统服务。

**实现位置**: `ohos_adapter/<adapter_name>/src/<adapter_name>_adapter_impl.cpp`

**约束**: ❌ **不允许新增 ohos_adapter 接口**

### 1. 图形和显示适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `graphic_adapter.h` | Surface, VSync, Window | Surface 管理、垂直同步、窗口管理 |
| `display_manager_adapter.h` | DisplayManager | 显示管理器（屏幕、显示器） |
| `ohos_drawing_text_adapter.h` | NativeDrawing | 文本绘制 |

### 2. Ability 框架适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `aafwk_app_mgr_client_adapter.h` | AbilityManager | 应用管理器 |
| `aafwk_browser_client_adapter.h` | AbilityManager | 浏览器 Ability 客户端 |
| `aafwk_browser_host_adapter.h` | AbilityManager | 浏览器宿主 |
| `aafwk_render_scheduler_host_adapter.h` | AbilityManager | 渲染调度器 |

### 3. 媒体适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `audio_renderer_adapter.h` | AudioFramework | 音频渲染（播放） |
| `audio_capturer_adapter.h` | AudioFramework | 音频捕获（录音） |
| `audio_system_manager_adapter.h` | AudioFramework | 音频系统管理 |
| `audio_device_desc_adapter.h` | AudioFramework | 音频设备描述符 |
| `audio_cenc_info_adapter.h` | AVCodec | 音频 CENC 信息 |
| `audio_codec_decoder_adapter.h` | AVCodec | 音频解码器 |
| `media_adapter.h` | AVCodec, PlayerFramework | 媒体编解码、播放器 |

### 4. 输入和交互适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `mmi_adapter.h` | MultiModalInput | 多模态输入设备管理 |
| `inputmethodframework_adapter.h` | InputMethodFramework | 输入法框架 |
| `custom_keyboard_handler.h` | InputMethodFramework | 自定义键盘 |

### 5. 网络适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `net_connect_adapter.h` | NetManager | 网络连接管理 |
| `net_proxy_adapter.h` | NetManager | 网络代理 |

### 6. 系统服务适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `power_mgr_client_adapter.h` | PowerManager | 电源管理 |
| `battery_mgr_client_adapter.h` | BatteryManager | 电池管理 |
| `location_adapter.h` | Locator | 位置服务（GPS） |
| `sensor_adapter.h` | Sensor | 传感器访问 |
| `camera_manager_adapter.h` | CameraManager | 相机管理 |
| `print_manager_adapter.h` | PrintFramework | 打印服务 |
| `screenlock_mgr_adapter.h` | ScreenLockManager | 屏幕锁定 |

### 7. 数据和存储适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `datashare_adapter.h` | DataShare | 数据共享 |
| `keystore_adapter.h` | Huks | 密钥存储 |
| `pasteboard_client_adapter.h` | Pasteboard | 剪贴板 |

### 8. 安全和权限适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `access_token_adapter.h` | AccessToken | 访问令牌权限校验 |
| `cert_mgr_adapter.h` | CertManager | 证书管理 |

### 9. DFX 适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `hiviewdfx_adapter.h` | HiSysEvent, HiTrace | 系统事件、链路追踪 |
| `hilog_adapter.h` | HiLog | 日志系统 |
| `hisysevent_adapter.h` | HiSysEvent | 系统事件上报 |
| `hitrace_adapter.h` | HiTrace | 链路追踪 |

### 10. 其他适配器

| 接口文件 | 系统服务 | 功能说明 |
|---------|---------|---------|
| `background_task_adapter.h` | BackgroundTaskMgr | 后台任务 |
| `event_handler_adapter.h` | EventHandler | 事件处理 |
| `flowbuffer_adapter.h` | ResourceSchedule | 流缓冲区 |
| `migration_manager_adapter.h` | MiscFrameworks | 迁移管理 |
| `ohos_image_adapter.h` | ImageFramework | 图像解码 |
| `ohos_native_buffer_adapter.h` | GraphicBuffer | Native Buffer |
| `ohos_resource_adapter.h` | ResourceManager | 资源管理 |
| `system_properties_adapter.h` | SystemProperties | 系统属性 |

## ohos_nweb 接口（系统侧 → 内核）

**说明**: 这些接口定义了应用如何使用 ArkWebCore.hap 内核提供的 WebView 功能。

**实现位置**: `ohos_nweb/src/` 或 `ohos_glue/ohos_nweb/bridge/`

**约束**: ✅ **允许新增 ohos_nweb 接口**

### 1. 核心类接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb.h` | NWeb 主类，提供 WebView 核心功能 |
| `nweb_helper.h` | NWebHelper 辅助类，引擎初始化和管理 |
| `nweb_handler.h` | 事件处理器，处理各种 WebView 事件 |
| `nweb_agent_handler.h` | Agent 处理器，处理页面生命周期 |
| `nweb_agent_manager.h` | Agent 管理器，管理多个 Agent |
| `nweb_engine.h` | 引擎接口，定义引擎相关操作 |
| `nweb_preference.h` | WebView 配置项 |

### 2. 回调接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_javascript_result_callback.h` | JavaScript 执行结果回调 |
| `nweb_download_callback.h` | 下载回调 |
| `nweb_find_callback.h` | 查找回调 |
| `nweb_snapshot_callback.h` | 截图回调 |
| `nweb_value_callback.h` | 值回调 |
| `nweb_release_surface_callback.h` | Surface 释放回调 |
| `nweb_app_link_callback.h` | App Link 回调 |
| `nweb_blankless_callback.h` | 白屏回调 |
| `nweb_data_resubmission_callback.h` | 数据重新提交回调 |

### 3. 数据类型接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_value.h` | WebView 值对象 |
| `nweb_drag_data.h` | 拖拽数据 |
| `nweb_hit_testresult.h` | 点击测试结果 |
| `nweb_context_menu_params.h` | 上下文菜单参数 |
| `nweb_accessibility_node_info.h` | 无障碍节点信息 |
| `nweb_console_log.h` | 控制台日志 |
| `nweb_user_agent_metadata.h` | User Agent 元数据 |
| `nweb_load_committed_details.h` | 加载提交详情 |
| `nweb_touch_handle_state.h` | 触摸处理状态 |

### 4. 管理器接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_cookie_manager.h` | Cookie 管理器 |
| `nweb_data_base.h` | 数据库管理器 |
| `nweb_web_storage.h` | WebStorage 管理器 |
| `nweb_download_manager.h` | 下载管理器 |
| `nweb_adsblock_manager.h` | 广告拦截管理器 |

### 5. 辅助功能接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_access_request.h` | 访问请求 |
| `nweb_autofill.h` | 自动填充 |
| `nweb_custom_keyboard_handler.h` | 自定义键盘 |
| `nweb_date_time_chooser.h` | 日期时间选择器 |
| `nweb_file_selector_params.h` | 文件选择参数 |
| `nweb_full_screen_exit_handler.h` | 全屏退出处理 |
| `nweb_select_popup_menu.h` | 选择弹出菜单 |
| `nweb_geolocation_callback_interface.h` | 地理位置 |

### 6. 网络相关接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_url_resource_request.h` | URL 资源请求 |
| `nweb_url_resource_response.h` | URL 资源响应 |
| `nweb_url_resource_error.h` | URL 资源错误 |
| `nweb_js_http_auth_result.h` | HTTP 认证结果 |
| `nweb_js_ssl_error_result.h` | SSL 错误结果 |
| `nweb_js_ssl_select_cert_result.h` | SSL 证书选择 |
| `nweb_js_verify_pin_result.h` | PIN 验证 |
| `nweb_js_dialog_result.h` | JS 对话框结果 |
| `nweb_proxy_changed_callback.h` | 代理变更回调 |

### 7. 媒体相关接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_native_media_player.h` | 原生媒体播放器 |
| `nweb_web_message.h` | Web 消息 |
| `nweb_runtime_connect_info.h` | 运行时连接信息 |

### 8. 性能和监控接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_first_meaningful_paint_details.h` | 首次有意义的绘制 |
| `nweb_largest_contentful_paint_details.h` | 最大内容绘制 |

### 9. 其他接口

| 接口文件 | 功能说明 |
|---------|---------|
| `nweb_api_level.h` | API 级别 |
| `nweb_export.h` | 导出宏定义 |
| `nweb_history_list.h` | 历史记录列表 |
| `nweb_key_event.h` | 按键事件 |
| `nweb_gesture_event_result.h` | 手势事件结果 |
| `nweb_accessibility_event_callback.h` | 无障碍事件回调 |
| `nweb_controller_handler.h` | 控制器处理器 |
| `nweb_native_message_callback.h` | 原生消息回调 |
| `nweb_spanstring_convert_html_callback.h` | Span 转换回调 |
| `nweb_vault_plain_text_callback.h` | 纯文本回调 |
| `nweb_store_web_archive_callback.h` | 存储 WebArchive |

## 核心接口示例

### NWeb 主接口示例

```cpp
// nweb.h
namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWeb {
public:
    // 加载 URL
    virtual void LoadURL(const std::string& url) = 0;

    // 后退/前进
    virtual void GoBack() = 0;
    virtual void GoForward() = 0;

    // 刷新
    virtual void Reload() = 0;

    // 执行 JavaScript
    virtual void ExecuteJavaScript(
        const std::string& script,
        std::shared_ptr<NWebJavaScriptResultCallback> callback) = 0;

    // 获取标题
    virtual std::string GetTitle() = 0;

    // 获取当前 URL
    virtual std::string GetUrl() = 0;

    // ... 更多接口
};

} // namespace OHOS::NWeb
```

### 适配器接口示例

```cpp
// graphic_adapter.h
namespace OHOS::NWeb {

class SurfaceAdapter {
public:
    virtual ~SurfaceAdapter() = default;

    // 创建 Surface
    static sptr<Surface> CreateSurface();

    // 设置宽高
    virtual int32_t SetDefaultWidthAndHeight(int32_t width, int32_t height) = 0;

    // 获取 Buffer
    virtual sptr<SurfaceBuffer> RequestBuffer(int32_t& fencingFd) = 0;

    // 刷新 Buffer
    virtual int32_t FlushBuffer(sptr<SurfaceBuffer> buffer) = 0;
};

class VSyncAdapter {
public:
    virtual ~VSyncAdapter() = default;

    // 请求 VSync 信号
    virtual void RequestFrame(
        const std::function<void(int64_t timestamp, int64_t period)>& callback
    ) = 0;

    // 销毁
    virtual void Destroy() = 0;
};

} // namespace OHOS::NWeb
```

## 接口统计

| 类型 | 数量 | 是否允许新增 |
|------|------|------------|
| ohos_adapter 接口 | 66 | ❌ 不允许 |
| ohos_nweb 接口 | 53 | ✅ 允许 |

## 使用说明

### 应用开发者

如果需要使用 WebView 功能:
1. 引用 `ohos_nweb/` 下的接口头文件
2. 通过 NAPI 接口或 NDK 接口使用
3. 参见 [主 README](../../README.md) 了解更多信息

### 内核开发者

如果需要访问系统服务:
1. 查看现有的 ohos_adapter 接口
2. 优先使用 NDK 接口(高性能场景)
3. 或通过 NAPI 调用 ArkTS 接口(性能不敏感)
4. **不允许新增** ohos_adapter 接口

## 相关文档

- [主 README](../README.md) - ohos_interface 总览
- [../../ohos_adapter/README.md](../../oh_adapter/README.md) - 适配器层文档
- [../../ohos_nweb/README.md](../../ohos_nweb/README.md) - 核心引擎文档
- [../../AGENT.md](../../AGENT.md) - 代码仓指南
