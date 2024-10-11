/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_web_permission_data_base_adapter_impl.h"

#include <cinttypes>
#include <unistd.h>

#include "nweb_log.h"
#include <AbilityKit/ability_runtime/application_context.h>

using namespace OHOS::NWeb;

namespace {
const int32_t RDB_VERSION = 1;
const std::string WEB_PERMISSION_DATABASE_FILE = "web_permission.db";

const std::string ID_COL = "_id";
const std::string GEOLOCATION_TABLE_NAME = "geolocation";
const std::string PERMISSION_ORIGIN_COL = "origin";
const std::string PERMISSION_RESULT_COL = "result";

const std::string CREATE_TABLE = "CREATE TABLE " + GEOLOCATION_TABLE_NAME
    + " (" + ID_COL + " INTEGER PRIMARY KEY, "
    + PERMISSION_ORIGIN_COL + " TEXT, " + PERMISSION_RESULT_COL + " INTEGER, "
    + " UNIQUE (" + PERMISSION_ORIGIN_COL + ") ON CONFLICT REPLACE);";

const std::string WEB_PATH = "/web";

const std::unordered_map<AbilityRuntime_AreaMode, Rdb_SecurityArea> AREA_MODE_MAP = {
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL1, Rdb_SecurityArea::RDB_SECURITY_AREA_EL1 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL2, Rdb_SecurityArea::RDB_SECURITY_AREA_EL2 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL3, Rdb_SecurityArea::RDB_SECURITY_AREA_EL3 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL4, Rdb_SecurityArea::RDB_SECURITY_AREA_EL4 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL5, Rdb_SecurityArea::RDB_SECURITY_AREA_EL5 }
};
}

OhosWebPermissionDataBaseAdapterImpl& OhosWebPermissionDataBaseAdapterImpl::GetInstance(const std::string& userPath)
{
    WVLOG_I("webdatabase get permission data base instance");
    static OhosWebPermissionDataBaseAdapterImpl instance(userPath);
    return instance;
}

Rdb_SecurityArea OhosWebPermissionDataBaseAdapterImpl::GetAreaMode(AbilityRuntime_AreaMode areaMode)
{
    auto mode = AREA_MODE_MAP.find(areaMode);
    return mode->second;
}

void OhosWebPermissionDataBaseAdapterImpl::GetOrOpen(const OH_Rdb_Config *config, int *errCode)
{
    rdbStore_ = OH_Rdb_GetOrOpen(config, errCode);
    if (rdbStore_ == nullptr) {
        WVLOG_E("web permission database get rdb store failed, errCode=%{public}d", *errCode);
        return;
    }

    int version = RDB_VERSION;
    if (OH_Rdb_GetVersion(rdbStore_, &version) != RDB_OK) {
        WVLOG_E("web permission database get rdb version failed");
        return;
    }

    if (version == 0) {
        if (OH_Rdb_Execute(rdbStore_, CREATE_TABLE.c_str()) != RDB_OK) {
            WVLOG_E("web permission database create table failed");
            return;
        }
        if (OH_Rdb_SetVersion(rdbStore_, RDB_VERSION) != RDB_OK) {
            WVLOG_E("web permission database set version failed");
            return;
        }
        WVLOG_I("web permission database create rdb succeed");
    } else {
        WVLOG_I("web permission database already exists");
    }
}

