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
        // ���� �ð� idle ���°� �����Ǹ� "�������� �����ϴ�. Ȯ���� �ʿ��մϴ�." �˸�
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
    // high ���¿��� ���� fall check ���·θ� �����Ѵ�.
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

    // ū ������ ���� FALL_CHECK_WAITING_VALUE ���� �����͸� �����ϸ� ����Ѵ�.
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
            enNextState = FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_2; // 2�� �˸� �ó�����
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
        // 1�� �˸� ���� FALL_DETECTION_SECOND_NOTIFY(10��) ���� �������� �������� ������ ���� 2�� �˸�
        if(pstFallDetect->nFallCheckWaitingCnt > FALL_DETECTION_SECOND_NOTIFY)
        {
            pstFallDetect->nFallCheckWaitingCnt = 0;
            pstFallDetect->nFallDetectionCnt = 0;
            FSMMovementStateSet(pstFallDetect, FSM_STATE_MOVEMENT_IDLE);
            retVal = FSP_RETURN_VALUE_MOTION_FELL;
        }
        break;

    case MOVEMENT_LEVEL_NORMAL:
        // 1�� �˸� ���� 10�� �̳��� 10sample �̻� �������� �����Ǹ� ���� 2�� �˸� ���� ����
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
        // 1�� �˸� ���� 10�� �̳��� 10sample �̻� �������� �����Ǹ� ���� 2�� �˸� ���� ����
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

    // peak ������ �˻��Ͽ� ���� ���� �Ǵ�, MOVEMENT_VALUE_1 �̸��� �� ������ ���� MOVEMENT_VALUE_3 �̻��� �� �����Ǹ� ����
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

    // ���� DETECTION_WAITING_VALUE ���� ������ üũ�Ͽ� peak ������ ����
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

        if(stPeak.nItemCount == 0) // ���� ������
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
        else if(stPeak.nItemCount == PEAK_DATA_MAX_VALIE) // Max value �ʰ��� ����
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

// �ֽ� ������ MAX_LIST_LEN �� ����
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ)
{
    int nCurrentAccX;
    int nCurrentAccY;
    int nCurrentAccZ;

    // nItemCount �� 0 �̸�
    if(pstFallDetect->listAccData.nItemCount == 0)
    {
        pstFallDetect->listAccData.nTailIndex = 0;
        pstFallDetect->listAccData.nItemCount = 1;
    }
    // nItemCount �� MAX_LIST_LEN ���� ������
    else if(pstFallDetect->listAccData.nItemCount < MAX_LIST_LEN)
    {
        pstFallDetect->listAccData.nTailIndex++;
        pstFallDetect->listAccData.nItemCount++;
    }
    // nItemCount �� MAX_LIST_LEN ���� ũ�ų� ������
    else
    {
        // MAX_LIST_LEN �� �迭 ��ȯ�ϸ� ����
        if(pstFallDetect->listAccData.nTailIndex < MAX_LIST_LEN - 1)
        {
            pstFallDetect->listAccData.nTailIndex++;
        }
        else // tail index �� MAX_LIST_LEN �̸� tail index round �Ǿ� 0 ���� �ٽ� ����
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

// low �����Ϳ� FALL_DETECTION_DATA_NORMALIZE_VALUE ��ŭ ����ȭ ����
// float �����͸� ���������� ��ȯ
int FSMMovementAccNormalize(float fAccData)
{
    return (int)(fAccData * FALL_DETECTION_DATA_NORMALIZE_VALUE);
}

// ������ ����ȭ ���� SVM(Sum Vector Magnitude)
unsigned short FSMMovementAccSvm(StFallingDetecion *pstFallDetect, int nCurrentAccX, int nCurrentAccY, int nCurrentAccZ)
{
    float fCalX = 0;
    float fCalY = 0;
    float fCalZ = 0;
    int nCalResult = 0;

    fCalX = pow(nCurrentAccX, 2);
    fCalY = pow(nCurrentAccY, 2);
    fCalZ = pow(nCurrentAccZ, 2);
    nCalResult = sqrt(fCalX + fCalY + fCalZ); // �Ǽ��� �����͸� ������ �����ͷ� type casting �Ͽ���. �Ҽ��� ���� ����.
    //nCalResult = abs(nCalResult - 986);

    return (unsigned short)nCalResult;
}

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  ���ϸ�: FallingDetectionFSM.c
 *
 *  ����: ���� ���� PC ���� - Finite Saate Machine
 *      
 * 
 *  �ۼ���:
 *
 *
 *  ��¥:
 *      2018/4/16
 */
