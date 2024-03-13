#ifndef SIMPLE_KALMAN_FILTER_H
#define SIMPLE_KALMAN_FILTER_H

class KalmanFilter {
public:
    bool initialized; // Flag to indicate if the filter has been initialized
    KalmanFilter(float process_noise, float measurement_noise, float estimated_error, int min_init = 10);
    float update(float measurement);
    bool checkOutlier(float measurement);

private:
    float Q; // Process noise
    float R; // Measurement noise
    float P; // Estimation error
    float K; // Kalman gain
    float value; // Filtered measurement
    int min_init_count; // Minimum number of measurements to initialize the filter
    float* init_buffer;
    int init_buffer_index;
    float outlier_threshold; // Threshold for detecting outliers
};

#endif // SIMPLE_KALMAN_FILTER_H
