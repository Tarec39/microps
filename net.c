#include "platform.h"

#include "util.h"

int
net_init(void)
{
    infof( "initialization..."); //p32-33。infofはutil.hに記載の筆者の自作関数。。普通の動作ログのため。
    if (platform_init() == -1) {
        errorf( "platform_init() failure");
        return -1;
    }
    return 0;
}

int
net_run(void)
{
    infof( "startup...");
    if (platform_run() == -1) {
        errorf( "platform_run() failure");
        return -1;
    }
    return 0;
}

int
net_shutdown(void)
{
    infof(" shutting down...");
    if (platform_shutdown() == -1) {
        warnf( "platform_shutdown() failure");
    }
    infof( "success");
    return 0;
}
