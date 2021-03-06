/*
 * Copyright (C) 2020 HERE Europe B.V.
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

#include <gtest/gtest.h>
#include <olp/authentication/AuthorizeRequest.h>
#include <olp/authentication/AuthorizeResult.h>

namespace {
using namespace olp::authentication;

TEST(DecisionApiClientTest, AuthorizeRequestTest) {
  EXPECT_EQ(AuthorizeRequest().WithServiceId("ServiceId").GetServiceId(),
            "ServiceId");
  EXPECT_EQ(
      AuthorizeRequest().WithContractId("ContractId").GetContractId().get(),
      "ContractId");
  auto request = AuthorizeRequest().WithAction("action1").WithAction(
      "action2", std::string("hrn::test"));
  EXPECT_EQ(AuthorizeRequest().GetDiagnostics(), false);
  EXPECT_EQ(AuthorizeRequest().WithDiagnostics(true).GetDiagnostics(), true);
  EXPECT_EQ(request.GetActions().size(), 2);
  auto actions_it = request.GetActions().begin();
  EXPECT_EQ(actions_it->first, "action1");
  EXPECT_EQ(actions_it->second, "");
  ++actions_it;
  EXPECT_EQ(actions_it->first, "action2");
  EXPECT_EQ(actions_it->second, "hrn::test");
  EXPECT_EQ(request.GetOperatorType(),
            AuthorizeRequest::DecisionOperatorType::kAnd);
  request.WithOperatorType(AuthorizeRequest::DecisionOperatorType::kOr);
  EXPECT_EQ(request.GetOperatorType(),
            AuthorizeRequest::DecisionOperatorType::kOr);
  request.WithServiceId("service");
  EXPECT_EQ(request.CreateKey(), "service");
  request.WithContractId("contract");
  EXPECT_EQ(request.CreateKey(), "service[contract]");
}

TEST(DecisionApiClientTest, AuthorizeResponceTest) {
  EXPECT_EQ(AuthorizeResult().GetDecision(), DecisionType::kDeny);
  EXPECT_EQ(ActionResult().GetDecision(), DecisionType::kDeny);
  EXPECT_EQ(AuthorizeResult().GetClientId(), "");
  ActionResult action;
  action.SetDecision(DecisionType::kAllow);
  action.SetPermissions({{"read", DecisionType::kAllow}});
  AuthorizeResult decision;
  decision.SetActionResults({action});
  EXPECT_EQ(decision.GetActionResults().front().GetPermissions().front().first,
            "read");
  EXPECT_EQ(decision.GetActionResults().front().GetPermissions().front().second,
            DecisionType::kAllow);
}

}  // namespace
