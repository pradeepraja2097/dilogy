/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 3rd Aug 2019 with the current ESP8266 Arduino core release:
 * SDK:2.2.1(cfd48f3)/Core:2.5.2-56-g403001e3=20502056/lwIP:STABLE-2_1_2_RELEASE/glue:1.1-7-g82abda3/BearSSL:6b9587f
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 * 
 * Sketch Modified by Stephen Borsay for www.udemy.com/course/exploring-aws-iot/
 * https://github.com/sborsay
 * Add in EOF certificate delimiter
 * Add in Char buffer utilizing sprintf to dispatch JSON data to AWS IoT Core
 * First 9 chars of certs obfusicated, use your own, but you can share root CA / x.509 until revoked
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "libb64/cdecode.h"
}

const char* ssid = "pradeepraja";
const char* password = "Yamuna@03";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint.  Endpoints ARE regional
const char* awsEndpoint = "a322whkmb0uwd7-ats.iot.us-east-2.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key, comment out the BEGIN and END 
// lines, add a quote character at the start of each line and a quote 
// and backslash at the end of each line:

// xxxxxxxxxx-certificate.pem.crt
const String certificatePemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDWjCCAkKgAwIBAgIVAIneeP3HpDiYumHskaFO9o7Kx9OXMA0GCSqGSIb3DQEB"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDA0MTcwNjM3"\
"MDFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC+Cw8Ywy2q8EzOAT+s"\
"by7QaO/A/Pn74KBt07qGP7wV3oEr7OL94/b3nkeyUJjI1nlB3f8jwylNkJ/Z5MAM"\
"W0DSZfgB87p+yfz6pJx82T5jw919oGGHYSAF/W10U4zORqXx7QHHpH70E9/gL+0A"\
"v62JJRXkCXj9JjWVYed2i2lJQ+AaFHSV3bYCtIXhaxGEEzOJI8IYSwrm8P/qCObJ"\
"bdyljH+jWGEoeIAkiv5RfpfOpoGF2/dJ+bf5dzTyZrFTx1eH+pZ8WPW6lDaDIJAB"\
"1x24EzxQPw3AHD9fHEaBFXyE1H/vM5giUaYYdS0WPMdUT57/qO8m1Zv4mMcgC3on"\
"p0xtAgMBAAGjYDBeMB8GA1UdIwQYMBaAFCNf20O0hG2x2c5EtHrM+OYiwuQ5MB0G"\
"A1UdDgQWBBRsuXMMyOl8BEMBhLvCPaEaZ2QlozAMBgNVHRMBAf8EAjAAMA4GA1Ud"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAZxQClPkTiLrgXeqWZerRdFqE"\
"DmoPdYYz/vJnuHtNz11Gv7wRPaH7RWmKqNwni5Kr3RDPuTe9s7g7s3cdbwCmY/BN"\
"DQms8fTIuV1jfxxlCOMpYZ4/Nz80D91rAAl2O6nKWV/KaxuV0Mv56+lw7TBv60Fc"\
"8KKuT3BXHoBZ7buS/CxN/7xqv4PuFlcd4LNg/Nh6qtJrsMHVqcyiyyVWDLvSXFoa"\
"wR4r8RkYd+w9+7ccV9Q0TkBEHt9uUsuQF0sYHWY4kZluakTR44atdD9aVakdwv6N"\
"QVP3MOnD4qlycB4VIoCJVs962g0pw6a1gWH115Dk8d7XE+lPgvjLQYllC/0B9Q==";
 
//-----END CERTIFICATE-----

