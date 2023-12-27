#!/usr/bin/python3
# -*- coding: 949 -*-

# ���ڵ� Ÿ���� 1,2 line ���� �־�� �� (�ٸ� line�� ������ ���� ���� �߻�)
# ���̽� ����� �����ϴ� ����� '����������' �� '��ũ��Ʈ ����' �� ������ ����� �ִ�
print("=======================")
print("!/usr/bin/python3")
print("-*- coding: 949 -*-")

# numpy ������ �迭 ��ü�� �ٷ�� �޼��� ����
# matplotlib �׷����� �׷��ִ� �׷��� ���̺귯��
# import ���                    << ��� ��ü import
# from ��� import ����(�Լ�)  << �ʿ��� ����(�Լ�) import
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.image import imread

# scalar : ũ�⸸ �����ϴ� 1������ ��
# vector : 1���� �迭
# matrix : vector �� n���� ���, 2���� �̻��� �迭
# tensor : ��ǥ ������(�Һ���)�� ���� ���� ���� ����

def Demo�Ƴ��ܴٹ�����():
    print("0. �Ƴ��ܴ� ������")
    img = imread('images\�Ƴ��ܴ�_������.png')
    plt.imshow(img)
    plt.title('�Ƴ��ܴ�_������')
    plt.show()

def DemoNdarray():
    print("1. ndarray demo")
    # numpy ���̺귯���� �����ϴ� ����Ʈ
    # ndarray : n���� �迭 ��ü
    x = np.array([1.0, 2.0, 3.0 ])
    y = np.array([2.0, 4.0, 6.0 ])
    print("x = " , x)
    print("y = " , y)
    type(x)

    # ndarray n���� �迭
    # shape : �� * �� ����
    # dtype : ������ �ڷ���
    A = np.array([[1,2],[3,4]])
    print("A = \n" , A)
    print("A.shape = \n" , A.shape)
    print("A.dtype = \n" , A.dtype)
    input("Press the button...")

def DemoOperators():
    print("2. arithmetic operators demo")
    # ndarray ��� ������
    # �迭�� ���Һ��� ������ (���Ұ� �ٸ��� ���� �߻�)
    x = np.array([1.0, 2.0, 3.0 ])
    y = np.array([2.0, 4.0, 6.0 ])
    print("x = " , x)
    print("y = " , y)
    print("x + y = " , x + y)
    print("x - y = " , x - y)
    print("x * y = " , x * y)
    print("x / y = " , x / y)

    # ndarray n���� �迭�� ��� ����
    # �迭�� ���Һ��� ������
    A = np.array([[1,2],[3,4]])
    B = np.array([[1,2],[3,4]])
    print("A = \n" , A)
    print("B = \n" , B)
    print("A + B = \n" , A + B)
    print("A * B = \n" , A * B)

    # ndarray �ε�ȣ ������
    X = np.array([[51, 55], [14, 19], [0,4]])
    print("X> 15 = \n" , X > 15)
    print("X[X> 15] = \n" , X[X > 15])
    input("Press the button...")

def DemoBroadcast():
    print("3. broadcast demo")
    # ndarray ��ε�ĳ��Ʈ
    # �迭�� ��ġ �ϳ�(��Į��) �������� ��� ���� ����
    x = np.array([1.0, 2.0, 3.0 ])
    print("x = " , x)
    print("x / 2.0 =", x / 2.0)

    # ndarray n���� �迭�� ��ε��ɽ�Ʈ
    A = np.array([[1,2],[3,4]])
    D = np.array([10, 20])
    print("A = \n" , A)
    print("D = \n" , D)
    print("A * D = \n" , A * D)
    input("Press the button...")

def DemoNdarray��������():
    print("4. ndarray �������� demo")
    # ndarray ���� ����
    X = np.array([[51, 55], [14, 19], [0,4]])
    print("X = \n" , X)
    print("X[0] = \n" , X[0])
    print("X[0][1] = \n" , X[0][1])
    
    print("X row =")
    for row in X:
        print(row)

    # X�� 1���� �迭�� ��ȯ
    X = X.flatten()
    print("X.flatten() = \n" , X)

    # index �� 0,2,4 �� ���� ���
    print("X[0], X[2], X[4]= \n", X[np.array([0,2,4])])
    input("Press the button...")

