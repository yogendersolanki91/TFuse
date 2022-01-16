/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2011 Yogender Solanki
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


#pragma once
#define _WINSOCKAPI_
#include <fuse3/fuse.h>

#include <FuseService.h>

#include <memory>

#include <blocking_queue.h>
#include <thrift_client.h>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

class thrift_fuse {
private: // private fields
    fuse_operations ops;
    blocking_queue<ThriftClientPtr>* _clientQueue;

public: // public field
private: // private function
public: // non static function
    thrift_fuse(blocking_queue<ThriftClientPtr>* clients);
    fuse_operations* get_operations();
    bool ping_host();
    int thrift_fuse_main(int argc, char* argv[]);

    inline ThriftClientPtr get_tclient()
    {
        ThriftClientPtr conn;
        _clientQueue->pop(conn);
        return conn;
    }

    inline void release_tclient(ThriftClientPtr client)
    {
        _clientQueue->push(client);
    }

public: // misc private function
    static inline thrift_fuse* get_tfuse_from_context()
    {
        return static_cast<thrift_fuse*>(fuse_get_context()->private_data);
    }
    static inline void fuse2thriftHandleInfo(fuse_file_info* fi,
        Fuse::FuseHandleInfo& handleInfo)
    {
        if (fi == nullptr) {
            handleInfo.__set_fh(-1);
            return;
        }
        handleInfo.__set_direct_io(fi->direct_io);
        handleInfo.__set_keep_cache(fi->keep_cache);
        handleInfo.__set_fh(fi->fh);
        handleInfo.__set_flags(fi->flags);
        handleInfo.__set_flush(fi->flush);
        handleInfo.__set_lock_owner(fi->lock_owner);
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

    static inline void t2fStatFS(Fuse::FuseStatFS statFs, fuse_statvfs* stBuf)
    {
        if (statFs.__isset.bavail)
            stBuf->f_bavail = statFs.bavail;
        if (statFs.__isset.bfree)
            stBuf->f_bfree = statFs.bfree;
        if (statFs.__isset.blocks)
            stBuf->f_blocks = statFs.blocks;
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
            st->st_blksize = stats.blksize;
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
    static inline void fuse2thriftContext(fuse_context* fuse_context, Fuse::FuseContext& context)
    {
        context.__set_gid(fuse_context->gid);
        context.__set_uid(fuse_context->uid);
        context.__set_pid(fuse_context->pid);
        context.__set_umask(fuse_context->umask);
    }
};
