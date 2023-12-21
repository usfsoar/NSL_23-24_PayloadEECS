#include <TFT_eSPI.h>
#include "esp_camera.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#include "camera_pins.h"

const int camera_width = 640;
const int camera_height = 480;

uint32_t imageCount = 1; // Use uint32_t for managing large numbers
bool camera_sign = false;  // Check camera status
bool sd_sign = false;      // Check SD status

TFT_eSPI tft = TFT_eSPI(); // TFT instance

void writeFile(fs::FS &fs, const char *path, uint8_t *data, size_t len) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.write(data, len) == len) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void setup() {
  Serial.begin(115200);
  // Camera pinout
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.frame_size = FRAMESIZE_SVGA; // Change this to the desired resolution
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 50;
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera ready");
  camera_sign = true; // Camera initialization check passes

  // Initialize SD card
  if (!SD.begin(21)) {
    Serial.println("Card Mount Failed");
    return;
  }
  sd_sign = true;
  uint8_t cardType = SD.cardType();

  // Find the next available imageCount
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }
    String fileName = entry.name();
    if (fileName.endsWith(".jpg")) {
      uint32_t number = fileName.substring(0, fileName.indexOf('.')).toInt();
      if (number >= imageCount) {
        imageCount = number + 1;
      }
    }
    entry.close();
  }
  Serial.println("Starting program");
}

uint32_t last_pic = 0;
void loop() {
  
  // Camera & SD available, start taking pictures
  if(camera_sign && sd_sign && millis()-last_pic > 500){
    // Take a photo
    camera_fb_t *fb = esp_camera_fb_get();
    if(!fb){
      fb = esp_camera_fb_get();
    }
    if (!fb) {
      Serial.println("Failed to get camera frame buffer");
      return;
    }
    char filename[32];
    sprintf(filename, "/%d.jpg", imageCount);
    // Save photo to file
    writeFile(SD, filename, fb->buf, fb->len);
    Serial.printf("Saved picture: %s\n", filename);
    imageCount++;

    // Release image buffer
    esp_camera_fb_return(fb);
    last_pic = millis();
  }
}
