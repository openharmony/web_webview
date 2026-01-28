# ohos_nweb

## 概述

`ohos_nweb` 是 ArkWeb WebView 组件的**核心引擎层**，负责封装和管理基于 Chromium/CEF 的 WebView 引擎。该层提供了 WebView 的核心功能接口，是整个 WebView 组件中最关键的部分。

**核心库**: `libnweb.so` (输出名: `arkweb_core_loader.so`)
**预编译包**: `ArkWebCore.hap`
**崩溃处理**: `arkweb_crashpad_handler`
**配置文件**: `web_config.xml`
**API 标签**: `innerapi_tags: [ "platformsdk" ]`

### 核心职责

1. **引擎加载** - 动态加载 Chromium/CEF 引擎库
2. **生命周期管理** - 管理 WebView 实例的创建和销毁
3. **配置管理** - 解析和应用 WebView 配置
4. **Surface 管理** - 管理 WebView 的渲染 Surface
5. **事件上报** - 上报系统事件和崩溃信息
6. **辅助功能** - 提供 Cookie、数据库、WebStorage 等辅助管理

## 目录结构

```
ohos_nweb/
├── include/                          # 公共头文件
│   ├── nweb_helper.h                 # NWebHelper 辅助类（核心）
│   ├── nweb_adapter_helper.h         # 适配器辅助类
│   ├── nweb_adapter_common.h         # 适配器公共定义
│   ├── nweb_c_api.h                  # C API 接口
│   ├── nweb_config_helper.h          # 配置辅助类
│   ├── nweb_enhance_surface_adapter.h # 增强 Surface 适配器
│   ├── nweb_hisysevent.h             # HiSysEvent 事件上报
│   ├── nweb_init_params.h            # 初始化参数
│   ├── nweb_log.h                    # 日志定义
│   ├── nweb_precompile_callback.h    # 预编译回调
│   ├── nweb_save_cookie_callback.h   # Cookie 保存回调
│   ├── nweb_snapshot_callback_impl.h # 截图回调
│   ├── nweb_store_web_archive_callback.h # WebArchive 回调
│   ├── nweb_surface_adapter.h        # Surface 适配器
│   └── webview_value.h               # WebView 值对象
│
├── src/                              # 源代码实现
│   ├── nweb_helper.cpp               # NWebHelper 实现
│   ├── nweb_surface_adapter.cpp      # Surface 适配器实现
│   ├── nweb_enhance_surface_adapter.cpp # 增强 Surface 实现
│   ├── nweb_hisysevent.cpp           # HiSysEvent 实现
│   ├── nweb_config_helper.cpp        # 配置辅助实现
│   ├── webview_value.cpp             # WebView 值实现
│   └── nweb_crashpad_handler_main.cpp # 崩溃处理主程序
│
├── etc/                              # 配置文件
│   ├── web_config.xml                # WebView 配置文件（核心）
│   ├── para/                         # 系统参数
│   │   ├── web.para                  # WebView 参数
│   │   └── web.para.dac              # 参数 DAC 控制
│   ├── alias.svg                     # 别名图标
│   └── context-menu.svg              # 上下文菜单图标
│
├── prebuilts/                        # 预编译文件
│   ├── arm64/                        # ARM64 架构
│   │   └── ArkWebCore.hap            # WebView 核心包
│   ├── arm/                          # ARM32 架构
│   │   └── ArkWebCore.hap            # WebView 核心包
│   └── NOTICE                        # 许可证通知
│
├── BUILD.gn                          # 构建配置
└── README.md                         # 本文档
```

## 核心架构

### 整体架构

