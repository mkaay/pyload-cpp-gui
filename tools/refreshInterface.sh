#!/bin/sh
TOOLS_PATH="`dirname \"$0\"`"
wget --no-check-certificate http://bitbucket.org/spoob/pyload/raw/tip/module/remote/thriftbackend/pyload.thrift -O $TOOLS_PATH/../src/thrift/interface/pyload.thrift