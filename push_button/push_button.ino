#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Настройки Wi-Fi
const char *ssid = "FollowMe";
const char *password = "12345678";

// Номер пина, к которому подключен светодиод
const int motorPin1 = D4;  // Пин для подключения к первому терминалу мотора
const int motorPin2 = D3;  // Пин для подключения ко второму терминалу мотора

ESP8266WebServer server(80);

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, HIGH);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Подключение к Wi-Fi...");
  }
  Serial.println("Подключено к Wi-Fi");

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/start", HTTP_GET, handleStart);
  server.on("/stop", HTTP_GET, handleStop);

  server.onNotFound([](){
    server.send(404, "text/plain", "Not Found");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>LED</h1>";
  html += "<button onmousedown=\"startMotor()\" onmouseup=\"stopMotor()\">Start</button><br>";
  html += "<script>var motorRunning = false;";
  html += "function startMotor() {";
  html += "if (!motorRunning) {";
  html += "  motorRunning = true;";
  html += "  sendCommand('/start');";
  html += "}}";
  html += "function stopMotor() {";
  html += "  motorRunning = false;";
  html += "  sendCommand('/stop');";
  html += "}";
  html += "function sendCommand(command) {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.open('GET', command, true);";
  html += "xhr.send();}</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleStart() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, HIGH);
  server.send(200, "text/plain", "OK");
}