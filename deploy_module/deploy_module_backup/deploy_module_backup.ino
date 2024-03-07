
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <queue>
#include "buzzer_notify.h"
#include "MyVL53L0X.h"

// TODO: Get rid of whatever this library is doing
#include <HardwareSerial.h>
#include "soar_barometer.h"
#include "DCMotor.h"
#include "ota_update.h"
#include "SOAR_Lora.h"

#define DEBUG_ALT true
#define DEBUG_BUZZ false
#define DEBUG_TRSHSET false
#define TEST_MOTOR false
#define TEST_MOTOR_BACK false
#define FAKE_ALT_DATA true
#if FAKE_ALT_DATA
float fake_data[] = {2.49, 5.809, 7.469, 9.96, 12.452, 15.775, 18.268, 22.424, 25.75, 28.246, 30.741, 35.735, 38.233, 41.564, 44.896, 49.897, 54.065, 57.402, 60.739, 66.582, 70.758, 75.771, 79.114, 84.131, 87.477, 91.66, 95.846, 100.871, 106.32119178180086, 112.606, 118.478, 122.674, 132.54131153416057, 2604.2163792140695, 136.115, 141.159, 147.047, 153.781, 158.834, 164.732, 169.791, 176.539, 180.759, 185.825, 190.894, 194.275, 201.039, 206.961, 212.041, 217.969, 225.597, 232.382, 236.624, 241.718, 245.965, 245.22883551450047, 255.313, 264.05039825007333, 268.075, 274.036, 280.0, 285.6525872546529, 289.38, 295.353, 301.33, 307.309, 309.28103644350864, 319.28, 326.126, 333.835, 339.834, 344.12, 349.26640066204385, 355.275, 361.286, 368.161, 375.04, 380.202, 385.367, 391.396, 396.566, 403.464, 409.504, 414.683, 419.865, 423.321, 429.372, 435.427, 441.485, 446.681, 451.879, 457.08, 461.416, 468.357, 473.567, 477.909, 483.123, 489.21, 7314.2284541000345, 500.522, 505.748, 509.233, 514.463, 519.696, 524.931, 532.788, 537.156, 540.652, 545.898, 551.146, 555.522, 562.527, 566.908, 570.414, 574.798, 580.061, 584.449, 590.596, 595.867, 599.383, 602.6088128665627, 608.178, 612.578, 617.861, 623.146, 625.789, 629.316, 634.607, 639.4121035070269, 642.549, 647.847, 652.264, 655.799, 659.336, 663.758, 668.182, 672.608, 678.807, 682.352, 685.011, 688.557, 692.992, 696.541, 701.867, 707.196, 710.75, 13523.70492526443, 717.862, 721.42, 725.869, 729.429, 734.772, 737.446, 740.119, 743.684, 748.143, 750.819, 756.173, 760.637, 764.21, 765.997, 770.465, 773.148, 776.724, 779.408, 782.093, 787.463, 14956.628535590677, 792.836, 796.421, 797.7868039101896, 802.696, 805.386, 808.974, 813.462, 817.053, 817.951, 826.3434954564773, 824.239, 827.834, 830.531, 833.229, 836.827, 841.326, 844.027, 845.828, 848.529, 851.232, 853.935, 856.639, 859.344, 862.952, 867.462, 869.267, 871.072, 872.878, 875.587, 878.297, 881.007, 882.814, 885.526, 888.239, 892.761, 890.8502864980268, 893.8537108003324, 898.19, 900.0, 902.716, 902.448336329124, 906.339, 909.057, 910.869, 913.588, 917.215, 919.936, 920.843, 921.75, 923.564, 925.379, 928.102, 11302.866003647108, 931.734, 933.55, 935.4552195620986, 937.184, 939.001, 942.637, 944.456, 944.456, 945.365, 947.183, 949.003, 950.822, 952.642, 953.552, 955.372, 957.192, 958.103, 960.835, 963.567, 965.389, 966.3, 966.3, 967.212, 968.123, 969.945, 971.769, 973.592, 974.504, 975.416, 976.328, 979.2736250863941, 978.152, 979.977, 982.714, 984.54, 984.54, 985.453, 985.453, 986.366, 987.279, 988.192, 989.105, 990.019, 990.932, 991.845, 992.759, 992.759, 992.759, 993.672, 995.5, 997.327, 998.241, 999.155, 999.155, 998.241, 999.155, 999.155, 1000.069, 1000.069, 1000.984, 1001.898, 1001.898, 1002.813, 1002.813, 1002.813, 1002.813, 1002.813, 1003.727, 1004.642, 1005.556, 1006.471, 1006.471, 1006.471, 1005.556, 1005.556, 1005.556, 1005.556, 1005.556, 921.75, 995.5, 1007.386, 1007.386, 1007.386, 1008.301, 1007.386, 1006.471, 1005.556, 1005.556, 1005.556, 1006.471, 1008.301, 1009.216, 1010.13, 1008.301, 11370.98929978838, 1005.556, 1001.435640242259, 1003.727, 1003.727, 1002.080702991261, 1004.642, 1004.642, 1004.642, 1001.326159599467, 1003.727, 1002.813, 1000.984, 998.241, 995.5, 994.586, 994.586, 995.5, 996.413, 995.5, 994.586, 992.759, 990.932, 990.932, 990.019, 990.019, 989.105, 988.192, 987.279, 987.279, 986.366, 986.366, 985.453, 984.54, 983.627, 983.627, 15761.026634367467, 980.89, 979.977, 979.065, 977.24, 975.416, 973.592, 971.769, 969.945, 968.123, 967.212, 965.389, 965.389, 964.478, 963.567, 962.656, 960.835, 959.924, 958.103, 952.8367743262451, 955.372, 954.462, 953.552, 952.642, 12987.630824562635, 947.269569041068, 950.822, 950.822, 949.912, 949.003, 947.183, 946.274, 942.637, 939.7902931403034, 939.91, 938.093, 939.001, 939.91, 938.093, 934.458, 11091.83937666005, 930.826, 929.01, 927.194, 925.379, 924.472, 922.657, 921.75, 922.657, 921.75, 917.215, 916.308, 916.308, 918.122, 916.142284966862, 913.588, 910.869, 908.151, 906.339, 904.528, 902.716, 901.811, 900.0, 898.19, 897.285, 898.19, 900.0, 897.285, 892.761, 887.8806531828723, 889.143, 882.0051378026303, 885.526, 886.43, 887.334, 881.911, 881.911, 888.239, 891.856, 888.239, 13768.597067070934, 877.393, 876.49, 877.393, 874.684, 868.365, 865.658, 864.755, 863.853, 862.952, 862.952, 862.049, 857.541, 853.935, 853.935, 855.738, 853.034, 850.331, 847.629, 846.728, 847.629, 846.728, 844.027, 842.0954298758529, 844.027, 844.027, 841.326, 837.726, 834.128, 831.43, 829.632, 827.834, 826.036, 827.7546745450204, 826.036, 824.239, 823.34, 821.543, 819.747, 817.951, 817.053, 817.053, 815.257, 809.872, 807.18, 806.283, 806.283, 804.489, 801.799, 804.2676464458618, 798.213, 797.317, 796.421, 795.525, 794.628, 792.836, 790.149, 788.358, 784.778, 782.987, 782.093, 780.303, 779.408, 778.513, 775.83, 773.148, 772.253, 769.571, 768.678, 767.784, 765.997, 764.21, 762.424, 761.53, 758.852, 757.066, 756.173, 756.173, 754.388, 750.7273737392209, 749.035, 747.251, 747.251, 744.576, 744.576, 744.576, 741.01, 737.446, 736.554, 733.882, 729.341538832588, 732.101, 729.429, 726.759, 725.869, 724.089, 723.199, 722.6786430882114, 719.641, 716.973, 715.195, 714.305, 714.305, 708.2537018116377, 709.861, 707.196, 707.196, 706.308, 703.643, 701.867, 699.204, 696.541, 695.654, 694.766, 693.879, 691.218, 690.331, 688.557, 685.897, 683.238, 681.465, 675.3002754328149, 681.057942859845, 679.694, 676.15, 675.265, 673.493, 669.3810877368547, 673.493, 669.952, 667.297, 666.412, 663.758, 664.643, 661.104, 657.568, 655.799, 653.148, 651.381, 651.381, 648.731, 646.08, 646.08, 643.432, 642.549, 643.432, 639.901, 636.371, 635.489, 633.725, 631.961, 629.316, 628.434, 627.553, 626.671, 624.908, 623.146, 621.384, 618.741, 9827.903470022795, 618.741, 617.861, 614.338, 613.3675167596954, 609.058, 605.3016508134725, 603.779, 603.779, 601.141, 599.383, 598.504, 598.504, 598.504, 596.746, 594.11, 591.475, 587.961, 587.083, 584.449, 582.694, 580.939, 579.184, 579.184, 578.307, 576.552, 573.921, 572.167, 570.414, 568.661, 566.032, 564.279, 562.527, 560.776, 559.024, 558.148, 556.398, 554.647, 553.771, 551.146, 549.396, 550.272, 552.022, 554.647, 555.522, 555.522, 553.771, 547.647, 543.275, 538.904, 535.409, 532.788, 526.2517220238752, 530.169, 529.295, 527.549, 6063.75532954271, 525.803, 524.058, 523.185, 522.312, 518.823, 515.335, 512.719, 513.6033716690121, 509.1139283458334, 506.619, 504.877, 504.006, 504.877, 505.748, 504.877, 499.652, 495.3, 496.17, 494.429, 490.079, 489.21, 489.21, 490.079, 491.819, 493.559, 492.689, 483.992, 8793.856707882287, 476.172, 477.909, 477.041, 472.698, 469.226, 467.489, 467.489, 467.489, 4890.4377495357785, 463.151, 459.681, 457.08, 456.213, 453.612, 451.879, 450.146, 448.413, 446.681, 444.948, 446.70189072601534, 444.083, 443.217, 441.485, 438.889, 437.158, 435.427, 431.966, 430.237, 428.508, 429.14393150469886, 425.914, 424.186, 425.914, 423.321, 419.865, 417.274, 418.31848704602504, 414.683, 412.093, 410.366, 409.504, 408.641, 407.778, 406.052, 403.464, 400.877, 399.152, 398.29, 396.566, 394.842, 396.566, 393.98, 389.673, 388.811, 388.811, 388.811, 387.089, 382.784, 379.341, 381.923, 382.784, 376.76, 373.319, 370.74, 368.161, 366.442, 364.723, 363.004, 362.145, 360.427, 358.3650241975691, 358.709, 357.851, 356.133, 355.275, 3574.6514085367066, 351.841, 349.267, 345.835, 343.263, 341.548, 340.691, 339.834, 339.7080488276205, 3403.9375872614237, 330.88244977526233, 332.121, 331.265, 330.408, 329.552, 327.839, 326.126, 4366.973358007875, 321.847, 319.28, 316.714, 315.003, 313.293, 312.438, 311.583, 310.728, 309.874, 307.309, 303.892, 302.184, 300.475, 299.622, 297.914, 296.206, 295.353, 295.353, 294.499, 293.646, 292.793, 291.939, 290.233, 285.115, 281.705, 279.148, 277.444, 275.74, 274.036, 273.184, 271.481, 269.778, 268.075, 266.373, 264.67, 2724.8046241936463, 262.969, 262.118, 261.267, 262.118, 261.267, 259.566, 256.163, 251.913, 247.664, 245.115, 243.416, 242.567, 240.869, 239.171, 237.473, 236.624, 235.776, 234.927, 233.23, 232.382, 230.685, 228.989, 225.597, 223.901, 223.054, 221.359, 221.359, 219.664, 217.122, 211.15038964018783, 211.194, 209.5, 208.654, 208.654, 207.808, 206.115, 205.269, 203.577, 201.885, 199.348, 196.811, 195.12, 192.584, 190.049, 188.359, 186.67, 184.981, 184.981, 184.136, 182.448, 179.915, 177.383, 174.852, 174.008, 173.164, 173.164, 174.852, 174.852, 174.008, 170.634, 165.575, 163.89, 161.362, 159.677, 158.834, 159.677, 160.519, 157.992, 155.465, 152.097, 148.73, 14.944, 159.677, 144.523, 141.159, 139.477, 137.795, 136.955, 135.274, 133.593, 133.593, 131.913, 131.072, 130.232, 130.232, 129.392, 131.913, 128.552, 123.514, 123.514, 127.713, 126.033, 118.478, 115.122, 115.961, 115.122, 112.606, 108.413, 106.737, 108.413, 107.82801064526437, 105.061, 1486.8783264373048, 97.521, 95.846, 1398.5530100067203, 99.196, 93.335, 88.313, 86.64, 85.804, 84.131, 84.131, 84.967, 80.786, 79.114, 77.442, 74.935, 74.099, 74.099, 74.6998698779066, 70.758, 67.417, 64.913, 64.078, 62.408, 62.408, 62.408, 60.739, 58.236, 56.568, 58.236, 57.402, 54.9, 51.564, 49.897, 48.23, 46.563, 45.729, 44.063, 41.564, 40.731, 39.898, 612.7424724907827, 37.4, 38.233, 35.735, 36.568, 34.07, 31.574, 29.91, 30.741, 29.91, 29.91, 33.238, 34.07, 29.077, 26.582, 22.424, 19.93, 17.437, 18.545624702903076, 13.283, 11.621, 9.13, 7.469, 6.639, 8.3, 9.96, 13.513383938842468, 8.3, 8.3, 8.3, 8.3, 7.469, 7.943922388593268, 5.809, 4.149, 4.149, 3.319, 3.319, 3.319, 2.49, 1.659, -0.829, -0.829, 0.0, 0.0, 0.0, 0.83, 1.659, 2.49, 2.49, 0.0, -1.659, -2.488};
int fake_idx = 0;
#endif

