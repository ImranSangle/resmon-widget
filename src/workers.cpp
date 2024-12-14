
#include <windows.h>
#include "hardwareinfo.h"

float processorDelta;
bool playingBeep = false;

void processorThread(){
    while(true){
       processorDelta = GetProcessorUsage();
    }
}

void beepWorker(){
    playingBeep = true;
    Beep(2000,1000);
    Sleep(1000);
    playingBeep = false;
}
