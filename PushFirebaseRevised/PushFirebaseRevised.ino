
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP32
 *
 * Copyright (c) 2023 mobizt
 *
 */

#include <Arduino.h>
#if defined(ESP32)
// #include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <FirebaseESP32.h>
#elif defined(ESP8266)
// #include <ESP8266WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
// #include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <FirebaseESP8266.h>
#endif

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#define ledindicator 2
// /* 1. Define the WiFi credentials */
// #define WIFI_SSID "wifi bayar"
// #define WIFI_PASSWORD "12345678"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyB3AhEYnuLIqsTAn3O3GaYn_9OaQtx6rdM"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://esp32-gilang-data-default-rtdb.asia-southeast1.firebasedatabase.app/"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "gilangfajrulfalah@gmail.com"
#define USER_PASSWORD "12345678"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
WiFiManager wm;
TaskHandle_t Task1;
TaskHandle_t Task2;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;
bool res;
bool connected = 0;

void setup() {

  Serial.begin(115200);
  pinMode(ledindicator, OUTPUT);
  digitalWrite(ledindicator, LOW);
  // Serial.println(xPortGetCoreID());

  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting to Wi-Fi");
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.print(".");
  //   delay(300);
  // }
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
  xTaskCreatePinnedToCore(
    Task2code, /* Task function. */
    "Task2",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task2,    /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
}

void Task1code(void* pvParameters) {
  res = wm.autoConnect("CardioSaver", "12345678");  // password protected ap

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  // Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // Or use legacy authenticate method
  // config.database_url = DATABASE_URL;
  // config.signer.tokens.legacy_token = "<database secret>";

  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  // You can use TCP KeepAlive in FirebaseData object and tracking the server connection status, please read this for detail.
  // https://github.com/mobizt/Firebase-ESP32#about-firebasedata-object
  // fbdo.keepAlive(5, 5, 1);

  /** Timeout options.

  //WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
  config.timeout.wifiReconnect = 10 * 1000;

  //Socket connection and SSL handshake timeout in ms (1 sec - 1 min).
  config.timeout.socketConnection = 10 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  Note:
  The function that starting the new TCP session i.e. first time server connection or previous session was closed, the function won't exit until the
  time of config.timeout.socketConnection.

  You can also set the TCP data sending retry with
  config.tcp_data_sending_retry = 1;

  */
  for (;;) {

    // Firebase.ready() should be called repeatedly to handle authentication tasks.

    if (Firebase.ready()) {
      sendDataPrevMillis = millis();

      // Serial.printf("Set bool... %s\n", Firebase.setBool(fbdo, F("/test/bool"), count % 2 == 0) ? "ok" : fbdo.errorReason().c_str());

      // Serial.printf("Get bool... %s\n", Firebase.getBool(fbdo, FPSTR("/test/bool")) ? fbdo.to<bool>() ? "true" : "false" : fbdo.errorReason().c_str());

      // bool bVal;
      // Serial.printf("Get bool ref... %s\n", Firebase.getBool(fbdo, F("/test/bool"), &bVal) ? bVal ? "true" : "false" : fbdo.errorReason().c_str());

      // Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/test/int"), count) ? "ok" : fbdo.errorReason().c_str());

      // Serial.printf("Get int... %s\n", Firebase.getInt(fbdo, F("/test/int")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());

      int iVal = 0;
      // Serial.printf("Get int ref... %s\n", Firebase.getInt(fbdo, F("/test/int"), &iVal) ? String(iVal).c_str() : fbdo.errorReason().c_str());

      // Serial.printf("Set float... %s\n", Firebase.setFloat(fbdo, F("/test/float"), count + 10.2) ? "ok" : fbdo.errorReason().c_str());

      // Serial.printf("Get float... %s\n", Firebase.getFloat(fbdo, F("/test/float")) ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());

      Serial.printf("Set double... %s\n", Firebase.pushDouble(fbdo, F("/test/double"), count + 35.517549723765) ? "ok" : fbdo.errorReason().c_str());

      Serial.printf("Get double... %s\n", Firebase.getDouble(fbdo, F("/test/double")) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());

      // Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("/test/string"), "Hello World!") ? "ok" : fbdo.errorReason().c_str());

      // Serial.printf("Get string... %s\n", Firebase.getString(fbdo, F("/test/string")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());

      // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
      // FirebaseJson json;

      // if (count == 0)
      // {
      //   json.set("value/round/" + String(count), F("cool!"));
      //   json.set(F("vaue/ts/.sv"), F("timestamp"));
      //   Serial.printf("Set json... %s\n", Firebase.set(fbdo, F("/test/json"), json) ? "ok" : fbdo.errorReason().c_str());
      // }
      // else
      // {
      //   json.add(String(count), "smart!");
      //   Serial.printf("Update node... %s\n", Firebase.updateNode(fbdo, F("/test/json/value/round"), json) ? "ok" : fbdo.errorReason().c_str());
      // }

      Serial.println();

      // For generic set/get functions.

      // For generic set, use Firebase.set(fbdo, <path>, <any variable or value>)

      // For generic get, use Firebase.get(fbdo, <path>).
      // And check its type with fbdo.dataType() or fbdo.dataTypeEnum() and
      // cast the value from it e.g. fbdo.to<int>(), fbdo.to<std::string>().

      // The function, fbdo.dataType() returns types String e.g. string, boolean,
      // int, float, double, json, array, blob, file and null.

      // The function, fbdo.dataTypeEnum() returns type enum (number) e.g. fb_esp_rtdb_data_type_null (1),
      // fb_esp_rtdb_data_type_integer, fb_esp_rtdb_data_type_float, fb_esp_rtdb_data_type_double,
      // fb_esp_rtdb_data_type_boolean, fb_esp_rtdb_data_type_string, fb_esp_rtdb_data_type_json,
      // fb_esp_rtdb_data_type_array, fb_esp_rtdb_data_type_blob, and fb_esp_rtdb_data_type_file (10)

      count++;
    }
    res = wm.autoConnect("CardioSaver", "12345678");  // password protected ap
    Serial.println(res);
    // if (res) {
    //   // connected = 0;
    //   // setup();
    // // } else {
    //   digitalWrite(ledindicator, HIGH);
    //   connected = 1;
    // }
    // // Serial.println(connected);
  }
}

void Task2code(void* pvParameters) {
  // for (;;) {
  //   if (!connected) {
  //     digitalWrite(ledindicator, !digitalRead(ledindicator));
  //     delay(1000);
  //   } else {
  //     connected = 0;
  //     delay(500);
  //   }
  // }
}

void loop() {
}

/// PLEASE AVOID THIS ////

// Please avoid the following inappropriate and inefficient use cases
/**
 *
 * 1. Call get repeatedly inside the loop without the appropriate timing for execution provided e.g. millis() or conditional checking,
 * where delay should be avoided.
 *
 * Everytime get was called, the request header need to be sent to server which its size depends on the authentication method used,
 * and costs your data usage.
 *
 * Please use stream function instead for this use case.
 *
 * 2. Using the single FirebaseData object to call different type functions as above example without the appropriate
 * timing for execution provided in the loop i.e., repeatedly switching call between get and set functions.
 *
 * In addition to costs the data usage, the delay will be involved as the session needs to be closed and opened too often
 * due to the HTTP method (GET, PUT, POST, PATCH and DELETE) was changed in the incoming request.
 *
 *
 * Please reduce the use of swithing calls by store the multiple values to the JSON object and store it once on the database.
 *
 * Or calling continuously "set" or "setAsync" functions without "get" called in between, and calling get continuously without set
 * called in between.
 *
 * If you needed to call arbitrary "get" and "set" based on condition or event, use another FirebaseData object to avoid the session
 * closing and reopening.
 *
 * 3. Use of delay or hidden delay or blocking operation to wait for hardware ready in the third party sensor libraries, together with stream functions e.g. Firebase.RTDB.readStream and fbdo.streamAvailable in the loop.
 *
 * Please use non-blocking mode of sensor libraries (if available) or use millis instead of delay in your code.
 *
 * 4. Blocking the token generation process.
 *
 * Let the authentication token generation to run without blocking, the following code MUST BE AVOIDED.
 *
 * while (!Firebase.ready()) <---- Don't do this in while loop
 * {
 *     delay(1000);
 * }
 *
 */