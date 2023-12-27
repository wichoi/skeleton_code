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
            #if fReadLineZ[nLineCnt] != 0:
            #    resultZ[nFileCnt-1][nLineCnt] = fReadLineZ[nLineCnt]
            #else:
            #    resultZ[nFileCnt-1][nLineCnt] = 0.000000001

        fileX.close()
        fileY.close()
        fileZ.close()

    return resultX, resultY, resultZ


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
    npCal = np.zeros(600, np.float)
    npRetVal = np.zeros(600, np.float)

    for nRowCnt in range(100, 600, 1):
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


# 움직임 정량화 변수 수치미분값 DSVM(Differential Sum Vector Magnitude)
def DoDSVM(npDataX, npDataY, npDataZ):
    print("Differential Sum Vector Magnitude")
    npCal = np.zeros(600, np.float)

    for nIndex in range(1, 600):
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

    npWorkingX, npWorkingY, npWorkingZ = ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ = ReadDataFromFile(strFilePathsSitDownAndStandUp)

    #DrawInputData(npWorkingX)
    #DrawInputData(npWorkingY)
    #DrawInputData(npWorkingZ)
    #DrawInputData(npSitNUpX)
    #DrawInputData(npSitNUpY)
    #DrawInputData(npSitNUpZ)

    #Threshold 값 : 단말기를 소지하고 움직이지 않는 상태에서 10초간의 평균값 (데이터 없어 임의값 사용)
    # 걷기 0번 데이터 Thrashold
    npResult = DoThresholdCheck(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1)
    DrawOutputData(npResult, "Threshold")

    # 걷기 0번 데이터 낙상 각도
    npResult = DoFallingAngle(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "FallingAngle")

    # 걷기 0번 데이터 움직임 정량화 변수
    npResult = DoSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "Sum Vector Magnitude")

    # 걷기 0번 데이터 정량화 변수 수치미분 값
    npResult = DoDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "Differential Sum Vector Magnitude")

    # 걷기 0번 데이터 중력방향 가중치 움직임 정량화 변수 수치미분 값
    npResult = DoGDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "gravity weighted Differential Sum Vector Magnitude")

    # 걷기 0번 데이터 중력방향 가중치 움직임 정량화 변수 값
    npResult = DoGSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "gravity weighted Sum Vector Magnitude")



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
