#include <stdio.h>
#include <math.h>

#include "AprotechSpeedyAlgorithm.h"

static SpeedData stCurrData = {0,};
static SpeedData stPrevData = {0,};

// ������ ����ȭ ���� SVM(Sum Vector Magnitude)
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

// ��ġ ���� (T0 ~ Tt ���� 100msec �������� ��ġ ����)
// �󱸰� T0 ~ T1 �� ���Ͽ� �������� �Լ�F(t)�� ��ġ ���� �Ѵ�.
// f(x1) = f(t1) - f(t0)
// f(x2) = f(t2) - f(t1)
// ...
// f(xt) = f(t) - f(t-1)
// Sum = f(x1) + f(x2) + ... + f(xt)
float AproSpeedyIntegral(float fCurrentData, float fPrevData)
{
    return fCurrentData - fPrevData;
}

// t �ð� ������ ������
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

    // idle ���¿��� 0.98 ���� Ȯ�εȴ�.
    // �߷� ���ӵ��� 9.8 �̱� ������ *10 ��ŭ �������
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
 *  ���ϸ�: AprotechSpeedyAlgorithm.c
 *
 *  ����: Aprotech Speedy Algorithm
 *      
 * 
 *  �ۼ���:
 *
 *
 *  ��¥:
 *      2018/5/21
 */