```
┌─────────────────────────────────────────────────────┐
│             应用层 (ArkTS/JS/C++)                    │
│            (通过 NAPI/Native 接口)                  │
└────────────────────┬────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────┐
│              NWebHelper (核心辅助类)                 │
│  ┌──────────────────────────────────────────────┐   │
│  │ - Init() / InitAndRun()                      │   │
│  │ - CreateNWeb()                               │   │
│  │ - LoadWebEngine()                             │   │
│  │ - GetCookieManager()                          │   │
│  │ - GetDataBase()                               │   │
│  └──────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────┘
                     │
                     ├──────────────────────────────┐
                     ▼                              ▼
┌──────────────────────────────┐    ┌─────────────────────┐
│      libnweb.so              │    │  ArkWebCore.hap     │
│  (arkweb_core_loader.so)     │    │  (Chromium 引擎)    │
│                             │    │                     │
│  - NWeb 封装                 │    │  - Renderer 进程    │
│  - Surface 适配              │    │  - GPU 进程        │
│  - 配置管理                  │    │  - Utility 进程     │
│  - 事件上报                  │    │  - 网络栈          │
└──────────────────────────────┘    └─────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────┐
│         ohos_adapter (适配器层)                      │
│    - graphic_adapter                                 │
│    - hiviewdfx_adapter                                │
│    - multimodalinput_adapter                          │
│    - ...                                              │
└─────────────────────────────────────────────────────┘
```

### 模块说明

#### 1. libnweb.so (arkweb_core_loader.so)

**功能**: WebView 核心封装库

**主要组件**:
- **NWebHelper**: 核心辅助类，提供引擎初始化和 WebView 创建
- **NWebSurfaceAdapter**: Surface 适配器，管理渲染 Surface
- **NWebEnhanceSurfaceAdapter**: 增强 Surface，支持硬件加速
- **NWebHisysevent**: 系统事件上报
- **WebViewValue**: WebView 值对象封装

**职责**:
- 动态加载 Chromium 引擎库
- 创建和管理 WebView 实例
- 管理 Surface 和渲染
- 配置管理
- 事件上报

#### 2. ArkWebCore.hap (预编译包)

**功能**: 基于 Chromium 的 WebView 引擎

**包含内容**:
- Renderer 进程：负责 Web 内容渲染
- GPU 进程：负责图形加速
- Utility 进程：负责辅助功能
- 网络栈：HTTP/HTTPS 通信
- V8 引擎：JavaScript 执行
- Blink 引擎：Web 内容解析

**架构支持**:
- `arm64`: 64 位 ARM 设备
- `arm`: 32 位 ARM 设备
- `x86_64`: 64 位 x86 设备（使用 arm64 版本）

#### 3. arkweb_crashpad_handler

**功能**: 崩溃处理程序

**职责**:
- 捕获 WebView 崩溃信息
- 生成 Minidump 文件
- 上传崩溃报告

## 核心组件

### NWebHelper

**定位**: 核心辅助类，提供 WebView 引擎的初始化和管理

**主要接口**:

```cpp
class OHOS_NWEB_EXPORT NWebHelper {
public:
    // 获取单例
    static NWebHelper& Instance();

    // 初始化引擎
    bool Init(bool from_ark = true);
    bool InitAndRun(bool from_ark = true);

    // 加载 Web 引擎
    bool LoadWebEngine(bool fromArk, bool runFlag);

    // 创建 WebView 实例
    std::shared_ptr<NWeb> CreateNWeb(
        std::shared_ptr<NWebCreateInfo> create_info);

    // 获取辅助管理器
    std::shared_ptr<NWebCookieManager> GetCookieManager();
    std::shared_ptr<NWebDataBase> GetDataBase();
    std::shared_ptr<NWebWebStorage> GetWebStorage();

    // 获取 WebView 实例
    std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id);

    // 设置 Bundle 路径
    void SetBundlePath(const std::string& path);

    // 暂停/恢复所有定时器
    void PauseAllTimers();
    void ResumeAllTimers();

    // 设置渲染进程模式
    void SetRenderProcessMode(RenderProcessMode mode);
    RenderProcessMode GetRenderProcessMode();

    // 设置自定义 User Agent
    void SetAppCustomUserAgent(const std::string& userAgent);
    void SetUserAgentForHosts(const std::string& userAgent,
                              const std::vector<std::string>& hosts);

    // 预取资源
    void PrefetchResource(
        const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers,
        const std::string& cache_key,
        const uint32_t& cache_valid_time);

    // 预热 Service Worker
    void WarmupServiceWorker(const std::string& url);

    // 获取默认 User Agent
    std::string GetDefaultUserAgent();

    // 获取广告拦截管理器
    std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager();
};
```

