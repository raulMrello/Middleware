#include "mbed.h"
#include "MQLib.h"
#include "MQSerialBridge.h"
#include "Logger.h"
#include "easy-connect.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"


// **************************************************************************
// *********** DEFINICIONES *************************************************
// **************************************************************************


/** Macro de impresión de trazas de depuración */
#define DEBUG_TRACE(format, ...)    if(logger){Thread::wait(2); logger->printf(format, ##__VA_ARGS__);}

//#define MQTTCLIENT_QOS2 1


// **************************************************************************
// *********** OBJETOS  *****************************************************
// **************************************************************************

/** Canal de comunicación remota */
static MQSerialBridge* qserial;
/** Canal de depuración */
static Logger* logger;


// **************************************************************************
// *********** TEST  ********************************************************
// **************************************************************************


char* MBED_CONF_APP_WIFI_SSID;
char* MBED_CONF_APP_WIFI_PASSWORD; 

int arrivedcount = 0;
  
void messageArrived(MQTT::MessageData& md){
    MQTT::Message &message = md.message;
    DEBUG_TRACE("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    DEBUG_TRACE("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}
 

//------------------------------------------------------------------------------------
void mqttCfgSubscription(const char* name, void* msg, uint16_t msg_len){
    DEBUG_TRACE("%s %s\r\n", name, msg);
    // obtengo SSID y PASSWD
    char* host;
    char* tport;
    char* essid;
    char *passwd;
    host = strtok((char*)msg, ",");
    tport = strtok(0, ",");
    essid = strtok(0, ",");
    passwd = strtok(0, ",");
    if(host && tport && essid && passwd){
        int port = 1883;
        MBED_CONF_APP_WIFI_SSID = essid;
        MBED_CONF_APP_WIFI_PASSWORD = passwd;
        
        port = atoi(tport);
        float version = 0.6;
        char* topic = "testMQTT";
     
        DEBUG_TRACE("HelloMQTT: version is %.2f\r\n", version);
     
        NetworkInterface* network = easy_connect(true);
        if (!network) {
            DEBUG_TRACE("Network ERROR\r\n");
            return;
        }
     
        MQTTNetwork mqttNetwork(network);
     
        MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);
     
        const char* hostname = host;
        DEBUG_TRACE("Connecting to %s:%d\r\n", hostname, port);
        int rc = mqttNetwork.connect(hostname, port);
        if (rc != 0)
            DEBUG_TRACE("rc from TCP connect is %d\r\n", rc);
     
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.MQTTVersion = 3;
        data.clientID.cstring = "testMQTT";
        data.username.cstring = "testuser";
        data.password.cstring = "testpassword";
        if ((rc = client.connect(data)) != 0)
            DEBUG_TRACE("rc from MQTT connect is %d\r\n", rc);
     
        if ((rc = client.subscribe(topic, MQTT::QOS2, messageArrived)) != 0)
            DEBUG_TRACE("rc from MQTT subscribe is %d\r\n", rc);
     
        MQTT::Message message;
     
        // QoS 0
        char buf[100];
        sprintf(buf, "Hello World!  QoS 0 message from app version %f\r\n", version);
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)buf;
        message.payloadlen = strlen(buf)+1;
        rc = client.publish(topic, message);
        while (arrivedcount < 1)
            client.yield(100);
     
     
        if ((rc = client.unsubscribe(topic)) != 0)
            DEBUG_TRACE("rc from unsubscribe was %d\r\n", rc);
     
        if ((rc = client.disconnect()) != 0)
            DEBUG_TRACE("rc from disconnect was %d\r\n", rc);
     
        mqttNetwork.disconnect();
     
        DEBUG_TRACE("Version %.2f: finish %d msgs\r\n", version, arrivedcount);        
    }
}
 
void test_MQTT(){
            
    // --------------------------------------
    // Inicia el canal de comunicación remota
    //  - Pines USBTX, USBRX a 115200bps y 256 bytes para buffers
    //  - Configurado por defecto en modo texto
    qserial = new MQSerialBridge(USBTX, USBRX, 115200, 256);
    
    // --------------------------------------
    // Inicia el canal de depuración (compartiendo salida remota)
    logger = (Logger*)qserial;    
    DEBUG_TRACE("\r\nIniciando test_TouchManager...\r\n");

    DEBUG_TRACE("\r\nSuscripción a mqtt/config ...");
    MQ::MQClient::subscribe("mqtt/config", new MQ::SubscribeCallback(&mqttCfgSubscription));
    DEBUG_TRACE("OK!\r\n");
    
    // --------------------------------------
    // Arranca el test
    DEBUG_TRACE("\r\n...................INICIO DEL TEST.........................\r\n");    
    DEBUG_TRACE("\r\n- Ajustar eventos: mqtt/config Host,Port,SSID,Pass");    
}

