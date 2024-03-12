#include "simple_kalman_filter.h"
#include <cmath>

KalmanFilter::KalmanFilter(float process_noise, float measurement_noise, float estimated_error, int min_init)
{
    Q = process_noise;
    R = measurement_noise;
    P = estimated_error;
    value = 0.0f;              // Initialize with 0, will be updated on first measurement
    initialized = false;       // Flag to indicate if the filter is initialized
    min_init_count = min_init; // Minimum number of measurements to initialize the filter
    init_buffer = new float[min_init_count];
    init_buffer_index = 0;
    /* Arbitrary threshold for outlier detection, adjust based on your data */
    outlier_threshold = 10.0;
}

float KalmanFilter::update(float measurement)
{
    if (!initialized)
    {
        // If buffer is not full, add the measurement to the buffer
        if (init_buffer_index < min_init_count)
        {
            init_buffer[init_buffer_index] = measurement;
            init_buffer_index++;
        }
        else
        {
            // Sort the buffer
            for (int i = 0; i < min_init_count; i++)
            {
                for (int j = i + 1; j < min_init_count; j++)
                {
                    if (init_buffer[i] > init_buffer[j])
                    {
                        float temp = init_buffer[i];
                        init_buffer[i] = init_buffer[j];
                        init_buffer[j] = temp;
                    }
                }
            }
            // Calculate the median
            value = init_buffer[min_init_count / 2];
            initialized = true;
        }
    }
    else
    {
        // Prediction update
        /* No actual prediction step because we assume a simple model */

        // Measurement update
        K = P / (P + R);
        value = value + K * (measurement - value);
        P = (1 - K) * P + Q;
    }

    return value;
}

bool KalmanFilter::checkOutlier(float measurement)
{
    return fabs(measurement - value) > outlier_threshold;
}
