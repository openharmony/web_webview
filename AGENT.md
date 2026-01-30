# AGENT.md - ArkWeb Project Guide

## Project Overview

This repository is the **core implementation of the ArkWeb component for OpenHarmony system**, providing WebView component capabilities. ArkWeb is a WebView Native engine built based on Chromium and CEF (Chromium Embedded Framework).

## Build and Test Commands

### Build System
This project uses **GN (Generate Ninja)** + **Ninja** build system.

### Full Build Commands
```bash
# Standard full build (execute in OpenHarmony code repository root)
./build.sh --product-name <product_name> --build-target webview
./build.sh --product-name rk3568 --build-target webview  # Example: RK3568 product

# Build specific modules only
./build.sh --product-name <product_name> --build-target //base/web/webview/ohos_nweb:libnweb
./build.sh --product-name <product_name> --build-target //base/web/webview/ohos_nweb:nweb_hap
```

### Unit Test Commands
```bash
# Run all unit tests
./build.sh --product-name <product_name> --build-target webview_test

# Run specific unit tests
./build.sh --product-name <product_name> --build-target //base/web/webview/test/unittest/nweb_helper_test:unittest
./build.sh --product-name <product_name> --build-target //base/web/webview/test/unittest/napi_web_cookie_manager_test:unittest

# Install and run tests (requires prior build)
# Test output path: out/<product_name>/tests/webview/
```

### Fuzz Test Commands
```bash
# Build fuzz tests
./build.sh --product-name <product_name> --build-target //base/web/webview/test/fuzztest/load_fuzzer:fuzztest
./build.sh --product-name <product_name> --build-target //base/web/webview/test/fuzztest/executejavascript_fuzzer:fuzztest

# Run fuzz tests (requires OpenHarmony test framework)
# Test binaries located at: out/<product_name>/tests/fuzztest/
```

## Core Architecture

### Overall Layered Architecture

```
Application Layer (ArkTS/JS/CJ/C++)
  ↓ interfaces/ (NAPI/Native/CJ/ANI)
  ↓
ohos_nweb/ Core Engine Layer
  ↓ ohos_interface/include/ohos_nweb/ (Glue Layer: 53+ interfaces)
  ↓
ArkWebCore.hap (Chromium Engine)
  ↓ ohos_interface/include/ohos_adapter/ (Glue Layer: 66+ interfaces)
  ↓
ohos_adapter/ System Adapter Layer (40+ adapters)
  ↓
OpenHarmony System Service Layer
```

