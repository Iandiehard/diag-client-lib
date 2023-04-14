/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_VEHICLE_INFO_MESSAGE_TYPE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_VEHICLE_INFO_MESSAGE_TYPE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace diag {
namespace client {
namespace vehicle_info {

/**
 * @brief       Structure containing available Vehicle Address Information
 */
struct VehicleAddrInfoResponse {
  /**
   * @brief       IP address of the vehicle
   */
  std::string ip_address{};

  /**
   * @brief       Logical address of the vehicle
   */
  std::uint16_t logical_address{};

  /**
   * @brief       VIN of the vehicle
   */
  std::string vin{};

  /**
   * @brief       Entity Identification of the vehicle
   */
  std::string eid{};

  /**
   * @brief       Group Identification of the vehicle
   */
  std::string gid{};
};

/**
 * @brief       Struct containing Vehicle selection mode.
 */
struct VehicleAddrInfoRequest {
  /**
   * @brief     Mode to be used during sending of Vehicle Identification request.
   *            0U : No preselection
   *            1U : DoIP Entities with given VIN
   *            2U : DoIP Entities with given EID
   */
  std::uint8_t preselection_mode{0U};

  /**
   * @brief     Value to be used based on preselection mode.
   *            VIN when preselection_mode = 1U
   *            EID when preselection_mode = 2U
   *            Empty when preselection_mode = 0U
   */
  std::string preselection_value{};
};

/**
 * @brief       Class provide storage of list of all available vehicle entity
 */
class VehicleInfoMessage {
public:
  /**
   * @brief       Alias to collection of Vehicle info response
   */
  using VehicleInfoListResponseType = std::vector<VehicleAddrInfoResponse>;

public:
  /**
   * @brief         Constructs an instance of VehicleInfoMessage
   */
  VehicleInfoMessage() = default;

  /**
   * @brief       Destructor an instance of VehicleInfoMessage
   */
  virtual ~VehicleInfoMessage() = default;

  /**
   * @brief       Function to get the list of vehicle available in the network.
   * @return      VehicleInfoListResponseType
   *              Result returned
   */
  virtual VehicleInfoListResponseType &GetVehicleList() = 0;
};

/**
 * @brief       Type alias of request storage type used while sending vehicle identification request
 */
using VehicleInfoListRequestType = VehicleAddrInfoRequest;

/**
 * @brief       The unique_ptr for Vehicle Identification Response Message
 */
using VehicleInfoMessageResponseUniquePtr = std::unique_ptr<VehicleInfoMessage>;

}  // namespace vehicle_info
}  // namespace client
}  // namespace diag

#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_VEHICLE_INFO_MESSAGE_TYPE_H
