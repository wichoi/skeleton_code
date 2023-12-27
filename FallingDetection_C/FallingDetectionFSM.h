#ifndef __FALLING_DETECTION_FSM_HEADER_H__
#define __FALLING_DETECTION_FSM_HEADER_H__

// ������ ���� ����
typedef enum FSM_FALLING_DETECTION_STATE_DEFINE
{
    FSM_STATE_MOVEMENT_INIT = -1,                   // DETECTION_WAITING_VALUE ���� �����Ͱ� ���϶����� ���
    FSM_STATE_MOVEMENT_IDLE = 0,                    // �������� ���� ����
    FSM_STATE_MOVEMENT_NORMAL = 2,                  // �ϻ� ��Ȱ ������
    FSM_STATE_MOVEMENT_HIGH = 3,                    // ū ������
    FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_1 = 4,      // ū ������ ���� ���� ���� üũ (ù��° �˸�)
    FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_2 = 5,      // ū ������ ���� ���� ���� üũ (10�� ���� �ι�° �˸�)
}EnDetectionState;

// nDataSvm �� ���� 3�ܰ�(������ ����, �ϻ���Ȱ ������, ū ������)
typedef enum FSM_FALLING_DETECTION_MOVEMENT_VALUE
{
    MOVEMENT_VALUE_1 = 900,
    MOVEMENT_VALUE_2 = 1100,
    MOVEMENT_VALUE_3 = 3800,
}EnDetectionValue;

#define FSM_MOVEMENT_VALUE_IDLE 1000

typedef enum FSM_FALLING_DETECTION_MOVEMENT_LEVEL
{
    MOVEMENT_LEVEL_IDLE = 0,                        // ������ ����
    MOVEMENT_LEVEL_NORMAL = 1,                      // �ϻ��Ȱ ������
    MOVEMENT_LEVEL_HIGH = 2,                        // ū ������
}EnDetectionLevel;

// return value
typedef enum FSM_FALLING_DETECTION_RETURN_VALUE
{
    FSP_RETURN_VALUE_MOTION_INIT = 0,               // �ʱ�ȭ ��
    //FSP_RETURN_VALUE_MOTION_IDLE = 1,              // ������ ����
    //FSP_RETURN_VALUE_MOTION_NORMAL = 2,            // �ϻ� ��Ȱ ������
    FSP_RETURN_VALUE_MOTION_CONTINUES = 3,          // �������� ������
    FSP_RETURN_VALUE_MOTION_HIGH = 4,               // ū ������
    FSP_RETURN_VALUE_MOTION_FALLING = 5,            // ����
    FSP_RETURN_VALUE_MOTION_FELL = 6,               // ���� (2�� �˸�)
    FSP_RETURN_VALUE_MOTION_BACK_TO_NORMAL = 7,     // ���� ���� �ϻ� ��Ȱ �������� ������
}EnDetectionRet;

#define FALL_DETECTION_DATA_NORMALIZE_VALUE 1000        // accelerometer low �����Ϳ� * 1000 ��ŭ ����ȭ ����

// 100msce ����
//#define MAX_LIST_LEN 100                    // �ֽ� 100�� ������ ���� (�� 10�� �з�)
//#define DETECTION_WAITING_VALUE 30          // �ּ� 30���� �����Ͱ� ����ɶ����� ���
//#define FALL_CHECK_WAITING_VALUE 15         // ū ������ ���� 15���� ������ ����
//#define FALL_DETECTION_VALIE 4              // ū ������ ���� ���� �������� ���� 4ȸ �ʰ� ���� �Ǹ� ����
//#define FALL_DETECTION_SECOND_NOTIFY 100    // 1�� �˸� ���� 10�ʰ� �������� ������ 2�� �˸�
//#define FALL_DETECTION_NOT_NOTIFY 10        // 1�� �˸� ���� 10���̳� 10sample �̻� �������� �����Ǹ� 2�� �˸� �������� ����

