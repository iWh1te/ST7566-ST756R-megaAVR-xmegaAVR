/*
 * ST7567_lib_v1.h
 *
 * Created: 19.04.2016 10:58:19
 *  Author: ������������ ������ �������������
 */ 


#ifndef ST7567_LIB_V1_H_
#define ST7567_LIB_V1_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

	//����� �������� �������
#define ST7567
//#define ST7565R 

	//����� ����������������
#define XMEGA
//#define MEGA
#ifdef MEGA

	#define PIN0_bm 0x01
	#define PIN0_bp 0
	#define PIN1_bm 0x02
	#define PIN1_bp 1
	#define PIN2_bm 0x04
	#define PIN2_bp 2
	#define PIN3_bm 0x08
	#define PIN3_bp 3
	#define PIN4_bm 0x10
	#define PIN4_bp 4
	#define PIN5_bm 0x20
	#define PIN5_bp 5
	#define PIN6_bm 0x40
	#define PIN6_bp 6
	#define PIN7_bm 0x80
	#define PIN7_bp 7

	#define GLCD_SDA_DDR	DDRB
	#define GLCD_SDA_PORT	PORTB
	#define GLCD_SDA_PIN	PIN5_bm

	#define GLCD_SCLK_DDR	DDRB
	#define GLCD_SCLK_PORT	PORTB
	#define GLCD_SCLK_PIN	PIN7_bm

	#define GLCD_A0_DDR		DDRB
	#define GLCD_A0_PORT	PORTB
	#define GLCD_A0_PIN		PIN6_bm

	#define GLCD_RES_DDR	DDRB
	#define GLCD_RES_PORT	PORTB
	#define GLCD_RES_PIN	PIN0_bm

	#define GLCD_CS_DDR		DDRB
	#define GLCD_CS_PORT	PORTB
	#define GLCD_CS_PIN		PIN4_bm

	#define GLCD_CS_LOW()			GLCD_CS_PORT &= ~GLCD_CS_PIN
	#define GLCD_CS_HIGH()			GLCD_CS_PORT |= GLCD_CS_PIN

	#define GLCD_SCLK_LOW()			GLCD_SCLK_PORT &= ~GLCD_SCLK_PIN
	#define GLCD_SCLK_HIGH()		GLCD_SCLK_PORT |= GLCD_SCLK_PIN

	#define GLCD_SDA_HIGH()			GLCD_SDA_PORT &= ~GLCD_SDA_PIN;
	#define GLCD_SDA_LOW()			GLCD_SDA_PORT.OUTCLR |= GLCD_SDA_PIN;

	#define GLCD_WRITE_COMMAND()	GLCD_A0_PORT &= ~GLCD_A0_PIN
	#define GLCD_WRITE_DATA()		GLCD_A0_PORT |= GLCD_A0_PIN
#endif

#ifdef XMEGA
	#define GLCD_SDA_DDR	GLCD_SDA_PORT.DIR
	#define GLCD_SDA_PORT	PORTE
	#define GLCD_SDA_PIN	PIN5_bm

	#define GLCD_SCLK_DDR	GLCD_SCLK_PORT.DIR
	#define GLCD_SCLK_PORT	PORTE
	#define GLCD_SCLK_PIN	PIN7_bm

	#define GLCD_A0_DDR		GLCD_A0_PORT.DIR
	#define GLCD_A0_PORT	PORTE
	#define GLCD_A0_PIN		PIN6_bm

	#define GLCD_RES_DDR	GLCD_RES_PORT.DIR
	#define GLCD_RES_PORT	PORTE
	#define GLCD_RES_PIN	PIN0_bm

	#define GLCD_CS_DDR		GLCD_CS_PORT.DIR
	#define GLCD_CS_PORT	PORTE
	#define GLCD_CS_PIN		PIN4_bm

	#define GLCD_CS_LOW()			GLCD_CS_PORT.OUTCLR = GLCD_CS_PIN
	#define GLCD_CS_HIGH()			GLCD_CS_PORT.OUTSET = GLCD_CS_PIN

	#define GLCD_SCLK_LOW()			GLCD_SCLK_PORT.OUTCLR = GLCD_SCLK_PIN
	#define GLCD_SCLK_HIGH()		GLCD_SCLK_PORT.OUTSET = GLCD_SCLK_PIN

	#define GLCD_SDA_HIGH()			GLCD_SDA_PORT.OUTSET = GLCD_SDA_PIN;
	#define GLCD_SDA_LOW()			GLCD_SDA_PORT.OUTCLR = GLCD_SDA_PIN;

	#define GLCD_WRITE_COMMAND()	GLCD_A0_PORT.OUTCLR = GLCD_A0_PIN
	#define GLCD_WRITE_DATA()		GLCD_A0_PORT.OUTSET = GLCD_A0_PIN
