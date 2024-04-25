/**
 * @ingroup     lg_reg.c
 * @{
 *
 * @file
 * @brief       Definitions for logistic regression functions
 *
 * @author      Lukas R. Jackson <LukasJacksonEG@gmail.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/lg_reg.h"

#define LEARNING_RATE 0.01      ///< Learning rate for gradient descent
#define MAX_ITERATIONS 10000    ///< Maximum number of iterations for gradient descent
#define EPSILON 0.0001          ///< Convergence threshold for gradient descent

/**
 * @brief Sigmoid function.
 *
 * @param z Input value.
 * @return The sigmoid of the input value.
 */
double sigmoid(double z)
{
    return 1.0 / (1.0 + exp(-z));
}

/**
 * @brief Hypothesis function.
 *
 * @param x Input feature value.
 * @param params Parameters of the logistic regression model.
 * @return The hypothesis value based on the input feature and model parameters.
 */
double hypothesis(double x, Parameters params)
{
    return sigmoid(params.slope * x + params.intercept);
}

/**
 * @brief Cost function for logistic regression.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Parameters of the logistic regression model.
 * @return The cost of the model based on the given data and parameters.
 */
double cost_function(Point *data, int num_points, Parameters params)
{
    double cost = 0.0;

    for (int i = 0; i < num_points; i++) {
        double h = hypothesis(data[i].x, params);
        cost += -data[i].y * log(h) - (1 - data[i].y) * log(1 - h);
    }
    return cost / num_points;
}

/**
 * @brief Update parameters using gradient descent.
 *
 * @param data Array of points (feature-value pairs).
 * @param num_points Number of points in the dataset.
 * @param params Pointer to the parameters of the logistic regression model to be updated.
 */
void gradient_descent(Point *data, int num_points, Parameters *params)
{
    double slope_gradient = 0.0;
    double intercept_gradient = 0.0;

    for (int i = 0; i < num_points; i++) {
        double h = hypothesis(data[i].x, *params);
        slope_gradient += (h - data[i].y) * data[i].x;
        intercept_gradient += (h - data[i].y);
    }
    params->slope -= LEARNING_RATE * slope_gradient / num_points;
    params->intercept -= LEARNING_RATE * intercept_gradient / num_points;
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
    double prev_cost = INFINITY;

    while (iterations < MAX_ITERATIONS) {
        gradient_descent(data, num_points, &params);
        double current_cost = cost_function(data, num_points, params);
        if (fabs(prev_cost - current_cost) < EPSILON) {
            break; // Convergence criteria met
        }
        prev_cost = current_cost;
        iterations++;
    }
    return params;
}
