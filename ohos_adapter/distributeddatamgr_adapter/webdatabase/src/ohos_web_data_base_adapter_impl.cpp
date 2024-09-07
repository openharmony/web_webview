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

#include "ohos_web_data_base_adapter_impl.h"

#include <cinttypes>
#include <unistd.h>

#include "nweb_log.h"
#include <AbilityKit/ability_runtime/application_context.h>

using namespace OHOS::NWeb;

namespace {
static const int32_t RDB_VERSION = 1;
static const std::string HTTP_AUTH_DATABASE_FILE = "http_auth.db";

static const std::string ID_COL = "_id";
static const std::string HTTPAUTH_TABLE_NAME = "httpauth";
static const std::string HTTPAUTH_HOST_COL = "host";
static const std::string HTTPAUTH_REALM_COL = "realm";
static const std::string HTTPAUTH_USERNAME_COL = "username";
static const std::string HTTPAUTH_PASSWORD_COL = "password";

static const std::string CREATE_TABLE = "CREATE TABLE " + HTTPAUTH_TABLE_NAME
    + " (" + ID_COL + " INTEGER PRIMARY KEY, "
    + HTTPAUTH_HOST_COL + " TEXT, " + HTTPAUTH_REALM_COL
    + " TEXT, " + HTTPAUTH_USERNAME_COL + " TEXT, "
    + HTTPAUTH_PASSWORD_COL + " BLOB," + " UNIQUE ("
    + HTTPAUTH_HOST_COL + ", " + HTTPAUTH_REALM_COL
    + ") ON CONFLICT REPLACE);";

const std::string WEB_PATH = "/web";

const std::unordered_map<AbilityRuntime_AreaMode, Rdb_SecurityArea> AREA_MODE_MAP = {
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL1, Rdb_SecurityArea::RDB_SECURITY_AREA_EL1 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL2, Rdb_SecurityArea::RDB_SECURITY_AREA_EL2 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL3, Rdb_SecurityArea::RDB_SECURITY_AREA_EL3 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL4, Rdb_SecurityArea::RDB_SECURITY_AREA_EL4 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL5, Rdb_SecurityArea::RDB_SECURITY_AREA_EL5 }
};
}

OhosWebDataBaseAdapterImpl& OhosWebDataBaseAdapterImpl::GetInstance()
{
    WVLOG_I("webdatabase get data base instance");
    static OhosWebDataBaseAdapterImpl instance;
    return instance;
}

Rdb_SecurityArea OhosWebDataBaseAdapterImpl::GetAreaMode(AbilityRuntime_AreaMode areaMode)
{
    auto mode = AREA_MODE_MAP.find(areaMode);
    return mode->second;
}

void OhosWebDataBaseAdapterImpl::GetOrOpen(const OH_Rdb_Config *config, int *errCode)
{
    rdbStore_ = OH_Rdb_GetOrOpen(config, errCode);
    if (rdbStore_ == nullptr) {
        WVLOG_E("webdatabase get rdb store failed, errCode=%{public}d", *errCode);
        return;
    }

    int version = RDB_VERSION;
    if (OH_Rdb_GetVersion(rdbStore_, &version) != RDB_OK) {
        WVLOG_E("webdatabase get rdb version failed");
        return;
    }

    if (version == 0) {
        if (OH_Rdb_Execute(rdbStore_, CREATE_TABLE.c_str()) != RDB_OK) {
            WVLOG_E("webdatabase create table failed");
            return;
        }
        if (OH_Rdb_SetVersion(rdbStore_, RDB_VERSION) != RDB_OK) {
            WVLOG_E("webdatabase set version failed");
            return;
        }
        WVLOG_I("webdatabase create rdb succeed");
    } else {
        WVLOG_I("webdatabase already exists");
    }
}

