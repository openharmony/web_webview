# test - ArkWeb 测试代码

本目录包含 ArkWeb 项目的所有测试代码,包括单元测试和模糊测试。

## 概述

### 测试框架

- **单元测试框架**: gtest/gmock
- **模糊测试框架**: LLVM Fuzzer + FuzzedDataProvider
- **测试构建系统**: GN + Ninja

### 测试类型

```
test/
├── unittest/        # 单元测试 (58个测试目标)
├── fuzztest/        # 模糊测试 (196个测试目标)
└── resource/        # 测试资源文件
```

## 测试目录结构

### 1. unittest/ - 单元测试

单元测试用于验证代码的正确性,采用白盒测试方式。

#### 统计数据

- **测试目标数量**: 58+
- **测试文件数量**: 数百个 `.cpp` 测试文件
- **测试框架**: gtest

#### 目录组织

```
unittest/
├── common/                              # 测试公共代码
│   ├── nweb_create_window.cpp          # 窗口创建辅助函数
│   └── nweb_create_window.h
│
├── ohos_adapter/                        # 适配器层测试
│   ├── access_token_adapter_test/      # 访问令牌适配器测试
│   ├── audio_adapter/                  # 音频适配器测试
│   ├── camera_adapter_impl_test/       # 相机适配器测试
│   ├── cert_mgr_adapter_test/          # 证书管理器测试
│   ├── datashare_adapter_impl_test/    # 数据共享适配器测试
│   ├── display_adapter/                # 显示适配器测试
│   ├── event_handler_adapter_test/     # 事件处理器测试
│   ├── keystore_adapter_impl_test/     # 密钥存储测试
│   ├── location_adapter/               # 位置服务测试
│   ├── media_adapter_test/             # 媒体适配器测试
│   ├── pasteboard_adapter/             # 剪贴板测试
│   ├── power_mgr_adapter/              # 电源管理测试
│   ├── print_manager_adapter_impl_test/# 打印管理测试
│   ├── screen_capture_adapter_impl_test/# 屏幕捕获测试
│   └── ...                             # 更多适配器测试
│
├── napi_common_test/                    # NAPI 通用测试
├── napi_web_cookie_manager_test/       # NAPI Cookie 管理测试
├── native_common_test/                  # Native 通用测试
├── interface_native_test/               # Native 接口测试
│
├── nweb_helper_test/                    # NWeb 辅助类测试
├── nweb_config_helper_test/             # NWeb 配置测试
├── nweb_hisysevent_test/                # HiSysEvent 测试
├── nweb_surface_adapter_test/           # Surface 适配器测试
├── nweb_value_test/                     # NWeb 值类型测试
│
├── ohos_adapter_helper/                 # 适配器辅助类测试
├── aafwk_app_client_adapter/            # AAfwk 客户端适配器测试
├── app_fwk_update_client_test/          # 应用框架更新测试
├── ark_hisysevent_adapter_test/         # Ark HiSysEvent 测试
├── arkweb_utils_test/                   # ArkWeb 工具测试
├── battery_mgr_adapter_impl/            # 电池管理测试
├── display_manager_adapter_impl_test/   # 显示管理测试
├── mmi_adapter/                         # MMI 输入适配器测试
├── net_connect_adapter_impl_test/       # 网络连接测试
├── power_mgr_client_adapter_impl_test/  # 电源管理客户端测试
├── soc_perf_adapter_test/               # SoC 性能测试
│
├── sa_web_native_messaging_common_test/ # 系统服务消息测试
├── web_native_messaging_client_test/    # Web 消息客户端测试
└── webview_value_test/                  # WebView 值测试
```

#### 单元测试命名规范

- **目录命名**: `<模块名>_test` 或 `<模块名>_impl_test`
- **文件命名**: `<模块名>_test.cpp`
- **测试类命名**: `<模块名>Test`
- **测试用例命名**: `TEST_F(<测试类名>, <测试场景>)` 或 `HWTEST_F(<测试类名>, <测试场景>)`

#### 单元测试示例

**测试文件**: `audio_adapter_impl_test/audio_adapter_impl_test.cpp`

