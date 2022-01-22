/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2022 Yogender Solanki
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *****************************************************************************
 */

namespace cpp Fuse

struct KeyValuePair {
    1:string key,
    2:string val
}


struct FuseContext {    
    1:optional i32 uid;
    2:optional i32 gid;
    3:optional i32 pid;    
    4:optional i32 umask;
}

enum FuseAccessMode {
    F_OK = 0;
    X_OK = 0x01;
    W_OK = 0x02;
    R_OK = 0x04;
}


// File mode flags

const i32  FUSE_MODE_MASK_IFMT=0xF000;
const i32  FUSE_MODE_MASK_IFSOC=0xC000;
const i32  FUSE_MODE_MASK_IFLNK=0xA000;
const i32  FUSE_MODE_MASK_IFREG=0x8000;
const i32  FUSE_MODE_MASK_IFBLK=0x6000;
const i32  FUSE_MODE_MASK_IFDIR =0x4000;
const i32  FUSE_MODE_MASK_IFCHR=0x2000;
const i32  FUSE_MODE_MASK_IFIFO=0x1000;
const i32  FUSE_MODE_MASK_ISUID=0x800;
const i32  FUSE_MODE_MASK_ISGID=0x400;
const i32  FUSE_MODE_MASK_ISVTX=0x200;


//
// Permission Masks
//

// Read-Write-Execute User permission
const i32  FUSE_MODE_MASK_IRWXU=0x1C0;
const i32  FUSE_MODE_MASK_IRUSR=0x100;
const i32  FUSE_MODE_MASK_IWUSR=0x80;
const i32  FUSE_MODE_MASK_IXUSR=0x40;

// Read-Write-Execute Owner Group permission
const i32  FUSE_MODE_MASK_IRWXG=0x38;
const i32  FUSE_MODE_MASK_IRGRP=0x20;
const i32  FUSE_MODE_MASK_IWGRP=0x10;
const i32  FUSE_MODE_MASK_IXGRP=0x8;

// Read-Write-Execute Others permission
const i32  FUSE_MODE_MASK_IRWXO=0x7;
const i32  FUSE_MODE_MASK_IROTH=0x4;
const i32  FUSE_MODE_MASK_IWOTH=0x2;
const i32  FUSE_MODE_MASK_IXOTH=0x1;

typedef list<KeyValuePair> KVList
typedef list<string> StringArray

enum FuseFSFlags {
  FUSE_ST_RDONL = 0x0001; /* mount read-only */
  FUSE_ST_NOSUID = 0x0002; /* ignore suid and sgid bits */
  FUSE_ST_NODEV = 0x0004; /* disallow access to device special files */
  FUSE_ST_NOEXEC = 0x0008; /* disallow program execution */
  FUSE_ST_SYNCHRONOUS = 0x0010; /* writes are synced at once */
  FUSE_ST_VALID = 0x0020; /* f_flags support is implemented */
  FUSE_ST_MANDLOCK = 0x0040; /* allow mandatory locks on an FS */
  FUSE_ST_NOATIME = 0x0400; /* do not update access times */
  FUSE_ST_NODIRATIME = 0x0800; /* do not update directory access times */
  FUSE_ST_RELATIME = 0x1000; /* update atime relative to mtime/ctime */
  FUSE_ST_NOSYMFOLLOW = 0x2000; /* do not follow symlinks */
}

struct FuseHandleInfo {
    1: optional i32 flags;
    2: optional i32 writepage;
    3: optional i32 direct_io;
    4: optional i32 keep_cache;
    5: optional i32 flush;
    6: optional i32 nonSeekable;
    7: optional i64 fh;
    8: optional i64 lock_owner;
    9: optional i32 poll_events;
    10: optional i64 padding;
}

struct FuseStat {
    1:optional i32 dev;
    2:optional i64 ino;
    3:optional i32 mode;
    4:optional i32 nlink;
    5:optional i32 uid;
    6:optional i32 gid;
    7:optional i32 rdev;
    8:optional i64 size;
    9:optional i32 blksize;
    10:optional i64 blocks;
    11:optional i32 accessTime;
    12:optional i32 modificationTime;
    13:optional i32 changeTime;
}

struct FuseStatFS {
    1:optional i32 bSize;
    2:optional i32 frSize;
    3:optional i32 blocks;
    4:optional i32 bfree;
    5:optional i32 bavail;
    6:optional i32 files;
    7:optional i32 free;
    8:optional i32 favail;
    9:optional i32 fsid;
    10:optional FuseFSFlags flags;
    11:optional i32 namemax;
}

