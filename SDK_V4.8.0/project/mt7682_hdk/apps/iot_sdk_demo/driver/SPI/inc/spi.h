#ifndef	__INCLUDE_SPI_H
#define	__INCLUDE_SPI_H



#if 0
/***这一组spi，用于控制BCM53115(SWITCH)、RA1027(FPGA)和XRT83VSH38(LIU)***/
/*SPI SCK用P0.4*/
#define SPI_CLK_PORT			(0)
#define SPI_CLK_PIN				BIT(4)
#define SPI_CLK_OUT				(IODIR_X(SPI_CLK_PORT) |= SPI_CLK_PIN)
#define SPI_CLK_H				(IOSET_X(SPI_CLK_PORT) |= SPI_CLK_PIN)
#define SPI_CLK_L				(IOCLR_X(SPI_CLK_PORT) |= SPI_CLK_PIN)

/*SPI MISO用P0.5*/
#define SPI_MISO_PORT			(0)
#define SPI_MISO_PIN			BIT(5)
#define SPI_MISO_IN				(IODIR_X(SPI_MISO_PORT) &= ~SPI_MISO_PIN)
#define SPI_MISO_INPUT			(IOPIN_X(SPI_MISO_PORT) & SPI_MISO_PIN)

/*SPI MOSI用P0.6*/
#define SPI_MOSI_PORT			(0)
#define SPI_MOSI_PIN			BIT(6)
#define SPI_MOSI_OUT			(IODIR_X(SPI_MOSI_PORT) |= SPI_MOSI_PIN)
#define SPI_MOSI_H				(IOSET_X(SPI_MOSI_PORT) |= SPI_MOSI_PIN)
#define SPI_MOSI_L				(IOCLR_X(SPI_MOSI_PORT) |= SPI_MOSI_PIN)
#endif

/***这一组SPI，用于控制FLASH芯片***/
/*FLASH SPI SCK用HAL_GPIO_17*/
#define FLASH_SPI_CS_OUT		(void)hal_gpio_set_direction(HAL_GPIO_17, HAL_GPIO_DIRECTION_OUTPUT)
#define FLASH_SPI_CS_H			(void)hal_gpio_set_output(HAL_GPIO_17,HAL_GPIO_DATA_HIGH)
#define FLASH_SPI_CS_L			(void)hal_gpio_set_output(HAL_GPIO_17,HAL_GPIO_DATA_LOW)


/*FLASH SPI SCK用HAL_GPIO_16*/
#define FLASH_SPI_CLK_OUT		(void)hal_gpio_set_direction(HAL_GPIO_16, HAL_GPIO_DIRECTION_OUTPUT)
#define FLASH_SPI_CLK_H			(void)hal_gpio_set_output(HAL_GPIO_16,HAL_GPIO_DATA_HIGH)
#define FLASH_SPI_CLK_L			(void)hal_gpio_set_output(HAL_GPIO_16,HAL_GPIO_DATA_LOW)		


/*FLASH SPI MISO用HAL_GPIO_14*/
#define FLASH_SPI_MISO_IN		(void)hal_gpio_set_direction(HAL_GPIO_14, HAL_GPIO_DIRECTION_INPUT)
#define FLASH_SPI_MISO_INPUT	hal_gpio_get_input_pin(HAL_GPIO_14)


/*FLASH SPI MOSI用HAL_GPIO_15*/
#define FLASH_SPI_MOSI_OUT		(void)hal_gpio_set_direction(HAL_GPIO_15, HAL_GPIO_DIRECTION_OUTPUT)
#define FLASH_SPI_MOSI_H		(void)hal_gpio_set_output(HAL_GPIO_15,HAL_GPIO_DATA_HIGH)
#define FLASH_SPI_MOSI_L		(void)hal_gpio_set_output(HAL_GPIO_15,HAL_GPIO_DATA_LOW)


/***FLASH二选一选通控制(初始化为高，FPGA自动加载，需MCU访问FLASH时置低)***/
/*#define FPGA_FLASH_CTL_PORT			(1)
#define FPGA_FLASH_CTL_PIN			BIT(23)
#define FPGA_FLASH_CTL_PORT_OUT		(IODIR_X(FPGA_FLASH_CTL_PORT) |= FPGA_FLASH_CTL_PIN)
#define FPGA_FLASH_CTL_PORT_H		(IOSET_X(FPGA_FLASH_CTL_PORT) |= FPGA_FLASH_CTL_PIN)
#define FPGA_FLASH_CTL_PORT_L		(IOCLR_X(FPGA_FLASH_CTL_PORT) |= FPGA_FLASH_CTL_PIN)
*/


OTP_VOID spi_send_byte_msb_flash(OTP_UINT8 data);
OTP_UINT8 spi_rcv_byte_msb_flash(void);
OTP_VOID spi_cs_flash(OTP_UINT8 data);

#if 0
OTP_VOID spi_send_byte_msb(OTP_UINT8 data);
OTP_UINT8 spi_rcv_byte_msb(void);

OTP_VOID spi_send_byte_msb_fpga(OTP_UINT8 data);
OTP_UINT8 spi_rcv_byte_msb_fpga(void);

OTP_VOID spi_send_byte_lsb(OTP_UINT8 data);
OTP_UINT8 spi_rcv_byte_lsb(void);
#endif
#endif

