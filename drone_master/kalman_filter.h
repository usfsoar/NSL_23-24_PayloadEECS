#ifndef KALMANFILTER_H
#define KALMANFILTER_H

class KalmanFilter {
public:
    KalmanFilter(float process_noise, float measurement_noise, float estimated_error, float initial_value);
    float update(float measurement);
    bool checkOutlier(float measurement);

private:
    float Q; // Process noise
    float R; // Measurement noise
    float P; // Estimation error
    float K; // Kalman gain
    float value; // Filtered measurement
    float outlier_threshold; // Threshold for detecting outliers
};

#endif // KALMANFILTER_H
