#ifndef LIB_LINUX_SOCKET_CAN_H
#define LIB_LINUX_SOCKET_CAN_H

// includes
#include "can_Types.h"


namespace libOsAbstraction {
namespace libLinux {
namespace libSocket {
namespace can {

/* forward declaration */
class createCANHardwareObjectHandle;
class createCANControllerHandle;

/*
 @ Class Name        : CANDriver
 @ Class Description : CAN driver base instance class
*/
class CANDriver
{
public:
  // ctor
  CANDriver(Can_ControllerIdType tot_num_controller, Can_HwHandleType tot_num_hoh);
  // dtor
  virtual ~CANDriver();
  // func to create or get actual controller (name = "can0"),return controller interface number or -1 on failure
  // @return : E_OK , E_NOT_OK, Controller Id
  Std_ReturnType CAN_CreateController(const char* name, std::uint32_t bitrate, Can_ControllerIdType* can_controllr_id);
  // func to create or get actual controller (name = "can0"),return controller interface number or -1 on failure
  // @return : E_OK , E_NOT_OK, Controller Id
  Std_ReturnType CAN_CreateController(const char* name, const Can_BitTimingType* bit_timeConf, Can_ControllerIdType* can_controllr_id);
  // func to get Controller bit timings already set
  // @return : E_OK , E_NOT_OK
  Std_ReturnType CAN_GetControllerConfig(Can_BitTimingType* bit_timeConf);
  // func to set Controller mode - start / stop actual CAN controller
  // @return : E_OK , E_NOT_OK
  Std_ReturnType CAN_SetControllerMode(Can_ControllerIdType can_controllr_id, Can_ControllerStateType mode);
  // func to destroy actual controller
  // @return : E_OK , E_NOT_OK
  Std_ReturnType CAN_DestroyController(Can_ControllerIdType can_controllr_id);
  // func to create HOH based on CAN controller interface number
  // @return : E_OK , E_NOT_OK
  Std_ReturnType CAN_CreateHardwareObjectHandle(Can_HwType* can_hwType, Can_HwHandleType* hoh_id);
  // func to destroy HOH
  // @return : E_OK , E_NOT_OK
  Std_ReturnType CAN_DestroyHardwareObjectHandle(Can_HwHandleType hoh_id);
  // func to register TxConfirmation and Reception Callback to HOH
  // @return : E_OK , E_NOT_OK
  Std_ReturnType CAN_HOH_RegisterRxCallback(Can_HwHandleType hoh_id, Can_RxCallbckType* rx_callbck);
  // func to set CAN hardware filter
  // @return : E_OK, E_NOT_OK
  Std_ReturnType CAN_HOH_SetHardwareFilter(Can_HwHandleType hoh_id, Can_HwFilterType* filter, std::uint8_t tot_num_filter);
  // func to place transmit request to HOH
  // @return : E_OK , E_NOT_OK , E_BUSY (Retry)
  Std_ReturnType CAN_HOH_TransmitRequest(Can_HwHandleType hth_id, const Can_PduType* pduInfo, std::uint8_t num_of_frame);
private:
  // mutex - locking critical sections 
  std::mutex lock_e;
  // store actual controller objects
  std::vector<std::unique_ptr<createCANControllerHandle>> can_controller_box_e;
  // store HOH objects
  std::vector<std::unique_ptr<createCANHardwareObjectHandle>> can_hoh_box_e;
  // store controller count
  Can_ControllerIdType can_controller_count = 0;
  // store hardware object count
  Can_HwHandleType can_hoh_count = 0;
private:
  // func to get controller id based on controller name
  Can_ControllerIdType CAN_GetControllerId(const char* name);
  // func to check whether controller id is valid or not
  Std_ReturnType CAN_ControllerIsValid(Can_ControllerIdType can_controllr_id);
  // func to check valid HOH handle
  Std_ReturnType CAN_HOHIsValid(Can_HwHandleType hoh_id);

};

} // can
} // libSocket
} // libLinux
} // libOsAbstraction

#endif // LIB_LINUX_SOCKET_CAN_H
