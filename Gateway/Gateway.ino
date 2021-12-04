#include <ThingsBoard.h>
#include <LoRa.h>
#include "boards.h"
#include <ArduinoJson.h>
#include <WiFi.h>

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);

int status = WL_IDLE_STATUS;

bool debug = true;

String received = "";
int rxCheckPercent = -1;

unsigned long packetId = 0;
unsigned short int boardId;
String umidade_str = "";
String temp_str = "";
double longitude = 0.0;
double latitude = 0.0;
String timestamp = "";

#define TICKS_ESPERA_ENVIO_JSON     ( TickType_t )10000
#define TEMPO_PARA_VERIFICAR_CONEXAO     1000  //ms
#define TAMANHO_FILA_POSICOES_JSON 100 //últimas 100 medições

//Fila de dados - RTOS
QueueHandle_t xQueue_JSON;

//Semáforo - RTOS
SemaphoreHandle_t xSerial_semaphore;



void taskLoraParser(void *pvParameters){
  while(true){
    
    vTaskDelay(10 / portTICK_PERIOD_MS);

    xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
    int packetSize = LoRa.parsePacket();
    
    if(packetSize){  
      if(debug){
        //print incoming packet size
        Serial.print("INFO: Incoming packet size: ");
        Serial.println(packetSize);
      }
    
      received = "";
      
      //read the incoming packet
      for (int i = 0; i < packetSize; i++) {
        received = received + (char)LoRa.read();
      }
    
      //print the incoming packet with RSSI
      rxCheckPercent = LoRa.packetRssi();
      
      //map rssi value to percentage
      rxCheckPercent = map(rxCheckPercent, -145, -30, 0, 100);
      
      xQueueSend(xQueue_JSON, ( void * ) &received, TICKS_ESPERA_ENVIO_JSON);
      
      if(debug){
        Serial.print("INFO: Received packet '");
        Serial.print(received);
        Serial.print("' with RSSI ");
        Serial.println(rxCheckPercent);
        
      }
    }
    xSemaphoreGive(xSerial_semaphore);
  }
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void InitLoRa(){
  if(debug)
    Serial.println("LoRa Receiver");
  LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);
  if (!LoRa.begin(LoRa_frequency)) {
      if(debug)
        Serial.println("Starting LoRa failed!");
      while (1);
  }
  LoRa.setSyncWord(0xF3);
  LoRa.enableCrc();
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      vTaskDelay( 500 / portTICK_PERIOD_MS );
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}