```cpp
#include <gtest/gtest.h>
#include "audio_adapter_impl.h"

namespace OHOS::NWeb {

class AudioAdapterImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前初始化
        adapter_ = std::make_shared<AudioAdapterImpl>();
    }

    void TearDown() override {
        // 测试后清理
        adapter_ = nullptr;
    }

    std::shared_ptr<AudioAdapterImpl> adapter_;
};

TEST_F(AudioAdapterImplTest, InitAudioRenderer) {
    // 测试音频渲染器初始化
    EXPECT_NE(adapter_, nullptr);
    bool result = adapter_->Init();
    EXPECT_TRUE(result);
}

HWTEST_F(AudioAdapterImplTest, PlayAudio, TestSize.Level1) {
    // 测试音频播放 (需要硬件)
    bool result = adapter_->Play();
    EXPECT_TRUE(result);
}

} // namespace OHOS::NWeb
```

**BUILD.gn**: `audio_adapter_impl_test/BUILD.gn`

```gni
import("../../../../config.gni")
import("//build/test.gni")

module_output_path = "webview/Multimedia"

config("module_private_config") {
  if (webview_audio_enable) {
    defines = [ "NWEB_AUDIO_ENABLE" ]
  }
}

ohos_unittest("nweb_audio_adapter_test") {
  module_out_path = module_output_path

  sources = [ "audio_adapter_impl_test.cpp" ]

  configs = [ ":module_private_config" ]

  deps = [ "$webview_path/ohos_adapter:nweb_ohos_adapter" ]

  external_deps = [
    "c_utils:utils",
    "googletest:gtest_main",
    "hilog:libhilog",
  ]

  if (webview_audio_enable) {
    external_deps += [
      "audio_framework:audio_client",
      "audio_framework:audio_renderer",
    ]
  }
}

group("unittest") {
  testonly = true
  deps = []
  if (webview_audio_enable) {
    deps += [ ":nweb_audio_adapter_test" ]
  }
}
```

### 2. fuzztest/ - 模糊测试

模糊测试用于发现内存错误、崩溃、安全漏洞等问题。

#### 统计数据

- **Fuzzer 目标数量**: 196+
- **测试框架**: LLVM Fuzzer
- **辅助工具**: FuzzedDataProvider

#### 目录组织

