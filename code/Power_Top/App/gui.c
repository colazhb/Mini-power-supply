#include "lcd.h"
#include "board_pin_def.h"
#include "systick.h"
#include "lcd_spi_drv.h"
#include "gui.h"
#include "power_data.h"
#include "pic.h"
#include "led_rgb.h"
//extern struct power_data_t power_data;
uint8_t a_tl = 61; // 宽度
uint8_t a_th = 36; // 高度

// 中间最大区域
uint8_t b_tl = 124;// 中间区域宽度
uint8_t b_th = 60;// 中间区域高度

uint8_t r_l = 3; // 圆角半径

// 最下面一行区域 128 - 4 - a_th - b_th - 3;
uint8_t c_tl = 25;


// out: 0:off 1:on 坐标
uint8_t gui_out_x = 6;
uint8_t gui_out_y = 16;

// vin 坐标
uint8_t gui_vin_x = 6;
uint8_t gui_vin_y = 26;

// set v 坐标
uint8_t gui_set_v_x = 12;
uint8_t gui_set_v_y = 20;

// set i 坐标
uint8_t gui_set_i_x = 12;
uint8_t gui_set_i_y = 36;

//  pwr v 坐标
uint8_t gui_pwr_v_x = 10;
uint8_t gui_pwr_v_y = 61;

//  pwr i 坐标
uint8_t gui_pwr_i_x = 10;
uint8_t gui_pwr_i_y = 88;

// p t 坐标
uint8_t gui_p_t_x = 6;
uint8_t gui_p_t_y = 114;

// alarm 坐标
uint8_t gui_alarm_x = 6;
uint8_t gui_alarm_y = 125;

// gui的logo显示
void gui_logo()
{
	 // 水平扫描，从左到右，从上到下，高位在前 16位色-565
    LCD_ShowPicture(32, 32, 64, 64, (uint8_t *)gImage_1);
        
}

// 绘制边框
void gui_board_draw(uint16_t color)
{
    // 将屏幕分为四个区域,最上面一行平均分为2个
    // 中间区域最大
    // 下面一个区域最小，显示告警
    // 四周预留2个像素的边框
    // 最上面的两个区域的宽度

	LCD_DrawRoundRect(2, 2, a_tl, a_th, r_l, color);      //左上矩形
	LCD_DrawLine(2, 15, a_tl+2, 15, color);

    LCD_DrawRoundRect(a_tl+4, 2, a_tl, a_th, r_l, color); // 右上矩形
	LCD_DrawLine(a_tl+4, 15, 2*a_tl+4, 15, color);

	LCD_DrawRoundRect(2, a_th+4, b_tl, b_th, r_l, color);
    LCD_DrawRoundRect(2, a_th + b_th + 6, b_tl, c_tl, r_l, color);

}

// gui的外观初始化
void gui_init_board()
{	
	// 清屏
	LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //uint16_t b_c = LGRAY; // 边框颜色

    gui_board_draw(LGRAY);


    // 左上画 标题栏 STATUS,在字下画一条线分隔
    LCD_ShowString(15, 3, (uint8_t *)"STATUS", LGRAY,BLACK, 12, 0);

    // 输入两行文字, 第一行显示 OUT:ON/OFF   第二行显示 VIN：12.34V
	
    LCD_ShowString(gui_out_x, gui_out_y, (uint8_t *)"OUT:ON", LGRAY,BLACK, 12, 0);
    LCD_ShowString(gui_vin_x, gui_vin_y, (uint8_t *)"VIN:0V", LGRAY,BLACK, 12, 0);


    // 右上画 标题栏 SET,在字下画一条线分隔
    LCD_ShowString(a_tl+24, 3, (uint8_t *)"SET", LGRAY,BLACK, 12, 0);
    

    // 输入两行文字, 第一行显示 V: 12.34V   第二行显示 I: 0.12A
	gui_set_v_x = a_tl+10;
	gui_set_v_y = 16;
	gui_set_i_x = a_tl+10;
	gui_set_i_y = 26;

    LCD_ShowString(gui_set_v_x, gui_set_v_y, (uint8_t *)"V: 0.00", LGRAY,BLACK, 12, 0);
    LCD_ShowString(gui_set_i_x, gui_set_i_y, (uint8_t *)"I: 0.00", LGRAY,BLACK, 12, 0);

    // 中间区域显示实际电压,电流,功率 使用 14号字体
	gui_pwr_v_x = 10;
	gui_pwr_v_y = a_th+5;
	gui_pwr_i_x = 10;
	gui_pwr_i_y = a_th+32;
    LCD_ShowString(gui_pwr_v_x, gui_pwr_v_y, (uint8_t *)" 0.00 V", LGRAY,BLACK, 32, 0);
    LCD_ShowString(gui_pwr_i_x, gui_pwr_i_y, (uint8_t *)" 0.00 A", LGRAY,BLACK, 32, 0);
    

    // 最下面是信息栏，显示告警信息,使用12号字体,没有告警的时候 显示功率 0.00W 温度 0.0C
	gui_p_t_x = 6;
	gui_p_t_y = a_th+b_th+8;
	gui_alarm_x = 6;
	gui_alarm_y = a_th+b_th+8+11;
    LCD_ShowString(gui_p_t_x, gui_p_t_y, (uint8_t *)"P: 0.00W   T: 0.0C", LGRAY,BLACK, 12, 0);
    LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER VOLTAGE", LGRAY,BLACK, 12, 0);

}


