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

def DemoDeapLearning():
    print("0 : DemoDeapLearning")
    input("Press the any button...")

def step_function(x):
#    y = x > 0
#    return y.astype(np.int)
    return np.array(x > 0, dtype = np.int)

def DemoStepFunction():
    print("1 : DemoStepFunction")
    #x = np.array([-1.0, 1.0, 2.0])
    x = np.arange(-5.0, 5.0, 0.1)
    y = step_function(x)
    print("x =\n", x)
    print("step_function(x) =\n", y)
    plt.plot(x, y)
    plt.xlabel("x") # x축 이름
    plt.ylabel("y") # y축 이름
    plt.title('Step Function') # 제목
    plt.ylim(-0.1, 1.1) # y축 범위 지정
    plt.grid(True)
    plt.show()

def sigmoid(x):
    return 1 / (1 + np.exp(-x))

def DemoSigmoidFunction():
    print("2 : DemoSigmoidFunction")
    x = np.arange(-5.0, 5.0, 0.1)
    y = sigmoid(x)
    print("x =\n", x)
    print("sigmoid(x) =\n", y)
    plt.plot(x, y)
    plt.xlabel("x") # x축 이름
    plt.ylabel("y") # y축 이름
    plt.title('Sigmoid Function') # 제목
    plt.ylim(-0.1, 1.1) # y축 범위 지정
    plt.grid(True)
    plt.show()

def relu(x):
    return np.maximum(0,x) # maximum 입력 중 큰 값 return 함수

def DemoReluFunction():
    print("3 : DemoReluFunction")
    x = np.arange(-5.0, 5.0, 0.1)
    y = relu(x)
    print("x =\n", x)
    print("relu(x) =\n", y)
    plt.plot(x, y)
    plt.xlabel("x") # x축 이름
    plt.ylabel("y") # y축 이름
    plt.title('ReLU Function') # 제목
    plt.ylim(-1, 5) # y축 범위 지정
    plt.grid(True)
    plt.show()

def Demo행렬의내적():
    print("4 : Demo행렬의내적")
    A = np.array([[1,2],[3,4]])
    B = np.array([[5,6],[7,8]])
    print("A = \n", A)
    print("B = \n", B)
    print("A.shape = ", A.shape) # 배열의 형상
    print("B.shape = ", B.shape)
    print("A.ndim = ", A.ndim) # 배열의 차원수
    print("B.ndim = ", B.ndim)
    print("np.dot(A, B) = \n", np.dot(A, B)) # 배열의 내적
    input("Press the any button...")

def Demo신경망의내적():
    print("5 : Demo신경망의내적")
    X = np.array([1,2])
    W = np.array([[1,3,5],[2,4,6]])
    print("X = \n", X)
    print("W = \n", W)
    print("X.shape = ", X.shape)
    print("W.shape = ", W.shape)
    print("X.ndim = ", X.ndim)
    print("W.ndim = ", W.ndim)
    print("np.dot(X, W) = \n", np.dot(X, W))
    input("Press the any button...")

def init_net():
    network = {}
    network['W1'] = np.array([[0.1,0.3,0.5],[0.2,0.4,0.6]])
    network['b1'] = np.array([0.1,0.2,0.3])
    network['W2'] = np.array([[0.1,0.4],[0.2,0.5],[0.3,0.6]])
    network['b2'] = np.array([0.1,0.2])
    network['W3'] = np.array([[0.1,0.3],[0.2,0.4]])
    network['b3'] = np.array([0.1,0.2])

    return network

def identity_function(x): # 항등 함수
    return x

def forward(network, x):
    W1, W2, W3 = network['W1'], network['W2'], network['W3']
    b1, b2, b3 = network['b1'], network['b2'], network['b3']

    # 은닉층 1층
    a1 = np.dot(x, W1) + b1
    z1 = sigmoid(a1)

    # 은닉층 2층
    a2 = np.dot(z1, W2) + b2
    z2 = sigmoid(a2)

    # 출력층 3층
    a3 = np.dot(z2, W3) + b3
    y = identity_function(a3)

    return y

def Demo신경망구현하기():
    print("6 : Demo신경망구현하기")
    network = init_net()
    x = np.array([1.0, 0.5])
    y = forward(network, x)
    print("입력값 :", x)
    print("출력값 :", y)
    input("Press the any button...")

def softmax(a):
    c = np.max(a) # 오버플로 대책
    exp_a = np.exp(a - c) # 지수 함수
    sum_exp_a = np.sum(exp_a) # 지수 함수의 합
    y = exp_a / sum_exp_a
    return y

