#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "FallingDetectionFSM.h"

void FSMFallingDetectionInit(StFallingDetecion *pstFallDetect)
{
    pstFallDetect->enDetectCurrentState = FSM_STATE_MOVEMENT_INIT;
    pstFallDetect->nFallCheckWaitingCnt = 0;
    pstFallDetect->nFallDetectionCnt = 0;

    //pstFallDetect->listAccData.nHeadIndex = 0;
    pstFallDetect->listAccData.nTailIndex = 0;
    pstFallDetect->listAccData.nItemCount = 0;
    memset(pstFallDetect->listAccData.nDataSVM, 0, sizeof(unsigned short) * MAX_LIST_LEN);
}

void FSMMovementStateSet(StFallingDetecion *pstFallDetect, EnDetectionState enState)
{
    pstFallDetect->enDetectCurrentState = enState;
}

EnDetectionState FSMMovementStateGet(StFallingDetecion *pstFallDetect)
{
    return pstFallDetect->enDetectCurrentState;
}

EnDetectionRet FSMMovementState(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ)
{
    EnDetectionRet enRetVal = FSP_RETURN_VALUE_MOTION_INIT;

    FSMMovementDataAppend(pstFallDetect, fAccDataX, fAccDataY, fAccDataZ);

    switch(pstFallDetect->enDetectCurrentState)
    {
    case FSM_STATE_MOVEMENT_INIT:
        FSMMovementStateInit(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_IDLE:
        FSMMovementStateIdle(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_NORMAL:
        FSMMovementStateNormal(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_HIGH:
        FSMMovementStateHigh(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_1:
        enRetVal = FSMMovementStateFallCheckStage1(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_2:
        enRetVal = FSMMovementStateFallCheckStage2(pstFallDetect);
        break;

    default:
        //printf("error, FSMMovementState() unknown state\n");
        break;
    }

    return enRetVal;
}

void FSMMovementStateInit(StFallingDetecion *pstFallDetect)
{
    if(FSMMovementDataReadItemCount(pstFallDetect) > DETECTION_WAITING_VALUE)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
    }
}

void FSMMovementStateIdle(StFallingDetecion *pstFallDetect)
{
    EnDetectionLevel enLatestData = FSMMovementDataReadTailState(pstFallDetect);
    switch(enLatestData)
    {
    case MOVEMENT_LEVEL_IDLE:
        // 오랜 시간 idle 상태가 관측되면 "움직임이 없습니다. 확인이 필요합니다." 알림
        //FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
        break;

    case MOVEMENT_LEVEL_NORMAL:
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_NORMAL);
        break;

    case MOVEMENT_LEVEL_HIGH:
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
        break;

    default:
        //printf("FSMMovementStateInit() undefined data");
        break;
    }
}

void FSMMovementStateNormal(StFallingDetecion *pstFallDetect)
{
    EnDetectionLevel enLatestData = FSMMovementDataReadTailState(pstFallDetect);
    switch(enLatestData)
    {
    case MOVEMENT_LEVEL_IDLE:
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
        break;

    case MOVEMENT_LEVEL_NORMAL:
        //FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_NORMAL);
        break;

    case MOVEMENT_LEVEL_HIGH:
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
        break;

    default:
        //printf("FSMMovementStateNormal() undefined data");
        break;
    }
}

void FSMMovementStateHigh(StFallingDetecion *pstFallDetect)
{
    EnDetectionLevel enLatestData = FSMMovementDataReadTailState(pstFallDetect);
    // high 상태에선 오직 fall check 상태로만 전이한다.
    if(enLatestData == MOVEMENT_LEVEL_HIGH)
    {
        //FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
    }
    else
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_1);
    }
}

EnDetectionRet FSMMovementStateFallCheckStage1(StFallingDetecion *pstFallDetect)
{
    EnDetectionLevel enLatestData = FSMMovementDataReadTailState(pstFallDetect);
    EnDetectionState enNextState = FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_1;
    EnDetectionRet retVal = FSP_RETURN_VALUE_MOTION_INIT;

    // 큰 움직임 이후 FALL_CHECK_WAITING_VALUE 개의 데이터를 관측하며 대기한다.
    switch(enLatestData)
    {
    case MOVEMENT_LEVEL_IDLE:
        pstFallDetect->nFallCheckWaitingCnt++;
        pstFallDetect->nFallDetectionCnt++;
        if(pstFallDetect->nFallCheckWaitingCnt > FALL_CHECK_WAITING_VALUE)
        {
            enNextState = FSM_STATE_MOVEMENT_IDLE;
        }
        break;

    case MOVEMENT_LEVEL_NORMAL:
        pstFallDetect->nFallCheckWaitingCnt++;
        pstFallDetect->nFallDetectionCnt = 0;
        if(pstFallDetect->nFallCheckWaitingCnt > FALL_CHECK_WAITING_VALUE)
        {
            enNextState = FSM_STATE_MOVEMENT_NORMAL;
        }
        break;

    case MOVEMENT_LEVEL_HIGH:
        enNextState = FSM_STATE_MOVEMENT_HIGH;
        break;

    default:
        //printf("FSMMovementStateFallCheckStage1() undefined data");
        break;
    }

    switch(enNextState)
    {
    case FSM_STATE_MOVEMENT_IDLE:
        if(pstFallDetect->nFallDetectionCnt > FALL_DETECTION_VALIE)
        {
            retVal = FSMMovementFallDetection(pstFallDetect);
        }

        if(retVal == FSP_RETURN_VALUE_MOTION_FALLING)
        {
            enNextState = FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_2; // 2차 알림 시나리오
        }

        pstFallDetect->nFallCheckWaitingCnt = 0;
        pstFallDetect->nFallDetectionCnt = 0;
        FSMMovementStateSet(pstFallDetect, enNextState);
        break;

    case FSM_STATE_MOVEMENT_NORMAL:
    case FSM_STATE_MOVEMENT_HIGH:
        pstFallDetect->nFallCheckWaitingCnt = 0;
        pstFallDetect->nFallDetectionCnt = 0;
        FSMMovementStateSet(pstFallDetect, enNextState);
        break;

    case FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_1:
    default:
        //FSMMovementStateSet(pstFallDetect, enNextState);
        break;
    }

    return retVal;
}

EnDetectionRet FSMMovementStateFallCheckStage2(StFallingDetecion *pstFallDetect)
{
    EnDetectionLevel enLatestData = FSMMovementDataReadTailState(pstFallDetect);
    EnDetectionRet retVal = FSP_RETURN_VALUE_MOTION_INIT;
    pstFallDetect->nFallCheckWaitingCnt++;
    switch(enLatestData)
    {
    case MOVEMENT_LEVEL_IDLE:
        // 1차 알림 이후 FALL_DETECTION_SECOND_NOTIFY(10초) 동안 움직임이 관측되지 않으면 낙상 2차 알림
        if(pstFallDetect->nFallCheckWaitingCnt > FALL_DETECTION_SECOND_NOTIFY)
        {
            pstFallDetect->nFallCheckWaitingCnt = 0;
            pstFallDetect->nFallDetectionCnt = 0;
            FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
            retVal = FSP_RETURN_VALUE_MOTION_FELL;
        }
        break;

    case MOVEMENT_LEVEL_NORMAL:
        // 1차 알림 이후 10초 이내에 10sample 이상 움직임이 관측되면 낙상 2차 알림 하지 않음
        pstFallDetect->nFallDetectionCnt++;
        if(pstFallDetect->nFallDetectionCnt > FALL_DETECTION_NOT_NOTIFY)
        {
            pstFallDetect->nFallCheckWaitingCnt = 0;
            pstFallDetect->nFallDetectionCnt = 0;
            FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_NORMAL);
            retVal = FSP_RETURN_VALUE_MOTION_BACK_TO_NORMAL;
        }
        break;

    case MOVEMENT_LEVEL_HIGH:
        // 1차 알림 이후 10초 이내에 10sample 이상 움직임이 관측되면 낙상 2차 알림 하지 않음
        pstFallDetect->nFallDetectionCnt++;
        if(pstFallDetect->nFallDetectionCnt > FALL_DETECTION_NOT_NOTIFY)
        {
            pstFallDetect->nFallCheckWaitingCnt = 0;
            pstFallDetect->nFallDetectionCnt = 0;
            FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
            retVal = FSP_RETURN_VALUE_MOTION_BACK_TO_NORMAL;
        }
        break;

    default:
        //printf("FSMMovementStateFallCheckStage2() undefined data");
        break;
    }

    return retVal;
}

