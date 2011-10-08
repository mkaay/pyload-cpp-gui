#!/bin/sh
TOOLS_PATH="`dirname \"$0\"`"
rm -r $TOOLS_PATH/../src/thrift/interface
thrift -strict -v --gen cpp $TOOLS_PATH/../src/thrift/pyload.thrift
mv gen-cpp $TOOLS_PATH/../src/thrift/interface
rm $TOOLS_PATH/../src/thrift/interface/Pyload_server.skeleton.cpp
