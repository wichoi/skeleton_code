#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "FallingDetectionReadData.h"
#include "FallingDetectionFSM.h"

// argv[1] : 0 or 1
// argv[2] : 0 or 1
// execute example : ./FallingDetection 1 1

#define ACCELERATION_DATA_LIST_LEN 500

int main(int argc, char **argv)
{
    int nRetVal = 0;
    char cInputChar = NULL;
    int nInputNumber = 0;
    bool bIsExit = false;
    bool bDataReadSuccess = false;

    // 가속도 데이터(실수형)
    float fAccDataListX[ACCELERATION_DATA_LIST_LEN] = {0.0,};
    float fAccDataListY[ACCELERATION_DATA_LIST_LEN] = {0.0,};
    float fAccDataListZ[ACCELERATION_DATA_LIST_LEN] = {0.0,};

    // 가속도 데이터(정수형) data normalize
    //int nAccDataListX[ACCELERATION_DATA_LIST_LEN] = {0,};
    //int nAccDataListY[ACCELERATION_DATA_LIST_LEN] = {0,};
    //int nAccDataListZ[ACCELERATION_DATA_LIST_LEN] = {0,};

    // 움직임 정량화 변수 순간 변위량
    //float fDsvmList[ACCELERATION_DATA_LIST_LEN] = {0,};

    // 움직임 정령화 변수 순간 변위량 moving average filter 적용
    //float fDsvmMavList[ACCELERATION_DATA_LIST_LEN] = {0,};

    printf("Enter FallingDetection Main \n");

    do
    {
        printf("======================= \n");
        printf("1. WatchData Walk \n");
        printf("2. WatchData Run \n");
        printf("3. WatchData Jump \n");
        printf("4. WatchData LieDown \n");
        printf("5. WatchData FallDownFront \n");
        printf("6. WatchData FallDownBack \n");
        printf("7. WatchData FallDownSide \n");
        printf("8. WatchData Idle \n");
        printf("9. CC2541Data Read \n");
        printf("q. Exit program \n");
        printf("======================= \n");
        printf("Input menu number : ");
        scanf("%c", &cInputChar);

        bDataReadSuccess = false;

        memset(fAccDataListX, 0, sizeof(float) * ACCELERATION_DATA_LIST_LEN);
        memset(fAccDataListY, 0, sizeof(float) * ACCELERATION_DATA_LIST_LEN);
        memset(fAccDataListZ, 0, sizeof(float) * ACCELERATION_DATA_LIST_LEN);

        switch(cInputChar)
        {
            case '1':
                nInputNumber = SelectFileNumber();
                printf("1. WatchData Walk [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("Walk", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '2':
                nInputNumber = SelectFileNumber();
                printf("2. WatchData Run [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("Run", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '3':
                nInputNumber = SelectFileNumber();
                printf("3. WatchData Jump [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("Jump", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '4':
                nInputNumber = SelectFileNumber();
                printf("4. WatchData LieDown [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("LieDown", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '5':
                nInputNumber = SelectFileNumber();
                printf("5. WatchData FallDownFront [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("FallDownFront", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '6':
                nInputNumber = SelectFileNumber();
                printf("6. WatchData FallDownBack [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("FallDownBack", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '7':
                nInputNumber = SelectFileNumber();
                printf("7. WatchData FallDownSide [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadData("FallDownSide", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '8':
                nInputNumber = SelectFileNumber();
                printf("8. WatchData Idle [%d] \n");
                bDataReadSuccess = FallingDetectionReadData("Idle", nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case '9':
                nInputNumber = SelectFileNumber();
                printf("9. CC2541Data Read [%d] \n", nInputNumber);
                bDataReadSuccess = FallingDetectionReadDataCC2541(nInputNumber, fAccDataListX, fAccDataListY, fAccDataListZ);
                break;
            case 'q':
                bIsExit = true;
                break;
            default:
                printf("Invalid input [ %c ] \n", cInputChar);                
                break;
        }

        if(bDataReadSuccess == true)
        {
            // 3축 data normalize 
            //FallingDetectionDataNormalize(fAccDataListX, nAccDataListX, ACCELERATION_DATA_LIST_LEN);
            //FallingDetectionDataNormalize(fAccDataListY, nAccDataListY, ACCELERATION_DATA_LIST_LEN);
            //FallingDetectionDataNormalize(fAccDataListZ, nAccDataListZ, ACCELERATION_DATA_LIST_LEN);

            // 움직임 정량화 변수 순간 변위량
            //FallingDatectionDSVM(nAccDataListX, nAccDataListY, nAccDataListZ, fDsvmList, ACCELERATION_DATA_LIST_LEN);

            // 움직임 정량화 변수 순간 변위량 + moving average filter
            //FallingDetectionDataMovingAverageFilter(fDsvmList, fDsvmMavList, ACCELERATION_DATA_LIST_LEN);

            // 낙삼 감지 함수
            //FallingDetectionMain(fAccDataListX, fAccDataListY, fAccDataListZ);
            FallingDetectionMainCC2541(fAccDataListX, fAccDataListY, fAccDataListZ);
        }

        while(1)
        {
            scanf("%c", &cInputChar);
            if(cInputChar == '\n')
                break;
        }
    }while (bIsExit != true);

     printf("Exit FallingDetection Main \n");
    return nRetVal;
}

int SelectFileNumber()
{
    char cInputChar = NULL;
    int nInputNumber = 0;
    while(1)
    {
        scanf("%c", &cInputChar);
        if(cInputChar == '\n')
            break;
    }

    printf("Input the file number : ");
    scanf("%d", &nInputNumber);
    return nInputNumber;
}

void FallingDetectionMain(float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ)
{
    EnDetectionRet enMovementState;
    StFallingDetecion *pstFallDetect;
    int nLoopIndex = 0;

    pstFallDetect = malloc(sizeof(StFallingDetecion));
    memset(pstFallDetect, 0, sizeof(StFallingDetecion));

    for(nLoopIndex = 0; nLoopIndex < ACCELERATION_DATA_LIST_LEN; nLoopIndex++)
    {
        enMovementState = FSMMovementState(pstFallDetect, pAccDataListX[nLoopIndex], pAccDataListY[nLoopIndex], pAccDataListZ[nLoopIndex]);
        if(enMovementState == FSP_RETURN_VALUE_MOTION_FALLING)
        {
            //printf("낙상이 감지되었습니다.\n");
            printf("Falling Detection !!! [%d] \n", nLoopIndex);
            break;
        }
        else if(enMovementState == FSP_RETURN_VALUE_MOTION_HIGH)
        {
            //printf("큰 움직임이 감지되었습니다.\n");
            printf("High Movement Detection !!! [%d] \n", nLoopIndex);
            break;
        }
        else if(nLoopIndex == ACCELERATION_DATA_LIST_LEN - 1)
        {
            //printf("일상생활 움직임이 감지되었습니다.\n");
            printf("Normal Movement Detection !!! [%d] \n", nLoopIndex);
            break;
        }
    }

    free(pstFallDetect);
}

void FallingDetectionMainCC2541(float *pAccDataListX, float *pAccDataListY, float *pAccDataListZ)
{
    EnDetectionRet enMovementState;
    StFallingDetecion *pstFallDetect;
    int nLoopIndex = 0;

    pstFallDetect = malloc(sizeof(StFallingDetecion));
    memset(pstFallDetect, 0, sizeof(StFallingDetecion));

    for(nLoopIndex = 0; nLoopIndex < ACCELERATION_DATA_LIST_LEN; nLoopIndex++)
    {
        enMovementState = FSMMovementState(pstFallDetect, pAccDataListX[nLoopIndex], pAccDataListY[nLoopIndex], pAccDataListZ[nLoopIndex]);
        //printf("index[%d], x[%f], y[%f], z[%f]\n", nLoopIndex, pAccDataListX[nLoopIndex], pAccDataListY[nLoopIndex], pAccDataListZ[nLoopIndex]);
        //AproSpeedyMain(pAccDataListX[nLoopIndex], pAccDataListY[nLoopIndex], pAccDataListZ[nLoopIndex]);
        //printf("index[%d], nDataSvm [%d]\n", nLoopIndex, pstFallDetect->listAccData.nDataSVM[nLoopIndex]);
//        printf("index[%d], nDataSvm [%d],(%f,%f,%f)\n", nLoopIndex, pstFallDetect->listAccData.nDataSvm[nLoopIndex],
//        pAccDataListX[nLoopIndex], pAccDataListY[nLoopIndex], pAccDataListZ[nLoopIndex]);
        if(enMovementState == FSP_RETURN_VALUE_MOTION_FALLING)
        {
            //printf("낙상이 감지되었습니다.\n");
            printf("Falling Detection !!! [%d] \n", nLoopIndex);
            //break;
        }
        else if(enMovementState == FSP_RETURN_VALUE_MOTION_HIGH)
        {
            //printf("큰 움직임이 감지되었습니다.\n");
            printf("High Movement Detection !!! [%d] \n", nLoopIndex);
            //break;
        }
        else if(nLoopIndex == ACCELERATION_DATA_LIST_LEN - 1)
        {
            //printf("일상생활 움직임이 감지되었습니다.\n");
            printf("Normal Movement Detection !!! [%d] \n", nLoopIndex);
            //break;
        }
    }
#if 0
    printf("\n");
    printf("DSVM \n");
    for(nLoopIndex = 0; nLoopIndex < ACCELERATION_DATA_LIST_LEN; nLoopIndex++)
    {
        //printf("%d, ", pstFallDetect->listAccData.nDataDsvmMav[nLoopIndex]);
        printf("%d, ", pstFallDetect->listAccData.nDataDsvm[nLoopIndex]);
    }
    printf("\n");
    printf("DSVM + MAV \n");
    for(nLoopIndex = 0; nLoopIndex < ACCELERATION_DATA_LIST_LEN; nLoopIndex++)
    {
        printf("%d, ", pstFallDetect->listAccData.nDataDsvmMav[nLoopIndex]);
        //printf("%d, ", pstFallDetect->listAccData.nDataDsvm[nLoopIndex]);
    }
    printf("\n");
#endif
    free(pstFallDetect);
}


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
