#include "common.h"
#include "iap.h"

int main(void)
{
    uint32_t bytes_read = 0;
    uint32_t timer1=720000;
    uint8_t c = 0;
    uint8_t buff[10];
    IAP_Init();

    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PC13 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOC,GPIO_Pin_13);						 //PC13 输出0
    do
    {
        if(SerialKeyPressed(&c)!=0)
        {
            if (c == '\n')
            {
                if(buff[bytes_read-1] == '\r')
                {
                    IAP_WriteFlag(UPDATE_FLAG_DATA);
                    break;
                }
            }
            if (bytes_read >= 10 )
            {
                SerialPutString(" Cmd size over.\r\n");
                bytes_read = 0;
                continue;
            }
            if ((c >= 0x20 && c <= 0x7E) || c == '\r')
            {
                buff[bytes_read++] = c;
                SerialPutChar(c);
            }
        }
        timer1--;
//				SerialPutChar('*');
    }
    while (timer1);//等待1秒
    GPIO_SetBits(GPIOC,GPIO_Pin_13);						 //PC13 输出高

    while(1)
    {
        switch(IAP_ReadFlag())
        {
        case APPRUN_FLAG_DATA://jump to app
            if( IAP_RunApp())
                IAP_WriteFlag(INIT_FLAG_DATA);
            break;
        case INIT_FLAG_DATA://initialze state (blank mcu)
            IAP_Main_Menu();
            break;
        case UPDATE_FLAG_DATA:// download app state
            if( !IAP_Update())
                IAP_WriteFlag(APPRUN_FLAG_DATA);
            else
                IAP_WriteFlag(INIT_FLAG_DATA);
            break;
        case UPLOAD_FLAG_DATA:// upload app state
//				if( !IAP_Upload())
//					IAP_WriteFlag(APPRUN_FLAG_DATA);
//				else
//					IAP_WriteFlag(INIT_FLAG_DATA);
            break;
        case ERASE_FLAG_DATA:// erase app state
            IAP_Erase();
            IAP_WriteFlag(INIT_FLAG_DATA);
            break;
        default:
            break;
        }
    }
}


