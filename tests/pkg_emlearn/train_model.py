#!/usr/bin/env python3

import joblib

from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn import metrics, datasets

rnd = 42
digits = datasets.load_digits()
Xtrain, Xtest, ytrain, ytest = train_test_split(digits.data, digits.target, random_state=rnd)

print('Loading digits dataset. 8x8=64 features')

# 0.95+ with n_estimators=10, max_depth=10
trees = 10
max_depth = 10
print('Training {} trees with max_depth {}'.format(trees, max_depth))
model = RandomForestClassifier(n_estimators=trees, max_depth=max_depth, random_state=rnd)
model.fit(Xtrain, ytrain)

# Predict
ypred = model.predict(Xtest)
print('Accuracy on validation set {:.2f}%'.format(metrics.accuracy_score(ypred, ytest)*100))

# Store the model in a binary file
joblib.dump(model, "model")
