#include <ESP8266WiFi.h>
#include <Servo.h>
const int trigPin = D5;
const int echoPin = D6;
long duration;
int distance;
int buzzer = D7;
int led = D0;
int ledo = D8;
void send_event(const char *event);
float level;


const int TRIG_PIN = D1;
const int ECHO_PIN = D2;

const int SERVO_PIN = D3;

#define DISTANCE_THRESHOLD  50 // centimeters

Servo servo; // create servo object to control a servo

// The below are variables, which can be changed
float duration_us, distance_cm;
const char* ssid = "Gidi";
const char* password = "Gidi123";

const char *host = "maker.ifttt.com";
const char *privateKey = "l8TFQ7J7x1pmiypHIG8i-kKyuEx6sXnIy8lCEIsXuWj"; 

WiFiServer server(80);

void setup() {
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
pinMode(buzzer, OUTPUT); 
pinMode(led, OUTPUT);
pinMode(ledo, OUTPUT);
digitalWrite(led, LOW);
pinMode(TRIG_PIN, OUTPUT); // set ESP32 pin to output mode
pinMode(ECHO_PIN, INPUT);  // set ESP32 pin to input mode
servo.attach(SERVO_PIN);   // attaches the servo on pin 9 to the servo object
servo.write(0);
Serial.begin(9600);
Serial.print("Connecting to Wifi Network");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("Successfully connected to WiFi.");
Serial.println("IP address is : ");
Serial.println(WiFi.localIP());
server.begin();
Serial.println("Server started");


}

void loop() {
 // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  if (distance_cm < DISTANCE_THRESHOLD and level <= 70)
    servo.write(90); // rotate servo motor to 90 degree
  else
    servo.write(0);  // rotate servo motor to 0 degree

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  delay(500);
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = duration * 0.0340 / 2;
Serial.println("Distance");
Serial.println(distance);
level =((28 -distance)/28.0)*100;
Serial.println("level");
Serial.println(level);

delay(1000);
if ( level >= 70) {
 digitalWrite (buzzer, HIGH); 
 digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite (buzzer, LOW);
    digitalWrite(led, LOW);

}
if ( level >= 70) {
send_event("dustbin_event");
 

}
if ( level <= 70) {
 digitalWrite(ledo, HIGH);
    delay(1000);
    digitalWrite(ledo, LOW);
 

}
WiFiClient client = server.available();

if (client)
{
Serial.println("Web Client connected ");
String request = client.readStringUntil('\r');
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close"); // the connection will be closed after completion of the response
client.println("Refresh: 10"); // update the page after 10 sec
client.println();
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.println("<style>html { font-family: Cairo; display: block; margin: 0px auto; text-align: center;color: #333333; background-color: ##f3ffee;}");
client.println("body{margin-top: 50px;}");
client.println("h1 {margin: 50px auto 30px; font-size: 50px; text-align: center;}");
client.println(".side_adjust{display: inline-block;vertical-align: middle;position: relative;}");
client.println(".text1{font-weight: 180; padding-left: 5px; font-size: 50px; width: 170px; text-align: left; color: #3498db;}");
client.println(".data1{font-weight: 180; padding-left: 1px; font-size: 50px;color: #3498db;}");
client.println(".data{padding: 1px;}");
client.println("</style>");
client.println("</head>");
client.println("<body>");
client.println("<div id=\"webpage\">");
client.println("<h1>IoT Based Dustbin</h1>");
client.println("<div class=\"data\">");
client.println("<div class=\"side_adjust text1\">Status:</div>");
client.println("<div class=\"side_adjust data1\">");
client.print(level);
client.println("<div class=\"side_adjust text1\">% filled</div>");
client.println("</div>");
client.println("</div>");
client.println("</body>");
client.println("</html>");
//client.println("<h1>Level Indicator</h1>");

if ( level >= 70) {
send_event("dustbin_event");
 

}
 
}
}

void send_event(const char *event)
{
Serial.print("Connecting to ");
Serial.println(host);

// Use WiFiClient class to create TCP connections
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort)) {
Serial.println("Connection failed");
return;
}

// We now create a URI for the request
String url = "/trigger/";
url += event;
url += "/with/key/";
url += privateKey;

Serial.print("Requesting URL: ");
Serial.println(url);

// This will send the request to the server
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"Connection: close\r\n\r\n");
while(client.connected())
{
if(client.available())
{
String line = client.readStringUntil('\r');
Serial.print(line);
} else {
// No data yet, wait a bit
delay(50);
};
}

Serial.println();
Serial.println("closing connection");
client.stop();
}