#define stepPin A3
#define dirPin A2
#define motorInterfaceType 1 // TODO: Get rid of this
#define buzzerPin A0

// ALTIMETER VARIABLES
#define SEALEVELPRESSURE_HPA (1013.25)
float altimeter_latest;
int ALT_TRSH_CHECK = 21;       // Use -10 for parking lot test and maybe change it on location
int LOW_ALT_TRSH_CHECK = 19;   //=300 or 350 for actual launch
int UPPER_ALT_TRSH_CHECK = 20; // 500 for actual launch
bool forwardStatus = false;
bool backwardStatus = false;

OTA_Update otaUpdater("soar-deploy", "TP-Link_BCBD", "10673881");

SOAR_Lora lora("5", "5", "905000000"); // LoRa

// STEPPER MOTOR DELAYS
static const int microDelay = 900;
static const int betweenDelay = 250;

// DC motor
DCMotor motor(A2, 50, 50);

SOAR_BAROMETER barometer;

class KalmanFilter {
public:
    KalmanFilter(float process_noise, float measurement_noise, float estimated_error, float initial_value) {
        Q = process_noise;
        R = measurement_noise;
        P = estimated_error;
        value = initial_value;
        
        /* Arbitrary threshold for outlier detection, adjust based on your data */
        outlier_threshold = 20.0; 
    }
    
