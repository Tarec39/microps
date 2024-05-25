#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "platform.h"

#include "util.h"
#include "net.h"

/*
 * NOTE: if you want to add/delete the entries after net_run(),
 *       you need to protect these lists with a lock.
 */
static struct net_device *devices; 
//staticはこのファイル内のグローバル変数
//devicesというアドレスが確保され続ける感じ。ちなみに初期値はNULL

struct net_device * //これは関数の返り値の型
net_device_alloc(void)
{
    struct net_device *dev;

    dev = memory_alloc(sizeof(*dev));
    if(!dev){
        errorf("memory_alloc() failure");
        return NULL;
    }
    return dev;
}

/*
 * NOTE: must not be call after net_run()
 */
int
net_device_register(struct net_device *dev)
{
    static unsigned int index = 0; //unsignedは符号化されていない、つまりマイナスとかついてないってことらしい。

    dev->index = index++; //今の値を使ったあとに1を足す。ただし実は2通りあって、もし++indexだったら、先に1を足してから値を使う。今回のは後置。

    snprintf(dev->name, sizeof(dev->name), "net%d", dev->index); //(書き込み先, 書き込み先のサイズ, フォーマット文字列, 埋め込む値);

    dev->next = devices; //新しいノードは古いノードすべてを記録する...net1 ---> net0 ---> NULLと新しいのが先頭。追加するたびに先頭に追加される感じ。
    devices =dev; //ちなみにこれで連結リストの先頭に追加するらしい。

    infof("success, dev=%s, type=0x%04x", dev->name, dev->type); //%04xは16進数。
    return 0;
}

static int
net_device_open(struct net_device *dev)
{
    infof("dev=%s", dev->name);

    if (NET_DEVICE_IS_UP(dev)){ //既にUPならば
        errorf("already opened, dev=%s", dev->name);
        return -1;
    }

    dev->flags |= NET_DEVICE_FLAG_UP; //ビット演算子のOR。任意のビットだけを立てる。
    return 0;
}

static int
net_device_close(struct net_device *dev)
{
    infof("dev=%s", dev->name);

    if(!NET_DEVICE_IS_UP(dev)){ //UPでないならば、
        errorf("not opened, dev=%s", dev->name);
        return -1;
    }
    //両方とも 1 のビットだけを 1 のまま残し、それ以外は全部 0 にする
    dev->flags &= ~NET_DEVICE_FLAG_UP; //ビット演算子のAND。~はビット列の反転。任意のビットだけ立ったままに。
    return 0;
}

int
net_device_output( //ネットワークデバイスからデータを送信するための関数。引数はインターネット層のプロトコルパケット
    struct net_device *dev, //ネットワークデバイス
    uint16_t type, //送信データのプロトコル種別 IPv4とかARPとか
    const uint8_t *data, //送信データ
    size_t len, //送信データの長さ
    const void *dst  //送信先アドレス
)
{
    debugf("dev=%s, type=0x%04x, data=%zu", dev->name, type, len); //%zu sizte_t型 C言語では、長さ、大きさ、サイズを表現する型
    debugdump(data, len);

    if(!NET_DEVICE_IS_UP(dev)){ //UPでないならば
        errorf("not opened, dev=%s", dev->name);
        return -1;
    }

    if(dev->mtu < len) { //転送最大単位よりも送信データが大きいならば
        errorf("too long, dev=%s, mtu=%u, len=%zu", dev->name, dev->mtu, len); //%uは符号なし整数 unsigned int。
        return -1;
    }

    return 0;
}

int
net_input(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev)
{
}

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
    struct net_device *dev;

    infof( "startup...");
    if (platform_run() == -1) {
        errorf( "platform_run() failure");
        return -1;
    }

    for (dev=devices; dev; dev=dev->next){ //初期化; 条件; 更新 if(..; dev;...)はノードがあるならば、つまりif (dev != NULL)=ノードが空でないならば、と同義
        net_device_open(dev);
    }
    infof("success");
    return 0;
}

int
net_shutdown(void)
{
    struct net_device *dev;

    infof(" shutting down...");
    if (platform_shutdown() == -1) {
        warnf( "platform_shutdown() failure");
    }

    for(dev = devices; dev; dev=dev->next){
        net_device_close(dev);
    }
    
    infof( "success");
    return 0;
}
