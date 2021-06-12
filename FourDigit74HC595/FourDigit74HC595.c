/*

  ******************************************************************************
  * @file 			( фаил ):   FourDigit74HC595.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

/* Includes ----------------------------------------------------------*/
#include "FourDigit74HC595.h"

uint8_t LED_NUM[] = {
	0xC0, //0
	0xF9, //1
	0xA4, //2
	0xB0, //3
	0x99, //4
	0x92, //5
	0x82, //6
	0xF8, //7
	0x80, //8
	0x90, //9
	0xBF  //-
};
	
/*
	******************************************************************************
	* @brief	 ( описание ):  функция для отправки данных в микросхему
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
static void FourDigit74HC595_sendData(uint8_t data){
	
	for(int8_t i=0; i<8; i++){         
		  if( data & (0x80 >> i ) ){
			HAL_GPIO_WritePin (DIO_GPIO_Port, DIO_Pin, GPIO_PIN_SET);
		  }
		  else{
			HAL_GPIO_WritePin (DIO_GPIO_Port, DIO_Pin, GPIO_PIN_RESET);  
		  }
		  
		  HAL_GPIO_WritePin (SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET );
		  HAL_GPIO_WritePin (SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET );
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция формирует одно разрядное число ( 1 цифру )
								в указаном разряде ( можно совместно включить точку )
	* @param	( параметры ):	1- позиция ( разряд ) где отабразить значение от 1 до 4
								2- сама цифра от 0 до 9 ( если нужен знак минус то ставим 10 )
								3- точка ( 1 включить,  0 выключить )
	* @return  ( возвращает ):	

	******************************************************************************
*/
void FourDigit74HC595_sendOneDigit(uint8_t pos, uint8_t digit, uint8_t dot){
	
	// отправляем число от 0 до 9 или занак минус ( число 10 )
	if( dot ){
		FourDigit74HC595_sendData( LED_NUM[ digit ] & 0x7F );	// если нужна точка
	}
	else{
		FourDigit74HC595_sendData( LED_NUM[ digit ]  );			// если точка не нужна
	}
	
	// отправляем номер разряда от 1 до 4
	if( pos == 0x01 ){
		FourDigit74HC595_sendData( 0x01 );
	}
	else if( pos == 0x02 ){
		FourDigit74HC595_sendData( 0x02 );
	}
	else if( pos == 0x03 ){
		FourDigit74HC595_sendData( 0x04 );
	}
	else if( pos == 0x04 ){
		FourDigit74HC595_sendData( 0x08 );
	}
	
	HAL_GPIO_WritePin (RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_RESET );	// отключаем засчелку
	HAL_GPIO_WritePin (RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_SET );	// включаем засчелку
	
	HAL_Delay (1);
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция для отображения любого числа на дисплее
	* @param	( параметры ):	строка которую нужно отабразить :
								символы цифры от 0 до 9 
								знак минус ( может быть не один )
								знак точка ( может быть не один )
								Напримар: "-10.56" "0.56" "-2 -2.3" "1.2.3.4." "-23-" и т.д

	* @return  ( возвращает ):	

	******************************************************************************
*/
void FourDigit74HC595_sendNumber(char* num){
	
	uint8_t len = strlen( num );
	uint8_t dot = 0;
	
	while( len ){
		if( num[len -1 ] == '.' ){
			dot++;
		}
		len--;
	}
	
	if( strlen( num ) - dot > 4 ){
		FourDigit74HC595_sendOneDigit( 0x01, 10, 0 );
		FourDigit74HC595_sendOneDigit( 0x02, 10, 0 );
		FourDigit74HC595_sendOneDigit( 0x03, 10, 0 );
		FourDigit74HC595_sendOneDigit( 0x04, 10, 0 );
		
		len = 0;
	}
	else{
		len = strlen( num );
	}
	
	uint8_t position = 1;
	dot = 0;
	
	while( len ){
		
		if(num[len-1] == '-'){
			FourDigit74HC595_sendOneDigit( position, 10, dot );	
			dot = 0;
			position++;
		}
		else if(num[len-1] == '.'){
			dot = 1;
		}
		else if(num[len-1] == ' '){
			position++;
		}
		else{
			FourDigit74HC595_sendOneDigit( position, num[len-1]-48, dot );	
			dot = 0;
			position++;
		}
		
		len--;
		
		if( position > 4 ) { 
			break; 
		}
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция для отображения INT целочисленного числа на дисплее
	* @param	( параметры ):	число целочисленное :
								цифры от -999 до 9999 

	* @return  ( возвращает ):	

	******************************************************************************
*/
void FourDigit74HC595_sendNumberInt(int16_t num){
	
	uint8_t position = 1;
	
	if( num == 0 ){
		
		FourDigit74HC595_sendOneDigit( 1, 0, 0 );
		
	}
	else if( ((num < 0) && (num / 1000)) || ((num > 0) && (num / 10000)) ){
		
		FourDigit74HC595_sendOneDigit( 0x01, 10, 0 );
		FourDigit74HC595_sendOneDigit( 0x02, 10, 0 );
		FourDigit74HC595_sendOneDigit( 0x03, 10, 0 );
		FourDigit74HC595_sendOneDigit( 0x04, 10, 0 );
		
	}
	else{
		uint8_t minus = 0;
		if( num < 0 ){
			minus = 1;
			num = num * -1;
		}
		while( num ){

			FourDigit74HC595_sendOneDigit( position, num % 10, 0 );
			num = num / 10;
			position++;
			
			if( position > 4 ) { break; }
		}
		if( minus ){
			FourDigit74HC595_sendOneDigit( position, 10, 0 );
		}
	}
	
}
//----------------------------------------------------------------------------------



/************************ (C) COPYRIGHT GKP *****END OF FILE****/