```
fuzztest/
├── ohos_adapter/                        # 适配器层模糊测试
│   ├── accesstoken_fuzzer/             # 访问令牌 Fuzzer
│   ├── ashmemcreate_fuzzer/            # Ashmem 创建 Fuzzer
│   ├── audio_adapter/                  # 音频适配器 Fuzzer
│   ├── backgroundtaskimpl_fuzzer/      # 后台任务 Fuzzer
│   ├── battery_adapter/                # 电池适配器 Fuzzer
│   ├── camera_adapter/                 # 相机适配器 Fuzzer
│   ├── cert_mgr_adapter/               # 证书管理 Fuzzer
│   ├── certmgr_fuzzer/                 # 证书管理器 Fuzzer
│   ├── datashareadapter_fuzzer/        # 数据共享 Fuzzer
│   ├── datetimeformatadapter_fuzzer/   # 日期时间格式 Fuzzer
│   ├── display_adapter/                # 显示适配器 Fuzzer
│   ├── drawingtextadapter_fuzzer/      # 绘制文本 Fuzzer
│   ├── enterprisedevicemanagementadapter_fuzzer/ # 企业设备管理 Fuzzer
│   ├── eventhandler_fuzzer/            # 事件处理器 Fuzzer
│   ├── flowbufferadapter_fuzzer/       # 流缓冲 Fuzzer
│   ├── graphic_adapter/                # 图形适配器 Fuzzer
│   ├── keystore_adapter/               # 密钥存储 Fuzzer
│   ├── location_adapter/               # 位置服务 Fuzzer
│   ├── media_adapter/                  # 媒体适配器 Fuzzer
│   ├── nativebufferadapter_fuzzer/     # Native 缓冲 Fuzzer
│   ├── nativeimage_fuzzer/             # Native 图像 Fuzzer
│   ├── ohosimagedecoder_fuzzer/        # 图像解码器 Fuzzer
│   ├── pasteboard_adapter/             # 剪贴板 Fuzzer
│   ├── power_mgr_adapter/              # 电源管理 Fuzzer
│   ├── printmanager_fuzzer/            # 打印管理 Fuzzer
│   ├── screencaptureadapterimpl_fuzzer/# 屏幕捕获 Fuzzer
│   ├── sensoradapter_fuzzer/           # 传感器 Fuzzer
│   ├── systemproperties_fuzzer/        # 系统属性 Fuzzer
│   └── ...                             # 更多适配器 Fuzzer
│
├── ohos_nweb/                          # 核心引擎模糊测试
│   ├── copyframe_fuzzer/               # 复制帧 Fuzzer
│   ├── flushbuffer_fuzzer/             # 刷新缓冲 Fuzzer
│   ├── getcookiemanager_fuzzer/        # Cookie 管理器 Fuzzer
│   ├── getdatabase_fuzzer/             # 数据库 Fuzzer
│   ├── getnweb_fuzzer/                 # NWeb 获取 Fuzzer
│   ├── getwebstorage_fuzzer/           # Web 存储 Fuzzer
│   ├── hisysevent_fuzzer/              # HiSysEvent Fuzzer
│   ├── initandrun_fuzzer/              # 初始化运行 Fuzzer
│   ├── loadlib_fuzzer/                 # 加载库 Fuzzer
│   ├── napi_fuzzer/                    # NAPI Fuzzer
│   ├── napi_webview_controller_fuzzer/ # NAPI WebViewController Fuzzer
│   ├── nwebhelper_fuzzer/              # NWebHelper Fuzzer
│   ├── requestbuffer_fuzzer/           # 请求缓冲 Fuzzer
│   ├── selectcertrequest_fuzzer/       # 选择证书请求 Fuzzer
│   ├── sethttpdns_fuzzer/              # 设置 HTTP DNS Fuzzer
│   └── setwebdebug_fuzzer/             # 设置 Web 调试 Fuzzer
│
├── arkwebutils_fuzzer/                 # ArkWeb 工具 Fuzzer
├── cannavigatebackorforward_fuzzer/    # 导航 Fuzzer
├── executejavascript_fuzzer/           # 执行 JS Fuzzer
├── executejavascriptcode_fuzzer/       # 执行 JS 代码 Fuzzer
├── load_fuzzer/                        # 加载 Fuzzer
├── loadurl_fuzzer/                     # 加载 URL Fuzzer
├── loadwithdata_fuzzer/                # 加载数据 Fuzzer
├── loadwithdataandbaseurl_fuzzer/      # 加载数据和基础 URL Fuzzer
├── ontouchmove_fuzzer/                 # 触摸移动 Fuzzer
├── ontouchpress_fuzzer/                # 触摸按下 Fuzzer
├── ontouchrelease_fuzzer/              # 触摸释放 Fuzzer
├── putbackgroundcolor_fuzzer/          # 设置背景色 Fuzzer
├── registerarkjsfunction_fuzzer/       # 注册 ArkJS 函数 Fuzzer
├── registerasyncjsproxy_fuzzer/        # 注册异步 JS 代理 Fuzzer
├── sendkeyevent_fuzzer/                # 发送按键事件 Fuzzer
├── unregisterarkjsfunction_fuzzer/     # 注销 ArkJS 函数 Fuzzer
├── verifypackageinstall_fuzzer/        # 验证包安装 Fuzzer
└── zoom_fuzzer/                        # 缩放 Fuzzer
```

#### 模糊测试结构

每个 Fuzzer 目录包含:

```
loadurl_fuzzer/
├── BUILD.gn           # 构建配置
├── project.xml        # Fuzzer 配置
├── corpus/            # 语料库 (种子输入)
├── loadurl_fuzzer.h   # 头文件
└── loadurl_fuzzer.cpp # Fuzzer 实现
```

**project.xml** - Fuzzer 配置文件:

