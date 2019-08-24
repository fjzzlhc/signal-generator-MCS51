#include    <reg51.h>
#include    <absacc.h>
#define		Led_dat		XBYTE[0xcfe8] //8279模块数据寄存器
#define		Led_ctl		XBYTE[0xcfe9] //8279模块控制寄存器
#define		PA		XBYTE[0xcfa0] //8255模块A,B,C口以及控制寄存器的地址空间
#define		PB		XBYTE[0xcfa1]
#define		PC		XBYTE[0xcfa2]
#define		PCTL	XBYTE[0xcfa3]
#define		clr_req	0x70 //8255模块PC第0位的位复位命令
#define		set_req	0x71 //8255 模块PC第0位的位置位命令
#define		da_port		XBYTE[0xcfa8] //DAC0832的两个寄存器地址空间
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
unsigned char Fchar[] = "166"; //OCMJ模块屏幕显示中的频率参数
unsigned char Achar[] = "250"; //OCMJ模块屏幕显示中的振幅参数
unsigned int AMP = 250; //计算用的振幅参数
unsigned char wavemodel = 1; //显示波形类型的标识
unsigned int t0 = 0; //控制频率的相关参数
bit ForA = 0; //识别设置模式的标识

void delay(unsigned int t)
{
	for (; t>0; t--);
}
void da_conv(unsigned char dat) //数模转换接口
{
	da_port = dat;
	buf_port = dat;
}
void writebyte(unsigned char dat) //OCMJ模块指令发送接口
{
	while ((PC & 0x80) == 0x80);
	PCTL = clr_req;
	PA = dat;
	PCTL = set_req;
	while ((PC & 0x80) != 0x80);
	PCTL = clr_req;
	delay(100);
}
void writeascii(char column, char line, unsigned char dat) //OCMJ屏幕ASCII字符显示
{
	writebyte(0xf1);
	writebyte(column);
	writebyte(line);
	writebyte(dat);
}
void LCDinit() //OCMJ模块初始化
{
	PCTL = 0x88;
	writebyte(0xf4);
}
void function() //波形输出函数
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
void LCDshow(void) //用以屏幕更新
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
void changetmp(void) //波形显示数据更新函数
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
	EX0 = 1; //开0号外部中断
	EX1 = 1; //开1号外部中断
	EA = 1; //开总中断
	function(); //显示波形 
}
void changewave(void) interrupt 0 //外部中断0的中断服务程序
{
	unsigned int i = 0;
	EA = 0;  //关闭总中断
	while (i++<200) {
		delay(100);
	}
	i = 0;
	wavemodel = (wavemodel + 1)>4 ? 1 : (wavemodel + 1); //更改波形类型标识
	changetmp(); //更改波形显示数据
	LCDshow(); //更新屏幕
	EA = 1;  //打开总中断
}
void changeAandF(void) interrupt 2 //外部中断1的中断服务程序
{
	unsigned char as = 0;
	unsigned int tmp = 0, i = 0, t = 0;
	unsigned char tmpc[] = "   ";
	EA = 0; //关闭总中断

	while (as != 11) //按下确认键才退出
	{
		if (ForA) {
			for (i = 0; i < 5; i++) {     //根据设置模式更新屏幕第4行的显示
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

		if ((Led_ctl & 0xf) != 0) //判断按键是否按下
		{
			Led_ctl = 0x40;
			as = Led_dat;
			as = as & 0xf;
			while (i++ < 200) {
				delay(100);
			}
			if (as == 10) //按下更改设置模式按键
			{
				ForA = ~ForA;

			}
			if ((as <= 9) && (as >= 0)) //按下数字键
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
		if (ForA) {  //修改完成，根据标志变量进行参数的重新设置
			AMP = tmp;
			for (i = 0; i < 3; i++) Achar[i] = tmpc[i];
			changetmp(); //更新了振幅后要更新数模转换数据
		}
		else {
			t0 = (1250 + tmp) / (2 * tmp) - 4; //更新了频率后要更新频率输出相关参数（用到四舍五入）
			for (i = 0; i < 3; i++) Fchar[i] = tmpc[i];
		}
	}
	LCDshow(); //进行屏幕显示更新
	EA = 1; //最后打开总中断口
}