enum FileLock {
    FUSE_F_RDLCK = 1;
    FUSE_F_WRLCK	= 2;
    FUSE_F_UNLCK	= 8;
}

enum Seek {
    FUSE_SEEK_SET = 0;
    FUSE_SEEK_CUR = 1;
    FUSE_SEEK_END = 2;
}


struct FuseFlock {
    1:optional  FileLock type;
    2:optional  Seek whence;
    3:optional  i64 start;
    4:optional  i64 len;
    5:optional  i64 pid;
}

struct FuseDirEntry {
    1:optional  string name;
    2:optional  FuseStat stats;
}

typedef list <FuseDirEntry> DirEntryList;

enum StatusCode {
  FUSE_SUCCESS = 0; /* Operation not permitted */
  FUSE_ERROREPERM = 1; /* Operation not permitted */
  FUSE_ERRORENOENT = 2; /* No such file or directory */
  FUSE_ERRORESRCH = 3; /* No such process */
  FUSE_ERROREINTR = 4; /* Interrupted system call */
  FUSE_ERROREIO = 5; /* I/O error */
  FUSE_ERRORENXIO = 6; /* No such device or address */
  FUSE_ERRORE2BIG = 7; /* Argument list too long */
  FUSE_ERRORENOEXEC = 8; /* Exec format error */
  FUSE_ERROREBADF = 9; /* Bad file number */
  FUSE_ERRORECHILD = 10; /* No child processes */
  FUSE_ERROREAGAIN = 11; /* Try again */
  FUSE_ERRORENOMEM = 12; /* Out of memory */
  FUSE_ERROREACCES = 13; /* Permission denied */
  FUSE_ERROREFAULT = 14; /* Bad address */
  FUSE_ERRORENOTBLK = 15; /* Block device required */
  FUSE_ERROREBUSY = 16; /* Device or resource busy */
  FUSE_ERROREEXIST = 17; /* File exists */
  FUSE_ERROREXDEV = 18; /* Cross-device link */
  FUSE_ERRORENODEV = 19; /* No such device */
  FUSE_ERRORENOTDIR = 20; /* Not a directory */
  FUSE_ERROREISDIR = 21; /* Is a directory */
  FUSE_ERROREINVAL = 22; /* Invalid argument */
  FUSE_ERRORENFILE = 23; /* File table overflow */
  FUSE_ERROREMFILE = 24; /* Too many open files */
  FUSE_ERRORENOTTY = 25; /* Not a typewriter */
  FUSE_ERRORETXTBSY = 26; /* Text file busy */
  FUSE_ERROREFBIG = 27; /* File too large */
  FUSE_ERRORENOSPC = 28; /* No space left on device */
  FUSE_ERRORESPIPE = 29; /* Illegal seek */
  FUSE_ERROREROFS = 30; /* Read-only file system */
  FUSE_ERROREMLINK = 31; /* Too many links */
  FUSE_ERROREPIPE = 32; /* Broken pipe */
  FUSE_ERROREDOM = 33; /* Math argument out of domain of func */
  FUSE_ERRORERANGE = 34; /* Math result not representable */
  FUSE_ENOTEMPTY = 39; /*Directory is not empty*/
  FUSE_ERRECANCELED = 158;
  
}

struct FuseConnectionInfo {
    1: optional i64 proto_major;
    2: optional i64 proto_minor;
    3: optional i64 max_write;
    4: optional i64 max_read;
    5: optional i64 max_readahead;
    6: optional i64 capable;
    7: optional i64 want;
    8: optional i64 max_background;
    9: optional i64 congestion_threshold;
    10: optional i64 time_gran;    
}

struct FuseConfig {
    1: optional i64 set_gid;
    2: optional i64 set_uid;
    3: optional i64 set_mode;
    4: optional i64 entry_timeout;
    5: optional i64 negative_timeout;
    6: optional i64 attr_timeout;
    7: optional i64 intr;
    8: optional i64 intr_singnal;
    9: optional i64 remember;
    10: optional i64 hard_remove;
    11: optional i64 use_ino;
    12: optional i64 readdir_ino;    
    13: optional i64 direct_io;
    14: optional i64 kernel_cache;
    15: optional i64 auto_cache;
    16: optional i64 ac_att_timeout_set;
    17: optional i64 nullpath_ok;
    18: optional i64 show_help
}

struct FuseTimeSpec {
   1: optional i32 accessTime;
   2: optional i32 modificationTime;
}