```xml
<fuzz_config>
  <fuzztest>
    <!-- 测试输入的最大长度 -->
    <max_len>1000</max_len>
    <!-- Fuzzer 运行的最长时间(秒) -->
    <max_total_time>300</max_total_time>
    <!-- 内存使用限制(MB) -->
    <rss_limit_mb>4096</rss_limit_mb>
  </fuzztest>
</fuzz_config>
```

**Fuzzer 实现**:

```cpp
#include "loadurl_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>
#include "nweb.h"
#include "nweb_create_window.h"

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 64;
constexpr int MAX_VECTOR_SIZE = 10;

std::shared_ptr<OHOS::NWeb::NWeb> g_nweb = nullptr;

bool LoadUrlFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }

    // 使用 FuzzedDataProvider 解析随机输入
    FuzzedDataProvider dataProvider(data, size);

    // 获取或创建测试用的 NWeb 实例
    g_nweb = NWeb::GetNwebForTest();
    if (g_nweb == nullptr) {
        return true;
    }

    // 生成随机 URL
    std::string url = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);

    // 生成随机 HTTP 头
    std::map<std::string, std::string> additionalHttpHeaders;
    int map_size = dataProvider.ConsumeIntegralInRange<int>(1, MAX_VECTOR_SIZE);
    for (int i = 0; i < map_size; i++) {
        std::string key = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        additionalHttpHeaders[key] = value;
    }

    // 调用被测试函数
    g_nweb->Load(url, additionalHttpHeaders);

    return true;
}
} // namespace OHOS

/* Fuzzer 入口点 */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::LoadUrlFuzzTest(data, size);
    return 0;
}
```

**BUILD.gn**:

```gni
import("//build/test.gni")

ohos_fuzztest("LoadUrlFuzzTest") {
  module_out_path = webview_fuzz_test_path
  fuzz_config_file = "$webview_path/test/fuzztest/loadurl_fuzzer"

  include_dirs = [
    "$webview_path/test/ohos_nweb",
    "$webview_path/test/unittest/common",
  ]

  cflags = [
    "-g",
    "-O0",  # 不优化以便调试
    "-Wno-unused-variable",
    "-fno-omit-frame-pointer",
  ]

  sources = [
    "../../unittest/common/nweb_create_window.cpp",
    "loadurl_fuzzer.cpp",
  ]

  external_deps = [
    "c_utils:utils",
    "hilog:libhilog",
    "webview:libnweb",
  ]
}

group("fuzztest") {
  testonly = true
  deps = [ ":LoadUrlFuzzTest" ]
}
```

### 3. resource/ - 测试资源

测试使用的资源文件:

```
resource/
└── image/
    └── images/    # 测试用图片资源
```

## 工作原理

### 单元测试工作流程

```
1. 编写测试代码
   ↓
2. 编写 BUILD.gn
   ↓
3. GN 构建测试目标
   ↓
4. Ninja 编译测试二进制
   ↓
5. 运行测试
   ↓
6. 收集测试结果
   ↓
7. 生成测试报告
```

#### 1. 测试代码结构

```cpp
// 1. 包含必要的头文件
#include <gtest/gtest.h>
#include "被测试的类.h"

// 2. 定义测试固件 (Test Fixture)
class MyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前初始化
        obj_ = new MyObject();
    }

    void TearDown() override {
        // 测试后清理
        delete obj_;
    }

    MyObject* obj_;
};

// 3. 编写测试用例
TEST_F(MyTest, NormalOperation) {
    EXPECT_NE(obj_, nullptr);
    EXPECT_TRUE(obj_->DoSomething());
}

// 4. 对于需要硬件的测试,使用 HWTEST_F
HWTEST_F(MyTest, HardwareRequired, TestSize.Level1) {
    // 需要硬件的测试
    EXPECT_TRUE(obj_->AccessHardware());
}
```

#### 2. 条件编译

测试代码支持条件编译,与主代码保持一致:

