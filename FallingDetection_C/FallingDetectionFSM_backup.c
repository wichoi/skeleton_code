#include <stdio.h>
#include <string.h>
#include <math.h>

#include "FallingDetectionFSM.h"

void FSMFallingDetectionInit(StFallingDetecion *pstFallDetect)
{
    pstFallDetect->enDetectState = FSM_STATE_MOVEMENT_INIT;
    pstFallDetect->nFallCheckWaitingCnt = 0;
    pstFallDetect->nFallDetectionCnt = 0;

    //pstFallDetect->listAccData.nHeadIndex = 0;
    pstFallDetect->listAccData.nTailIndex = 0;
    pstFallDetect->listAccData.nItemCount = 0;
    memset(pstFallDetect->listAccData.nDataAccX, 0, sizeof(int) * MAX_LIST_LEN);
    memset(pstFallDetect->listAccData.nDataAccY, 0, sizeof(int) * MAX_LIST_LEN);
    memset(pstFallDetect->listAccData.nDataAccZ, 0, sizeof(int) * MAX_LIST_LEN);
    memset(pstFallDetect->listAccData.nDataDsvm, 0, sizeof(int) * MAX_LIST_LEN);
    memset(pstFallDetect->listAccData.nDataDsvmMav, 0, sizeof(int) * MAX_LIST_LEN);
}

void FSMMovementStateSet(StFallingDetecion *pstFallDetect, EnDetectionState enState)
{
    pstFallDetect->enDetectState = enState;
}

EnDetectionState FSMMovementStateGet(StFallingDetecion *pstFallDetect)
{
    return pstFallDetect->enDetectState;
}

EnDetectionRet FSMMovementState(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ)
{
    EnDetectionRet enRetVal = FSP_RETURN_VALUE_MOTION_INIT;

    FSMMovementDataAppend(pstFallDetect, fAccDataX, fAccDataY, fAccDataZ);

    switch(pstFallDetect->enDetectState)
    {
    case FSM_STATE_MOVEMENT_INIT:
        FSMMovementStateInit(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_IDLE:
        FSMMovementStateIdle(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_LOW:
        FSMMovementStateLow(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_MIDDLE:
        FSMMovementStateMiddle(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_HIGH:
        FSMMovementStateHigh(pstFallDetect);
        break;

    case FSM_STATE_MOVEMENT_FALL_CHECK:
        enRetVal = FSMMovementStateFallCheck(pstFallDetect);
        break;

    default:
        printf("error, FSMMovementState() unknown state\n");
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
    int nLatestData = FSMMovementDataReadTail(pstFallDetect);
    if(nLatestData <= MOVEMENT_LEVEL_1)
    {
        // 오랜 시간 idle 상태가 관측되면 "움직임이 없습니다. 확인이 필요합니다." 알림
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
    }
    else if(nLatestData > MOVEMENT_LEVEL_1 && nLatestData <= MOVEMENT_LEVEL_2)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_LOW);
    }
    else if(nLatestData > MOVEMENT_LEVEL_2 && nLatestData <= MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_MIDDLE);
    }
    else if(nLatestData > MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
    }
    else
    {
        printf("FSMMovementStateInit() undefined data : nLatestData[%f] \n", nLatestData);
    }
}

void FSMMovementStateLow(StFallingDetecion *pstFallDetect)
{
    int nLatestData = FSMMovementDataReadTail(pstFallDetect);
    if(nLatestData <= MOVEMENT_LEVEL_1)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
    }
    else if(nLatestData > MOVEMENT_LEVEL_1 && nLatestData <= MOVEMENT_LEVEL_2)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_LOW);
    }
    else if(nLatestData > MOVEMENT_LEVEL_2 && nLatestData <= MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_MIDDLE);
    }
    else if(nLatestData > MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
    }
    else
    {
        printf("FSMMovementStateLow() undefined data : nLatestData[%f] \n", nLatestData);
    }

}

