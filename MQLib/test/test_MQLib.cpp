#include "mbed.h"
#include "MQLib.h"
#include "MQSerialBridge.h"
#include "Logger.h"


// **************************************************************************
// *********** DEFINICIONES *************************************************
// **************************************************************************


/** Macro de impresión de trazas de depuración */
#define DEBUG_TRACE(format, ...)    if(logger){logger->printf(format, ##__VA_ARGS__);}


// **************************************************************************
// *********** OBJETOS  *****************************************************
// **************************************************************************

/** Canal de comunicación remota */
static MQSerialBridge* qserial;
static Logger* logger;

/** Callbacks MQLib */
static MQ::PublishCallback publ_cb;

/** Topics de publicación, suscripción */
static const char* sub_topic = "/move/stop";
static char* txt_msg = "Hello";
static void* msg = (void*) txt_msg;
static uint32_t msg_len = strlen(txt_msg)+1;


// **************************************************************************
// *********** TEST  ********************************************************
// **************************************************************************

static void publCb(const char* name, int32_t){
}


//------------------------------------------------------------------------------------
static void subscriptionCb(const char* topic, void* msg, uint16_t msg_len){
    if(MQ::MQClient::isTopicToken(topic, "/A")){
        DEBUG_TRACE("\r\nTopic:%s, token:/A msg:%s\r\n", topic, (char*)msg);
        MQ::MQClient::publish("config/start/AB", msg, msg_len, &publ_cb);
        return;
    }
    if(MQ::MQClient::isTopicToken(topic, "/AB")){
        DEBUG_TRACE("\r\nTopic:%s, token:/AB msg:%s\r\n", topic, (char*)msg);
        MQ::MQClient::publish("config/start/ABC", msg, msg_len, &publ_cb);
        return;
    }
    if(MQ::MQClient::isTopicToken(topic, "/ABC")){
        DEBUG_TRACE("\r\nTopic:%s, token:/ABC msg:%s\r\n", topic, (char*)msg);
        MQ::MQClient::publish("config/start/ABCD", msg, msg_len, &publ_cb);
        return;
    }
    if(MQ::MQClient::isTopicToken(topic, "/ABCD")){
        DEBUG_TRACE("\r\nTopic:%s, token:/ABCD msg:%s\r\n", topic, (char*)msg);
        return;
    }
}



//------------------------------------------------------------------------------------
void test_MQLib(){
            
    publ_cb = callback(&publCb);
    
    // --------------------------------------
    // Inicia el canal de comunicación remota
    //  - Pines USBTX, USBRX a 115200bps y 256 bytes para buffers
    //  - Configurado por defecto en modo texto
    qserial = new MQSerialBridge(USBTX, USBRX, 115200, 256);
    logger = (Logger*)qserial;

    // --------------------------------------
    // Inicia el canal de depuración (compartiendo salida remota)
    logger = qserial;    
    DEBUG_TRACE("\r\nIniciando test_MQLib...\r\n");


        
    
    DEBUG_TRACE("\r\nSuscripción a eventos move/stop/# ...");
    MQ::MQClient::subscribe(sub_topic, new MQ::SubscribeCallback(&subscriptionCb));
    DEBUG_TRACE("OK!\r\n");
    
    // --------------------------------------
    // Arranca el test
    DEBUG_TRACE("\r\n...................INICIO DEL TEST.........................\r\n");  
    DEBUG_TRACE("\r\nPublish on config/start/A \r\n");    
    MQ::MQClient::publish("config/start/A", msg, msg_len, &publ_cb);

}

