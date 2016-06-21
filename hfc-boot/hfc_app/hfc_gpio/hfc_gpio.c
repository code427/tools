#include <common.h>
#include <command.h>
#include <malloc.h>
#include <config.h>
#include <asm/arch-mx25/imx-regs.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/iomux.h>
#include <asm/arch-mx25/mx25.h>
#include <asm/spi.h>
#include <asm/gpio.h>


static unsigned int hc595_state;
static struct spi_device *hc595_spi;

void eth_phy_init(void)
{
    /* setup pins for FEC */
    mxc_request_iomux(MX25_PIN_FEC_TX_CLK, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_RX_DV, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_RDATA0, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_TDATA0, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_TX_EN, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_MDC, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_MDIO, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_RDATA1, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_FEC_TDATA1, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_A19, MUX_CONFIG_ALT7);
    mxc_request_iomux(MX25_PIN_UPLL_BYPCLK, MUX_CONFIG_ALT5);

#define FEC_PAD_CTL1 (PAD_CTL_HYS_SCHMITZ | PAD_CTL_PUE_PUD | \
            PAD_CTL_PKE_ENABLE)
#define FEC_PAD_CTL2 (PAD_CTL_PUE_PUD)
    mxc_iomux_set_pad(MX25_PIN_FEC_TX_CLK, FEC_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_FEC_RX_DV, FEC_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_FEC_RDATA0, FEC_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_FEC_TDATA0, FEC_PAD_CTL2);
    mxc_iomux_set_pad(MX25_PIN_FEC_TX_EN, FEC_PAD_CTL2);
    mxc_iomux_set_pad(MX25_PIN_FEC_MDC, FEC_PAD_CTL2);
    mxc_iomux_set_pad(MX25_PIN_FEC_MDIO, FEC_PAD_CTL1 | PAD_CTL_22K_PU);
    mxc_iomux_set_pad(MX25_PIN_FEC_RDATA1, FEC_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_FEC_TDATA1, FEC_PAD_CTL2);

    mxc_iomux_set_pad(MX25_PIN_A19, FEC_PAD_CTL1);
    mxc_iomux_set_input(MUX_IN_FEC_FEC_RX_ER, INPUT_CTL_PATH0);

    mxc_set_gpio_direction(MX25_PIN_UPLL_BYPCLK, 0);
    mxc_set_gpio_dataout(MX25_PIN_UPLL_BYPCLK, 0);
}



void hfc_hc595_data_latch_init(void)
{
	/* A10 AS GPIO OUTPUT FOR U1 DATAT-LATCH0 CLK */
	mxc_request_iomux(MX25_PIN_CSPI1_RDY, MUX_CONFIG_ALT5);
	/* 1 (or non-zero) for input; 0 for output*/
	mxc_set_gpio_direction(MX25_PIN_CSPI1_RDY, 0); 
	mxc_set_gpio_dataout(MX25_PIN_CSPI1_RDY, 0);
}

void hfc_hc595_data_latch_high(void)
{
	mxc_set_gpio_dataout(MX25_PIN_CSPI1_RDY, 1);
} 

void hfc_hc595_data_latch_low(void)
{
	mxc_set_gpio_dataout(MX25_PIN_CSPI1_RDY, 0);
}

void gpio_set_hc595_output(int num, int value)
{
	unsigned int frame[2] = {0};

	hc595_state = ((hc595_state & (~(1 << num))) | (value << num));
	
	hfc_hc595_data_latch_low();

    hc595_spi = hfc_get_hc595_device();

	if(2 != hc595_spi->chip_select)
	{
		imx25_spi_setup(hc595_spi, 1000000, 32, 2);
	}
	frame[0] = hc595_state;
	poll_spi_write(hc595_spi, frame, 1);
	
	hfc_hc595_data_latch_high();
	hfc_hc595_data_latch_low();
}

void hfc_battery_charge_gpio_init(void)
{
	mxc_request_iomux(MX25_PIN_RTCK, MUX_CONFIG_ALT5); 
	mxc_iomux_set_pad(MX25_PIN_RTCK, PAD_CTL_DRV_NORMAL);
	mxc_set_gpio_direction(MX25_PIN_RTCK, 1);
}

void hfc_libat_measure_control(int val)
{
	mxc_set_gpio_direction(MX25_PIN_RTCK,0);// output
	mxc_set_gpio_dataout(MX25_PIN_RTCK,val);
}

int hfc_get_libat_charge_status(void)
{
	mxc_set_gpio_direction(MX25_PIN_RTCK,1);// IN
	return mxc_get_gpio_datain(MX25_PIN_RTCK);
}