// xxxxxxxxxx-private.pem.key
const String privatePemKey = \
//-----BEGIN RSA PRIVATE KEY-----
"MIIEowIBAAKCAQEAvgsPGMMtqvBMzgE/rG8u0GjvwPz5++CgbdO6hj+8Fd6BK+zi"\
"/eP2955HslCYyNZ5Qd3/I8MpTZCf2eTADFtA0mX4AfO6fsn8+qScfNk+Y8PdfaBh"\
"h2EgBf1tdFOMzkal8e0Bx6R+9BPf4C/tAL+tiSUV5Al4/SY1lWHndotpSUPgGhR0"\
"ld22ArSF4WsRhBMziSPCGEsK5vD/6gjmyW3cpYx/o1hhKHiAJIr+UX6XzqaBhdv3"\
"Sfm3+Xc08maxU8dXh/qWfFj1upQ2gyCQAdcduBM8UD8NwBw/XxxGgRV8hNR/7zOY"\
"IlGmGHUtFjzHVE+e/6jvJtWb+JjHIAt6J6dMbQIDAQABAoIBAGZpZUBN5e1JQ8p8"\
"B8kvvUWYVyNj8H8YxLuKruirrfsHBgsaxBRX5f34sOBZ6IvKcFvbeyPVhwuClXIB"\
"gLDn+B2pz6WVp3mt9gNH9ACfvaxbn5+Ubxpab7xqnrJiV+3NSp39v8jLT5LOpwWU"\
"Lh8KLEVM0+goZqfdEGEo+mf6pslQudSSGqyBTdDGGptRnS5VWfbfHt0Howmi6Il1"\
"ryQbcMJiu+MqBraqkDa9kopwtFt+E0Rm7EbLQTQp2KlEnnETUyJeeBVdK0BSznEK"\
"FW9c8yHCDwVVlp1Ca6yEmqfWbCOyoPCNjoEUOz9uNsOaDwdWcOWr804CFawCsy60"\
"CFsMCY0CgYEA3w0xKbkgsvlS3BChRKrKi7ydVYYiPr9OXm1F3NbFG64LKslS9jz3"\
"pkUJnJzsviSNkK+6kEAIeYyxSwD3C1PH8i+ji/YFc0e39HkgA4/higMvHyKRoBtj"\
"VUxw5K78JShWPLfQZpqNIOkBhRZR0KaloDqGpaZXBndneQkLivU1MT8CgYEA2h2j"\
"0lW2AAckarhsjNYiULZUQ/aHDufPuwWI5KmU3U9rmRsDVpNDgbo8BlJdC6C6o0XJ"\
"fHQO+k5zoshOK0pC91PZRic3DeE8Iyk33AgrgNN25KaWZlaCYiPGM2eM/k3ATNzr"\
"weAB12FMGzX3BU98g/wQsAbjeQngnKIT5Yxta1MCgYAOhqFpAjFq9mm7Ayo9epHu"\
"P75PT4jzZSoJZDxkgWgcwVzg0HOirZ3DKkPR+zY/u8msXoVb14+1ur/hDpMIHdCr"\
"qQaRmtgIBeeo+CppkPkzL7nl4S5yqtXxPCUt/2dUVCgplB7yT6ZZOlNrA2UYnYGz"\
"yZvzNb39IdGPRFOIHSf1qQKBgGTNbfi3Q/WfV2U69m/gRtumaCtAtApfd9GnLS0A"\
"cpwVKFUJ9vD7/ibmOvdw+S92iMsMZaHuwT1NJIQGrpmzKF/FkJSravYvMevOENLP"\
"s8tF75ycoMWLFZblkaQQJ0kgBc+ACjT/odo4TUQGHLOHmLQmuk+f32EA/51kCm38"\
"CFWhAoGBANz0ipdEbf0TAyQFrFu9qXULgt7rdjtJrfWcbQTaNKpM//1IugPq4G2w"\
"4b4GTOq+P1//fH7LVzfoL0oMvSo5psZmsw+M8aJcMwhmy5tK7yHsOERIC7mKb8UD"\
"KmnZHVyeEYTeNccw4p18DqzTHzI3ieyrDYkoQktmxVW1RUhyTdXj";
//-----END RSA PRIVATE KEY-----

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
const String caPemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF"\
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6"\
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL"\
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv"\
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj"\
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM"\
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw"\
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6"\
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L"\
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm"\
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC"\
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA"\
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI"\
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs"\
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv"\
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU"\
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy"\
"rqXRfboQnoZsG4q5WTP468SQvvG5";
//-----END CERTIFICATE-----

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  uint8_t binaryCert[certificatePemCrt.length() * 3 / 4];
  int len = b64decode(certificatePemCrt, binaryCert);
  wiFiClient.setCertificate(binaryCert, len);
  
  uint8_t binaryPrivate[privatePemKey.length() * 3 / 4];
  len = b64decode(privatePemKey, binaryPrivate);
  wiFiClient.setPrivateKey(binaryPrivate, len);

  uint8_t binaryCA[caPemCrt.length() * 3 / 4];
  len = b64decode(caPemCrt, binaryCA);
  wiFiClient.setCACert(binaryCA, len);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  //Add a JSON package of fake data to deliver to AWS IoT
  //Uses snprintf but other viable options are: sprintf, strcpy, strncpy, or
  //Use the ArduinoJson library for Efficient JSON serialization
  char fakeData[100];
//choose your own number of variables and data types, but avoid data buffer size overload
  float var1 =  random(55,77); //fake number range, adjust as you like
  float var2 =  random(77,99);
  sprintf(fakeData,  "{\"uptime\":%lu,\"temp\":%f,\"humid\":%f}", millis() / 1000, var1, var2);


  

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;

   pubSubClient.publish("outTopic", fakeData);
   Serial.print("Published: "); Serial.println(fakeData);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

int b64decode(String b64Text, uint8_t* output) {
  base64_decodestate s;
  base64_init_decodestate(&s);
  int cnt = base64_decode_block(b64Text.c_str(), b64Text.length(), (char*)output, &s);
  return cnt;
}

void setCurrentTime() {
  configTime(3 * 3600, 0,"pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