#endif

//������� ��� ���������� ������
#define GLCD_SCLK_AS_OUTPUT()	GLCD_SCLK_DDR |= GLCD_SCLK_PIN
#define GLCD_CS_AS_OUTPUT()		GLCD_CS_DDR |= GLCD_CS_PIN
#define GLCD_A0_AS_OUTPUT()		GLCD_A0_DDR |= GLCD_A0_PIN
#define GLCD_SDA_AS_OUTPUT()	GLCD_SDA_DDR |= GLCD_SDA_PIN
	
//������� �����������
#define DISPLAY_ON_OFF_ADR			0xAE
#define DISPLAY_ON					0x01
#define DISPLAY_OFF					0x00

#define SET_START_LINE_ADR			0x40
#define SET_PAGE_ADR				0xB0
#define SET_COLUMN_ADR_MSB			0x10
#define SET_COLUMN_ADR_LSB			0x00

#define SEG_DIRECTION_ADR			0xA0
#define SEG_DIRECTION_MX_NORMAL		0x00
#define SEG_DIRECTION_MX_REVERS		0x01

#define INVERSE_DISPLAY_ADR			0xA6
#define INVERSE_DISPLAY_NORMAL		0x00
#define INVERSE_DISPLAY_INVERSE		0x01

#define ALL_PIXEL_ON_ADR			0xA4
#define ALL_PIXEL_ON				0x01
#define ALL_PIXEL_OFF				0x00

#define BIAS_SELECT_ADR				0xA2
#define BIAS_SELECT_1_9				0x00
#define BIAS_SELECT_1_7				0x01

#define READ_MODIFY_WRITE_ADR		0xE0	//����� ������ ������-���������-������
#define EXIT_READ_MODIFY_WRITE_ADR	0xEE	//����� �� ������ ������-���������-������
#define RESET_ADR					0xE2	//���������� �����

#define COM_DIRECTION_ADR			0xC0	
#define COM_DIRECTION_MY_NORMAL		(0<<3)
#define COM_DIRECTION_MY_REVERSE	(1<<3)

#define POWER_CONTROL_ADR			0x28
#define POWER_CONTROL_VB_ON			(1<<2)
#define POWER_CONTROL_VB_OFF		(0<<2)
#define POWER_CONTROL_VR_ON			(1<<1)
#define POWER_CONTROL_VR_OFF		(0<<1)
#define POWER_CONTROL_VF_ON			(1<<0)
#define POWER_CONTROL_VF_OFF		(0<<0)

#define REGULATION_RATIO_ADR		0x20
#define REGULATION_RATIO_3_0		0x00
#define REGULATION_RATIO_3_5		0x01
#define REGULATION_RATIO_4_0		0x02
#define REGULATION_RATIO_4_5		0x03
#define REGULATION_RATIO_5_0		0x04
#define REGULATION_RATIO_5_5		0x05
#define REGULATION_RATIO_6_0		0x06
#define REGULATION_RATIO_6_5		0x07

#define ELECTRONIC_VOL_MODE_SET_ADR		0x81

#define STATIC_INDICATOR_ON_OFF_ADR		0xAC
#define STATIC_INDICATOR_ON				0<<1
#define STATIC_INDICATOR_OFF			0<<1

#define STATIC_INDICATOR_REG_SET_ADR	0x00
#define STATIC_INDICATOR_REG_SET_ON		(1<<0)
#define STATIC_INDICATOR_REG_SET_OFF	(0<<0)

#define PAGE_BLINK_ADR					0xD5
#define PAGE_BLINK_NO_BLINK				0x00

#define DRIVING_MODE_SET
//�������� � 21 �� 25 �������

#define RESOLUTION_X		129	//���������� �� �
#define RESOLUTION_Y		64 // ���������� �� Y
#define MAX_PAGE			RESOLUTION_Y/8 //������������ ���������� �������

// ������ ��� ��������� ������� � ������� X Y
#define GLCD_GoTo(x, y)		Px = x;\
							Py = y;

uint8_t Px, Py; //���������� X Y

uint8_t Screen[RESOLUTION_X][RESOLUTION_Y/8]; // �.�. ��������� 4-Wire �� ����� ������ ������, ������ � RAM ������ ����� ������

// ������� �������
enum fontTable
{
	standart,
	TAHOMA14X14,
	TAHOMA18X18,
	TAHOMA28X28,
	COMIC_SANS_30_30,
	COMIC_SANS_19x22	
};


/************************************************************************/
/*					��������� �������                                   */
/************************************************************************/

