#!/usr/bin/python3
# -*- coding: 949 -*-

#import sys, os
#sys.path.append(os.pardir) # 부모 디렉터리의 파일을 가져올 수 있도록 설정
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.image import imread
from PIL import Image
#import pylab
#import math

# 한번에 최대 500개의 데이터만 처리한다.
def ReadDataMaxLen():
    #print("ReadDataMaxLen")
    return int(500)

def ReadDataFromFile(strFilePath):
    print("ReadDataFromFile strFilePath : ", strFilePath)

    # 입력받은 float 데이터를 정수형으로 변환
    # 정규화 데이터 : * 1000 + 1 만큼 정규화 진행 (소수점 이하는 무시함)
    npResultX = np.ones((10,ReadDataMaxLen()), np.int)
    npResultY = np.ones((10,ReadDataMaxLen()), np.int)
    npResultZ = np.ones((10,ReadDataMaxLen()), np.int)

    # low데이터 x y z 를 하나의 배열로 취합
    npLowResult = np.zeros((10,3,ReadDataMaxLen()), np.float)

    for nFileCnt in range(1,11):
        strFileNameX = strFilePath + str(nFileCnt) + "_x.txt"
        strFileNameY = strFilePath + str(nFileCnt) + "_y.txt"
        strFileNameZ = strFilePath + str(nFileCnt) + "_z.txt"
        fileX = open(strFileNameX)
        fileY = open(strFileNameY)
        fileZ = open(strFileNameZ)

        fReadLineX = np.array(fileX.readlines(), np.float)
        fReadLineY = np.array(fileY.readlines(), np.float)
        fReadLineZ = np.array(fileZ.readlines(), np.float)

        # x,y,z 중 가장 짧은 데이터 기준으로 데이터 취합
        nReadLenX = len(fReadLineX)
        nReadLenY = len(fReadLineY)
        nReadLenZ = len(fReadLineZ)
        nDataLen = ReadDataMaxLen()

        if nDataLen > nReadLenX:
            nDataLen = nReadLenX

        if nDataLen > nReadLenY:
            nDataLen = nReadLenY

        if nDataLen > nReadLenZ:
            nDataLen = nReadLenZ

        for nLineCnt in range(0, nDataLen):
            # 입력받은 float 데이터를 정수형으로 변환
            # low 데이터에 * 1000 + 1 만큼 정규화 진행 (소수점 이하는 무시함)
            # 차후 추가 데이터 확보시 정규화 값 수정 필요
            # 정규화 값 수정시 양자화 값도 같이 수정되어야 한다
            npResultX[nFileCnt-1][nLineCnt] = int(fReadLineX[nLineCnt] * 1000 + 1)
            npResultY[nFileCnt-1][nLineCnt] = int(fReadLineY[nLineCnt] * 1000 + 1)
            npResultZ[nFileCnt-1][nLineCnt] = int(fReadLineZ[nLineCnt] * 1000 + 1)
            npLowResult[nFileCnt-1][0][nLineCnt] = fReadLineX[nLineCnt]
            npLowResult[nFileCnt-1][1][nLineCnt] = fReadLineY[nLineCnt]
            npLowResult[nFileCnt-1][2][nLineCnt] = fReadLineZ[nLineCnt]

        fileX.close()
        fileY.close()
        fileZ.close()

    return npResultX, npResultY, npResultZ, npLowResult


def DrawInputData(npDrawData):
    print("DrawInputData")
    nLoopCnt = npDrawData.shape

    for nColumnCnt in range(0, nLoopCnt[0]):
        nDrawX = np.arange(0, len(npDrawData[nColumnCnt]))
        nDrawY = np.array(npDrawData[nColumnCnt], np.float)
        strLabel = "Data" + str(nColumnCnt)
        plt.xlabel("10msec") # x축 이름
        plt.ylabel("sample data") # y축 이름
        plt.title('data graph') # 제목
        plt.legend() # 범례 표시
        plt.grid(True)
        plt.plot(nDrawX, nDrawY, label=strLabel)
    plt.show()


