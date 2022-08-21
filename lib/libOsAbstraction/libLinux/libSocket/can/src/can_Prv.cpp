
// includes
#include "can_Prv.h"

namespace libOsAbstraction {
namespace libLinux {
namespace libSocket {
namespace can {


/*
 @ Class Name        : createCANControllerHandle
 @ Class Description : Class used to create a handle for configuring , monitoring,
                       start/stop controller
*/

//ctor CAN controller
createCANControllerHandle::createCANControllerHandle(const char* name, Can_ControllerIdType can_controllr_id)
{
  // store the device name
  strncpy(device_name, name, strlen(name));
  // store the controller id
  can_controllr_id_e = can_controllr_id;
  exit_requested     = false;
}

//dtor
createCANControllerHandle::~createCANControllerHandle()
{

}

// set controller config
// @return :
Std_ReturnType createCANControllerHandle::CAN_SetBitTiming(const Can_BitTimingType* bit_timeConf)
{
#ifdef __linux__
  struct can_bittiming          can_bit_timeConf;
  Std_ReturnType                retval                 = E_NOT_OK;

  // set only when controller state = uninit
  if(state_e == Can_ControllerStateType::kUninit)
    {
      can_bit_timeConf.bitrate              = bit_timeConf->bitrate;
      can_bit_timeConf.sample_point         = bit_timeConf->sample_point;
      can_bit_timeConf.tq                   = bit_timeConf->tq;
      can_bit_timeConf.prop_seg             = bit_timeConf->prop_seg;
      can_bit_timeConf.phase_seg1           = bit_timeConf->phase_seg1;
      can_bit_timeConf.phase_seg2           = bit_timeConf->phase_seg2;
      can_bit_timeConf.sjw                  = bit_timeConf->sjw;
      can_bit_timeConf.brp                  = bit_timeConf->brp;

      if(can_set_bittiming(device_name, &can_bit_timeConf) < 0)
        {// failed, print error message
        }
      else
        {// success
          /* spawn thread for error detection */
          //thread_e = std::thread(&createCANControllerHandle::ReadErrorDetection, this);
          state_e = Can_ControllerStateType::kInit; 
          retval = E_OK;
        }
    }
  else
    {// error, print
    }
#else
   Std_ReturnType retval                 = E_NOT_OK;
#endif
  return retval;
}

// get controller config
// @return : E_OK, E_NOT_OK
Std_ReturnType createCANControllerHandle::CAN_GetBitTiming(Can_BitTimingType* bit_timeConf)
{
#ifdef __linux__
  struct can_bittiming can_bit_timeConf;
  Std_ReturnType retval                 = E_NOT_OK;

  if(can_get_bittiming(device_name, &can_bit_timeConf) < 0)
    {// failed, print error message
    }
  else
    {// success
      bit_timeConf->bitrate              = can_bit_timeConf.bitrate;
      bit_timeConf->sample_point         = can_bit_timeConf.sample_point;
      bit_timeConf->tq                   = can_bit_timeConf.tq;
      bit_timeConf->prop_seg             = can_bit_timeConf.prop_seg;
      bit_timeConf->phase_seg1           = can_bit_timeConf.phase_seg1;
      bit_timeConf->phase_seg2           = can_bit_timeConf.phase_seg2;
      bit_timeConf->sjw                  = can_bit_timeConf.sjw;
      bit_timeConf->brp                  = can_bit_timeConf.brp;
      retval = E_OK;
    }
#else
  Std_ReturnType retval                 = E_NOT_OK;
#endif
  return retval;
}

// start the controller
// @return :
Std_ReturnType createCANControllerHandle::CAN_SetControllerStart(void)
{
  Std_ReturnType retval                 = E_NOT_OK;
#ifdef __linux__
  // check for controller state 
  if((state_e == Can_ControllerStateType::kInit) || (state_e == Can_ControllerStateType::kStop))
  {
    retval = (can_do_start(device_name) < 0 ) ? E_NOT_OK : E_OK;

    if(retval == E_OK)
      {
        state_e = Can_ControllerStateType::kStart;
      }
    else
      {// print error

      }
  }
#else
  retval = E_OK;
#endif
  return retval;
}

// stop the controller
// @return : E_OK / E_NOT_OK
Std_ReturnType createCANControllerHandle::CAN_SetControllerStop(void)
{
  Std_ReturnType retval                 = E_NOT_OK;
#ifdef __linux__
  // check for controller state 
  if((state_e == Can_ControllerStateType::kStart) || (state_e == Can_ControllerStateType::kInit))
  {
    retval = (can_do_stop(device_name) < 0 ) ? E_NOT_OK : E_OK;

    if(retval == E_OK)
      {
        state_e = Can_ControllerStateType::kStop;
      }
    else
      {// print error

      }
  }
  else
  {// Controller not initialized 

  }
#else
  retval = E_OK;
#endif
  return retval;
}

// func to restart CAN Controller after bus off detection
// @return :
Std_ReturnType createCANControllerHandle::CAN_SetControllerRestart(std::uint32_t restart_ms)
{
  Std_ReturnType retval                 = E_NOT_OK;
#ifdef __linux__
  retval = (can_set_restart_ms(device_name, restart_ms) < 0 ) ? E_NOT_OK : E_OK;
#else
  retval = E_OK;
#endif
  return retval;
}

// get controller id
// @return :
Can_ControllerIdType createCANControllerHandle::CAN_GetControllerId(void)
{
  return can_controllr_id_e;
}

// get controller state
// @return :
Can_ControllerStateType createCANControllerHandle::CAN_GetControllerState(void)
{
  return  state_e;
}

// get CAN Controller Clock Frequency
// @return : Frequency in Hz
std::uint32_t createCANControllerHandle::CAN_GetCANControllerClock(void)
{
#ifdef __linux__
  struct can_clock clock;
  std::uint32_t retval_freq = 0;
  if(can_get_clock(device_name, &clock) < 0)
    {// failed
    }
  else
    {
      retval_freq = clock.freq;
    }
#else
  std::uint32_t retval_freq = 0;
#endif
  return retval_freq;
}

// get error counter number
// @return : Return error counter of CAN controller
Std_ReturnType createCANControllerHandle::CAN_GetErrorCounter(Can_ErrCounterType* err_counter)
{
#ifdef __linux__
  struct can_berr_counter     err_countr;
  Std_ReturnType retval     = E_NOT_OK;

  retval = (can_get_berr_counter(device_name, &err_countr) < 0 ) ? E_NOT_OK : E_OK;

  if(retval == E_OK)
    {
      err_counter->txerr = err_countr.txerr;
      err_counter->rxerr = err_countr.rxerr;
    }
#else
  Std_ReturnType retval     = E_NOT_OK;
  retval = E_OK;
#endif
  return retval;
}

// detect error frames in CAN controller
// @return : void
void createCANControllerHandle::ReadErrorDetection(void)
{
  /* Open the socket */

  while (!exit_requested)
  {
    /* code */
  }
  
}


/*
 @ Class Name        : createCANHardwareObjectHandle
 @ Class Description : Class used to create a logical handle for handling transmission
                       and reception of CAN message to/from CAN Controller
*/

//ctor
createCANHardwareObjectHandle::createCANHardwareObjectHandle(Can_HwType *can_hwType, Can_HwHandleType hoh_id,
                                                             createCANControllerHandle* controller_ref)
{
  can_hoh_type_e            = can_hwType->can_ho_type;
  can_id_type_e             = can_hwType->can_id_type;
  can_hoh_id_e              = hoh_id;
  canfd_enable_e            = can_hwType->canfd_enable;
  can_rx_pollng_time_ms     = can_hwType->can_rx_pollng_time_ms;
  can_controller_handle_e   = controller_ref;
}

//dtor
createCANHardwareObjectHandle::~createCANHardwareObjectHandle()
{
}

// Func to open HOH - socket creation / binding
Std_ReturnType createCANHardwareObjectHandle::CAN_OpenCANHardwareObjectHandle(void)
{
  Std_ReturnType retval     = E_NOT_OK;
#ifdef __linux__
  // create can raw socket
  socket_e = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if(socket_e < 0)
    {// failed, print exit
    }
  else
    {
      struct ifreq ifr;
      timeval tv;
      char device_name[4];
      int retval_sockopt = -1;
      // get device name
      can_controller_handle_e->CAN_GetControllerName(&device_name[0]);

      memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
      strcpy(ifr.ifr_name, device_name);
      ioctl(socket_e, SIOCGIFINDEX, &ifr);

      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;
      // set socket options
      // set non blocking read socket
      tv.tv_sec = 0;
      tv.tv_usec = can_rx_pollng_time_ms * 1000;
      retval_sockopt = setsockopt(socket_e, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

      if(retval_sockopt < 0)
        {// fail , SO_RCVTIMEO
        }
      else
        {// enable local loopback
          int loopback = 1;
          retval_sockopt = setsockopt(socket_e, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
          if(retval_sockopt < 0)
            {// fail CAN_RAW_LOOPBACK
            }
          else
            {// enable reception of send message - txconfirmation
              int recv_own_msgs = 1;
              retval_sockopt = setsockopt(socket_e, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
              if(retval_sockopt < 0)
                {// fail, CAN_RAW_RECV_OWN_MSGS

                }
            }
        }

      /* Check for canfd support */
      if(canfd_enable_e != false)
      {
        int canfd_on = 1;
        retval_sockopt = setsockopt(socket_e, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));
      }   

      retval = (retval_sockopt < 0) ? E_NOT_OK : E_OK;
      // bind the socket
      if(retval == E_OK)
        {
          if(bind(socket_e, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            {// bind failed
              retval = E_NOT_OK;
            }
          else
            {// bind successful , spawn the Rx thread
              thread_e = std::thread(&createCANHardwareObjectHandle::ReadFromHardwareObjectHandleRx, this);
            }
        }
    }
#endif

  return retval;
}

// Func to close HOH - socket destruction
Std_ReturnType createCANHardwareObjectHandle::CAN_CloseCANHardwareObjectHandle(void)
{
  Std_ReturnType retval     = E_NOT_OK;
  // close socket
  if(close(socket_e) < 0)
  {// closure of socket failed    
  }
  else
  {
    retval = E_OK;
  }
  // destroy thread
  return retval;
}

// Func to get HOH id
Can_HwHandleType createCANHardwareObjectHandle::CAN_GetCANHardwareObjectHandleId(void)
{
  return can_hoh_id_e;
}

// Func to register rx callbacks
Std_ReturnType createCANHardwareObjectHandle::CAN_RegisterRxCallback(Can_RxCallbckType* rx_callbck)
{
    Std_ReturnType retval     = E_OK;
    rx_callbck_e = rx_callbck;
    return retval;
}

// Func to set Hardware filter
Std_ReturnType createCANHardwareObjectHandle::CAN_SetHardwareFilter(Can_HwFilterType* filter, std::uint8_t tot_num_filter)
{
    Std_ReturnType retval     = E_NOT_OK;
    return retval;
}

// Func to transmit CAN frames
Std_ReturnType createCANHardwareObjectHandle::CAN_Write(const Can_PduType* pduInfo, std::uint8_t num_of_frame)
{
    Std_ReturnType retval         = E_NOT_OK;
    ssize_t        bytes_written  = 0;  
    // check for controller state 
    if(can_controller_handle_e->CAN_GetControllerState() == Can_ControllerStateType::kStart)
    { 
      if(canfd_enable_e != false)
      {
        struct canfd_frame frame[num_of_frame];

        for (std::uint8_t i = 0; i < num_of_frame; i++)
        {
          frame[i].can_id = (pduInfo->frametype == Can_FrameType::kStandard_Fd) ? (pduInfo->id & CAN_SFF_MASK) : (pduInfo->id & CAN_EFF_MASK) | CAN_EFF_FLAG;
          frame[i].len    = pduInfo->length;
          memcpy(&frame[i].data, pduInfo->sdu_ptr, frame[i].len);
        }      
        // execute socket write 
        bytes_written = write(socket_e, &frame, num_of_frame * sizeof(struct canfd_frame));       
      }
      else
      {
        struct can_frame frame[num_of_frame];
        
        for (std::uint8_t i = 0; i < num_of_frame; i++)
        {
          frame[i].can_id   = (pduInfo->frametype == Can_FrameType::kStandard_Normal) ? (pduInfo->id & CAN_SFF_MASK) : (pduInfo->id & CAN_EFF_MASK) | CAN_EFF_FLAG;;
          frame[i].can_dlc  =  pduInfo->length;
          memcpy(&frame[i].data, pduInfo->sdu_ptr, frame[i].can_dlc);
        }    
        // execute socket write
        bytes_written = write(socket_e, &frame, num_of_frame * sizeof(struct can_frame));
      }

      if(bytes_written < 0)
      {
        switch (errno)
        {
          case ENOBUFS:
            // try again
            retval = E_BUSY; 
            break;
          
          default:
            break;
        }
      }
      else if(bytes_written != pduInfo->length)
      {// write failure
        retval = E_NOT_OK;
      }
      else if(bytes_written = pduInfo->length)
      {
        retval = E_OK;
      }        
      else
      {// do nothing
      }
    }
    else
    {

    }
    
    return retval;
}

// Func to read from socket
// @return : void
void createCANHardwareObjectHandle::ReadFromHardwareObjectHandleRx(void)
{
  struct can_frame        normal_frame;
  struct canfd_frame      fd_frame;
  struct msghdr           msg;
  struct iovec            iov;
  struct cmsghdr*         cmsg;

  if(canfd_enable_e != false)
  {
    iov.iov_base  = &fd_frame;
    iov.iov_len   = sizeof(fd_frame);
  }
  else
  {
    iov.iov_base  = &normal_frame;
    iov.iov_len   = sizeof(normal_frame);
  }
  msg.msg_name    = &addr;
	msg.msg_iov     = &iov;
	msg.msg_iovlen  = 1;	
	msg.msg_namelen = sizeof(addr);
	msg.msg_flags   = 0;

  while (!exit_requested)
  { 
    Can_PduType can_pdu;  
    int received_bytes = 0;
    // read from socket 
    received_bytes = recvmsg(socket_e, &msg, 0);
    if(received_bytes < 0)
    {/* check for errno */
      switch (errno)
      {
        case EAGAIN:
          /* no frame received , read again */
          break;

        case ENETDOWN:
          break;
        
        default:
          break;
      }
    }
    else
    {
      if((ssize_t)received_bytes == CAN_MTU)
      {
        can_pdu.frametype   = (normal_frame.can_id & CAN_EFF_FLAG == CAN_EFF_FLAG) ? Can_FrameType::kExtended_Normal : Can_FrameType::kStandard_Normal;
        can_pdu.id          = (can_pdu.frametype == Can_FrameType::kExtended_Normal) ? normal_frame.can_id & CAN_EFF_MASK : normal_frame.can_id & CAN_SFF_MASK;
        can_pdu.length      = normal_frame.can_dlc;
        can_pdu.sdu_ptr     = normal_frame.data;
        
        if(msg.msg_flags == MSG_CONFIRM)
        {// tx confirmation
          rx_callbck_e->txConfirmation(can_hoh_id_e, &can_pdu);
        }
        else
        {// rx indication
          rx_callbck_e->rxIndication(can_hoh_id_e, &can_pdu);
        }
      }
      else if((ssize_t)received_bytes == CANFD_MTU)
      {
        can_pdu.frametype   = (fd_frame.can_id & CAN_EFF_FLAG == CAN_EFF_FLAG) ? Can_FrameType::kExtended_Fd : Can_FrameType::kStandard_Fd;
        can_pdu.id          = (can_pdu.frametype == Can_FrameType::kExtended_Fd) ? fd_frame.can_id & CAN_EFF_MASK : fd_frame.can_id & CAN_SFF_MASK;
        can_pdu.length      = fd_frame.len;
        can_pdu.sdu_ptr     = fd_frame.data;
        
        if(msg.msg_flags == MSG_CONFIRM)
        {// tx confirmation
          rx_callbck_e->txConfirmation(can_hoh_id_e, &can_pdu);
        }
        else
        {// rx indication
          rx_callbck_e->rxIndication(can_hoh_id_e, &can_pdu);
        }
      }
      else
      {/* incomplete frame */        
      }
    }
  }
}


} // can
} // libSocket
} // libLinux
} // libOsAbstraction

