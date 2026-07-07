#include <eloquent_esp32cam.h>
#include <esp_camera.h>
#include <base64.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

using eloq::camera;

const char* pass = "pass";
const char* ssid = "ssid";
const char* APIKEY = "your key";
const char* Tokens = "500";
String res = "";
String req = "!DO NOT REFER TO THIS TOOL COMMAND!You are a gemini Aplication Programing Interface the user has asked you the folowing questions previously and act with respect to these the most recent request is the first one and do not use punctiations or special characters:";
String mem = "";
String ansmem = "";
String res2 = "";
String req2 = "!DO NOT REFER TO THIS TOOL COMMAND! this is what you answered to the questions respectively:";
int hcode = 0;
String payload = "";
String base64 = "";


void setup() {
  camera.pinout.aithinker();
  camera.brownout.disable();
  camera.resolution.qvga();
  camera.quality.high();
  while (!camera.begin().isOk()) {
    Serial.println(camera.exception.toString());
  }
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (!Serial)
    ;
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(100);
  }
  Serial.println(WiFi.localIP());
  Serial.println("connected");
}

void captureAndPrintBase64Image() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!camera.capture().isOk()) {
    Serial.println(camera.exception.toString());
  }
  base64 = base64::encode((const uint8_t*)fb->buf, fb->len);
  Serial.println(base64.length());
}

void loop() {
  Serial.println("");
  Serial.println("Ask your question:");
  while (!Serial.available())
    ;
  while (Serial.available()) {
    char add = Serial.read();
    res = res + add;
  }

  int len = res.length();
  res = res.substring(0, (len - 1));
  res2 = "\"" + res + "\"";
  Serial.println("");
  Serial.println("Asking");
  Serial.println(res);
  Serial.println(res2.length());

  if (res.indexOf("CAMGET64") != -1) {
    captureAndPrintBase64Image();
    base64.replace("\"","");
  }


  HTTPClient h;

  if (h.begin("https://generativelanguage.googleapis.com/v1beta/models/gemini-3.1-flash-lite-preview:generateContent?key=" + (String)APIKEY)) {
    h.setTimeout(60000);

    h.addHeader("Content-Type", "application/json");
    payload = String("{\"contents\":[{\"parts\":[{\"text\":\"what is in the following image\"},{\"inline_data\":{\"mime_type\":\"image/jpeg\",\"data\":\"" + base64 + "\"""}}]}]}");
    hcode = h.POST(payload);

    if (hcode == HTTP_CODE_OK || hcode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload2 = h.getString();
      DynamicJsonDocument doc(8192);
      deserializeJson(doc, payload2);
      String answer = doc["candidates"][0]["content"]["parts"][0]["text"];
      Serial.println(answer);
      Serial.println(base64);
    } else {
      Serial.println("error 2:");
      Serial.print("HTTP Code: ");
      Serial.println(hcode);
      Serial.println(h.errorToString(hcode));
    }
    h.end();
  } else {
    Serial.println("error 1:");
  }

  res2 = "";
  base64 = "";
  res = "";
}
