#!/bin/sh

# create the c interface and immplementation from .proto files
#

# @author Attilio Dona'

PROTOBUF_SRC=$1
PROTO_PATH=/home/adona/ZAKKE/burba/cfg/protoc

echo dir: $PROTOBUF_SRC
protoc-c --c_out=$PROTOBUF_SRC --proto_path=$PROTO_PATH /home/adona/ZAKKE/burba/cfg/protoc/messages.proto

protoc -I=$PROTO_PATH --python_out=/home/adona/ZAKKE/burba/lib/pyproto $PROTO_PATH/messages.proto