// 将设置值转换为显示值
void conv_val_to_set_val()
{
	 power_data.set_v_h = (int8_t)(power_data.set_voltage / 1000);
	 power_data.set_v_l = (int8_t)(power_data.set_voltage % 1000 / 100);

	 power_data.set_i_h = (int8_t)(power_data.set_current / 1000);
	 power_data.set_i_l = (int8_t)(power_data.set_current % 1000 / 100);

	//printf("set_voltage:%d set_current:%d\r\n", power_data.set_voltage, power_data.set_current);
}

// GUI 初始化
void gui_init()
{
	//
	power_data.set_current  = 1000;
	power_data.set_voltage = 3300;
	power_data.ina_read_current = 0;
	power_data.ina_read_voltage = 0;
	power_data.input_voltage = 5000;
};

// 错误显示 0:无错误 1:有错误
/*
		    ALARM_NULL,
    ALARM_OVER_MAX_VOLTAGE,         // 过压,最大输出电压
    ALARM_OVER_MAX_CURRENT,         // 过流,最大输出电流
    ALARM_OVER_MAX_TEMP,            // 过温,最大温度
    ALARM_OVER_MAX_POWER,           // 过功率,最大输出功率
    ALARM_OVER_PROTECT_VOLTAGE,     // 过保护电压
    ALARM_OVER_PROTECT_CURRENT,     // 过保护电流
    ALARM_SC8815_OTP,               // 发生 OTP(过温) 故障中断
    ALARM_SC8815_SHORT,             // 发生短路故障中断
	*/
void gui_set_err(uint8_t err)
{
	if(err!=0 ) led_led_on(LED_R);

	switch (err)
	{
		case ALARM_NULL:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: ----        ", LGRAY,BLACK, 12, 0);
			break;
		case ALARM_OVER_MAX_VOLTAGE:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER VOLTAGE", RED,BLACK, 12, 0);
			break;
		case ALARM_OVER_MAX_CURRENT:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER CURRENT", RED,BLACK, 12, 0);
			break;
		case ALARM_OVER_MAX_TEMP:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER TEMP   ", RED,BLACK, 12, 0);
			break;
		case ALARM_OVER_MAX_POWER:	
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER POWER  ", RED,BLACK, 12, 0);
			break;
		case ALARM_OVER_PROTECT_VOLTAGE:	
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER VOLTAGE", YELLOW,BLACK, 12, 0);
			break;
		case ALARM_OVER_PROTECT_CURRENT:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER CURRENT", YELLOW,BLACK, 12, 0);
			break;
		case ALARM_SC8815_OTP:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: SC8815 OTP  ", LGRAY,BLACK, 12, 0);
			break;
		case ALARM_SC8815_SHORT:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: SC8815 SHORT", LGRAY,BLACK, 12, 0);
			break;
		default:
			LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: ----        ", LGRAY,BLACK, 12, 0);
			break;
	}
	
	// if(err)
	// {
	// 	 LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: OVER VOLTAGE", LGRAY,RED, 12, 0);
	// }
	// else
	// {
	// 	 LCD_ShowString(gui_alarm_x, gui_alarm_y, (uint8_t *)"ALARM: NULL        ", LGRAY,BLACK, 12, 0);
	// }
	


}