def Demo소프트맥스함수():
    print("7 : Demo소프트맥스함수")
    a = np.array([0.3, 2.9, 4.0])
    y = softmax(a)
    print("입력값 : ", a)
    print("결과값 : ", y)
    print("결과값의 총합 : ", np.sum(y))
    input("Press the any button...")

def img_show(img):
    pil_img = Image.fromarray(np.uint8(img))
    pil_img.show()

def DemoLoadMnist():
    print("8 : DemoLoadMnist")
    (x_train, t_train), (x_test, t_test) = load_mnist(flatten=True, normalize=False)

    # 각 데이터의 형상 출력
    print("x_train.shape = ", x_train.shape)
    print("t_train.shape = ", t_train.shape)
    print("x_test.shape = ", x_test.shape)
    print("t_test.shape = ", t_test.shape)

    # MNIST 훈련 이미지     중  0번째  이미지 출력
    img = x_train[0]
    label = t_train[0]
    print(label)
    print(img.shape)
    img = img.reshape(28, 28)
    print(img.shape)
    img_show(img)
    input("Press the any button...")

    # MNIST 중 테스트 이미지 셋 반환
    # (훈련이미지, 훈련레이블), (시험이미지, 시험레이블)
def get_data():
    (x_train, t_train), (x_test, t_test) = load_mnist(flatten=True, normalize=False)
    return x_test, t_test

    # 학습된 가중치 매개변수(sample_weight.pkl) 를 읽어온다.
    # 가중치와 편향 매개변수가 딕셔너리 변수로 저장되어 있다.
def init_network(): 
    with open("sample_weight.pkl", 'rb') as f: # with 블록을 벗어나면 자동으로 파일이 닫힌다. 
        network = pickle.load(f)
    return network

# 파이썬 사용중 생성된 객체를 저장할 경우 일반적으로 .pkl 로 저정한다.
# from cPickle import dump, load
# x = np.random.rand(10)
# f = open("x.pkl", "w")
# dump(x, f)
# f.close()

def predict(network, x):
    W1, W2, W3 = network['W1'], network['W2'], network['W3']
    b1, b2, b3 = network['b1'], network['b2'], network['b3']

    a1 = np.dot(x, W1) + b1
    z1 = sigmoid(a1)
    a2 = np.dot(z1, W2) + b2
    z2 = sigmoid(a2)
    a3 = np.dot(z2, W3) + b3
    y = softmax(a3)

    return y

def Demo신경망추론():
    print("9 : Demo신경망추론")
    x, t = get_data()
    network = init_network()

    accuracy_cnt = 0
    for i in range(len(x)):
        y = predict(network, x[i])
        p = np.argmax(y) # 확률이 가장 높은 원소의 인덱스를 얻는다.
        if p == t[i]:
            accuracy_cnt += 1
    print("정확도 : " + str(float(accuracy_cnt) / len(x)))
    input("Press the any button...")

def Demo배치처리():
    print("10 : Demo배치처리")
    x, t = get_data()
    network = init_network()

    batch_size = 100 #배치 크기
    accuracy_cnt = 0

    for i in range(0, len(x), batch_size):
        x_batch = x[i:i+batch_size]
        y_batch = predict(network, x_batch)
        p = np.argmax(y_batch, axis=1)
        accuracy_cnt += np.sum(p == t[i:i+batch_size])
    print("정확도 : " + str(float(accuracy_cnt) / len(x)))
    input("Press the any button...")

# main 함수
if __name__ == "__main__":
    print("Entried DeepLearning_demo function")

while 1:
    # example list
    print("=======================")
    print("0 : DemoDeapLearning")
    print("1 : DemoStepFunction")
    print("2 : DemoSigmoidFunction")
    print("3 : DemoReluFunction")
    print("4 : Demo행렬의내적")
    print("5 : Demo신경망의내적")
    print("6 : Demo신경망구현하기")
    print("7 : Demo소프트맥스함수")
    print("8 : DemoLoadMnist")
    print("9 : Demo신경망추론")
    print("10 : Demo배치처리")
    print("q. exit")
    print("=======================")
    input_num = input("메뉴를 입력하세요 : ")

    if input_num == 'q':
        print("Exit Program")
        break

# example dictionary
    example_dic = {
                    0 : DemoDeapLearning,
                    1 : DemoStepFunction,
                    2 : DemoSigmoidFunction,
                    3 : DemoReluFunction,
                    4 : Demo행렬의내적,
                    5 : Demo신경망의내적,
                    6 : Demo신경망구현하기,
                    7 : Demo소프트맥스함수,
                    8 : DemoLoadMnist,
                    9 : Demo신경망추론,
                    10 : Demo배치처리
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
#  파일명: DeepLearning_demp.py
#
#  설명:
#
# 
#  작성자:
#
#
#  날짜:
#      2017/12/22
#
