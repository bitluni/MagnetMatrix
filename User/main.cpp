
#include "debug.h"
#include "fastFrame.h"

void initRainbow()
{
    int rainbowScale = colorCount / xres;
    for(int y = 0; y < yres; y++)
        for(int x = 0; x < xres; x++)
        {
            int r = (x) * rainbowScale;
            int g = (y) * rainbowScale;
            int b = (7-x) * rainbowScale;
            pix(x, y, r, g, b);
        }

}

void drawSides(int sides = 0b1111)
{
    int f = (frame >> 2) & 7;
    int f1 = (f - 1) & 7;
    int f2 = (f + 1) & 7;
    if(sides & 4)
    {
        pix(f, 0, 0, colorMax, 0);
        pix(f1, 0, 0, colorMax, 0);
        pix(f2, 0, 0, 0, colorMax);
    }
    if(sides & 1)
    {
        pix(0, 7 - f, 0, colorMax, 0);
        pix(0, 7 - f1, 0, colorMax, 0);
        pix(0, 7 - f2, 0, 0, colorMax);
    }
    if(sides & 2)
    {
        pix(7 - f, 7, 0, colorMax, 0);
        pix(7 - f1, 7, 0, colorMax, 0);
        pix(7 - f2, 7, 0, 0, colorMax);
    }
    if(sides & 8)
    {
        pix(7, f, 0, colorMax, 0);
        pix(7, f1, 0, colorMax, 0);
        pix(7, f2, 0, 0, colorMax);
    }
}

void fillFrame(int r = 0, int g = 0, int b = 0)
{
    for(int y = 0; y < yres; y++)
        for(int x = 0; x < xres; x++)
            pix(x, y, r, g, b);
}

void initGrey()
{
    for(int y = 0; y < yres; y++)
        for(int x = 0; x < xres; x++)
            pix(x, y, 1 << (colorBits - 1), 1 << (colorBits - 1), 1 << (colorBits - 1));
}


const int buttonPins[] = {2, 3, 4, 5, 6};

void initButtons()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    for(int i = 0; i < 5; i++)
    {
        GPIO_InitStructure.GPIO_Pin = 1 << buttonPins[i];
        GPIO_Init(GPIOD, &GPIO_InitStructure);
    }
}

bool getButton(int b)
{
    if(GPIO_ReadInputDataBit(GPIOD, 1 << buttonPins[b]))
        return false;
    return true;
}

void initSound()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = 1 << 15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//                  left    bottom  up      right
int IOPins[4][2] = {{0, 1}, {7, 8}, {5, 6}, {9, 10}};

void initIO()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    for(int i = 0; i < 4; i++)
    {
        GPIO_InitStructure.GPIO_Pin = 1 << IOPins[i][0];
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_WriteBit(GPIOA, 1 << IOPins[i][0], Bit_RESET);
    }
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    for(int i = 0; i < 4; i++)
    {
        GPIO_InitStructure.GPIO_Pin = 1 << IOPins[i][1];
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
}