    float update(float measurement) {
        // Prediction update
        /* No actual prediction step because we assume a simple model */
        
        // Measurement update
        K = P / (P + R);
        value = value + K * (measurement - value);
        P = (1 - K) * P + Q;
        
        return value;
    }
    
    bool checkOutlier(float measurement) {
        return fabs(measurement - value) > outlier_threshold;
    }
    
private:
    float Q; // Process noise
    float R; // Measurement noise
    float P; // Estimation error
    float K; // Kalman gain
    float value; // Filtered measurement
    float Q_prev;
    float P_prev;
    float value_prev;

    float outlier_threshold; // Threshold for detecting outliers
};



class AltitudeTrigger
{
private:
  float _max_height = 0;
  float _h0;
  float _h1;
  float _h2;
  float _prev_altitude = -500;
  std::queue<float> altitudeQueue;
  float _average = 0;
  float _sum = 0;
  float _max_distance = 0;
  const float _MACH = 175; //0.5 * speed of sound (maximum velocity per second - 171.5)
  uint32_t _last_checkpoint = 0; //for time control
  KalmanFilter _kf;

public:
  int state=0;

  AltitudeTrigger(float H0, float H1, float H2) : _kf(1.0, 1.0, 1.0, 10.0)
  {
    _h0 = H0;
    _h1 = H1;
    _h2 = H2;
    
  }
  float GetMaxAltitude(){
    return _max_height;
  }
  bool CheckAltitude(float curr_altitude)
  {
    _kf.update(curr_altitude);
    if(_kf.checkOutlier(curr_altitude)) return false;
    switch (state)
    {
      case 0://if next value is greater than 100
        if (curr_altitude > _h0){
          state = 1;
         }
        break;
      case 1:
        if ((curr_altitude > _max_height) && (curr_altitude - _prev_altitude > 0))
        {
            _max_height = curr_altitude;
        }
        if ((_max_height-curr_altitude>10) && (curr_altitude - _prev_altitude < 0))
        {
          state = 2;
        }
        break;
      case 2:
        if (curr_altitude < _h2 && curr_altitude > _h1)
        {
          state = 3;
        }
        break;
      case 3:
        if (curr_altitude < _h1)
        {
          state = 4;
        }
        break;
      case 4:
        //retracting
        break;
    }
    _prev_altitude=curr_altitude;
    return true;
  }
  
