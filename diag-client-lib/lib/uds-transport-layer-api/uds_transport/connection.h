/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONNECTION_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONNECTION_H
/* includes */
#include <cstdint>

#include "core/include/span.h"
#include "uds_transport/protocol_handler.h"
#include "uds_transport/protocol_mgr.h"
#include "uds_transport/protocol_types.h"

namespace uds_transport {

/**
 * @brief    Interface class to handle connection between two layers
 */
class Connection {
 public:
  /**
   * @brief   Type alias for connection id
   */
  using ConnectionId = std::uint8_t;

  /**
   * @brief   Type alias for Initialization result
   */
  using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

  /**
   * @brief       Constructor to create a new connection
   * @param[in]   connection_id
   *              The connection identification
   * @param[in]   conversation_handler
   *              The reference to conversation handler
   */
  Connection(ConnectionId connection_id, uds_transport::ConversionHandler const &conversation_handler) noexcept
      : conversation_handler_{conversation_handler},
        connection_id_{connection_id} {}

  /**
   * @brief         Destruct an instance of Connection
   */
  virtual ~Connection() = default;

  /**
   * @brief        Function to initialize the connection
   * @return       The initialization result
   */
  virtual InitializationResult Initialize() = 0;

  /**
   * @brief        Function to get the connection id
   * @return       The connection id
   */
  ConnectionId GetConnectionId() const noexcept { return connection_id_; }

  /**
   * @brief        Function to start the connection
   */
  virtual void Start() = 0;

  /**
   * @brief        Function to stop the connection
   */
  virtual void Stop() = 0;

  /**
   * @brief        Function to check if connected to host remote server
   * @return       True if connection, False otherwise
   */
  virtual bool IsConnectToHost() = 0;

  /**
   * @brief       Function to establish connection to remote host server
   * @param[in]   message
   *              The connection message
   * @return      Connection result
   */
  virtual UdsTransportProtocolMgr::ConnectionResult ConnectToHost(UdsMessageConstPtr message) = 0;

  /**
   * @brief       Function to disconnect from remote host server
   * @return      Disconnection result
   */
  virtual UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() = 0;

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
      core_type::Span<std::uint8_t const> payload_info) = 0;

  /**
   * @brief       Function to transmit a valid Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request.
   */
  virtual UdsTransportProtocolMgr::TransmissionResult Transmit(UdsMessageConstPtr message) = 0;

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  virtual void HandleMessage(UdsMessagePtr message) = 0;

 protected:
  /**
   * @brief        Store the conversation handler
   */
  uds_transport::ConversionHandler const &conversation_handler_;

 private:
  /**
   * @brief        Store the connection id
   */
  ConnectionId connection_id_;
};

}  // namespace uds_transport

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONNECTION_H