void taskWifiTBSender(void *pvParameter) {

  String JSON_str = "";

  xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
  InitWiFi(); //Connect to wifi
  xSemaphoreGive(xSerial_semaphore);

  xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
  InitLoRa(); //Connect and setup LoRa
  xSemaphoreGive(xSerial_semaphore);

  
  //INICIO DO LOOP
  while(true){
    
    vTaskDelay( TEMPO_PARA_VERIFICAR_CONEXAO / portTICK_PERIOD_MS );
    //Check if Wifi is connected, else tries to reconnect
    xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
    if (WiFi.status() != WL_CONNECTED) {
      reconnect();
    }
    xSemaphoreGive(xSerial_semaphore);


    if( xQueueReceive( xQueue_JSON, &( JSON_str ), TICKS_ESPERA_ENVIO_JSON) ){

      String boardToken = "";
      StaticJsonDocument<300> data;
    
      //convert incoming string in json object using ArduinoJson.h library
      DeserializationError error = deserializeJson(data, JSON_str);
      if (error) {
        if(debug){
          xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
          Serial.print(F("ERROR: deserializeJson() failed: "));
          Serial.println(error.f_str());
          xSemaphoreGive(xSerial_semaphore);
        }
      }
    
      //extract sensors values from json object
      packetId = data["packet_id"];
      boardId = data["board_id"];
      timestamp = (const char*)data["ts"];
      temp_str = (const char*)data["temperature"];
      umidade_str = (const char*)data["humidity"];
      latitude = data["latitude"];
      longitude = data["longitude"];
      
      
    
      if(debug){
        xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
        Serial.print("INFO: ID mes: ");
        Serial.println(packetId);
        Serial.print("INFO: DHT 11 Temp: ");
        Serial.println(temp_str);
        Serial.print("INFO: DHT 11 Hum: ");
        Serial.println(umidade_str);
        xSemaphoreGive(xSerial_semaphore);
      }
    
        //subscribed = false;
        
      switch(boardId){
  
        case 1 :
        boardToken = TOKEN_1;
        break;
  
        case 2 :
        boardToken = TOKEN_2;
        break;
        
      }
      // Connect to the ThingsBoard
      if(debug){
        xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
        Serial.print("Connecting to: ");
        Serial.print(THINGSBOARD_SERVER);
        Serial.print(" with token ");
        Serial.println(boardToken);
        xSemaphoreGive(xSerial_semaphore);
      }
      xSemaphoreTake(xSerial_semaphore, portMAX_DELAY );
      if (!tb.connect(THINGSBOARD_SERVER, boardToken.c_str())) {
        if(debug)
          Serial.println("Failed to connect");
      }
      else{
        tb.sendTelemetryInt("temperature", temp_str.toInt());
        tb.sendTelemetryInt("humidity", umidade_str.toInt());
        tb.sendTelemetryFloat("latitude",latitude);
        tb.sendTelemetryFloat("longitude",longitude);
        tb.sendTelemetryString("ts",timestamp.c_str());
        tb.sendAttributeInt("board_id",boardId);
        tb.loop();
        tb.disconnect();
        if (debug){
          Serial.println("Sending data to ThingsBoard");
          Serial.print("Board ID: ");
          Serial.println(boardId);
          Serial.print("Temp:");
          Serial.print(temp_str.toInt());
          Serial.print("\tHum:");
          Serial.println(umidade_str.toInt());
          Serial.print("Latitude:");
          Serial.print(latitude);
          Serial.print("\tLongitude:");
          Serial.println(longitude);
          if (u8g2) {
            u8g2->clearBuffer();
            char buf[256];
            u8g2->drawStr(0, 9, "Received OK!");
            //u8g2->drawStr(0, 20, recv.c_str());
            snprintf(buf, sizeof(buf), "Temp:%s", temp_str);
            u8g2->drawStr(0, 20, buf);
            snprintf(buf, sizeof(buf), "Humidity:%s", umidade_str);
            u8g2->drawStr(0, 31, buf);
            snprintf(buf, sizeof(buf), "Lat:%f", latitude);
            u8g2->drawStr(0, 42, buf);
            snprintf(buf, sizeof(buf), "Long:%f", longitude);
            u8g2->drawStr(0, 54, buf);
            snprintf(buf, sizeof(buf), "BoardID:%d", boardId);
            u8g2->drawStr(0, 66, buf);
            u8g2->sendBuffer();
          }
        }
      }
      xSemaphoreGive(xSerial_semaphore);
    }
  }
}


void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    vTaskDelay(1500 / portTICK_PERIOD_MS);

    /* Criação da fila */         
    xQueue_JSON = xQueueCreate(TAMANHO_FILA_POSICOES_JSON, 12);
    
    if (xQueue_JSON == NULL){
      Serial.println("Falha ao inicializar filas. O programa nao pode prosseguir. O ESP32 sera reiniciado...");
      delay(2000);
      ESP.restart();
    }
    
    /* Criação dos semáforos */        
    xSerial_semaphore = xSemaphoreCreateMutex();

    if (xSerial_semaphore == NULL){
      Serial.println("Falha ao inicializar semaforos. O programa nao pode prosseguir. O ESP32 sera reiniciado...");
      delay(2000);
      ESP.restart();
    }
    /* Configuração das tarefas */
    xTaskCreatePinnedToCore(
      taskLoraParser                    /* Funcao a qual esta implementado o que a tarefa deve fazer */
    , "LoRa_read"                     /* Nome (para fins de debug, se necessário) */
    ,  4096                             /* Tamanho da stack (em words) reservada para essa tarefa */
    ,  NULL                             /* Parametros passados (nesse caso, não há) */
    ,  6                                /* Prioridade */
    ,  NULL                          /* Handle da tarefa, opcional (nesse caso, não há) */
    ,  1);
    
    xTaskCreatePinnedToCore(
    taskWifiTBSender             
    , "wifi_TB"        
    ,  4096    
    ,  NULL                      
    ,  5
    ,  NULL
    ,  1);
    
}

void loop()
{

}
