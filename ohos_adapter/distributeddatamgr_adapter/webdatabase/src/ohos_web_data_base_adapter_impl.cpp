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
#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/application_context.h"
#include "nweb_log.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::NWeb;

static const int32_t RDB_VERSION = 1;
static const std::string HTTP_AUTH_DATABASE_FILE = "http_auth.db";
static const std::string ENCRYPTION_LEVEL = "el2";

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
    + HTTPAUTH_PASSWORD_COL + " TEXT," + " UNIQUE ("
    + HTTPAUTH_HOST_COL + ", " + HTTPAUTH_REALM_COL
    + ") ON CONFLICT REPLACE);";

int32_t DataBaseRdbOpenCallBack::OnCreate(OHOS::NativeRdb::RdbStore& store)
{
    WVLOG_I("webdatabase rdb opened, create table: %{public}s", CREATE_TABLE.c_str());
    return store.ExecuteSql(CREATE_TABLE);
}

int32_t DataBaseRdbOpenCallBack::OnUpgrade(OHOS::NativeRdb::RdbStore& rdbStore,
    int32_t currentVersion, int32_t targetVersion)
{
    WVLOG_I("webdatabase rdb upgrade");
    return OHOS::NativeRdb::E_OK;
}

OhosWebDataBaseAdapterImpl& OhosWebDataBaseAdapterImpl::GetInstance()
{
    WVLOG_I("webdatabase get data base instance");
    static OhosWebDataBaseAdapterImpl instance;
    return instance;
}

OhosWebDataBaseAdapterImpl::OhosWebDataBaseAdapterImpl()
{
    WVLOG_I("webdatabase create rdb store");
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        OHOS::AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (context == nullptr) {
        WVLOG_E("webdatabase get context failed");
        return;
    }
    std::string bundleName = context->GetBundleName();
    std::string databaseDir = context->GetDatabaseDir();
    std::string relativePath = HTTP_AUTH_DATABASE_FILE;

    RdbStoreConfig config(databaseDir + "/" + relativePath);
    config.SetBundleName(bundleName);
    config.SetName(HTTP_AUTH_DATABASE_FILE);
    config.SetRelativePath(relativePath);
    config.SetEncryptLevel(ENCRYPTION_LEVEL);
    WVLOG_I("webdatabase databaseDir=%{public}s", databaseDir.c_str());
    WVLOG_I("webdatabase bundleName=%{public}s", bundleName.c_str());

    int32_t errCode = NativeRdb::E_OK;
    DataBaseRdbOpenCallBack callBack;
    rdbStore_ = RdbHelper::GetRdbStore(config, RDB_VERSION, callBack, errCode);
    if (rdbStore_ == nullptr) {
        WVLOG_I("webdatabase get rdb store failed, errCode=%{public}d", errCode);
    }
    WVLOG_I("webdatabase create rdb store end");
}

void OhosWebDataBaseAdapterImpl::SaveHttpAuthCredentials(const std::string& host, const std::string& realm,
    const std::string& username, const char* password)
{
    WVLOG_I("webdatabase save http auth info");
    if (host.empty() || realm.empty() || username.empty() || password == nullptr) {
        return;
    }
    if (rdbStore_ == nullptr) {
        return;
    }

    int32_t errCode;
    int64_t outRowId;
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.Clear();
    valuesBucket.PutString(HTTPAUTH_HOST_COL, host);
    valuesBucket.PutString(HTTPAUTH_REALM_COL, realm);
    valuesBucket.PutString(HTTPAUTH_USERNAME_COL, username);
    valuesBucket.PutString(HTTPAUTH_PASSWORD_COL, password);
    errCode = rdbStore_->Insert(outRowId, HTTPAUTH_TABLE_NAME, valuesBucket);
    if (errCode != NativeRdb::E_OK) {
        WVLOG_E("webdatabase rdb store insert failed, errCode=%{public}d", errCode);
        return;
    }
    WVLOG_I("webdatabase save http auth info end");
}

void OhosWebDataBaseAdapterImpl::GetHttpAuthCredentials(const std::string& host, const std::string& realm,
    std::vector<std::string>& usernamePassword) const
{
    WVLOG_I("webdatabase get username and password");
    if (host.empty() || realm.empty()) {
        return;
    }
    if (rdbStore_ == nullptr) {
        return;
    }

    std::unique_ptr<AbsSharedResultSet> resultSet = nullptr;
    std::vector<std::string> columns;
    NativeRdb::AbsRdbPredicates dirAbsPred(HTTPAUTH_TABLE_NAME);
    dirAbsPred.EqualTo(HTTPAUTH_HOST_COL, host);
    dirAbsPred.EqualTo(HTTPAUTH_REALM_COL, realm);
    resultSet = rdbStore_->Query(dirAbsPred, columns);
    if ((resultSet == nullptr) || (resultSet->GoToFirstRow() != NativeRdb::E_OK)) {
        WVLOG_E("webdatabase rdb store query failed");
        return;
    }

    int32_t columnIndex;
    std::string userName;
    std::string passWord;
    resultSet->GetColumnIndex(HTTPAUTH_USERNAME_COL, columnIndex);
    resultSet->GetString(columnIndex, userName);
    resultSet->GetColumnIndex(HTTPAUTH_PASSWORD_COL, columnIndex);
    resultSet->GetString(columnIndex, passWord);
    usernamePassword.push_back(userName);
    usernamePassword.push_back(passWord);
    return;
}

bool OhosWebDataBaseAdapterImpl::ExistHttpAuthCredentials() const
{
    WVLOG_I("webdatabase check exist http auth info");
    if (rdbStore_ == nullptr) {
        return false;
    }

    int64_t outValue = 0;
    NativeRdb::AbsRdbPredicates dirAbsPred(HTTPAUTH_TABLE_NAME);
    if (rdbStore_->Count(outValue, dirAbsPred) != NativeRdb::E_OK) {
        return false;
    }
    WVLOG_I("webdatabase exist http auth info num = %{public}" PRId64, outValue);
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
    int32_t deletedRows = 0;
    NativeRdb::AbsRdbPredicates dirAbsPred(HTTPAUTH_TABLE_NAME);
    int32_t ret = rdbStore_->Delete(deletedRows, dirAbsPred);
    WVLOG_I("webdatabase clear all http auth info: ret=%{public}d, deletedRows=%{public}d", ret, deletedRows);
    return;
}
