#ifndef __FALLING_DETECTION_FSM_HEADER_H__
#define __FALLING_DETECTION_FSM_HEADER_H__

// 움직임 상태 정의
typedef enum FSM_FALLING_DETECTION_STATE_DEFINE
{
    FSM_STATE_MOVEMENT_INIT = -1,                   // DETECTION_WAITING_VALUE 개의 데이터가 쌓일때까지 대기
    FSM_STATE_MOVEMENT_IDLE = 0,                    // 움직임이 없는 상태
    FSM_STATE_MOVEMENT_NORMAL = 2,                  // 일상 생활 움직임
    FSM_STATE_MOVEMENT_HIGH = 3,                    // 큰 움직임
    FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_1 = 4,      // 큰 움직임 이후 낙상 여부 체크 (첫번째 알림)
    FSM_STATE_MOVEMENT_FALL_CHECK_STAGE_2 = 5,      // 큰 움직임 이후 낙상 여부 체크 (10초 이후 두번째 알림)
}EnDetectionState;

// nDataSvm 값 기준 3단계(움직임 없음, 일생생활 움직임, 큰 움직임)
typedef enum FSM_FALLING_DETECTION_MOVEMENT_VALUE
{
    MOVEMENT_VALUE_1 = 900,
    MOVEMENT_VALUE_2 = 1100,
    MOVEMENT_VALUE_3 = 3800,
}EnDetectionValue;

#define FSM_MOVEMENT_VALUE_IDLE 1000

typedef enum FSM_FALLING_DETECTION_MOVEMENT_LEVEL
{
    MOVEMENT_LEVEL_IDLE = 0,                        // 움직임 없음
    MOVEMENT_LEVEL_NORMAL = 1,                      // 일상생활 움직임
    MOVEMENT_LEVEL_HIGH = 2,                        // 큰 움직임
}EnDetectionLevel;

// return value
typedef enum FSM_FALLING_DETECTION_RETURN_VALUE
{
    FSP_RETURN_VALUE_MOTION_INIT = 0,               // 초기화 중
    //FSP_RETURN_VALUE_MOTION_IDLE = 1,              // 움직임 없음
    //FSP_RETURN_VALUE_MOTION_NORMAL = 2,            // 일상 생활 움직임
    FSP_RETURN_VALUE_MOTION_CONTINUES = 3,          // 연속적인 움직임
    FSP_RETURN_VALUE_MOTION_HIGH = 4,               // 큰 움직임
    FSP_RETURN_VALUE_MOTION_FALLING = 5,            // 낙상
    FSP_RETURN_VALUE_MOTION_FELL = 6,               // 낙상 (2차 알림)
    FSP_RETURN_VALUE_MOTION_BACK_TO_NORMAL = 7,     // 낙상 이후 일생 생활 움직임이 감지됨
}EnDetectionRet;

#define FALL_DETECTION_DATA_NORMALIZE_VALUE 1000        // accelerometer low 데이터에 * 1000 만큼 정규화 진행

// 100msce 간격
//#define MAX_LIST_LEN 100                    // 최신 100개 데이터 저장 (약 10초 분량)
//#define DETECTION_WAITING_VALUE 30          // 최소 30개의 데이터가 저장될때까지 대기
//#define FALL_CHECK_WAITING_VALUE 15         // 큰 움직임 이후 15개의 데이터 관측
//#define FALL_DETECTION_VALIE 4              // 큰 움직임 이후 작은 움직임이 관측 4회 초과 관측 되면 낙상
//#define FALL_DETECTION_SECOND_NOTIFY 100    // 1차 알림 이후 10초간 움직임이 없으면 2차 알림
//#define FALL_DETECTION_NOT_NOTIFY 10        // 1차 알림 이후 10초이내 10sample 이상 움직임이 관측되면 2차 알림 수행하지 않음

// 20msec 간격 
#define MAX_LIST_LEN 300                    // 최신 400개 데이터 저장 (약 8초 분량)
#define DETECTION_WAITING_VALUE 150         // 최소 150개의(3초) 데이터가 저장될때까지 대기
#define FALL_CHECK_WAITING_VALUE 75         // 큰 움직임 이후 75개의(1.5초) 데이터 관측
#define FALL_DETECTION_VALIE 20             // 큰 움직임 이후 idle 움직임이 10회(0.4초) 초과 관측되면 낙상
#define FALL_DETECTION_SECOND_NOTIFY 500    // 1차 알림 이후 500개(10초) 움직임이 없으면 2차 알림
#define FALL_DETECTION_NOT_NOTIFY 50        // 1차 알림 이후 10초이내 50개 이상 움직임이 관측되면 2차 알림 수행하지 않음

