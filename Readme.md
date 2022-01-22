TFuse - FUSE File system implementaion with Thrift for Windows / Linux/ OSX
=======================================================
There are multiple projects across different platforms that allows you to write your own file system. But when it comes to write a file system backend and use or access it from anywhere, we does not have many option. There are constraints in terms of backend implementation and user space vs kernel space.

This project focuses on all above problem by wrapping different file system implementation with FUSE and allow it to be serve our choice of transport. Basically use the THRIFT for FUSE (hence TFuse).

From implementation perspective, It has Fuse.thrift IDL which could be used for file system stub generation in any language supported by the Thrift library.

Documentation 
-----------------------------------
TBD:


Feature
-------------------

- Supports Local/Remote File system.
- Work with any file system backend implementation with thrift IDL for Fuse (`Fuse.thrift`).
- Configurable file system host.


Status 
-------------------

- Windows - Beta, Working on it.
- UNIX - WIP
- OSX - Need helping hand.



Performance Comparision 
-------------------


Licencse
-------------------
- `Fuse.thrift` IDL -  Apahce 2.0 
- `TFuse` -  GPLv3

and all linked project comes with their respective license.