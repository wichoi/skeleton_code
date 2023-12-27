#!/usr/bin/python3
# -*- coding: 949 -*-

# 인코딩 타입은 1,2 line 에만 있어야 함 (다른 line에 있으면 빌드 에러 발생)
# 파이썬 명령을 수행하는 방식은 '인터프리터' 와 '스크립트 파일' 두 가지가 방식이 있다
print("=======================")
print("!/usr/bin/python3")
print("-*- coding: 949 -*-")

# numpy 다차원 배열 객체를 다루는 메서드 제공
# matplotlib 그래프를 그려주는 그래픽 라이브러리
# import 모듈                    << 모듈 전체 import
# from 모듈 import 변수(함수)  << 필요한 변수(함수) import
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.image import imread

# scalar : 크기만 존재하는 1차원의 수
# vector : 1차원 배열
# matrix : vector 가 n개인 경우, 2차원 이상의 배열
# tensor : 좌표 독립성(불변성)을 같는 다중 백터 모음

def Demo아나콘다배포판():
    print("0. 아나콘다 배포판")
    img = imread('images\아나콘다_배포판.png')
    plt.imshow(img)
    plt.title('아나콘다_배포판')
    plt.show()

def DemoNdarray():
    print("1. ndarray demo")
    # numpy 라이브러리가 제공하는 리스트
    # ndarray : n차원 배열 객체
    x = np.array([1.0, 2.0, 3.0 ])
    y = np.array([2.0, 4.0, 6.0 ])
    print("x = " , x)
    print("y = " , y)
    type(x)

    # ndarray n차원 배열
    # shape : 행 * 열 정보
    # dtype : 원소의 자료형
    A = np.array([[1,2],[3,4]])
    print("A = \n" , A)
    print("A.shape = \n" , A.shape)
    print("A.dtype = \n" , A.dtype)
    input("Press the button...")

def DemoOperators():
    print("2. arithmetic operators demo")
    # ndarray 산술 연산자
    # 배열의 원소별로 행해짐 (원소가 다르면 오류 발생)
    x = np.array([1.0, 2.0, 3.0 ])
    y = np.array([2.0, 4.0, 6.0 ])
    print("x = " , x)
    print("y = " , y)
    print("x + y = " , x + y)
    print("x - y = " , x - y)
    print("x * y = " , x * y)
    print("x / y = " , x / y)

    # ndarray n차원 배열의 산술 연산
    # 배열의 원소별로 행해짐
    A = np.array([[1,2],[3,4]])
    B = np.array([[1,2],[3,4]])
    print("A = \n" , A)
    print("B = \n" , B)
    print("A + B = \n" , A + B)
    print("A * B = \n" , A * B)

    # ndarray 부등호 연산자
    X = np.array([[51, 55], [14, 19], [0,4]])
    print("X> 15 = \n" , X > 15)
    print("X[X> 15] = \n" , X[X > 15])
    input("Press the button...")

def DemoBroadcast():
    print("3. broadcast demo")
    # ndarray 브로드캐스트
    # 배열과 수치 하나(스칼라값) 조합으로 산술 연산 수행
    x = np.array([1.0, 2.0, 3.0 ])
    print("x = " , x)
    print("x / 2.0 =", x / 2.0)

    # ndarray n차원 배열의 브로드케스트
    A = np.array([[1,2],[3,4]])
    D = np.array([10, 20])
    print("A = \n" , A)
    print("D = \n" , D)
    print("A * D = \n" , A * D)
    input("Press the button...")

def DemoNdarray원소접근():
    print("4. ndarray 원소접근 demo")
    # ndarray 원소 접근
    X = np.array([[51, 55], [14, 19], [0,4]])
    print("X = \n" , X)
    print("X[0] = \n" , X[0])
    print("X[0][1] = \n" , X[0][1])
    
    print("X row =")
    for row in X:
        print(row)

    # X를 1차원 배열로 변환
    X = X.flatten()
    print("X.flatten() = \n" , X)

    # index 가 0,2,4 인 원소 얻기
    print("X[0], X[2], X[4]= \n", X[np.array([0,2,4])])
    input("Press the button...")

