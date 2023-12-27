#ifndef __FALLING_DETECTION_READ_DATA_HEADER_H__
#define __FALLING_DETECTION_READ_DATA_HEADER_H__

bool FallingDetectionReadData(char *pFileName, int nFileNumber, float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ);
bool FallingDetectionReadDataCC2541(int nFileNumber, float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ);
void FallingDetectionDataNormalize(float *pSrcDataList, int *pDestDataList, int nDataLen);
void FallingDatectionDSVM(int *pAccDataListX, int *pAccDataListY, int *pAccDataListZ, float *pDestList, int nDataLen);
void FallingDetectionDataMovingAverageFilter(float *pSrcList, float *pDestList, int nDataLen);

#endif /* __FALLING_DETECTION_READ_DATA_HEADER_H__ */


/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  파일명: FallingDetectionReadData.h
 *
 *  설명: 낙상 감지 PC 데모 - 3축 데이터 읽어오기, 기본 수식.
 *      
 * 
 *  작성자:
 *
 *
 *  날짜:
 *      2018/4/16
 */
