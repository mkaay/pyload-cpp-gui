#!/bin/sh
rm -rf thriftinterface
thrift -strict -v --gen cpp pyload.thrift
mv gen-cpp thriftinterface
rm thriftinterface/Pyload_server.skeleton.cpp
