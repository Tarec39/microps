#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdint.h>

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#define NET_DEVICE_TYPE_DUMMY     0x0000
#define NET_DEVICE_TYPE_LOOPBACK  0x0001
#define NET_DEVICE_TYPE_ETHERNET  0x0002

#define NET_DEVICE_FLAG_UP        0x0001 //0xは16進数を表す。000 00000 0000 0000 
#define NET_DEVICE_FLAG_LOOPBACK  0x0010 //0000が実際の数字。0001であれば、最後列4ビットが分かり、1000であれば最前列4ビットが変わる
#define NET_DEVICE_FLAG_BROADCAST 0x0020 //1=0001 2=0010 4=0100 8=1000
#define NET_DEVICE_FLAG_P2P       0x0040
#define NET_DEVICE_FLAG_NEED_ARP  0x0100

#define NET_DEVICE_ADDR_LEN 16

#define NET_DEVICE_IS_UP(x) ((x)->flags & NET_DEVICE_FLAG_UP) //任意の（後者の）ビットが立っていれば0x0001=Trueが返る。でなければ0でFalseが返る。
#define NET_DEVICE_STATE(x) (NET_DEVICE_IS_UP(x) ? "UP" : "DOWN")

struct net_device {
    struct net_device *next; //C言語ではこれで連結リスト（ノード）を作成する
    unsigned int index;
    char name[IFNAMSIZ];
    uint16_t type; //unit16_t 符号なし(unsigned )16bit整数型のこと 0000 0000 0000 0000
    uint16_t mtu; //Max Transmission Unit
    uint16_t flags; //デバイスが有効なのかオプションも使っているのかとか
    uint16_t hlen; //Ethernetの長さ
    uint16_t alen; //Macアドレスの長さ
    uint8_t addr[NET_DEVICE_ADDR_LEN]; //Macアドレスそのもの
    uint8_t broadcast[NET_DEVICE_ADDR_LEN]; //ブロードキャストアドレス。イーサネットの場合はFF:FF:FF:FF:FF:FF
};

struct net_device_ops {
    int (*open)(struct net_device *dev);
    int (*close)(struct net_device *dev);
    int (*output)(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst);
};

extern struct net_device *
net_device_alloc(void);
extern int
net_device_register(struct net_device *dev);
extern int
net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst);

extern int
net_input(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev);

extern int
net_init(void);
extern int
net_run(void);
extern int
net_shutdown(void);

#endif