// 运行/停止显示 0:停止 1:运行
void gui_set_run_stop(uint8_t run_stop)
{
	if (run_stop)
	{
		LCD_ShowString(gui_out_x, gui_out_y, (uint8_t *)"OUT: ON", LGRAY,BLACK, 12, 0);
		gui_board_draw(GREEN);
	}
	else
	{
		LCD_ShowString(gui_out_x, gui_out_y, (uint8_t *)"OUT:OFF", LGRAY,BLACK, 12, 0);
		gui_board_draw(LGRAY);
	}
}

// // CC/CV 显示 0:CC 1:CV
// void gui_set_cc_cv(uint8_t cc_cv, uint8_t type)
// {
// 	if (type == 0) // 显示完整数据
// 	{
// 		if (cc_cv)
// 		{
// 			//LCD_ShowString(56, 5, "CV", GREEN, BLACK, 16, 0);
// 		}
// 		else
// 		{
// 			//LCD_ShowString(56, 5, "CC", YELLOW, BLACK, 16, 0);
// 		}
// 	}
// 	else
// 	{
// 		//LCD_ShowString(56, 5, "  ", YELLOW, BLACK, 16, 0);
// 	}
// }

// 功率 温度显示,功率 00.00w 温度 00C
void gui_power_temp(uint16_t power, int8_t temp)
{
	uint8_t str[20];
	sprintf(&str, "P: %2d.%02dW   T: %2dC", power/1000, power%1000/10, temp);
	//LCD_ShowString(12, 114, str, WHITE, BLACK, 12, 0);
	LCD_ShowString(gui_p_t_x, gui_p_t_y, (uint8_t *)str, LGRAY,BLACK, 12, 0);
};

// 输入电压显示
void gui_ref_in_v(uint16_t val)
{
	uint8_t str[5];
	sprintf(&str, "VIN:%2dV", val/1000);
	//LCD_ShowString(72, 5, str, WHITE, BLACK, 16, 0);
	LCD_ShowString(gui_vin_x, gui_vin_y, (uint8_t *)str, LGRAY,BLACK, 12, 0);
}

// type: 0:显示完整数据 1:设置整数位 2:设置小数位
void gui_set_v(uint16_t val, uint8_t type)
{
	uint8_t str[14];
	// 整数部分
	uint8_t num_h = val / 1000;

	// 小数部分
	uint16_t num_l = val % 1000;
	num_l /= 10;

	if (type == 0) // 显示完整数据
	{
		sprintf(&str, "V: %2d.%02d", num_h, num_l);
	}
	else if (type == 1) // 设置整数位
	{
		sprintf(&str, "V:   .%02d", num_l);
	}
	else if (type == 2) // 设置小数位
	{
		sprintf(&str, "V: %2d.  ", num_h);
	}
	//LCD_ShowString(12, 20, str, WHITE, BLACK, 16, 0);
	LCD_ShowString(gui_set_v_x, gui_set_v_y, (uint8_t *)str, LGRAY,BLACK, 12, 0);
}


// type 0:显示完整数据 1:设置整数位 2:设置小数位
void gui_set_i(uint16_t val, uint8_t type)
{
	uint8_t str[14];
	// 整数部分
	uint8_t num_h = val / 1000;

	// 小数部分
	uint16_t num_l = val % 1000;
	num_l /= 10;

	if (type == 0) // 显示完整数据
	{
		sprintf(&str, "I: %2d.%02d", num_h, num_l);
	}
	else if (type == 1) // 设置整数位
	{
		sprintf(&str, "I:   .%02d", num_l);
	}
	else if (type == 2) // 设置小数位
	{
		sprintf(&str, "I: %2d.  ", num_h);
	}
	//LCD_ShowString(12, 36, str, WHITE, BLACK, 16, 0);
	LCD_ShowString(gui_set_i_x, gui_set_i_y, (uint8_t *)str, LGRAY,BLACK, 12, 0);
}

#define GUI_REF_V_Y 52
void gui_ref_v(uint16_t val)
{
	uint8_t str[5];
	// 整数部分
	uint8_t num_h = val / 1000;

	// 小数部分
	uint16_t num_l = val % 1000;
	num_l /= 10;

	sprintf(&str, "%2d.%02d V", num_h, num_l);

	//LCD_ShowString(12, GUI_REF_V_Y, str, WHITE, BLACK, 32, 0);

	//LCD_ShowChar(100, GUI_REF_V_Y + 16, 'V', WHITE, BLACK, 16, 0);

	LCD_ShowString(gui_pwr_v_x, gui_pwr_v_y, (uint8_t *)str, LGRAY,BLACK, 32, 0);
}

