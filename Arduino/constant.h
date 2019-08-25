#ifndef __constant_h_
#define __constant_h_
typedef enum
{
 Idle_State,
 ir_decode_State,
 ir_RCV_State,
 wifi_reset_State,
 ir_server_State,
}EServerState;

typedef enum
{
 ir_decode_cmd = 'd',
 ir_send_cmd = 's',
 toggle_dbg_cmd = 'x',
}ECmd;

#endif