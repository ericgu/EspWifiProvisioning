#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <ESP8266HTTPClient.h>

void hitStatusPage(String host)
{
  // We now create a URI for the request
  String url = "/status";

  host = "192.168.4.1";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host.c_str(), httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String request = String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n";
  Serial.println(request);
               
  // This will send the request to the server
  client.print(request);
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Response:");
  while (client.connected())
  {
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
}

void provisionServerToClient(String host)
{
  Serial.println("Telling server to provision");
  HTTPClient client;

  client.begin("http://192.168.4.1/provision");
  client.addHeader("Content-Type", "text/plain");

  char* payload = "ssid=ASUS&password=Gunnerson";
  int httpCode = client.POST((uint8_t *) payload, strlen(payload));
  Serial.println(httpCode);

#ifdef fred
  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Response:");
  while (client.connected())
  {
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
#endif
}

static int connectedCount = 0;

void pingEdp(String ssid)
{

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);  // workaround?
  WiFi.mode(WIFI_STA);
  delay(100);

  Serial.print("Connecting to: ");
  Serial.println(ssid.c_str());
  WiFi.begin(ssid.c_str(), ssid.c_str());

  int timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout > 0)
  {
    delay(500);
    Serial.print(WiFi.status());
    timeout--;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected");
    Serial.println(connectedCount++);

    //hitStatusPage(ssid);
    provisionServerToClient(ssid);
  }
  
  WiFi.disconnect();
}

void enumerateNetworks()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

    Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      String ssid = WiFi.SSID(i);
      if (ssid.startsWith("EDP_"))
      {
        pingEdp(ssid);
      }
      else {
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
        delay(10);
      }
    }
  }
  Serial.println("");

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(50);                        // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  // Wait a bit before scanning again
  delay(50);

}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();
}

void loop()
{
  enumerateNetworks();
  
  //Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);   
}