# npDataX, npDataY, npDataZ : x, y, z 순간 가속도값
# fThresholdValue : 문턱값
# 지난 1000msec 간 센서 값 취합하여 Threshold 보다 높으면 True
def DoThresholdCheck(npDataX, npDataY, npDataZ, fThresholdValue):
    print("DoThresholdCheck")
    npCal = np.zeros(ReadDataMaxLen(), np.float)
    npRetVal = np.zeros(ReadDataMaxLen(), np.float)

    for nRowCnt in range(100, ReadDataMaxLen(), 1):
        npCalX = (npDataX[nRowCnt] - npDataX[nRowCnt-1])**2
        npCalY = (npDataY[nRowCnt] - npDataY[nRowCnt-1])**2
        npCalZ = npDataZ[nRowCnt] - npDataZ[nRowCnt-1]
        npCal[nRowCnt] = npCalX + npCalY + npCalZ

        if npCal[nRowCnt] > fThresholdValue:
            npRetVal[nRowCnt] = 1
        else:
            npRetVal[nRowCnt] = 0

    return npCal
    #return npRetVal


# 낙상 각도
def DoFallingAngle(npDataX, npDataY, npDataZ):
    print("DoFallingAngle")
    npCal = npDataX**2 + npDataY**2 / npDataZ
    npRetValue = 1.0 / np.tan(npCal)
    #print("DoFallingAngle fResultValue : ", npRetValue)
    return npRetValue


# 움직임 정량화 변수 SVM(Sum Vector Magnitude)
def DoSVM(npDataX, npDataY, npDataZ):
    print("Sum Vector Magnitude")
    npCal = (npDataX**2 + npDataY**2 + npDataZ**2)**0.5
    #print("Sum Vector Magnitude : ", npCal)
    return npCal


# 움직임 정량화 변수 모션 평균 필터 적용 SVM(Sum Vector Magnitude Moving average value)
def DoSVMMAV(npDataX, npDataY, npDataZ):
    print("Sum Vector Magnitude Moving average value")
    npAverageValue = np.zeros(ReadDataMaxLen(), np.float)
    npCal = DoSVM(npDataX, npDataY, npDataZ)

    # 50 개 단위로 moving average filter 적용
    npAverageValue[0] = npCal[0]
    for nIndex in range(1, ReadDataMaxLen()):
        nFiterIndex = nIndex
        if nIndex > 50:
            nFiterIndex = 50
        npAverageValue[nIndex] = ((nFiterIndex - 1)/ nFiterIndex) * npAverageValue[nIndex - 1] + ((1 / nFiterIndex) * npCal[nIndex])

    #print("Sum Vector Magnitude Moving average value : ", npAverageValue)
    return npAverageValue


# 움직임 정량화 변수 모션 평균 필터 + 양자화 SVM(Sum Vector Magnitude Moving average value Quantization value)
def DoSVMQuan(npDataX, npDataY, npDataZ):
    print("Sum Vector Magnitude Quantization value")
    npQuanVal = np.zeros(ReadDataMaxLen(), np.float)
    npSVMVal = DoSVMMAV(npDataX, npDataY, npDataZ)

    # 임의의 크기로(100 단위) 양자화 하였다.
    # 차후 추가 데이터 확보시 양자화 값 수정 필요
    # 정규화 값 수정시 양자화 값도 같이 수정되어야 한다
    for nIndex in range(0, ReadDataMaxLen()):
        if npSVMVal[nIndex] < 100:
            npQuanVal[nIndex] = 1
        elif npSVMVal[nIndex] >= 100 and npSVMVal[nIndex] < 200:
            npQuanVal[nIndex] = 2
        elif npSVMVal[nIndex] >= 200 and npSVMVal[nIndex] < 300:
            npQuanVal[nIndex] = 3    
        elif npSVMVal[nIndex] >= 300 and npSVMVal[nIndex] < 400:
            npQuanVal[nIndex] = 4
        elif npSVMVal[nIndex] >= 400 and npSVMVal[nIndex] < 500:
            npQuanVal[nIndex] = 5
        elif npSVMVal[nIndex] >= 500 and npSVMVal[nIndex] < 600:
            npQuanVal[nIndex] = 6
        elif npSVMVal[nIndex] >= 600 and npSVMVal[nIndex] < 700:
            npQuanVal[nIndex] = 7
        elif npSVMVal[nIndex] >= 700:
            npQuanVal[nIndex] = 8

    #print("Sum Vector Magnitude Quantization value", npQuanVal)
    return np.array(npQuanVal, np.int)