  // bool isContinuous(float curr_altitude){
  //   curr_altitude = abs(curr_altitude);
  //   if(altitudeQueue.size() < 10){
  //     altitudeQueue.push(curr_altitude);
  //     _sum += curr_altitude;
  //     _last_checkpoint = millis();
  //     return true;
  //   }
  //   if(altitudeQueue.size() == 10){//if the altitude is greater than mach, multiply seconds*mach. If the difference is greater than the possible distance then function is not continuous
  //     _average=_sum / 10;
  //     _sum -= altitudeQueue.front();
  //     altitudeQueue.pop();
  //     _max_distance = (millis() - _last_checkpoint) * _MACH;
  //     switch(state){
  //       case 1:
  //         if(curr_altitude >= _average && curr_altitude <= _max_distance){
  //           altitudeQueue.push(curr_altitude);
  //           _sum += curr_altitude;
  //           _last_checkpoint = millis();
  //           return true; //altitude is in the expected range
  //       case 2:
  //     }
      
  //     }else if(state == 2 && (curr_altitude <= _average && curr_altitude <= _max_distance)){
  //       altitudeQueue.push(curr_altitude);
  //       _sum += curr_altitude;
  //     }
  //   }
  //   //get checkpoint
  //   return false;
  // }


};

AltitudeTrigger altTrigger(914,107,183);