**使用流程**:

```cpp
// 1. 初始化引擎
NWebHelper& helper = NWebHelper::Instance();
helper.Init();

// 2. 创建 WebView 实例
auto createInfo = std::make_shared<NWebCreateInfo>();
createInfo->com_name = "MyWebView";
createInfo->surface_adapter = surfaceAdapter;
auto nweb = helper.CreateNWeb(createInfo);

// 3. 使用 WebView
nweb->LoadURL("https://www.example.com");

// 4. 获取辅助管理器
auto cookieManager = helper.GetCookieManager();
auto dataBase = helper.GetDataBase();
auto webStorage = helper.GetWebStorage();
```

### NWebSurfaceAdapter

**定位**: Surface 适配器，管理 WebView 的渲染 Surface

**主要功能**:
- 创建和配置 Surface
- 管理 Surface 的生命周期
- 处理 VSync 信号
- 支持硬件加速

**接口示例**:
```cpp
class NWebSurfaceAdapter {
public:
    // 创建 Surface
    static sptr<Surface> CreateSurface();

    // 设置宽高
    int32_t SetDefaultWidthAndHeight(int32_t width, int32_t height);

    // 获取 Buffer
    sptr<SurfaceBuffer> RequestBuffer(int32_t& fencingFd);

    // 刷新 Buffer
    int32_t FlushBuffer(sptr<SurfaceBuffer> buffer);

    // 连接消费者
    int32_t LinkToConsumer(sptr<SurfaceAdapter>& consumer);
};
```

### NWebEnhanceSurfaceAdapter

**定位**: 增强 Surface 适配器，提供额外的渲染功能

**主要功能**:
- 支持增强的渲染模式
- 支持硬件缓冲区
- 支持 VSync 同步
- 支持多层渲染

### NWebHisysevent

**定位**: HiSysEvent 事件上报

**主要功能**:
- 上报 WebView 事件
- 上报崩溃信息
- 上报性能指标
- 上报错误信息

**事件类型**:
- `WEBVIEW_CRASH`: WebView 崩溃事件
- `WEBVIEW_ERROR`: WebView 错误事件
- `WEBVIEW_PERFORMANCE`: 性能事件

## 配置文件

### web_config.xml

**路径**: `ohos_nweb/etc/web_config.xml`

**功能**: WebView 核心配置文件，控制引擎的行为和特性

**主要配置项**:

#### 1. 渲染配置
```xml
<renderConfig>
    <!-- 渲染进程数量 -->
    <renderProcessCount>20</renderProcessCount>
</renderConfig>
```

#### 2. 媒体配置
```xml
<mediaConfig>
    <!-- 后台媒体是否暂停 -->
    <backgroundMediaShouldSuspend>true</backgroundMediaShouldSuspend>
</mediaConfig>
```

#### 3. 触摸事件配置
```xml
<touchEventConfig>
    <!-- 是否注册触摸事件 -->
    <touchEventShouldRegister>true</touchEventShouldRegister>
</touchEventConfig>
```

#### 4. 设置配置
```xml
<settingConfig>
    <!-- 是否启用 HTTP 缓存 -->
    <enableHttpCacheSimple>true</enableHttpCacheSimple>

    <!-- 是否启用打印 -->
    <enablePrinting>false</enablePrinting>

    <!-- 是否启用简单后端 -->
    <enableSimpleBackendIsDefault>true</enableSimpleBackendIsDefault>

    <!-- 是否计算平板模式 -->
    <enableCalcTabletMode>true</enableCalcTabletMode>

    <!-- 是否设置 HTTP 缓存最大大小 -->
    <enableSetHttpCacheMaxSize>true</enableSetHttpCacheMaxSize>
</settingConfig>
```

