
// includes
#include "can_Prv.h"

namespace libOsAbstraction {
namespace libLinux {
namespace libSocket {
namespace can {

// ctor
CANDriver::CANDriver(Can_ControllerIdType tot_num_controller,
                     Can_HwHandleType tot_num_hoh)
{
  // reserve space for controller vector
  can_controller_box_e.reserve(tot_num_controller);
  // reserve space for hoh vector
  can_hoh_box_e.reserve(tot_num_hoh);
}

// dtor
CANDriver::~CANDriver()
{
    
}

// func to create actual controller (name = "can0"),return controller interface number or -1 on failure
// @return : E_OK , E_NOT_OK
Std_ReturnType CANDriver::CAN_CreateController(const char* name, 
                                               const Can_BitTimingType* bit_timeConf,
                                               Can_ControllerIdType* can_controllr_id)
{
  Std_ReturnType retval                                     = E_NOT_OK;
  Can_ControllerIdType controller_id                        = 0xFF;

  //Check for null pointer
  if((name != nullptr)
      &&(bit_timeConf != nullptr)
      &&(can_controllr_id != nullptr))
    {// get the controller id
      controller_id = CAN_GetControllerId(name);
      // verify whether controller exists ?
      if(controller_id == 0xFF)
        {// no controller with name present, create a new one
          // successfully set, store the object
          for(std::uint8_t indx = 0; indx < can_controller_box_e.size(); indx ++)
            {
              if(can_controller_box_e[indx] == nullptr)
                {// empty ,use it
                  can_controller_box_e[indx] = std::make_unique<createCANControllerHandle>(name, (Can_ControllerIdType)indx);
                  // set bit timings
                  if(can_controller_box_e[indx]->CAN_SetBitTiming(bit_timeConf) == E_OK)
                    {
                      *can_controllr_id = (Can_ControllerIdType)indx;
                      can_controller_count ++;
                      retval = E_OK;
                    }
                  else
                    {
                      can_controller_box_e[indx].reset(nullptr);
                    }
                    break;
                }
            }
        }
      else
        {// already created, return the Id
          *can_controllr_id = controller_id;
          retval = E_OK;
        }
    }
  else
    {// E_NOT_OK
    }
  return retval;
}

// func to set Controller mode - start / stop actual CAN controller
// @return : E_OK , E_NOT_OK
Std_ReturnType CANDriver::CAN_SetControllerMode(Can_ControllerIdType can_controllr_id, 
                                                Can_ControllerStateType mode)
{
  Std_ReturnType retval              = E_NOT_OK;
  // Is controller id valid ?
  if(CAN_ControllerIsValid(can_controllr_id) == E_OK)
    {
      switch (mode)
      {/* Check for modes */
        case Can_ControllerStateType::kStart:
          // start the controller
          retval = can_controller_box_e[can_controllr_id]->CAN_SetControllerStart();
          break;
        case Can_ControllerStateType::kStop:
          // stop the controller
          retval = can_controller_box_e[can_controllr_id]->CAN_SetControllerStop();
          break;
        default:
          retval = E_NOT_OK;
          break;
      }
    }
  return retval;
}

// func to destroy actual controller
// @return : E_OK , E_NOT_OK
Std_ReturnType CANDriver::CAN_DestroyController(Can_ControllerIdType can_controllr_id)
{
  Std_ReturnType retval              = E_NOT_OK;
  /* Find the controller based on id */
  if(CAN_ControllerIsValid(can_controllr_id) == E_OK)
    {
      can_controller_box_e[can_controllr_id].reset(nullptr);
      retval = E_OK;
    }

  return retval;
}

// func to create HOH based on CAN controller interface number
// @return : E_OK , E_NOT_OK
Std_ReturnType CANDriver::CAN_CreateHardwareObjectHandle(Can_HwType* can_hwType, Can_HwHandleType* hoh_id)
{
  Std_ReturnType retval                                  = E_NOT_OK;
  
  // Null pointer check
  if((can_hwType != nullptr)
      &&(hoh_id != nullptr))
    {// check if controller valid ?
      if(CAN_ControllerIsValid(can_hwType->can_controllr_id) == E_OK)
        {// valid, search for free hoh
          for(std::uint8_t indx = 0; indx < can_hoh_box_e.size(); indx ++)
            {
              if(can_hoh_box_e[indx] == nullptr)
                {// empty, create new                  
                  can_hoh_box_e[indx] = std::make_unique<createCANHardwareObjectHandle>(can_hwType, (Can_HwHandleType)indx,
                                                                                        can_controller_box_e[can_hwType->can_controllr_id].get());
                  // opening success ?
                  if(can_hoh_box_e[indx]->CAN_OpenCANHardwareObjectHandle() == E_OK)
                    {
                      *hoh_id = (Can_HwHandleType)indx;
                      can_hoh_count ++;
                      retval = E_OK;
                    }
                  else
                    {// destroy created object
                      can_hoh_box_e[indx].reset(nullptr);
                    }
                    break;
                }
              else
                {// retval = E_NOT_OK
                }
            }
        }
      else
        {// retval = E_NOT_OK
        }
    }
  else
    {// retval = E_NOT_OK
    }

  return retval;
}

// func to destroy HOH
// @return : E_OK , E_NOT_OK
Std_ReturnType CANDriver::CAN_DestroyHardwareObjectHandle(Can_HwHandleType hoh_id)
{
  Std_ReturnType retval              = E_NOT_OK;
  // Is HOH valid ?
  if(CAN_HOHIsValid(hoh_id) == E_OK)
    {
      if(can_hoh_box_e[hoh_id]->CAN_CloseCANHardwareObjectHandle() == E_OK)
      {// closure done
        can_hoh_box_e[hoh_id].reset(nullptr);
        retval = E_OK;
      }      
    }
  else
    {// E_NOT_OK
    }
  return retval;
}

// func to register TxConfirmation and Reception Callback to HOH
// @return : E_OK , E_NOT_OK
Std_ReturnType CANDriver::CAN_HOH_RegisterRxCallback(Can_HwHandleType hoh_id,
                                                     Can_RxCallbckType* rx_callbck)
{
  Std_ReturnType retval              = E_NOT_OK;
  // null pointer check
  if(rx_callbck != nullptr)
    {// Is HOH valid ?
      if(CAN_HOHIsValid(hoh_id) == E_OK)
        {
          retval = can_hoh_box_e[hoh_id]->CAN_RegisterRxCallback(rx_callbck);
        }
      else
        {// E_NOT_OK
        }
    }
  else
    {// E_NOT_OK
    }
  return retval;
}

// func to set CAN hardware filter
// @return : E_OK, E_NOT_OK
Std_ReturnType CANDriver::CAN_HOH_SetHardwareFilter(Can_HwHandleType hoh_id, Can_HwFilterType* filter, std::uint8_t tot_num_filter)
{
  Std_ReturnType retval              = E_NOT_OK;
  // null pointer check
  if(filter != nullptr)
    {// Is HOH valid ?
      if(CAN_HOHIsValid(hoh_id) == E_OK)
        {
          retval = can_hoh_box_e[hoh_id]->CAN_SetHardwareFilter(filter, tot_num_filter);
        }
      else
        {// E_NOT_OK
        }
    }
  else
    {// E_NOT_OK
    }
  return retval;
}

// func to place transmit request to HOH
// @return : E_OK , E_NOT_OK , E_BUSY (Retry)
Std_ReturnType CANDriver::CAN_HOH_TransmitRequest(Can_HwHandleType hth_id,
                                       const Can_PduType* pduInfo,
                                       std::uint8_t num_of_frame)
{
  Std_ReturnType retval              = E_NOT_OK;
  // null pointer check
  if((pduInfo != nullptr)
    &&(pduInfo->sdu_ptr != nullptr))
    {// Is HOH valid ?
      if(CAN_HOHIsValid(hth_id) == E_OK)
        {
          retval = can_hoh_box_e[hth_id]->CAN_Write(pduInfo, num_of_frame);
        }
      else
        {// E_NOT_OK
        }
    }
  else
    {// E_NOT_OK
    }
  return retval;
}

// func to get controller id based on controller name
Can_ControllerIdType CANDriver::CAN_GetControllerId(const char* name)
{
  Can_ControllerIdType retval = 0xFF;

  return retval;
}

// func to check whether controller id is valid or not
Std_ReturnType CANDriver::CAN_ControllerIsValid(Can_ControllerIdType can_controllr_id)
{
  Std_ReturnType retval = E_NOT_OK;
  if(can_controller_box_e[can_controllr_id] != nullptr)
    {
      if(can_controller_box_e[can_controllr_id]->CAN_GetControllerId() == can_controllr_id)
        {// matches with id
          retval = E_OK;
        }
    }
  return retval;
}

// func to check valid HOH handle
Std_ReturnType CANDriver::CAN_HOHIsValid(Can_HwHandleType hoh_id)
{
  Std_ReturnType retval = E_NOT_OK;
  if(can_hoh_box_e[hoh_id] != nullptr)
    {
      if(can_hoh_box_e[hoh_id]->CAN_GetCANHardwareObjectHandleId() == hoh_id)
        {// matches with id
          retval = E_OK;
        }
    }
  return retval;
}

} // can
} // libSocket
} // libLinux
} // libOsAbstraction