float previous_altitude = -300;
float max_candidate = -300;
int alt_trigger_count = 0;
int low_alt_trigger_count = 0;
float immediate_previous = -6000;
int altitudeTrigger(float current_altitude)
{
#if DEBUG_ALT
  Serial.print("Dif:");
  Serial.println(current_altitude - previous_altitude);
  Serial.print("Prev:");
  Serial.println(previous_altitude);
#endif
  int res = 0;
  // Check if the altitude is decreasing and above ALT_TRSH_CHECK
  if ((current_altitude > ALT_TRSH_CHECK) && (current_altitude - previous_altitude < -2))
  {
    res = 0;
  }
  if (current_altitude > previous_altitude)
  {
    if (current_altitude - immediate_previous < 800 || immediate_previous == -60000)
    { // Default value for errors
      previous_altitude = current_altitude;
    }
  }

  if ((current_altitude - previous_altitude) < -2 && current_altitude <= UPPER_ALT_TRSH_CHECK && current_altitude > LOW_ALT_TRSH_CHECK)
  {
    res = 1; // move forward status
  }
  if ((current_altitude - previous_altitude) < -2 && current_altitude <= LOW_ALT_TRSH_CHECK)
  {
    res = 2;
  }

  if (current_altitude - immediate_previous > 800)
  { // If altitude shows sudden changes it must be a glitch
    res = 0;
  }
  // Update previous_altitude for the next function call
  immediate_previous = current_altitude;
#if DEBUG_ALT
  Serial.print("Returned value: ");
  Serial.println(res);
#endif
  return res;
}

