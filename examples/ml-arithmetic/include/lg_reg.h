/*
 * Copyright (C) 2014 Lukas R. Jackson, Vanguard Defense International
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef INCLUDE_LOGISTIC_REGRESSION_H
#define INCLUDE_LOGISTIC_REGRESSION_H

#include <stdint.h> // For fixed-point arithmetic

/**
 * @defgroup    lg_reg Logistic Regression
 * @brief       Functions and structures for logistic regression.
 * @{
 */

/**
 * @file
 * @brief       Declarations for logistic regression
 * @author      Lukas R. Jackson <LukasJacksonEG@gmail.com>
 */

typedef struct {
    int32_t x; ///< Feature value (scaled by a factor)
    int32_t y; ///< Target value (scaled by a factor)
} Point;

typedef struct {
    int32_t slope; ///< Slope parameter of the logistic regression model (scaled by a factor)
    int32_t intercept; ///< Intercept parameter of the logistic regression model (scaled by a factor)
} Parameters;

/**
 * @brief Sigmoid function.
 * 
 * @param z Input value (scaled by a factor).
 * @return The sigmoid of the input value (scaled by a factor).
 */
int32_t sigmoid(int32_t z);

/**
 * @brief Hypothesis function.
 * 
 * @param x Input feature value (scaled by a factor).
 * @param params Parameters of the logistic regression model (scaled by a factor).
 * @return The hypothesis value based on the input feature and model parameters (scaled by a factor).
 */
int32_t hypothesis(int32_t x, Parameters params);

/**
 * @brief Cost function for logistic regression.
 * 
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Parameters of the logistic regression model (scaled by a factor).
 * @return The cost of the model based on the given data and parameters (scaled by a factor).
 */
int32_t cost_function(Point *data, int num_points, Parameters params);

/**
 * @brief Update parameters using gradient descent.
 * 
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Pointer to the parameters of the logistic regression model to be updated (scaled by a factor).
 */
void gradient_descent(Point *data, int num_points, Parameters *params);

/**
 * @brief Perform logistic regression on the given data.
 * 
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @return Parameters of the logistic regression model (scaled by a factor).
 */
Parameters logistic_regression(Point *data, int num_points);

/** @} */

#endif /* INCLUDE_LOGISTIC_REGRESSION_H */
