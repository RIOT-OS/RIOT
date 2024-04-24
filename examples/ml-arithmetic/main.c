#include "include/lg_reg.h"
#include <stdio.h>

int main(void)
{
    // Example data points
    Point data[] = {{1, 0}, {2, 0}, {3, 0}, {4, 1}, {5, 1}};
    int num_points = sizeof(data) / sizeof(data[0]);

    // Training logistic regression model
    Parameters params = logistic_regression(data, num_points);

    // Printing the parameters of the model
    printf("Slope: %f\n", params.slope);
    printf("Intercept: %f\n", params.intercept);

    return 0;
}
