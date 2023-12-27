#!/usr/bin/python3
# -*- coding: 949 -*-

#import sys, os
#sys.path.append(os.pardir) # �θ� ���͸��� ������ ������ �� �ֵ��� ����
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.image import imread
from PIL import Image
#import pylab
#import math

# �ѹ��� �ִ� 500���� �����͸� ó���Ѵ�.
def ReadDataMaxLen():
    #print("ReadDataMaxLen")
    return int(500)

def ReadDataFromFile(strFilePath):
    print("ReadDataFromFile strFilePath : ", strFilePath)

    # �Է¹��� float �����͸� ���������� ��ȯ
    # ����ȭ ������ : * 1000 + 1 ��ŭ ����ȭ ���� (�Ҽ��� ���ϴ� ������)
    npResultX = np.ones((10,ReadDataMaxLen()), np.int)
    npResultY = np.ones((10,ReadDataMaxLen()), np.int)
    npResultZ = np.ones((10,ReadDataMaxLen()), np.int)

    # low������ x y z �� �ϳ��� �迭�� ����
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

        # x,y,z �� ���� ª�� ������ �������� ������ ����
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
            # �Է¹��� float �����͸� ���������� ��ȯ
            # low �����Ϳ� * 1000 + 1 ��ŭ ����ȭ ���� (�Ҽ��� ���ϴ� ������)
            # ���� �߰� ������ Ȯ���� ����ȭ �� ���� �ʿ�
            # ����ȭ �� ������ ����ȭ ���� ���� �����Ǿ�� �Ѵ�
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
        plt.xlabel("10msec") # x�� �̸�
        plt.ylabel("sample data") # y�� �̸�
        plt.title('data graph') # ����
        plt.legend() # ���� ǥ��
        plt.grid(True)
        plt.plot(nDrawX, nDrawY, label=strLabel)
    plt.show()


# npDataX, npDataY, npDataZ : x, y, z ���� ���ӵ���
# fThresholdValue : ���ΰ�
# ���� 1000msec �� ���� �� �����Ͽ� Threshold ���� ������ True
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


# ���� ����
def DoFallingAngle(npDataX, npDataY, npDataZ):
    print("DoFallingAngle")
    npCal = npDataX**2 + npDataY**2 / npDataZ
    npRetValue = 1.0 / np.tan(npCal)
    #print("DoFallingAngle fResultValue : ", npRetValue)
    return npRetValue


# ������ ����ȭ ���� SVM(Sum Vector Magnitude)
def DoSVM(npDataX, npDataY, npDataZ):
    print("Sum Vector Magnitude")
    npCal = (npDataX**2 + npDataY**2 + npDataZ**2)**0.5
    #print("Sum Vector Magnitude : ", npCal)
    return npCal


# ������ ����ȭ ���� ��� ��� ���� ���� SVM(Sum Vector Magnitude Moving average value)
def DoSVMMAV(npDataX, npDataY, npDataZ):
    print("Sum Vector Magnitude Moving average value")
    npAverageValue = np.zeros(ReadDataMaxLen(), np.float)
    npCal = DoSVM(npDataX, npDataY, npDataZ)

    # 50 �� ������ moving average filter ����
    npAverageValue[0] = npCal[0]
    for nIndex in range(1, ReadDataMaxLen()):
        nFiterIndex = nIndex
        if nIndex > 50:
            nFiterIndex = 50
        npAverageValue[nIndex] = ((nFiterIndex - 1)/ nFiterIndex) * npAverageValue[nIndex - 1] + ((1 / nFiterIndex) * npCal[nIndex])

    #print("Sum Vector Magnitude Moving average value : ", npAverageValue)
    return npAverageValue


# ������ ����ȭ ���� ��� ��� ���� + ����ȭ SVM(Sum Vector Magnitude Moving average value Quantization value)
def DoSVMQuan(npDataX, npDataY, npDataZ):
    print("Sum Vector Magnitude Quantization value")
    npQuanVal = np.zeros(ReadDataMaxLen(), np.float)
    npSVMVal = DoSVMMAV(npDataX, npDataY, npDataZ)

    # ������ ũ���(100 ����) ����ȭ �Ͽ���.
    # ���� �߰� ������ Ȯ���� ����ȭ �� ���� �ʿ�
    # ����ȭ �� ������ ����ȭ ���� ���� �����Ǿ�� �Ѵ�
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


# ������ ����ȭ ���� ��ġ�̺а� DSVM(Differential Sum Vector Magnitude)
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


# �߷¹��� ����ġ ������ ����ȭ �̺а� GDSVM(gravity weighted Differential Sum Vector Magnitude)
def DoGDSVM(npDataX, npDataY, npDataZ):
    print("gravity weighted Differential Sum Vector Magnitude")
    npRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ) / 90) * DoDSVM(npDataX, npDataY, npDataZ)
    #print("gravity weighted Differential Sum Vector Magnitude : ", npRetVal)
    return npRetVal


