#include "ina226.h"
#include "math.h"
/*https://github.com/jarzebski/Arduino-INA226/blob/dev/src/INA226.cpp*/


#define INA226_REG_MANUFACTURER         0xFE
#define INA226_REG_DIE_ID               0xFF


void ina226_init(struct ina226_t *dev)
{
    uint16_t config = 0x4127;
    dev->write16(dev->addr, INA226_REG_CONFIG, config);
}


void ina226_config(struct ina226_t *dev, ina226_averages_t avg, ina226_busConvTime_t busConvTime, ina226_shuntConvTime_t shuntConvTime, ina226_mode_t mode)
{
    uint16_t config = 0x0000;
    config |= (avg << 9);
    config |= (busConvTime << 6);
    config |= (shuntConvTime << 3);
    config |= (mode << 0);

    dev->vBusMax = 36.0;
    dev->vShuntMax = 0.08192;

    dev->write16(dev->addr, INA226_REG_CONFIG, config);
}
uint16_t ina226_config_read(struct ina226_t *dev)
{
    uint16_t config;
    dev->read16(dev->addr, INA226_REG_CONFIG, &config);
    return config;
}
void ina226_calibrate(struct ina226_t *dev, float rShunt, float iMaxExpected)
{
   

    /*
    uint16_t calibrationValue;
    rShunt = rShuntValue;

    float iMaxPossible, minimumLSB;

    iMaxPossible = vShuntMax / rShunt;

    minimumLSB = iMaxExpected / 32767;

    currentLSB = (uint32_t)(minimumLSB * 100000000);
    currentLSB /= 100000000;
    currentLSB /= 0.0001;
    currentLSB = ceil(currentLSB);
    currentLSB *= 0.0001;

    powerLSB = currentLSB * 25;

    calibrationValue = (uint16_t)((0.00512) / (currentLSB * rShunt));

    writeRegister16(INA226_REG_CALIBRATION, calibrationValue);
    */

    uint16_t calibrationValue;
    dev->rShunt = rShunt;
    float iMaxPossible, minimumLSB;
    iMaxPossible = dev->vShuntMax / dev->rShunt;
    minimumLSB = iMaxExpected / 32767;
    dev->currentLSB = (uint32_t)(minimumLSB * 100000000);
    dev->currentLSB /= 100000000;
    dev->currentLSB /= 0.0001;
    dev->currentLSB = ceil(dev->currentLSB);
    dev->currentLSB *= 0.0001;
    dev->powerLSB = dev->currentLSB * 25;
    calibrationValue = (uint16_t)((0.00512) / (dev->currentLSB * dev->rShunt));
    dev->write16(dev->addr, INA226_REG_CALIBRATION, calibrationValue);




}
uint16_t ina226_calibrate_read(struct ina226_t *dev)
{
    uint16_t cal;
    dev->read16(dev->addr, INA226_REG_CALIBRATION, &cal);
    return cal;
}
float ina226_get_max_possible_current(struct ina226_t *dev)
{
    return dev->vShuntMax / dev->rShunt;
}

float ina226_get_max_current(struct ina226_t *dev)
{
    float max_current = dev->currentLSB * 32767.0;
    float max_possible_current = ina226_get_max_possible_current(dev);
    return max_current < max_possible_current ? max_current : max_possible_current;
}

float ina226_get_max_power(struct ina226_t *dev)
{
    return dev->vBusMax * dev->vShuntMax / dev->rShunt;
}

float ina226_get_max_shunt_voltage(struct ina226_t *dev)
{
    float max_voltage =  ina226_get_max_current(dev) * dev->rShunt;
    return max_voltage < dev->vShuntMax ? max_voltage : dev->vShuntMax;
}

float ina226_read_bus_power(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_POWER, &data);
    return data * dev->powerLSB;
};

float ina226_read_shunt_current(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_CURRENT, &data);
    return data * dev->currentLSB;
};

int16_t ina226_read_raw_shunt_current(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_CURRENT, &data);
    return data;
};  

