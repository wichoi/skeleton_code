#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ACCELERATION_DATA_FILE_PATH_LEN 256
#define ACCELERATION_DATA_STRING_LEN 128
#define ACCELERATION_DATA_LINE_LEN 550
#define ACCELERATION_DATA_SEQUENCE_LEN 11
#define ACCELERATION_DATA_LIST_LEN 500
#define ACCELERATION_DATA_LIST_LEN_CC2541 500

bool FallingDetectionReadData(char *pFileName, int nFileNumber, float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ)
{
    // file 포인터
    FILE *fileReadDataX = NULL;
    FILE *fileReadDataY = NULL;
    FILE *fileReadDataZ = NULL;

    // file path
    char strAccDataFilePathX[ACCELERATION_DATA_FILE_PATH_LEN] = {0,};
    char strAccDataFilePathY[ACCELERATION_DATA_FILE_PATH_LEN] = {0,};
    char strAccDataFilePathZ[ACCELERATION_DATA_FILE_PATH_LEN] = {0,};

    // 가속도 데이터(문자열)
    char strAccDataX[ACCELERATION_DATA_STRING_LEN] = {0,};
    char strAccDataY[ACCELERATION_DATA_STRING_LEN] = {0,};
    char strAccDataZ[ACCELERATION_DATA_STRING_LEN] = {0,};

    // 가속도 데이터(실수형)
    //float fAccDataX[ACCELERATION_DATA_LIST_LEN] = {0.0,};
    //float fAccDataY[ACCELERATION_DATA_LIST_LEN] = {0.0,};
    //float fAccDataZ[ACCELERATION_DATA_LIST_LEN] = {0.0,};

    // 가속도 데이터(문자열 길이)
    int nAccDataStrLenX = 0;
    int nAccDataStrLenY = 0;
    int nAccDataStrLenZ = 0;

    int nLineLenCnt = 0;
    int nSequenceCnt = 0;
    int nListCnt = 0; // list index 0 ~ 499 (total index 500)

    memset(pAccDataListX, 0.0, ACCELERATION_DATA_LIST_LEN);
    memset(pAccDataListY, 0.0, ACCELERATION_DATA_LIST_LEN);
    memset(pAccDataListZ, 0.0, ACCELERATION_DATA_LIST_LEN);


    memset(strAccDataFilePathX, 0, ACCELERATION_DATA_FILE_PATH_LEN);
    memset(strAccDataFilePathY, 0, ACCELERATION_DATA_FILE_PATH_LEN);
    memset(strAccDataFilePathZ, 0, ACCELERATION_DATA_FILE_PATH_LEN);

    sprintf(strAccDataFilePathX, "WatchData/%s/x/%d_x.txt", pFileName, nFileNumber);
    sprintf(strAccDataFilePathY, "WatchData/%s/y/%d_y.txt", pFileName, nFileNumber);
    sprintf(strAccDataFilePathZ, "WatchData/%s/z/%d_z.txt", pFileName, nFileNumber);

    fileReadDataX = fopen(strAccDataFilePathX, "r");
    fileReadDataY = fopen(strAccDataFilePathY, "r");
    fileReadDataZ = fopen(strAccDataFilePathZ, "r");

    if(fileReadDataX == NULL ||
        fileReadDataY == NULL ||
        fileReadDataZ == NULL )
    {
        printf("File open failed !!! [ %d ] \n", nFileNumber);
        fclose(fileReadDataX);
        fclose(fileReadDataY);
        fclose(fileReadDataZ);
        return false;
    }

    nAccDataStrLenX = 0;
    nAccDataStrLenY = 0;
    nAccDataStrLenZ = 0;

    nListCnt = 0;
    for(nLineLenCnt = 0; nLineLenCnt < ACCELERATION_DATA_LINE_LEN; nLineLenCnt++)
    {
        memset(strAccDataX, 0, ACCELERATION_DATA_STRING_LEN);
        memset(strAccDataY, 0, ACCELERATION_DATA_STRING_LEN);
        memset(strAccDataZ, 0, ACCELERATION_DATA_STRING_LEN);

        fscanf(fileReadDataX, "%s", strAccDataX);
        fscanf(fileReadDataY, "%s", strAccDataY);
        fscanf(fileReadDataZ, "%s", strAccDataZ);

        nAccDataStrLenX += strlen(strAccDataX) + 1;
        nAccDataStrLenY += strlen(strAccDataY) + 1;
        nAccDataStrLenZ += strlen(strAccDataZ) + 1;

        //printf("strAccDataX [%s], nSquLen[%d][%d][%d]\n", strAccDataX, nAccDataStrLenX, nAccDataStrLenY, nAccDataStrLenZ);

        // 시퀀스 넘버는 항상 맞다고 가정하고 구현하였다. * + 숫자(0부터 시작, 순차 증가)
        if(nLineLenCnt % ACCELERATION_DATA_SEQUENCE_LEN == 0)
        {
        }
        else
        {
            pAccDataListX[nListCnt] = atof(strAccDataX);
            pAccDataListY[nListCnt] = atof(strAccDataY);
            pAccDataListZ[nListCnt] = atof(strAccDataZ);

            //printf("%f[%d]\n", pAccDataX[nListCnt],nListCnt);
            nListCnt++;
        }

        fseek(fileReadDataX, nAccDataStrLenX, SEEK_SET);
        fseek(fileReadDataY, nAccDataStrLenY, SEEK_SET);
        fseek(fileReadDataZ, nAccDataStrLenZ, SEEK_SET);
    }

    fclose(fileReadDataX);
    fclose(fileReadDataY);
    fclose(fileReadDataZ);

    printf("File read success !!! [ %d ] \n", nFileNumber);
    return true;
}