# 움직임 정량화 변수 수치미분값 DSVM(Differential Sum Vector Magnitude)
def DoDSVM(npDataX, npDataY, npDataZ):
    print("Differential Sum Vector Magnitude")
    npCal = np.zeros(ReadDataMaxLen(), np.float)

    for nIndex in range(1, ReadDataMaxLen()):
        npCalX = (npDataX[nIndex] - npDataX[nIndex - 1])**2
        npCalY = (npDataY[nIndex] - npDataY[nIndex - 1])**2
        npCalZ = (npDataZ[nIndex] - npDataZ[nIndex - 1])**2
        npCal[nIndex] = (npCalX + npCalY + npCalZ)**0.5

    #print("Differential Sum Vector Magnitude : ", npCal)
    return npCal


# 중력방향 가중치 움직임 정량화 미분값 GDSVM(gravity weighted Differential Sum Vector Magnitude)
def DoGDSVM(npDataX, npDataY, npDataZ):
    print("gravity weighted Differential Sum Vector Magnitude")
    npRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ) / 90) * DoDSVM(npDataX, npDataY, npDataZ)
    #print("gravity weighted Differential Sum Vector Magnitude : ", npRetVal)
    return npRetVal


# 중력방향 가중치 움직임 정량화 변수 GSVM(gravity weighted Sum Vector Magnitude)
def DoGSVM(npDataX, npDataY, npDataZ):
    print("gravity weighted Sum Vector Magnitude")
    fRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ) / 90) * DoSVM(npDataX, npDataY, npDataZ)
    #print("gravity weighted Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# 중력방향 가중치 움직임 정량화 변수 GSVM(gravity weighted Sum Vector Magnitude Moving average value)
def DoGSVMMAV(npDataX, npDataY, npDataZ):
    print("gravity weighted Sum Vector Magnitude Moving average value")
    npAverageValue = np.zeros(ReadDataMaxLen(), np.float)
    npCal = DoGSVM(npDataX, npDataY, npDataZ)

    # 50 개 단위로 moving average filter 적용
    npAverageValue[0] = npCal[0]
    for nIndex in range(1, ReadDataMaxLen()):
        nFiterIndex = nIndex
        if nIndex > 50:
            nFiterIndex = 50
        npAverageValue[nIndex] = ((nFiterIndex - 1)/ nFiterIndex) * npAverageValue[nIndex - 1] + ((1 / nFiterIndex) * npCal[nIndex])

    #print("Sum Vector Magnitude Moving average value : ", npAverageValue)
    return npAverageValue

# 중력방향 움직임 정량화 변수 모션 평균 필터 + 양자화 GSVM(gravity weighted Sum Vector Magnitude Moving average value Quantization value)
def DoGSVMQuan(npDataX, npDataY, npDataZ):
    print("gravity weighted Sum Vector Magnitude Quantization value")
    npQuanVal = np.zeros(ReadDataMaxLen(), np.float)
    npSVMVal = DoGSVMMAV(npDataX, npDataY, npDataZ)

    # 임의의 크기로(100 단위) 양자화 하였다.
    # 차후 추가 데이터 확보시 양자화 값 수정 필요
    # 정규화 값 수정시 양자화 값도 같이 수정되어야 한다
    for nIndex in range(0, ReadDataMaxLen()):
        if npSVMVal[nIndex] < 10000:
            npQuanVal[nIndex] = 1
        elif npSVMVal[nIndex] >= 10000 and npSVMVal[nIndex] < 20000:
            npQuanVal[nIndex] = 2
        elif npSVMVal[nIndex] >= 20000 and npSVMVal[nIndex] < 30000:
            npQuanVal[nIndex] = 3    
        elif npSVMVal[nIndex] >= 30000 and npSVMVal[nIndex] < 40000:
            npQuanVal[nIndex] = 4
        elif npSVMVal[nIndex] >= 40000 and npSVMVal[nIndex] < 50000:
            npQuanVal[nIndex] = 5
        elif npSVMVal[nIndex] >= 50000 and npSVMVal[nIndex] < 60000:
            npQuanVal[nIndex] = 6
        elif npSVMVal[nIndex] >= 60000 and npSVMVal[nIndex] < 70000:
            npQuanVal[nIndex] = 7
        elif npSVMVal[nIndex] >= 70000:
            npQuanVal[nIndex] = 8

    #print("Sum Vector Magnitude Quantization value", npQuanVal)
    return npQuanVal