def DemoMatplotlib1():
    print("5. matplotlib demo 1")
    # 0에서 6까지 0.1 간격 (0.1, 0.2 ... 5.8, 5.9)
    x = np.arange(0, 6, 0.1)
    y = np.sin(x) # sin 함수
    print("x = \n" , x)
    print("y = \n" , y)

    # 그래프 그리기
    plt.plot(x, y)
    plt.show()

def DemoMatplotlib2():
    print("6. matplotlib demo 2")
    x = np.arange(0, 6, 0.1)
    y1 = np.sin(x) # sin 함수
    y2 = np.cos(x) # cos 함수
    print("y1 = \n" , y1)
    print("y2 = \n" , y2)

    plt.plot(x, y1, label="sin")
    plt.plot(x, y2, linestyle="--", label="cos") # cos 함수는 점선으로 그리기
    plt.xlabel("x") # x축 이름
    plt.ylabel("y") # y축 이름
    plt.title('sin & cos') # 제목
    plt.legend() # 범례 표시
    plt.grid(True)
    plt.show()

def DemoMatplotlibImage():
    print("7. matplotlib image demo")
    img = imread('images\lena.png')
    plt.imshow(img)
    plt.title('lena.png')
    plt.show()

def DemoPerceptron1():
    print("8. perceptron algorism 1")
    img = imread('images\perceptron1.png')
    plt.imshow(img)
    plt.title('perceptron1')
    plt.show()

def DemoPerceptron2():
    print("9. perceptron algorism 2")
    img = imread('images\perceptron2.png')
    plt.imshow(img)
    plt.title('perceptron2')
    plt.show()

# AND 함수
# w1, w2 에 0.5 의 가중치 부여
# theta 값은 0.7
# 연산 결과가 theta 보다 크면 true, 작거나 같으면 false
def AND(x1, x2):
    w1, w2, theta = 0.5, 0.5, 0.7
    tmp = x1 * w1 + x2 * w2
    if tmp <= theta:
        return 0
    elif tmp > theta:
        return 1

# 가중치(weight)와 편향(bias)
# AND, NAND, OR 형식은 동일하고 가중치와 편향 값만 다르다.
def AND_bias(x1, x2):
    x = np.array([x1, x2])      # 입력값
    w = np.array([0.5, 0.5])    # 가중치
    b = -0.7                    # 편향
    tmp = np.sum(w*x) + b
    if tmp <= 0:
        return 0
    else:
        return 1

def NAND(x1, x2):
    x = np.array([x1, x2])          # 입력값
    w = np.array([-0.5, -0.5])      # 가중치
    b = 0.7                         # 편향
    tmp = np.sum(w*x) + b
    if tmp <= 0:
        return 0
    else:
        return 1

def OR(x1, x2):
    x = np.array([x1, x2])          # 입력값
    w = np.array([0.5, 0.5])        # 가중치
    b = -0.2                        # 편향
    tmp = np.sum(w*x) + b
    if tmp <= 0:
        return 0
    else:
        return 1

def XOR(x1, x2):
    s1 = NAND(x1, x2)
    s2 = OR(x1, x2)
    y = AND(s1, s2)
    return y;

def DemoPerceptronAND1():
    print("10. perceptron AND")
    print("AND(0,0) = ", AND(0,0))
    print("AND(1,0) = ", AND(1,0))
    print("AND(0,1) = ", AND(0,1))
    print("AND(1,1) = ", AND(1,1))
    input("Press the button...")

def DemoPerceptronAND2():
    print("11. perceptron AND 2")
    print("AND_bias(0,0) = ", AND_bias(0,0))
    print("AND_bias(1,0) = ", AND_bias(1,0))
    print("AND_bias(0,1) = ", AND_bias(0,1))
    print("AND_bias(1,1) = ", AND_bias(1,1))
    input("Press the button...")

