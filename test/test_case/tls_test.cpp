/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "socket/tcp/tls_client.h"
#include "socket/tcp/tls_server.h"

/**
 * @brief       Fixture to test the TLS connection between client and server with supported cipher list
 * 
 */
class TLSFixture : public ::testing::Test {
 protected:
 private:
};