EnDetectionRet FSMMovementFallDetection(StFallingDetecion *pstFallDetect)
{
    EnDetectionRet enRetVal = FSP_RETURN_VALUE_MOTION_INIT;
    short nLoopIndex = 0;
    unsigned short nCurrentIndex = pstFallDetect->listAccData.nTailIndex;

    StPeakDetection stPeak = {0,};
    stPeak = FSMMovementCalPeakData(pstFallDetect);

    // peak 데이터 검색하여 낙상 여부 판단, MOVEMENT_VALUE_1 미만의 값 관측된 직후 MOVEMENT_VALUE_3 이상의 값 관측되면 낙상
    if(stPeak.nItemCount > 0)
    {
        bool bPeakHigh = false;
        for(nLoopIndex = stPeak.nItemCount - 1; nLoopIndex >= 0 ; nLoopIndex--)
        {
            if(stPeak.stData[nLoopIndex].nValue >= MOVEMENT_VALUE_3)
                bPeakHigh = true;

            if(bPeakHigh == true)
            {
                if(stPeak.stData[nLoopIndex].nValue < MOVEMENT_VALUE_1)
                {
                    enRetVal = FSP_RETURN_VALUE_MOTION_FALLING;
                    break;
                }
            }
        }
    }

    return enRetVal;
}

