#include    <reg51.h>
#include    <absacc.h>
#define		Led_dat		XBYTE[0xcfe8] //8279ģ�����ݼĴ���
#define		Led_ctl		XBYTE[0xcfe9] //8279ģ����ƼĴ���
#define		PA		XBYTE[0xcfa0] //8255ģ��A,B,C���Լ����ƼĴ����ĵ�ַ�ռ�
#define		PB		XBYTE[0xcfa1]
#define		PC		XBYTE[0xcfa2]
#define		PCTL	XBYTE[0xcfa3]
#define		clr_req	0x70 //8255ģ��PC��0λ��λ��λ����
#define		set_req	0x71 //8255 ģ��PC��0λ��λ��λ����
#define		da_port		XBYTE[0xcfa8] //DAC0832�������Ĵ�����ַ�ռ�
#define		buf_port	XBYTE[0xcfa9]
code unsigned char sindata[] = { 128,140,152,165,176,188,198,208,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,208,198,188,176,165,152,140,128,115,103,90,79,67,57,47,37,29,21,15,10,5,2,1,0,1,2,5,10,15,21,29,37,47,57,67,79,90,103,115 };
code unsigned char triangledata[] = { 0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,248,240,232,224,216,208,200,192,184,176,168,160,152,144,136,128,120,112,104,96,88,80,72,64,56,48,40,32,24,16,8,0 };
code unsigned char sawtoothdata[] = { 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,180,184,188,192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252 };
code unsigned char squaredata[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255 };
code unsigned char LCDtri[] = "waveform:TRI";
code unsigned char LCDsin[] = "waveform:SIN";
code unsigned char LCDsqu[] = "waveform:SQU";
code unsigned char LCDsaw[] = "waveform:SAW";
code unsigned char set[] = "setup mode:";
code unsigned char setA[] = "setA:";
code unsigned char setF[] = "setF:";
idata unsigned char tmpwave[] = { 128,140,152,165,176,188,198,208,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,208,198,188,176,165,152,140,128,115,103,90,79,67,57,47,37,29,21,15,10,5,2,1,0,1,2,5,10,15,21,29,37,47,57,67,79,90,103,115 };
unsigned char Fchar[] = "166"; //OCMJģ����Ļ��ʾ�е�Ƶ�ʲ���
unsigned char Achar[] = "250"; //OCMJģ����Ļ��ʾ�е��������
unsigned int AMP = 250; //�����õ��������
unsigned char wavemodel = 1; //��ʾ�������͵ı�ʶ
unsigned int t0 = 0; //����Ƶ�ʵ���ز���
bit ForA = 0; //ʶ������ģʽ�ı�ʶ

