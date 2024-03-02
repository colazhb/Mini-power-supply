#include "lcd_spi_drv.h"
#include "board_pin_def.h"
#include "lcd.h"
struct lcd_spi_drv_obj lcd_drv;
void cs(uint8_t level)
{
    gpio_bit_write(LCD_CS_GPIO_PORT,LCD_CS_PIN,level);
 // gpio_bit_write(LCD_SCL_GPIO_PORT,LCD_SCL_PIN,level);
 // gpio_bit_write(LCD_SDA_GPIO_PORT,LCD_SDA_PIN,level);  
}

void rst(uint8_t level)
{
    gpio_bit_write(LCD_RST_GPIO_PORT,LCD_RST_PIN,level);
}

void dc(uint8_t level)
{
    gpio_bit_write(LCD_RS_GPIO_PORT,LCD_RS_PIN,level);
}

void blk(uint8_t level)
{
    gpio_bit_write(LCD_PWM_GPIO_PORT,LCD_PWM_PIN,level);
}

void clk(uint8_t level)
{
    gpio_bit_write(LCD_SCL_GPIO_PORT,LCD_SCL_PIN,level);
}

void sda(uint8_t level)
{
    gpio_bit_write(LCD_SDA_GPIO_PORT,LCD_SDA_PIN,level);
}

void lcd_soft_spi_send(uint8_t dat) 
{	
	uint8_t i;
	
	for(i=0;i<8;i++)
	{			  
		lcd_drv.clk(0);
 
		if(dat&0x80)
		{
		   lcd_drv.sda(1);
		}
		else
		{
		    lcd_drv.sda(0);
		}
		lcd_drv.clk(1);
		dat<<=1;
	}	
	
}


void spi_send(uint8_t data)
{
#if USE_HW_SPI
     while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) {
      }
      spi_i2s_data_transmit(SPI0, data);  
#else
 lcd_soft_spi_send(data);

#endif
}
void lcd_gpio_config(void)
{
   // 初始化gpio
    gpio_mode_set(LCD_CS_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_CS_PIN);
    gpio_output_options_set(LCD_CS_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_CS_PIN);

    gpio_mode_set(LCD_RS_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_RS_PIN);
    gpio_output_options_set(LCD_RS_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_RS_PIN);

    gpio_mode_set(LCD_RST_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_RST_PIN);
    gpio_output_options_set(LCD_RST_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_RST_PIN);

    gpio_mode_set(LCD_PWM_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_PWM_PIN);
    gpio_output_options_set(LCD_PWM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_PWM_PIN);

#if USE_HW_SPI

    /* configure SPI0 GPIO: NSS/PA4, SCK/PA5, MOSI/PA7 */
    gpio_af_set(GPIOA, GPIO_AF_0,  GPIO_PIN_5  | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,  GPIO_PIN_5  | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_5  | GPIO_PIN_7);
   
#else
    gpio_mode_set(LCD_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_SCL_PIN);
    gpio_output_options_set(LCD_SCL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_SCL_PIN);

    gpio_mode_set(LCD_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_SDA_PIN);
    gpio_output_options_set(LCD_SDA_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_SDA_PIN);

#endif


}
void lcd_spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);

    spi_struct_para_init(&spi_init_struct);

    /* configure SPI0 parameter */

    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);


    /* configure SPI1 byte access to FIFO */
    spi_fifo_access_size_config(SPI0, SPI_BYTE_ACCESS);
    spi_nss_output_disable(SPI0);
    spi_enable(SPI0);

   // spi_bidirectional_transfer_config(SPI0,SPI_BIDIRECTIONAL_TRANSMIT);
   

}

void lcd_spi_drv_init(struct lcd_spi_drv_obj *obj)
{
 
    lcd_gpio_config();

    // 初始化spi
    lcd_spi_config();


    // 初始化dma

    obj->cs = cs;
    obj->dc = dc;
    obj->rst = rst;
    obj->blk = blk;
    obj->clk = clk;
    obj->sda = sda;
    obj->spi_send = spi_send;
}

// spi 传输接口

