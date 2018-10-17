#ifndef  __PRODUCT_H__
#define  __PRODUCT_H__


#include <stdint.h>

#define DP_OPERATION    "operation"
#define DP_CURRENT_POS  "currentPosition"
#define DP_TARGET_POS   "targetPosition"
#define DP_DIRECTION    "direction"
#define DP_CURRENT_STATE	"currentState"
#define curWifiRssi	"RSSI"


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

void curtainMsgReport();
void product_init();
void product_led_set(int led_status );
void product_fac_test();

#endif
