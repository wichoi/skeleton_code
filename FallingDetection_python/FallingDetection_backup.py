#!/usr/bin/python3
# -*- coding: 949 -*-

#import sys, os
#sys.path.append(os.pardir) # �θ� ���͸��� ������ ������ �� �ֵ��� ����
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
        plt.xlabel("10msec") # x�� �̸�
        plt.ylabel("sample data") # y�� �̸�
        plt.title('data graph') # ����
        plt.legend() # ���� ǥ��
        plt.grid(True)
        plt.plot(nDrawX, nDrawY, label=strLabel)
    plt.show()


# npDataX, npDataY, npDataZ : x, y, z ���� ���ӵ���
# nTime : ��� ���� �ð�
# nSampleingCnt : �ʴ� ���ø��� ���� (10msec * 100 = 1000msec)
# fThresholdValue : ���ΰ�
def DoThresholdCheck(npDataX, npDataY, npDataZ, nTime, nSampleingCnt, fThresholdValue):
    if nTime < 1000:
        print("DoThresholdCheck : nTime need more 1000msec") # �ּ� ���� 1�ʰ��� ���ӵ� �ʿ�
        return False

    fResultSum = 0
    nStartIndex = int(nTime / 10)
    nEndIndex = nStartIndex - nSampleingCnt
    for nRowCnt in range(nStartIndex, nEndIndex, -1):
        npCalX = (npDataX[nRowCnt] - npDataX[nRowCnt-1])**2
        npCalY = (npDataY[nRowCnt] - npDataY[nRowCnt-1])**2
        npCalZ = npDataZ[nRowCnt] - npDataZ[nRowCnt-1]
        npCal = npCalX + npCalY + npCalZ
        if npCal > 0: # ��� ó����??
            fResultSum += npCal**0.5


    print("DoThresholdCheck fResultSum : ", fResultSum, ", fThresholdValue : ", fThresholdValue)
    if fResultSum > fThresholdValue:
        print("DoThresholdCheck return True")
        return True
    else:
        print("DoThresholdCheck return False")
        return False

# ���� ����
def DoFallingAngle(npDataX, npDataY, npDataZ, nTime):
    nCheckIndex = int(nTime / 10)
    npCal = npDataX[nCheckIndex]**2 + npDataY[nCheckIndex]**2 / npDataZ[nCheckIndex]
    fResultValue = 1.0 / np.tan(npCal)

    print("DoFallingAngle fResultValue : ", fResultValue)
    return fResultValue


# ������ ����ȭ ���� SVM(Sum Vector Magnitude)
def DoSVM(npDataX, npDataY, npDataZ, nTime):
    fRetVal = 0
    nCheckIndex = int(nTime / 10)
    npCal = npDataX[nCheckIndex]**2 + npDataY[nCheckIndex]**2 + npDataZ[nCheckIndex]**2
    if npCal != 0:
        fRetVal = npCal**0.5

    print("Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# ������ ����ȭ ���� ��ġ�̺а� DSVM(Differential Sum Vector Magnitude)
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


# �߷¹��� ����ġ ������ ����ȭ �̺а� GDSVM(gravity weighted Differential Sum Vector Magnitude)
def GDSVM(npDataX, npDataY, npDataZ, nTime):
    if nTime < 10:
        print("GDSVM need more 10msec")
        return 0

    fRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ, nTime) / 90) * DoDSVM(npDataX, npDataY, npDataZ, nTime)
    print("gravity weighted Differential Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# �߷¹��� ����ġ ������ ����ȭ ���� GDSVM(gravity weighted Sum Vector Magnitude)
def GDSVM(npDataX, npDataY, npDataZ, nTime):
    if nTime < 10:
        print("GSVM need more 10msec")
        return 0

    fRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ, nTime) / 90) * DoSVM(npDataX, npDataY, npDataZ, nTime)
    print("gravity weighted Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# main �Լ�
if __name__ == "__main__":
    print("===== FallingDetection main function =====")

    # data ���� ���, 
    strFilePathWorking = "SampleData/�ȱ�/"
    strFilePathsSitDownAndStandUp = "SampleData/�ɾҴ��Ͼ��/"

    npWorkingX, npWorkingY, npWorkingZ = ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ = ReadDataFromFile(strFilePathsSitDownAndStandUp)

    #DrawInputData(npWorkingX)
    #DrawInputData(npWorkingY)
    #DrawInputData(npWorkingZ)
    #DrawInputData(npSitNUpX)
    #DrawInputData(npSitNUpY)
    #DrawInputData(npSitNUpZ)

    #Threshold �� : �ܸ��⸦ �����ϰ� �������� �ʴ� ���¿��� 10�ʰ��� ��հ�
    DoThresholdCheck(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000, 100, 1) # �ȱ� 0�� ������ 1000msec ���� TH üũ
    DoFallingAngle(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # �ȱ� 0�� ������ 1000msec ���� ���� ����
    DoSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # �ȱ� 0�� ������ 1000msec ���� ������ ����ȭ ����
    DoDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # �ȱ� 0�� ������ 1000msec ���� ������ ����ȭ ���� ��ġ�̺� ��
    GDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1000) # �ȱ� 0�� ������ 1000msec �߷¹��� ����ġ ������ ����ȭ ���� ��ġ�̺� ��



# ====================================================================
# The Aprotech Software License, Version 1.0 
# Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
# ====================================================================
#
#  ���ϸ�: MaldivesDemo.py
#
#  ����:
#
# 
#  �ۼ���:
#
#
#  ��¥:
#      2018/01/24
#
