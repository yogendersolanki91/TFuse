TFuse - FUSE File system implementaion with Thrift for Windows/Unix(Todo)/OSX(Todo)
------------------------------------------
[TODO]

The purpose is to mount anything, written in any langugage on any platform servered over varied of protocol/transport. File system could be hosted locally or a remove file system should also work fine.

- It supports Everyhing which thrift supports.

- Targeted for
	- Windows using WinFSP
	- Linux using FUSE
	- Mac OSX using FUSE

- Hosted Over
	- Remote - TCP/IP, HTTP/JSON,
	- Locally - PIPE/SOCKET

- Advantage -
    - Only dedpendecy is Thrift for development.
	- Use all languages supported with the thrift intface defined for FUSE.