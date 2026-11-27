#!/usr/bin/env python3

import math

import emlearn
import joblib

estimator = joblib.load("model")
cmodel = emlearn.convert(estimator)

# emlearn truncates the thresholds to integers and compares with "<", while
# scikit-learn compares with "<=". As the pixels are integers,
# "pixel <= threshold" is "pixel < floor(threshold) + 1".
nodes, _, _ = cmodel.forest_
for node in nodes:
    node[1] = math.floor(node[1]) + 1

cmodel.save(file='model.h', name='model')
