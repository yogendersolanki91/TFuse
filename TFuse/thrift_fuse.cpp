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

// Include thirft_fuse first to avoid refdefination error
#include <thrift_fuse.h>

#include <fuse_native.h>
#include <logger.h>


using namespace Fuse;

thrift_fuse::thrift_fuse(blocking_queue<ThriftClientPtr>* clients)
{
    _clientQueue = clients;
    ops = {
        fuse_native::getattr,
        fuse_native::readlink,
        fuse_native::mknod,
        fuse_native::mkdir,
        fuse_native::unlink,
        fuse_native::rmdir,
        fuse_native::symlink,
        fuse_native::rename,
        fuse_native::link,
        fuse_native::chmod,
        fuse_native::chown,
        fuse_native::truncate,
        fuse_native::open,
        fuse_native::read,
        fuse_native::write,
        fuse_native::statfs,
        fuse_native::flush,
        fuse_native::release,
        fuse_native::fsync, // fsync
        fuse_native::setxattr,
        fuse_native::getxattr,
        fuse_native::listxattr,
        fuse_native::removexattr,
        fuse_native::opendir,
        fuse_native::readdir,
        fuse_native::releasedir,
        fuse_native::fsyncdir, // fsyncdir
        fuse_native::init,
        fuse_native::destroy, // destroy
        fuse_native::access, // access
        fuse_native::create, // create
        fuse_native::lock, // lock
        fuse_native::utimens,
        fuse_native::bmap, // bmap
#if 0
			fuse_native::ioctl,
#endif
    };
}

fuse_operations*
thrift_fuse::get_operations()
{
    return &ops;
}

bool thrift_fuse::ping_host()
{
    return false;
}

int thrift_fuse::thrift_fuse_main(int argc, char* argv[])
{
    return fuse_main(argc, argv, get_operations(), this);
}
