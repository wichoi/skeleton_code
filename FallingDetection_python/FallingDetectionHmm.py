#!/usr/bin/python3
# -*- coding: 949 -*-

from __future__ import absolute_import

import numpy as np
import pytest

from hmmlearn import hmm

#from . import log_likelihood_increasing, normalized

import FallingDetection as SampleData


# 각 행동별 HMM이 존재함 (걷기, 뛰기, 점프, 눕기, 앞으로 넘어지기, 뒤로 넘어지기, 옆으로 넘어지기)
# 행동별 데이터 사용하여 행동에 대한 HMM 학습
# 추론시에는 각 루프를 돌며 학습한 HMM을 모두 검색하여 확률이 높은 HMM을 찾는다.

class FallingDetectionHmm(object):
    def setup_method(self, method):
        self.n_components = 4                                                           # 은닉 상태 갯수
        self.n_features = 8                                                             # 출력 갯수
        self.h = hmm.MultinomialHMM(self.n_components)
        self.h.startprob_ =  np.array([0.1, 0.2, 0.3, 0.4])                             # 시작 확률 (확률의 합 1.0)
        self.h.transmat_ = np.array([
            [0.1, 0.2, 0.3, 0.4],
            [0.1, 0.2, 0.3, 0.4],
            [0.1, 0.2, 0.3, 0.4],
            [0.1, 0.2, 0.3, 0.4]
            ])     # 상태 전이 확률 4*4행렬, (확률의 합 1.0)
        self.h.emissionprob_ = np.array([
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3],
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3],
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3],
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3]
            ])   # 방출 확률

    def test_decode_viterbi(self, npArray):
        lengths = [1] * 500
        logprob, state_sequence = self.h.decode(npArray, lengths=lengths, algorithm="viterbi")
        print("logprob : ", logprob)
        print("state_sequence : ", state_sequence)

    def test_predict(self, npArray):
        lengths = [1] * 500
        state_sequence = self.h.predict(npArray, lengths=lengths)
        posteriors = self.h.predict_proba(npArray, lengths=lengths)
        print("state_sequence : ", state_sequence)
        print("posteriors : ", posteriors)


    def test_fit(self, npArray):
        npArray, state_sequence = self.h.sample(500)
        #print("sample : ", npArray)
        lengths = [1] * 500
        self.h._init(npArray)
        self.h.fit(npArray, lengths)


# main 함수
if __name__ == "__main__":
    print("===== FallingDetectionHmm main function =====")

    # data 저장 경로, 
    strFilePathWorking = "SampleData/걷기/"
    strFilePathsSitDownAndStandUp = "SampleData/앉았다일어나기/"

    npWorkingX, npWorkingY, npWorkingZ, npLowWorking = SampleData.ReadDataFromFile(strFilePathWorking)
    npSitNUpX, npSitNUpY, npSitNUpZ, npLowSitNUp = SampleData.ReadDataFromFile(strFilePathsSitDownAndStandUp)
    npResult = SampleData.DoSVMQuan(npWorkingX[0], npWorkingY[0], npWorkingY[0])
    npReshape = npResult.reshape(-1,1)

    model = FallingDetectionHmm()
    model.setup_method(model)

    #print("npReshape : ", npReshape)
    model.test_decode_viterbi(npReshape)
    model.test_predict(npReshape)
    model.test_fit(npReshape)



# ====================================================================
# The Aprotech Software License, Version 1.0 
# Copyright (c) 2018 Aprotech, Inc.  All rights reserved.
# ====================================================================
#
#  파일명: FallingDetectionHmm.py
#
#  설명:
#
# 
#  작성자:
#
#
#  날짜:
#      2018/02/02
#
