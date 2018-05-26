#include "filters.h"

void nullBuffer(float* buff){ // zeroes out all buffer elements
    int i;
    for(i = 0; i < BUFFLEN; i++){
        buff[i] = 0;
    }
}
void shiftBuffer(float new_read, float* buff){
    int i;
    for(i = 0; i < BUFFLEN-1; i++){
        float tmp = buff[i];
        buff[i+1] = tmp;
    }
    buff[0] = new_read;
}

float MAF(float* buff){
    int i;
    float avg = 0;
    
    for(i = 0; i < BUFFLEN; i++){
        avg = avg + buff[i];
    }

    return avg/BUFFLEN;
}

float FIR(float* w, float* buff){
    float tmp = w[0];
    int i;
    
    for (i = 0; i<BUFFLEN; i++){
        tmp = tmp + w[i+1]*buff[i];
    } 
    
    return tmp;
}