  #if defined(__AVR__)
  #define OPERATING_VOLTAGE   5.0
  #else
  #define OPERATING_VOLTAGE   3.3
  #endif
  
  constexpr auto PIN_SENSOR = A1;
    
  void QRD() 
  {
    int valueSensor = analogRead(PIN_SENSOR);

    float voltageSensor = valueSensor * OPERATING_VOLTAGE / 1024.0;

    Serial.print("Value = "); 
    Serial.print(voltageSensor);
    Serial.println(" Volts");
  }