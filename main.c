/*
 * linetrace.c.c
 *
 * Created: 2022-05-17 오후 11:32:01
 * Author : Seon
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

#define left 0
#define right 1
#define on 1
#define off 0
#define nom 0
#define zr 1



float left_dut = 1;
float right_dut = 1;
int left_pwm = 0;
int right_pwm = 0;

int line_pos = left;


int adc;

int led = 0b00000000;
int sensdata = 0x00;
int afterLinecnt = 0;
//int beforeLinecnt = 0;
int senscnt = 0;
int crosscnt = 0;
int lineOvercnt = 0;
int fgoBreakcnt = off;
int LfindLine = off;
int RfindLine = off;
int beforesensdata = 0;

void fgo(int max_speed, int breakcnt);
void bmgo(int leftsSd, int rightSpd, int mdelay);
void turn(int max_speed, int dir, int turn_kind);


void Uart_Init(void);
void Uart_Trans(unsigned char data);
void Uart_Transnum(unsigned char* data, int size);

int main(void)
{	
	Uart_Init();
	DDRA = 0b11111111;
	DDRF = 0b00000000;
	ADCSRA = 0b10000111;

	DDRD = 0b00001000;
	ADCSRA = 0x87;

	DDRB = 0b11111111;
	DDRE = 0b00001111;
	TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (0<<CS02) | (0<<CS01) | (1<<CS00);
	PORTE = 0b00001010;
	ICR1 = 799;
	OCR1A = 160;
	OCR1B = 160;



	#define ms 250

	fgo(ms,4);
	fgo(ms,4);
	fgo(ms,4);

	turn(ms,right,nom);

	fgo(ms,4);
	
	turn(ms,left,nom);
	
	fgo(ms,4);
	
	
	/*///
	turn(ms,left,nom);

	
	
	fgo(ms,4);
	
	turn(ms,right,nom);
	
	
	
	fgo(ms,4);
	
	turn(ms,right,nom);
	
	fgo(ms,4);
	
	turn(ms,left,nom);
*/	
	fgo(ms,4);
	fgo(ms,4);
	
	turn(ms,left,nom);
	
	fgo(ms,4);
	
	turn(ms,right,nom);
	
	fgo(ms,4);
	
	turn(ms,right,nom);
	
	/////2
	
	fgo(ms,4);
	fgo(ms,4);
	
	turn(ms,left,nom);
	
	fgo(ms,2);
	fgo(ms,2);
	fgo(ms,2);
	fgo(ms,2);
	fgo(ms,2);
	fgo(ms,2);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	fgo(ms,0);
	
	fgo(400,0);///가속
	
	fgo(ms,4);
	turn(ms,left,nom);
	fgo(ms,0);
	fgo(ms,0);
	for(int j = 1; j < 4; j++)	_delay_ms(1000);
	fgo(ms,4);
	turn(ms,left,nom);
	turn(ms,left,nom); /////U턴
	
	fgo(ms,4);
	fgo(ms,4);
	
	PORTE = 0b00001111;
	
	
	
	
	return 0;
}

