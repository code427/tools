#include <common.h>
#include <asm/arch-mx25/imx-regs.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/iomux.h>
#include <asm/arch-mx25/mx25.h>
#include <malloc.h>
#include <asm/io.h>
#include <irq.h>
#include <asm/spi.h>


#undef SPI_DEBUG
//#define SPI_DEBUG

#ifdef SPI_DEBUG
#define SPI_DBG(x...) printf(x)
#else
#define SPI_DBG(x...)
#endif


#define CSPI1_NUM 	1
#define CSPI1_INT 	MXC_INT_CSPI1

#define CSPI3_NUM	2
#define CSPI3_INT	MXC_INT_CSPI3

#define SPI_PAD_CTL1 (PAD_CTL_HYS_SCHMITZ|PAD_CTL_PKE_ENABLE| \
              PAD_CTL_100K_PU)


#define MXC_SPI_BUF_RX(type)    \
void mxc_spi_buf_rx_##type(struct mxc_spi *master_drv_data, u32 val)\
{\
    type *rx = master_drv_data->transfer.rx_buf;\
    *rx++ = (type)val;\
    master_drv_data->transfer.rx_buf = rx;\
}

#define MXC_SPI_BUF_TX(type)    \
unsigned int mxc_spi_buf_tx_##type(struct mxc_spi *master_drv_data)\
{\
    unsigned int val;\
    const type *tx = master_drv_data->transfer.tx_buf;\
    val = *tx++;\
    master_drv_data->transfer.tx_buf = tx;\
    return val;\
}

typedef struct mxc_spi * sp_mxc_spi;
typedef struct spi_device * sp_spi_dev;

struct mxc_spi spi_master0;
struct spi_device spi0_dev;

struct mxc_spi spi_master2;
struct spi_device spi2_dev;


MXC_SPI_BUF_RX(u8)
MXC_SPI_BUF_TX(u8)
MXC_SPI_BUF_RX(u16)
MXC_SPI_BUF_TX(u16)
MXC_SPI_BUF_RX(u32)
MXC_SPI_BUF_TX(u32)

static struct mxc_spi_unique_def spi_ver_0_7 = {
	.intr_bit_shift = 8,
	.cs_shift = 12,
	.bc_shift = 20,
	.bc_mask = 0xFFF,
	.drctrl_shift = 8,
	.xfer_complete = (1 << 7),
	.bc_overflow = 0,
	.fifo_size = 8,
	.ctrl_reg_addr = 0,
	.stat_reg_addr = 0x14,
	.period_reg_addr = 0x18,
	.test_reg_addr = 0x1C,
	.reset_reg_addr = 0x0,
	.mode_mask = 0x1,
	.spi_enable = 0x1,
	.xch = (1 << 2),
	.mode_shift = 1,
	.master_enable = 1 << 1,
	.tx_inten_dif = 0,
	.rx_inten_dif = 0,
	.int_status_dif = 0,
	.low_pol_shift = 4,
	.pha_shift = 5,
	.ss_ctrl_shift = 6,
	.ss_pol_shift = 7,
	.max_data_rate = 0x7,
	.data_mask = 0x7,
	.data_shift = 16,
	.lbc = (1 << 14),
	.rx_cnt_off = 4,
	.rx_cnt_mask = (0xF << 4),
	.reset_start = 1,
};


/*!
 * This function enables CSPI interrupt(s)
 *
 * @param        master_data the pointer to mxc_spi structure
 * @param        irqs        the irq(s) to set (can be a combination)
 *
 * @return       This function returns 0 if successful, -1 otherwise.
 */
static int spi_enable_interrupt(struct mxc_spi *master_data, unsigned int irqs)
{
	if (irqs & ~((1 << master_data->spi_ver_def->intr_bit_shift) - 1)) {
		return -1;
	}

	__raw_writel((irqs | __raw_readl(MXC_CSPIINT + master_data->ctrl_addr)),
		     MXC_CSPIINT + master_data->ctrl_addr);
	return 0;
}

/*!
 * This function disables CSPI interrupt(s)
 *
 * @param        master_data the pointer to mxc_spi structure
 * @param        irqs        the irq(s) to reset (can be a combination)
 *
 * @return       This function returns 0 if successful, -1 otherwise.
 */
