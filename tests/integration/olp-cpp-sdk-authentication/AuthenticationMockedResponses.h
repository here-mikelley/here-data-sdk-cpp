/*
 * Copyright (C) 2019 HERE Europe B.V.
 *
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
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */

#pragma once

#include <string>

const std::string kResponseValidJson = R"JSON(
   {"accessToken":"tyJhbGciOiJSUzUxMiIsImN0eSI6IkpXVCIsImlzcyI6IkhFUkUiLCJhaWQiOiJTcFR5dkQ0RjZ1dWhVY0t3ZjBPRCIsImlhdCI6MTUyMjY5OTY2MywiZXhwIjoxNTIyNzAzMjYzLCJraWQiOiJqMSJ9.ZXlKaGJHY2lPaUprYVhJaUxDSmxibU1pT2lKQk1qVTJRMEpETFVoVE5URXlJbjAuLkNuSXBWVG14bFBUTFhqdFl0ODVodVEuTk1aMzRVSndtVnNOX21Zd3pwa1UydVFfMklCbE9QeWw0VEJWQnZXczcwRXdoQWRld0tpR09KOGFHOWtKeTBoYWg2SS03Y01WbXQ4S3ppUHVKOXZqV2U1Q0F4cER0LU0yQUxhQTJnZWlIZXJuaEEwZ1ZRR3pVakw5OEhDdkpEc2YuQXhxNTRPTG9FVDhqV2ZreTgtZHY4ZUR1SzctRnJOWklGSms0RHZGa2F5Yw.bfSc5sXovW0-yGTqWDZtsVvqIxeNl9IGFbtzRBRkHCHEjthZzeRscB6oc707JTpiuRmDKJe6oFU03RocTS99YBlM3p5rP2moadDNmP3Uag4elo6z0ZE_w1BP7So7rMX1k4NymfEATdmyXVnjAhBlTPQqOYIWV-UNCXWCIzLSuwaJ96N1d8XZeiA1jkpsp4CKfcSSm9hgsKNA95SWPnZAHyqOYlO0sDE28osOIjN2UVSUKlO1BDtLiPLta_dIqvqFUU5aRi_dcYqkJcZh195ojzeAcvDGI6HqS2zUMTdpYUhlwwfpkxGwrFmlAxgx58xKSeVt0sPvtabZBAW8uh2NGg","tokenType":"bearer","expiresIn":3599}
    )JSON";
const std::string kResponseInvalidJson = R"JSON({x\})JSON";
const std::string kResponseNoToken = R"JSON(
   {"tokenType":"bearer","expiresIn":3599}
    )JSON";
const std::string kResponseNoTokenType = R"JSON(
   {"accessToken":"tyJhbGciOiJSUzUxMiIsImN0eSI6IkpXVCIsImlzcyI6IkhFUkUiLCJhaWQiOiJTcFR5dkQ0RjZ1dWhVY0t3ZjBPRCIsImlhdCI6MTUyMjY5OTY2MywiZXhwIjoxNTIyNzAzMjYzLCJraWQiOiJqMSJ9.ZXlKaGJHY2lPaUprYVhJaUxDSmxibU1pT2lKQk1qVTJRMEpETFVoVE5URXlJbjAuLkNuSXBWVG14bFBUTFhqdFl0ODVodVEuTk1aMzRVSndtVnNOX21Zd3pwa1UydVFfMklCbE9QeWw0VEJWQnZXczcwRXdoQWRld0tpR09KOGFHOWtKeTBoYWg2SS03Y01WbXQ4S3ppUHVKOXZqV2U1Q0F4cER0LU0yQUxhQTJnZWlIZXJuaEEwZ1ZRR3pVakw5OEhDdkpEc2YuQXhxNTRPTG9FVDhqV2ZreTgtZHY4ZUR1SzctRnJOWklGSms0RHZGa2F5Yw.bfSc5sXovW0-yGTqWDZtsVvqIxeNl9IGFbtzRBRkHCHEjthZzeRscB6oc707JTpiuRmDKJe6oFU03RocTS99YBlM3p5rP2moadDNmP3Uag4elo6z0ZE_w1BP7So7rMX1k4NymfEATdmyXVnjAhBlTPQqOYIWV-UNCXWCIzLSuwaJ96N1d8XZeiA1jkpsp4CKfcSSm9hgsKNA95SWPnZAHyqOYlO0sDE28osOIjN2UVSUKlO1BDtLiPLta_dIqvqFUU5aRi_dcYqkJcZh195ojzeAcvDGI6HqS2zUMTdpYUhlwwfpkxGwrFmlAxgx58xKSeVt0sPvtabZBAW8uh2NGg",expiresIn":3599}
    )JSON";
