/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "test/tests/api/implMocks.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace okapi;


class VoltageControllerOutputTest : public ::testing::Test {
  protected:
  void SetUp() override {
    motor = std::make_shared<MockMotor>();
  }

  void TearDown() override {}

  std::shared_ptr<MockMotor> motor;
};

TEST_F(VoltageControllerOutputTest, OneVoltageControllerOutputPerMotor) {
  auto output1 = motor->getVoltageControllerOutput();
  EXPECT_NE(output1, nullptr);

  auto output2 = motor->getVoltageControllerOutput();
  EXPECT_EQ(output1, output2);
}

TEST_F(VoltageControllerOutputTest, SetsMotorVoltage) {
  auto output = motor->getVoltageControllerOutput();
  double setpoint = 0.5;

  output->controllerSet(setpoint);
  EXPECT_EQ(motor->lastVoltage, 12000*setpoint);
}
