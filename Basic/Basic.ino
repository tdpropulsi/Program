
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
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <FirebaseESP8266.h>
#endif

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "wifi bayar"
#define WIFI_PASSWORD "12345678"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyB3AhEYnuLIqsTAn3O3GaYn_9OaQtx6rdM"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://esp32-gilang-data-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "gilangfajrulfalah@gmail.com"
#define USER_PASSWORD "12345678"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseJsonArray sdata;

FirebaseAuth auth;
FirebaseConfig config;

double x[] = { 2.21157592143821, 1.91845243041487, 1.62622976818985, 1.33638170085965, 1.04574670246877, 0.87953729097029, 0.802153534799653, 0.724262293807299, 0.65531316459534, 0.724840143746502, 0.801658601050074, 0.878412268697874, 0.8989158032545, 0.89938683213145, 0.900130876143427, 0.900425071068738, 0.89932180121624, 0.946314083735258, 1.02105192135533, 1.09027962522303, 1.15018099841095, 1.19758496583272, 1.23046061602548, 1.24752636630622, 1.24781372079968, 1.23090898150461, 1.19754485852316, 1.14970127550003, 1.09004979528935, 1.02141878795231, 0.946766067665274, 0.899197021401779, 0.899863251571795, 0.899912245994274, 0.899883875594634, 0.899774600773141, 0.899668922608705, 0.899692407178688, 0.89980904021348, 0.899883998159673, 0.901855434438638, 0.922385307787087, 0.93397360749791, 0.931269731668428, 0.915295735703946, 0.899428982421826, 0.89962413212286, 0.899789442947154, 0.899953216115886, 0.899892377480906, 0.899667098803339, 0.899593208267521, 0.899793715330206, 0.899989700134094, 0.899881679718654, 0.899605740771348, 0.899575396385152, 0.899874100000171, 0.90006866380735, 0.899795945016652, 0.899377334692989, 0.899691534208209, 0.91359250882356, 0.994413554128169, 1.05963003229033, 1.10372776149601, 1.12467706719045, 1.12773612751152, 1.10115657878858, 1.04853876586974, 0.971262603442039, 0.89859988594634, 0.899779392949062, 0.90055925815742, 0.89983002621955, 0.89861560352531, 0.899629105588206, 0.945725444925452, 1.23910608639798, 1.53074636262396, 1.82010656334299, 2.11049140545113, 2.40917460671675, 2.30168555588866, 2.01522743486622, 1.72500060779151, 1.43274762647558, 1.14220267152259, 0.894491493296662, 0.826424468862433, 0.75217689519845, 0.673833751108118, 0.700956985305267, 0.775405144238556, 0.851155718601131, 0.9009637094623, 0.899790224277496, 0.899202500447148, 0.899408531389769, 0.900445170562991, 0.919641158843293, 0.996579196922619, 1.06789584017488, 1.1311641817097, 1.18337149886825, 1.22159190285406, 1.24382274445207, 1.24930371243584, 1.23807460294738, 1.21052530100297, 1.16758809913896, 1.11124484081877, 1.04457942829487, 0.971305906426911, 0.901034288666808, 0.899502001669472, 0.899636400392251, 0.899745759489174, 0.899861319799422, 0.899896966798475, 0.899799364939956, 0.899684677250042, 0.899669716591856, 0.899538091797477, 0.916635358672207, 0.931735983655361, 0.933520569266505, 0.921307238835222, 0.901068987780551, 0.899831887795285, 0.899663677968865, 0.899609382107985, 0.899778065170756, 0.899961606525947, 0.899893268466162, 0.899652705030683, 0.899586752170228, 0.899811185728122, 0.900008999782749, 0.89986838932541, 0.899568167661613, 0.899571800466496, 0.899941340275328, 0.900129483833417, 0.899605192138571, 0.898761215681302, 0.969764000799307, 1.03957976491171, 1.09183254827904, 1.11887738489436, 1.12990800701704, 1.11259792919159, 1.0688389173305, 0.998911430941267, 0.913620656813775, 0.899420624815758, 0.899133889649427, 0.900184972225225, 0.900746862528637, 0.899159506176946, 0.896573178106239, 1.14195823597353, 1.43267156894219, 1.72501678018539, 2.01525464883881, 2.30169937291313, 2.40918546027952 };

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
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
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

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
}

void loop()
{

  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready()){
    for(int i = 0; i<250;i++){
      sdata.set("/["+String[i]+"]" + x[i]);
      delay(10);
    }
    Firebase.setArray(fbdo, "test/int", sdata);
  }
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