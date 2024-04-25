/**
 * @defgroup     lg_reg Logistic Regression
 * @brief        Definitions for logistic regression functions
 * @{
 *
 * @file
 * @brief        Definitions for logistic regression functions
 * @author       Lukas R. Jackson <LukasJacksonEG@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/lg_reg.h"

#define LEARNING_RATE_FIXED 100 ///< Learning rate for gradient descent (scaled by a factor)
#define MAX_ITERATIONS 10000    ///< Maximum number of iterations for gradient descent
#define EPSILON_FIXED 1         ///< Convergence threshold for gradient descent (scaled by a factor)

/**
 * @brief Sigmoid function.
 *
 * @param z Input value (scaled by a factor).
 * @return The sigmoid of the input value (scaled by a factor).
 */
int32_t sigmoid(int32_t z)
{
    // Scaling factor: 2^16
    const int32_t scale = 65536;
    return scale / (scale + exp(-z));
}

/**
 * @brief Hypothesis function.
 *
 * @param x Input feature value (scaled by a factor).
 * @param params Parameters of the logistic regression model (scaled by a factor).
 * @return The hypothesis value based on the input feature and model parameters (scaled by a factor).
 */
int32_t hypothesis(int32_t x, Parameters params)
{
    return sigmoid(params.slope * x + params.intercept);
}

/**
 * @brief Cost function for logistic regression.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Parameters of the logistic regression model (scaled by a factor).
 * @return The cost of the model based on the given data and parameters (scaled by a factor).
 */
int32_t cost_function(Point *data, int num_points, Parameters params)
{
    // Scaling factor: 2^16
    const int32_t scale = 65536;
    int32_t cost = 0;

    for (int i = 0; i < num_points; i++) {
        int32_t h = hypothesis(data[i].x, params);
        cost += -data[i].y * log(h / scale) - (scale - data[i].y) * log(1 - h / scale);
    }
    return cost / num_points;
}

/**
 * @brief Update parameters using gradient descent.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Pointer to the parameters of the logistic regression model to be updated (scaled by a factor).
 */
void gradient_descent(Point *data, int num_points, Parameters *params)
{
    // Scaling factor: 2^16
    const int32_t scale = 65536;
    int32_t slope_gradient = 0;
    int32_t intercept_gradient = 0;

    for (int i = 0; i < num_points; i++) {
        int32_t h = hypothesis(data[i].x, *params);
        slope_gradient += (h - data[i].y) * data[i].x;
        intercept_gradient += (h - data[i].y);
    }
    params->slope -= (LEARNING_RATE_FIXED * slope_gradient) / (num_points * scale);
    params->intercept -= (LEARNING_RATE_FIXED * intercept_gradient) / (num_points * scale);
}

/**
 * @brief Perform logistic regression on the given data.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @return Parameters of the logistic regression model.
 */
Parameters logistic_regression(Point *data, int num_points)
{
    Parameters params = { 0, 0 }; // Initial parameters
    int iterations = 0;
    int32_t prev_cost = INT32_MAX; // Largest positive value for int32_t

    while (iterations < MAX_ITERATIONS) {
        gradient_descent(data, num_points, &params);
        int32_t current_cost = cost_function(data, num_points, params);
        if (abs(prev_cost - current_cost) < EPSILON_FIXED) {
            break; // Convergence criteria met
        }
        prev_cost = current_cost;
        iterations++;
    }
    return params;
}

/** @} */
