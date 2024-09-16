#pragma once

const int xres = 8;
const int yres = 8;
const int colorBits = 12;
const int colorCount = 1 << colorBits;
const int colorMax = colorCount - 1;
unsigned short frameBuffer[yres][xres][3];
volatile unsigned long portFrameBuffer[colorBits][yres];
const int minTicksPerSubframe = 37;
//const int minTicksPerSubframe = 128;

#include "rick.h"

void pix(int x, int y, int r, int g, int b)
{
    frameBuffer[y][x][0] = r;
    frameBuffer[y][x][1] = g;
    frameBuffer[y][x][2] = b;
}

void convertFrameBuffer(unsigned short *frameBuffer, int bitShift = 0)
{
	for(int bit = 0; bit < colorBits; bit++)
		for(int y = 0; y < yres; y++)
		{
			int r = 0;
			int g = 0;
			int b = 0;
			for(int x = 0; x < xres; x++)
			{
				r |= ((frameBuffer[((y * xres) + x) * 3 + 0] >> (bit + bitShift)) & 1) << x;
				g |= ((frameBuffer[((y * xres) + x) * 3 + 1] >> (bit + bitShift)) & 1) << x;
				b |= ((frameBuffer[((y * xres) + x) * 3 + 2] >> (bit + bitShift)) & 1) << x;
			}
			portFrameBuffer[bit][y] = (b << 16) | (r << 8) | g;
		}
}

void copyPortFrameBuffer(const unsigned long *f, int sourceBits = 8, int bitShift = 0)
{
    for(int i = 0; i < sourceBits * yres; i++)
        ((unsigned long *)portFrameBuffer[bitShift])[i] = f[i];
    for(int i = 0; i < bitShift * yres; i++)
        ((unsigned long *)portFrameBuffer[0])[i] = 0;
    for(int i = 0; i < (colorBits - bitShift - sourceBits) * yres; i++)
        ((unsigned long *)portFrameBuffer[bitShift + sourceBits])[i] = 0;
}

volatile int subFrame = 0;
volatile int frame = 0;

void __attribute__((interrupt("WCH-Interrupt-fast"))) SystickIntSubframe_(void)  //not optimized
{
    subFrame++;
    SysTick->SR = 0;
    static int y = 0;
    static int bit = 0;
    unsigned long ports = portFrameBuffer[bit][y];
    GPIOB->OUTDR = ports & 0xffff;
    GPIOC->OUTDR = (256 << y) | (ports >> 16);
    SysTick->CMP = minTicksPerSubframe << bit;
    y++;
    if(y == yres)
    {
        y = 0;
        bit++;
        if(bit == colorBits)
        {
            bit = 0;
            frame++;
        }
    }
}

void __attribute__((interrupt("WCH-Interrupt-fast"))) SystickIntSubframe(void)
{
    static int y = 0;
    static int bit = 0;
    static unsigned short *p = (unsigned short*)portFrameBuffer[0];
    SysTick->CMP = minTicksPerSubframe << bit;
    subFrame++;
    SysTick->SR = 0;
    GPIOB->OUTDR = *(p++);
    GPIOC->OUTDR = (256 << y) | *(p++);
    //GPIOC->BCR = (0b11111111 << 8);
    if(y == yres - 1)
    {
        y = 0;
        if(bit == colorBits - 1)
        {
            frame++;
            bit = 0;
            p = (unsigned short*)portFrameBuffer[0];
        }
        else
            bit++;
    }
    else
        y++;
}

void initSystick()
{
    NVIC_SetPriority(SysTicK_IRQn, 1);
    SetVTFIRQ((u32)SystickIntSubframe, SysTicK_IRQn, 0, ENABLE);
    NVIC_EnableIRQ(SysTicK_IRQn);

    SysTick->SR = 0;
    SysTick->CNT = 0;
    SysTick->CMP = minTicksPerSubframe;
    SysTick->CTLR = 1 | //enable
                    2 | //interrupt enable
                    4 | //HCLK as source
                    15;  //restart counting
}

void initFastFrame()
{
	//R B08-B15
	//G B00-B07
	//B C00-C08
	//ROW C08-C15
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	for(int i = 0; i < 16; i++)
	{
		GPIO_InitStructure.GPIO_Pin = 1 << i;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	initSystick();
}