const std::string kResponseNoExpiry = R"JSON(
   {"accessToken":"tyJhbGciOiJSUzUxMiIsImN0eSI6IkpXVCIsImlzcyI6IkhFUkUiLCJhaWQiOiJTcFR5dkQ0RjZ1dWhVY0t3ZjBPRCIsImlhdCI6MTUyMjY5OTY2MywiZXhwIjoxNTIyNzAzMjYzLCJraWQiOiJqMSJ9.ZXlKaGJHY2lPaUprYVhJaUxDSmxibU1pT2lKQk1qVTJRMEpETFVoVE5URXlJbjAuLkNuSXBWVG14bFBUTFhqdFl0ODVodVEuTk1aMzRVSndtVnNOX21Zd3pwa1UydVFfMklCbE9QeWw0VEJWQnZXczcwRXdoQWRld0tpR09KOGFHOWtKeTBoYWg2SS03Y01WbXQ4S3ppUHVKOXZqV2U1Q0F4cER0LU0yQUxhQTJnZWlIZXJuaEEwZ1ZRR3pVakw5OEhDdkpEc2YuQXhxNTRPTG9FVDhqV2ZreTgtZHY4ZUR1SzctRnJOWklGSms0RHZGa2F5Yw.bfSc5sXovW0-yGTqWDZtsVvqIxeNl9IGFbtzRBRkHCHEjthZzeRscB6oc707JTpiuRmDKJe6oFU03RocTS99YBlM3p5rP2moadDNmP3Uag4elo6z0ZE_w1BP7So7rMX1k4NymfEATdmyXVnjAhBlTPQqOYIWV-UNCXWCIzLSuwaJ96N1d8XZeiA1jkpsp4CKfcSSm9hgsKNA95SWPnZAHyqOYlO0sDE28osOIjN2UVSUKlO1BDtLiPLta_dIqvqFUU5aRi_dcYqkJcZh195ojzeAcvDGI6HqS2zUMTdpYUhlwwfpkxGwrFmlAxgx58xKSeVt0sPvtabZBAW8uh2NGg"}
    )JSON";
const std::string kResponseCreated = R"JSON(
    {"id": "scbe-uuid-for-password-reset-object-instance","userId": "HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";
const std::string kResponseNoContent = R"JSON({})JSON";
const std::string kResponseBadRequest = R"JSON(
    {"errorCode":400002, "message":"Invalid JSON."}
    )JSON";
const std::string kResponseUnauthorized = R"JSON(
    {"errorCode":401300, "message":"Signature mismatch. Authorization signature or client credential is wrong."}
    )JSON";
const std::string kResponseNotFound = R"JSON(
    {"errorCode":404000, "message":"User for the given access token cannot be found."}
    )JSON";
const std::string kResponseConflict = R"JSON(
    {"errorCode":409100, "message":"A password account with the specified email address already exists."}
    )JSON";
const std::string kResponsePreconditionFailed = R"JSON(
    {"errorCode":412001, "message":"Precondition Failed"}
    )JSON";
const std::string kResponseTooManyRequests = R"JSON(
    {"errorCode":429002, "message":"Request blocked because too many requests were made. Please wait for a while before making a new request."}
    )JSON";
const std::string kResponseInternalServerError = R"JSON(
    {"errorCode":500203, "message":"Missing Thing Encrypted Secret."}
    )JSON";
const std::string kUserSigninResponse = R"JSON(
    {"accessToken":"password_grant_token","tokenType":"bearer","expiresIn":3599,"refreshToken":"5j687leur4njgb4osomifn55p0","userId":"HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";
const std::string kFacebookSigninResponse = R"JSON(
    {"accessToken":"facebook_grant_token","tokenType":"bearer","expiresIn":3599,"refreshToken":"5j687leur4njgb4osomifn55p0","userId":"HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";
const std::string kGoogleSigninResponse = R"JSON(
    {"accessToken":"google_grant_token","tokenType":"bearer","expiresIn":3599,"refreshToken":"5j687leur4njgb4osomifn55p0","userId":"HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";
const std::string kArcgisSigninResponse = R"JSON(
    {"accessToken":"arcgis_grant_token","tokenType":"bearer","expiresIn":3599,"refreshToken":"5j687leur4njgb4osomifn55p0","userId":"HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";
const std::string kSigninUserFirstTimeResponse = R"JSON(
    {"termsReacceptanceToken":"h1.2nIUQOhp7...RfsAVQ==.D3qoGkpNQJm/+64mEcqgJ6ea3eAdBVNBrtzuB...Vmo2Cog/xOw==","url":{"tos":"http://here.com/terms?locale=en-US#mapsTermsDiv","pp":"http://here.com/terms?locale=en-US#privacyPolicyDiv","tosJSON": "http://here.com/terms/?cc=us&lang=en-US&out=json","ppJSON": "http://here.com/privacy/privacy-policy/us/?lang=en-US&out=json"}}
    )JSON";
const std::string kRefreshSigninResponse = R"JSON(
    {"accessToken":"refresh_grant_token","tokenType":"bearer","expiresIn":3599,"refreshToken":"5j687leur4njgb4osomifn55p0","userId":"HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";
const std::string kResponseErrorFields = R"JSON(
    {"errorCode":400200,"message":"Received invalid data.  See json element 'errorFields' for more information.", "errorFields":[{"name":"password","errorCode":400205,"message":"Black listed password."},{"name":"lastname","errorCode":400203,"message":"Illegal last name."}]}
    )JSON";
const std::string kSignupHereUserResponse = R"JSON(
    {"userId": "HERE-5fa10eda-39ff-4cbc-9b0c-5acba4685649"}
    )JSON";