int IOstate(int ioIndex)
{
    if(GPIO_ReadInputDataBit(GPIOA, 1 << IOPins[ioIndex][1]))
        return 1;
    return 0;
}
/*
const unsigned char smiley[16][16][3] = {
{{95, 129, 183}, {90, 125, 180}, {95, 129, 182}, {89, 125, 179}, {93, 127, 181}, {92, 126, 181}, {88, 122, 178}, {95, 128, 182}, {92, 126, 181}, {88, 123, 178}, {95, 129, 182}, {89, 125, 179}, {95, 129, 182}, {92, 126, 182}, {97, 131, 184}, {97, 130, 183}, },
{{102, 134, 185}, {93, 127, 181}, {99, 132, 184}, {91, 125, 179}, {73, 103, 149}, {7, 22, 49}, {14, 13, 0}, {18, 15, 0}, {18, 15, 0}, {14, 13, 0}, {8, 23, 49}, {72, 102, 149}, {92, 126, 181}, {100, 133, 184}, {97, 130, 183}, {100, 132, 184}, },
{{97, 130, 182}, {106, 137, 187}, {92, 126, 181}, {28, 49, 82}, {16, 13, 0}, {124, 100, 0}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {125, 100, 0}, {17, 14, 0}, {26, 47, 82}, {102, 134, 185}, {99, 132, 184}, {107, 139, 188}, },
{{103, 135, 185}, {100, 132, 183}, {29, 49, 82}, {33, 26, 0}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {193, 157, 9}, {33, 26, 0}, {27, 47, 81}, {110, 141, 188}, {101, 132, 184}, },
{{111, 141, 188}, {79, 107, 151}, {13, 11, 0}, {76, 61, 0}, {45, 36, 0}, {139, 111, 0}, {193, 157, 9}, {193, 157, 9}, {159, 128, 0}, {77, 62, 4}, {51, 40, 0}, {96, 76, 0}, {193, 157, 9}, {17, 14, 0}, {74, 103, 148}, {111, 141, 188}, },
{{104, 135, 184}, {11, 25, 47}, {43, 33, 0}, {245, 245, 245}, {11, 9, 10}, {11, 8, 0}, {193, 157, 9}, {193, 157, 9}, {68, 55, 7}, {252, 252, 252}, {37, 36, 36}, {2, 1, 0}, {169, 136, 0}, {125, 100, 0}, {13, 26, 47}, {111, 141, 187}, },
{{119, 147, 191}, {15, 14, 0}, {89, 76, 27}, {255, 255, 255}, {238, 238, 238}, {73, 69, 56}, {193, 157, 9}, {193, 157, 9}, {79, 74, 58}, {255, 255, 255}, {247, 247, 247}, {138, 137, 138}, {105, 85, 0}, {193, 157, 9}, {15, 14, 0}, {113, 143, 188}, },
{{111, 141, 187}, {18, 15, 0}, {91, 73, 0}, {74, 60, 0}, {74, 60, 0}, {72, 57, 0}, {193, 157, 9}, {193, 157, 9}, {66, 53, 0}, {68, 54, 0}, {69, 55, 0}, {60, 47, 0}, {159, 128, 0}, {193, 157, 9}, {18, 15, 0}, {117, 147, 191}, },
{{116, 146, 189}, {18, 15, 0}, {152, 122, 0}, {79, 63, 0}, {79, 63, 0}, {79, 63, 0}, {79, 63, 0}, {79, 63, 0}, {79, 63, 0}, {79, 63, 0}, {79, 63, 0}, {88, 70, 0}, {193, 157, 9}, {193, 157, 9}, {18, 15, 0}, {126, 154, 195}, },
{{131, 159, 197}, {17, 15, 0}, {180, 145, 2}, {10, 3, 0}, {3, 0, 1}, {3, 0, 1}, {3, 0, 1}, {3, 0, 1}, {3, 0, 1}, {3, 0, 1}, {3, 0, 1}, {3, 0, 1}, {162, 130, 0}, {193, 157, 9}, {15, 14, 0}, {116, 145, 189}, },
{{71, 107, 172}, {3, 20, 51}, {125, 100, 0}, {43, 31, 0}, {6, 0, 2}, {6, 0, 2}, {6, 0, 2}, {6, 0, 2}, {6, 0, 2}, {6, 0, 2}, {6, 0, 2}, {3, 0, 1}, {162, 130, 0}, {125, 100, 0}, {5, 21, 50}, {88, 123, 179}, },
{{71, 108, 172}, {47, 81, 141}, {16, 13, 0}, {162, 130, 0}, {6, 0, 1}, {6, 0, 2}, {6, 0, 2}, {59, 0, 25}, {108, 33, 66}, {92, 11, 46}, {9, 0, 4}, {15, 8, 0}, {193, 157, 9}, {16, 13, 0}, {50, 84, 142}, {66, 103, 170}, },
{{95, 128, 182}, {95, 128, 182}, {21, 42, 82}, {32, 25, 0}, {97, 77, 0}, {6, 0, 2}, {49, 0, 21}, {167, 102, 132}, {167, 102, 132}, {123, 67, 93}, {33, 14, 0}, {153, 122, 0}, {32, 26, 0}, {22, 44, 82}, {83, 117, 176}, {91, 125, 179}, },
{{95, 128, 180}, {105, 135, 184}, {109, 139, 186}, {30, 49, 82}, {16, 13, 0}, {89, 71, 0}, {49, 37, 0}, {54, 31, 0}, {47, 31, 0}, {66, 52, 0}, {117, 94, 0}, {16, 13, 0}, {32, 51, 82}, {103, 134, 184}, {120, 149, 192}, {111, 142, 187}, },
{{141, 166, 201}, {114, 144, 187}, {128, 156, 195}, {118, 147, 189}, {90, 115, 153}, {15, 27, 46}, {16, 14, 0}, {18, 15, 0}, {18, 15, 0}, {16, 15, 0}, {15, 27, 46}, {94, 119, 155}, {117, 145, 188}, {138, 164, 199}, {119, 147, 189}, {123, 151, 192}, },
{{139, 164, 199}, {165, 186, 212}, {135, 160, 197}, {152, 175, 205}, {143, 168, 200}, {138, 164, 198}, {132, 158, 195}, {150, 173, 204}, {130, 156, 194}, {141, 166, 199}, {142, 166, 199}, {147, 171, 203}, {156, 179, 207}, {138, 164, 198}, {160, 182, 210}, {165, 186, 212}, },
};*/