static int spi_disable_interrupt(struct mxc_spi *master_data, unsigned int irqs)
{
	if (irqs & ~((1 << master_data->spi_ver_def->intr_bit_shift) - 1)) {
		return -1;
	}

	__raw_writel((~irqs &
		      __raw_readl(MXC_CSPIINT + master_data->ctrl_addr)),
		     MXC_CSPIINT + master_data->ctrl_addr);
	return 0;
}


/*index: 1 ~ 3  */
static void cspi_clk_enable(int index)
{
	unsigned int ccm_cgr2;	
	ccm_cgr2 = __REG(CCM_CGR1);
	ccm_cgr2 |=(0x1<<5);
	__REG(CCM_CGR1)=ccm_cgr2;
}

static void cspi_clk_disable(int index)
{
	unsigned int ccm_cgr2;	
	ccm_cgr2 = __REG(CCM_CGR1);
	ccm_cgr2 &=	(~(0x1<<5));	  //ipg clk
	__REG(CCM_CGR1)=ccm_cgr2;
}


/*!
 * This function sets the baud rate for the SPI module.
 *
 * @param        master_data the pointer to mxc_spi structure
 * @param        baud        the baud rate
 *
 * @return       This function returns the baud rate divisor.
 */
static unsigned int spi_find_baudrate(struct mxc_spi *master_data,
				      unsigned int baud)
{
	unsigned int divisor;
	unsigned int shift = 0;

	/* Calculate required divisor (rounded) */
	divisor = (mx25_get_ipg_clk() + baud / 2) / baud;
	while (divisor >>= 1)
		shift++;

 
		shift -= 2;

	if (shift > spi_ver_0_7.max_data_rate)
		shift =  spi_ver_0_7.max_data_rate;

	return (shift <<  spi_ver_0_7.data_shift);
}


/*!
 * This function loads the transmit fifo.
 *
 * @param  base             the CSPI base address
 * @param  count            number of words to put in the TxFIFO
 * @param  master_drv_data  spi master structure
 */
static void spi_put_tx_data(void *base, unsigned int count,
			    struct mxc_spi *master_drv_data)
{
	unsigned int ctrl_reg;
	unsigned int data;
	int i = 0;

	/* Perform Tx transaction */
	for (i = 0; i < count; i++) {
		data = master_drv_data->transfer.tx_get(master_drv_data);
		__raw_writel(data, base + MXC_CSPITXDATA);
	}
	
	ctrl_reg = __raw_readl(base + MXC_CSPICTRL);
	ctrl_reg |= master_drv_data->spi_ver_def->xch;
	__raw_writel(ctrl_reg, base + MXC_CSPICTRL);

	return;
}



/*!
 * This function configures the hardware CSPI for the current SPI device.
 * It sets the word size, transfer mode, data rate for this device.
 *
 * @param       spi     	the current SPI device
 * @param	is_active 	indicates whether to active/deactivate the current device
 */
