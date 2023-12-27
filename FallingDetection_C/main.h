#ifndef __FALLING_DETECTION_MAIN_HEADER_H__
#define __FALLING_DETECTION_MAIN_HEADER_H__

int SelectFileNumber();

// 낙상 감지 함수
void FallingDetectionMain(float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ);
void FallingDetectionMainCC2541(float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ);

#endif /* __FALLING_DETECTION_MAIN_HEADER_H__ */

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  파일명: main.c
 *
 *  설명: 낙상 감지 PC 데모 - 메인 함수
 *      
 * 
 *  작성자:
 *
 *
 *  날짜:
 *      2018/4/16
 */
