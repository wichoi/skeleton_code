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


# ������ ����ȭ ���� ��ġ�̺а� DSVM(Differential Sum Vector Magnitude)
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

    npWorkingX, npWorkingY, npWorkingZ = ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ = ReadDataFromFile(strFilePathsSitDownAndStandUp)

    #DrawInputData(npWorkingX)
    #DrawInputData(npWorkingY)
    #DrawInputData(npWorkingZ)
    #DrawInputData(npSitNUpX)
    #DrawInputData(npSitNUpY)
    #DrawInputData(npSitNUpZ)

    #Threshold �� : �ܸ��⸦ �����ϰ� �������� �ʴ� ���¿��� 10�ʰ��� ��հ� (������ ���� ���ǰ� ���)
    # �ȱ� 0�� ������ Thrashold
    npResult = DoThresholdCheck(npWorkingX[0], npWorkingY[0], npWorkingY[0], 1)
    DrawOutputData(npResult, "Threshold")

    # �ȱ� 0�� ������ ���� ����
    npResult = DoFallingAngle(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "FallingAngle")

    # �ȱ� 0�� ������ ������ ����ȭ ����
    npResult = DoSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "Sum Vector Magnitude")

    # �ȱ� 0�� ������ ����ȭ ���� ��ġ�̺� ��
    npResult = DoDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "Differential Sum Vector Magnitude")

    # �ȱ� 0�� ������ �߷¹��� ����ġ ������ ����ȭ ���� ��ġ�̺� ��
    npResult = DoGDSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "gravity weighted Differential Sum Vector Magnitude")

    # �ȱ� 0�� ������ �߷¹��� ����ġ ������ ����ȭ ���� ��
    npResult = DoGSVM(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    DrawOutputData(npResult, "gravity weighted Sum Vector Magnitude")



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
