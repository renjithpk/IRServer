if [ -z $1 ]; then
    echo "

typedef enum
{
 ir_decode_cmd = 'd',
 ir_send_cmd = 's',
 toggle_dbg_cmd = 'x',
}ECmd;

typedef enum
{
 Idle_State,
 ir_decode_State,
 ir_RCV_State,
 wifi_reset_State,
 ir_server_State,
}EServerState;

"
    exit 0
fi
export IO_USERNAME="renjith83"
export IO_KEY="442ba7a9e7cd47e186f1fffc91d321f6"
curl -F "value=$1" -H "X-AIO-Key: ${IO_KEY}" https://io.adafruit.com/api/v2/${IO_USERNAME}/feeds/irsever.config/data