typedef enum Falling_Peak_Define
{
    FALL_DETECTION_PEAK_HIGH = 1,
    FALL_DETECTION_PEAK_LOW = 2
}EnPeakDef;

typedef struct Peak_Data                    // FallCheck 상태에서 이전 15개의 데이터 체크하여 low peak, high peak 데이터를 찾는다
{
    unsigned short nIndex;                  // peak 데이티 최초  관측된  index
    unsigned short nValue;                  // peak 데이터의 값
    unsigned short nCount;                  // peak가 연속으로 관측될 경우 최초 index 이후  몇개의 데이터가 관측되는지 관측 갯수 저장
    EnPeakDef enPeakDef;
} StPeakData;

#define PEAK_DATA_MAX_VALIE 20              // 최대 20개의 peak data 저장
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
    EnDetectionState enDetectCurrentState;      // 움직임 상태
    ListNode listAccData;                       // 움직임 데이터
    unsigned short nFallCheckWaitingCnt;        // fall check state 상태에서 FALL_CHECK_WAITING_VALUE 동안 대기하며 움직임 값 관찰하여 낙상여부 판단
    unsigned short nFallDetectionCnt;           // fall check state 상태에서 FALL_DETECTION_VALIE 보다 작은 움직임 관측되면 낙상 알림
}StFallingDetecion;

// 움직임 상태 머신 초기화
void FSMFallingDetectionInit(StFallingDetecion *pstFallDetect);

// 움직임 상태 set/get 함수
void FSMMovementStateSet(StFallingDetecion *pstFallDetect, EnDetectionState enState);
EnDetectionState FSMMovementStateGet(StFallingDetecion *pstFallDetect);

// 움직임 상태 머신
EnDetectionRet FSMMovementState(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ);
void FSMMovementStateInit(StFallingDetecion *pstFallDetect);
void FSMMovementStateIdle(StFallingDetecion *pstFallDetect);
void FSMMovementStateNormal(StFallingDetecion *pstFallDetect);
void FSMMovementStateHigh(StFallingDetecion *pstFallDetect);
EnDetectionRet FSMMovementStateFallCheckStage1(StFallingDetecion *pstFallDetect);
EnDetectionRet FSMMovementStateFallCheckStage2(StFallingDetecion *pstFallDetect);

// 낙상 판별 함수
EnDetectionRet FSMMovementFallDetection(StFallingDetecion *pstFallDetect);

// peak 계산 함수
void FSMMovementPeakDataSave(StPeakDetection* pstPeak, unsigned short nSaveIndex, unsigned short nDataIndex, unsigned short nDataValue, EnPeakDef enPeakDef);
void FSMMovementPeakDataReplace(StPeakDetection* pstPeak, unsigned short nSaveIndex, unsigned short nDataIndex, unsigned short nDataValue, EnPeakDef enPeakDef);
void FSMMovementPeakDataIncreaseCount(StPeakDetection* pstPeak, unsigned short nSaveIndex);
StPeakDetection FSMMovementCalPeakData(StFallingDetecion *pstFallDetect);

// 움직임 값 list(자료형) 함수
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ); // 낙상감지 data list에 item 추가
unsigned short FSMMovementDataReadItemCount(StFallingDetecion *pstFallDetect); // 낙상감지 data list에 저장된 item 갯수
unsigned short FSMMovementDataReadTail(StFallingDetecion *pstFallDetect); // 낙상감지 data list에 저장된 최신 데이터
EnDetectionLevel FSMMovementDataReadTailState(StFallingDetecion *pstFallDetect);

// 3축 데이터 가공 함수
int FSMMovementAccNormalize(float fAccData);
unsigned short FSMMovementAccSvm(StFallingDetecion *pstFallDetect, int nCurrentAccX, int nCurrentAccY, int nCurrentAccZ);

#endif /* __FALLING_DETECTION_FSM_HEADER_H__ */

/* ====================================================================
 * The Aprotech Software License, Version 1.0 
 * Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
 * ====================================================================
 *
 *  파일명: FallingDetectionFSM.h
 *
 *  설명: 낙상 감지 PC 데모 - Finit State Machine
 *      
 * 
 *  작성자:
 *
 *
 *  날짜:
 *      2018/4/16
 */