#### 5. 性能配置
```xml
<performanceConfig>
    <LowerFrameRateConfig>
        <!-- 可见区域比例阈值 -->
        <visibleAreaRatio>0.1</visibleAreaRatio>
        <visibleAreaRatioV2>0.3</visibleAreaRatioV2>
    </LowerFrameRateConfig>

    <flowBufferConfig>
        <!-- 最大文件描述符数量 -->
        <maxFdNumber>0</maxFdNumber>
    </flowBufferConfig>
</performanceConfig>
```

#### 6. User Agent 配置
```xml
<userAgentConfig>
    <!-- User Agent 类型: Phone/Tablet/Desktop -->
    <userAgentValue>Phone</userAgentValue>
</userAgentConfig>
```

### web.para

**路径**: `ohos_nweb/etc/para/web.para`

**功能**: WebView 系统参数，运行时可调整

**主要参数**:
- 渲染相关参数
- 网络相关参数
- 性能相关参数
- 调试相关参数

### web.para.dac

**路径**: `ohos_nweb/etc/para/web.para.dac`

**功能**: WebView 参数 DAC 控制，定义参数的访问权限

## 构建配置

### BUILD.gn 分析

#### 1. libnweb (arkweb_core_loader.so)

```gn
ohos_shared_library("libnweb") {
  output_name = "arkweb_core_loader"

  # 分支保护（arm64）
  if (target_cpu == "arm64") {
    branch_protector_ret = "pac_ret"
  }

  # 源文件
  sources = [
    "src/nweb_enhance_surface_adapter.cpp",
    "src/nweb_helper.cpp",
    "src/nweb_hisysevent.cpp",
    "src/nweb_surface_adapter.cpp",
    "src/webview_value.cpp",
  ]

  # 依赖
  deps = [
    ":alias",                          # 别名图标
    ":context-menu",                   # 上下文菜单
    ":web.para",                       # 系统参数
    ":web.para.dac",                   # 参数 DAC
    ":web_config",                     # WebView 配置
    ":web_configs",                    # 配置辅助
    "${webview_path}/ohos_glue:ohos_adapter_glue_source",
    "${webview_path}/ohos_nweb_glue_source",
    "${webview_path}/arkweb_utils:libarkweb_utils",
  ]

  # 标记为平台 SDK
  innerapi_tags = [ "platformsdk" ]
}
```

#### 2. web_configs

```gn
ohos_shared_library("web_configs") {
  sources = [ "src/nweb_config_helper.cpp" ]

  deps = [
    ":web_config",
    "${webview_path}/arkweb_utils:libarkweb_utils",
  ]
}
```

#### 3. nweb_hap (预编译包)

```gn
ohos_prebuilt_etc("nweb_hap") {
  # 根据架构选择 HAP
  if (target_cpu == "arm64") {
    web_source = "prebuilts/arm64/ArkWebCore.hap"
  } else if (target_cpu == "arm") {
    web_source = "prebuilts/arm/ArkWebCore.hap"
  } else if (target_cpu == "x86_64") {
    web_source = "prebuilts/arm64/ArkWebCore.hap"
  }

  source = web_source
  module_install_dir = "app/${webview_package_name}"
}
```

#### 4. chrome_crashpad_handler

```gn
ohos_executable("chrome_crashpad_handler") {
  output_name = "arkweb_crashpad_handler"

  sources = [ "src/nweb_crashpad_handler_main.cpp" ]

  install_enable = true
}
```

### 条件编译

```gn
# 架构相关定义
if (target_cpu == "arm64") {
  defines = [ "webview_arm64" ]
} else if (target_cpu == "arm") {
  defines = []
} else if (target_cpu == "x86_64") {
  defines = [ "webview_x86_64" ]
}

# ASAN 支持
if (use_hwasan) {
  defines += [
    "IS_ASAN",
    "WEBVIEW_SANDBOX_LIB_PATH_ASAN=\"${asan_webview_sandbox_lib_path}\"",
  ]
}
```

