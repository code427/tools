#ifndef __SPI_H__
#define __SPI_H__ 

#include <linux/types.h>

#define MXC_CSPIRXDATA		0x00
#define MXC_CSPITXDATA		0x04
#define MXC_CSPICTRL		0x08
#define MXC_CSPICONFIG		0x08
#define MXC_CSPIINT			0x0C

#define MXC_CSPICTRL_DISABLE	0x0
#define MXC_CSPICTRL_SLAVE	0x0
#define MXC_CSPICTRL_CSMASK	0x3
#define MXC_CSPICTRL_SMC	(1 << 3)

#define MXC_CSPIINT_TEEN_SHIFT		0
#define MXC_CSPIINT_THEN_SHIFT	1
#define MXC_CSPIINT_TFEN_SHIFT		2
#define MXC_CSPIINT_RREN_SHIFT		3
#define MXC_CSPIINT_RHEN_SHIFT       4
#define MXC_CSPIINT_RFEN_SHIFT        5
#define MXC_CSPIINT_ROEN_SHIFT        6

#define MXC_HIGHPOL	0x0
#define MXC_NOPHA	0x0
#define MXC_LOWSSPOL		0x0

#define MXC_CSPISTAT_TE		0
#define MXC_CSPISTAT_TH		1
#define MXC_CSPISTAT_TF		2
#define MXC_CSPISTAT_RR		3
#define MXC_CSPISTAT_RH		4
#define MXC_CSPISTAT_RF		5
#define MXC_CSPISTAT_RO		6

#define MXC_CSPIPERIOD_32KHZ	(1 << 15)



struct spi_transfer {
	/* it's ok if tx_buf == rx_buf (right?)
	 * for MicroWire, one buffer must be null
	 * buffers must work with dma_*map_single() calls, unless
	 *   spi_message.is_dma_mapped reports a pre-existing mapping
	 */
	const void	*tx_buf;
	void		*rx_buf;
	unsigned int	len;

	dma_addr_t	tx_dma;
	dma_addr_t	rx_dma;

	unsigned int	cs_change:1;
	unsigned char bits_per_word;
	unsigned short		delay_usecs;
	unsigned int		speed_hz;

};


#define	BITBANG_CS_ACTIVE	1	/* normally nCS, active low */
#define	BITBANG_CS_INACTIVE	0

struct mxc_spi_unique_def {
	/* Width of valid bits in MXC_CSPIINT */
	unsigned int intr_bit_shift;
	/* Chip Select shift */
	unsigned int cs_shift;
	/* Bit count shift */
	unsigned int bc_shift;
	/* Bit count mask */
	unsigned int bc_mask;
	/* Data Control shift */
	unsigned int drctrl_shift;
	/* Transfer Complete shift */
	unsigned int xfer_complete;
	/* Bit counnter overflow shift */
	unsigned int bc_overflow;
	/* FIFO Size */
	unsigned int fifo_size;
	/* Control reg address */
	unsigned int ctrl_reg_addr;
	/* Status reg address */
	unsigned int stat_reg_addr;
	/* Period reg address */
	unsigned int period_reg_addr;
	/* Test reg address */
	unsigned int test_reg_addr;
	/* Reset reg address */
	unsigned int reset_reg_addr;
	/* SPI mode mask */
	unsigned int mode_mask;
	/* SPI enable */
	unsigned int spi_enable;
	/* XCH bit */
	unsigned int xch;
	/* Spi mode shift */
	unsigned int mode_shift;
	/* Spi master mode enable */
	unsigned int master_enable;
	/* TX interrupt enable diff */
	unsigned int tx_inten_dif;
	/* RX interrupt enable bit diff */
	unsigned int rx_inten_dif;
	/* Interrupt status diff */
	unsigned int int_status_dif;
	/* Low pol shift */
	unsigned int low_pol_shift;
	/* Phase shift */
	unsigned int pha_shift;
	/* SS control shift */
	unsigned int ss_ctrl_shift;
	/* SS pol shift */
	unsigned int ss_pol_shift;
	/* Maximum data rate */
	unsigned int max_data_rate;
	/* Data mask */
	unsigned int data_mask;
	/* Data shift */
	unsigned int data_shift;
	/* Loopback control */
	unsigned int lbc;
	/* RX count off */
	unsigned int rx_cnt_off;
	/* RX count mask */
	unsigned int rx_cnt_mask;
	/* Reset start */
	unsigned int reset_start;
	/* SCLK control inactive state shift */
	unsigned int sclk_ctl_shift;
};


