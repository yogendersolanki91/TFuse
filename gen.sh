#!/bin/sh

# comment below line, once done
echo "Please update the Source root THRIFT_FUSE  and Compiler Path THRIFTC_PATH"
exit 1

export THRIFT_FUSE="C:\Workshop\Source\ThriftFuse"
export THRIFTC_PATH="C:\DevTools\vcpkg\vcpkg\installed\x86-windows\tools\thrift\thrift.exe"

$THRIFTC_PATH -r -debug --gen cpp --out $THRIFT_FUSE/ThriftFuse/gen-cpp/ $THRIFT_FUSE/Fuse.thrift
$THRIFTC_PATH -r -debug --gen netstd --out $THRIFT_FUSE/ThriftFuseMem/gen-netstd/ $THRIFT_FUSE/Fuse.thrift