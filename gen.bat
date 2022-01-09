set THRIFT_FUSE="C:\Workshop\Source\ThriftFuse"
set THRIFTC_PATH="C:\DevTools\vcpkg\vcpkg\installed\x86-windows\tools\thrift\thrift.exe"
del /S /q %THRIFT_FUSE%\TFuse\gen-cpp\*
del /S /q %THRIFT_FUSE%\TFuseMem\gen-netstd\*
%THRIFTC_PATH% -r -debug --gen cpp --out %THRIFT_FUSE%\TFuse\gen-cpp\ %THRIFT_FUSE%\Fuse.thrift
%THRIFTC_PATH% -r -debug --gen netstd --out %THRIFT_FUSE%\TFuseMem\gen-netstd\ %THRIFT_FUSE%\Fuse.thrift