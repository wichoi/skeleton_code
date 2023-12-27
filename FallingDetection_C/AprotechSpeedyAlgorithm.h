#ifndef __APROTECH_SPEEDY_ALGORITHM_HEADER_H__
#define __APROTECH_SPEEDY_ALGORITHM_HEADER_H__

#define GRAVITY_VALUE 9.81
#define ACCELEROMETER_SAMPLING_DURATION 0.01   // 100msec ���� ���ӵ� ������ ȹ��

typedef struct AproSpeedAlgorithm
{
    float fAccDataX;
    float fAccDataY;
    float fAccDataZ;
} SpeedData;


float AproSpeedySvm(float fCurrentAccX, float fCurrentAccY, float fCurrentAccZ);
float AproSpeedyIntegral(float fCurrentData , float fPrevData);

#endif /* __APROTECH_SPEEDY_ALGORITHM_HEADER_H__ */

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  ���ϸ�: AprotechSpeedyAlgorithm.h
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

