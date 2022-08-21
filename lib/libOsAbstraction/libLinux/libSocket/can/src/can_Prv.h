#ifndef LIB_LINUX_SOCKET_CAN_PRV_H
#define LIB_LINUX_SOCKET_CAN_PRV_H

// includes
#include "can.h"
#include "can_Types.h"
#include "Linux_Header.h"

namespace libOsAbstraction {
namespace libLinux {
namespace libSocket {
namespace can {

/*
 @ Class Name        : createCANControllerHandle
 @ Class Description : Class used to create a handle for configuring , monitoring,
                       start/stop controller
*/
class createCANControllerHandle
{
public:
  //ctor
  createCANControllerHandle(const char* name, Can_ControllerIdType can_controllr_id);
  //dtor
  virtual ~createCANControllerHandle();
  // set controller bitrate
  Std_ReturnType CAN_SetBitrate(const std::uint32_t bit_rate);
  // set controller config
  Std_ReturnType CAN_SetBitTiming(const Can_BitTimingType* bit_timeConf);
  // get controller config
  Std_ReturnType CAN_GetBitTiming(Can_BitTimingType* bit_timeConf);
  // start the controller
  Std_ReturnType CAN_SetControllerStart(void);
  // stop the controller
  Std_ReturnType CAN_SetControllerStop(void);
  // func to restart CAN Controller after bus off detection
  Std_ReturnType CAN_SetControllerRestart(std::uint32_t restart_ms);
  // get controller names
  void CAN_GetControllerName(char* name);
  // get controller id
  Can_ControllerIdType CAN_GetControllerId(void);
  // get controller state
  Can_ControllerStateType CAN_GetControllerState(void);
  // get CAN Controller Clock Frequency
  // @return : Frequency in Hz
  std::uint32_t CAN_GetCANControllerClock(void);
private:
  // set controller mode
  Std_ReturnType CAN_SetControlMode(void);
  // get error counter number
  Std_ReturnType CAN_GetErrorCounter(Can_ErrCounterType* err_counter);
private:
  // store device name , "canX" X = 0,1,2
  char device_name[4];
  // store controller id
  Can_ControllerIdType can_controllr_id_e;
  // controller error state
  Can_ErrorStateType error_state_e = Can_ErrorStateType::kError_NA;
  // controller state
  Can_ControllerStateType state_e = Can_ControllerStateType::kUninit;
  // store controller config
  Can_BitTimingType  bit_time_conf_e; 
  // thread for detection of CAN error
  std::thread thread_e;
private:
  // Socket linked to HOH
  int socket_e = -1;
  // exit requested
  std::atomic_bool exit_requested;
  // conditional variable
  std::condition_variable cond_var;
  // locking var
  std::mutex mutx_lck;
  // error detection
  void ReadErrorDetection(void);
};

/*
 @ Class Name        : createCANHardwareObjectHandle
 @ Class Description : Class used to create a logical handle for handling transmission
                       and reception of CAN message to/from CAN Controller
*/
class createCANHardwareObjectHandle
{
public:
  // ctor
  createCANHardwareObjectHandle(Can_HwType *can_hwType, Can_HwHandleType hoh_id, createCANControllerHandle* controller_ref);
  // dtor
  virtual ~createCANHardwareObjectHandle();
  // Func to open HOH - socket creation / binding
  Std_ReturnType CAN_OpenCANHardwareObjectHandle(void);
  // Func to close HOH - socket destruction
  Std_ReturnType CAN_CloseCANHardwareObjectHandle(void);
  // Func to get HOH id
  Can_HwHandleType CAN_GetCANHardwareObjectHandleId(void);
  // Func to register rx callbacks
  Std_ReturnType CAN_RegisterRxCallback(Can_RxCallbckType* rx_callbck);
  // Func to set Hardware filter
  Std_ReturnType CAN_SetHardwareFilter(Can_HwFilterType* filter, std::uint8_t tot_num_filter);
  // Func to transmit CAN frames
  Std_ReturnType CAN_Write(const Can_PduType* pduInfo, std::uint8_t num_of_frame);
private:
  // Store CANController Handle info
  Can_HwHandleType can_hoh_id_e;
  // CAN Id Type - Standard , Extended or Mixed mode Identifier
  Can_IdType can_id_type_e;
  // CAN Object Type - Receive or Transmit
  Can_HOType can_hoh_type_e;
  // CanFd support
  bool canfd_enable_e;
  // CAN rx polling time - in ms
  std::uint32_t can_rx_pollng_time_ms;
  // reference to actual CAN controller
  createCANControllerHandle* can_controller_handle_e;
  // store register callback
  Can_RxCallbckType* rx_callbck_e; 
  // store CAN Hardware Filter
  std::vector<Can_HwFilterType> can_filter;
  // thread for socket read
  std::thread thread_e; 
private:
  // Socket linked to HOH
  int socket_e = -1;
  // address can
  struct sockaddr_can addr;
  // exit requested
  std::atomic_bool exit_requested;
  // conditional variable
  std::condition_variable cond_var;
  // locking var
  std::mutex mutx_lck;
  // read from socket function
  void ReadFromHardwareObjectHandleRx(void);
  // process received frame
  void ProcessReceivedFrame(void);
};

} // can
} // libSocket
} // libLinux
} // libOsAbstraction

#endif // LIB_LINUX_SOCKET_CAN_PRV_H