BuzzerNotify buzzerNotify = BuzzerNotify(buzzerPin);

MyVL53L0X distanceSensor;

class Deployment
{
private:
  int _state = 0; // _standby = 0; _forward = 1; _wait = 2; _retract = 3; _complete = 4;  _paused = 5;
  const char *message[6] = {"STANDBY", "FORWARD", "WAITING", "RETRACTING", "COMPLETED", "PAUSED"};
  bool sensor_trigger = false;
  uint32_t _forward_checkpoint = 0;
  uint32_t _wait_checkpoint = 0;
  uint32_t _retract_checkpoint = 0;
  uint32_t _last_checkpoint = 0;
  uint32_t _forward_duration = 3500; // 2.5 seconds 3500
  uint32_t _retract_duration = 8000; // Around half of move duration
  uint32_t _wait_duration = 20000;   // 10 seconds
  int last_state = 0;
  bool _warn = false;
  int fwd_sensor_checks = 0;
  int retract_sensor_checks = 0;

public:
  Deployment(){};
  void Deploy()
  {
    if (_state == 0)
    {
      _state = 1;
    }
    if (_state == 5)
    {
      _state = last_state;
      switch (last_state)
      {
      case 1:
        _forward_checkpoint = millis();
        break;
      case 2:
        _wait_checkpoint = millis();
        break;
      case 3:
        _retract_checkpoint = millis();
        break;
      }
    }
  };

  void Stop()
  {
    last_state = _state;
    _state = 5;
    motor.DC_STOP();
  };