void FSMMovementPeakDataSave(StPeakDetection* pstPeak, unsigned short nSaveIndex, unsigned short nDataIndex, unsigned short nDataValue, EnPeakDef enPeakDef)
{
    pstPeak->stData[nSaveIndex].nIndex = nDataIndex;
    pstPeak->stData[nSaveIndex].nValue = nDataValue;
    pstPeak->stData[nSaveIndex].nCount = 0;
    pstPeak->stData[nSaveIndex].enPeakDef = enPeakDef;
    pstPeak->nItemCount++;

}

void FSMMovementPeakDataReplace(StPeakDetection* pstPeak, unsigned short nSaveIndex, unsigned short nDataIndex, unsigned short nDataValue, EnPeakDef enPeakDef)
{
    pstPeak->stData[nSaveIndex].nIndex = nDataIndex;
    pstPeak->stData[nSaveIndex].nValue = nDataValue;
    pstPeak->stData[nSaveIndex].nCount++;
    pstPeak->stData[nSaveIndex].enPeakDef = enPeakDef;
}

void FSMMovementPeakDataIncreaseCount(StPeakDetection* pstPeak, unsigned short nSaveIndex)
{
    pstPeak->stData[nSaveIndex].nCount++;
}

StPeakDetection FSMMovementCalPeakData(StFallingDetecion *pstFallDetect)
{
    short nLoopIndex = 0;
    unsigned short nCurrentIndex = pstFallDetect->listAccData.nTailIndex;
    StPeakDetection stPeak = {0,};

    // 이전 DETECTION_WAITING_VALUE 개의 데이터 체크하여 peak 데이터 저장
    for(nLoopIndex = DETECTION_WAITING_VALUE; nLoopIndex >= 0; nLoopIndex--)
    {
        if(nLoopIndex <= pstFallDetect->listAccData.nTailIndex)
        {
            nCurrentIndex = pstFallDetect->listAccData.nTailIndex - nLoopIndex;
        }
        else
        {
            nCurrentIndex = pstFallDetect->listAccData.nTailIndex - nLoopIndex + MAX_LIST_LEN;
        }

        if(stPeak.nItemCount == 0) // 최초 데이터
        {
            if(pstFallDetect->listAccData.nDataSVM[nCurrentIndex] < FSM_MOVEMENT_VALUE_IDLE)
            {
                FSMMovementPeakDataSave(&stPeak, stPeak.nItemCount, nCurrentIndex, pstFallDetect->listAccData.nDataSVM[nCurrentIndex], FALL_DETECTION_PEAK_LOW);
            }
            else if(pstFallDetect->listAccData.nDataSVM[nCurrentIndex] >= MOVEMENT_VALUE_3)
            {
                FSMMovementPeakDataSave(&stPeak, stPeak.nItemCount, nCurrentIndex, pstFallDetect->listAccData.nDataSVM[nCurrentIndex], FALL_DETECTION_PEAK_HIGH);
            }

        }
        else if(stPeak.nItemCount == PEAK_DATA_MAX_VALIE) // Max value 초과시 버림
        {
            // printf("Peak data exceed max value\n");
        }
        else
        {
            if(pstFallDetect->listAccData.nDataSVM[nCurrentIndex] < FSM_MOVEMENT_VALUE_IDLE)
            {
                if(stPeak.stData[stPeak.nItemCount - 1].enPeakDef == FALL_DETECTION_PEAK_LOW)
                {
                    if(pstFallDetect->listAccData.nDataSVM[nCurrentIndex] < stPeak.stData[stPeak.nItemCount - 1].nValue)
                    {
                        FSMMovementPeakDataReplace(&stPeak, stPeak.nItemCount - 1, nCurrentIndex, pstFallDetect->listAccData.nDataSVM[nCurrentIndex], FALL_DETECTION_PEAK_LOW);
                    }
                    else
                    {
                        FSMMovementPeakDataIncreaseCount(&stPeak, stPeak.nItemCount - 1);
                    }
                }
                else
                {
                    FSMMovementPeakDataSave(&stPeak, stPeak.nItemCount, nCurrentIndex, pstFallDetect->listAccData.nDataSVM[nCurrentIndex], FALL_DETECTION_PEAK_LOW);
                }
            }
            else
            {
                if(stPeak.stData[stPeak.nItemCount - 1].enPeakDef == FALL_DETECTION_PEAK_HIGH)
                {
                    if(pstFallDetect->listAccData.nDataSVM[nCurrentIndex] > stPeak.stData[stPeak.nItemCount - 1].nValue)
                    {
                        FSMMovementPeakDataReplace(&stPeak, stPeak.nItemCount - 1, nCurrentIndex, pstFallDetect->listAccData.nDataSVM[nCurrentIndex], FALL_DETECTION_PEAK_HIGH);
                    }
                    else
                    {
                        FSMMovementPeakDataIncreaseCount(&stPeak, stPeak.nItemCount - 1);
                    }
                }
                else
                {
                    FSMMovementPeakDataSave(&stPeak, stPeak.nItemCount, nCurrentIndex, pstFallDetect->listAccData.nDataSVM[nCurrentIndex], FALL_DETECTION_PEAK_HIGH);
                }
            }
        } 
    }

    return stPeak;
}