void delay(unsigned int t)
{
	for (; t>0; t--);
}
void da_conv(unsigned char dat) //��ģת���ӿ�
{
	da_port = dat;
	buf_port = dat;
}
void writebyte(unsigned char dat) //OCMJģ��ָ��ͽӿ�
{
	while ((PC & 0x80) == 0x80);
	PCTL = clr_req;
	PA = dat;
	PCTL = set_req;
	while ((PC & 0x80) != 0x80);
	PCTL = clr_req;
	delay(100);
}
void writeascii(char column, char line, unsigned char dat) //OCMJ��ĻASCII�ַ���ʾ
{
	writebyte(0xf1);
	writebyte(column);
	writebyte(line);
	writebyte(dat);
}
void LCDinit() //OCMJģ���ʼ��
{
	PCTL = 0x88;
	writebyte(0xf4);
}
void function() //�����������
{
	unsigned num;
	while (1)
	{
		for (num = 0; num<64; num += 1)
		{
			da_conv(tmpwave[num]);
			delay(t0);
		}
	}
}
void LCDshow(void) //������Ļ����
{
	unsigned char i;
	writebyte(0xf4);
	if (wavemodel == 1) {
		for (i = 0; i<12; i++) { writeascii(i, 0, LCDsin[i]); }
	}
	if (wavemodel == 2) {
		for (i = 0; i<12; i++) { writeascii(i, 0, LCDtri[i]); }
	}
	if (wavemodel == 3) {
		for (i = 0; i<12; i++) { writeascii(i, 0, LCDsaw[i]); }
	}
	if (wavemodel == 4) {
		for (i = 0; i<12; i++) { writeascii(i, 0, LCDsqu[i]); }
	}
	writeascii(0, 8, 'F');
	writeascii(1, 8, ':');
	for (i = 0; i<3; i++) { writeascii(i + 2, 8, Fchar[i]); }
	writeascii(5, 8, 'H');
	writeascii(6, 8, 'Z');
	writeascii(0, 16, 'A');
	writeascii(1, 16, ':');
	writeascii(2, 16, Achar[0]);
	writeascii(3, 16, '.');
	writeascii(4, 16, Achar[1]);
	writeascii(5, 16, Achar[2]);
	writeascii(6, 16, 'V');
}
void changetmp(void) //������ʾ���ݸ��º���
{
	unsigned char num;
	switch (wavemodel) {
	case 1:for (num = 0; num<64; num++) { tmpwave[num] = sindata[num] * AMP / 250; }
		   break;
	case 2:for (num = 0; num<64; num++) { tmpwave[num] = triangledata[num] * AMP / 250; }
		   break;
	case 3:for (num = 0; num<64; num++) { tmpwave[num] = sawtoothdata[num] * AMP / 250; }
		   break;
	case 4:for (num = 0; num<64; num++) { tmpwave[num] = squaredata[num] * AMP / 250; }
		   break;
	}
}
void main(void)
{
	LCDinit();
	Led_ctl = 0xd1;
	while ((Led_ctl & 0x80) == 0x80);
	Led_ctl = 0x31;
	LCDshow();
	EX0 = 1; //��0���ⲿ�ж�
	EX1 = 1; //��1���ⲿ�ж�
	EA = 1; //�����ж�
	function(); //��ʾ���� 
}
void changewave(void) interrupt 0 //�ⲿ�ж�0���жϷ������
{
	unsigned int i = 0;
	EA = 0;  //�ر����ж�
	while (i++<200) {
		delay(100);
	}
	i = 0;
	wavemodel = (wavemodel + 1)>4 ? 1 : (wavemodel + 1); //���Ĳ������ͱ�ʶ
	changetmp(); //���Ĳ�����ʾ����
	LCDshow(); //������Ļ
	EA = 1;  //�����ж�
}
void changeAandF(void) interrupt 2 //�ⲿ�ж�1���жϷ������
{
	unsigned char as = 0;
	unsigned int tmp = 0, i = 0, t = 0;
	unsigned char tmpc[] = "   ";
	EA = 0; //�ر����ж�

	while (as != 11) //����ȷ�ϼ����˳�
	{
		if (ForA) {
			for (i = 0; i < 5; i++) {     //��������ģʽ������Ļ��4�е���ʾ
				writeascii(i, 24, setA[i]);
			}
			writeascii(8, 24, '*');
			writeascii(9, 24, '0');
			writeascii(10, 24, '.');
			writeascii(11, 24, '0');
			writeascii(12, 24, '1');
			writeascii(13, 24, 'V');
		}
		else {
			for (i = 0; i < 5; i++) {
				writeascii(i, 24, setF[i]);
			}
			writeascii(8, 24, 'H');
			writeascii(9, 24, 'Z');
			writeascii(10, 24, ' ');
			writeascii(11, 24, ' ');
			writeascii(12, 24, ' ');
			writeascii(13, 24, ' ');
		}

		if ((Led_ctl & 0xf) != 0) //�жϰ����Ƿ���
		{
			Led_ctl = 0x40;
			as = Led_dat;
			as = as & 0xf;
			while (i++ < 200) {
				delay(100);
			}
			if (as == 10) //���¸�������ģʽ����
			{
				ForA = ~ForA;

			}
			if ((as <= 9) && (as >= 0)) //�������ּ�
			{

				if (t < 3) {
					tmpc[t++] = as + 48;
					writeascii(5 + t - 1, 24, as + 48);
				}
				tmp = 10 * tmp + as;
			}

		}
	}
	if (t == 2) { tmpc[2] = tmpc[1]; tmpc[1] = tmpc[0]; tmpc[0] = '0'; }
	if (t == 1) { tmpc[2] = tmpc[0]; tmpc[0] = tmpc[1] = '0'; }
	if (t != 0) {
		if (ForA) {  //�޸���ɣ����ݱ�־�������в�������������
			AMP = tmp;
			for (i = 0; i < 3; i++) Achar[i] = tmpc[i];
			changetmp(); //�����������Ҫ������ģת������
		}
		else {
			t0 = (1250 + tmp) / (2 * tmp) - 4; //������Ƶ�ʺ�Ҫ����Ƶ�������ز������õ��������룩
			for (i = 0; i < 3; i++) Fchar[i] = tmpc[i];
		}
	}
	LCDshow(); //������Ļ��ʾ����
	EA = 1; //�������жϿ�
}