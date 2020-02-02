#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define IO_BASE 0x01c20800

#define GPIO_BANK(pin)       ((pin) >> 5)
#define GPIO_CFG_INDEX(pin)  (((pin) & 0x1F) >> 3)
#define GPIO_CFG_OFFSET(pin) ((((pin) & 0x1F) & 0x7) << 2)
#define GPIO_NUM(pin)        ((pin) & 0x1F)

#define GPIO_ON  1
#define GPIO_OFF 0
#define GPIO_OUTPUT 1
#define GPIO_INPUT  0

// pinout can be found here
// http://www.orangepi.org/orangepibbsen/data/attachment/forum/201601/24/204946qco5z1icsnigci37.png
#define GPIO_PORT_G    192
#define GPIO_LED_GREEN GPIO_PORT_G + 6
#define GPIO_LED_RED   GPIO_PORT_G + 7


struct gpio {
    uint32_t cfg[4];
    uint32_t dat;
    uint32_t drv[2];
    uint32_t pull[2];
};

/* gpio interrupt control */
struct gpio_int {
    uint32_t cfg[3];
    uint32_t ctl;
    uint32_t sta;
    uint32_t deb;
};

struct gpio_reg {
    struct  gpio gpio_bank[9];
    uint8_t res[0xbc];
    struct  gpio_int gpio_int;
};

int gpio_init(uintptr_t* base) {
    int32_t fd = open("/dev/mem", O_RDWR);
    if (fd < 0) {
        return -1;
    }

    uint32_t PageSize = sysconf(_SC_PAGESIZE);
    uint32_t PageMask = (~(PageSize - 1));

    uint32_t addr_start = IO_BASE & PageMask;
    uint32_t addr_offset = IO_BASE & ~PageMask;

    void *pc = (void *) mmap(0, PageSize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr_start);
    if (pc == MAP_FAILED) {
        return -2;
    }

    uintptr_t base_addr = (uintptr_t) pc;
    uintptr_t with_offset = base_addr + addr_offset;

    // store return address
    *base = with_offset;
    close(fd);
    return 0;
}

int gpio_config_pin(uintptr_t addr, uint32_t pin, uint8_t val) {
    uint32_t cfg;
    uint32_t bank   = GPIO_BANK(pin);
    uint32_t index  = GPIO_CFG_INDEX(pin);
    uint32_t offset = GPIO_CFG_OFFSET(pin);

    struct gpio *pio = &((struct gpio_reg *) addr)->gpio_bank[bank];

    cfg = *(&pio->cfg[0] + index);
    cfg &= ~(0xf << offset);
    cfg |= val << offset;

    *(&pio->cfg[0] + index) = cfg;

    return 0;
}

int gpio_set_output(uintptr_t addr, uint32_t pin, uint8_t val) {
    uint32_t bank = GPIO_BANK(pin);
    uint32_t num  = GPIO_NUM(pin);

    struct gpio *pio = &((struct gpio_reg *) addr)->gpio_bank[bank];

    if (val)
        *(&pio->dat) |= 1 << num;
    else
        *(&pio->dat) &= ~(1 << num);

    return 0;
}
