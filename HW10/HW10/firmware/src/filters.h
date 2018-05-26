#ifndef FILTERS_H__
#define FILTERS_H__

#define BUFFLEN 5

void nullBuffer(float* buffer);
void shiftBuffer(float, float* buffer);
float MAF(float* buffer);
float FIR(float* w, float* buffer);

#endif