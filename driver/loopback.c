#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "util.h"
#include "net.h"

#define LOOPBACK_MTU UINT16_MAX /* Maximum size of IP datagram */

static int //ループバックデバイスの出力関数。引数は、送信するデータの種類、内容、長さ、宛先アドレス。
loopback_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst)
{
    debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
    debugdump(data, len);
    return net_input(type, data, len, dev); //ループバックデバイスは、送信されたパケットをそのまま受信する。引数のパケットをそのままプロトコルスタックに渡す。
}

static struct net_device_ops loopback_ops = {
    .output = loopback_output,
};

struct net_device *
loopback_init(void)
{
    struct net_device *dev;

    dev = net_device_alloc();
    if(!dev){
        errorf("net_device_alloc() failure");
        return NULL;
    }

    dev->type = NET_DEVICE_TYPE_LOOPBACK;
    dev->mtu = LOOPBACK_MTU;
    dev->flags = NET_DEVICE_FLAG_LOOPBACK;
    dev->hlen = 0; //noheader
    dev->alen = 0; //noaddress
    dev->ops = &loopback_ops; //&はアドレス演算子。loopback_opsのアドレスをdev->opsに代入する。

    if(net_device_register(dev) == -1){ //登録処理をして、失敗したら
        errorf("net_device_register() failure");
        return NULL;
    }

    infof("success, dev=%s", dev->name);
    return dev;
}
    