OhosWebPermissionDataBaseAdapterImpl::OhosWebPermissionDataBaseAdapterImpl(const std::string& userPath)
{
    WVLOG_I("web permission database create rdb store");

    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    constexpr int32_t NATIVE_BUFFER_SIZE = 1024;

    if (access(userPath.c_str(), F_OK) != 0) {
        WVLOG_E("web permission fail to access cache web dir:%{public}s", userPath.c_str());
        return;
    }

    char bundleName[NATIVE_BUFFER_SIZE];
    int32_t bundleNameLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetBundleName(bundleName, NATIVE_BUFFER_SIZE, &bundleNameLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextGetBundleName failed:err=%{public}d", code);
        return;
    }

    AbilityRuntime_AreaMode areaMode = ABILITY_RUNTIME_AREA_MODE_EL2;
    code = OH_AbilityRuntime_ApplicationContextGetAreaMode(&areaMode);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextGetAreaMode failed:err=%{public}d", code);
        return;
    }

    std::string name = WEB_PERMISSION_DATABASE_FILE;
    OH_Rdb_Config config = {0};
    config.selfSize = sizeof(OH_Rdb_Config);
    config.dataBaseDir = userPath.c_str();
    config.bundleName = bundleName;
    config.storeName = name.c_str();
    config.area = GetAreaMode(areaMode);
    config.securityLevel = OH_Rdb_SecurityLevel::S3;
    WVLOG_I("web permission database databaseDir=%{public}s", userPath.c_str());
    WVLOG_I("web permission database bundleName=%{public}s", bundleName);

    int errCode = static_cast<int>(RDB_OK);
    GetOrOpen(&config, &errCode);
}

OhosWebPermissionDataBaseAdapterImpl::~OhosWebPermissionDataBaseAdapterImpl()
{
    int errCode = OH_Rdb_CloseStore(rdbStore_);
    if (errCode == RDB_OK) {
        WVLOG_I("web permission database delete rdb succeed");
    } else {
        WVLOG_E("web permission database delete rdb failed");
    }
}

std::string OhosWebPermissionDataBaseAdapterImpl::KeyToTableName(const WebPermissionType& key) const
{
    if (key == WebPermissionType::GEOLOCATION) {
        return GEOLOCATION_TABLE_NAME;
    }
    return "";
}

bool OhosWebPermissionDataBaseAdapterImpl::ExistPermissionByOrigin(
    const std::string& origin, const WebPermissionType& key)
{
    WVLOG_I("web permission database check exist permissions origin=%{public}s info", origin.c_str());
    if (rdbStore_ == nullptr || origin.empty()) {
        return false;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return false;
    }

    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(tableName.c_str());
    OH_VObject *valueObject = OH_Rdb_CreateValueObject();
    valueObject->putText(valueObject, origin.c_str());
    dirAbsPred->equalTo(dirAbsPred, PERMISSION_ORIGIN_COL.c_str(), valueObject);
    valueObject->destroy(valueObject);

    OH_Cursor *cursor = OH_Rdb_Query(rdbStore_, dirAbsPred, NULL, 0);
    dirAbsPred->destroy(dirAbsPred);

    if ((cursor == nullptr) || (cursor->goToNextRow(cursor) != RDB_OK)) {
        WVLOG_E("web permissions database rdb store query failed");
        if (cursor != nullptr) {
            cursor->destroy(cursor);
        }
        return false;
    }
    cursor->destroy(cursor);
    return true;
}

bool OhosWebPermissionDataBaseAdapterImpl::GetPermissionResultByOrigin(const std::string& origin,
    const WebPermissionType& key, bool& result)
{
    WVLOG_I("web permission database get permissions origin=%{public}s info", origin.c_str());
    if (rdbStore_ == nullptr || origin.empty()) {
        return false;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return false;
    }

    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(tableName.c_str());
    OH_VObject *valueObject = OH_Rdb_CreateValueObject();
    valueObject->putText(valueObject, origin.c_str());
    dirAbsPred->equalTo(dirAbsPred, PERMISSION_ORIGIN_COL.c_str(), valueObject);
    valueObject->destroy(valueObject);

    OH_Cursor *cursor = OH_Rdb_Query(rdbStore_, dirAbsPred, NULL, 0);
    dirAbsPred->destroy(dirAbsPred);

    if ((cursor == nullptr) || (cursor->goToNextRow(cursor) != RDB_OK)) {
        WVLOG_E("web permissions database rdb store query failed");
        if (cursor != nullptr) {
            cursor->destroy(cursor);
        }
        return false;
    }

    int columnIndex;
    int64_t resultVal = 0;
    cursor->getColumnIndex(cursor, PERMISSION_RESULT_COL.c_str(), &columnIndex);
    cursor->getInt64(cursor, columnIndex, &resultVal);
    result = resultVal ? true : false;
    cursor->destroy(cursor);
    return true;
}