/**
	*\ ������� ������������� ����������
*/
void GLCD_Init(void);
/**
	?\ ������� �������� ������� ����������
	?\ ����������� �������� uin8_t byte  - ���� ������
*/
void WriteByte(uint8_t byte);

/**
	?\ ������� �������� ������� ����������
	?\ ����������� �������� uin8_t commandByte  - �������
*/
void GLCD_SendCommand(uint8_t commandByte);

/**
	?\ ������� �������� ������� ����������
	?\ ����������� �������� uin8_t dataByte  - ������	
*/
void GLCD_SendData(uint8_t dataByte);

/**
	?\ ������� �������� �������
	?\ ����������� �������� bool invDisplay  - true/false	
*/
void GLCD_InverseDisplay(bool invDisplay);

/**
	?\ ������� ��������� ������������� �������
	?\ ����������� �������� uint8_t contrast - �������� ���������
	?\ �������� �� 0 �� 63
*/
void GLCD_SetContrast(uint8_t contrast);

/**
	?\ ������� ��������� ������������� �������
	?\ ����������� �������� uint8_t contrast - �������� ���������
	?\ �������� �� 0 �� 63
*/
void GLCD_PowerDown(bool set);
/**
	?\ ������� ���������� �������� ������� �������� �
*/
uint8_t GLCD_GetColum();

/**
	?\ ������� ���������� �������� ������� �������� Y
*/
uint8_t GLCD_GetLine();

/**
	?\ ������� ���������� ������ ��� ������ �� ��������
*/
void GLCD_Update(void);
/**
	?\ ������� ���������� ������� ������
	?\ uint8_t Xi - ��������� �������� � (1 - 127)
	?\ uint8_t Xj - �������� ���������� X
	?\ uint8_t Yi - ��������� ���������� Y 
	?\ uint8_t Yj - �������� ���������� Y
*/
void GLCD_UpdateAlign(uint8_t Xi, uint8_t Xj, uint8_t Yi, uint8_t Yj);

//����������� �����

/**
	?\ ������� ��������� ������� � �������� ����������
	?\ ������������ ��������:
				uint8_t x - ���������� �� �
				uint8_t y - ���������� �� Y
*/
void GLCD_SetPixel(uint8_t x, uint8_t y);

/**
	?\ ������� �������� ������� � �������� ����������
	?\ ������������ ��������:
				uint8_t x - ���������� �� �
				uint8_t y - ���������� �� Y
*/
void GLCD_ClearPixel(uint8_t x, uint8_t y);

/**
	?\ ����� ������� � ������� �������
	?\ ������������ ��������:
				uint8_t charachter - ��� �������
				uint8_t fontType - ��� ������
*/
uint8_t GLCD_SearchChar(uint8_t charachter, uint8_t fontType);

/**
	?\ ����� ������� �� �����
	?\ ������������ ��������:
				uint8_t ch - ��� �������
				uint8_t fontType - ��� ������
				bool inversion - ������� ��������
*/
void GLCD_DrawChar(uint8_t ch, uint8_t fontType, bool inversion);

/**
	?\ ����� ������ �� �����
	?\ ������������ ��������:
				const char *s - ��������� �� ������
				uint8_t fontType - ��� ������
				bool inversion - ������� ��������
*/
void GLCD_DrawString(const char *s, uint8_t fontType, bool inversion);
/**
	?\ ����� ������ � ��������� ������ �� �����
	?\ ������������ ��������:
	float value - �������� � ��������� ������
	int accuratcy - ��������
	uint8_t fontType - ��� ������
	bool inversion - ������� ��������
*/
void GLCD_DrawStringFloat(float value, int accuratcy, uint8_t fontType, bool inversion);
/**
	?\������� �������
*/
void GLCD_Clear(void);

/**
	?\������� ������ ������
*/
void GLCD_ClearBuffer(void);

/**
	?\ ������� �������� �� ������
*/
void GLCD_ClearAlign(uint8_t Xi, uint8_t Xj, uint8_t Yi, uint8_t Yj);

void GLCD_SetQuadrat(uint8_t Xi, uint8_t Xj, uint8_t Yi, uint8_t Yj);
void GLCD_SetVerticalLine(uint8_t X, uint8_t Y, uint8_t Yi);
void GLCD_SetHorisontalLine(uint8_t X, uint8_t Xi, uint8_t Y);


/**
	����� �������� �� �����
	?\����� ������� �������� ���������� ������ �������� GLCD_GoTo(x, y)
*/
void GLCD_DrawPicture(const char *picture, uint8_t width, uint8_t height);


#endif /* ST7567_LIB_V1_H_ */