// 최신 데이터 MAX_LIST_LEN 개 유지
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ)
{
    int nCurrentAccX;
    int nCurrentAccY;
    int nCurrentAccZ;

    // nItemCount 가 0 이면
    if(pstFallDetect->listAccData.nItemCount == 0)
    {
        pstFallDetect->listAccData.nTailIndex = 0;
        pstFallDetect->listAccData.nItemCount = 1;
    }
    // nItemCount 가 MAX_LIST_LEN 보다 작으면
    else if(pstFallDetect->listAccData.nItemCount < MAX_LIST_LEN)
    {
        pstFallDetect->listAccData.nTailIndex++;
        pstFallDetect->listAccData.nItemCount++;
    }
    // nItemCount 가 MAX_LIST_LEN 보다 크거나 같으면
    else
    {
        // MAX_LIST_LEN 개 배열 순환하며 저장
        if(pstFallDetect->listAccData.nTailIndex < MAX_LIST_LEN - 1)
        {
            pstFallDetect->listAccData.nTailIndex++;
        }
        else // tail index 가 MAX_LIST_LEN 이면 tail index round 되어 0 부터 다시 시작
        {
            pstFallDetect->listAccData.nTailIndex = 0;
        }
        pstFallDetect->listAccData.nItemCount = MAX_LIST_LEN;
    }

    nCurrentAccX = FSMMovementAccNormalize(fAccDataX);
    nCurrentAccY = FSMMovementAccNormalize(fAccDataY);
    nCurrentAccZ = FSMMovementAccNormalize(fAccDataZ);
    pstFallDetect->listAccData.nDataSVM[pstFallDetect->listAccData.nTailIndex] = FSMMovementAccSvm(pstFallDetect, nCurrentAccX, nCurrentAccY, nCurrentAccZ);
}