float ina226_read_shunt_voltage(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_SHUNTVOLTAGE, &data);
    return data * 0.0000025;
};

float ina226_read_bus_voltage(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_BUSVOLTAGE, &data);
    return data* 1.25 * 0.001;
};

//getAverages
uint16_t ina226_get_averages(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_CONFIG, &data);
    return (data >> 9) & 0x07;
};

//getBusConversionTime
uint16_t ina226_get_bus_conversion_time(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_CONFIG, &data);
    return (data >> 6) & 0x07;
};

//getShuntConversionTime
uint16_t ina226_get_shunt_conversion_time(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_CONFIG, &data);
    return (data >> 3) & 0x07;
};

// getMode
uint16_t ina226_get_mode(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_CONFIG, &data);
    return (data >> 0) & 0x07;
};

// setMaskEnable
void ina226_set_mask_enable(struct ina226_t *dev, uint16_t mask)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data |= mask;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// getMaskEnable
uint16_t ina226_get_mask_enable(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    return data;
};

// enableShuntOverLimitAlert
void ina226_enable_shunt_over_limit_alert(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data |= INA226_BIT_SOL;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// enableBusOvertLimitAlert
void ina226_enable_bus_over_limit_alert(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data |= INA226_BIT_BOL;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// enableBusUnderLimitAlert
void ina226_enable_bus_under_limit_alert(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data |= INA226_BIT_BUL;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// enableOverPowerLimitAlert
void ina226_enable_over_power_limit_alert(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data |= INA226_BIT_POL;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// enableConversionReadyAlert
void ina226_enable_conversion_ready_alert(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data |= INA226_BIT_CNVR;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// disableAlerts
void ina226_disable_alerts(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    data &= ~(INA226_BIT_SOL | INA226_BIT_SUL | INA226_BIT_BOL | INA226_BIT_BUL | INA226_BIT_POL | INA226_BIT_CNVR);
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// setBusVoltageLimit
void ina226_set_bus_voltage_limit(struct ina226_t *dev, float limit)
{
    uint16_t data = (uint16_t)(limit / 0.00125);
    dev->write16(dev->addr, INA226_REG_BUSVOLTAGE, data);
};

// setShuntVoltageLimit
void ina226_set_shunt_voltage_limit(struct ina226_t *dev, float limit)
{
    uint16_t data = (uint16_t)(limit / 0.0000025);
    dev->write16(dev->addr, INA226_REG_SHUNTVOLTAGE, data);
};

// setPowerLimit
void ina226_set_power_limit(struct ina226_t *dev, float limit)
{
    uint16_t data = (uint16_t)(limit / dev->powerLSB);
    dev->write16(dev->addr, INA226_REG_POWER, data);
};

// setAlertInvertedPolarity
void ina226_set_alert_inverted_polarity(struct ina226_t *dev, uint8_t inverted)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    if (inverted)
        data |= INA226_BIT_APOL;
    else
        data &= ~INA226_BIT_APOL;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// setAlertLatch
void ina226_set_alert_latch(struct ina226_t *dev, uint8_t latch)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    if (latch)
        data |= INA226_BIT_LEN;
    else
        data &= ~INA226_BIT_LEN;
    dev->write16(dev->addr, INA226_REG_MASKENABLE, data);
};

// isMathOverflow
uint8_t ina226_is_math_overflow(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    return (data & INA226_BIT_OVF) ? 1 : 0;
};

// isAlert
uint8_t ina226_is_alert(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MASKENABLE, &data);
    return (data & INA226_BIT_AFF) ? 1 : 0;
};

//   uint16_t getManufacturerID();   //  should return 0x5449
uint16_t ina226_get_manufacturer_id(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_MANUFACTURER, &data);
    return data;
};


//  uint16_t getDieID();            //  should return 0x2260
uint16_t ina226_get_die_id(struct ina226_t *dev)
{
    uint16_t data;
    dev->read16(dev->addr, INA226_REG_DIE_ID, &data);
    return data;
};