**Core Innovation**: `ohos_interface/include/` glue layer implements bidirectional decoupling
- **ohos_nweb/** (System side → Kernel, 53+ interfaces): ✅ New additions allowed
- **ohos_adapter/** (Kernel → System side, 66+ interfaces): ❌ New additions NOT allowed

See: [ohos_interface/README.md](./ohos_interface/README.md)

### Core Components Description

| Component | Description | Detailed Documentation |
|-----------|-------------|----------------------|
| **ohos_nweb/** | Core engine layer, provides NWebHelper, SurfaceAdapter, etc. | [ohos_nweb/README.md](./ohos_nweb/README.md) |
| **ohos_adapter/** | System adapter layer, 40+ adapters connect to OpenHarmony system | [ohos_adapter/README.md](./ohos_adapter/README.md) |
| **ohos_interface/** | Glue layer interface definitions, key to bidirectional decoupling | [ohos_interface/README.md](./ohos_interface/README.md) |
| **ohos_wrapper/** | Wrapper layer, depends on OpenHarmony system | [ohos_wrapper/README.md](./ohos_wrapper/README.md) |
| **arkweb_utils/** | Utility library, configuration parsing and other common functions | [arkweb_utils/README.md](./arkweb_utils/README.md) |
| **interfaces/** | Multi-language interfaces (NAPI, Native, CJ, ANI) | NAPI: [interfaces/kits/napi/README.md](interfaces/kits/napi/README.md)<br/>ANI: [interfaces/kits/ani/README.md](interfaces/kits/ani/README.md)<br/>CJ API: [interfaces/kits/cj/README.md](interfaces/kits/cj/README.md)<br/>Native API: [interfaces/native/README.md](interfaces/native/README.md) |
| **sa/** | System services (app_fwk_update, web_native_messaging) | [sa/app_fwk_update/README.md](./sa/app_fwk_update/README.md)<br/>[sa/web_native_messaging/README.md](./sa/web_native_messaging/README.md) |
| **test/** | Test code (unittest, fuzztest) | [test/README.md](test/README.md) |

### Important Directories and Files

**Interface Layer** - Provides multi-language bindings for ArkWeb
- **interfaces/kits/napi/** - NAPI binding for ArkTS/JavaScript
- **interfaces/kits/nativecommon/** - Shared components for NAPI/ANI (with reference counting)
- **interfaces/kits/ani/** - ANI binding for high-performance ArkTS
- **interfaces/kits/cj/** - CJ FFI binding for Rust/Python/C++
- **interfaces/native/** - Native C++ NDK interface
- **ohos_interface/include/ohos_nweb/** - 53+ NWeb interfaces (✅ allows new additions)
- **ohos_interface/include/ohos_adapter/** - 66+ Adapter interfaces (❌ does NOT allow new additions)

**Core Files**
- **ohos_nweb/etc/web_config.xml** - WebView core configuration (renderer process count, cache, media, etc.)
- **ohos_nweb/etc/para/web.para** - Runtime adjustable parameters
- **ohos_nweb/include/nweb_helper.h** - Core helper class (engine initialization, WebView creation)
- **config.gni** - Global feature switches (audio, location, camera, media, etc.)

**Other Directories**
- **interfaces/kits/** - Multi-language interfaces (NAPI, Native, CJ, ANI)
- **ohos_glue/** - Bridge code between interface layer and glue layer
- **sa/** - System services (app_fwk_update, web_native_messaging)
- **test/** - Test code (unittest, fuzztest)

## Development Workflow

### Common Development Scenarios

#### 1. Adding New Public API (Application Layer Interface)

Providing new WebView functionality APIs for ArkTS/JS/C++/Rust/Python and other languages, including the following steps:

1. **[Glue Layer Interface Definition](ohos_interface/README.md)** (`ohos_interface/include/ohos_nweb/`) - Define interface classes
2. **[Core Engine Implementation](ohos_nweb/README.md)** (`ohos_nweb/`) - Implement specific functionality
3. **Multi-language Bindings**:
   - **[NAPI Binding](interfaces/kits/napi/README.md)** (`interfaces/kits/napi/`) - ArkTS interface
   - **[Native Binding](interfaces/native/README.md)** (`interfaces/native/`) - C/C++ NDK interface
   - **[ANI Binding](interfaces/kits/ani/README.md)** (`interfaces/kits/ani/`) - High-performance ArkTS Static interface
   - **[CJ FFI Binding](interfaces/kits/cj/README.md)** (`interfaces/kits/cj/`) - CJ interface
4. **Testing** (`test/unittest/`) - Unit tests
5. **Build Configuration** - Update BUILD.gn and bundle.json

See: **[interfaces/kits/napi/README.md](./interfaces/kits/napi/README.md)**

#### 2. Adding Glue Layer API (ohos_adapter Direction)

Adding adapter interfaces for ArkWebCore.hap to access OpenHarmony system services:

1. **Glue Layer Interface Definition** (`ohos_interface/include/ohos_adapter/`) - Define interface classes
2. **Adapter Implementation** (`ohos_adapter/`) - Implement system service calls
3. **Core Engine Integration** (`ohos_nweb/`) - Use adapter interfaces

⚠️ **Important Constraint**: ohos_adapter interfaces **do NOT allow new additions**, functionality can only be extended within existing interfaces

#### 3. Adding Configuration Items

ArkWeb supports two configuration methods, **PARAM parameters are recommended**:

##### 3.1 PARAM Parameter Configuration (Runtime Configuration) ✅ Recommended

- **File Location**: `ohos_nweb/etc/para/web.para`
- **Permission Configuration**: `ohos_nweb/etc/para/web.para.dac`
- **Code Reading**: Use functions like `OHOS::system::GetBoolParameter()`
- **Runtime Modification**: `param set web.performance.turbo true`
- **Advantages**: Runtime modifiable, immediate effect, easy debugging

See: **[ohos_nweb/HOW_TO_ADD_PARAM_CONFIG.md](./ohos_nweb/HOW_TO_ADD_PARAM_CONFIG.md)**

##### 3.2 XML Configuration (Compile-time Configuration)

- **File Location**: `ohos_nweb/etc/web_config.xml`
- **Configuration Mapping**: configMap in `ohos_nweb/src/nweb_config_helper.cpp`
- **Applicable Scenarios**: Core architecture configuration, configurations that must be determined at startup
- **Disadvantages**: Requires recompilation, low flexibility

See: **[ohos_nweb/HOW_TO_ADD_XML_CONFIG.md](./ohos_nweb/HOW_TO_ADD_XML_CONFIG.md)**

#### 4. Adding nativecommon Common Components

Adding shared data structure encapsulation for NAPI and ANI interfaces:

1. **Define Data Structures** (`interfaces/kits/nativecommon/`) - Inherit from TransferReferencedCount
2. **NAPI Usage** - Use napi_wrap to wrap, automatically manage reference counting
3. **ANI Usage** - Use SetNativePointer to wrap, automatically manage reference counting

See: **[interfaces/kits/nativecommon/README.md](./interfaces/kits/nativecommon/README.md)**

#### 5. Adding Test Cases

Adding unit tests and fuzz tests for new functionality:

1. **Unit Tests** (`test/unittest/`) - Using gtest framework
   - Create test directory: `test/unittest/<module_name>_test/`
   - Write test class inheriting from `::testing::Test`
   - Implement `SetUp()` and `TearDown()`
   - Write test cases using `TEST_F()` or `HWTEST_F()`
   - Write BUILD.gn to configure test targets

2. **Fuzz Tests** (`test/fuzztest/`) - Using LLVM Fuzzer framework
   - Create fuzzer directory: `test/fuzztest/<feature_name>_fuzzer/`
   - Implement `LLVMFuzzerTestOneInput()` function
   - Use `FuzzedDataProvider` to parse random input
   - Configure project.xml to set runtime parameters
   - Write BUILD.gn to configure fuzzer targets

3. **Common Code** (`test/unittest/common/`) - Reuse test helper code
   - `nweb_create_window.h/cpp` - Window creation helper functions
   - Test Fixture
   - Mock classes and helper functions

See: **[test/README.md](./test/README.md)**

## Debugging Guide

### Log Viewing
```bash
# View WebView related logs
hilog -T ArkWeb
hilog -T webadapter

# View configuration
cat /etc/web/web_config.xml
param get web.*

# Check processes
ps -A | grep arkweb

# SA memory analysis
hidumper -s 8610 -a --memory
```

### Problem Localization

**Compilation Errors**:
- Check if required dependencies are declared in `bundle.json`
- Check `deps` and `external_deps` in `BUILD.gn`
- Confirm precompiled files for target architecture (arm64/arm/x86_64) exist

**Runtime Crashes**:
- Use `hilog` to view logs: `hilog -T ArkWeb`
- Check if `arkweb_crashpad_handler` is working properly
- Check if `ohos_nweb/etc/web_config.xml` configuration is correct
- Crash file location: `/data/log/crash/`

**Functional Abnormalities**:
- Use HiTrace from `hiviewdfx_adapter` for chain tracing
- Check HiSysEvent event reporting from `hiviewdfx_adapter`
- View event definitions in `ohos_nweb/src/nweb_hisysevent.cpp`

## Important Notes

- **Adapter Pattern**: All system interactions are decoupled through `ohos_adapter/`, facilitating porting and testing
- **Multi-process Architecture**: Renderer/GPU/Utility processes are managed by Chromium and controlled through `web_config.xml`
- **Crash Handling**: `arkweb_crashpad_handler` captures crashes and generates minidump to `/data/log/crash/`
