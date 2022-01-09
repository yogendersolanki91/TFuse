#include "fuseImpl.h"
#include "Logger.h"

using namespace Fuse;

std::shared_ptr<ThriftClient> FuseImpl::_client = NULL;
std::shared_ptr<FuseImpl> FuseImpl::_fsInstance = NULL;

FuseImpl::FuseImpl()
{
    ops = {
        getattr,
        readlink,
        mknod,
        mkdir,
        unlink,
        rmdir,
        symlink,
        rename,
        link,
        chmod,
        chown,
        truncate,
        open,
        read,
        write,
        statfs,
        flush,
        release,
        fsync, // fsync
        setxattr,
        getxattr,
        listxattr,
        removexattr,
        opendir,
        readdir,
        releasedir,
        fsyncdir, // fsyncdir
        init,
        destroy, // destroy
        access, // access
        create, // create
        lock, // lock
        utimens,
        bmap, // bmap
#if 0
			ioctl,
#endif
    };
}

fuse_operations*
FuseImpl::Operations()
{
    return &ops;
}

bool FuseImpl::IntializeConnection()
{
    return false;
}

bool FuseImpl::PingFileSystemHost()
{
    return false;
}

int FuseImpl::RunFileSystem(int argc, char* argv[])
{   
    return fuse_main(argc, argv, Operations(), this);
}

int FuseImpl::getattr(const char* path, fuse_stat* stbuf, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__ << " Path "<<path;
    FileSystemResponse resp;
    FuseHandleInfo handle;    
    f2tHandle(fi, handle);
    
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, getattr(resp, path, handle, context));
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        t2fFileStat(resp.stats, stbuf);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::readlink(const char* path, char* buf, size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, readlink(resp, path, size, context));
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        strncpy(buf, resp.linkPath.c_str(), size);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(
        _client, mknod(resp, path, mode, dev, context));

    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) { 
         LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::mkdir(const char* path, fuse_mode_t mode)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, mkdir(resp, path, mode, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::unlink(const char* path)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, unlink(resp, path, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::rmdir(const char* path)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, rmdir(resp, path, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::symlink(const char* dstpath, const char* srcpath)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, symlink(resp, dstpath, srcpath, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << srcpath << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::rename(const char* oldpath, const char* newpath, unsigned int flags)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, rename(resp, newpath, oldpath, flags, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << oldpath << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::link(const char* srcpath, const char* dstpath)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, link(resp, srcpath, dstpath, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << srcpath << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::chown(const char* path,
    fuse_uid_t uid,
    fuse_gid_t gid,
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, chown(resp, path, uid, gid, handle, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::chmod(const char* path, fuse_mode_t mode, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(
        _client, chmod(resp, path, mode, handle, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::truncate(const char* path, fuse_off_t size, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, truncate(resp, path, size, handle, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::open(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, open(resp, path, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::read(const char* path,
    char* buf,
    size_t size,
    fuse_off_t off,
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, read(resp, path, size, off, handle, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        strncpy(buf, resp.data.c_str(), resp.data.size());
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::write(const char* path,
    const char* buf,
    size_t size,
    fuse_off_t off,
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, write(resp, path, off, buf, handle, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        return static_cast<int>(resp.dataWritten);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::statfs(const char* path, fuse_statvfs* stbuf)
{
    LOG_DEBUG << "Called " << __FUNCTION__;    
    FileSystemResponse resp;
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, statfs(resp, path, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        t2fStatFS(resp.statfs, stbuf);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::flush(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, flush(resp, path, handle, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::release(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, release(resp, path, handle, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::create(const char* path, fuse_mode_t mode, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return 0;
}

int FuseImpl::lock(const char* path, fuse_file_info* fi, int cmd, fuse_flock* flock)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return 0;
}

int FuseImpl::bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return 0;
}

int FuseImpl::setxattr(const char* path,
    const char* name0,
    const char* value,
    size_t size,
    int flags)
{

    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, setxattr(resp, path, name0, value, size, flags, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::getxattr(const char* path,
    const char* name0,
    char* value,
    size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, getxattr(resp, path, name0, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        strncpy(value, resp.atrributeValue.c_str(), size);
        if (resp.atrributeValue.size() > size) {
            return StatusCode::FUSE_ERRORENOMEM;
        }
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::listxattr(const char* path, char* namebuf, size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    // TODO:: fix it.
    return StatusCode::FUSE_ERROREPERM;
}

int FuseImpl::removexattr(const char* path, const char* name0)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return StatusCode::FUSE_ERROREPERM;
}

int FuseImpl::opendir(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    f2tContext(fuse_get_context(), context); 
    THRIFT_OP(_client, opendir(resp, path, context));
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::readdir(const char* path,
    void* buf,
    fuse_fill_dir_t filler,
    fuse_off_t off,
    fuse_file_info* fi,
    fuse_readdir_flags flag)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, readdir(resp, path, off, handle, context));
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        for (auto entry : resp.dirEntry) {
            fuse_stat statBuf;
            t2fFileStat(entry.stats, &statBuf);
            if (filler(buf, entry.name.c_str(), &statBuf, 0, FUSE_FILL_DIR_PLUS) != 0) {
                break;
            }
        }
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::releasedir(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, releasedir(resp, path, handle,context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::utimens(const char* path,
    const fuse_timespec tmsp[2],
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);
    FuseTimeSpec timeSpec;
    timeSpec.accessTime = tmsp[0].tv_nsec;
    timeSpec.modificationTime = tmsp[0].tv_nsec;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, utimens(resp, path, timeSpec, handle, context));
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::fsync(const char* path, int datasync, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);
    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, fsync(resp, path, datasync, handle, context));
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        t2fHandle(handle, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::fsyncdir(const char* path, int datasync, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    f2tHandle(fi, handle);

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(_client, fsyncdir(resp, path, datasync, handle, context));
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        t2fHandle(handle, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int FuseImpl::access(const char* path, int flag)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    f2tContext(fuse_get_context(), context);
    
    THRIFT_OP(
        _client, access(resp, path, static_cast<FuseAccessMode::type>(flag), context));
    return resp.status;
}

void FuseImpl::destroy(void* fuse)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
}

void* FuseImpl::init(fuse_conn_info* conn, fuse_config* conf)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    conn->want |= (conn->capable & FUSE_CAP_READDIRPLUS);
    return _fsInstance.get();
}
