#include "thrift_fuse.h"
#include "fuse_native.h"
#include "logger.h"

using namespace Fuse;

thrift_fuse::thrift_fuse(std::shared_ptr<thrift_client> client) {
    _client = client;
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
