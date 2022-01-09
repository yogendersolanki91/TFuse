

#include <FuseService.h>

#include "thriftClient.h"
#include <fuse3/fuse.h>
#include <memory>
#include <winfsp/winfsp.h>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

#define COPY_TO_THRIFT(t,f,field) \
    if (t->)

#pragma once
class FuseImpl {
public:
    FuseImpl();
    fuse_operations* Operations();

    bool IntializeConnection();
    bool PingFileSystemHost();
    int RunFileSystem(int argc, char* argv[]);

    // Fuse specific function
    static int getattr(const char* path,
        struct fuse_stat* stbuf,
        struct fuse_file_info* fi);
    static int readlink(const char* path, char* buf, size_t size);
    static int mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev);
    static int mkdir(const char* path, fuse_mode_t mode);
    static int unlink(const char* path);
    static int rmdir(const char* path);
    static int symlink(const char* dstpath, const char* srcpath);
    static int rename(const char* oldpath,
        const char* newpath,
        unsigned int flags);
    static int link(const char* srcpath, const char* dstpath);
    static int chown(const char* path,
        fuse_uid_t uid,
        fuse_gid_t gid,
        struct fuse_file_info* fi);
    static int chmod(const char* path,
        fuse_mode_t mode,
        struct fuse_file_info* fi);
    static int truncate(const char* path,
        fuse_off_t size,
        struct fuse_file_info* fi);
    static int open(const char* path, struct fuse_file_info* fi);
    static int read(const char* path,
        char* buf,
        size_t size,
        fuse_off_t off,
        struct fuse_file_info* fi);
    static int write(const char* path,
        const char* buf,
        size_t size,
        fuse_off_t off,
        struct fuse_file_info* fi);
    static int statfs(const char* path, struct fuse_statvfs* stbuf);
    static int flush(const char* path, struct fuse_file_info* fi);
    static int release(const char* path, struct fuse_file_info* fi);
    static int create(const char* path,
        fuse_mode_t mode,
        struct fuse_file_info* fi);
    static int lock(const char* path,
        struct fuse_file_info* fi,
        int cmd,
        fuse_flock* flock);
    static int bmap(const char* path, size_t blocksize, uint64_t* idx);

    static int setxattr(const char* path,
        const char* name0,
        const char* value,
        size_t size,
        int flags);
    static int getxattr(const char* path,
        const char* name0,
        char* value,
        size_t size);
    static int listxattr(const char* path, char* namebuf, size_t size);
    static int removexattr(const char* path, const char* name0);

    static int opendir(const char* path, struct fuse_file_info* fi);
    static int readdir(const char* path,
        void* buf,
        fuse_fill_dir_t filler,
        fuse_off_t off,
        struct fuse_file_info* fi,
        enum fuse_readdir_flags);
    static int releasedir(const char* path, struct fuse_file_info* fi);
    static int utimens(const char* path,
        const struct fuse_timespec tmsp[2],
        struct fuse_file_info* fi);
    static int fsync(const char* path, int datasync, struct fuse_file_info* fi);
    static int fsyncdir(const char* path,
        int datasync,
        struct fuse_file_info* fi);
    static void destroy(void* fuse);
    static int access(const char* path, int flag);
    static void* init(struct fuse_conn_info* conn, struct fuse_config* conf);
    static inline void SetClient(std::shared_ptr<ThriftClient> client)
    {
        _client = client;
    }
    static std::shared_ptr<FuseImpl> _fsInstance;

private:
    static inline void f2tHandle(fuse_file_info* fi,
        Fuse::FuseHandleInfo& handleInfo)
    {
        if (fi == nullptr) {
            handleInfo.__set_fh(-1);
            return;
        }
        handleInfo.__set_direct_io(fi->direct_io);
        handleInfo.__set_keep_cache (fi->keep_cache);
        handleInfo.__set_fh(fi->fh);
        handleInfo.__set_flags( fi->flags);
        handleInfo.__set_flush (fi->flush);
        handleInfo.__set_lock_owner (fi->lock_owner);
        handleInfo.__set_nonSeekable(fi->nonseekable);
        handleInfo.__set_poll_events(fi->poll_events);                
    }
    

    static inline void t2fHandle(Fuse::FuseHandleInfo handleInfo, fuse_file_info* fi)
    {
        if (handleInfo.__isset.direct_io)
            fi->direct_io = handleInfo.direct_io;
        if (handleInfo.__isset.keep_cache)
            fi->keep_cache = handleInfo.keep_cache;
        if (handleInfo.__isset.fh)
            fi->fh = handleInfo.fh;
        if (handleInfo.__isset.flags)
            fi->flags = handleInfo.flags;
        if (handleInfo.__isset.flush)
            fi->flush = handleInfo.flush;
        if (handleInfo.__isset.lock_owner)
            fi->lock_owner = handleInfo.lock_owner;
        if (handleInfo.__isset.nonSeekable)
            fi->nonseekable = handleInfo.nonSeekable;
        if (handleInfo.__isset.poll_events)
            fi->poll_events = handleInfo.poll_events;        
    }

    static inline void t2fStatFS(Fuse::FuseStatFS statFs, fuse_statvfs* stBuf) { 
        if (statFs.__isset.bavail)
            stBuf->f_bavail = statFs.bavail;
        if (statFs.__isset.bfree)
            stBuf->f_bfree = statFs.bfree;
        if (statFs.__isset.blocks)
            stBuf->f_blocks= statFs.blocks;
        if (statFs.__isset.bSize)
            stBuf->f_bsize = statFs.bSize;
        if (statFs.__isset.bavail)
            stBuf->f_favail = statFs.favail;
        if (statFs.__isset.files)
            stBuf->f_files = statFs.files;
        if (statFs.__isset.flags)
            stBuf->f_flag = statFs.flags;
        if (statFs.__isset.free)
            stBuf->f_ffree = statFs.free;
        if (statFs.__isset.frSize)
            stBuf->f_frsize = statFs.frSize;
        if (statFs.__isset.fsid)
            stBuf->f_fsid = statFs.fsid;
        if (statFs.__isset.namemax)
            stBuf->f_namemax = statFs.namemax;            
    }

    static inline void t2fFileStat(Fuse::FuseStat& stats, fuse_stat* st)
    {
        if (stats.__isset.accessTime)
            st->st_atim = { stats.accessTime };
        if (stats.__isset.blksize)
            st->st_blksize= stats.blksize;
        if (stats.__isset.blocks)
            st->st_blocks = stats.blocks;
        if (stats.__isset.changeTime)
            st->st_ctim = { stats.changeTime };
        if (stats.__isset.dev)
            st->st_dev = stats.dev;
        if (stats.__isset.gid)
            st->st_gid = stats.gid;
        if (stats.__isset.ino)
            st->st_ino = stats.ino;
        if (stats.__isset.mode)
            st->st_mode = stats.mode;
        if (stats.__isset.modificationTime)
            st->st_mtim = { stats.modificationTime };
        if (stats.__isset.nlink)
            st->st_nlink = stats.nlink;
        if (stats.__isset.rdev)
            st->st_rdev = stats.rdev;
        if (stats.__isset.size)
            st->st_size = stats.size;
        if (stats.__isset.uid)
            st->st_uid = stats.uid;        

    }
    static inline void f2tContext(fuse_context* fuse_context, Fuse::FuseContext& context)
    {
        context.__set_gid(fuse_context->gid);
        context.__set_uid(fuse_context->uid);
        context.__set_pid(fuse_context->pid);
        context.__set_umask(fuse_context->umask);
    }

    fuse_operations ops;
    static std::shared_ptr<ThriftClient> _client;
 };
