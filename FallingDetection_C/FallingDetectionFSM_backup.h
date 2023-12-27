#ifndef __FALLING_DETECTION_FSM_HEADER_H__
#define __FALLING_DETECTION_FSM_HEADER_H__

// 움직임 상태 정의
typedef enum FSM_FALLING_DETECTION_STATE_DEFINE
{
    FSM_STATE_MOVEMENT_INIT = -1,           // 100 개의 데이터가 쌓일때까지 대기 (10msec sampling 기준)
    FSM_STATE_MOVEMENT_IDLE = 0,            // 움직임이 없는 상태
    FSM_STATE_MOVEMENT_LOW = 1,             // 작은 움직임
    FSM_STATE_MOVEMENT_MIDDLE = 2,          // 일상 생활 움직임
    FSM_STATE_MOVEMENT_HIGH = 3,            // 큰 움직임
    FSM_STATE_MOVEMENT_FALL_CHECK = 4,      // 큰 움직임 이후 낙상 여부 체크
}EnDetectionState;

// DSVMMAV 기준 4단계 양자화
typedef enum FSM_FALLING_DETECTION_MOVEMENT_LEVEL
{
    MOVEMENT_LEVEL_1 = 50, // 1,
    MOVEMENT_LEVEL_2 = 100, // 40,
    MOVEMENT_LEVEL_3 = 1000, // 150,
}EnDetectionLevel;

// return value
typedef enum FSM_FALLING_DETECTION_RETURN_VALUE
{
    FSP_RETURN_VALUE_MOTION_INIT = -1,      // 초기화 중
    //FSP_RETURN_VALUE_MOTION_IDLE = 0,      // 움직임 없음
    //FSP_RETURN_VALUE_MOTION_LOW = 1,       // 작은 움직임
    //FSP_RETURN_VALUE_MOTION_MIDDLE = 2,    // 일상 생활 움직임
    FSP_RETURN_VALUE_MOTION_HIGH = 3,       // 큰 움직임
    FSP_RETURN_VALUE_MOTION_FALLING = 4,    // 낙상
}EnDetectionRet;

#define FALL_DETECTION_DATA_NORMALIZE_VALUE 1000 // 100        // low 데이터에 * 100 만큼 정규화 진행
#define FALL_DETECTION_DATA_MOVING_AVERAGE_FILTER 5 // 50    // 50개 단위 moving average filter

// 10msec sampling 기준 데이터 초기화
#define MAX_LIST_LEN 500              // 최신 300개 데이터 저장
#define DETECTION_WAITING_VALUE 13 // 150   // 최소 150개의 데이터가 저장될때까지 대기
#define FALL_CHECK_WAITING_VALUE 10 // 100  // 큰 움직임 이후 100개의 데이터 관측
#define FALL_DECECTION_VALIE 2 // 5        // 큰 움직임 이후 작은 움직임이 관측 5회 이상 관측 되면 낙상

typedef struct Falling__node
{
    //int nHeadIndex;             // oldest data
    int nTailIndex;             // newly data
    int nItemCount;             // total item count
    int nDataAccX[MAX_LIST_LEN]; // 가속도 X 정규화 데이터 (fAccDataX * 100, 소수점 이하 버림)
    int nDataAccY[MAX_LIST_LEN]; // 가속도 Y 정규화 데이터 (fAccDataY * 100, 소수점 이하 버림)
    int nDataAccZ[MAX_LIST_LEN]; // 가속도 Z 정규화 데이터 (fAccDataZ * 100, 소수점 이하 버림)
    int nDataDsvm[MAX_LIST_LEN]; // DSVM(Differential Sum Vector Magnitude)
    int nDataDsvmMav[MAX_LIST_LEN]; // DSVM + Moving average filter
    int nDataSvm[MAX_LIST_LEN];
    int nDataSvmMav[MAX_LIST_LEN];

} ListNode;

typedef struct FSMFallingDetectionStruct
{
    EnDetectionState enDetectState;     // 움직임 상태
    ListNode listAccData;               // 움직임 데이터
    int nFallCheckWaitingCnt;           // 큰 움직임 이후 움직임 상태 체크
    int nFallDetectionCnt;              // 작은 움직임 체크 변수
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
void FSMMovementStateLow(StFallingDetecion *pstFallDetect);
void FSMMovementStateMiddle(StFallingDetecion *pstFallDetect);
void FSMMovementStateHigh(StFallingDetecion *pstFallDetect);
EnDetectionRet FSMMovementStateFallCheck(StFallingDetecion *pstFallDetect);

// 움직임 값 list(자료형) 함수
void FSMMovementDataAppend(StFallingDetecion *pstFallDetect, float fAccDataX, float fAccDataY, float fAccDataZ); // 낙상감지 data list에 item 추가
int FSMMovementDataReadItemCount(StFallingDetecion *pstFallDetect); // 낙상감지 data list에 저장된 item 갯수
int FSMMovementDataReadTail(StFallingDetecion *pstFallDetect); // 낙상감지 data list에 저장된 최신 데이터
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
