#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/lg_reg.h"

#define LEARNING_RATE 0.01
#define MAX_ITERATIONS 10000
#define EPSILON 0.0001

// Sigmoid function
double sigmoid(double z)
{
    return 1.0 / (1.0 + exp(-z));
}

// Hypothesis function
double hypothesis(double x, Parameters params)
{
    return sigmoid(params.slope * x + params.intercept);
}

// Cost function
double cost_function(Point *data, int num_points, Parameters params)
{
    double cost = 0.0;
    for (int i = 0; i < num_points; i++)
    {
        double h = hypothesis(data[i].x, params);
        cost += -data[i].y * log(h) - (1 - data[i].y) * log(1 - h);
    }
    return cost / num_points;
}

// Update parameters using gradient descent
void gradient_descent(Point *data, int num_points, Parameters *params)
{
    double slope_gradient = 0.0;
    double intercept_gradient = 0.0;
    for (int i = 0; i < num_points; i++)
    {
        double h = hypothesis(data[i].x, *params);
        slope_gradient += (h - data[i].y) * data[i].x;
        intercept_gradient += (h - data[i].y);
    }
    params->slope -= LEARNING_RATE * slope_gradient / num_points;
    params->intercept -= LEARNING_RATE * intercept_gradient / num_points;
}

// Logistic regression function
Parameters logistic_regression(Point *data, int num_points)
{
    Parameters params = {0, 0}; // Initial parameters
    int iterations = 0;
    double prev_cost = INFINITY;
    while (iterations < MAX_ITERATIONS)
    {
        gradient_descent(data, num_points, &params);
        double current_cost = cost_function(data, num_points, params);
        if (fabs(prev_cost - current_cost) < EPSILON)
        {
            break; // Convergence criteria
        }
        prev_cost = current_cost;
        iterations++;
    }
    return params;
}
