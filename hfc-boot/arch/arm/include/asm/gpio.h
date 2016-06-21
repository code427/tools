#include <asm/arch/gpio.h>
#include <asm-generic/gpio.h>
#include <asm/arch-mx25/mx25_pins.h>

#define HC595_EXT_IO_AD_C2          3
#define HC595_EXT_IO_AD_C1          6
#define HC595_EXT_IO_AD_C3          7

void mxc_set_gpio_direction(iomux_pin_name_t pin, int is_input);
void mxc_set_gpio_dataout(iomux_pin_name_t pin, u32 data);
int mxc_get_gpio_datain(iomux_pin_name_t pin);