void mxc_spi_chipselect(struct spi_device *spi, int is_active)
{
	struct mxc_spi *master_drv_data;
	struct mxc_spi_xfer *ptransfer;
	struct mxc_spi_unique_def *spi_ver_def;
	unsigned int ctrl_reg = 0;
	unsigned int xfer_len;

	if (is_active == BITBANG_CS_INACTIVE) {
		/*Need to deselect the slave */
		return;
	}

	/* Get the master controller driver data from spi device's master */

	master_drv_data = spi->master;
	cspi_clk_enable(master_drv_data->bus_num);
	
	spi_ver_def = master_drv_data->spi_ver_def;
	xfer_len = spi->bits_per_word;

	{
		/* Control Register Settings for transfer to this slave */
		ctrl_reg = master_drv_data->spi_ver_def->spi_enable;
		ctrl_reg |=
		    (((spi->chip_select & MXC_CSPICTRL_CSMASK) << spi_ver_def->cs_shift) | spi_ver_def->mode_mask <<
		     spi_ver_def->mode_shift);
		ctrl_reg |=
		    spi_find_baudrate(master_drv_data, spi->max_speed_hz);
		ctrl_reg |=
		    (((xfer_len -
		       1) & spi_ver_def->bc_mask) << spi_ver_def->bc_shift);
		if (spi->mode & SPI_CPHA)
			ctrl_reg |=
			    spi_ver_def->mode_mask << spi_ver_def->pha_shift;
		else
			ctrl_reg &=
			    (~(spi_ver_def->mode_mask << spi_ver_def->pha_shift));
		if ((spi->mode & SPI_CPOL))
			ctrl_reg |=
			    spi_ver_def->mode_mask << spi_ver_def->low_pol_shift;
		else
			{
			
			ctrl_reg &=
			    (~(spi_ver_def->mode_mask << spi_ver_def->low_pol_shift));
			}
		if (spi->mode & SPI_CS_HIGH)
			ctrl_reg |=
			    spi_ver_def->mode_mask << spi_ver_def->ss_pol_shift;
		if (spi_ver_def == &spi_ver_0_7)
			ctrl_reg |=
			    spi_ver_def->mode_mask << spi_ver_def->
			    ss_ctrl_shift;

		__raw_writel(ctrl_reg, master_drv_data->base + MXC_CSPICTRL);
	}

	/* Initialize the functions for transfer */
	ptransfer = &master_drv_data->transfer;
	if (xfer_len <= 8) {
		
		ptransfer->rx_get = mxc_spi_buf_rx_u8;
		ptransfer->tx_get = mxc_spi_buf_tx_u8;
	} else if (xfer_len <= 16) {
		
		ptransfer->rx_get = mxc_spi_buf_rx_u16;
		ptransfer->tx_get = mxc_spi_buf_tx_u16;
	} else {
		
		ptransfer->rx_get = mxc_spi_buf_rx_u32;
		ptransfer->tx_get = mxc_spi_buf_tx_u32;
	}

	cspi_clk_disable(master_drv_data->bus_num);
	return;
}



int mxc_spi_setup(struct spi_device *spi)
{
	if (spi->max_speed_hz < 0) {
		return -1;
	}

	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	SPI_DBG("%s: mode %d, %u bpw, %d hz\n", __FUNCTION__,
		 spi->mode, spi->bits_per_word, spi->max_speed_hz);

	return 0;
}



static int mxc_spi_setup_transfer(struct spi_device *spi, struct spi_transfer *t)
{
	return 0;
}

void spi0_gpio_init(void)
{
	mxc_request_iomux(MX25_PIN_CSPI1_MOSI, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_CSPI1_MISO, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_CSPI1_SS0, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_CSPI1_SCLK, MUX_CONFIG_FUNC);
    
	mxc_iomux_set_pad(MX25_PIN_CSPI1_MOSI, SPI_PAD_CTL1);
	mxc_iomux_set_pad(MX25_PIN_CSPI1_MISO, SPI_PAD_CTL1);
	mxc_iomux_set_pad(MX25_PIN_CSPI1_SS0, SPI_PAD_CTL1);
	mxc_iomux_set_pad(MX25_PIN_CSPI1_SCLK, SPI_PAD_CTL1);
}


void spi2_gpio_init(void)
{
    mxc_request_iomux(MX25_PIN_CS4, MUX_CONFIG_ALT6); /*MOSI*/
    mxc_request_iomux(MX25_PIN_CS5, MUX_CONFIG_ALT6); /*MISO*/
    mxc_request_iomux(MX25_PIN_ECB, MUX_CONFIG_ALT6); /*SCLK*/
    mxc_request_iomux(MX25_PIN_EB1, MUX_CONFIG_ALT6); /*SS1*/

    mxc_iomux_set_pad(MX25_PIN_CS4, SPI_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_CS5, SPI_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_ECB, SPI_PAD_CTL1);
    mxc_iomux_set_pad(MX25_PIN_EB1, SPI_PAD_CTL1);

    mxc_iomux_set_input(MUX_IN_CSPI3_IPP_IND_MISO, INPUT_CTL_PATH0);
    mxc_iomux_set_input(MUX_IN_CSPI3_IPP_IND_MOSI, INPUT_CTL_PATH0);
    mxc_iomux_set_input(MUX_IN_CSPI3_IPP_IND_SS1_B,INPUT_CTL_PATH0);
    mxc_iomux_set_input(MUX_IN_CSPI3_IPP_CSPI_CLK_IN,INPUT_CTL_PATH0);
}


/*!
 * This function is called when the data has to transfer from/to the
 * current SPI device in poll mode

 *
 * @param        spi        the current spi device
 * @param        t          the transfer request - read/write buffer pairs
 *
 * @return       Returns 0 on success.
 */