#define	SPI_CPHA	0x01			/* clock phase */
#define	SPI_CPOL	0x02			/* clock polarity */
#define	SPI_MODE_0	(0|0)			/* (original MicroWire) */
#define	SPI_MODE_1	(0|SPI_CPHA)
#define	SPI_MODE_2	(SPI_CPOL|0)
#define	SPI_MODE_3	(SPI_CPOL|SPI_CPHA)
#define	SPI_CS_HIGH	0x04			/* chipselect active high? */
#define	SPI_LSB_FIRST	0x08			/* per-word bits-on-wire */
#define	SPI_3WIRE	0x10			/* SI/SO signals shared */
#define	SPI_LOOP	0x20			/* loopback mode */
#define	SPI_NO_CS	0x40			/* 1 dev/bus, no chipselect */
#define	SPI_READY	0x80			/* slave pulls low to pause */

struct spi_device {
	struct mxc_spi	*master;
	unsigned int			max_speed_hz;
	unsigned char			chip_select;
	unsigned char			mode;
	unsigned char			bits_per_word;
};

struct mxc_spi_xfer {
    const void *tx_buf;
	void *rx_buf;
	unsigned int count;
	unsigned int rx_count;
	void (*rx_get)(struct mxc_spi *, unsigned int val);
	unsigned int (*tx_get) (struct mxc_spi *);
};

struct mxc_spi{
    signed short			bus_num;
	unsigned short			num_chipselect;
	unsigned short			dma_alignment;
	unsigned short			mode_bits;
	unsigned short			flags;
	struct mxc_spi_xfer transfer;
	void *base;
	int irq;
	unsigned long spi_ipg_clk;
    struct mxc_spi_unique_def *spi_ver_def;
	void *ctrl_addr;
	void *stat_addr;
	void *period_addr;
	void *test_addr;
	void *reset_addr;

	void (*chipselect_active) (int cspi_mode, int status, int chipselect);
	void (*chipselect_inactive) (int cspi_mode, int status, int chipselect);
    
	int			(*setup)(struct spi_device *spi);
	void			(*cleanup)(struct spi_device *spi);

    int	(*setup_transfer)(struct spi_device *spi,struct spi_transfer *t);

	void	(*chipselect)(struct spi_device *spi, int is_on);
    #define	BITBANG_CS_ACTIVE	1	/* normally nCS, active low */
    #define	BITBANG_CS_INACTIVE	0
	int	(*txrx_bufs)(struct spi_device *spi, struct spi_transfer *t);

	/* txrx_word[SPI_MODE_*]() just looks like a shift register */
	unsigned int	(*txrx_word[4])(struct spi_device *spi,
			unsigned nsecs,
			unsigned int word, unsigned char bits);
};


extern void imx25_spi_setup(struct spi_device *spi,unsigned int speed, 
						    unsigned char bits_per_word,int ssN);
extern int poll_spi_write(struct spi_device *spi,void *buf, size_t len);

extern int hfc_tft_spi_probe(void);
extern struct spi_device  *hfc_get_tft_device(void);

extern int hfc_pn512_probe(void);
extern struct spi_device  *hfc_get_pn512_device(void);

extern struct spi_device  *hfc_get_hc595_device(void);

#endif

