#!/usr/bin/env python3

import emlearn
import joblib

estimator = joblib.load("model")
cmodel = emlearn.convert(estimator)
cmodel.save(file='model.h')