int mxc_spi_poll_transfer(struct spi_device *spi, struct spi_transfer *t)
{
	struct mxc_spi *master_drv_data = NULL;
	int count, i;
	volatile unsigned int status;
	unsigned int rx_tmp;
	unsigned int fifo_size;
	int chipselect_status;
	if(NULL == spi ) 
	{
		printf("Spi device not inited \r\n");
		return 0;
	}
	if(NULL == spi->master ) 
		{
		printf("Spi master not inited \r\n");
		return 0;
	}
	mxc_spi_chipselect(spi, BITBANG_CS_ACTIVE);

	/* Get the master controller driver data from spi device's master */
	master_drv_data = spi->master;

	chipselect_status = __raw_readl(MXC_CSPICONFIG +
					master_drv_data->ctrl_addr);
	chipselect_status >>= master_drv_data->spi_ver_def->ss_pol_shift &
	    master_drv_data->spi_ver_def->mode_mask;
	if (master_drv_data->chipselect_active)
		master_drv_data->chipselect_active(spi->master->bus_num,
						   chipselect_status,
						   (spi->chip_select &
						    MXC_CSPICTRL_CSMASK) + 1);

	cspi_clk_enable(master_drv_data->bus_num);

	/* Modify the Tx, Rx, Count */
	master_drv_data->transfer.tx_buf = t->tx_buf;
	master_drv_data->transfer.rx_buf = t->rx_buf;
	master_drv_data->transfer.count = t->len;
	fifo_size = master_drv_data->spi_ver_def->fifo_size;

	count = (t->len > fifo_size) ? fifo_size : t->len;
	//DEBUGF("FILE:%s LINE: %d Transfer data \r\n",__FILE__,__LINE__);
	spi_put_tx_data(master_drv_data->base, count, master_drv_data);

	while(__raw_readl(master_drv_data->base+MXC_CSPICTRL)&(master_drv_data->spi_ver_def->xch)) udelay(1);
	
	while ((((status = __raw_readl(master_drv_data->test_addr)) &
		 master_drv_data->spi_ver_def->rx_cnt_mask) >> master_drv_data->
		spi_ver_def->rx_cnt_off) != count) ;

	for (i = 0; i < count; i++) {
		rx_tmp = __raw_readl(master_drv_data->base + MXC_CSPIRXDATA);
		//DEBUGF("FILE:%s LINE: %d Transfer data RX=0x%02X\r\n",__FILE__,__LINE__,rx_tmp);
		master_drv_data->transfer.rx_get(master_drv_data, rx_tmp);
	}

	cspi_clk_disable(master_drv_data->bus_num);
	if (master_drv_data->chipselect_inactive)
		master_drv_data->chipselect_inactive(spi->master->bus_num,
						     chipselect_status,
						     (spi->chip_select &
						      MXC_CSPICTRL_CSMASK) + 1);
	return 0;
}


/*!
 * This function is called when the data has to transfer from/to the
 * current SPI device. It enables the Rx interrupt, initiates the transfer.
 * When Rx interrupt occurs, the completion flag is set. It then disables
 * the Rx interrupt.
 *
 * @param        spi        the current spi device
 * @param        t          the transfer request - read/write buffer pairs
 *
 * @return       Returns 0 on success -1 on failure.
 */
