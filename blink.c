#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>

#include  "io.c"

int main() {
    uintptr_t base;

    int errno = gpio_init(&base);
    if (errno < 0) {
        printf("failed to init gpio: errno = %d\n", errno);
        return -1;
    }

    gpio_config_pin(base, LED_RED, OUTPUT);
    gpio_config_pin(base, LED_GREEN, OUTPUT);

    struct timespec ts = {.tv_nsec = 50000000L};

    uint8_t val = OFF;
    for (int i = 0; i < 100; ++i)
    {
        printf("val = %u\n", val);
        gpio_set_output(base, LED_RED,   val ? ON : OFF);
        gpio_set_output(base, LED_GREEN, val ? OFF : ON);
        val = val ? OFF : ON;
        nanosleep(&ts, NULL);
    }

    return 0;
}
