#include <stdio.h>
#include <math.h>

#include "AprotechSpeedyAlgorithm.h"

static SpeedData stCurrData = {0,};
static SpeedData stPrevData = {0,};

// 움직임 정량화 변수 SVM(Sum Vector Magnitude)
float AproSpeedySvm(float fCurrentAccX, float fCurrentAccY, float fCurrentAccZ)
{
    float fCalX = 0;
    float fCalY = 0;
    float fCalZ = 0;
    float fCalResult = 0;

    fCalX = pow(fCurrentAccX, 2);
    fCalY = pow(fCurrentAccY, 2);
    fCalZ = pow(fCurrentAccZ, 2);
    fCalResult = sqrt(fCalX + fCalY + fCalZ);

    return (unsigned short)fCalResult;
}

// 수치 적분 (T0 ~ Tt 까지 100msec 간격으로 수치 적분)
// 폐구간 T0 ~ T1 에 대하여 연속적인 함수F(t)를 수치 적분 한다.
// f(x1) = f(t1) - f(t0)
// f(x2) = f(t2) - f(t1)
// ...
// f(xt) = f(t) - f(t-1)
// Sum = f(x1) + f(x2) + ... + f(xt)
float AproSpeedyIntegral(float fCurrentData, float fPrevData)
{
    return fCurrentData - fPrevData;
}

// t 시간 동안의 변위량
float AproSpeedySumDurationT(float fCurrentData, float fDuration)
{
    return fCurrentData * fDuration;
}

float AproVelocity1()
{
    float fCurrSVM = 0.0;

    fCurrSVM = AproSpeedySvm(stCurrData.fAccDataX, stCurrData.fAccDataY, stCurrData.fAccDataZ) - GRAVITY_VALUE;
    return AproSpeedySumDurationT(fCurrSVM, ACCELEROMETER_SAMPLING_DURATION);
}

float AproVelocity2()
{
    float fCalAccX = 0.0;
    float fCalAccY = 0.0;
    float fCalAccZ = 0.0;
    float fSumAcc = 0.0;

    fCalAccX = pow(AproSpeedySumDurationT(stCurrData.fAccDataX, ACCELEROMETER_SAMPLING_DURATION), 2);
    fCalAccY = pow(AproSpeedySumDurationT(stCurrData.fAccDataY, ACCELEROMETER_SAMPLING_DURATION), 2);
    fCalAccZ = pow(AproSpeedySumDurationT(stCurrData.fAccDataZ, ACCELEROMETER_SAMPLING_DURATION), 2);
    fSumAcc = sqrt(fCalAccX + fCalAccY + fCalAccZ);
    return fSumAcc - AproSpeedySumDurationT(GRAVITY_VALUE, ACCELEROMETER_SAMPLING_DURATION);
}

int AproSpeedyMain(float fCurrentAccX, float fCurrentAccY, float fCurrentAccZ)
{
    int nRetVal = 0;
    float fVelocity1 = 0.0;
    float fVelocity2 = 0.0;

    stPrevData.fAccDataX = stCurrData.fAccDataX;
    stPrevData.fAccDataY = stCurrData.fAccDataY;
    stPrevData.fAccDataZ = stCurrData.fAccDataZ;

    // idle 상태에서 0.98 값이 확인된다.
    // 중력 가속도가 9.8 이기 때문에 *10 만큼 곱해줬다
    stCurrData.fAccDataX = fCurrentAccX * 10;
    stCurrData.fAccDataY = fCurrentAccY * 10;
    stCurrData.fAccDataZ = fCurrentAccZ * 10;

    fVelocity1 = AproVelocity1();
    fVelocity2 = AproVelocity2();

    printf("%f, %f\n\r", fVelocity1, fVelocity2);

    return nRetVal;
}

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  파일명: AprotechSpeedyAlgorithm.c
 *
 *  설명: Aprotech Speedy Algorithm
 *      
 * 
 *  작성자:
 *
 *
 *  날짜:
 *      2018/5/21
 */
