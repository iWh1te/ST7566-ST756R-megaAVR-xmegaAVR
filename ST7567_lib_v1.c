/*
 * ST7567_lib_v1.c
 *
 * Created: 19.04.2016 10:58:33
 *  Author: Белолипецкий Андрей Александрович
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "ST7567_lib_v1.h"
#include "FloatToString.h"

//Шрифты
#include "font_5_7.h"
#include "ComicSans.h"
#include "Tahoma14x14.h"
#include "TAHOMA18x18.h"
#include "TAHOMA28X28.h"

#ifdef ST7567 
	#define OFFSET_CLR			0x04 //смещение для очистки экрана
	#define OFFSET_PIXEL		0x04 //смещение для вывода пикселя
#endif

#ifdef ST756R
	#define OFFSET_CLR			0x00 //смещение для очистки экрана
	#define OFFSET_PIXEL		0x00 //смещение для вывода пикселя
#endif

#define CHECK_PX(x)				if(x > RESOLUTION_X) {Px = 0;}

uint8_t minX = 0, minY = 0; //минимальная координа для обновления экрана при работе со строками
uint8_t maxX = 0, maxY = 0; //максимальная координата для обновления экрана при работе со строками

//Работа с дисплеем

void WriteByte(uint8_t byte)
{
	GLCD_SCLK_HIGH();
	for (uint8_t bit = 0; bit < 8; bit++)
	{	
		if ((byte << bit) & 0x80)
		{
			GLCD_SDA_HIGH();
		}
		else
		{
			GLCD_SDA_LOW();
		}		
		GLCD_SCLK_LOW();
		asm("nop");
		GLCD_SCLK_HIGH();
		GLCD_SDA_LOW();
	}
}

void GLCD_SendCommand(uint8_t commandByte)
{
	GLCD_CS_LOW();
	GLCD_WRITE_COMMAND();
	
	WriteByte(commandByte);
	GLCD_CS_HIGH();
	GLCD_WRITE_DATA();
}

void GLCD_SendData(uint8_t dataByte)
{
	GLCD_CS_LOW();
	GLCD_WRITE_DATA();
	WriteByte(dataByte);
	GLCD_CS_HIGH();
	GLCD_WRITE_DATA();
}

void GLCD_Init(void)
{
	GLCD_SDA_AS_OUTPUT();
	GLCD_CS_AS_OUTPUT();
	GLCD_A0_AS_OUTPUT();
	GLCD_SCLK_AS_OUTPUT();
		
	GLCD_SCLK_HIGH();
	GLCD_CS_HIGH();
	GLCD_WRITE_DATA();
	GLCD_SDA_HIGH();

	GLCD_SendCommand(DISPLAY_ON_OFF_ADR | DISPLAY_OFF);
	GLCD_SendCommand(RESET_ADR);
	GLCD_SendCommand(BIAS_SELECT_ADR | BIAS_SELECT_1_9);
	GLCD_SendCommand(SEG_DIRECTION_ADR | SEG_DIRECTION_MX_NORMAL);
	GLCD_SendCommand(COM_DIRECTION_ADR | COM_DIRECTION_MY_REVERSE);
	GLCD_SendCommand(REGULATION_RATIO_ADR | REGULATION_RATIO_5_0); //
//	GLCD_SendCommand(48); //Разобраться
	GLCD_SendCommand(POWER_CONTROL_ADR | POWER_CONTROL_VB_ON | POWER_CONTROL_VF_ON | POWER_CONTROL_VR_ON); //
	GLCD_SendCommand(INVERSE_DISPLAY_ADR | INVERSE_DISPLAY_NORMAL);
	GLCD_SendCommand(ALL_PIXEL_ON_ADR + ALL_PIXEL_OFF);
	GLCD_SendCommand(SET_START_LINE_ADR + 0x00);
	GLCD_SendCommand(SET_PAGE_ADR + 0x00);
	GLCD_SendCommand(SET_COLUMN_ADR_MSB + (OFFSET_PIXEL >> 4)); //установка адреса по х
	GLCD_SendCommand(SET_COLUMN_ADR_LSB + (OFFSET_PIXEL & 0x0F));
	GLCD_Clear();
	GLCD_SendCommand(DISPLAY_ON_OFF_ADR + DISPLAY_ON); //display ON
}

void GLCD_InverseDisplay(bool invDisplay)
{
	if (invDisplay)
	{
		GLCD_SendCommand(INVERSE_DISPLAY_ADR | INVERSE_DISPLAY_INVERSE);
	}
	else
	{
		GLCD_SendCommand(INVERSE_DISPLAY_ADR | INVERSE_DISPLAY_NORMAL);
	}
}

void GLCD_SetContrast(uint8_t contrast)
{
	GLCD_CS_LOW();
	GLCD_WRITE_COMMAND();
	
	WriteByte(ELECTRONIC_VOL_MODE_SET_ADR);
	WriteByte(contrast & 0x3F);
	GLCD_CS_HIGH();
	GLCD_WRITE_DATA();
}

void GLCD_PowerDown(bool set)
{
	if (set)
	{
		GLCD_SendCommand(DISPLAY_ON_OFF_ADR + DISPLAY_OFF);
		GLCD_SendCommand(ALL_PIXEL_ON_ADR + ALL_PIXEL_ON);
	}
	else
	{
		GLCD_SendCommand(ALL_PIXEL_ON_ADR + ALL_PIXEL_OFF);
		GLCD_SendCommand(DISPLAY_ON_OFF_ADR + DISPLAY_ON);
	}
}

void GLCD_Update(void)
{
	GLCD_SendCommand(READ_MODIFY_WRITE_ADR);
	
	for (uint8_t Yy = minY; Yy != MAX_PAGE; Yy++)
	{
		GLCD_SendCommand(SET_PAGE_ADR + Yy); //определяем страницу
		GLCD_SendCommand(SET_COLUMN_ADR_MSB + ((minX + OFFSET_PIXEL)>>4)); //установка адреса по х
		GLCD_SendCommand(SET_COLUMN_ADR_LSB + ((minX+OFFSET_PIXEL) & 0x0F));
		for (uint8_t Xx = minX; Xx != RESOLUTION_X; Xx++)
		{
			GLCD_SendData(Screen[Xx][Yy]);
		}
	}
	GLCD_SendCommand(EXIT_READ_MODIFY_WRITE_ADR);
}

void GLCD_UpdateAlign(uint8_t Xi, uint8_t Xj, uint8_t Yi, uint8_t Yj)
{
	
	for (uint8_t Yy = Yi/8; Yy != (Yj/8)+1; Yy++)
	{
		GLCD_SendCommand(READ_MODIFY_WRITE_ADR);
		GLCD_SendCommand(SET_PAGE_ADR + Yy); //определяем страницу
		GLCD_SendCommand(SET_COLUMN_ADR_MSB + ((Xi + OFFSET_PIXEL)>>4)); //установка адреса по х
		GLCD_SendCommand(SET_COLUMN_ADR_LSB + ((Xj+OFFSET_PIXEL) & 0x0F));
		for (uint8_t Xx = Xi; Xx != Xj+1; Xx++)
		{
			GLCD_SendData(Screen[Xx][Yy]);
		}
		GLCD_SendCommand(EXIT_READ_MODIFY_WRITE_ADR);	
	}
	//GLCD_SendCommand(EXIT_READ_MODIFY_WRITE_ADR);	
}

void GLCD_Clear(void)
{
	GLCD_SendCommand(READ_MODIFY_WRITE_ADR);
	for (uint8_t y = 0; y != MAX_PAGE; y++)
	{	
		GLCD_SendCommand(SET_PAGE_ADR + y); //установка страницы
		GLCD_SendCommand(SET_COLUMN_ADR_MSB + ((0+ OFFSET_CLR)>>4)); //установка адреса по х
		GLCD_SendCommand(SET_COLUMN_ADR_LSB + ((0+OFFSET_CLR) & 0x0F)); //0x04 смещение
		for (uint8_t x = 0; x != RESOLUTION_X; x++)
		{
			Screen[x][y] = 0x00;
			GLCD_SendData(Screen[x][y]);
		}
	}
	GLCD_SendCommand(EXIT_READ_MODIFY_WRITE_ADR);
}

void GLCD_ClearBuffer()
{
	for (uint8_t y = 0; y != MAX_PAGE; y++)
	{
		for (uint8_t x = 0; x != RESOLUTION_X; x++)
		{
			Screen[x][y] = 0x00;
		}
	}	
}

void GLCD_ClearAlign(uint8_t Xi, uint8_t Xj, uint8_t Yi, uint8_t Yj)
{
	for (uint8_t yY = Yi; yY != Yj; yY++)
	{
		for (uint8_t xX = Xi; xX != Xj; xX++)
		{
			GLCD_ClearPixel(xX, yY);
		}
	}	
}

uint8_t GLCD_GetLine()
{
	return (Py);
}

uint8_t GLCD_GetColum()
{
	return (Px);
}

//Работа с графикой

void GLCD_SetPixel(uint8_t x, uint8_t y)
{
	CHECK_PX(x);
	Screen[x][y/8] |= (0x01 << (y%8));
}

void GLCD_ClearPixel(uint8_t x, uint8_t y)
{
	CHECK_PX(x);
	Screen[x][y/8] &= ( 0xff ^ (0x01 << ((y%8))));
}

void GLCD_SetHorisontalLine(uint8_t X, uint8_t Xi, uint8_t Y)
{
	for (uint8_t i = X; i < Xi+1; i++)
	{
		GLCD_SetPixel(i, Y);
	}
}

void GLCD_SetVerticalLine(uint8_t X, uint8_t Y, uint8_t Yi)
{
	for (uint8_t i = Y; i < Yi+1; i++)
	{
		GLCD_SetPixel(X, i);
	}
}

void GLCD_SetQuadrat(uint8_t Xi, uint8_t Xj, uint8_t Yi, uint8_t Yj)
{
	GLCD_SetHorisontalLine(Xi, Xj, Yi);
	GLCD_SetHorisontalLine(Xi, Xj, Yj);
	GLCD_SetVerticalLine(Xi, Yi, Yj);
	GLCD_SetVerticalLine(Xj, Yi, Yj);
}

//Работа со строками

uint8_t GLCD_SearchChar(uint8_t charachter, uint8_t fontType)
{
	if ( (fontType == standart))
	{
		if ((charachter >= 0x20) && (charachter <= 0x7E))
		{
			charachter -= 0x20;
		}
		else if ((charachter >= 0xC0) && (charachter <= 0xFF))
		{
			charachter -= 0x5F;
		}
	}
	else if (fontType == TAHOMA14X14)
	{
		switch(charachter)
		{
			case ' ': charachter = 0; break;
			case '%': charachter = 1; break;
			case ',': charachter = 2; break;
			case '-': charachter = 3; break;
			case '.': charachter = 4; break;
			case '/': charachter = 5; break;
			case '0': charachter = 6; break;
			case '1': charachter = 7; break;
			case '2': charachter = 8; break;
			case '3': charachter = 9; break;
			case '4': charachter = 10; break;
			case '5': charachter = 11; break;
			case '6': charachter = 13; break;
			case '7': charachter = 13; break;
			case '8': charachter = 14; break;
			case '9': charachter = 15; break;
			case 'C': charachter = 16; break;
			case 'H': charachter = 17; break;
			case 'N': charachter = 18; break;
			case 'O': charachter = 19; break;
			case 'S': charachter = 20; break;
			case 'l': charachter = 21; break;
			case 'm': charachter = 22; break;
			case 'p': charachter = 23; break;
			case 'г': charachter = 24; break;
			case 'м': charachter = 25; break;
			case ':': charachter = 26; break;
			case 'L': charachter = 27; break;
		}
	}
	else if (fontType == TAHOMA18X18)
	{
		switch (charachter)
		{
			case ' ': charachter = 0; break;
			case  '%': charachter = 1; break;
			case ',': charachter = 2; break;
			case '/': charachter = 3; break;
			case '0': charachter = 4; break;
			case '1': charachter = 5; break;
			case '2': charachter = 6; break;
			case '3': charachter = 7; break;
			case '4': charachter = 8; break;
			case '5': charachter = 9; break;
			case '6': charachter = 10; break;
			case '7': charachter = 11; break;
			case '8': charachter = 12; break;
			case '9': charachter = 13; break;
			case 'C': charachter = 14; break;
			case 'H': charachter = 15; break;
			case 'N': charachter = 16; break;
			case 'O': charachter = 17; break;
			case 'S': charachter = 18; break;
			case 'l': charachter = 19; break;
			case 'm': charachter = 20; break;
			case 'p': charachter = 21; break;
			case 'м': charachter = 22; break;
			case 'г': charachter = 23; break;
			case 'L': charachter = 24; break;
			case 'А': charachter = 25; break;
			case 'Д': charachter = 26; break;
			case 'Е': charachter = 27; break;
			case 'Н': charachter = 28; break;
			case 'Т': charachter = 29; break;
		}
	}
	else if ((fontType == TAHOMA28X28))
	{
		switch(charachter)
		{
			case '.': charachter = 0; break;
			case '0': charachter = 1; break;
			case '1': charachter = 2; break;
			case '2': charachter = 3; break;
			case '3': charachter = 4; break;
			case '4': charachter = 5; break;
			case '5': charachter = 6; break;
			case '6': charachter = 7; break;
			case '7': charachter = 8; break;
			case '8': charachter = 9; break;
			case '9': charachter = 10; break;
			case ' ': charachter = 11; break;
			case '-': charachter = 12; break;
		}
	}
	else if (fontType == COMIC_SANS_30_30)
	{
		switch(charachter)
		{
			case 'А': charachter = 0; break;
			case 'Л': charachter = 1; break;
			case 'Н': charachter = 2; break;
			case 'Э': charachter = 3; break;
		}
	}
	else if (fontType == COMIC_SANS_19x22)
	{
		switch (charachter)
		{
			case '/': charachter = 0; break;
			case '2': charachter = 1; break;
			case '3': charachter = 2; break;
			case 'C': charachter = 3; break;
			case 'H': charachter = 4; break;
			case 'L': charachter = 5; break;
			case 'N': charachter = 6; break;
			case 'O': charachter = 7; break;
			case 'S': charachter = 8; break;
		}
	}
	return charachter;
}

void GLCD_DrawChar(uint8_t ch, uint8_t fontType, bool inversion)
{
	uint16_t x = 0;
	uint16_t y =0;
	ch = GLCD_SearchChar(ch, fontType); //ищем в массивах нужный символ//поиск нужного символа		
	switch(fontType)
	{
		case standart:
		{
			if (inversion)
			{
				if (Py % 8 == 0)
				{
					Screen[Px-1][(Py/8+1)] |= 0x01; //Подрисовка затемнения вначале строки
					Screen[Px-1][(Py/8)] |= 0xff; //Подрисовка затемнения вначале строки
					Screen[Px+FONT_5_7_X][(Py/8)] |= 0xff; //Подрисовка затемнения в конце строки
					Screen[Px+FONT_5_7_X][(Py/8+1)] |= 0x01;//Подрисовка затемнения в конце строки
					do
					{																			//отрисовка символа
						Screen[Px+x][(Py/8)] |= 0xff;												  //сдвиг на один пиксель вниз
						Screen[Px+x][(Py/8)] &= ((~(pgm_read_byte(font5x7 + (FONT_5_7_X * ch) + x))) << 0x01) | 0x01;
						Screen[Px+x][(Py/8+1)] |= 0x01;
						x++;
						CHECK_PX(Px+x);
					} while (x < FONT_5_7_X);
				}
				else
				{
					Screen[Px-1][(Py/8)] |= 0xFF << (Py%8) | 0x01 << (Py - 8*(Py/8) -1);   //подрисовка затемнения
					Screen[Px-1][(Py/8+1)] |= 0xFF >> (8-(Py%8));	//вначале строки
					do
					{																						//позиция на линии
						Screen[Px+x][(Py/8)] = ((~(pgm_read_byte(font5x7 + ((FONT_5_7_X * ch) + x)) << (Py%8)) & (0xff << (Py%8))) | (0x01<< (Py - 8*(Py/8) -1)) ) | (Screen[Px+x][(Py/8)]);
						Screen[Px+x][(Py/8+1)] = ((~(pgm_read_byte(font5x7 + (FONT_5_7_X * ch) + x))  >> (8-(Py%8))) & (0xff >> (8-(Py%8)))) | Screen[Px+x][(Py/8+1)];
						x++;
						CHECK_PX(Px+x);
					} while (x < FONT_5_7_X);
					Screen[Px+FONT_5_7_X][(Py/8)] |= 0xff << (Py%8) >> 0x01;	//подрисовка затемнения
					Screen[Px+FONT_5_7_X][(Py/8+1)] |= 0xFF >> (8-(Py%8)); //вконце строки
				}
			}
			else
			{			
				do
				{
					if (Py % 8 == 0)
					{
						Screen[Px+x][(Py/8)] |= pgm_read_byte(font5x7 + (FONT_5_7_X * ch) + x);
					}
					else
					{
						Screen[Px+x][(Py/8)] |= pgm_read_byte(font5x7 + (FONT_5_7_X * ch) + x)  << (Py%8);  //
						Screen[Px+x][(Py/8+1)] |= pgm_read_byte(font5x7 + (FONT_5_7_X * ch) + x)  >> (8-(Py%8));
					}
					x++;
					CHECK_PX(Px+x);
				} while (x < FONT_5_7_X);
			}
			Px += FONT_5_7_X+1; //следующая позиция		
				
			break;
		}
		case TAHOMA14X14:
		{
			x = 1;
			uint8_t width = pgm_read_byte(Tahoma14x14 + ((TAHOMA14X14_X*2+1) * ch)); //следующая позиция
			do
			{
				if (Py % 8 == 0)
				{
					for (uint8_t i = 0; i < 2; i++)
					{
						Screen[Px+y][(Py/8 + i)] |= pgm_read_byte(Tahoma14x14 + ((TAHOMA14X14_X*2+1) * ch) + x + i);  //
					}
				}
				else //корректный вывод
				{
					Screen[Px+y][(Py/8)] |= pgm_read_byte(Tahoma14x14 + ((TAHOMA14X14_X*2+1) * ch) + x) << (Py%8);  //
					Screen[Px+y][(Py/8+1)] |= pgm_read_byte(Tahoma14x14 + ((TAHOMA14X14_X*2+1) * ch) + x) >> (8-(Py%8)) | pgm_read_byte(Tahoma14x14 + ((TAHOMA14X14_X*2+1) * ch) + x+1) << (Py%8);
					Screen[Px+y][(Py/8+2)] |= pgm_read_byte(Tahoma14x14 + ((TAHOMA14X14_X*2+1) * ch) + x+1) >> (8-(Py%8));
				}
				x = x + 2;
				y++;
				CHECK_PX(Px+y);
			}while (x < TAHOMA14X14_X*2+1);		//придумать как использовать ширину	символа динамически
			Px += width+1; //следующая позиция			
			break;
		}
		case TAHOMA18X18:
		{
			x = 1;
			uint8_t width = pgm_read_byte(Tahoma18x18 + ((TAHOMA18X18_X*3+1) * ch)); //следующая позиция
			do
			{
				if (Py % 8 == 0)
				{
					for (uint8_t i = 0; i < 3; i++)
					{
						Screen[Px+y][(Py/8 + i)] |= pgm_read_byte(Tahoma18x18 + ((TAHOMA18X18_X*3+1) * ch) + x + i);  //
					}
				}
				else
				{
					Screen[Px+y][(Py/8)] |= pgm_read_byte(Tahoma18x18 + ((TAHOMA18X18_X*3+1) * ch) + x) << (Py%8);  //
					for (uint8_t i = 1; i < 3; i++)
					{
						Screen[Px+y][(Py/8+i)] |= pgm_read_byte(Tahoma18x18 + ((TAHOMA18X18_X*3+1) * ch) + x + i-1) >> (8-(Py%8)) | pgm_read_byte(Tahoma18x18 + ((TAHOMA18X18_X*3+1) * ch) + x+i) << (Py%8);
					}
					Screen[Px+y][(Py/8+3)] |= pgm_read_byte(Tahoma18x18 + ((TAHOMA18X18_X*3+1) * ch) + x+2) >> (8-(Py%8));
				}
				x = x + 3;		
				y++;
				CHECK_PX(Px+y);
			}while (x < TAHOMA18X18_X*3);
			Px += width+1;
			break;
		}
		case TAHOMA28X28:
		{
			x = 1;
			uint8_t width = pgm_read_byte(Tahoma28x28 + ((TAHOMA28X28_X*4+1) * ch)); //следующая позиция
			do
			{
				if (Py % 8 == 0)
				{
					for (uint8_t i = 0; i < 4; i++)
					{
						Screen[Px+y][(Py/8 + i)] |= pgm_read_byte(Tahoma28x28 + ((TAHOMA28X28_X*4+1) * ch) + x + i);  //
					}
				}
				else
				{
					Screen[Px+y][(Py/8)] |= pgm_read_byte(Tahoma28x28 + ((TAHOMA28X28_X*4+1) * ch) + x) << (Py%8);  //
					for (uint8_t i = 1; i < 4; i++)
					{
						Screen[Px+y][(Py/8+i)] |= pgm_read_byte(Tahoma28x28 + ((TAHOMA28X28_X*4+1) * ch) + x + i-1) >> (8-(Py%8)) | pgm_read_byte(Tahoma28x28 + ((TAHOMA28X28_X*4+1) * ch) + x+i) << (Py%8);
					}
				}
				x = x + 4;
				y++;
				CHECK_PX(Px+y);
			}while (x < TAHOMA28X28_X*4);
			Px += width+1;
			break;
		}
		case COMIC_SANS_19x22:
		{
			x = 1;
			uint8_t width = pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch)); //следующая позиция
			do
			{
				if (Py % 8 == 0)
				{
					for (uint8_t i = 0; i < 3; i++)
					{
						Screen[Px+y][(Py/8 + i)] |= pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x + i);  //
					}
				}
				else
				{
					Screen[Px+y][(Py/8)] |= pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x) << (Py%8);  //
					for (uint8_t i = 1; i < 3; i++)
					{
						Screen[Px+y][(Py/8+i)] |= pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x + i-1) >> (8-(Py%8)) | pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x+i) << (Py%8);
					}
				}
				x = x + 3;
				y++;
				CHECK_PX(Px+y);
			}while (x < COMIC_SANS_19x22_X*3);
			Px += width;
			break;
		}
		case COMIC_SANS_30_30:
		{
			x = 1;
			uint8_t width = pgm_read_byte(Comic_Sans_MS30x30 + ((COMIC_SANS_30_30_X*4+1) * ch)); //следующая позиция
			do
			{
				if (Py % 8 == 0)
				{
					for (uint8_t i = 0; i < 4; i++)
					{
						Screen[Px+y][(Py/8 + i)] |= pgm_read_byte(Comic_Sans_MS30x30 + ((COMIC_SANS_30_30_X*4+1) * ch) + x + i);  //
					}
				}
				else
				{
					Screen[Px+y][(Py/8)] |= pgm_read_byte(Comic_Sans_MS30x30 + ((COMIC_SANS_30_30_X*4+1) * ch) + x) << (Py%8);  //
					for (uint8_t i = 1; i < 4; i++)
					{
						Screen[Px+y][(Py/8+i)] |= pgm_read_byte(Comic_Sans_MS30x30 + ((COMIC_SANS_30_30_X*4+1) * ch) + x + i-1) >> (8-(Py%8)) | pgm_read_byte(Comic_Sans_MS30x30 + ((COMIC_SANS_30_30_X*4+1) * ch) + x+i) << (Py%8);
					}
				}
				x = x + 4;
				y++;
				CHECK_PX(Px+y);
			}while (x < COMIC_SANS_30_30_X*4);
			Px += width;
			break;
		}
		
	}
}

void GLCD_DrawString(const char *s, uint8_t fontType, bool inversion)
{
	while(*s)
	{
		GLCD_DrawChar(*s++, fontType, inversion);
	}
}

void GLCD_DrawStringFloat(float value, int accuratcy, uint8_t fontType, bool inversion)
{
	GLCD_DrawString((char*)gftoa(value, accuratcy),  fontType, inversion);
}

//работа с картинками

void GLCD_DrawPicture(const char *picture, uint8_t width, uint8_t height)
{
	//Расчитываем конечные координаты
	uint8_t Y_end = height/8;
	uint8_t X_end = width;
	
	for (uint8_t y = (Py/8); y < Y_end; y++)
	{
		GLCD_SendCommand(SET_PAGE_ADR + y); //определяем страницу
		for (uint8_t x = Px; x < Px+X_end; x++)
		{	
			Screen[x][y] |= (uint8_t)pgm_read_byte(picture);
			GLCD_SendCommand(SET_COLUMN_ADR_MSB + ((x + OFFSET_PIXEL)>>4)); //установка адреса по х
			GLCD_SendCommand(SET_COLUMN_ADR_LSB + ((x+OFFSET_PIXEL) & 0x0F));	
			GLCD_SendData(Screen[x][y]);	
			picture++;
		}
	}
}


/************************************************************************/
/*		Пример вывода больших шрифтов                                   */

/*
	uint8_t x = 1;
	uint8_t y = 0;
	uint8_t width = pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch)); //определяем ширину символа
	do
	{
		if (Py % 8 == 0)
		{
			for (uint8_t i = 0; i < 3; i++)
			{
				Screen[Px+y][(Py/8 + i)] |= pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x + i);  //
			}
		}
		else
		{
			Screen[Px+y][(Py/8)] |= pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x) << (Py%8);  //
			for (uint8_t i = 1; i < 3; i++)
			{
				Screen[Px+y][(Py/8+i)] |= pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x + i-1) >> (8-(Py%8)) | pgm_read_byte(Comic_Sans_MS19x22 + ((COMIC_SANS_19x22_X*3+1) * ch) + x+i) << (Py%8);
			}
		}
		x = x + 3;
		y++;
	}while (x < COMIC_SANS_19x22_X*3);
	Px += width;
*/

/************************************************************************/