  void ProcedureCheck()
  {
    uint16_t distance;
    int speed_fwd;
    switch (_state)
    {
    case 0: // standby
      break;
    case 1: // forward
      if (_forward_checkpoint == 0)
      {
        GetStatus();
        for (int i = 0; i < 5; i++)
        {
          buzzerNotify.Trigger();
          delay(100);
        }
        _forward_checkpoint = millis();
      }
      speed_fwd = 100;
      // Sensor and time logic comes first
      distance = distanceSensor.readDistance();
      Serial.println(distance);
      sensor_trigger = distance > 350 && distance != 65535;
      if (sensor_trigger)
      {
        for (int i = 0; i < 3; i++)
        {
          distance += distanceSensor.readDistance();
        }
        sensor_trigger = (distance / 4) > 350;
      }
      else if (distance > 280)
      {
        speed_fwd = 50;
      }

      if (sensor_trigger || (millis() - _forward_checkpoint) > _forward_duration)
      {
        if (sensor_trigger)
        {
          Serial.println("Stop triggered by sensor");
        }
        Serial.println("Stopped.");
        speed_fwd = 0;
        _state = 2;
      }
      motor.DC_MOVE(speed_fwd);
      break;
    case 2: // wait
      if (_wait_checkpoint == 0)
      {
        GetStatus();
        _wait_checkpoint = millis();
      }
      motor.DC_STOP();
      if ((millis() - _wait_checkpoint) > _wait_duration)
      {
        _state = 3;
      }
      break;
    case 3: // retract
      if (_retract_checkpoint == 0)
      {
        GetStatus();
        for (int i = 0; i < 5; i++)
        {
          buzzerNotify.Trigger();
          delay(100);
        }
        _retract_checkpoint = millis();
      }
      // Sensor and time logic comes first
      distance = distanceSensor.readDistance();
      Serial.println(distance);
      sensor_trigger = distance < 90 && distance != 65535;
      if (sensor_trigger)
      {
        for (int i = 0; i < 3; i++)
        {
          distance += distanceSensor.readDistance();
        }
        sensor_trigger = (distance / 3) < 90;
      }
      else
        retract_sensor_checks = 0;
      if (sensor_trigger || (millis() - _retract_checkpoint) > _retract_duration)
      {
        if (sensor_trigger)
        {
          Serial.println("Stop triggered by sensor");
        }
        Serial.println("Stopped.");
        motor.DC_STOP();
        _state = 4;
      }
      else
      {
        // Move back logic comes second
        motor.DC_MOVE(-50);
      }
      break;
    case 4: // complete
      motor.DC_STOP();
      break;
    case 5: // paused
      motor.DC_STOP();
      break;
    }
  };
  void Reset()
  {
    _state = 0;
    _forward_checkpoint = 0;
    _wait_checkpoint = 0;
    _retract_checkpoint = 0;
    _warn = false;
  };
  void Retract()
  {
    if (_state == 0 || _state == 2)
      _state = 3;
  }
  String GetStatus()
  {
    return message[_state];
  };
};
Deployment deployment;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      String value_str = "";
      for (int i = 0; i < value.length(); i++)
        value_str += value[i];
      Serial.print("Received Value: ");
      Serial.println(value_str);
      if (value_str == "DEPLOY")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Deploy procedure\n");
        deployment.Deploy();
      }
      else if (value_str == "STOP")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Stoping deployment\n");
        deployment.Stop();
      }
      else if (value_str == "RESET")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Resetting deployment state\n");
        deployment.Reset();
      }
      else if (value_str == "RETRACT")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Rtracting deployment\n");
        deployment.Retract();
      }
      else if (value_str == "BEEP")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Rtracting deployment\n");
        buzzerNotify.Trigger();
      }
      else if (value_str == "STATUS")
      {
        String sts = deployment.GetStatus();
        String stat = "DEPLOY-STATUS:" + sts;
        std::string stat_std = stat.c_str(); // Convert Arduino String to std::string
        pCharacteristic->setValue(stat_std); // Set the value using std::string
      }
    }
  }
};

