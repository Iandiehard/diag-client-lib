/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_UDS_MESSAGE_TYPE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_UDS_MESSAGE_TYPE_H

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace diag {
namespace client {
namespace uds_message {

/**
 * @brief    Class represents a UDS message exchanged between User of diag-client-lib and implementation of
 *           diag-client-lib on diagnostic request reception path or diagnostic response transmission path.
 *           UdsMessage provides the storage for UDS requests/responses.
 */
class UdsMessage {
 public:
  /**
   * @brief         Type alias of ip address type
   */
  using IpAddress = std::string_view;

  /**
   * @brief         Type alias of byte vector type
   */
  using ByteVector = std::vector<std::uint8_t>;

 public:
  /**
   * @brief         Default constructs an instance of UdsMessage
   */
  UdsMessage() = default;

  /**
   * @brief         Copy constructs an instance of UdsMessage
   */
  UdsMessage(const UdsMessage &other) = default;

  /**
   * @brief         Move constructs an instance of UdsMessage
   */
  UdsMessage(UdsMessage &&other) noexcept = default;

  /**
   * @brief         Copy assignment of UdsMessage
   */
  UdsMessage &operator=(const UdsMessage &other) = default;

  /**
   * @brief         Move assignment of UdsMessage
   */
  UdsMessage &operator=(UdsMessage &&other) noexcept = default;

  /**
   * @brief         Destructs an instance of UdsMessage
   */
  virtual ~UdsMessage() = default;

  /**
   * @brief        Get the UDS message data starting with the SID (A_Data as per ISO)
   * @return       const ByteVector&
   *               The entire payload (A_Data)
   */
  virtual const ByteVector &GetPayload() const = 0;

  /**
   * @brief        Return the underlying buffer for write access
   * @return       ByteVector&
   *               payload of the UDSMessage starting from SID.
   */
  virtual ByteVector &GetPayload() = 0;

  /**
   * @brief        Get the remote ip address present
   * @return       IpAddress
   *               Ip address stored
   */
  virtual IpAddress GetHostIpAddress() const noexcept = 0;
};

/**
 * @brief         Type alias of unique_ptr for constant UdsRequestMessage
 */
using UdsRequestMessageConstPtr = std::unique_ptr<UdsMessage const>;

/**
 * @brief         Type alias of unique_ptr for Request Message
 */
using UdsRequestMessagePtr = std::unique_ptr<UdsMessage>;

/**
 * @brief         Type alias of unique_ptr for Response Message
 */
using UdsResponseMessagePtr = std::unique_ptr<UdsMessage>;
}  // namespace uds_message
}  // namespace client
}  // namespace diag

#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_UDS_MESSAGE_TYPE_H
