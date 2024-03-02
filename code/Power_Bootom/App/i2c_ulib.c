#include "i2c_ulib.h"
void i2c_ulib_feq(struct i2c_ulib_dev_t *dev , uint8_t feq)
{
    dev->delay = 1000 / feq / 2;
}

void i2c_ulib_start(struct i2c_ulib_dev_t *dev)
{
    /*
    
    ---
       |            <- SDA
       -----------
    
    -------
          |         <- SCL
          -----------
        
    */
    dev->sda_write(1);
    dev->scl_write(1);
    dev->us_delay(dev->delay);
    dev->sda_write(0);
    dev->us_delay(dev->delay);
    dev->scl_write(0);
    dev->us_delay(dev->delay);
}

void i2c_ulib_stop(struct i2c_ulib_dev_t *dev)
{
    /*
    
          --------
          |            <- SDA
    ------
    
       -----------
       |               <- SCL
    ----
        
    */
    dev->scl_write(0);
    dev->us_delay(dev->delay);    
    dev->sda_write(0);
    dev->us_delay(dev->delay);
    dev->scl_write(1);
    dev->us_delay(dev->delay);
    dev->sda_write(1);
    dev->us_delay(dev->delay);
}

/* 读取从机 ack  0:应答 1:非应答 */
uint8_t i2c_ulib_s_ack(struct i2c_ulib_dev_t *dev)
{
    uint8_t ack;
    dev->sda_write(1);
    dev->sda_dir(0);
    dev->us_delay(dev->delay);
    dev->scl_write(1);
    dev->us_delay(dev->delay);
    if (dev->sda_read())
        ack = 1;  // 非应答
    else
        ack = 0; // 应答
    
    dev->scl_write(0);
    dev->sda_dir(1);
    dev->us_delay(dev->delay);
    return ack;
}

/* 主机发送 ack */
void i2c_ulib_m_ack(struct i2c_ulib_dev_t *dev)
{
    dev->sda_write(0);
    dev->us_delay(dev->delay);
    dev->scl_write(1);
    dev->us_delay(dev->delay);
    dev->scl_write(0);
    dev->us_delay(dev->delay);
    dev->sda_write(1);
}

/* 主机发送 nack */
void i2c_ulib_m_nack(struct i2c_ulib_dev_t *dev)
{    
    dev->sda_write(1);
    dev->us_delay(dev->delay);
    dev->scl_write(1);
    dev->us_delay(dev->delay);
    dev->scl_write(0);    
}

void i2c_ulib_write_byte(struct i2c_ulib_dev_t *dev, uint8_t data)
{
    uint8_t i;    
    for (i = 0; i < 8; i++)
    {
        if(data & 0x80)
            dev->sda_write(1);
        else
            dev->sda_write(0);

        dev->scl_write(1);
        dev->us_delay(dev->delay);
        dev->scl_write(0);
        if (i == 7)
            dev->sda_write(1);
        data <<= 1;
        dev->us_delay(dev->delay);
    }
}

uint8_t i2c_ulib_read_byte(struct i2c_ulib_dev_t *dev, uint8_t mack)
{
    uint8_t i,temp;

    dev->sda_dir(0);
    dev->us_delay(dev->delay);
    for (i = 0; i < 8; i++)
    {      
        temp <<= 1;  
        dev->scl_write(1);
        dev->us_delay(dev->delay);        
        if (dev->sda_read())
            temp |= 0x01;
        dev->scl_write(0);
        dev->us_delay(dev->delay);
    }
    dev->sda_dir(1);
    dev->us_delay(dev->delay);
    if (mack)
        i2c_ulib_m_ack(dev);
    else
        i2c_ulib_m_nack(dev);
    return temp;
}

// 0:成功 1:失败
uint8_t i2c_ulib_write(struct i2c_ulib_dev_t *dev, uint8_t addr, uint8_t *data , uint16_t len ,uint8_t stop)
{
    uint16_t i;
    i2c_ulib_start(dev);
    i2c_ulib_write_byte(dev, addr<<1);
    if (i2c_ulib_s_ack(dev))
    {
        i2c_ulib_stop(dev);
        return 1;
    }       

    for (i = 0; i < len; i++)
    {
        i2c_ulib_write_byte(dev, data[i]);
        if (i2c_ulib_s_ack(dev))
        {
            i2c_ulib_stop(dev);
            return 1;
        }
            
    }
    if(stop)
        i2c_ulib_stop(dev);
    return 0;
}

uint8_t i2c_ulib_read(struct i2c_ulib_dev_t *dev, uint8_t addr, uint8_t *data , uint16_t len) 
{
    uint16_t i;
    i2c_ulib_start(dev);
    i2c_ulib_write_byte(dev, addr<<1 | 0x01);
    if (i2c_ulib_s_ack(dev))
    {
        i2c_ulib_stop(dev);
        return 1;
    }       

    for (i = 0; i < len; i++)
    {
        data[i] = i2c_ulib_read_byte(dev, i == len - 1 ? 0 : 1);
    }
    i2c_ulib_stop(dev);
    return 0;
};




/*---------------------------------- test -----------------------------------*/
#if 0
#include "gd32e23x.h"
void test_i2c_hw_init()
{
    // SCL -> PB6
    // SDA -> PB7

    GPIO_InitTypeDef gpioDef;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpioDef.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioDef.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpioDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &gpioDef);


}
void scl_write(uint8_t data)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)data);
};
void sda_write(uint8_t data)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)data);

};
void sda_dir(uint8_t data)
{
    GPIO_InitTypeDef gpioDef;
    gpioDef.GPIO_Mode = data ? GPIO_Mode_Out_PP : GPIO_Mode_IPU;
    gpioDef.GPIO_Pin = GPIO_Pin_7;
    gpioDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &gpioDef);

};
uint8_t sda_read()
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
};
void i2c_delay(uint32_t x)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < 1000; j++)
            __NOP();
    }
}
void test_i2c()
{
    uint8_t data[5] = {0x55, 0x66, 0x77, 0x88, 0x99};
    struct i2c_ulib_dev_t dev;

    test_i2c_hw_init();

    i2c_ulib_feq(&dev, 100);
    dev.us_delay = i2c_delay;
    dev.scl_write = scl_write;
    dev.sda_write = sda_write;
    dev.sda_read = sda_read;
    dev.sda_dir = sda_dir;

    //i2c_ulib_write_byte(&dev, 0x55);
    //i2c_ulib_write(&dev, 0x50, data, 2);
    i2c_ulib_read(&dev, 0x50, data, 4);
    while (1);
    
}
#endif