# �߷¹��� ����ġ ������ ����ȭ ���� GSVM(gravity weighted Sum Vector Magnitude)
def DoGSVM(npDataX, npDataY, npDataZ):
    print("gravity weighted Sum Vector Magnitude")
    fRetVal = (90 - DoFallingAngle(npDataX, npDataY, npDataZ) / 90) * DoSVM(npDataX, npDataY, npDataZ)
    #print("gravity weighted Sum Vector Magnitude : ", fRetVal)
    return fRetVal


# �߷¹��� ����ġ ������ ����ȭ ���� GSVM(gravity weighted Sum Vector Magnitude Moving average value)
def DoGSVMMAV(npDataX, npDataY, npDataZ):
    print("gravity weighted Sum Vector Magnitude Moving average value")
    npAverageValue = np.zeros(ReadDataMaxLen(), np.float)
    npCal = DoGSVM(npDataX, npDataY, npDataZ)

    # 50 �� ������ moving average filter ����
    npAverageValue[0] = npCal[0]
    for nIndex in range(1, ReadDataMaxLen()):
        nFiterIndex = nIndex
        if nIndex > 50:
            nFiterIndex = 50
        npAverageValue[nIndex] = ((nFiterIndex - 1)/ nFiterIndex) * npAverageValue[nIndex - 1] + ((1 / nFiterIndex) * npCal[nIndex])

    #print("Sum Vector Magnitude Moving average value : ", npAverageValue)
    return npAverageValue

# �߷¹��� ������ ����ȭ ���� ��� ��� ���� + ����ȭ GSVM(gravity weighted Sum Vector Magnitude Moving average value Quantization value)
def DoGSVMQuan(npDataX, npDataY, npDataZ):
    print("gravity weighted Sum Vector Magnitude Quantization value")
    npQuanVal = np.zeros(ReadDataMaxLen(), np.float)
    npSVMVal = DoGSVMMAV(npDataX, npDataY, npDataZ)

    # ������ ũ���(100 ����) ����ȭ �Ͽ���.
    # ���� �߰� ������ Ȯ���� ����ȭ �� ���� �ʿ�
    # ����ȭ �� ������ ����ȭ ���� ���� �����Ǿ�� �Ѵ�
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
    plt.xlabel("x") # x�� �̸�
    plt.ylabel("y") # y�� �̸�
    plt.title(strTitle) # ����
    plt.legend() # ���� ǥ��
    plt.grid(True)
    plt.plot(nDrawX, nDrawY)
    plt.show()


# main �Լ�
if __name__ == "__main__":
    print("===== FallingDetection main function =====")

    # data ���� ���, 
    strFilePathWorking = "SampleData/�ȱ�/"
    strFilePathsSitDownAndStandUp = "SampleData/�ɾҴ��Ͼ��/"

    npWorkingX, npWorkingY, npWorkingZ, npLowWorking = ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ, npLowSitNUp = ReadDataFromFile(strFilePathsSitDownAndStandUp)

    #DrawInputData(npWorkingX)
    #DrawInputData(npWorkingY)
    #DrawInputData(npWorkingZ)
    #DrawInputData(npSitNUpX)
    #DrawInputData(npSitNUpY)
    #DrawInputData(npSitNUpZ)

    #Threshold �� : �ܸ��⸦ �������� �ʰ� �״�� �� ���¿��� 10�ʰ��� ��հ� (������ ���� ���ǰ� ���)
    # �ȱ� 0�� ������ Thrashold
    npResult = DoThresholdCheck(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1)
    #DrawOutputData(npResult, "Threshold")

    # �ȱ� 0�� ������ ���� ����
    npResult = DoFallingAngle(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "FallingAngle")

    # �ȱ� 0�� ������ ������ ����ȭ ����
    npResult = DoSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "Sum Vector Magnitude")

    # �ȱ� 0�� ������ ������ ����ȭ ���� + ��� ��� ����
    npResult = DoSVMMAV(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "SVM Moving average value")

    # �ȱ� 0�� ������ 8�ܰ� ����ȭ
    npResult = DoSVMQuan(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "SVM Quantization value")
    #for nIndex in range(0,10):
    #    #npResult = DoSVMQuan(npWorkingX[nIndex], npWorkingY[nIndex], npWorkingY[nIndex])
    #    npResult = DoSVMQuan(npSitNUpX[nIndex], npSitNUpY[nIndex], npSitNUpZ[nIndex])
    #    DrawOutputData(npResult, "SVM Quantization value")

    # �ȱ� 0�� ������ ����ȭ ���� ��ġ�̺� ��
    npResult = DoDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "Differential Sum Vector Magnitude")

    # �ȱ� 0�� ������ �߷¹��� ����ġ ������ ����ȭ ���� ��ġ�̺� ��
    npResult = DoGDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "gravity weighted Differential Sum Vector Magnitude")

    # �ȱ� 0�� ������ �߷¹��� ����ġ ������ ����ȭ ���� ��
    npResult = DoGSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "gravity weighted Sum Vector Magnitude")

    # GSVM ��� ��� ���� ����
    npResult = DoGSVMMAV(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    #DrawOutputData(npResult, "GSVM Moving average value")

    # GSVM ����ȭ
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
#  ���ϸ�: FallingDetection.py
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