#define GUI_REF_I_Y 82
void gui_ref_i(uint16_t val)
{
	uint8_t str[5];
	// 整数部分
	uint8_t num_h = val / 1000;


	// 小数部分
	uint16_t num_l = val % 1000;
	num_l /= 10;

	sprintf(&str, "%2d.%02d A", num_h, num_l);

	//LCD_ShowString(12, GUI_REF_I_Y, str, WHITE, BLACK, 32, 0);

	//LCD_ShowChar(100, GUI_REF_I_Y + 16, 'A', WHITE, BLACK, 16, 0);
	 LCD_ShowString(gui_pwr_i_x, gui_pwr_i_y, (uint8_t *)str, LGRAY,BLACK, 32, 0);
}
static uint8_t gui_status = GUI_S_INIT;
void gui_set_status(uint8_t status)
{
	gui_status = status;
}

void gui_loop()
{
	static uint32_t last_time = 0;
	static uint8_t dis_flag = 0;
	static uint32_t delay_cnt = 0;
	// 10ms 进入一次
	// if(get_sys_tick_diff(last_time) < 20)
	// {
	// 	return;
	// }
	last_time = get_sys_tick_count();
	//printf("gui_loop\r\n");

	delay_cnt++;
	gui_ref_v(power_data.ina_read_voltage);
	gui_ref_i(power_data.ina_read_current);
	gui_ref_in_v(power_data.input_voltage);
	gui_set_err(power_data.alarm);
	//
	uint16_t power_temp = power_data.ina_read_voltage * power_data.ina_read_current / 1000;
	gui_power_temp(power_temp, power_data.ntc_temp);

	if (power_data.gui_mode == 0) // 初始化显示
	{
		gui_set_run_stop(power_data.switch_status);
		
		gui_ref_in_v(power_data.input_voltage);
		gui_set_err(power_data.alarm);

		gui_set_v(power_data.set_voltage, 0);
		gui_set_i(power_data.set_current, 0);

	
		power_data.gui_mode = 1;
	}
	else if (power_data.gui_mode == 1) // 显示模式
	{
		


	}
	else // 参数设置模式 , 闪烁模式
	{
	
		switch (power_data.gui_set_mode_id)
		{
		case 0:
		case 1:
		case 2:
			if(delay_cnt<20)
			{
				break;
			}
			delay_cnt=0;

			gui_set_v(power_data.set_voltage, dis_flag);			
			if (dis_flag == 0)
			{
				dis_flag = 1;
			}
			else
			{
				dis_flag = 0;
			}
			break;
		case 3:
			gui_set_v(power_data.set_voltage, 0);
			dis_flag = 0;
			power_data.gui_set_mode_id = 4;
			break;

		case 4:
			if(delay_cnt<20)
			{
				break;
			}
			delay_cnt=0;

			gui_set_v(power_data.set_voltage, dis_flag);
			
			if (dis_flag == 0)
			{
				dis_flag = 2;
			}
			else
			{
				dis_flag = 0;
			}
			break;

		case 5:
			gui_set_v(power_data.set_voltage, 0);
			dis_flag = 0;
			power_data.gui_set_mode_id = 6;
			break;
		case 6:
			if(delay_cnt<20)
			{
				break;
			}
			delay_cnt=0;

			gui_set_i(power_data.set_current, dis_flag);
			
			if (dis_flag == 0)
			{
				dis_flag = 1;
			}
			else
			{
				dis_flag = 0;
			}
			break;
		case 7:
			gui_set_i(power_data.set_current, 0);
			dis_flag = 0;
			power_data.gui_set_mode_id = 8;
		case 8:
			if(delay_cnt<20)
			{
				break;
			}
			delay_cnt=0;

			gui_set_i(power_data.set_current, dis_flag);
			
			if (dis_flag == 0)
			{
				dis_flag = 2;
			}
			else
			{
				dis_flag = 0;
			}
		break;
		case 9:
			gui_set_i(power_data.set_current, 0);
			dis_flag = 0;
			power_data.gui_set_mode_id = 0;
		break;

		default:
			break;
		}

		//printf("set voltage:%d set current:%d\r\n", power_data.set_voltage, power_data.set_current);
	}
}