int mxc_spi_transfer(struct spi_device *spi, struct spi_transfer *t)
{
	struct mxc_spi *master_drv_data = NULL;
	int count;
	int chipselect_status;
	unsigned int fifo_size;

	/* Get the master controller driver data from spi device's master */
	master_drv_data = spi->master;

	chipselect_status = __raw_readl(MXC_CSPICONFIG +
					master_drv_data->ctrl_addr);
	chipselect_status >>= master_drv_data->spi_ver_def->ss_pol_shift &
	    master_drv_data->spi_ver_def->mode_mask;
	if (master_drv_data->chipselect_active)
		master_drv_data->chipselect_active(spi->master->bus_num,
						   chipselect_status,
						   (spi->chip_select &
						    MXC_CSPICTRL_CSMASK) + 1);

	cspi_clk_enable(master_drv_data->bus_num);
	/* Modify the Tx, Rx, Count */
	master_drv_data->transfer.tx_buf = t->tx_buf;
	master_drv_data->transfer.rx_buf = t->rx_buf;
	master_drv_data->transfer.count = t->len;
	fifo_size = master_drv_data->spi_ver_def->fifo_size;
	

	/* Enable the Rx Interrupts */
	spi_enable_interrupt(master_drv_data,
			     1 << (MXC_CSPIINT_RREN_SHIFT +
				   master_drv_data->spi_ver_def->rx_inten_dif));

	
	count = (t->len > fifo_size) ? fifo_size : t->len;

	/* Perform Tx transaction */
	master_drv_data->transfer.rx_count = count;
	spi_put_tx_data(master_drv_data->base, count, master_drv_data);

	/* Disable the Rx Interrupts */
	spi_disable_interrupt(master_drv_data,
			      1 << (MXC_CSPIINT_RREN_SHIFT +
				    master_drv_data->spi_ver_def->
				    rx_inten_dif));

	cspi_clk_disable(master_drv_data->bus_num);
	if (master_drv_data->chipselect_inactive)
		master_drv_data->chipselect_inactive(spi->master->bus_num,
						     chipselect_status,
						     (spi->chip_select &
						      MXC_CSPICTRL_CSMASK) + 1);
	return (t->len - master_drv_data->transfer.count);
}



int spi_setup(struct spi_device *spi)
{
	int		status;

	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	status = mxc_spi_setup(spi);

	SPI_DBG("setup mode %d, %s%s%s%s"
				"%u bits/w, %u Hz max --> %d\n",
			(int) (spi->mode & (SPI_CPOL | SPI_CPHA)),
			(spi->mode & SPI_CS_HIGH) ? "cs_high, " : "",
			(spi->mode & SPI_LSB_FIRST) ? "lsb, " : "",
			(spi->mode & SPI_3WIRE) ? "3wire, " : "",
			(spi->mode & SPI_LOOP) ? "loopback, " : "",
			spi->bits_per_word, spi->max_speed_hz,
			status);

	return status;
}

/* defines to get compatible with Linux driver */
#define IRQ_NONE	0x0
#define IRQ_HANDLED	0x01

/*!
 * This function is called when an interrupt occurs on the SPI modules.
 * It is the interrupt handler for the SPI modules.
 *
 * @param        irq        the irq number
 * @param        dev_id     the pointer on the device
 *
 * @return       The function returns IRQ_HANDLED when handled.
 */
static void mxc_spi_isr(unsigned int irq)
{
	
	struct mxc_spi *master_drv_data = NULL;
	unsigned int status;
	int fifo_size;
	unsigned int pass_counter;
	if(CSPI3_INT == irq)
		master_drv_data = &spi_master2;
	
	if(CSPI1_INT == irq)
		master_drv_data = &spi_master0;
	
	if(NULL == master_drv_data->transfer.rx_get)
	{
		 __raw_readl(master_drv_data->base + MXC_CSPIRXDATA);
		
	}
	fifo_size = master_drv_data->spi_ver_def->fifo_size;
	pass_counter = fifo_size;

	/* Read the interrupt status register to determine the source */
	status = __raw_readl(master_drv_data->stat_addr);
	do {
		unsigned int rx_tmp =
		    __raw_readl(master_drv_data->base + MXC_CSPIRXDATA);

		if (master_drv_data->transfer.rx_buf)
			master_drv_data->transfer.rx_get(master_drv_data,
							 rx_tmp);
		(master_drv_data->transfer.count)--;
		(master_drv_data->transfer.rx_count)--;
		if (pass_counter-- == 0) {
			break;
		}
		status = __raw_readl(master_drv_data->stat_addr);
	} while (status &
		 (1 <<
		  (MXC_CSPISTAT_RR +
		   master_drv_data->spi_ver_def->int_status_dif)));

	if (master_drv_data->transfer.rx_count)
		return;

	if (master_drv_data->transfer.count) {
		if (master_drv_data->transfer.tx_buf) {
			unsigned int count = (master_drv_data->transfer.count >
				     fifo_size) ? fifo_size :
			    master_drv_data->transfer.count;
			master_drv_data->transfer.rx_count = count;
			spi_put_tx_data(master_drv_data->base, count,
					master_drv_data);
		}
	} 

	return;
}


int poll_spi_write(struct spi_device *spi,void *buf, size_t len)
{
	struct spi_transfer t = {
		.tx_buf = (const void *)buf,
		.rx_buf = buf,
		.len = len,
		.cs_change = 0,
		.delay_usecs = 0,
	};

	if (!spi)
		return -1;

	mxc_spi_poll_transfer(spi, &t);
	return 0;
}