struct FileSystemResponse {
    1: required StatusCode status;
    2: optional FuseHandleInfo info;
    3: optional FuseStat stats;
    4: optional string linkPath;
    5: optional binary data;
    6: optional FuseStatFS statfs;
    7: optional string atrributeValue;
    8: optional StringArray attributes;
    9: optional i64 dataWritten;    
    10: optional DirEntryList dirEntry;
    11: optional FileLock flock;
    12: optional i64 blockIndex;
}

service FuseService {
  
   /*
    * int(* 	getattr )(const char *, struct stat *, struct fuse_file_info *fi)
    * Return file attributes. The "stat" structure is described in detail in the stat(2) manual page. 
    * For the given pathname, this should fill in the elements of the "stat" structure. If a field is 
    * meaningless or semi-meaningless (e.g., st_ino) then it should be set to 0 or given a "reasonable" value. 
    * This call is pretty much required for a usable filesystem.
    */
   FileSystemResponse getattr(1:string path, 2:FuseHandleInfo handleInfo, 3:FuseContext context);
  
   /*
    * readlink(const char* path, char* buf, size_t size)
    * If path is a symbolic link, fill buf with its target, up to size. 
    * See readlink(2) for how to handle a too-small buffer and for error codes. Not required if you don't support symbolic links. 
    * NOTE: Symbolic-link support requires only readlink and symlink. FUSE itself will take care of tracking symbolic links in paths, 
    *  so your path-evaluation code doesn't need to worry about it.
    */
   FileSystemResponse readlink(1:string path, 2:i32 maxSize, 3:FuseContext context);
  
   /*
   * mkdir(const char* path, mode_t mode)
   * Create a directory with the given name. The directory permissions are encoded in mode. 
   * See mkdir(2) for details. This function is needed for any reasonable read/write filesystem.
   */
   FileSystemResponse mkdir(1:string path, 2:i32 mode, 3:FuseContext context);
  
   /*
   * unlink(const char* path)
   * Remove (delete) the given file, symbolic link, hard link, or special node. 
   * Note that if you support hard links, unlink only deletes the data when the last hard link is removed. 
   * See unlink(2) for details.
   */
   FileSystemResponse unlink(1:string path, 2:FuseContext context);
   
   /*
   * rmdir(const char* path) 
   * Remove the given directory. This should succeed only if the directory is empty (except for "." and ".."). 
   * See rmdir(2) for details.
   */
   FileSystemResponse rmdir(1:string path, 2:FuseContext context);

   /*
   * symlink(const char* to, const char* from)
   * Create a symbolic link named "from" which, when evaluated, will lead to "to". 
   * Not required if you don't support symbolic links. NOTE: Symbolic-link support requires only readlink and symlink.
   * FUSE itself will take care of tracking symbolic links in paths, so your path-evaluation code doesn't need to worry about it.
   */
   FileSystemResponse symlink(1:string destination, 2:string source, 3:FuseContext context);
   
   /*
   * int(* 	rename )(const char *, const char *, unsigned int flags)
   * Rename the file, directory, or other object "from" to the target "to". 
   * Note that the source and target don't have to be in the same directory, 
   * so it may be necessary to move the source to an entirely new directory. See rename(2) for full details.
   */
   FileSystemResponse rename(1:string source, 2:string destination, 3:i64 flags, 4:FuseContext context);

   /*
   * link(const char* from, const char* to)
   * Create a hard link between "from" and "to". Hard links aren't required for a working filesystem, 
   * and many successful filesystems don't support them. If you do implement hard links, be aware that 
   * they have an effect on how unlink works. See link(2) for details.
   */
   FileSystemResponse link(1:string to, 2:string destination, 3:FuseContext context);
   
   /*
   * int(* 	chown )(const char *, uid_t, gid_t, struct fuse_file_info *fi) 
   * Change the mode (permissions) of the given object to the given new permissions. 
   * Only the permissions bits of mode should be examined. See chmod(2) for details.
   */
   FileSystemResponse chmod(1:string path, 2:i32 mode, 3: FuseHandleInfo handleInfo, 4:FuseContext context);  
   
   /*
   * int(* 	chown )(const char *, uid_t, gid_t, struct fuse_file_info *fi)
   * Change the given object's owner and group to the provided values. 
   * See chown(2) for details. NOTE: FUSE doesn't deal particularly well with file ownership, 
   * since it usually runs as an unprivileged user and this call is restricted to the superuser. 
   * It's often easier to pretend that all files are owned by the user who mounted the filesystem, 
   * and to skip implementing this function.
   */
   FileSystemResponse chown(1:string path, 2:i32 uid, 3:i32 gid, 4: FuseHandleInfo handleInfo, 5:FuseContext context);
   
   /*
   * int(* 	truncate )(const char *, off_t, struct fuse_file_info *fi)
   * Truncate or extend the given file so that it is precisely size bytes long. See truncate(2) for details. 
   * This call is required for read/write filesystems, because recreating a file will first truncate it.
   */
   FileSystemResponse truncate(1:string path, 2:i64 offset, 3: FuseHandleInfo handleInfo, 4:FuseContext context);
   
   /*
   * open(const char* path, struct fuse_file_info* fi)
   * Open a file. If you aren't using file handles, this function should just check for existence and permissions and 
   * return either success or an error code. If you use file handles, you should also allocate any necessary structures and set fi->fh. 
   * In addition, fi has some other fields that an advanced filesystem might find useful; see the structure definition in fuse_common.h 
   * for very brief commentary.
   */
   FileSystemResponse open(1:string path, 2:FuseContext context);
   
   /*
   * read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
   * Read sizebytes from the given file into the buffer buf, beginning offset bytes into the file. See read(2) for full details. Returns the number of bytes transferred, or 0 if offset was at or beyond the end of the file. Required for any sensible filesystem.
   */
   FileSystemResponse read(1:string path, 2:i32 size, 3:i64 offset, 4: FuseHandleInfo handleInfo, 5:FuseContext context);
   
  /*
   * int(* 	write )(const char *, const char *, size_t, off_t, struct fuse_file_info *)
   * As for read above, except that it can't return 0.
   */
   FileSystemResponse write(1:string path, 2:binary buffer, 3:i64 offset, 4:i32 size, 5: FuseHandleInfo handleInfo, 6:FuseContext context);
   
   /*
   * statfs(const char* path, struct statvfs* stbuf
   * Return statistics about the filesystem. See statvfs(2) for a description of the structure contents. Usually, you can ignore the path. Not required, but handy for read/write filesystems since this is how programs like df determine the free space.
   */
   FileSystemResponse statfs(1:string path, 2:FuseContext context);
   

  /*
   * int(* 	flush )(const char *, struct fuse_file_info *)
   * Possibly flush cached data
   * BIG NOTE: This is not equivalent to fsync(). It's not a request to sync dirty data. 
   * Flush is called on each close() of a file descriptor, as opposed to release which is called on the close of the last file descriptor for a file. Under Linux, 
   * errors returned by flush() will be passed to userspace as errors from close(), so flush() is a good place to write back any cached dirty data. 
   * However, many applications ignore errors on close(), and on non-Linux systems, close() may succeed even if flush() returns an error. For these reasons, 
   * filesystems should not assume that errors returned by flush will ever be noticed or even delivered.
   * NOTE: The flush() method may be called more than once for each open(). This happens if more than one file descriptor refers to an open file handle, 
   * e.g. due to dup(), dup2() or fork() calls. It is not possible to determine if a flush is final, so each flush should be treated equally. 
   * Multiple write-flush sequences are relatively rare, so this shouldn't be a problem.
   * Filesystems shouldn't assume that flush will be called at any particular point. It may be called more times than expected, or not at all.
   */
   FileSystemResponse flush(1:string path, 2:FuseHandleInfo handleInfo, 3:FuseContext context)

   /*
   * release(const char* path, struct fuse_file_info *fi)
   * This is the only FUSE function that doesn't have a directly corresponding system call, although close(2) is related. Release is called when FUSE is completely done with a file; at that point, you can free up any temporarily allocated data structures. The IBM document claims that there is exactly one release per open, but I don't know if that is true.
   */
   FileSystemResponse release(1:string path,  2:FuseHandleInfo handleInfo, 3:FuseContext context);

   
   /*
   * fsync(const char* path, int isdatasync, struct fuse_file_info* fi)
   * Flush any dirty information about the file to disk. If isdatasync is nonzero, only data, not metadata, needs to be flushed. 
   * When this call returns, all file data should be on stable storage. Many filesystems leave this call unimplemented, 
   * although technically that's a Bad Thing since it risks losing data. If you store your filesystem inside a plain file on another filesystem, 
   * you can implement this by calling fsync(2) on that file, which will flush too much data (slowing performance) but achieve the desired guarantee.
   */
   FileSystemResponse fsync(1:string path, 2:i64 isdatasync, 3: FuseHandleInfo handleInfo, 4:FuseContext context);



   /*
   * setxattr(const char* path, const char* name, const char* value, size_t size, int flags)
   * Set an extended attribute. See setxattr(2). This should be implemented only if HAVE_SETXATTR is true.
   */
   FileSystemResponse setxattr(1:string path,2:string name, 3:string val, 4:i16 valsize, 5:i32 flags, 6:FuseContext context);
   
   /*
   * getxattr(const char* path, const char* name, char* value, size_t size)
   * Read an extended attribute. See getxattr(2). This should be implemented only if HAVE_SETXATTR is true.
   */
   
   FileSystemResponse getxattr(1:string path, 2:string name, 3:FuseContext context);
   
   /*
   * listxattr(const char* path, const char* list, size_t size)
   * List the names of all extended attributes. See listxattr(2). This should be implemented only if HAVE_SETXATTR is true.
   */
   
   FileSystemResponse listxattr(1:string path, 2:FuseContext context);
    
   /*
   * int(* fuse_operations::removexattr)(const char *, const char *)
   * Remove extended attributes
   */
   FileSystemResponse removexattr(1:string path,2:string attributeKey, 3:FuseContext context);
 
    /*
    * opendir(const char* path, struct fuse_file_info* fi)
    * Open a directory for reading.
    */
   FileSystemResponse opendir(1:string path, 2:FuseContext context);
 
   /*
   * readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
   * Return one or more directory entries (struct dirent) to the caller. This is one of the most complex FUSE functions. 
   * It is related to, but not identical to, the readdir(2) and getdents(2) system calls, and the readdir(3) library function. 
   * Because of its complexity, it is described separately below. Required for essentially any filesystem, 
   * since it's what makes ls and a whole bunch of other things work.
   */
   FileSystemResponse readdir(1:string path, 2:i64 offset,  3:FuseHandleInfo handleInfo, 4:FuseContext context);

   /*
    * releasedir(const char* path, struct fuse_file_info *fi)
    * This is like release, except for directories.
    */
   FileSystemResponse releasedir(1:string path,  2:FuseHandleInfo handleInfo, 3:FuseContext context);
   
   /*
   * fsyncdir(const char* path, int isdatasync, struct fuse_file_info* fi)
   * Like fsync, but for directories.
   */
   FileSystemResponse fsyncdir(1:string path, 2:i64 isdatasync,  3:FuseHandleInfo handleInfo, 4:FuseContext context);

    /*
    * void *(* 	init )(struct fuse_conn_info *conn, struct fuse_config *cfg)
    * Initialize the filesystem. This function can often be left unimplemented, but it can be a handy way to perform one-time setup such as allocating variable-sized data structures or initializing a new filesystem. The fuse_conn_info structure gives information about what features are supported by FUSE, and can be used to request certain capabilities (see below for more information). The return value of this function is available to all file operations in the private_data field of fuse_context. It is also passed as a parameter to the destroy() method.
    */
    FileSystemResponse init(1:FuseConnectionInfo connn, 2:FuseConfig config);
   
    /*
    * void destroy(void* private_data)
    * Called when the filesystem exits. The private_data comes from the return value of init.
    */
    void destroy(1:i16 fsPrivateId);
   
    /*
    * access(const char* path, mask)
    * This is the same as the access(2) system call. It returns -ENOENT if the path doesn't exist, -EACCESS 
    * if the requested permission isn't available, or 0 for success. Note that it can be called on files, directories, 
    * or any other object that appears in the filesystem. This call is not required but is highly recommended.
    */
    FileSystemResponse access(1:string path,2:FuseAccessMode accessMask, 3:FuseContext context)

   /*
   * int(* 	create )(const char *, mode_t, struct fuse_file_info *)
   * Create and open a file If the file does not exist, first create it with the specified mode, and then open it.
   * If this method is not implemented or under Linux kernel versions earlier than 2.6.15, 
   * the mknod() and open() methods will be called instead.
   */
    FileSystemResponse create(1:string path,2:i32 mode, 3:FuseContext context)

   /*
   * lock(const char* path, struct fuse_file_info* fi, int cmd, struct flock* locks)
   * Perform POSIX file locking operation The cmd argument will be either F_GETLK, F_SETLK or F_SETLKW.
   * For the meaning of fields in 'struct flock' see the man page for fcntl(2). The l_whence field will always be set to SEEK_SET.
   * For checking lock ownership, the 'fuse_file_info->owner' argument must be used.
   * For F_GETLK operation, the library will first check currently held locks, and 
   * if a conflicting lock is found it will return information without calling this method. 
   * This ensures, that for local locks the l_pid field is correctly filled in. 
   * The results may not be accurate in of race conditions and in the presence of hard links, 
   * but it's unlikely that an application would rely on accurate GETLK results in these cases. 
   * If a conflicting lock is not found, this method will be called, and the filesystem may filsl out l_pid by a meaningful value,
   * or it may leave this field zero.
   * For F_SETLK and F_SETLKW the l_pid field will be set to the pid of the process performing the locking operation.
   * Note: if this method is not implemented, the kernel will still allow file locking to work locally. 
   * Hence it is only interesting for network filesystems and similar.
   */

   FileSystemResponse lock(1:string path,  2:FuseHandleInfo handleInfo, 3:i32 cmd, 4:FuseFlock flock, 5:FuseContext context)
    
   /*
   *int(* 	utimens )(const char *, const struct timespec tv[2], struct fuse_file_info *fi)
   * Update the last access time of the given object from ts[0] and the last modification time from ts[1]. 
   * Both time specifications are given to nanosecond resolution, but your filesystem doesn't have to be that precise; 
   * see utimensat(2) for full details. Note that the time specifications are allowed to have certain special values;
   * however, I don't know if FUSE functions have to support them. 
   * This function isn't necessary but is nice to have in a fully functional filesystem.
   */
   FileSystemResponse utimens(1:string path, 2:optional FuseTimeSpec timeSpec, 3: FuseHandleInfo info, 4:FuseContext context)

   /*
   * bmap(const char* path, size_t blocksize, uint64_t* blockno) 
   * This function is similar to bmap(9). If the filesystem is backed by a block device, 
   * it converts blockno from a file-relative block number to a device-relative block. 
   * It isn't entirely clear how the blocksize parameter is intended to be used.
   */
   FileSystemResponse bmap(1:string path, 2:i64 blocksize, 3:i64 blockIndex, 4:FuseContext context);

   /*
    * int(* 	mknod )(const char *, mode_t, dev_t)
    * Create a file node 
    * This is called for creation of all non-directory, non-symlink nodes. If the filesystem defines a create() method, 
    * then for regular files that will be called instead.
    */
   FileSystemResponse mknod(1:string path, 2:i32 mode, 3:i64 deviceId, 4:FuseContext context);

   /*
   * ===== TODO: To Keep it miniminalistinc filter out these if  no needed
   *
   * int (*write_buf) (const char *, struct fuse_bufvec *buf, off_t off,
   *                  struct fuse_file_info *);
   * int (*read_buf) (const char *, struct fuse_bufvec **bufp,
   *                 size_t size, off_t off, struct fuse_file_info *);
   * int (*flock) (const char *, struct fuse_file_info *, int op);
   *
   * int (*fallocate) (const char *, int, off_t, off_t,
   *                  struct fuse_file_info *);
   *
   * ssize_t (*copy_file_range) (const char *path_in,
   *                            struct fuse_file_info *fi_in,
   *                            off_t offset_in, const char *path_out,
   *                            struct fuse_file_info *fi_out,
   *                            off_t offset_out, size_t size, int flags);
   * off_t (*lseek) (const char *, off_t off, int whence, struct fuse_file_info *);
   *
   * ===== Not Planning to Support/Optinal Support.
   *
   * ioctl(const char* path, int cmd, void* arg, struct fuse_file_info* fi, unsigned int flags, void* data
   * Support the ioctl(2) system call. As such, almost everything is up to the filesystem. On a 64-bit machine, FUSE_IOCTL_COMPAT will be set for 32-bit ioctls. The size and direction of data is determined by _IOC_*() decoding of cmd. For _IOC_NONE, data will be NULL; for _IOC_WRITE data is being written by the user; for _IOC_READ it is being read, and if both are set the data is bidirectional. In all non-NULL cases, the area is _IOC_SIZE(cmd) bytes in size.
   * poll(const char* path, struct fuse_file_info* fi, struct fuse_pollhandle* ph, unsigned* reventsp);
   * Poll for I/O readiness. If ph is non-NULL, when the filesystem is ready for I/O it should call fuse_notify_poll (possibly asynchronously) with the specified ph; this will clear all pending polls. The callee is responsible for destroying ph with fuse_pollhandle_destroy() when ph is no longer needed.
   */
}