OhosWebDataBaseAdapterImpl::OhosWebDataBaseAdapterImpl()
{
    WVLOG_I("webdatabase create rdb store");

    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    constexpr int32_t NATIVE_BUFFER_SIZE = 1024;
    char cacheDir[NATIVE_BUFFER_SIZE];
    int32_t cacheDirLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetCacheDir(cacheDir, NATIVE_BUFFER_SIZE, &cacheDirLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextGetCacheDir failed:err=%{public}d", code);
        return;
    }
    std::string stringDir(cacheDir);
    std::string databaseDir = stringDir + WEB_PATH;

    if (access(databaseDir.c_str(), F_OK) != 0) {
        WVLOG_E("webdatabase fail to access cache web dir:%{public}s", databaseDir.c_str());
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

    std::string name = HTTP_AUTH_DATABASE_FILE;
    OH_Rdb_Config config = {0};
    config.selfSize = sizeof(OH_Rdb_Config);
    config.dataBaseDir = databaseDir.c_str();
    config.bundleName = bundleName;
    config.storeName = name.c_str();
    config.area = GetAreaMode(areaMode);
    config.isEncrypt = true;
    WVLOG_I("webdatabase databaseDir=%{public}s", databaseDir.c_str());
    WVLOG_I("webdatabase bundleName=%{public}s", bundleName);

    int errCode = static_cast<int>(RDB_OK);
    GetOrOpen(&config, &errCode);
}

OhosWebDataBaseAdapterImpl::~OhosWebDataBaseAdapterImpl()
{
    int errCode = OH_Rdb_CloseStore(rdbStore_);
    if (errCode == RDB_OK) {
        WVLOG_I("webdatabase delete rdb succeed");
    } else {
        WVLOG_E("webdatabase delete rdb failed");
    }
}

void OhosWebDataBaseAdapterImpl::SaveHttpAuthCredentials(const std::string& host, const std::string& realm,
    const std::string& username, const char* password)
{
    WVLOG_I("webdatabase save http auth info");
    if (host.empty() || username.empty() || password == nullptr) {
        return;
    }
    if (rdbStore_ == nullptr) {
        return;
    }

    int errCode;
    std::vector<uint8_t> passwordVector(password, password + strlen(password));
    OH_VBucket *valueBucket = OH_Rdb_CreateValuesBucket();
    valueBucket->clear(valueBucket);
    valueBucket->putText(valueBucket, HTTPAUTH_HOST_COL.c_str(), host.c_str());
    valueBucket->putText(valueBucket, HTTPAUTH_REALM_COL.c_str(), realm.c_str());
    valueBucket->putText(valueBucket, HTTPAUTH_USERNAME_COL.c_str(), username.c_str());
    valueBucket->putBlob(valueBucket, HTTPAUTH_PASSWORD_COL.c_str(), passwordVector.data(),
        static_cast<uint32_t>(passwordVector.size()));
    (void)memset(&passwordVector[0], 0, passwordVector.size());
    errCode = OH_Rdb_Insert(rdbStore_, HTTPAUTH_TABLE_NAME.c_str(), valueBucket);
    valueBucket->destroy(valueBucket);
    if (errCode == RDB_ERR || errCode == RDB_E_INVALID_ARGS) {
        WVLOG_E("webdatabase rdb store insert failed, errCode=%{public}d", errCode);
        return;
    }
    WVLOG_I("webdatabase save http auth info end");
}

void OhosWebDataBaseAdapterImpl::GetHttpAuthCredentials(const std::string& host, const std::string& realm,
    std::string& username, char* password, uint32_t passwordSize)
{
    WVLOG_I("webdatabase get username and password");
    if (host.empty() || password == nullptr || passwordSize == 0) {
        return;
    }
    if (rdbStore_ == nullptr) {
        return;
    }

    std::vector<std::string> columns;
    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(HTTPAUTH_TABLE_NAME.c_str());
    OH_VObject *valueObject = OH_Rdb_CreateValueObject();
    valueObject->putText(valueObject, host.c_str());
    dirAbsPred->equalTo(dirAbsPred, HTTPAUTH_HOST_COL.c_str(), valueObject);
    valueObject->putText(valueObject, realm.c_str());
    dirAbsPred->equalTo(dirAbsPred, HTTPAUTH_REALM_COL.c_str(), valueObject);
    valueObject->destroy(valueObject);
    OH_Cursor *cursor = OH_Rdb_Query(rdbStore_, dirAbsPred, NULL, 0);
    dirAbsPred->destroy(dirAbsPred);

    if ((cursor == nullptr) || (cursor->goToNextRow(cursor) != RDB_OK)) {
        WVLOG_E("webdatabase rdb store query failed");
        if (cursor != nullptr) {
            cursor->destroy(cursor);
        }
        return;
    }

    int columnIndex;
    cursor->getColumnIndex(cursor, HTTPAUTH_USERNAME_COL.c_str(), &columnIndex);
    size_t size = 0;
    cursor->getSize(cursor, columnIndex, &size);
    char *name = new char[size + 1]();
    cursor->getText(cursor, columnIndex, name, size + 1);
    username = name;
    delete[] name;

    cursor->getColumnIndex(cursor, HTTPAUTH_PASSWORD_COL.c_str(), &columnIndex);
    cursor->getSize(cursor, columnIndex, &size);
    if (size > passwordSize - 1) {
        WVLOG_E("webdatabase get credential fail: pwd too long");
        cursor->destroy(cursor);
        return;
    }
    unsigned char *passwd = new unsigned char[size + 1]();
    cursor->getBlob(cursor, columnIndex, passwd, size);
    cursor->destroy(cursor);
    (void)memcpy(password, passwd, size + 1);
    (void)memset(passwd, 0, size + 1);
    delete[] passwd;
}

bool OhosWebDataBaseAdapterImpl::ExistHttpAuthCredentials()
{
    WVLOG_I("webdatabase check exist http auth info");
    if (rdbStore_ == nullptr) {
        return false;
    }

    int64_t outValue = 0;
    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(HTTPAUTH_TABLE_NAME.c_str());
    static const std::string SELECT_COUNT = "select count(1) from " + HTTPAUTH_TABLE_NAME;
    OH_Cursor *cursor = OH_Rdb_ExecuteQuery(rdbStore_, SELECT_COUNT.c_str());
    if (cursor == nullptr || cursor->goToNextRow(cursor) != RDB_OK ||
        cursor->getInt64(cursor, 0, &outValue) != RDB_OK) {
        dirAbsPred->destroy(dirAbsPred);
        if (cursor != nullptr) {
            cursor->destroy(cursor);
        }
        return false;
    }
    WVLOG_I("webdatabase exist http auth info num = %{public}" PRId64, outValue);
    dirAbsPred->destroy(dirAbsPred);
    cursor->destroy(cursor);
    if (outValue <= 0) {
        return false;
    }
    return true;
}

void OhosWebDataBaseAdapterImpl::DeleteHttpAuthCredentials()
{
    WVLOG_I("webdatabase clear all http auth info");
    if (rdbStore_ == nullptr) {
        return;
    }
    OH_Predicates *dirAbsPred = OH_Rdb_CreatePredicates(HTTPAUTH_TABLE_NAME.c_str());
    int ret = OH_Rdb_Delete(rdbStore_, dirAbsPred);
    if (ret != RDB_ERR && ret != RDB_E_INVALID_ARGS) {
        WVLOG_I("webdatabase clear all http auth info succ: deleted rows=%{public}d", ret);
    } else {
        WVLOG_E("webdatabase clear all http auth info fail: ret=%{public}d", ret);
    }
    dirAbsPred->destroy(dirAbsPred);
    return;
}
