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

def MaldivesDemo():
    print("0 : MaldivesDemo")
    input("Press the any button...")

def GetSampleData():
    print("1 : GetSampleData")

    resultX = [[0]*15 for i in range(400)] # 360, 370
    resultY = [[0]*15 for i in range(400)] # 360, 370
    resultZ = [[0]*15 for i in range(400)] # 360, 370

    for nFileCnt in range(1,11):
        strFileNameX = "SampleData/걷기/" + str(nFileCnt) + "_x.txt"
        strFileNameY = "SampleData/걷기/" + str(nFileCnt) + "_y.txt"
        strFileNameZ = "SampleData/걷기/" + str(nFileCnt) + "_z.txt"
        fileX = open(strFileNameX)
        fileY = open(strFileNameY)
        fileZ = open(strFileNameZ)
        linesX = fileX.readlines()
        linesY = fileY.readlines()
        linesZ = fileZ.readlines()
        #for line in linesX:
        #    print(line)
        resultX[nFileCnt -1] = linesX
        resultY[nFileCnt -1] = linesY
        resultZ[nFileCnt -1] = linesZ
        fileX.close()
        fileY.close()
        fileZ.close()

    for nFileCnt in range(1,11):
        x1 = np.arange(0, len(resultX[nFileCnt -1]), 1)
        y1 = np.array(resultX[nFileCnt -1], np.float)
        strLabel = "x" + str(nFileCnt -1)
        pylab.figure(1)
        pylab.xlabel("10msec") # x축 이름
        pylab.ylabel("x") # y축 이름
        pylab.title('x data') # 제목
        pylab.legend() # 범례 표시
        pylab.grid(True)
        pylab.plot(x1, y1, label=strLabel)

        x2 = np.arange(0, len(resultY[nFileCnt -1]), 1)
        y2 = np.array(resultY[nFileCnt -1], np.float)
        strLabel = "y" + str(nFileCnt -1)
        pylab.figure(2)
        pylab.xlabel("10msec") # x축 이름
        pylab.ylabel("y") # y축 이름
        pylab.title('y data') # 제목
        pylab.legend() # 범례 표시
        pylab.grid(True)
        pylab.plot(x2, y2, label=strLabel)

        x3 = np.arange(0, len(resultZ[nFileCnt -1]), 1)
        y3 = np.array(resultZ[nFileCnt -1], np.float)
        strLabel = "z" + str(nFileCnt -1)
        pylab.figure(3)
        pylab.xlabel("10msec") # x축 이름
        pylab.ylabel("z") # y축 이름
        pylab.title('z data') # 제목
        pylab.legend() # 범례 표시
        pylab.grid(True)
        pylab.plot(x3, y3, label=strLabel)

    pylab.show()

    #input("Press the any button...")


# main 함수
if __name__ == "__main__":
    print("Entried MaldivesDemo function")

while 1:
    # example list
    print("=======================")
    print("0 : MaldivesDemo")
    print("1 : GetSampleData")
    print("q. exit")
    print("=======================")
    input_num = input("메뉴를 입력하세요 : ")

    if input_num == 'q':
        print("Exit Program")
        break

# example dictionary
    example_dic = {
                    0 : MaldivesDemo,
                    1 : GetSampleData,
                  }

    try:
        example_dic[int(input_num)]()
    except ValueError as e:
        print(str(e))
    except KeyError as e:
        print("Invalid input ", str(e))

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
