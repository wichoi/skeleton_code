#ifndef __FALLING_DETECTION_FSM_HEADER_H__
#define __FALLING_DETECTION_FSM_HEADER_H__

// ������ ���� ����
typedef enum FSM_FALLING_DETECTION_STATE_DEFINE
{
    FSM_STATE_MOVEMENT_INIT = -1,           // 100 ���� �����Ͱ� ���϶����� ��� (10msec sampling ����)
    FSM_STATE_MOVEMENT_IDLE = 0,            // �������� ���� ����
    FSM_STATE_MOVEMENT_LOW = 1,             // ���� ������
    FSM_STATE_MOVEMENT_MIDDLE = 2,          // �ϻ� ��Ȱ ������
    FSM_STATE_MOVEMENT_HIGH = 3,            // ū ������
    FSM_STATE_MOVEMENT_FALL_CHECK = 4,      // ū ������ ���� ���� ���� üũ
}EnDetectionState;

// DSVMMAV ���� 4�ܰ� ����ȭ
typedef enum FSM_FALLING_DETECTION_MOVEMENT_LEVEL
{
    MOVEMENT_LEVEL_1 = 50, // 1,
    MOVEMENT_LEVEL_2 = 100, // 40,
    MOVEMENT_LEVEL_3 = 1000, // 150,
}EnDetectionLevel;

// return value
typedef enum FSM_FALLING_DETECTION_RETURN_VALUE
{
    FSP_RETURN_VALUE_MOTION_INIT = -1,      // �ʱ�ȭ ��
    //FSP_RETURN_VALUE_MOTION_IDLE = 0,      // ������ ����
    //FSP_RETURN_VALUE_MOTION_LOW = 1,       // ���� ������
    //FSP_RETURN_VALUE_MOTION_MIDDLE = 2,    // �ϻ� ��Ȱ ������
    FSP_RETURN_VALUE_MOTION_HIGH = 3,       // ū ������
    FSP_RETURN_VALUE_MOTION_FALLING = 4,    // ����
}EnDetectionRet;

#define FALL_DETECTION_DATA_NORMALIZE_VALUE 1000 // 100        // low �����Ϳ� * 100 ��ŭ ����ȭ ����
#define FALL_DETECTION_DATA_MOVING_AVERAGE_FILTER 5 // 50    // 50�� ���� moving average filter

// 10msec sampling ���� ������ �ʱ�ȭ
#define MAX_LIST_LEN 500              // �ֽ� 300�� ������ ����
#define DETECTION_WAITING_VALUE 13 // 150   // �ּ� 150���� �����Ͱ� ����ɶ����� ���
#define FALL_CHECK_WAITING_VALUE 10 // 100  // ū ������ ���� 100���� ������ ����
#define FALL_DECECTION_VALIE 2 // 5        // ū ������ ���� ���� �������� ���� 5ȸ �̻� ���� �Ǹ� ����

typedef struct Falling__node
{
    //int nHeadIndex;             // oldest data
    int nTailIndex;             // newly data
    int nItemCount;             // total item count
    int nDataAccX[MAX_LIST_LEN]; // ���ӵ� X ����ȭ ������ (fAccDataX * 100, �Ҽ��� ���� ����)
    int nDataAccY[MAX_LIST_LEN]; // ���ӵ� Y ����ȭ ������ (fAccDataY * 100, �Ҽ��� ���� ����)
    int nDataAccZ[MAX_LIST_LEN]; // ���ӵ� Z ����ȭ ������ (fAccDataZ * 100, �Ҽ��� ���� ����)
    int nDataDsvm[MAX_LIST_LEN]; // DSVM(Differential Sum Vector Magnitude)
    int nDataDsvmMav[MAX_LIST_LEN]; // DSVM + Moving average filter
    int nDataSvm[MAX_LIST_LEN];
    int nDataSvmMav[MAX_LIST_LEN];

} ListNode;

typedef struct FSMFallingDetectionStruct
{
    EnDetectionState enDetectState;     // ������ ����
    ListNode listAccData;               // ������ ������
    int nFallCheckWaitingCnt;           // ū ������ ���� ������ ���� üũ
    int nFallDetectionCnt;              // ���� ������ üũ ����
}StFallingDetecion;

// ������ ���� �ӽ� �ʱ�ȭ
void FSMFallingDetectionInit(StFallingDetecion *pstFallDetect);

// ������ ���� set/get �Լ�
void FSMMovementStateSet(StFallingDetecion *pstFallDetect, EnDetectionState enState);
EnDetectionState FSMMovementStateGet(StFallingDetecion *pstFallDetect);

// ������ ���� �ӽ�
EnDetectionRet FSMMovementState(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ);
void FSMMovementStateInit(StFallingDetecion *pstFallDetect);
void FSMMovementStateIdle(StFallingDetecion *pstFallDetect);
void FSMMovementStateLow(StFallingDetecion *pstFallDetect);
void FSMMovementStateMiddle(StFallingDetecion *pstFallDetect);
void FSMMovementStateHigh(StFallingDetecion *pstFallDetect);
EnDetectionRet FSMMovementStateFallCheck(StFallingDetecion *pstFallDetect);

// ������ �� list(�ڷ���) �Լ�
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ); // ������ data list�� item �߰�
int FSMMovementDataReadItemCount(StFallingDetecion *pstFallDetect); // ������ data list�� ����� item ����
int FSMMovementDataReadTail(StFallingDetecion *pstFallDetect); // ������ data list�� ����� �ֽ� ������
EnDetectionRet FSMMovementDataJumpCheck(StFallingDetecion *pstFallDetect, EnDetectionRet prevRetVal);
int FSMMovementDataNormalize(float fAccData);
int FSMMovementDataSvm(StFallingDetecion *pstFallDetect);
int FSMMovementDataDsvm(StFallingDetecion *pstFallDetect);
int FSMMovementMovingAverageFilter(StFallingDetecion *pstFallDetect);

#endif /* __FALLING_DETECTION_FSM_HEADER_H__ */

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  ���ϸ�: FallingDetectionFSM.h
 *
 *  ����: ���� ���� PC ���� - Finit State Machine
 *      
 * 
 *  �ۼ���:
 *
 *
 *  ��¥:
 *      2018/4/16
 */