def DemoMatplotlib1():
    print("5. matplotlib demo 1")
    # 0���� 6���� 0.1 ���� (0.1, 0.2 ... 5.8, 5.9)
    x = np.arange(0, 6, 0.1)
    y = np.sin(x) # sin �Լ�
    print("x = \n" , x)
    print("y = \n" , y)

    # �׷��� �׸���
    plt.plot(x, y)
    plt.show()

def DemoMatplotlib2():
    print("6. matplotlib demo 2")
    x = np.arange(0, 6, 0.1)
    y1 = np.sin(x) # sin �Լ�
    y2 = np.cos(x) # cos �Լ�
    print("y1 = \n" , y1)
    print("y2 = \n" , y2)

    plt.plot(x, y1, label="sin")
    plt.plot(x, y2, linestyle="--", label="cos") # cos �Լ��� �������� �׸���
    plt.xlabel("x") # x�� �̸�
    plt.ylabel("y") # y�� �̸�
    plt.title('sin & cos') # ����
    plt.legend() # ���� ǥ��
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

# AND �Լ�
# w1, w2 �� 0.5 �� ����ġ �ο�
# theta ���� 0.7
# ���� ����� theta ���� ũ�� true, �۰ų� ������ false
def AND(x1, x2):
    w1, w2, theta = 0.5, 0.5, 0.7
    tmp = x1 * w1 + x2 * w2
    if tmp <= theta:
        return 0
    elif tmp > theta:
        return 1

# ����ġ(weight)�� ����(bias)
# AND, NAND, OR ������ �����ϰ� ����ġ�� ���� ���� �ٸ���.
def AND_bias(x1, x2):
    x = np.array([x1, x2])      # �Է°�
    w = np.array([0.5, 0.5])    # ����ġ
    b = -0.7                    # ����
    tmp = np.sum(w*x) + b
    if tmp <= 0:
        return 0
    else:
        return 1

def NAND(x1, x2):
    x = np.array([x1, x2])          # �Է°�
    w = np.array([-0.5, -0.5])      # ����ġ
    b = 0.7                         # ����
    tmp = np.sum(w*x) + b
    if tmp <= 0:
        return 0
    else:
        return 1

def OR(x1, x2):
    x = np.array([x1, x2])          # �Է°�
    w = np.array([0.5, 0.5])        # ����ġ
    b = -0.2                        # ����
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

def Demo�ۼ�Ʈ�����Ѱ�():
    print("14.. �ۼ�Ʈ���� �Ѱ�")
    img = imread('images\�ۼ�Ʈ�����Ѱ�.png')
    plt.imshow(img)
    plt.title('�ۼ�Ʈ�����Ѱ�')
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

# main �Լ�
if __name__ == "__main__":
    print("Entried phthon_demo function")
    while 1:
        # example list
        print("=======================")
        print("0. �Ƴ��ܴ� ������")
        print("1. ndarray demo")
        print("2. arithmetic operators demo")
        print("3. broadcast demo")
        print("4. ndarray �������� demo")
        print("5. matplotlib demo 1")
        print("6. matplotlib demo 2")
        print("7. matplotlib image demo")
        print("8. perceptron algorism 1")
        print("9. perceptron algorism 2")
        print("10. perceptron AND 1")
        print("11. perceptron AND 2")
        print("12. perceptron NAND")
        print("13. perceptron OR")
        print("14. �ۼ�Ʈ���� �Ѱ�")
        print("15. multi layer perceptron")
        print("16. perceptron XOR")
        print("17. perceptron summary")
        print("q. exit")
        print("=======================")
        input_num = input("�޴��� �Է��ϼ��� : ")

        if input_num == 'q':
            print("Exit Program")
            break

        # example dictionary
        example_dic = {
                        0 : Demo�Ƴ��ܴٹ�����,
                        1 : DemoNdarray,
                        2 : DemoOperators,
                        3 : DemoBroadcast,
                        4 : DemoNdarray��������,
                        5 : DemoMatplotlib1,
                        6 : DemoMatplotlib2,
                        7 : DemoMatplotlibImage,
                        8 : DemoPerceptron1,
                        9 : DemoPerceptron2,
                        10 : DemoPerceptronAND1,
                        11 : DemoPerceptronAND2,
                        12 : DemoPerceptronNAND,
                        13 : DemoPerceptronOR,
                        14 : Demo�ۼ�Ʈ�����Ѱ�,
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
#  ���ϸ�: python_deom.py
#
#  ����:
#
# 
#  �ۼ���:
#
#
#  ��¥:
#      2017/12/20
#
