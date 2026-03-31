#!/bin/bash
#
# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Bridge文件生成脚本
# 用法: ./bridge_gen.sh [module_name] [--log-dir log_dir] [--stamp stamp_file]
# 参数: module_name - nweb, adapter（不指定则生成所有）
#       --log-dir   - 日志目录路径（用于输出执行日志到 prepare.log）
#       --stamp     - stamp 文件路径（用于 GN 构建系统）
#
# 使用示例:
#   1. 开发调试（输出日志）: ./bridge_gen.sh --log-dir /tmp/log
#   2. GN 构建（生成 stamp）: ./bridge_gen.sh --stamp out/rk3568/.bridge_stamp
#   3. 生成所有模块: ./bridge_gen.sh
#   4. 生成特定模块: ./bridge_gen.sh nweb --log-dir /tmp/log

set -e

# 脚本所在目录
SCRIPT_DIR=$(cd "$(dirname "$0")"; pwd)
# INTERFACE_DIR 是 ohos_interface 目录
INTERFACE_DIR=$(cd "${SCRIPT_DIR}/../.."; pwd)

INTERFACE_OHOS_GLUE_DIR="${INTERFACE_DIR}/ohos_glue"
CONFIG_FILE="${INTERFACE_OHOS_GLUE_DIR}/scripts/bridge_generation.conf.json"

# 解析参数
MODULE=""
LOG_DIR=""
STAMP_FILE=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --stamp)
      STAMP_FILE="$2"
      shift 2
      ;;
    --log-dir)
      LOG_DIR="$2"
      shift 2
      ;;
    *)
      if [ -z "$MODULE" ]; then
        MODULE="$1"
      fi
      shift
      ;;
  esac
done

# 日志函数
log_message() {
  local message="$1"
  if [ -n "$LOG_DIR" ]; then
    echo "$message" >> "${LOG_DIR}/prepare.log"
  fi
  # 同时输出到控制台
  echo "$message"
}

# 记录开始日志
log_message "========================================"
log_message "开始执行 bridge_gen.sh..."
log_message "模块: ${MODULE:-所有模块}"
log_message "时间: $(date '+%Y-%m-%d %H:%M:%S')"
if [ -n "$LOG_DIR" ]; then
  log_message "日志目录: $LOG_DIR"
fi
if [ -n "$STAMP_FILE" ]; then
  log_message "Stamp 文件: $STAMP_FILE"
fi
log_message "========================================"

# 如果没有指定模块，生成所有模块
if [ -z "$MODULE" ]; then
  MODULES="nweb adapter"
else
  MODULES="$MODULE"
fi

# 生成桥接文件的函数
generate_bridge() {
  local module=$1
  local bridge_system=""

  # 模块名到 bridge_system 的映射
  case "${module}" in
    nweb|webcore) bridge_system="ohos_nweb" ;;
    adapter|webview) bridge_system="ohos_adapter" ;;
    *)
      log_message "Error: Unknown module '${module}'" >&2
      return 1
    ;;
  esac

  if [ ! -f "$CONFIG_FILE" ]; then
    log_message "Error: Config file not found: $CONFIG_FILE" >&2
    return 1
  fi

  log_message "Generating bridge files for ${bridge_system}..."

  (
    cd "${INTERFACE_DIR}"
    python3 "${INTERFACE_OHOS_GLUE_DIR}/scripts/bridge_gen_files.py" \
      --config "$CONFIG_FILE" \
      --output "$INTERFACE_OHOS_GLUE_DIR" \
      --system "$bridge_system" \
      --base-dir "." 2>&1
  ) | while IFS= read -r line; do
    log_message "$line"
  done

  log_message "Bridge files generation completed for ${bridge_system}"
}

# 遍历所有模块并生成
for module in $MODULES; do
  generate_bridge "$module"
done

# 记录结束日志
log_message "========================================"
log_message "bridge_gen.sh 执行完成"
log_message "时间: $(date '+%Y-%m-%d %H:%M:%S')"
log_message "========================================"

# 生成 stamp 文件（如果指定了）
if [ -n "$STAMP_FILE" ]; then
  mkdir -p "$(dirname "$STAMP_FILE")"
  echo "Generated bridge files for $MODULES" > "$STAMP_FILE"
  log_message "Stamp 文件已生成: $STAMP_FILE"
fi
