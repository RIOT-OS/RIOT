#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H

// Structure to hold data points
typedef struct {
    double x;
    double y;
} Point;

// Structure to hold parameters
typedef struct {
    double slope;
    double intercept;
} Parameters;

// Sigmoid function
double sigmoid(double z);

// Hypothesis function
double hypothesis(double x, Parameters params);

// Cost function
double cost_function(Point *data, int num_points, Parameters params);

// Update parameters using gradient descent
void gradient_descent(Point *data, int num_points, Parameters *params);

// Logistic regression function
Parameters logistic_regression(Point *data, int num_points);

#endif /* LOGISTIC_REGRESSION_H */
