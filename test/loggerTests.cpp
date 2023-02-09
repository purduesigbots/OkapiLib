/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "okapi/api/util/logging.hpp"
#include "test/tests/api/implMocks.hpp"
#include <gtest/gtest.h>

using namespace okapi;

class LoggerTest : public ::testing::Test {
  protected:
  virtual void SetUp() {
    logBuffer = new char[logSize];
    // For testing, we always write, then rewind and read
    // so it is not an issue to open in write vs append
    logFile = fmemopen(logBuffer, logSize, "w+");
  }

  virtual void TearDown() {
    // Call close after every case so other tests don't end up with a NULL logfile pointer
    if (logger) {
      logger->close();
    }
  }

  void logData(const std::shared_ptr<Logger> &) const {
    LOG_ERROR_S("MSG");
    LOG_WARN_S("MSG");
    LOG_INFO_S("MSG");
    LOG_DEBUG_S("MSG");
  }

  FILE *logFile;
  char *logBuffer;
  size_t logSize = 10000;
  std::shared_ptr<Logger> logger;
};

TEST_F(LoggerTest, OffLevel) {
  logger = std::make_shared<Logger>(
    std::make_unique<ConstantMockTimer>(0_ms), logFile, Logger::LogLevel::off);

  logData(logger);
  rewind(logFile);

  char *line = nullptr;
  size_t len;

  // Check that we are done reading
  EXPECT_EQ(getline(&line, &len, logFile), EOF);

  if (line) {
    free(line);
  }
}

TEST_F(LoggerTest, ErrorLevel) {
  logger = std::make_shared<Logger>(
    std::make_unique<ConstantMockTimer>(0_ms), logFile, Logger::LogLevel::error);

  logData(logger);
  rewind(logFile);

  char *line = nullptr;
  size_t len;

  getline(&line, &len, logFile);
  std::string expected = "0 (" + CrossplatformThread::getName() + ") ERROR: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  EXPECT_EQ(getline(&line, &len, logFile), EOF);

  if (line) {
    free(line);
  }
}

TEST_F(LoggerTest, WarningLevel) {
  logger = std::make_shared<Logger>(
    std::make_unique<ConstantMockTimer>(0_ms), logFile, Logger::LogLevel::warn);

  logData(logger);
  rewind(logFile);

  char *line = nullptr;
  size_t len;

  getline(&line, &len, logFile);
  std::string expected = "0 (" + CrossplatformThread::getName() + ") ERROR: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  getline(&line, &len, logFile);
  expected = "0 (" + CrossplatformThread::getName() + ") WARN: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  EXPECT_EQ(getline(&line, &len, logFile), EOF);

  if (line) {
    free(line);
  }
}

TEST_F(LoggerTest, InfoLevel) {
  logger = std::make_shared<Logger>(
    std::make_unique<ConstantMockTimer>(0_ms), logFile, Logger::LogLevel::info);

  logData(logger);
  rewind(logFile);

  char *line = nullptr;
  size_t len;

  getline(&line, &len, logFile);
  std::string expected = "0 (" + CrossplatformThread::getName() + ") ERROR: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  getline(&line, &len, logFile);
  expected = "0 (" + CrossplatformThread::getName() + ") WARN: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  getline(&line, &len, logFile);
  expected = "0 (" + CrossplatformThread::getName() + ") INFO: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  EXPECT_EQ(getline(&line, &len, logFile), EOF);

  if (line) {
    free(line);
  }
}

TEST_F(LoggerTest, DebugLevel) {
  logger = std::make_shared<Logger>(
    std::make_unique<ConstantMockTimer>(0_ms), logFile, Logger::LogLevel::debug);

  logData(logger);
  rewind(logFile);

  char *line = nullptr;
  size_t len;

  getline(&line, &len, logFile);
  std::string expected = "0 (" + CrossplatformThread::getName() + ") ERROR: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  getline(&line, &len, logFile);
  expected = "0 (" + CrossplatformThread::getName() + ") WARN: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  getline(&line, &len, logFile);
  expected = "0 (" + CrossplatformThread::getName() + ") INFO: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  getline(&line, &len, logFile);
  expected = "0 (" + CrossplatformThread::getName() + ") DEBUG: MSG\n";
  EXPECT_STREQ(line, expected.c_str());

  EXPECT_EQ(getline(&line, &len, logFile), EOF);

  if (line) {
    free(line);
  }
}

TEST_F(LoggerTest, TestLazyLogging) {
  rewind(logFile);
  logger = std::make_shared<Logger>(
    std::make_unique<ConstantMockTimer>(0_ms), logFile, Logger::LogLevel::info);

  int x = 0;
  logger->debug([=, &x]() {
    x++;
    return std::string("");
  });

  EXPECT_EQ(x, 0);

  char *line = nullptr;
  size_t len;

  getline(&line, &len, logFile);

  if (line) {
    free(line);
  }
}