void FSMMovementStateMiddle(StFallingDetecion *pstFallDetect)
{
    int nLatestData = FSMMovementDataReadTail(pstFallDetect);
    if(nLatestData <= MOVEMENT_LEVEL_1)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
    }
    else if(nLatestData > MOVEMENT_LEVEL_1 && nLatestData <= MOVEMENT_LEVEL_2)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_LOW);
    }
    else if(nLatestData > MOVEMENT_LEVEL_2 && nLatestData <= MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_MIDDLE);
    }
    else if(nLatestData > MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
    }
    else
    {
        printf("FSMMovementStateMiddle() undefined data : nLatestData[%f] \n", nLatestData);
    }

    // 필요할 경우 "움직임이 감지되었습니다." 알림
}

void FSMMovementStateHigh(StFallingDetecion *pstFallDetect)
{
    int nLatestData = FSMMovementDataReadTail(pstFallDetect);

    // high 상태에선 오직 fall check 상태로만 전이한다.
    if(nLatestData > MOVEMENT_LEVEL_3)
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_HIGH);
    }
    else
    {
        FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_FALL_CHECK);
    }
}

EnDetectionRet FSMMovementStateFallCheck(StFallingDetecion *pstFallDetect)
{
    int nLatestData = FSMMovementDataReadTail(pstFallDetect);
    EnDetectionState enNextState = FSM_STATE_MOVEMENT_FALL_CHECK;
    EnDetectionRet retVal = FSP_RETURN_VALUE_MOTION_INIT;

    if(nLatestData <= MOVEMENT_LEVEL_1)
    {
        pstFallDetect->nFallCheckWaitingCnt++;
        pstFallDetect->nFallDetectionCnt++;
        if(pstFallDetect->nFallDetectionCnt > FALL_DECECTION_VALIE)
        {
            enNextState = FSM_STATE_MOVEMENT_IDLE;
            retVal = FSP_RETURN_VALUE_MOTION_FALLING; // 낙상이 감지되었습니다
        }
    }
    else if(nLatestData > MOVEMENT_LEVEL_1 && nLatestData <= MOVEMENT_LEVEL_2)
    {
        pstFallDetect->nFallCheckWaitingCnt++;
        pstFallDetect->nFallDetectionCnt++;
        if(pstFallDetect->nFallDetectionCnt > FALL_DECECTION_VALIE)
        {
            enNextState = FSM_STATE_MOVEMENT_LOW;
            retVal = FSP_RETURN_VALUE_MOTION_FALLING; // 낙상이 감지되었습니다
        }
    }
    else if(nLatestData > MOVEMENT_LEVEL_2 && nLatestData <= MOVEMENT_LEVEL_3)
    {
        // 큰 움직임 이후 100 sampling 동안 일상 움직임 관측 되면 일상 움직임 상태로 전이
        pstFallDetect->nFallCheckWaitingCnt++;
        pstFallDetect->nFallDetectionCnt = 0;
        if(pstFallDetect->nFallCheckWaitingCnt > FALL_CHECK_WAITING_VALUE)
        {
            enNextState = FSM_STATE_MOVEMENT_MIDDLE;
            retVal = FSP_RETURN_VALUE_MOTION_HIGH; // 큰 움직임이 감지되었습니다
        }
    }
    else if(nLatestData > MOVEMENT_LEVEL_3)
    {
        enNextState = FSM_STATE_MOVEMENT_HIGH;
    }
    else
    {
        printf("FSMMovementStateFallCheck() undefined data : nLatestData[%f] \n", nLatestData);
    }

    if(enNextState != FSM_STATE_MOVEMENT_FALL_CHECK)
    {
        //if(enNextState != FSM_STATE_MOVEMENT_HIGH)
        //    retVal = FSMMovementDataJumpCheck(pstFallDetect, retVal);
        pstFallDetect->nFallCheckWaitingCnt = 0;
        pstFallDetect->nFallDetectionCnt = 0;
        FSMMovementStateSet(pstFallDetect, enNextState);
    }

    return retVal; 
}