void turn(int max_speed, int dir, int turn_kind)
{
		if(dir == left)
		{
				PORTA = ~led;
				if(turn_kind == zr)
				{
					PORTE = 0b00001110;

					OCR1B = 0;			 	//왼쪽
					OCR1A = max_speed + 367; //오른쪽
				}
				else if(turn_kind == nom)
				{
					PORTE = 0b00000110;

					OCR1B = max_speed + 367; //왼쪽
					OCR1A = max_speed + 367; //오른쪽
				}
				_delay_ms(500);
				PORTA = ~led;
			while(1)
			{

				led = 0b00000000;
				sensdata = 0b00000000;
				for(int i = 0; i < 8; i++)
				{
					ADMUX = 0x40 | i;
					ADCSRA |= (1 << ADSC);
					while(!(ADCSRA & (1 << ADIF)));
					adc = ADC;
					if(adc >= 970)
					{
						led |= (128 >> i);
						sensdata |= (1 << i);
					}
				}
			/*	
				if((sensdata == 0b11100111) || (sensdata == 0b11110111) || (sensdata == 0b11101111)) //선이 중심에 있다면 벗어나기
				{
					while(1)
					{
						led = 0b00000000;
						sensdata = 0b00000000;
						for(int i = 0; i < 8; i++)
						{
							ADMUX = 0x40 | i;
							ADCSRA |= (1 << ADSC);
							while(!(ADCSRA & (1 << ADIF)));
							adc = ADC;
							if(adc >= 970)
							{
								led |= (128 >> i);
								sensdata |= (1 << i);
							}
						}
						if((sensdata != 0b11100111) || (sensdata != 0b11110111) || (sensdata != 0b11101111))break;
					}
					
				}
			*/	
				if((sensdata == 0b11100111) || (sensdata == 0b11110111) || (sensdata == 0b11101111))
				{
					OCR1A = 0;
					OCR1B = 0;

					break;
				}
			}
		}
		else if(dir == right)
		{
				if(turn_kind == zr)
				{
					PORTE = 0b00001011;

					OCR1A = 0;
					OCR1B = max_speed + 367;
				}
				else if(turn_kind == nom)
				{
					PORTE = 0b00001001;

					OCR1A = max_speed + 367;
					OCR1B = max_speed + 367;
				}
				_delay_ms(500);

			while(1)
			{	
				PORTA = ~led;
				led = 0b00000000;
				sensdata = 0b00000000;
				for(int i = 0; i < 8; i++)
				{
					ADMUX = 0x40 | i;
					ADCSRA |= (1 << ADSC);
					while(!(ADCSRA & (1 << ADIF)));
					adc = ADC;
					if(adc >= 970)
					{
						led |= (128 >> i);
						sensdata |= (1 << i);
					}
				}
/*
				if((sensdata == 0b11100111) || (sensdata == 0b11110111) || (sensdata == 0b11101111)) //선이 중심에 있다면 벗어나기
				{
					while(1)
					{
						led = 0b00000000;
						sensdata = 0b00000000;
						for(int i = 0; i < 8; i++)
						{
							ADMUX = 0x40 | i;
							ADCSRA |= (1 << ADSC);
							while(!(ADCSRA & (1 << ADIF)));
							adc = ADC;
							if(adc >= 970)
							{
								led |= (128 >> i);
								sensdata |= (1 << i);
							}
						}
						if((sensdata != 0b11100111) || (sensdata != 0b11110111) || (sensdata != 0b11101111))break;
					}
							
				}
*/
				if((sensdata == 0b11100111) || (sensdata == 0b11110111) || (sensdata == 0b11101111))
				{
					OCR1A = 0;
					OCR1B = 0;

					break;
				}
						
			}
		}
}	