void setup()
{
  // Set the maximum speed and acceleration

#if TEST_MOTOR
  deployment.TriggerProcedure();
#if TEST_MOTOR_BACK
  deployment.Retract();
#endif
#endif
  Serial.begin(115200);
  Wire.begin();
  // LORA SETUP
  lora.begin();

  buzzerNotify.Setup();
  // Stepper setup------------------
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  // BMP setup---------------------
  buzzerNotify.Trigger();

  // Bluetooth setup---------------
  BLEDevice::init("SOAR_DeployModule");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
  pCharacteristic = pService->createCharacteristic(
      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  buzzerNotify.Trigger();
  barometer.Initialize();
  buzzerNotify.Trigger();
  motor.DC_SETUP();
  buzzerNotify.Trigger();

  distanceSensor.begin();
  otaUpdater.Setup();

  // Distance sensor setup
  delay(500);
  lora.sendCommand("AWAKE");
}

void loop()
{
  // Disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // Give the Bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // Restart advertising
    Serial.println("Advertising started");
    oldDeviceConnected = deviceConnected;
  }

  // // Connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  // Automated Altitude Trigger Check
  float altitude;
  static bool valid_value=true;
#if FAKE_ALT_DATA
  static uint32_t last_read=0;
  altitude = fake_data[fake_idx];
  if(fake_idx<999 && (millis()-last_read > 5 ||  !valid_value)){
    fake_idx++;
    last_read = millis();
  }
    
#else
  altitude = barometer.get_last_altitude_reading();
  altimeter_latest = altitude;
#endif
  valid_value = altTrigger.CheckAltitude(altitude);
  if(!valid_value){
    Serial.println("OUTLIER: "+String(altitude));
  }
  int descending = altTrigger.state;
  
#if DEBUG_ALT
  Serial.println("Altitude: "+String(altitude)+" | Max: "+String(altTrigger.GetMaxAltitude()) + "| State: "+String(descending));
#endif
  if (descending == 3 && forwardStatus == false)
  {
    Serial.println("Triggering deployment");
    Serial.println("Deployment activated by res val");
    forwardStatus = true;
    deployment.Deploy();
  }
  else if (descending == 4 && backwardStatus == false)
  {
      Serial.println("Low altitude detected");
      if (deployment.GetStatus() == "FORWARD")
      { // In case we haven't finished extended by the time we reach the lower altitude
        deployment.Stop();
        deployment.Reset();
      }
      Serial.println("Retracting activated by res val");
      backwardStatus = true;
      deployment.Retract();
  }

  // Deployment Procedure Constant Check
  deployment.ProcedureCheck();

  if (lora.available())
  {
    String incomingString = "";
    Serial.print("Request Received: ");
    String data_str = lora.read();
    if (data_str == "PING")
    {
      lora.queueCommand("PONG");
    }
    else if (data_str == "DEPLOY")
    {
      Serial.println("Deployment Triggered");
      deployment.Deploy();
      lora.queueCommand("DEPLOY:TRIGGERING");
    }
    else if (data_str == "STOP")
    {
      deployment.Stop();
      lora.queueCommand("DEPLOY:STOPING");
    }
    else if (data_str == "RESET")
    {
      deployment.Reset();
      lora.queueCommand("DEPLOY:RESETING");
    }
    else if (data_str == "STATUS")
    {
      String stat = "DEPLOY-STATUS:" + deployment.GetStatus();
      lora.queueCommand(stat);
    }
    else if (data_str == "RETRACT")
    {
      deployment.Retract();
      lora.queueCommand("DEPLOY:RETRACTING");
    }
    else if (data_str == "ALTITUDE")
    {
      char altimeter_latest_str[9];
      dtostrf(altimeter_latest, 4, 2, altimeter_latest_str);
      char altitude_str[100] = "ALTITUDE:";
      strcat(altitude_str, altimeter_latest_str);
      lora.queueCommand(altitude_str);
    }
    else if (data_str == "DISTANCE")
    {
      char distance_data[5];
      sprintf(distance_data, "%u", distanceSensor.readDistance());
      char distance_str[100] = "DISTANCE:";
      strcat(distance_str, distance_data);
      lora.queueCommand(distance_str);
    }
    else if (data_str.indexOf("THRESHOLD") >= 0)
    {
      try
      {
        for (int i = 0; i < data_str.length(); i++)
        {
          if (data_str[i] == ':')
          {
            String curstr = data_str.substring(i + 1);
            int new_trsh = curstr.toInt();
            ALT_TRSH_CHECK = new_trsh;
            break;
          }
        }
#if DEBUG_TRSHSET
        Serial.print("New Trsh: ");
        Serial.println(ALT_TRSH_CHECK);
#endif
        lora.queueCommand("THRESHOLD:SET");
      }
      catch (String error)
      {
        lora.queueCommand("THRESHOLD:ERROR");
      }
    }
    else if (data_str == "JOG:FWD")
    {
      lora.queueCommand("JOG:FWD+RCV");
      motor.DC_MOVE(50);
      motor.DC_STOP();
    }
    else if (data_str == "JOG:REV")
    {
      lora.queueCommand("JOG:REV+RCV");
      motor.DC_MOVE(-50);
      motor.DC_STOP();
    }
    else
    {
      lora.queueCommand(data_str + "+INVALID");
    }
  }
  // Vital Sign Indicator
  lora.handleQueue();
  buzzerNotify.Check();
  otaUpdater.Handle();
}