## 工作流程

### 引擎初始化流程

```
1. NWebHelper::Init()
   │
   ├─ 2. 加载配置
   │   └─ 解析 web_config.xml
   │
   ├─ 3. 设置 Bundle 路径
   │   └─ SetBundlePath()
   │
   ├─ 4. 加载 ArkWebCore.hap
   │   └─ LoadWebEngine()
   │       ├─ dlopen("libarkweb_engine.so")
   │       ├─ 加载符号
   │       └─ 初始化引擎
   │
   ├─ 5. 初始化适配器
   │   └─ 通过 ohos_glue 初始化
   │
   └─ 6. 上报启动事件
       └─ NWebHisysevent
```

### WebView 创建流程

```
1. 应用请求创建 WebView
   │
   ├─ 2. 调用 NWebHelper::CreateNWeb()
   │   │
   │   ├─ 3. 创建 NWebCreateInfo
   │   │   ├─ 设置组件名称
   │   │   ├─ 设置 Surface 适配器
   │   │   └─ 设置回调接口
   │   │
   │   ├─ 4. 创建 NWeb 实例
   │   │   └─ 调用引擎 API 创建 WebView
   │   │
   │   └─ 5. 返回 NWeb 指针
   │
   ├─ 6. 应用配置 WebView
   │   ├─ 设置大小
   │   ├─ 设置前景色
   │   └─ 设置背景色
   │
   ├─ 7. 加载 URL
   │   └─ nweb->LoadURL(url)
   │
   └─ 8. 开始渲染
       ├─ Renderer 进程渲染内容
       ├─ GPU 进程合成画面
       └─ Surface 显示
```

### Surface 管理流程

```
1. 创建 Surface
   ├─ NWebSurfaceAdapter::CreateSurface()
   └─ 获取生产者 Surface
       │
2. 配置 Surface
   ├─ SetDefaultWidthAndHeight()
   ├─ SetUserData()
   └─ SetDefaultUsage()
       │
3. 连接消费者
   ├─ LinkToConsumer()
   └─ GPU 进程连接到 Surface
       │
4. 渲染循环
   ├─ RequestBuffer()    # 请求 Buffer
   ├─ GPU 渲染           # 渲染内容
   ├─ FlushBuffer()      # 提交 Buffer
   └─ VSync 同步         # 垂直同步
```

## 使用场景

### 场景 1: 初始化 WebView 引擎

```cpp
#include "nweb_helper.h"

using namespace OHOS::NWeb;

// 1. 获取 NWebHelper 单例
NWebHelper& helper = NWebHelper::Instance();

// 2. 初始化引擎
bool success = helper.Init();
if (!success) {
    // 初始化失败
    return;
}

// 3. 设置 Bundle 路径
helper.SetBundlePath("/data/app/el2/100/base/com.example.app");

// 4. 或者使用 InitAndRun() 一步到位
success = helper.InitAndRun();
```

### 场景 2: 创建 WebView 实例

```cpp
#include "nweb_helper.h"
#include "nweb_create_info.h"

using namespace OHOS::NWeb;

// 1. 创建 Surface
auto surfaceAdapter = std::make_shared<NWebSurfaceAdapterImpl>();
surfaceAdapter->Init();

// 2. 创建 WebView 信息
auto createInfo = std::make_shared<NWebCreateInfo>();
createInfo->com_name = "MyWebView";
createInfo->surface_adapter = surfaceAdapter;

// 3. 创建回调
auto delegate = std::make_shared<MyNWebDelegate>();
createInfo->nweb_delegate = delegate;

// 4. 创建 WebView
auto nweb = helper.CreateNWeb(createInfo);
if (!nweb) {
    // 创建失败
    return;
}

// 5. 配置 WebView
nweb->Resize(1920, 1080);
nweb->SetBackgroundColor(0xFFFFFFFF);

// 6. 加载 URL
nweb->LoadURL("https://www.example.com");
```

### 场景 3: 使用辅助管理器