def DrawOutputData(npDrawData, strTitle):
    print("DrawOutputData")

    nDrawX = np.arange(0, len(npDrawData))
    nDrawY = np.array(npDrawData, np.float)
    plt.xlabel("x") # x축 이름
    plt.ylabel("y") # y축 이름
    plt.title(strTitle) # 제목
    plt.legend() # 범례 표시
    plt.grid(True)
    plt.plot(nDrawX, nDrawY)
    plt.show()


# main 함수
if __name__ == "__main__":
    print("===== FallingDetection main function =====")

    # data 저장 경로, 
    strFilePathWorking = "SampleData/걷기/"
    strFilePathsSitDownAndStandUp = "SampleData/앉았다일어나기/"

    npWorkingX, npWorkingY, npWorkingZ, npLowWorking = ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ, npLowSitNUp = ReadDataFromFile(strFilePathsSitDownAndStandUp)

    #DrawInputData(npWorkingX)
    #DrawInputData(npWorkingY)
    #DrawInputData(npWorkingZ)
    #DrawInputData(npSitNUpX)
    #DrawInputData(npSitNUpY)
    #DrawInputData(npSitNUpZ)

    #Threshold 값 : 단말기를 소지하지 않고 그대로 둔 상태에서 10초간의 평균값 (데이터 없어 임의값 사용)
    # 걷기 0번 데이터 Thrashold
    npResult = DoThresholdCheck(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1)
    #DrawOutputData(npResult, "Threshold")

    # 걷기 0번 데이터 낙상 각도
    npResult = DoFallingAngle(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "FallingAngle")

    # 걷기 0번 데이터 움직임 정량화 변수
    npResult = DoSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "Sum Vector Magnitude")

    # 걷기 0번 데이터 움직임 정량화 변수 + 모션 평균 필터
    npResult = DoSVMMAV(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "SVM Moving average value")

    # 걷기 0번 데이터 8단계 양자화
    npResult = DoSVMQuan(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "SVM Quantization value")
    #for nIndex in range(0,10):
    #    #npResult = DoSVMQuan(npWorkingX[nIndex], npWorkingY[nIndex], npWorkingY[nIndex])
    #    npResult = DoSVMQuan(npSitNUpX[nIndex], npSitNUpY[nIndex], npSitNUpZ[nIndex])
    #    DrawOutputData(npResult, "SVM Quantization value")

    # 걷기 0번 데이터 정량화 변수 수치미분 값
    npResult = DoDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "Differential Sum Vector Magnitude")

    # 걷기 0번 데이터 중력방향 가중치 움직임 정량화 변수 수치미분 값
    npResult = DoGDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "gravity weighted Differential Sum Vector Magnitude")

    # 걷기 0번 데이터 중력방향 가중치 움직임 정량화 변수 값
    npResult = DoGSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "gravity weighted Sum Vector Magnitude")

    # GSVM 모션 평균 필터 적용
    npResult = DoGSVMMAV(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "GSVM Moving average value")

    # GSVM 양자화
    npResult = DoGSVMQuan(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "GSVM Quantization value")
    #for nIndex in range(0,10):
    #    #npResult = DoGSVMQuan(npWorkingX[nIndex], npWorkingY[nIndex], npWorkingY[nIndex])
    #    npResult = DoGSVMQuan(npSitNUpX[nIndex], npSitNUpY[nIndex], npSitNUpZ[nIndex])
    #    DrawOutputData(npResult, "GSVM Quantization value")


# ====================================================================
# The Aprotech Software License, Version 1.0 
# Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
# ====================================================================
#
#  파일명: FallingDetection.py
#
#  설명:
#
# 
#  작성자:
#
#
#  날짜:
#      2018/01/24
#
