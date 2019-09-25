
/************************************************************************/
/*	  瑞斯康达科技发展有限公司			版权所有						*/
/************************************************************************/
/*	 @@BEGAIN_INTERNAL_LEGAL@@											*/
/*																		*/
/*					 Copyright(C) Description							*/
/* Raisecom	 Science & Technology Development Co.,Ltd. Beijing,	China	*/
/*	   Unpublished work-rights reserved	under the China	Copyright Act.	*/
/*	   Use,duplication,	or disclosure by the government	is subject to	*/
/*	   restrictions	set	forth in the CDTT commercial license agreement.	*/
/*																		*/
/*	 @@END_INTERNAL_LEGAL@@												*/
/************************************************************************/
/*	 程序文件名：spi.c													*/
/*	 Filename   ：spi.c													*/
/*	 作者	   ：王海龙													*/
/*	 Author	   ：Wang H.L.												*/
/*	 日期	   ：08/08/2008												*/
/*	 Date	   ：08/08/2008												*/
/*	 版本	   ：1.0													*/
/*	 Version     ：1.0													*/
/*	 实现目的  ： GPIO仿真SPI的驱动										*/
/*	 Purpose   ：														*/
/*																		*/
/************************************************************************/


//#ifdef INCLUDE_SPI
#include "type_common.h"
#include "hal_gpio.h"
#include "spi.h"



/*延时100ns*/
void delay_1us(OTP_UINT16 time)
{
	OTP_UINT16 i,j;
	
	for( i = 0; i < time; i++ )
	{
		for( j = 0; j < 1; j++ )
		{

		}
	}
}


hal_gpio_data_t hal_gpio_get_input_pin(hal_gpio_pin_t gpio_pin)
{

    hal_gpio_data_t data;

    (void)hal_gpio_get_input(gpio_pin, &data);

    return data;

}



/*MSB*/
OTP_VOID spi_send_byte_msb_flash(OTP_UINT8 data)
{
	OTP_UINT8 temp = 0;

	FLASH_SPI_MOSI_OUT;
	FLASH_SPI_CLK_OUT;
	FLASH_SPI_CLK_L;

	for(temp = 0x80; temp != 0; temp >>= 1)
	{
		if(temp & data) 
		{
			FLASH_SPI_MOSI_H;
		}
		else
		{
			FLASH_SPI_MOSI_L;
		}

		FLASH_SPI_CLK_H;
		delay_1us(1);
		FLASH_SPI_CLK_L;
		delay_1us(1);
	}
}

/*MSB,上升沿采样*/
OTP_UINT8 spi_rcv_byte_msb_flash(void)
{
	OTP_UINT8 temp = 0;
	OTP_UINT8 val = 0;	

	FLASH_SPI_MISO_IN;
	FLASH_SPI_CLK_OUT;
	FLASH_SPI_CLK_L;

	for(temp = 0x80; temp != 0; temp >>= 1)
	{ 
		FLASH_SPI_CLK_H;
		delay_1us(1);

		if(FLASH_SPI_MISO_INPUT)
		{
			val |= temp;
		}

		FLASH_SPI_CLK_L;
		delay_1us(1);	
	}

	return(val);
}

OTP_VOID spi_cs_flash(OTP_UINT8 data)
{
	
	if(data==0)
	{
		FLASH_SPI_CS_H;		
	}
	else
	{
		FLASH_SPI_CS_L;		
	}

}

#if 0
/*MSB*/
OTP_VOID spi_send_byte_msb(OTP_UINT8 data)
{
	OTP_UINT8 temp = 0;

	SPI_MOSI_OUT;
	SPI_CLK_OUT;
	SPI_CLK_L;

	for(temp = 0x80; temp != 0; temp >>= 1)
	{
		if(temp & data) 
		{
			SPI_MOSI_H;
		}
		else
		{
			SPI_MOSI_L;
		}

		SPI_CLK_H;
		NOP2;
		SPI_CLK_L;
		NOP2;
	}
}

/*MSB,上升沿采样*/
OTP_UINT8 spi_rcv_byte_msb(void)
{
	OTP_UINT8 temp = 0;
	OTP_UINT8 val = 0;	

	SPI_MISO_IN;
	SPI_CLK_OUT;
	SPI_CLK_L;

	for(temp = 0x80; temp != 0; temp >>= 1)
	{ 
		SPI_CLK_H;
		NOP2;

		if(SPI_MISO_INPUT)
		{
			val |= temp;
		}

		SPI_CLK_L;
		NOP2;	
	}

	return(val);
}

/*MSB*/
OTP_VOID spi_send_byte_msb_fpga(OTP_UINT8 data)
{
	OTP_UINT8 temp = 0;

	SPI_MOSI_OUT;
	SPI_CLK_OUT;
	SPI_CLK_L;

	for(temp = 0x80; temp != 0; temp >>= 1)
	{
		if(temp & data) 
		{
			SPI_MOSI_H;
		}
		else
		{
			SPI_MOSI_L;
		}

		SPI_CLK_H;
		delay_10us(1);
		SPI_CLK_L;
		delay_10us(1);
	}
}

/*MSB,上升沿采样*/
OTP_UINT8 spi_rcv_byte_msb_fpga(void)
{
	OTP_UINT8 temp = 0;
	OTP_UINT8 val = 0;

	SPI_MISO_IN;
	SPI_CLK_OUT;
	SPI_CLK_L;

	for(temp = 0x80; temp != 0; temp >>= 1)
	{ 
		if(SPI_MISO_INPUT)
		{
			val |= temp;
		}
		SPI_CLK_H;
		delay_10us(1);
		SPI_CLK_L;
		delay_10us(1);
	}

	return(val);
}

/*LSB，上升沿采样*/
OTP_VOID spi_send_byte_lsb(OTP_UINT8 data)
{
	OTP_UINT8 temp = 0;

	SPI_MOSI_OUT;
	SPI_CLK_OUT;
	SPI_CLK_L;

	for(temp = 1; temp != 0; temp <<= 1)
	{
		if(temp & data) 
		{
			SPI_MOSI_H;
		}
		else
		{
			SPI_MOSI_L;
		}

		SPI_CLK_H;
		NOP2;
		SPI_CLK_L;
		NOP2;
	} 
}

/*LSB，上升沿采样*/
OTP_UINT8 spi_rcv_byte_lsb(void)
{
	OTP_UINT8 temp = 0;
	OTP_UINT8 val = 0;	

	SPI_MISO_IN;
	SPI_CLK_OUT;
	SPI_CLK_L;

	for(temp = 1; temp != 0; temp <<= 1)
	{
		if(SPI_MISO_INPUT)
		{
			val |= temp;
		}

		SPI_CLK_H;
		NOP2;
		SPI_CLK_L;
		NOP2;
	}

	return(val);
}
#endif
//#endif