// 최신 데이터 300개 유지
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ)
{
    // nItemCount 가 0 이면
    if(pstFallDetect->listAccData.nItemCount == 0)
    {
        pstFallDetect->listAccData.nTailIndex = 0;
        pstFallDetect->listAccData.nItemCount = 1;
    }
    // nItemCount 가 300 보다 작으면
    else if(pstFallDetect->listAccData.nItemCount < MAX_LIST_LEN)
    {
        pstFallDetect->listAccData.nTailIndex++;
        pstFallDetect->listAccData.nItemCount++;
    }
    // nItemCount 가 300 보다 크거나 같으면
    else
    {
        // 300개 배열 순환하며 저장
        if(pstFallDetect->listAccData.nTailIndex < MAX_LIST_LEN - 1)
        {
            pstFallDetect->listAccData.nTailIndex++;
        }
        else // tail index 가 299 이면 tail index round 되어 0 부터 다시 시작
        {
            pstFallDetect->listAccData.nTailIndex = 0;
        }
        pstFallDetect->listAccData.nItemCount = MAX_LIST_LEN;
    }

    pstFallDetect->listAccData.nDataAccX[pstFallDetect->listAccData.nTailIndex] = FSMMovementDataNormalize(fAccDataX);
    pstFallDetect->listAccData.nDataAccY[pstFallDetect->listAccData.nTailIndex] = FSMMovementDataNormalize(fAccDataY);
    pstFallDetect->listAccData.nDataAccZ[pstFallDetect->listAccData.nTailIndex] = FSMMovementDataNormalize(fAccDataZ);
    pstFallDetect->listAccData.nDataDsvm[pstFallDetect->listAccData.nTailIndex] = FSMMovementDataDsvm(pstFallDetect);
    pstFallDetect->listAccData.nDataSvm[pstFallDetect->listAccData.nTailIndex] = FSMMovementDataSvm(pstFallDetect);
    pstFallDetect->listAccData.nDataDsvmMav[pstFallDetect->listAccData.nTailIndex] = FSMMovementMovingAverageFilter(pstFallDetect);
}

int FSMMovementDataReadItemCount(StFallingDetecion *pstFallDetect)
{
    return pstFallDetect->listAccData.nItemCount;
}

int FSMMovementDataReadTail(StFallingDetecion *pstFallDetect)
{
    return pstFallDetect->listAccData.nDataDsvmMav[pstFallDetect->listAccData.nTailIndex];
}

// 이전 130 sample 을 체크하여 80보다 큰 sample 이 90 개 이상 존재하면 점프
EnDetectionRet FSMMovementDataJumpCheck(StFallingDetecion *pstFallDetect, EnDetectionRet prevRetVal)
{
    EnDetectionRet retVal = prevRetVal;
    int nLoopIndex = 0;
    int nCurrentIndex = pstFallDetect->listAccData.nTailIndex;
    int nJumpCheckCnt = 0;

    for(nLoopIndex = 0; nLoopIndex < 130; nLoopIndex++)
    {
        if(nLoopIndex <= pstFallDetect->listAccData.nTailIndex)
        {
            nCurrentIndex = pstFallDetect->listAccData.nTailIndex - nLoopIndex;
        }
        else
        {
            nCurrentIndex = pstFallDetect->listAccData.nTailIndex - nLoopIndex + MAX_LIST_LEN;
        }

        if(pstFallDetect->listAccData.nDataDsvmMav[nCurrentIndex] > 80)
            nJumpCheckCnt++;
    }

    if (nJumpCheckCnt > 90)
        retVal = FSP_RETURN_VALUE_MOTION_HIGH;

    return retVal;
}

