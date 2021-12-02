#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"

#define LED 1

int aw_main (void)
{
    aw_kprintf("\r\nApplication Start. \r\n");
    
    while (1) {
        aw_led_toggle(LED);
        aw_mdelay(500);
    }

    return 0;
}
