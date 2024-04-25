/*
 * Copyright (C) 2014 Lukas R. Jackson, Vanguard Defense International
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef INCLUDE_LOGISTIC_REGRESSION_H
#define INCLUDE_LOGISTIC_REGRESSION_H

/**
 * @ingroup     lg_reg.h
 * @{
 *
 * @file
 * @brief       Declarations for logistic regression
 *
 * @author      Lukas R. Jackson <LukasJacksonEG@gmail.com>
 *
 */

typedef struct {
    double x;   ///< Feature value
    double y;   ///< Target value
} Point;

typedef struct {
    double slope;       ///< Slope parameter of the logistic regression model
    double intercept;   ///< Intercept parameter of the logistic regression model
} Parameters;

/**
 * @brief Sigmoid function.
 *
 * @param z Input value.
 * @return The sigmoid of the input value.
 */
double sigmoid(double z);

/**
 * @brief Hypothesis function.
 *
 * @param x Input feature value.
 * @param params Parameters of the logistic regression model.
 * @return The hypothesis value based on the input feature and model parameters.
 */
double hypothesis(double x, Parameters params);

/**
 * @brief Cost function for logistic regression.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Parameters of the logistic regression model.
 * @return The cost of the model based on the given data and parameters.
 */
double cost_function(Point *data, int num_points, Parameters params);

/**
 * @brief Update parameters using gradient descent.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Pointer to the parameters of the logistic regression model to be updated.
 */
void gradient_descent(Point *data, int num_points, Parameters *params);

/**
 * @brief Perform logistic regression on the given data.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @return Parameters of the logistic regression model.
 */
Parameters logistic_regression(Point *data, int num_points);

#endif /* INCLUDE_LOGISTIC_REGRESSION_H */
