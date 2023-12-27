#!/usr/bin/python3
# -*- coding: 949 -*-

#import sys, os
#sys.path.append(os.pardir) # 부모 디렉터리의 파일을 가져올 수 있도록 설정
import numpy as np
import pickle
import matplotlib.pyplot as plt
from matplotlib.image import imread
from dataset.mnist import load_mnist
from PIL import Image
import pylab
import math

def ReadDataFromFile(strFilePath):
    print("ReadDataFromFile strFilePath : ", strFilePath)

    resultX = np.zeros((10,600), np.float)
    resultY = np.zeros((10,600), np.float)
    resultZ = np.zeros((10,600), np.float)

    for nFileCnt in range(1,11):
        strFileNameX = strFilePath + str(nFileCnt) + "_x.txt"
        strFileNameY = strFilePath + str(nFileCnt) + "_y.txt"
        strFileNameZ = strFilePath + str(nFileCnt) + "_z.txt"
        fileX = open(strFileNameX)
        fileY = open(strFileNameY)
        fileZ = open(strFileNameZ)

        fReadLineX = np.array(fileX.readlines(), np.float)
        for nLineCnt in range(0, len(fReadLineX)):
            resultX[nFileCnt-1][nLineCnt] = fReadLineX[nLineCnt]

        fReadLineY = np.array(fileY.readlines(), np.float)
        for nLineCnt in range(0, len(fReadLineY)):
            resultY[nFileCnt-1][nLineCnt] = fReadLineY[nLineCnt]

        fReadLineZ = np.array(fileZ.readlines(), np.float)
        for nLineCnt in range(0, len(fReadLineZ)):
            resultZ[nFileCnt-1][nLineCnt] = fReadLineZ[nLineCnt]

        fileX.close()
        fileY.close()
        fileZ.close()

    return resultX, resultY, resultZ


def DrawInputData(npDrawData):
    print("DrawGraph")
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
# nTime : 계산 기준 시간
# nSampleingCnt : 초당 샘플링된 갯수 (10msec * 100 = 1000msec)
# fThresholdValue : 문턱값
def DoThresholdCheck(npDataX, npDataY, npDataZ, nTime, nSampleingCnt, fThresholdValue):
    if nTime < 1000:
        print("DoThresholdCheck : nTime need more 1000msec") # 최소 이전 1초간의 가속도 필요
        return False

    fResultSum = 0
    nStartIndex = int(nTime / 10)
    nEndIndex = nStartIndex - nSampleingCnt
    for nRowCnt in range(nStartIndex, nEndIndex, -1):
        npCalX = (npDataX[nRowCnt] - npDataX[nRowCnt-1])**2
        npCalY = (npDataY[nRowCnt] - npDataY[nRowCnt-1])**2
        npCalZ = npDataZ[nRowCnt] - npDataZ[nRowCnt-1]
        npCal = npCalX + npCalY + npCalZ
        if npCal > 0: # 허수 처리는??
            fResultSum += npCal**0.5


    print("DoThresholdCheck fResultSum : ", fResultSum, ", fThresholdValue : ", fThresholdValue)
    if fResultSum > fThresholdValue:
        print("DoThresholdCheck return True")
        return True
    else:
        print("DoThresholdCheck return False")
        return False

# 낙상 각도
def DoFallingAngle(npDataX, npDataY, npDataZ, nTime):
    nCheckIndex = int(nTime / 10)
    npCal = npDataX[nCheckIndex]**2 + npDataY[nCheckIndex]**2 / npDataZ[nCheckIndex]
    fResultValue = 1.0 / np.tan(npCal)

    print("DoFallingAngle fResultValue : ", fResultValue)
    return fResultValue


# 움직임 정량화 변수 SVM(Sum Vector Magnitude)
def DoSVM(npDataX, npDataY, npDataZ, nTime):
    fRetVal = 0
    nCheckIndex = int(nTime / 10)
    npCal = npDataX[nCheckIndex]**2 + npDataY[nCheckIndex]**2 + npDataZ[nCheckIndex]**2
    if npCal != 0:
        fRetVal = npCal**0.5

    print("Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# 움직임 정량화 변수 수치미분값 DSVM(Differential Sum Vector Magnitude)
def DoDSVM(npDataX, npDataY, npDataZ, nTime):
    if nTime < 10:
        print("DoDSVM need more 10msec")
        return 0

    fRetVal = 0
    nCheckIndex = int(nTime / 10)
    npCalX = (npDataX[nCheckIndex] - npDataX[nCheckIndex - 1])**2
    npCalY = (npDataY[nCheckIndex] - npDataY[nCheckIndex - 1])**2
    npCalZ = (npDataZ[nCheckIndex] - npDataZ[nCheckIndex - 1])**2
    npCal = npCalX + npCalY + npCalZ
    if npCal != 0:
        fRetVal = npCal**0.5

    print("Differential Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# 중력방향 가중치 움직임 정량화 미분값 GDSVM(gravity weighted Differential Sum Vector Magnitude)
def GDSVM(npDataX, npDataY, npDataZ, nTime):
    if nTime < 10:
        print("GDSVM need more 10msec")
        return 0

    fRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ, nTime) / 90) * DoDSVM(npDataX, npDataY, npDataZ, nTime)
    print("gravity weighted Differential Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# 중력방향 가중치 움직임 정량화 변수 GDSVM(gravity weighted Sum Vector Magnitude)
def GDSVM(npDataX, npDataY, npDataZ, nTime):
    if nTime < 10:
        print("GSVM need more 10msec")
        return 0

    fRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ, nTime) / 90) * DoSVM(npDataX, npDataY, npDataZ, nTime)
    print("gravity weighted Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# main 함수
if __name__ == "__main__":
    print("===== FallingDetection main function =====")

    # data 저장 경로, 
    strFilePathWorking = "SampleData/걷기/"
    strFilePathsSitDownAndStandUp = "SampleData/앉았다일어나기/"

    npWorkingX, npWorkingY, npWorkingZ = ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ = ReadDataFromFile(strFilePathsSitDownAndStandUp)

    #DrawInputData(npWorkingX)
    #DrawInputData(npWorkingY)
    #DrawInputData(npWorkingZ)
    #DrawInputData(npSitNUpX)
    #DrawInputData(npSitNUpY)
    #DrawInputData(npSitNUpZ)

    #Threshold 값 : 단말기를 소지하고 움직이지 않는 상태에서 10초간의 평균값
    DoThresholdCheck(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000, 100, 1) # 걷기 0번 데이터 1000msec 순간 TH 체크
    DoFallingAngle(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # 걷기 0번 데이터 1000msec 순간 낙상 각도
    DoSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # 걷기 0번 데이터 1000msec 순간 움직임 정량화 변수
    DoDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # 걷기 0번 데이터 1000msec 순간 움직임 정량화 변수 수치미분 값
    GDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # 걷기 0번 데이터 1000msec 중력방향 가중치 움직임 정량화 변수 수치미분 값



# ====================================================================
# The Aprotech Software License, Version 1.0 
# Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
# ====================================================================
#
#  파일명: MaldivesDemo.py
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
