#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float sum = 0;
    int i;
    for(i = 0; i<N; i++){
        sum = sum + fabs((x[i]*x[i]));
    }
    float power = 10*log10((sum*1/N)+1e-10);
    return power;
}

float compute_power_window(const float *x, unsigned int N) {
    float sumSignal = 0;
    float sumWindow = 0;
    float *w;
    int n;
    for(n = 0; n<N; n++){
        w[n] = 0.54 - 0.46*cosf(2.0*M_PI*n/(N-1));
        sumSignal = sumSignal + fabs((x[n]*w[n])*(x[n]*w[n]));
        sumWindow = sumWindow + fabs((w[n]*w[n]));
    }
    float power = (float) 10*log10((sumSignal/sumWindow)+1e-10);
    return power;
}

float compute_am(const float *x, unsigned int N) {
    float sum = 0;
    int i;
    for(i = 0; i<N; i++){
        sum = sum + fabs(x[i]);
    }
    float amp = sum/N;
    return amp;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    int sum = 0;
    int i;
    for(i = 1; i < N; i++) {
        if ((x[i] >= 0 && x[i-1] < 0) || (x[i] <= 0 && x[i-1] > 0)) {
            sum++;
        }
    }
    float zcr = (sum * fm) / (2*(N-1));
    return zcr;
}