void imx25_spi_setup(struct spi_device *spi,unsigned int speed, 
					 unsigned char bits_per_word,int ssN)
{	
	if(NULL == spi) 
		return;
	spi->bits_per_word = bits_per_word;
	spi->max_speed_hz = speed;
	spi->chip_select = ssN;
	spi->mode = SPI_MODE_0;
	spi_setup(spi);
}

struct spi_device  *hfc_get_hc595_device(void)
{
	return &spi0_dev;
}

struct spi_device  *hfc_get_tft_device(void)
{
	return &spi0_dev;
}

struct spi_device  *hfc_get_pn512_device(void)
{
	return &spi2_dev;
}


unsigned int mx25_get_ipg_clk(void)
{
	return mxc_get_clock(MXC_IPG_CLK);
}  

int mxc_spi_probe(struct mxc_spi *spi_master, struct spi_device *spi_dev, 
				  int SPI_NUM, int IRQ_NUM, char *irq_name,int speed, 
				  unsigned int SPI_BASE_ADDR, void (*gpio_init)(void))
{
	int ret = -1;
	
	memset(spi_master,0,sizeof(struct mxc_spi));
    memset(spi_dev,0,sizeof(struct spi_device));	
	spi_master->bus_num = SPI_NUM;
	spi_master->num_chipselect = 4;
	spi_master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;

    spi_master->spi_ver_def = &spi_ver_0_7;
	spi_dev->max_speed_hz = speed;

    /* Set the master bitbang data */
	spi_master->chipselect = mxc_spi_chipselect;
	spi_master->txrx_bufs = mxc_spi_transfer;
	spi_master->setup = mxc_spi_setup;
	spi_master->setup_transfer = mxc_spi_setup_transfer;
	spi_master->irq = IRQ_NUM;
	
	/* Register for SPI Interrupt */
	//ret = irq_request(CSPI1_INT, mxc_spi_isr, "cspi1");
	ret = irq_request(IRQ_NUM, mxc_spi_isr, irq_name);
	if(ret !=0) 
	{
		printf("CSPI_INT irq request failed \n");	
		return ret;
	}
	
	gpio_init();

	/* Enable the CSPI Clock, CSPI Module, set as a master */
	spi_master->base = (void *)SPI_BASE_ADDR;
	spi_master->ctrl_addr =
	    spi_master->base + spi_master->spi_ver_def->ctrl_reg_addr;
	spi_master->stat_addr =
	    spi_master->base + spi_master->spi_ver_def->stat_reg_addr;
	spi_master->period_addr =
	    spi_master->base +
	    spi_master->spi_ver_def->period_reg_addr;
	spi_master->test_addr =
	    spi_master->base +spi_master->spi_ver_def->test_reg_addr;
	spi_master->reset_addr =
	    spi_master->base +
	    spi_master->spi_ver_def->reset_reg_addr;

	spi_master->spi_ipg_clk = mx25_get_ipg_clk(); //ipg clk

	cspi_clk_enable(spi_master->bus_num);
	
	__raw_writel(spi_master->spi_ver_def->reset_start,
		     spi_master->reset_addr);
	
	udelay(1);
	__raw_writel((spi_master->spi_ver_def->spi_enable +
		     spi_master->spi_ver_def->master_enable),
		     spi_master->base + MXC_CSPICTRL);
	__raw_writel(MXC_CSPIPERIOD_32KHZ, spi_master->period_addr);
	__raw_writel(0, MXC_CSPIINT + spi_master->ctrl_addr);

	spi_dev->master = spi_master;
	cspi_clk_disable(spi_master->bus_num);

	return ret;
}

int hfc_tft_spi_probe(void)
{
	mxc_spi_probe(&spi_master0,&spi0_dev,1,MXC_INT_CSPI1,"cspi1_irq",
				8000000,IMX_CSPI1_BASE,spi0_gpio_init); 
	return 0;
}

int hfc_pn512_probe(void)
{
	mxc_spi_probe(&spi_master2,&spi2_dev,3,MXC_INT_CSPI3,"cspi3_irq",
				2000000,IMX_CSPI3_BASE,spi2_gpio_init); 
	return 0;
}