void bmgo(int leftsSd, int rightSpd, int mdelay) //0.1초 단위
{
		PORTE = 0b00000101;
		OCR1A = leftsSd + 367;
		OCR1B = rightSpd + 367;	
		for(int k = 0; k < mdelay; k++) _delay_ms(100);
		OCR1A = 0;
		OCR1B = 0;
		PORTE = 0b00001010;
	
}
void fgo(int max_speed, int mdelay) // 0~430
{
		crosscnt = 0;
		lineOvercnt = 0;
		fgoBreakcnt = off;
		beforesensdata = 0b11100111;

		while (1)
	    {
		    PORTA = ~led;

			PORTE = 0b00001010;
			led = 0b00000000;
			sensdata = 0x00;
			afterLinecnt = 0;
			senscnt = 0;
			LfindLine = off;
			RfindLine = off;

		    
		    for(int i = 0; i < 8; i++)
		    {
			    ADMUX = 0x40 | i;
			    ADCSRA |= (1 << ADSC);
			    while(!(ADCSRA & (1 << ADIF)));
			    adc = ADC;
			    if(adc >= 970)
			    {
				    led |= (128 >> i);
				    sensdata |= (1 << i);
			    }
		    }
		    
			senscnt = sensdata;

		    for(afterLinecnt=0; senscnt!=0; afterLinecnt++) {
			    senscnt&=(senscnt-1);
		    }
		    if(afterLinecnt <= 4)lineOvercnt = on; // 1110 0111 -> 1100 0011
		    
			if(sensdata == 0b11111111)sensdata = beforesensdata;
			
		    switch(sensdata) //속도 설정
		    {
			    case 0b01111111:
			    	if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						LfindLine = on;
					}
					break;
			    case 0b00111111:
			    	if(lineOvercnt == on)fgoBreakcnt = on;
			    	else
			    	{
						left_dut = 0.17;
						right_dut = 1;
			    	}
			    	 break;
				case 0b10111111:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
				    	left_dut = 0.3;
				    	right_dut = 1;
					}
					break;	
				case 0b10011111:
			    	if(lineOvercnt == on)fgoBreakcnt = on;
			    	else
			    	{
				    	left_dut = 0.46;
				    	right_dut = 1;
			    	}
			    	break;
				case 0b11011111:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 0.66;
						right_dut = 1;
					}
					break;
			    case 0b11001111:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 0.80;
						right_dut = 1;
					}
					break;
			    case 0b11101111:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 0.95;
						right_dut = 1;
					}
					break;
					
					
					////////////////////////
					
			    case 0b11100111:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else 
					{
						left_dut = 1;
						right_dut = 1;
					}
					break;
					
					
					/////////////////////////
			    case 0b11111110:
			   	if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						RfindLine = on;
					}
					break;
				case 0b11111100:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 1;
						right_dut = 0.17;
					}
					 break;
			    case 0b11111101:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 1;
						right_dut = 0.3;
					}
					break;
				case 0b11111001:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 1;
						right_dut = 0.46;
					}
					break;
			    case 0b11111011:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 1;
						right_dut = 0.66;
					}
					break;
			    case 0b11110011:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 1;
						right_dut = 0.80;
					}
					break;
			    case 0b11110111:
					if(lineOvercnt == on)fgoBreakcnt = on;
					else
					{
						left_dut = 1;
						right_dut = 0.95;
					}
					break;
					//////////////////////
					

				default:
					left_dut = -0.5;
					right_dut = -0.5;
					break;
	
		    }
	
			beforesensdata = sensdata;

			if(fgoBreakcnt == on)
			{
				OCR1A = max_speed + 367;
				OCR1B = max_speed + 367;
				for(int k = 0; k < mdelay; k++) _delay_ms(100);
				OCR1A = 0;
				OCR1B = 0;
				PORTE = 0b00001111;
				break;
			}
			if(RfindLine == on)
			{
				while(1)
				{
					PORTE = 0b00000110;

					OCR1A = 200 + 367;
					OCR1B = 200 + 367;
				
					PORTA = ~led;
					led = 0b00000000;
					sensdata = 0b00000000;
					for(int i = 0; i < 8; i++)
					{
						ADMUX = 0x40 | i;
						ADCSRA |= (1 << ADSC);
						while(!(ADCSRA & (1 << ADIF)));
						adc = ADC;
						if(adc >= 970)
						{
							led |= (128 >> i);
							sensdata |= (1 << i);
						}
					}
					if((sensdata == 0b11100111) || (sensdata == 0b11110111) || (sensdata == 0b11101111) || (sensdata == 0b11000111) || (sensdata == 0b11100011))
					{
						PORTE = 0b00001010;

						OCR1A = 367 + max_speed;
						OCR1B = 367 + max_speed;

						break;
					}
						
				}
			}
			else if(LfindLine == on)
			{
				while(1)
				{
					PORTE = 0b00001001;

					OCR1A = 200 + 367;
					OCR1B = 200 + 367;
					
					PORTA = ~led;
					led = 0b00000000;
					sensdata = 0b00000000;
					for(int i = 0; i < 8; i++)
					{
						ADMUX = 0x40 | i;
						ADCSRA |= (1 << ADSC);
						while(!(ADCSRA & (1 << ADIF)));
						adc = ADC;
						if(adc >= 970)
						{
							led |= (128 >> i);
							sensdata |= (1 << i);
						}
					}
					if((sensdata == 0b11100111) || (sensdata == 0b11110111) || (sensdata == 0b11101111) || (sensdata == 0b11000111) || (sensdata == 0b11100011))
					{
						PORTE = 0b00001010;

						OCR1A = 367 + max_speed;
						OCR1B = 367 + max_speed;

						break;
					}
					
				}				
			}
			else
			{
				//PWM 계산
				left_pwm = (float)max_speed * left_dut;
				right_pwm = (float)max_speed * right_dut;
		    
				if(right_pwm < 0) PORTE = 0b00001001;
				else if(left_pwm < 0) PORTE = 0b00000110;
				else if((right_pwm < 0) && (left_pwm < 0)) PORTE = 0b00000101;
				else PORTE = 0b00001010;
			
					OCR1B = right_pwm + 367;
			
					OCR1A = left_pwm + 367;
			}
		    
		    int number[3] = {sensdata, OCR1B, OCR1A};
		    unsigned char onumber[2][4];
		    
		    for(int i = 0 ; i < 3 ; i++)
		    {
			    onumber[i][0] = (number[i]%(10000))/1000 + 48;
			    onumber[i][1] = (number[i]%(1000))/100 + 48;
			    onumber[i][2] = (number[i]%(100))/10 + 48;
			    onumber[i][3] = number[i]%(10) + 48;

			    Uart_Transnum(onumber[i], 4); //숫자 변환
			    Uart_Trans(0x20); //띄어버렷
		    }
		    Uart_Trans(0x0D); //엔터키

	    }
	
}

void Uart_Init(void)
{
	UCSR1A = 0x00;
	UCSR1B = (1<<TXEN1); //송신 기능 활성화
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); //8bit


	UBRR1H = 0;
	UBRR1L = 8; // 115200
}

void Uart_Trans(unsigned char data)
{
	while(!(UCSR1A & (1<<UDRE1))); //데이터를 받을 준비가 되면 탈출
	UDR1 = data; //udr에 데이터 저장
}

void Uart_Transnum(unsigned char* data, int size)
{
	for(int i = 0 ; i < size ;i++)
	{
		Uart_Trans(data[i]);
	}
}
