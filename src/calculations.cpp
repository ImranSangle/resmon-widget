
#include <math.h>
#include <thread>
#include <windows.h>

#include "workers.h"
#include "hardwareinfo.h"

float processor;
float alpha = 0;

extern float processorDelta;
extern int x;
extern int y;
extern float windowWidth;
extern float windowHeight;
extern int view;
extern bool playingBeep;

void makeBeep(float value){
    float normValue = (value-0.0f)/(4.9f-0.0f);
    if(normValue > 0.8f && playingBeep == false){
        std::thread thread(beepWorker);
        thread.detach();
    }
}

float roundToDecimalPlaces(float value, int decimalPlaces) {
    double factor = std::pow(10, decimalPlaces);
    return std::round(value * factor) / factor;
}

void updateValue(HWND hwnd){
        float procdelt = roundToDecimalPlaces(processorDelta,2);
        processor = roundToDecimalPlaces(processor,2);
        if(processor > procdelt){
            processor-=0.01;
        }else if(processor < procdelt){
            processor+=0.01;
        }else{
            processor = procdelt;
        }
        if(view == 1){
        alpha = processor;
        }else{
        alpha = GetRamUsage();
        makeBeep(alpha);
        }
        x = (cos(fmax(fmin(alpha,5.2f),0)+2.25)*(windowWidth/300)*100)+windowWidth/2;
        y = (sin(fmax(fmin(alpha,5.2f),0)+2.25)*(windowHeight/300)*100)+windowHeight/2;
       
}
