/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef TEST_COMPONENT_COMPONENT_TEST_H_
#define TEST_COMPONENT_COMPONENT_TEST_H_

#include <gtest/gtest.h>

#include "common/logger.h"

namespace test {
namespace component {

class ComponentTest : public testing::Test {
 public:
  static void SetUpTestSuite() {
    // Initialize logger
    common::ComponentTestLogger::GetComponentTestLogger();
  }
};

}  // namespace component
}  // namespace test
#endif  // TEST_COMPONENT_COMPONENT_TEST_H_