/*void drawSmiley(int x0, int y0)
{
    for(int y = 0; y < 8; y++)
        for(int x = 0; x < 8; x++)
            pix(x, y, smiley[y0 + y][x0 + x][0] << 4, smiley[y0 + y][x0 + x][1] << 4, smiley[y0 + y][x0 + x][2] << 4);
}*/

volatile int mode = 02;
int main(void)
{
    initRainbow();
	initButtons();
	initSound();
    initIO();
    initFastFrame();
    //drawSmiley(8, 0);

    while(1)
    {
        static int bitShift = 3;
        if(getButton(0))
        {
            initRainbow();
            mode = 0;
        }
        if(getButton(1))
        {
            mode = 1;
        }
        if(getButton(2))
        {
            mode = 2;
        }
        if(getButton(4))
        {
            fillFrame(0, 0, 0);
            mode = 3;
        }
        if(colorBits > 8 && getButton(3) && ((frame & 63) == 0)) bitShift = (bitShift + 1) % (colorBits - 8);
//       if(getButton(4))
        //GPIOA->OUTDR ^= ((subFrame >> 8) & 1) << 15;
       int f = frame;
       while (f == frame)
       {
       };
       static int sides = 0;
       switch(mode)
       {
           case 0:
               convertFrameBuffer(frameBuffer[0][0], (colorBits - 8) - bitShift - 1);
               break;
           case 1:
               copyPortFrameBuffer(anim[(frame >> 2) % frameCount][0], 8, bitShift);
               break;
           case 2:
               convertFrameBuffer(frameBuffer[0][0], (colorBits - 8) - bitShift - 1);
               fillFrame(0, 0, 0);
               drawSides(sides);
               break;
           case 3:
               fillFrame(0, 0, 0);
               for(int i = 0; i < 4; i++)
               {
                   if(GPIOA->INDR & (1 << IOPins[i][0])) pix(i, 0, colorMax, colorMax, colorMax);
                   if(GPIOA->INDR & (1 << IOPins[i][1])) pix(i, 1, colorMax, colorMax, colorMax);
               }
               convertFrameBuffer(frameBuffer[0][0], (colorBits - 8) - bitShift - 1);
               break;
       }
       sides = 0;
       if(IOstate(0))
           sides |= 1;
       if(IOstate(1))
           sides |= 2;
       if(IOstate(2))
           sides |= 4;
       if(IOstate(3))
           sides |= 8;
    }
}