unsigned short FSMMovementDataReadItemCount(StFallingDetecion *pstFallDetect)
{
    return pstFallDetect->listAccData.nItemCount;
}

unsigned short FSMMovementDataReadTail(StFallingDetecion *pstFallDetect)
{
    return pstFallDetect->listAccData.nDataSVM[pstFallDetect->listAccData.nTailIndex];
}

EnDetectionLevel FSMMovementDataReadTailState(StFallingDetecion *pstFallDetect)
{
    unsigned short nLatestData = FSMMovementDataReadTail(pstFallDetect);
    EnDetectionLevel enRetVal = MOVEMENT_LEVEL_IDLE;

    if(nLatestData <= MOVEMENT_VALUE_1)
    {
        enRetVal = MOVEMENT_LEVEL_NORMAL;
    }
    else if(nLatestData > MOVEMENT_VALUE_1 && nLatestData <= MOVEMENT_VALUE_2)
    {
        enRetVal = MOVEMENT_LEVEL_IDLE;
    }
    else if(nLatestData > MOVEMENT_VALUE_2 && nLatestData <= MOVEMENT_VALUE_3)
    {
        enRetVal = MOVEMENT_LEVEL_NORMAL;
    }
    else if(nLatestData > MOVEMENT_VALUE_3)
    {
        enRetVal = MOVEMENT_LEVEL_HIGH;
    }
    else
    {
        //printf("FSMMovementDataReadTailState() undefined data : nLatestData[%d] \n", nLatestData);
    }

    return enRetVal;
}

// low 데이터에 FALL_DETECTION_DATA_NORMALIZE_VALUE 만큼 정규화 진행
// float 데이터를 정수형으로 변환
int FSMMovementAccNormalize(float fAccData)
{
    return (int)(fAccData * FALL_DETECTION_DATA_NORMALIZE_VALUE);
}

// 움직임 정량화 변수 SVM(Sum Vector Magnitude)
unsigned short FSMMovementAccSvm(StFallingDetecion *pstFallDetect, int nCurrentAccX, int nCurrentAccY, int nCurrentAccZ)
{
    float fCalX = 0;
    float fCalY = 0;
    float fCalZ = 0;
    int nCalResult = 0;

    fCalX = pow(nCurrentAccX, 2);
    fCalY = pow(nCurrentAccY, 2);
    fCalZ = pow(nCurrentAccZ, 2);
    nCalResult = sqrt(fCalX + fCalY + fCalZ); // 실수형 데이터를 정수형 데이터로 type casting 하였다. 소수점 이하 버림.
    //nCalResult = abs(nCalResult - 986);

    return (unsigned short)nCalResult;
}

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  파일명: FallingDetectionFSM.c
 *
 *  설명: 낙상 감지 PC 데모 - Finite Saate Machine
 *      
 * 
 *  작성자:
 *
 *
 *  날짜:
 *      2018/4/16
 */