```cpp
// Cookie 管理
auto cookieManager = helper.GetCookieManager();
cookieManager->SetCookie("https://www.example.com", "key=value");
auto cookies = cookieManager->GetCookies("https://www.example.com");

// 数据库管理
auto dataBase = helper.GetDataBase();
dataBase->SetQuota(origin, 100 * 1024 * 1024);  // 100MB
auto quota = dataBase->GetQuota(origin);

// WebStorage 管理
auto webStorage = helper.GetWebStorage();
webStorage->SetLocalStorage(origin, "key", "value");
auto value = webStorage->GetLocalStorage(origin, "key");
```

### 场景 4: 设置 User Agent

```cpp
// 1. 设置应用自定义 User Agent
helper.SetAppCustomUserAgent("MyApp/1.0 (Custom)");

// 2. 为特定域名设置 User Agent
std::vector<std::string> hosts = {
    "www.example.com",
    "api.example.com"
};
helper.SetUserAgentForHosts("MobileApp/1.0", hosts);

// 3. 获取默认 User Agent
std::string defaultUA = helper.GetDefaultUserAgent();
```

### 场景 5: 资源预取

```cpp
// 1. 创建预取参数
auto preArgs = std::make_shared<NWebEnginePrefetchArgs>();
preArgs->url = "https://www.example.com/resource.js";

// 2. 设置 HTTP 头
std::map<std::string, std::string> headers = {
    {"Accept", "application/javascript"},
    {"Cache-Control", "max-age=3600"}
};

// 3. 预取资源
helper.PrefetchResource(
    preArgs,
    headers,
    "cache_key_123",      // 缓存键
    3600                   // 有效期（秒）
);
```

## 依赖关系

### 内部依赖

```gn
deps = [
    ":alias",                          # 别名图标
    ":context-menu",                   # 上下文菜单
    ":web.para",                       # 系统参数
    ":web.para.dac",                   # 参数 DAC
    ":web_config",                     # WebView 配置
    ":web_configs",                    # 配置辅助
    "${webview_path}/ohos_glue:ohos_adapter_glue_source",    # 适配器 Glue
    "${webview_path}/ohos_glue:ohos_base_glue_source",       # 基础 Glue
    "${webview_path}/ohos_glue:ohos_nweb_glue_source",        # NWeb Glue
    "${webview_path}/arkweb_utils:libarkweb_utils",          # 工具库
]
```

### 外部依赖

```gn
external_deps = [
    # Ability 框架
    "ability_runtime:ability_manager",
    "ability_runtime:app_context",
    "ability_runtime:app_manager",

    # Bundle 框架
    "bundle_framework:appexecfwk_base",
    "bundle_framework:appexecfwk_core",

    # 图形显示
    "graphic_2d:libcomposer",
    "graphic_2d:librender_service_client",
    "graphic_surface:surface",
    "window_manager:libwm",

    # 系统服务
    "hilog:libhilog",
    "hisysevent:libhisysevent",
    "init:libbegetutil",
    "input:libmmi-client",
    "ipc:ipc_core",

    # 其他
    "c_utils:utils",
    "config_policy:configpolicy_util",
    "i18n:intl_util",
    "libxml2:libxml2",
]
```

## 性能优化

### 1. 渲染进程数控制

```xml
<!-- web_config.xml -->
<renderProcessCount>20</renderProcessCount>
```

**说明**: 控制最大渲染进程数量，避免过多进程消耗内存

### 2. 帧率自适应

```xml
<LowerFrameRateConfig>
    <visibleAreaRatio>0.1</visibleAreaRatio>
    <visibleAreaRatioV2>0.3</visibleAreaRatioV2>
</LowerFrameRateConfig>
```

**说明**: 根据可见区域比例自动降低帧率，节省电量

### 3. 后台媒体暂停

```xml
<backgroundMediaShouldSuspend>true</backgroundMediaShouldSuspend>
```

**说明**: WebView 进入后台时自动暂停媒体播放

### 4. HTTP 缓存优化