// low 데이터에 * FALL_DETECTION_DATA_NORMALIZE_VALUE 만큼 정규화 진행
// float 데이터를 정수형으로 변환
int FSMMovementDataNormalize(float fAccData)
{
    return (int)(fAccData * FALL_DETECTION_DATA_NORMALIZE_VALUE);
}

int FSMMovementDataSvm(StFallingDetecion *pstFallDetect)
{
    int nCalX = 0;
    int nCalY = 0;
    int nCalZ = 0;
    float fCalResult = 0;
    int nCurrIndex = pstFallDetect->listAccData.nTailIndex;
    int nCalResult = 0;

    nCalX = powf(pstFallDetect->listAccData.nDataAccX[nCurrIndex], 2);
    nCalY = powf(pstFallDetect->listAccData.nDataAccY[nCurrIndex], 2);
    nCalZ = powf(pstFallDetect->listAccData.nDataAccZ[nCurrIndex], 2);
    fCalResult = sqrtf(nCalX + nCalY + nCalZ);
    nCalResult = fCalResult;
    nCalResult = abs(nCalResult - 986);

    return (int)fCalResult;
}

// 움직임 정량화 변수 순간 변위량 DSVM(Differential Sum Vector Magnitude)
int FSMMovementDataDsvm(StFallingDetecion *pstFallDetect)
{
    int nCalX = 0;
    int nCalY = 0;
    int nCalZ = 0;
    float fCalResult = 0;
    int nCurrIndex = pstFallDetect->listAccData.nTailIndex;
    int nPrevIndex = pstFallDetect->listAccData.nTailIndex - 1;

    if(pstFallDetect->listAccData.nItemCount > 1)
    {
        if(nCurrIndex == 0)
            nPrevIndex = MAX_LIST_LEN -1;

        nCalX = powf(pstFallDetect->listAccData.nDataAccX[nCurrIndex] - pstFallDetect->listAccData.nDataAccX[nPrevIndex], 2);
        nCalY = powf(pstFallDetect->listAccData.nDataAccY[nCurrIndex] - pstFallDetect->listAccData.nDataAccY[nPrevIndex], 2);
        nCalZ = powf(pstFallDetect->listAccData.nDataAccZ[nCurrIndex] - pstFallDetect->listAccData.nDataAccZ[nPrevIndex], 2);
        fCalResult = sqrtf(nCalX + nCalY + nCalZ);
    }

    return (int)fCalResult; // 실수형 데이터를 정수형 데이터로 type casting 하였다. 소수점 이하 버림.
}

// FALL_DETECTION_DATA_MOVING_AVERAGE_FILTER 단위 moving average filter
int FSMMovementMovingAverageFilter(StFallingDetecion *pstFallDetect)
{
    int nItemCount = pstFallDetect->listAccData.nItemCount;
    int nCurrentIndex = pstFallDetect->listAccData.nTailIndex;
    int nLoopIndex = 0;
    int nSum = 0;
    int nSumSVM = 0;

    if (nItemCount >= FALL_DETECTION_DATA_MOVING_AVERAGE_FILTER)
    {
        nItemCount = FALL_DETECTION_DATA_MOVING_AVERAGE_FILTER;
    }

    for (nLoopIndex; nLoopIndex < nItemCount; nLoopIndex++)
    {
        if(nLoopIndex <= pstFallDetect->listAccData.nTailIndex)
        {
            nCurrentIndex = pstFallDetect->listAccData.nTailIndex - nLoopIndex;
        }
        else
        {
            nCurrentIndex = pstFallDetect->listAccData.nTailIndex - nLoopIndex + MAX_LIST_LEN;
        }
        nSum += pstFallDetect->listAccData.nDataDsvm[nCurrentIndex];
        nSumSVM += pstFallDetect->listAccData.nDataSvm[nCurrentIndex];
    }

    pstFallDetect->listAccData.nDataSvmMav[pstFallDetect->listAccData.nTailIndex] = (int)(nSumSVM / nItemCount);

    return (int)(nSum / nItemCount);
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
