#include <FuseService.h>

#include "Logger.h"
#include "fuse_native.h"
#include "thrift_client.h"
#include "thrift_fuse.h"

using namespace Fuse;

#define THRIFT_OP(c, func, ...)                                                          \
    try {                                                                                \
        if ((c) == nullptr) {                                                            \
                                                                                         \
        } else {                                                                         \
            (c->GetStub()->func(__VA_ARGS__));                                           \
        }                                                                                \
    } catch (std::exception & ex) {                                                      \
        resp.status = StatusCode::FUSE_ERRECANCELED;                                     \
        LOG_ERROR << __FUNCTION__ << " Operation failed due to exception " << ex.what(); \
        thrift_client::HandleException(ex);                                              \
    }

int fuse_native::getattr(const char* path, fuse_stat* stbuf, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__ << " Path " << path;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), getattr, resp, path, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fFileStat(resp.stats, stbuf);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::readlink(const char* path, char* buf, size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), readlink, resp, path, size, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        strncpy(buf, resp.linkPath.c_str(), size);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(
        thrift_fuse::get_tfuse_from_context()->get_tclient(), mknod, resp, path, mode, dev, context);

    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::mkdir(const char* path, fuse_mode_t mode)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), mkdir, resp, path, mode, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::unlink(const char* path)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), unlink, resp, path, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::rmdir(const char* path)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), rmdir, resp, path, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::symlink(const char* dstpath, const char* srcpath)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), symlink, resp, dstpath, srcpath, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << srcpath << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::rename(const char* oldpath, const char* newpath, unsigned int flags)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), rename, resp, newpath, oldpath, flags, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << oldpath << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::link(const char* srcpath, const char* dstpath)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), link, resp, srcpath, dstpath, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << srcpath << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::chown(const char* path,
    fuse_uid_t uid,
    fuse_gid_t gid,
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), chown, resp, path, uid, gid, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::chmod(const char* path, fuse_mode_t mode, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(
        thrift_fuse::get_tfuse_from_context()->get_tclient(), chmod, resp, path, mode, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::truncate(const char* path, fuse_off_t size, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), truncate, resp, path, size, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::open(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), open, resp, path, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::read(const char* path,
    char* buf,
    size_t size,
    fuse_off_t off,
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), read, resp, path, size, off, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        strncpy(buf, resp.data.c_str(), resp.data.size());
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::write(const char* path,
    const char* buf,
    size_t size,
    fuse_off_t off,
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), write, resp, path, off, buf, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        return static_cast<int>(resp.dataWritten);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::statfs(const char* path, fuse_statvfs* stbuf)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), statfs, resp, path, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fStatFS(resp.statfs, stbuf);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::flush(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), flush, resp, path, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::release(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), release, resp, path, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::create(const char* path, fuse_mode_t mode, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return 0;
}

int fuse_native::lock(const char* path, fuse_file_info* fi, int cmd, fuse_flock* flock)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return 0;
}

int fuse_native::bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return 0;
}

int fuse_native::setxattr(const char* path,
    const char* name0,
    const char* value,
    size_t size,
    int flags)
{

    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);
    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), setxattr, resp, path, name0, value, size, flags, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::getxattr(const char* path,
    const char* name0,
    char* value,
    size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), getxattr, resp, path, name0, context);
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

int fuse_native::listxattr(const char* path, char* namebuf, size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    // TODO:: fix it.
    return StatusCode::FUSE_ERROREPERM;
}

int fuse_native::removexattr(const char* path, const char* name0)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return StatusCode::FUSE_ERROREPERM;
}

int fuse_native::opendir(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);
    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), opendir, resp, path, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::readdir(const char* path,
    void* buf,
    fuse_fill_dir_t filler,
    fuse_off_t off,
    fuse_file_info* fi,
    fuse_readdir_flags flag)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), readdir, resp, path, off, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        for (auto entry : resp.dirEntry) {
            fuse_stat statBuf;
            thrift_fuse::t2fFileStat(entry.stats, &statBuf);
            if (filler(buf, entry.name.c_str(), &statBuf, 0, FUSE_FILL_DIR_PLUS) != 0) {
                break;
            }
        }
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::releasedir(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), releasedir, resp, path, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::utimens(const char* path,
    const fuse_timespec tmsp[2],
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);
    FuseTimeSpec timeSpec;
    timeSpec.accessTime = tmsp[0].tv_nsec;
    timeSpec.modificationTime = tmsp[0].tv_nsec;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), utimens, resp, path, timeSpec, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::fsync(const char* path, int datasync, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);
    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), fsync, resp, path, datasync, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(handle, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::fsyncdir(const char* path, int datasync, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;
    FuseHandleInfo handle;
    thrift_fuse::f2tHandle(fi, handle);

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    THRIFT_OP(thrift_fuse::get_tfuse_from_context()->get_tclient(), fsyncdir, resp, path, datasync, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(handle, fi);
    } else {
        LOG_DEBUG << "Failed " << __FUNCTION__ << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::access(const char* path, int flag)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::f2tContext(fuse_get_context(), context);

    //THRIFT_OP(static_cast<FuseImpl*>(fuse_get_context()->private_data())->;, access, resp, path, static_cast<FuseAccessMode::type>(flag), context);
    return resp.status;
}

void fuse_native::destroy(void* fuse)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
}

void* fuse_native::init(fuse_conn_info* conn, fuse_config* conf)
{

    //ToDO:: Connect
    conn->want |= (conn->capable & FUSE_CAP_READDIRPLUS);
    return thrift_fuse::get_tfuse_from_context();
}
