#ifndef LIB_LINUX_SOCKET_CAN_TYPES_H
#define LIB_LINUX_SOCKET_CAN_TYPES_H

// includes
#include "common_Header.h"

namespace libOsAbstraction {
namespace libLinux {
namespace libSocket {
namespace can {

// CAN controller modes
enum class Can_ControllerStateType : std::uint8_t
{
  // state
  kUninit               = 0x00,
  kInit,                                /* Initialization completed */
  kStart,                               /* start the controller */
  kStop,                                /* stop the controller  */
  kSleep
};

// CAN Controller error Modes
enum class Can_ErrorStateType : std::uint8_t
{
  kError_NA           = 0x00,
  kError_Active,
  kError_Warning,
  kError_Passive,
  kError_BusOff,
  kError_Stopped,
  kError_Sleeping
};

// CAN hardware Object type
enum class Can_HOType : std::uint8_t
{
  kReceive              = 0x00,
  kTransmit
};

// CAN Controller BitTiming config
typedef struct
{
  std::uint32_t         bitrate;          /* Bit-rate in bits/second */
  std::uint32_t         sample_point;     /* Sample point in one-tenth of a percent */
  std::uint32_t         tq;               /* Time quanta (TQ) in nanoseconds */
  std::uint32_t         prop_seg;         /* Propagation segment in TQs */
  std::uint32_t         phase_seg1;       /* Phase buffer segment 1 in TQs */
  std::uint32_t         phase_seg2;       /* Phase buffer segment 2 in TQs */
  std::uint32_t         sjw;              /* Synchronisation jump width in TQs */
  std::uint32_t         brp;              /* Bit-rate prescaler */
}Can_BitTimingType;

// CAN Error Counter
typedef struct
{
  std::uint16_t         txerr;
  std::uint16_t         rxerr;
}Can_ErrCounterType;

// CAN Controller Id type 
typedef std::uint8_t Can_ControllerIdType;

// CAN ID Type
typedef std::uint32_t Can_IdType;

// CAN Hardware Object handle type
typedef std::uint16_t Can_HwHandleType;

// CAN Frame type
enum class Can_FrameType : std::uint8_t
{
  kStandard_Normal     = 0x00,
  kStandard_Fd,
  kExtended_Normal,
  kExtended_Fd
};


// CAN Hardware Type
typedef struct
{
  Can_IdType            can_id_type;            /* Can Identification Type - Standard / Extended */
  std::uint32_t         can_rx_pollng_time_ms;  /* Rx polling time - in ms*/
  Can_ControllerIdType  can_controllr_id;       /* Actual CAN controller Id */
  Can_HOType            can_ho_type;            /* Transmit or Receive */
  bool                  canfd_enable;           /* Flag to indicate support of CAN-FD */
}Can_HwType;

// CAN PDU info type
typedef struct
{
  Can_IdType            id;
  Can_FrameType         frametype;
  std::uint8_t          length;
  std::uint8_t*         sdu_ptr;
}Can_PduType;

// CAN Hardware Filter
typedef struct
{
  Can_IdType            can_id;
  Can_IdType            can_mask;
}Can_HwFilterType;

// Function pointer definition
typedef void (*Can_RxIndication_FP_Type)(const Can_HwHandleType hoh_id, const Can_PduType* pduInfo);
typedef void (*Can_TxConfirmation_FP_Type)(const Can_HwHandleType hoh_id, const Can_PduType* pduInfo);

// CAN Rx Callback Type
typedef struct 
{
  Can_RxIndication_FP_Type    rxIndication;
  Can_TxConfirmation_FP_Type  txConfirmation;
}Can_RxCallbckType;

// CAN Error Types
constexpr uint8_t kCan_Error_Bit_Monitoring1               = 0x1;
constexpr uint8_t kCan_Error_Bit_Monitoring0               = 0x2;
constexpr uint8_t kCan_Error_Bit                           = 0x3;
constexpr uint8_t kCan_Error_Check_Ack_Failed              = 0x4;
constexpr uint8_t kCan_Error_Ack_Delimiter                 = 0x5;
constexpr uint8_t kCan_Error_Arbitration_Lost              = 0x6;
constexpr uint8_t kCan_Error_Overload                      = 0x7;
constexpr uint8_t kCan_Error_Check_Form_Failed             = 0x8;
constexpr uint8_t kCan_Error_Check_Stuffing_Failed         = 0x9;
constexpr uint8_t kCan_Error_Check_CRC_Failed              = 0xA;
constexpr uint8_t kCan_Error_Bus_Lock                      = 0xB;

} // can
} // libSocket
} // libLinux
} // libOsAbstraction

#endif // LIB_LINUX_SOCKET_CAN_TYPES_H
