#ifndef  __PRODUCT_H__
#define  __PRODUCT_H__


#include <stdint.h>
/*
*	0：433单向
*	1：433双向有行程电机
*	2：433双向无行程电机
*	3：其他有行程电机机(如wifi管状电机)
*/
#define Product_wirelessMode		3

#define DP_OPERATION    "operation"
#define DP_CURRENT_POS  "currentPosition"
#define DP_TARGET_POS   "targetPosition"
#define DP_DIRECTION    "direction"
#define DP_CURRENT_STATE	"currentState"
#define curWifiRssi	"RSSI"
#define wirelessMode	"wirelessMode"


typedef struct tagDT72_DP{


    uint8_t operation;
    uint8_t currentPosition;
    uint8_t targetPosition;
    uint8_t direction;
    uint8_t currentState;
    

}DT72_DP;

typedef enum
{
	DP_OPS_ERR = -1,
	DP_OPS_BEGIN_CONTROL,
	DP_OPS_CLOSE_CURTAIN = DP_OPS_BEGIN_CONTROL,
	DP_OPS_OPEN_CURTAIN,
	DP_OPS_PAUSE_CURTAIN,
	DP_OPS_REVERSE,
	DP_OPS_JOURNEY,
	DP_OPS_REPORT,
	DP_OPS_PointMoveUp = 7,
	DP_OPS_PointMoveDn,
	MAX_DP_OPS
} dt72_allow_ops;


typedef enum
{
	DP_DIRECTION_ERR = -1,
	DP_DIRECTION_BEGIN_CONTROL,
	DP_DIRECTION_NORMAL = DP_DIRECTION_BEGIN_CONTROL,
	DP_DIRECTION_REVERSE,
	DP_DIRECTION_NO_CALIBRITION,
	MAX_DP_DIRECTION
} dt72_allow_direction;

void curtainPauseSet();
void curtainMsgReport();
void product_init();
void product_led_set(int led_status );
void product_fac_test();

#endif