bool FallingDetectionReadDataCC2541(int nFileNumber, float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ)
{
    // file 포인터
    FILE *fileReadData = NULL;

    // file path
    char strAccDataFilePath[ACCELERATION_DATA_FILE_PATH_LEN] = {0,};

    // 가속도 데이터(ex, -0.125,0.938,0.375)
    char strAccData[ACCELERATION_DATA_LIST_LEN_CC2541] = {0,};

    char *pAccData = NULL;

    // 가속도 데이터(ex, -0.125,0.938,0.375)
    int nAccDataStrLen = 0;

    int nLineLenCnt = 0;

    memset(pAccDataListX, 0.0, ACCELERATION_DATA_LIST_LEN_CC2541);
    memset(pAccDataListY, 0.0, ACCELERATION_DATA_LIST_LEN_CC2541);
    memset(pAccDataListZ, 0.0, ACCELERATION_DATA_LIST_LEN_CC2541);

    memset(strAccDataFilePath, 0, ACCELERATION_DATA_FILE_PATH_LEN);

    sprintf(strAccDataFilePath, "CC2541/%d.txt", nFileNumber);

    fileReadData = fopen(strAccDataFilePath, "r");

    if(fileReadData == NULL)
    {
        printf("File open failed !!! [ %d ] \n", nFileNumber);
        fclose(fileReadData);
        return false;
    }

    nAccDataStrLen = 0;

    for(nLineLenCnt = 0; nLineLenCnt < ACCELERATION_DATA_LIST_LEN_CC2541; nLineLenCnt++)
    {
        memset(strAccData, 0, ACCELERATION_DATA_LIST_LEN_CC2541);

        fscanf(fileReadData, "%s", strAccData);

        if(strlen(strAccData) == 0)
        {
            printf("nLineLenCnt [%d]\n", nLineLenCnt);
            break;
        }

        nAccDataStrLen += strlen(strAccData) + 1;

        //printf("strAccData [%s]\n", strAccData);

        // AccDataX
        pAccData = strtok(strAccData, ",");
        //printf("pAccData [%s]\n", pAccData); 
        pAccDataListX[nLineLenCnt] = atof(pAccData);

        // AccDataY
        pAccData = strtok(NULL, ",");
        //printf("pAccData [%s]\n", pAccData); 
        pAccDataListY[nLineLenCnt] = atof(pAccData);

        // AccDataZ
        pAccData = strtok(NULL, "\n");
        //printf("pAccData [%s]\n", pAccData); 
        pAccDataListZ[nLineLenCnt] = atof(pAccData);

        //printf("nLineLenCnt [%d] (%f, %f, %f)\n", nLineLenCnt, pAccDataListX[nLineLenCnt], pAccDataListY[nLineLenCnt], pAccDataListZ[nLineLenCnt]);

        fseek(fileReadData, nAccDataStrLen, SEEK_SET);
    }

    fclose(fileReadData);

    printf("File read success !!! [ %d ] \n", nFileNumber);
    return true;
}


// low 데이터에 * 100 만큼 정규화 진행
// float 데이터를 정수형으로 변환
void FallingDetectionDataNormalize(float *pSrcDataList, int *pDestDataList, int nDataLen)
{
    int nLoopCnt = 0;
    for (nLoopCnt = 0; nLoopCnt < nDataLen; nLoopCnt++)
    {
        pDestDataList[nLoopCnt] = (int)(pSrcDataList[nLoopCnt] * 100);
        //printf("pDestDataList[%d] = %d\n", nLoopCnt, pDestDataList[nLoopCnt]);
    }
}

// 움직임 정량화 변수 수치미분값 DSVM(Differential Sum Vector Magnitude)
void FallingDatectionDSVM(int *pAccDataListX, int *pAccDataListY, int *pAccDataListZ, float *pDestList, int nDataLen)
{
    int nLoopCnt = 0;

    float fCalX = 0;
    float fCalY = 0;
    float fCalZ = 0;

    memset(pDestList, 0, nDataLen);

    for(nLoopCnt = 1; nLoopCnt < nDataLen; nLoopCnt++)
    {
        fCalX = powf(pAccDataListX[nLoopCnt] - pAccDataListX[nLoopCnt - 1], 2);
        fCalY = powf(pAccDataListY[nLoopCnt] - pAccDataListY[nLoopCnt - 1], 2);
        fCalZ = powf(pAccDataListZ[nLoopCnt] - pAccDataListZ[nLoopCnt - 1], 2);
        pDestList[nLoopCnt] = sqrtf(fCalX + fCalY + fCalZ);
        //printf("pDestList[%d] = %f\n", nLoopCnt, pDestList[nLoopCnt]);
    }
}

// 50개 단위 moving average filter
void FallingDetectionDataMovingAverageFilter(float *pSrcList, float *pDestList, int nDataLen)
{
    int nIndex = 0;
    int nSubIndex = 0;

    float fSum = 0;
    float fDivide = 1;
    int nCalMaxIndex = -1;

    memset(pDestList, 0, nDataLen);

    for(nIndex = 0 ; nIndex < nDataLen; nIndex++)
    {
        fSum = 0;
        if(nIndex > 49)
        {
            nCalMaxIndex = nIndex - 50;
            fDivide = 50;
        }
        else
        {
            nCalMaxIndex = - 1;
            fDivide = nIndex + 1;
        }

        for(nSubIndex = nIndex; nSubIndex > nCalMaxIndex; nSubIndex--)
        {
            fSum = fSum + pSrcList[nSubIndex];
        }

        pDestList[nIndex] = fSum / fDivide;
        //printf("pDestList[%d] = %f\n", nIndex, pDestList[nIndex]);
    }
}

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  파일명: FallingDetectionReadData.c
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