```cpp
// BUILD.gn
config("module_private_config") {
  if (webview_audio_enable) {
    defines = [ "NWEB_AUDIO_ENABLE" ]
  }
}

// 测试代码
#if defined(NWEB_AUDIO_ENABLE)
TEST_F(AudioTest, PlayAudio) {
    // 仅在音频启用时编译
    audio_adapter_->Play();
}
#endif
```

#### 3. Mock 实现

对于不可用的功能,使用 Mock 实现:

```cpp
#ifdef NWEB_AUDIO_ENABLE
    // 真实实现
    auto adapter = std::make_shared<AudioAdapterImpl>();
#else
    // Mock 实现
    auto adapter = std::make_shared<AudioAdapterMock>();
#endif
```

### 模糊测试工作流程

```
1. Fuzzer 生成随机输入
   ↓
2. LLVMFuzzerTestOneInput 接收输入
   ↓
3. FuzzedDataProvider 解析输入
   ↓
4. 调用被测试函数
   ↓
5. 检测崩溃/内存错误
   ↓
6. 保存失败用例到 corpus
   ↓
7. 使用失败用例重新生成输入
```

#### FuzzedDataProvider 使用

`FuzzedDataProvider` 是辅助工具,用于将原始字节转换为有意义的输入:

```cpp
FuzzedDataProvider dataProvider(data, size);

// 消费随机长度字符串
std::string str = dataProvider.ConsumeRandomLengthString(MAX_LEN);

// 消费固定长度字符串
std::string fixedStr = dataProvider.ConsumeBytes(10);

// 消费整数
int intValue = dataProvider.ConsumeIntegral<int>();
int rangeInt = dataProvider.ConsumeIntegralInRange<int>(0, 100);

// 消费布尔值
bool flag = dataProvider.ConsumeBool();

// 消费字节
uint8_t byte = dataProvider.ConsumeIntegral<uint8_t>();

// 剩余字节数
size_t remaining = dataProvider.remaining_bytes();
```

## 构建和运行测试

### 构建测试

#### 1. 构建所有单元测试

```bash
# 在 OpenHarmony 代码仓根目录执行
./build.sh --product-name rk3568 --build-target //base/web/webview/test:unittest
```

#### 2. 构建单个测试

```bash
# 构建音频适配器测试
./build.sh --product-name rk3568 \
  --build-target //base/web/webview/test/unittest/audio_adapter_impl_test:unittest
```

#### 3. 构建所有模糊测试

```bash
./build.sh --product-name rk3568 --build-target //base/web/webview/test:fuzztest
```

#### 4. 构建单个 Fuzzer

```bash
./build.sh --product-name rk3568 \
  --build-target //base/web/webview/test/fuzztest/loadurl_fuzzer:fuzztest
```

### 运行测试

#### 1. 运行单元测试

```bash
# 方法 1: 使用 runtest
runtest --part-name webview --test-case nweb_audio_adapter_test

# 方法 2: 直接执行测试二进制
hdc shell mount -o remount,rw /
hdc shell push out/rk3568/tests/unittest/webview/Multimedia/nweb_audio_adapter_test /data/local/tmp/
hdc shell chmod +x /data/local/tmp/nweb_audio_adapter_test
hdc shell /data/local/tmp/nweb_audio_adapter_test

# 方法 3: 使用 hdc shell
hdc shell
cd /data/local/tmp/
./nweb_audio_adapter_test
```

#### 2. 运行模糊测试

```bash
# 方法 1: 使用 runtest
runtest --part-name webview --test-case LoadUrlFuzzTest --fuzztest

# 方法 2: 直接执行 Fuzzer
hdc shell push out/rk3568/tests/fuzztest/webview/LoadUrlFuzzTest /data/local/tmp/
hdc shell chmod +x /data/local/tmp/LoadUrlFuzzTest

# 运行 Fuzzer (可以指定时间或 corpus)
hdc shell /data/local/tmp/LoadUrlFuzzTest -max_total_time=60

# 使用 corpus 重新运行
hdc shell mkdir /data/local/tmp/corpus
hdc shell push corpus/* /data/local/tmp/corpus/
hdc shell /data/local/tmp/LoadUrlFuzzTest /data/local/tmp/corpus
```

#### 3. 查看测试结果

