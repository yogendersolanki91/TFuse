#pragma once
#include <fuse3/fuse.h>
#include <winfsp/winfsp.h>

class fuse_native {
public:
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
};
