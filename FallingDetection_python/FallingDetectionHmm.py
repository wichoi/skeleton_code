#!/usr/bin/python3
# -*- coding: 949 -*-

from __future__ import absolute_import

import numpy as np
import pytest

from hmmlearn import hmm

#from . import log_likelihood_increasing, normalized

import FallingDetection as SampleData


# �� �ൿ�� HMM�� ������ (�ȱ�, �ٱ�, ����, ����, ������ �Ѿ�����, �ڷ� �Ѿ�����, ������ �Ѿ�����)
# �ൿ�� ������ ����Ͽ� �ൿ�� ���� HMM �н�
# �߷нÿ��� �� ������ ���� �н��� HMM�� ��� �˻��Ͽ� Ȯ���� ���� HMM�� ã�´�.

class FallingDetectionHmm(object):
    def setup_method(self, method):
        self.n_components = 4                                                           # ���� ���� ����
        self.n_features = 8                                                             # ��� ����
        self.h = hmm.MultinomialHMM(self.n_components)
        self.h.startprob_ =  np.array([0.1, 0.2, 0.3, 0.4])                             # ���� Ȯ�� (Ȯ���� �� 1.0)
        self.h.transmat_ = np.array([
            [0.1, 0.2, 0.3, 0.4],
            [0.1, 0.2, 0.3, 0.4],
            [0.1, 0.2, 0.3, 0.4],
            [0.1, 0.2, 0.3, 0.4]
            ])     # ���� ���� Ȯ�� 4*4���, (Ȯ���� �� 1.0)
        self.h.emissionprob_ = np.array([
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3],
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3],
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3],
            [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3]
            ])   # ���� Ȯ��

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


# main �Լ�
if __name__ == "__main__":
    print("===== FallingDetectionHmm main function =====")

    # data ���� ���, 
    strFilePathWorking = "SampleData/�ȱ�/"
    strFilePathsSitDownAndStandUp = "SampleData/�ɾҴ��Ͼ��/"

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
#  ���ϸ�: FallingDetectionHmm.py
#
#  ����:
#
# 
#  �ۼ���:
#
#
#  ��¥:
#      2018/02/02
#