```bash
# 单元测试输出
# 运行测试后会生成报告
# ========== Test Summary ==========
# Total Tests: 150
# Passed: 148
# Failed: 2
# Skipped: 0

# 模糊测试输出
# Fuzzer 会显示发现的崩溃
# INFO: Seed: 1234567890
# INFO: -max_len is not provided; libFuzzer will not generate inputs larger than 1000 bytes
# INFO: A corpus is not provided, starting from an empty corpus
# #2      INITED cov: 12 ft: 12 corp: 1/1b exec/s: 0 rss: 25Mb
# #1024   NEW    cov: 15 ft: 15 corp: 2/3b lim: 8 exec/s: 0 rss: 25Mb
```

## 测试覆盖范围

### 适配器层测试 (ohos_adapter)

覆盖以下适配器:

- **多媒体**: audio, camera, media, av_codec, av_session
- **网络**: net_connect, net_proxy
- **位置**: location
- **电源**: battery_manager, power_manager
- **显示**: display_manager, graphic, surface
- **输入**: mmi, imf
- **存储**: database, keystore, share_preferences
- **剪贴板**: pasteboard
- **打印**: print_manager
- **证书**: cert_manager
- **传感器**: sensor
- **时间**: date_time_format
- **事件**: event_handler, hisysevent
- **其他**: access_token, background, data_share, enterprise, screen_capture, screen_lock, soc_perf, system_properties

### 核心引擎测试 (ohos_nweb)

- **NWebHelper**: 引擎初始化、配置
- **NWeb**: 核心功能 (Load, JavaScript, GoBack, Forward 等)
- **CookieManager**: Cookie 管理
- **WebStorage**: Web 存储
- **Database**: 数据库访问
- **HiSysEvent**: 系统事件上报

### 接口层测试

- **NAPI 接口**: JavaScript 绑定测试
- **Native 接口**: C++ NDK 接口测试
- **NativeCommon**: 通用组件测试

### 系统服务测试

- **app_fwk_update**: 应用框架更新
- **web_native_messaging**: Web 消息服务

## 测试最佳实践

### 1. 测试命名

#### 目录命名

- 使用描述性名称
- 格式: `<模块名>_test` 或 `<模块名>_impl_test`
- 示例: `audio_adapter_impl_test`, `nweb_helper_test`

#### 测试类命名

- 格式: `<被测试类>Test`
- 示例: `AudioAdapterImplTest`, `NWebHelperTest`

#### 测试用例命名

- 格式: `TEST_F(<测试类名>, <测试场景>)`
- 描述性命名,说明测试什么
- 示例:
  - `TEST_F(AudioTest, PlayAudio)`
  - `TEST_F(NWebTest, LoadUrlWithInvalidUrl)`
  - `TEST_F(CookieTest, SetCookieWithExpiry)`

### 2. 测试结构

```cpp
class MyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 1. 初始化被测试对象
        obj_ = std::make_shared<MyClass>();

        // 2. 准备测试数据
        test_data_ = "test";
    }

    void TearDown() override {
        // 3. 清理资源
        obj_.reset();
    }

    // 4. 测试辅助方法
    void PrepareTestEnvironment();

    std::shared_ptr<MyClass> obj_;
    std::string test_data_;
};

TEST_F(MyTest, NormalCase) {
    // Arrange (准备)
    std::string input = "valid_input";

    // Act (执行)
    bool result = obj_->Process(input);

    // Assert (断言)
    EXPECT_TRUE(result);
}
```

### 3. 断言使用

#### EXPECT_* vs ASSERT_*

```cpp
// EXPECT_*: 失败后继续执行
EXPECT_EQ(obj_->GetValue(), 10);
EXPECT_TRUE(obj_->IsValid());

// ASSERT_*: 失败后立即停止
ASSERT_NE(obj_, nullptr);  // 确保对象有效
obj_->DoSomething();
```

#### 常用断言

