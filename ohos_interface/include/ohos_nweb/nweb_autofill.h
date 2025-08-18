/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_AUTOFILL_H
#define NWEB_AUTOFILL_H

#include <string>

namespace OHOS::NWeb {

const std::string NWEB_VIEW_DATA_KEY_FOCUS = "focus";
const std::string NWEB_VIEW_DATA_KEY_RECT_X = "x";
const std::string NWEB_VIEW_DATA_KEY_RECT_Y = "y";
const std::string NWEB_VIEW_DATA_KEY_RECT_W = "width";
const std::string NWEB_VIEW_DATA_KEY_RECT_H = "height";
const std::string NWEB_VIEW_DATA_KEY_VALUE = "value";
const std::string NWEB_VIEW_DATA_KEY_PLACEHOLDER = "placeholder";

const std::string NWEB_AUTOFILL_EVENT_SAVE = "save";
const std::string NWEB_AUTOFILL_EVENT_FILL = "fill";
const std::string NWEB_AUTOFILL_EVENT_UPDATE = "update";
const std::string NWEB_AUTOFILL_EVENT_CLOSE = "close";

const std::string NWEB_AUTOFILL_EVENT_TYPE = "event";
const std::string NWEB_AUTOFILL_PAGE_URL = "pageUrl";
const std::string NWEB_AUTOFILL_IS_USER_SELECTED = "isUserSelected";
const std::string NWEB_AUTOFILL_IS_OTHER_ACCOUNT = "isOtherAccount";
const std::string NWEB_AUTOFILL_EVENT_SOURCE = "source";
const std::string NWEB_AUTOFILL_FOR_LOGIN = "login";

const std::string NWEB_AUTOFILL_STREET_ADDRESS = "street-address";
const std::string NWEB_AUTOFILL_ADDRESS_LEVEL_3 = "address-level3";
const std::string NWEB_AUTOFILL_ADDRESS_LEVEL_2 = "address-level2";
const std::string NWEB_AUTOFILL_ADDRESS_LEVEL_1 = "address-level1";
const std::string NWEB_AUTOFILL_COUNTRY = "country";
const std::string NWEB_AUTOFILL_NAME = "name";
const std::string NWEB_AUTOFILL_FAMILY_NAME = "family-name";
const std::string NWEB_AUTOFILL_GIVEN_NAME = "given-name";
const std::string NWEB_AUTOFILL_TEL_NATIONAL = "tel-national";
const std::string NWEB_AUTOFILL_TEL = "tel";
const std::string NWEB_AUTOFILL_TEL_COUNTRY_CODE = "tel-country-code";
const std::string NWEB_AUTOFILL_EMAIL = "email";
const std::string NWEB_AUTOFILL_CC_NUMBER = "cc-number";
const std::string NWEB_AUTOFILL_ID_CARD_NUMBER = "id-card-number";
const std::string NWEB_AUTOFILL_NICKNAME = "nickname";
const std::string NWEB_AUTOFILL_USERNAME = "username";
const std::string NWEB_AUTOFILL_PASSWORD = "password";
const std::string NWEB_AUTOFILL_NEW_PASSWORD = "new-password";
const std::string NWEB_AUTOFILL_PASSPORT_NUMBER = "passport-number";
const std::string NWEB_AUTOFILL_VALIDITY = "validity";
const std::string NWEB_AUTOFILL_ISSUE_AT = "issue-at";
const std::string NWEB_AUTOFILL_ORGANIZATION = "organization";
const std::string NWEB_AUTOFILL_TAX_ID = "tax-id";
const std::string NWEB_AUTOFILL_ADDRESS_CITY_AND_STATE = "address-city-and-state";
const std::string NWEB_AUTOFILL_FLIGHT_NUMBER = "flight-number";
const std::string NWEB_AUTOFILL_LICENSE_NUMBER = "license-number";
const std::string NWEB_AUTOFILL_LICENSE_FILE_NUMBER = "license-file-number";
const std::string NWEB_AUTOFILL_LICENSE_PLATE = "license-plate";
const std::string NWEB_AUTOFILL_ENGINE_NUMBER = "engine-number";
const std::string NWEB_AUTOFILL_LICENSE_CHASSIS_NUMBER = "license-chassis-number";
const std::string NWEB_AUTOFILL_DETAIL_INFO_WITHOUT_STREET = "detail-info-without-street";
const std::string NWEB_AUTOFILL_FORMAT_ADDRESS = "format-address";

enum NWebAutofillEvent {
    UNKNOWN = 0,
    SAVE,
    FILL,
    UPDATE,
    CLOSE,
};

} // namespace OHOS::NWeb

#endif // NWEB_AUTOFILL_H