```xml
<enableHttpCacheSimple>true</enableHttpCacheSimple>
<enableSetHttpCacheMaxSize>true</enableSetHttpCacheMaxSize>
```

**说明**: 启用简单的 HTTP 缓存策略，提升加载速度

## 故障处理

### 崩溃处理

**崩溃处理程序**: `arkweb_crashpad_handler`

**功能**:
- 捕获崩溃信号
- 生成 Minidump 文件
- 记录崩溃堆栈
- 上传崩溃报告

**崩溃文件位置**:
```
/data/log/crash/
├── WebView.dmp
└── WebView.log
```

### 事件上报

**NWebHisysevent** 上报以下事件：

1. **WEBVIEW_CRASH**
   - WebView 崩溃事件
   - 包含崩溃原因和堆栈信息

2. **WEBVIEW_ERROR**
   - WebView 错误事件
   - 包含错误码和错误信息

3. **WEBVIEW_PERFORMANCE**
   - 性能事件
   - 包含加载时间、渲染时间等指标

### 调试建议

1. **查看日志**:
   ```bash
   hilog -T ArkWeb
   hilog -T webadapter
   ```

2. **查看配置**:
   ```bash
   cat /etc/web/web_config.xml
   param get web.*
   ```

3. **检查进程**:
   ```bash
   ps -A | grep arkweb
   ```

4. **内存分析**:
   ```bash
   hidumper -s 8610 -a --memory
   ```

## 常见问题

### Q1: libnweb.so 为什么输出名是 arkweb_core_loader.so？

**A**:
- `libnweb.so` 是编译时的名称
- `arkweb_core_loader.so` 是运行时的输出名称
- 这样命名更清晰地表达其功能：加载 ArkWeb 核心引擎

### Q2: ArkWebCore.hap 是预编译的，能否修改？

**A**:
- ArkWebCore.hap 是基于 Chromium 编译的预编译包
- 一般不需要修改
- 如需修改，需要修改 Chromium 源码并重新编译

### Q3: 如何调整 WebView 的性能？

**A**:
- 修改 `web_config.xml` 中的配置项
- 调整 `renderProcessCount`
- 启用/禁用各种优化选项
- 使用性能监控工具分析瓶颈

### Q4: WebView 支持哪些架构？

**A**:
- `arm64`: 主要支持，64位 ARM 设备
- `arm`: 支持，32位 ARM 设备
- `x86_64`: 有限支持，使用 arm64 版本

### Q5: 如何启用 WebView 的特定功能？

**A**:
- 检查 `web_config.xml` 中的 `enableXxx` 配置项
- 修改对应的值为 `true`
- 重新编译并部署

## 维护建议

### 代码规范

1. **头文件保护**: 使用 `#ifndef` 宏
2. **导出符号**: 使用 `OHOS_NWEB_EXPORT` 宏
3. **日志记录**: 使用 `WVLOG_*` 宏
4. **错误处理**: 统一使用错误码和日志

### 添加新功能

1. **定义接口**: 在 `include/` 中添加头文件
2. **实现功能**: 在 `src/` 中添加实现
3. **更新配置**: 如需配置，更新 `web_config.xml`
4. **添加测试**: 在 `test/` 中添加单元测试
5. **更新文档**: 更新 README 和接口文档

### 测试要点

1. **单元测试**: 测试各个组件的功能
2. **集成测试**: 测试 WebView 的完整流程
3. **性能测试**: 测试加载速度、渲染性能
4. **稳定性测试**: 长时间运行测试
5. **兼容性测试**: 测试不同网站的兼容性

## 相关文档

- [主 README.md](../README.md) - WebView 组件总览
- [AGENT.md](../AGENT.md) - 代码仓指南
- [arkweb_utils/README.md](../arkweb_utils/README.md) - 工具库文档
- [ohos_adapter/README.md](../ohos_adapter/README.md) - 适配器层文档
- [ohos_glue/](../ohos_glue/) - Glue 代码层

## 许可证

Apache License 2.0
