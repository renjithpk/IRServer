LIMIT=$1
export IO_USERNAME="renjith83"
export IO_KEY="442ba7a9e7cd47e186f1fffc91d321f6"
if [ ! -z $LIMIT ]; then
    QUERY="?limit=${LIMIT}"
fi

curl -H "X-AIO-Key: ${IO_KEY}" https://io.adafruit.com/api/v2/${IO_USERNAME}/feeds/irsever.decoded/data${QUERY}