```cpp
// 布尔值
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);

// 相等性
EXPECT_EQ(expected, actual);
EXPECT_NE(val1, val2);

// 比较
EXPECT_LT(val1, val2);  // Less than
EXPECT_LE(val1, val2);  // Less or equal
EXPECT_GT(val1, val2);  // Greater than
EXPECT_GE(val1, val2);  // Greater or equal

// 指针
EXPECT_NE(ptr, nullptr);
EXPECT_EQ(ptr, expected_ptr);

// 浮点数
EXPECT_FLOAT_EQ(float1, float2);
EXPECT_NEAR(val1, val2, 0.001);  // 允许误差

// 字符串
EXPECT_STREQ(str1, str2);
EXPECT_STRCASEEQ(str1, str2);  // 忽略大小写
```

### 4. 测试覆盖

#### 正常路径

```cpp
TEST_F(MyTest, NormalOperation) {
    EXPECT_TRUE(obj_->DoSomething("valid_input"));
}
```

#### 边界条件

```cpp
TEST_F(MyTest, BoundaryConditions) {
    // 空输入
    EXPECT_FALSE(obj_->DoSomething(""));

    // 最大值
    EXPECT_TRUE(obj_->SetValue(INT_MAX));

    // 最小值
    EXPECT_TRUE(obj_->SetValue(INT_MIN));
}
```

#### 异常情况

```cpp
TEST_F(MyTest, ErrorHandling) {
    // 无效输入
    EXPECT_FALSE(obj_->DoSomething(nullptr));

    // 超出范围
    EXPECT_FALSE(obj_->SetValue(INT_MAX + 1));
}
```

### 5. Mock 使用

对于依赖外部系统的功能,使用 Mock:

```cpp
class MockAudioSystem : public AudioSystem {
public:
    MOCK_METHOD(bool, Play, (), (override));
    MOCK_METHOD(void, Stop, (), (override));
};

TEST_F(AudioTest, UsingMock) {
    auto mockAudio = std::make_shared<MockAudioSystem>();

    // 设置预期行为
    EXPECT_CALL(*mockAudio, Play())
        .Times(1)
        .WillOnce(Return(true));

    // 使用 Mock 测试
    audio_adapter_->SetAudioSystem(mockAudio);
    EXPECT_TRUE(audio_adapter_->PlayAudio());
}
```

### 6. 参数化测试

使用相同逻辑测试多组数据:

```cpp
class LoadUrlTest : public ::testing::TestWithParam<std::string> {
protected:
    std::shared_ptr<NWeb> nweb_;
};

TEST_P(LoadUrlTest, LoadVariousUrls) {
    std::string url = GetParam();
    EXPECT_TRUE(nweb_->Load(url));
}

INSTANTIATE_TEST_SUITE_P(
    ValidUrls,
    LoadUrlTest,
    ::testing::Values(
        "https://www.example.com",
        "http://test.org",
        "file:///data/local/file.html"
    )
);
```

### 7. 条件编译测试

确保测试代码与主代码同步:

```cpp
// BUILD.gn
config("module_private_config") {
  if (webview_audio_enable) {
    defines = [ "NWEB_AUDIO_ENABLE" ]
  }
}

// 测试代码
#if defined(NWEB_AUDIO_ENABLE)
TEST_F(AudioTest, PlayAudio) {
    EXPECT_TRUE(audio_adapter_->Play());
}
#endif

TEST_F(AudioTest, CheckAvailability) {
#ifdef NWEB_AUDIO_ENABLE
    EXPECT_TRUE(audio_adapter_->IsAvailable());
#else
    EXPECT_FALSE(audio_adapter_->IsAvailable());
#endif
}
```

### 8. 测试隔离

每个测试应该独立:

```cpp
TEST_F(MyTest, Test1) {
    // 不要依赖 Test2 的执行顺序
    // 不要依赖 Test2 修改的状态
    // 每个测试在 SetUp 中初始化,在 TearDown 中清理
}
```

### 9. 资源清理

确保资源正确释放:

```cpp
class MyTest : public ::testing::Test {
protected:
    void TearDown() override {
        // 清理文件
        if (access(test_file_.c_str(), F_OK) == 0) {
            unlink(test_file_.c_str());
        }

        // 清理对象
        obj_.reset();

        // 断开连接
        if (connection_) {
            connection_->Close();
        }
    }
};
```

