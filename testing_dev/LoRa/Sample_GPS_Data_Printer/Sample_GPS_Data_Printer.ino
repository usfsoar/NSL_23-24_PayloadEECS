void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Sample output from USF
  Serial.println("<LORA>+RCV=7,31,GPS: A,2803.9643,N,08225.0154,W\r</LORA>");
  
  delay(2000);
  Serial.println("<LORA>+RCV=7,31,Alt: -26.1");
  delay(2000);

  // Sample output from Varn
  // Note: This output was artificially created by modifying the above received USF output and 
  // placing Varn coordinates in.
  // Serial.println("<LORA>+RCV=7,31,GPS: A,2805.5864,N,08210.5439,W\r</LORA>")
}
