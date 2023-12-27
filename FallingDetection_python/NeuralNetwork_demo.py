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
    plt.xlabel("x") # x�� �̸�
    plt.ylabel("y") # y�� �̸�
    plt.title('Step Function') # ����
    plt.ylim(-0.1, 1.1) # y�� ���� ����
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
    plt.xlabel("x") # x�� �̸�
    plt.ylabel("y") # y�� �̸�
    plt.title('Sigmoid Function') # ����
    plt.ylim(-0.1, 1.1) # y�� ���� ����
    plt.grid(True)
    plt.show()

def relu(x):
    return np.maximum(0,x) # maximum �Է� �� ū �� return �Լ�

def DemoReluFunction():
    print("3 : DemoReluFunction")
    x = np.arange(-5.0, 5.0, 0.1)
    y = relu(x)
    print("x =\n", x)
    print("relu(x) =\n", y)
    plt.plot(x, y)
    plt.xlabel("x") # x�� �̸�
    plt.ylabel("y") # y�� �̸�
    plt.title('ReLU Function') # ����
    plt.ylim(-1, 5) # y�� ���� ����
    plt.grid(True)
    plt.show()

def Demo����ǳ���():
    print("4 : Demo����ǳ���")
    A = np.array([[1,2],[3,4]])
    B = np.array([[5,6],[7,8]])
    print("A = \n", A)
    print("B = \n", B)
    print("A.shape = ", A.shape) # �迭�� ����
    print("B.shape = ", B.shape)
    print("A.ndim = ", A.ndim) # �迭�� ������
    print("B.ndim = ", B.ndim)
    print("np.dot(A, B) = \n", np.dot(A, B)) # �迭�� ����
    input("Press the any button...")

def Demo�Ű���ǳ���():
    print("5 : Demo�Ű���ǳ���")
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

def identity_function(x): # �׵� �Լ�
    return x

def forward(network, x):
    W1, W2, W3 = network['W1'], network['W2'], network['W3']
    b1, b2, b3 = network['b1'], network['b2'], network['b3']

    # ������ 1��
    a1 = np.dot(x, W1) + b1
    z1 = sigmoid(a1)

    # ������ 2��
    a2 = np.dot(z1, W2) + b2
    z2 = sigmoid(a2)

    # ����� 3��
    a3 = np.dot(z2, W3) + b3
    y = identity_function(a3)

    return y

def Demo�Ű�������ϱ�():
    print("6 : Demo�Ű�������ϱ�")
    network = init_net()
    x = np.array([1.0, 0.5])
    y = forward(network, x)
    print("�Է°� :", x)
    print("��°� :", y)
    input("Press the any button...")

def softmax(a):
    c = np.max(a) # �����÷� ��å
    exp_a = np.exp(a - c) # ���� �Լ�
    sum_exp_a = np.sum(exp_a) # ���� �Լ��� ��
    y = exp_a / sum_exp_a
    return y

def Demo����Ʈ�ƽ��Լ�():
    print("7 : Demo����Ʈ�ƽ��Լ�")
    a = np.array([0.3, 2.9, 4.0])
    y = softmax(a)
    print("�Է°� : ", a)
    print("����� : ", y)
    print("������� ���� : ", np.sum(y))
    input("Press the any button...")

def img_show(img):
    pil_img = Image.fromarray(np.uint8(img))
    pil_img.show()

def DemoLoadMnist():
    print("8 : DemoLoadMnist")
    (x_train, t_train), (x_test, t_test) = load_mnist(flatten=True, normalize=False)

    # �� �������� ���� ���
    print("x_train.shape = ", x_train.shape)
    print("t_train.shape = ", t_train.shape)
    print("x_test.shape = ", x_test.shape)
    print("t_test.shape = ", t_test.shape)

    # MNIST �Ʒ� �̹���     ��  0��°  �̹��� ���
    img = x_train[0]
    label = t_train[0]
    print(label)
    print(img.shape)
    img = img.reshape(28, 28)
    print(img.shape)
    img_show(img)
    input("Press the any button...")

    # MNIST �� �׽�Ʈ �̹��� �� ��ȯ
    # (�Ʒ��̹���, �Ʒ÷��̺�), (�����̹���, ���跹�̺�)
def get_data():
    (x_train, t_train), (x_test, t_test) = load_mnist(flatten=True, normalize=False)
    return x_test, t_test

    # �н��� ����ġ �Ű�����(sample_weight.pkl) �� �о�´�.
    # ����ġ�� ���� �Ű������� ��ųʸ� ������ ����Ǿ� �ִ�.
def init_network(): 
    with open("sample_weight.pkl", 'rb') as f: # with ����� ����� �ڵ����� ������ ������. 
        network = pickle.load(f)
    return network

# ���̽� ����� ������ ��ü�� ������ ��� �Ϲ������� .pkl �� �����Ѵ�.
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

def Demo�Ű���߷�():
    print("9 : Demo�Ű���߷�")
    x, t = get_data()
    network = init_network()

    accuracy_cnt = 0
    for i in range(len(x)):
        y = predict(network, x[i])
        p = np.argmax(y) # Ȯ���� ���� ���� ������ �ε����� ��´�.
        if p == t[i]:
            accuracy_cnt += 1
    print("��Ȯ�� : " + str(float(accuracy_cnt) / len(x)))
    input("Press the any button...")

def Demo��ġó��():
    print("10 : Demo��ġó��")
    x, t = get_data()
    network = init_network()

    batch_size = 100 #��ġ ũ��
    accuracy_cnt = 0

    for i in range(0, len(x), batch_size):
        x_batch = x[i:i+batch_size]
        y_batch = predict(network, x_batch)
        p = np.argmax(y_batch, axis=1)
        accuracy_cnt += np.sum(p == t[i:i+batch_size])
    print("��Ȯ�� : " + str(float(accuracy_cnt) / len(x)))
    input("Press the any button...")

# main �Լ�
if __name__ == "__main__":
    print("Entried DeepLearning_demo function")

while 1:
    # example list
    print("=======================")
    print("0 : DemoDeapLearning")
    print("1 : DemoStepFunction")
    print("2 : DemoSigmoidFunction")
    print("3 : DemoReluFunction")
    print("4 : Demo����ǳ���")
    print("5 : Demo�Ű���ǳ���")
    print("6 : Demo�Ű�������ϱ�")
    print("7 : Demo����Ʈ�ƽ��Լ�")
    print("8 : DemoLoadMnist")
    print("9 : Demo�Ű���߷�")
    print("10 : Demo��ġó��")
    print("q. exit")
    print("=======================")
    input_num = input("�޴��� �Է��ϼ��� : ")

    if input_num == 'q':
        print("Exit Program")
        break

# example dictionary
    example_dic = {
                    0 : DemoDeapLearning,
                    1 : DemoStepFunction,
                    2 : DemoSigmoidFunction,
                    3 : DemoReluFunction,
                    4 : Demo����ǳ���,
                    5 : Demo�Ű���ǳ���,
                    6 : Demo�Ű�������ϱ�,
                    7 : Demo����Ʈ�ƽ��Լ�,
                    8 : DemoLoadMnist,
                    9 : Demo�Ű���߷�,
                    10 : Demo��ġó��
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
#  ���ϸ�: DeepLearning_demp.py
#
#  ����:
#
# 
#  �ۼ���:
#
#
#  ��¥:
#      2017/12/22
#