### 10. 测试性能

对于性能关键代码,使用性能测试:

```cpp
TEST_F(PerformanceTest, LoadUrlPerformance) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++) {
        nweb_->Load("https://example.com");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_LT(duration.count(), 5000);  // 100次加载应在5秒内完成
}
```

## 调试测试

### 1. 启用详细输出

```bash
# 运行测试时启用详细输出
hdc shell /data/local/tmp/my_test --gtest_verbose=1

# 仅运行特定测试
hdc shell /data/local/tmp/my_test --gtest_filter=MyTest.TestName
```

### 2. 调试模式构建

```bash
# 构建 Debug 版本
./build.sh --product-name rk3568 \
  --build-target //base/web/webview/test/unittest/my_test:unittest \
  --gn-args="is_debug=true"
```

### 3. 使用日志

```cpp
#include "hilog/log.h"

TEST_F(MyTest, DebugTest) {
    WVLOG_I("Starting test");

    bool result = obj_->DoSomething();
    WVLOG_I("Result: %{public}d", result);

    EXPECT_TRUE(result);
}
```

### 4. GDB 调试

```bash
# 在设备上使用 gdb
hdc shell
gdb /data/local/tmp/my_test
(gdb) break MyTest::TestName
(gdb) run
```

## 常见问题

### Q1: 测试编译失败,找不到依赖?

**原因**: BUILD.gn 中未声明依赖或 external_deps

**解决**:
```gni
deps = [
  "$webview_path/ohos_adapter:nweb_ohos_adapter",
]

external_deps = [
  "c_utils:utils",
  "hilog:libhilog",
  "googletest:gtest_main",
]
```

### Q2: 测试运行时崩溃?

**原因**: 未初始化、空指针、依赖不可用

**解决**:
- 确保 SetUp 中正确初始化
- 使用 `#ifdef` 检查功能可用性
- 查看 `hilog -T ArkWeb` 日志

### Q3: HWTEST_F 测试跳过?

**原因**: 缺少硬件支持

**解决**:
- 使用 `--gtest_also_run_disabled_tests` 强制运行
- 或在真实设备上运行

### Q4: Fuzzer 内存不足?

**原因**: rss_limit_mb 限制太低

**解决**: 修改 project.xml
```xml
<rss_limit_mb>8192</rss_limit_mb>
```

### Q5: 如何跳过某个测试?

```cpp
TEST_F(MyTest, DisabledTest) {
    GTEST_SKIP() << "Skipping this test";
    // 不会执行
}

// 运行时跳过
hdc shell /data/local/tmp/my_test --gtest_filter=-MyTest.DisabledTest
```

## 相关文档

- **[../AGENT.md](../AGENT.md)** - 项目概述
- **[../ohos_adapter/README.md](../ohos_adapter/README.md)** - 适配器层文档
- **[../ohos_nweb/README.md](../ohos_nweb/README.md)** - 核心引擎文档
- **[../interfaces/kits/napi/README.md](../interfaces/kits/napi/README.md)** - NAPI 接口文档
- **[gtest 文档](https://google.github.io/googletest/)** - 测试框架文档
- **[LLVM Fuzzer 文档](https://llvm.org/docs/LibFuzzer.html)** - Fuzzer 框架文档
- **[FuzzedDataProvider](https://github.com/google/fuzzing/blob/master/docs/structure-aware-fuzzing.md)** - Fuzzer 辅助工具

## 测试命令速查

```bash
# 构建所有单元测试
./build.sh --product-name rk3568 --build-target //base/web/webview/test:unittest

# 构建所有模糊测试
./build.sh --product-name rk3568 --build-target //base/web/webview/test:fuzztest

# 运行单元测试
runtest --part-name webview --test-case nweb_audio_adapter_test

# 运行模糊测试
runtest --part-name webview --test-case LoadUrlFuzzTest --fuzztest

# 查看测试日志
hilog -T ArkWeb

# 列出所有测试
hdc shell /data/local/tmp/my_test --gtest_list_tests
```
