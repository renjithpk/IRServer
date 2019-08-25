DATA=$1
EP="code"
export IO_USERNAME="renjith83"
export IO_KEY="442ba7a9e7cd47e186f1fffc91d321f6"
curl -F "value=${DATA}" -H "X-AIO-Key: ${IO_KEY}" https://io.adafruit.com/api/v2/${IO_USERNAME}/feeds/irsever.${EP}/data