// 20msec ���� 
#define MAX_LIST_LEN 300                    // �ֽ� 400�� ������ ���� (�� 8�� �з�)
#define DETECTION_WAITING_VALUE 150         // �ּ� 150����(3��) �����Ͱ� ����ɶ����� ���
#define FALL_CHECK_WAITING_VALUE 75         // ū ������ ���� 75����(1.5��) ������ ����
#define FALL_DETECTION_VALIE 20             // ū ������ ���� idle �������� 10ȸ(0.4��) �ʰ� �����Ǹ� ����
#define FALL_DETECTION_SECOND_NOTIFY 500    // 1�� �˸� ���� 500��(10��) �������� ������ 2�� �˸�
#define FALL_DETECTION_NOT_NOTIFY 50        // 1�� �˸� ���� 10���̳� 50�� �̻� �������� �����Ǹ� 2�� �˸� �������� ����

typedef enum Falling_Peak_Define
{
    FALL_DETECTION_PEAK_HIGH = 1,
    FALL_DETECTION_PEAK_LOW = 2
}EnPeakDef;

typedef struct Peak_Data                    // FallCheck ���¿��� ���� 15���� ������ üũ�Ͽ� low peak, high peak �����͸� ã�´�
{
    unsigned short nIndex;                  // peak ����Ƽ ����  ������  index
    unsigned short nValue;                  // peak �������� ��
    unsigned short nCount;                  // peak�� �������� ������ ��� ���� index ����  ��� �����Ͱ� �����Ǵ��� ���� ���� ����
    EnPeakDef enPeakDef;
} StPeakData;

#define PEAK_DATA_MAX_VALIE 20              // �ִ� 20���� peak data ����
typedef struct Falling_Peak_Detection
{
    StPeakData stData[PEAK_DATA_MAX_VALIE];
    unsigned char nItemCount;
} StPeakDetection;

typedef struct Falling__node
{
    //unsigned short nHeadIndex;                 // oldest data
    unsigned short nTailIndex;                  // newly data
    unsigned short nItemCount;                  // total item count
    unsigned short nDataSVM[MAX_LIST_LEN];      // SVM(Sum Vector Magnitude)
} ListNode;

typedef struct FSMFallingDetectionStruct
{
    EnDetectionState enDetectCurrentState;      // ������ ����
    ListNode listAccData;                       // ������ ������
    unsigned short nFallCheckWaitingCnt;        // fall check state ���¿��� FALL_CHECK_WAITING_VALUE ���� ����ϸ� ������ �� �����Ͽ� ���󿩺� �Ǵ�
    unsigned short nFallDetectionCnt;           // fall check state ���¿��� FALL_DETECTION_VALIE ���� ���� ������ �����Ǹ� ���� �˸�
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
void FSMMovementStateNormal(StFallingDetecion *pstFallDetect);
void FSMMovementStateHigh(StFallingDetecion *pstFallDetect);
EnDetectionRet FSMMovementStateFallCheckStage1(StFallingDetecion *pstFallDetect);
EnDetectionRet FSMMovementStateFallCheckStage2(StFallingDetecion *pstFallDetect);

// ���� �Ǻ� �Լ�
EnDetectionRet FSMMovementFallDetection(StFallingDetecion *pstFallDetect);

// peak ��� �Լ�
void FSMMovementPeakDataSave(StPeakDetection* pstPeak, unsigned short nSaveIndex, unsigned short nDataIndex, unsigned short nDataValue, EnPeakDef enPeakDef);
void FSMMovementPeakDataReplace(StPeakDetection* pstPeak, unsigned short nSaveIndex, unsigned short nDataIndex, unsigned short nDataValue, EnPeakDef enPeakDef);
void FSMMovementPeakDataIncreaseCount(StPeakDetection* pstPeak, unsigned short nSaveIndex);
StPeakDetection FSMMovementCalPeakData(StFallingDetecion *pstFallDetect);

// ������ �� list(�ڷ���) �Լ�
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ); // ������ data list�� item �߰�
unsigned short FSMMovementDataReadItemCount(StFallingDetecion *pstFallDetect); // ������ data list�� ����� item ����
unsigned short FSMMovementDataReadTail(StFallingDetecion *pstFallDetect); // ������ data list�� ����� �ֽ� ������
EnDetectionLevel FSMMovementDataReadTailState(StFallingDetecion *pstFallDetect);

// 3�� ������ ���� �Լ�
int FSMMovementAccNormalize(float fAccData);
unsigned short FSMMovementAccSvm(StFallingDetecion *pstFallDetect, int nCurrentAccX, int nCurrentAccY, int nCurrentAccZ);

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