void OhosWebPermissionDataBaseAdapterImpl::SetPermissionByOrigin(const std::string& origin,
    const WebPermissionType& key, bool result)
{
    WVLOG_I("web permission database set info origin:%{public}s key:%{public}d", origin.c_str(), key);
    if (rdbStore_ == nullptr || origin.empty()) {
        return;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    int errCode;
    OH_VBucket *valueBucket = OH_Rdb_CreateValuesBucket();
    valueBucket->clear(valueBucket);
    valueBucket->putText(valueBucket, PERMISSION_ORIGIN_COL.c_str(), origin.c_str());
    valueBucket->putInt64(valueBucket, PERMISSION_RESULT_COL.c_str(), (int64_t)result);
    errCode = OH_Rdb_Insert(rdbStore_, tableName.c_str(), valueBucket);
    valueBucket->destroy(valueBucket);
    if (errCode == RDB_ERR || errCode == RDB_E_INVALID_ARGS) {
        WVLOG_E("web permission database set info failed, errCode=%{public}d", errCode);
        return;
    }
    WVLOG_I("web permission database set info end");
}

void OhosWebPermissionDataBaseAdapterImpl::ClearPermissionByOrigin(const std::string& origin,
    const WebPermissionType& key)
{
    if (rdbStore_ == nullptr || origin.empty()) {
        return;
    }
    WVLOG_I("web permission database clear origin:%{public}s info", origin.c_str());
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(tableName.c_str());
    OH_VObject *valueObject = OH_Rdb_CreateValueObject();
    valueObject->putText(valueObject, origin.c_str());
    dirAbsPred->equalTo(dirAbsPred, PERMISSION_ORIGIN_COL.c_str(), valueObject);
    int ret = OH_Rdb_Delete(rdbStore_, dirAbsPred);
    if (ret != RDB_ERR && ret != RDB_E_INVALID_ARGS) {
        WVLOG_I("web permission database clear succ: deleted rows=%{public}d", ret);
    } else {
        WVLOG_E("web permission database clear fail: ret=%{public}d", ret);
    }
    dirAbsPred->destroy(dirAbsPred);
    valueObject->destroy(valueObject);
}

void OhosWebPermissionDataBaseAdapterImpl::ClearAllPermission(const WebPermissionType& key)
{
    if (rdbStore_ == nullptr) {
        return;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    WVLOG_I("web permission database clear all permission:%{public}s info", tableName.c_str());

    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(tableName.c_str());
    int ret = OH_Rdb_Delete(rdbStore_, dirAbsPred);
    if (ret != RDB_ERR && ret != RDB_E_INVALID_ARGS) {
        WVLOG_I("web permission database clear all succ: deleted rows=%{public}d", ret);
    } else {
        WVLOG_E("web permission database clear all fail: ret=%{public}d", ret);
    }
    dirAbsPred->destroy(dirAbsPred);
}

void OhosWebPermissionDataBaseAdapterImpl::GetOriginsByPermission(const WebPermissionType& key,
    std::vector<std::string>& origins)
{
    if (rdbStore_ == nullptr) {
        return;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(tableName.c_str());
    OH_Cursor *cursor = OH_Rdb_Query(rdbStore_, dirAbsPred, NULL, 0);
    if ((cursor == nullptr) || (cursor->goToNextRow(cursor) != RDB_OK)) {
        WVLOG_E("web permissions database rdb store query failed");
        if (cursor != nullptr) {
            cursor->destroy(cursor);
        }
        return;
    }

    int columnIndex;
    cursor->getColumnIndex(cursor, PERMISSION_ORIGIN_COL.c_str(), &columnIndex);
    WVLOG_I("web permission database origin columnIndex:%{public}d", columnIndex);
    do {
        size_t size = 0;
        cursor->getSize(cursor, columnIndex, &size);
        char *origin = new char[size + 1]();
        cursor->getText(cursor, columnIndex, origin, size + 1);
        origins.push_back(origin);
        delete[] origin;
    } while (cursor->goToNextRow(cursor) == RDB_OK);
}
