/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2022 Yogender Solanki
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************
 */
#include <FuseService.h>

#include <Logger.h>
#include <fuse_native.h>
#include <thrift_client.h>
#include <thrift_fuse.h>

#include <chrono>

using namespace std::chrono;
using namespace Fuse;

struct scope_exit {
    scope_exit(std::function<void(void)> f)
        : f_(f)
    {
    }
    ~scope_exit(void) { f_(); }

private:
    std::function<void(void)> f_;
};
#define WARNING_CHANNEL_THRESHOLD 100

#define THRIFT_OP(func, ...)                                                               \
    try {                                                                                  \
        auto start = high_resolution_clock::now();                                         \
        auto client = thrift_fuse::get_tfuse_from_context()->get_tclient();                \
        auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start); \
        if (duration.count() > WARNING_CHANNEL_THRESHOLD) {                                \
            LOG_WARNING                                                                    \
                << "High latency to get client "                                           \
                << duration.count() << " ms";                                              \
        }                                                                                  \
        scope_exit relaseChannel([client](void) {                                          \
            thrift_fuse::get_tfuse_from_context()->release_tclient(client);                \
        });                                                                                \
        LOG_DEBUG << "Calling host " << " => [" << client->get_client_id() << "]"; \
        client->GetStub()->func(__VA_ARGS__);                                              \
    } catch (std::exception & ex) {                                                        \
        resp.status = StatusCode::FUSE_ERRECANCELED;                                       \
        LOG_ERROR << " Operation failed due to exception " << ex.what();   \
        thrift_client::HandleException(ex);                                                \
    }

int fuse_native::getattr(const char* path, fuse_stat* stbuf, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << " Path " << path;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(getattr, resp, std::string(path), handle, context);

    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS && resp.__isset.stats) {
        thrift_fuse::t2fFileStat(resp.stats, stbuf);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::readlink(const char* path, char* buf, size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(readlink, resp, path, size, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        strncpy(buf, resp.linkPath.c_str(), size);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(mknod, resp, path, mode, dev, context);

    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::mkdir(const char* path, fuse_mode_t mode)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(mkdir, resp, path, mode, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::unlink(const char* path)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(unlink, resp, path, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::rmdir(const char* path)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(rmdir, resp, path, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::symlink(const char* dstpath, const char* srcpath)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(symlink, resp, dstpath, srcpath, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << srcpath << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::rename(const char* oldpath, const char* newpath, unsigned int flags)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(rename, resp, oldpath, newpath, flags, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << oldpath << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::link(const char* srcpath, const char* dstpath)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(link, resp, srcpath, dstpath, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << srcpath << "Error " << resp.status;
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
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(chown, resp, path, uid, gid, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::chmod(const char* path, fuse_mode_t mode, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(chmod, resp, path, mode, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::truncate(const char* path, fuse_off_t size, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(truncate, resp, path, size, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::open(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(open, resp, path, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
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
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(read, resp, path, size, off, handle, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        if (resp.__isset.data) { 
            memcpy(buf, resp.data.c_str(), resp.data.size());
        } else {
            return 0;
        }
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
        return resp.status;
    }
    return resp.data.size();
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
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);
  //  LOG_INFO << "Write  " << path << " Offset " << off << " Size " << size;
    
    THRIFT_OP(write, resp, path, std::string(buf, size), off, size, handle, context);
    
    if (resp.status == StatusCode::FUSE_SUCCESS) {        
    //   LOG_INFO << "Written  " << path << " Offset " << off << " Size " << size;
       return static_cast<int>(resp.dataWritten);
    } else {
        LOG_ERROR << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.dataWritten;
}

int fuse_native::statfs(const char* path, fuse_statvfs* stbuf)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(statfs, resp, path, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fStatFS(resp.statfs, stbuf);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::flush(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(flush, resp, path, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::release(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(release, resp, path == nullptr ? "" : std::string(path), handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::create(const char* path, fuse_mode_t mode, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(create, resp, path, mode, context);

    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
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
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(setxattr, resp, path, name0, value, size, flags, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
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
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(getxattr, resp, path, name0, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        strncpy(value, resp.atrributeValue.c_str(), size);
        if (resp.atrributeValue.size() > size) {
            return StatusCode::FUSE_ERRORENOMEM;
        }
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}




int fuse_native::opendir(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(opendir, resp, path, context);
    if (resp.status == StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(resp.info, fi);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
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
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(readdir, resp, path, off, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        for (auto entry : resp.dirEntry) {
            fuse_stat statBuf;
            thrift_fuse::t2fFileStat(entry.stats, &statBuf);
            if (filler(buf, entry.name.c_str(), &statBuf, 0, FUSE_FILL_DIR_PLUS) != 0) {
                break;
            }
        }
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::releasedir(const char* path, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(releasedir, resp, path, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::utimens(const char* path,
    const fuse_timespec tmsp[2],
    fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseTimeSpec timeSpec;
    timeSpec.accessTime = tmsp[0].tv_sec;
    timeSpec.modificationTime = tmsp[0].tv_sec;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(utimens, resp, path, timeSpec, handle, context);
    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::fsync(const char* path, int datasync, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(fsync, resp, path, datasync, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(handle, fi);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::fsyncdir(const char* path, int datasync, fuse_file_info* fi)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseHandleInfo handle;
    thrift_fuse::fuse2thriftHandleInfo(fi, handle);

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(fsyncdir, resp, path, datasync, handle, context);
    if (resp.status == Fuse::StatusCode::FUSE_SUCCESS) {
        thrift_fuse::t2fHandle(handle, fi);
    } else {
        LOG_DEBUG << "Failed " << " Path " << path << "Error " << resp.status;
    }
    return resp.status;
}

int fuse_native::access(const char* path, int flag)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    FileSystemResponse resp;

    FuseContext context;
    thrift_fuse::fuse2thriftContext(fuse_get_context(), context);

    THRIFT_OP(access, resp, path, static_cast<FuseAccessMode::type>(flag), context);

    if (resp.status != Fuse::StatusCode::FUSE_SUCCESS) {        
        LOG_DEBUG << "Failed "
                  << " Path " << path << "Error " << resp.status;
    }
    return 0;
}

// Todo:: need to be implemented
int fuse_native::removexattr(const char* path, const char* name0)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    return StatusCode::FUSE_ERROREPERM;
}

int fuse_native::listxattr(const char* path, char* namebuf, size_t size)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    // TODO:: fix it.
    return StatusCode::FUSE_ERROREPERM;
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



void fuse_native::destroy(void* fuse)
{
    LOG_DEBUG << "Called " << __FUNCTION__;
    auto fs = static_cast<thrift_fuse*>(fuse);

    delete static_cast<thrift_fuse*>(fuse);
}

void* fuse_native::init(fuse_conn_info* conn, fuse_config* conf)
{        
    conn->want |= (conn->capable & FUSE_CAP_READDIRPLUS);
    return thrift_fuse::get_tfuse_from_context();
}
