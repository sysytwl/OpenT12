#include "OpenT12.h"



//校准界面
void Calibration(void) {
    Pop_Windows("长按取消校准");
    delay(1000);

    bool ExitCalibration_Flag = false;
    uint8_t key=0;
    char buffer[50];

    sprintf(buffer, "顺时针或逆时针旋转摇杆");
    Display();

    for (uint8_t i = 0;i < 2;) {
      switch(i){
        case 0: {
          // read and find the max and min value
          uint16_t xaxis = GetADC0(ROTARY_PIN2);
          uint16_t yaxis = GetADC0(ROTARY_PIN1);
          // draw the region

          break;
        }

        case 1: {
          // read the center value
          break;
        }

        default:break;
      }

      //处理按键
      key = sys_KeyProcess();
      switch(key) {
        case 1:
        case 3:
          delay(50);
          i++;
        break;

        case 2:
          ExitCalibration_Flag = true;
          i = 255;
        break;

        default:break;
      }
    }


    //若中途退出，则不保存
    if (!ExitCalibration_Flag) {
        Pop_Windows("曲线拟合完成!");
        delay(800);
        //Save();
    }else{
        Pop_Windows("取消校准");
        delay(1000);
    }
}
