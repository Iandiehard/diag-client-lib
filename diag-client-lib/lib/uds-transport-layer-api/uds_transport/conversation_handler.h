/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONVERSATION_H
/* includes */
#include "core/include/span.h"
#include "uds_transport/protocol_types.h"

namespace uds_transport {

/**
 * @brief    Class to manage reception from transport protocol handler to connection handler
 */
class ConversionHandler {
 public:
  /**
   * @brief         Constructs an instance of ConversionHandler
   * @param[in]     handler_id
   *                The handle id of conversation
   */
  explicit ConversionHandler(conversion_manager::ConversionHandlerID handler_id) : handler_id_{handler_id} {}

  /**
   * @brief         Default copy assignment and copy constructor
   */
  ConversionHandler(const ConversionHandler &other) noexcept = default;
  ConversionHandler &operator=(const ConversionHandler &other) noexcept = default;

  /**
   * @brief         Default move assignment and move constructor
   */
  ConversionHandler(ConversionHandler &&other) noexcept = default;
  ConversionHandler &operator=(ConversionHandler &&other) noexcept = default;

  /**
   * @brief         Destructs an instance of ConversionHandler
   */
  virtual ~ConversionHandler() noexcept = default;

  /**
   * @brief       Function to indicate a start of reception of message
   * @details     This is called to indicate the reception of new message by underlying transport protocol handler
   * @param[in]   source_addr
   *              The UDS source address of message
   * @param[in]   target_addr
   *              The UDS target address of message
   * @param[in]   type
   *              The indication whether its is phys/func request
   * @param[in]   channel_id
   *              The transport protocol channel on which message start happened
   * @param[in]   size
   *              The size in bytes of the UdsMessage starting from SID
   * @param[in]   priority
   *              The priority of the given message, used for prioritization of conversations
   * @param[in]   protocol_kind
   *              The identifier of protocol kind associated to message
   * @param[in]   payload_info
   *              The view onto the first received payload bytes, if any. This view shall be used only within this function call.
   *              It is recommended that the TP provides at least the first two bytes of the request message,
   *              so the DM can identify a functional TesterPresent
   * @return      std::pair< IndicationResult, UdsMessagePtr >
   *              The pair of IndicationResult and a pointer to UdsMessage owned/created by DM core and returned
   *              to the handler to get filled
   */
  virtual std::pair<UdsTransportProtocolMgr::IndicationResult, UdsMessagePtr> IndicateMessage(
      UdsMessage::Address source_addr, UdsMessage::Address target_addr, UdsMessage::TargetAddressType type,
      ChannelID channel_id, std::size_t size, Priority priority, ProtocolKind protocol_kind,
      core_type::Span<std::uint8_t const> payload_info) const noexcept = 0;

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  virtual void HandleMessage(UdsMessagePtr message) const noexcept = 0;

 protected:
  /**
   * @brief         Store the conversation handle id
   */
  conversion_manager::ConversionHandlerID handler_id_;
};
}  // namespace uds_transport
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONVERSATION_H