def DemoPerceptronNAND():
    print("12. perceptron NAND")
    print("NAND(0,0) = ", NAND(0,0))
    print("NAND(1,0) = ", NAND(1,0))
    print("NAND(0,1) = ", NAND(0,1))
    print("NAND(1,1) = ", NAND(1,1))
    input("Press the button...")

def DemoPerceptronOR():
    print("13. perceptron OR")
    print("OR(0,0) = ", OR(0,0))
    print("OR(1,0) = ", OR(1,0))
    print("OR(0,1) = ", OR(0,1))
    print("OR(1,1) = ", OR(1,1))
    input("Press the button...")

def Demo퍼셉트론의한계():
    print("14.. 퍼셉트론의 한계")
    img = imread('images\퍼셉트론의한계.png')
    plt.imshow(img)
    plt.title('퍼셉트론의한계')
    plt.show()

def DemoMultiLayerPerceptron():
    print("15. multi layer perceptron")
    img = imread('images\MultiLayerPerceptron.png')
    plt.imshow(img)
    plt.title('MultiLayerPerceptron')
    plt.show()

def DemoPerceptronXOR():
    print("16. perceptron XOR")
    print("XOR(0,0) = ", XOR(0,0))
    print("XOR(1,0) = ", XOR(1,0))
    print("XOR(0,1) = ", XOR(0,1))
    print("XOR(1,1) = ", XOR(1,1))
    input("Press the button...")

def DemoPerceptronSummary():
    print("17. perceptron summary")
    img = imread('images\PerceptronSummary.png')
    plt.imshow(img)
    plt.title('PerceptronSummary')
    plt.show()

# main 함수
if __name__ == "__main__":
    print("Entried phthon_demo function")
    while 1:
        # example list
        print("=======================")
        print("0. 아나콘다 배포판")
        print("1. ndarray demo")
        print("2. arithmetic operators demo")
        print("3. broadcast demo")
        print("4. ndarray 원소접근 demo")
        print("5. matplotlib demo 1")
        print("6. matplotlib demo 2")
        print("7. matplotlib image demo")
        print("8. perceptron algorism 1")
        print("9. perceptron algorism 2")
        print("10. perceptron AND 1")
        print("11. perceptron AND 2")
        print("12. perceptron NAND")
        print("13. perceptron OR")
        print("14. 퍼셉트론의 한계")
        print("15. multi layer perceptron")
        print("16. perceptron XOR")
        print("17. perceptron summary")
        print("q. exit")
        print("=======================")
        input_num = input("메뉴를 입력하세요 : ")

        if input_num == 'q':
            print("Exit Program")
            break

        # example dictionary
        example_dic = {
                        0 : Demo아나콘다배포판,
                        1 : DemoNdarray,
                        2 : DemoOperators,
                        3 : DemoBroadcast,
                        4 : DemoNdarray원소접근,
                        5 : DemoMatplotlib1,
                        6 : DemoMatplotlib2,
                        7 : DemoMatplotlibImage,
                        8 : DemoPerceptron1,
                        9 : DemoPerceptron2,
                        10 : DemoPerceptronAND1,
                        11 : DemoPerceptronAND2,
                        12 : DemoPerceptronNAND,
                        13 : DemoPerceptronOR,
                        14 : Demo퍼셉트론의한계,
                        15 : DemoMultiLayerPerceptron,
                        16 : DemoPerceptronXOR,
                        17 : DemoPerceptronSummary,
                      }

        try:
            example_dic[int(input_num)]()
        except ValueError as e:
            print(str(e))
        except KeyError as e:
            print("Invalid input ", str(e))

# ====================================================================
# The Aprotech Software License, Version 1.0 
# Copyright (c) 2017 Aprotech, Inc.  All rights reserved.
# ====================================================================
#
#  파일명: python_deom.py
#
#  설명:
#
# 
#  작성자:
#
#
#  날짜:
#      2017/12/20
#
