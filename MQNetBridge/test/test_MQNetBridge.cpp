#include "mbed.h"
#include "MQLib.h"
#include "MQNetBridge.h"
#include "Logger.h"


// **************************************************************************
// *********** DEFINICIONES *************************************************
// **************************************************************************


/** Macro de impresión de trazas de depuración */
#define DEBUG_TRACE(format, ...)    if(logger){logger->printf(format, ##__VA_ARGS__);}


// **************************************************************************
// *********** OBJETOS  *****************************************************
// **************************************************************************


/** Canal de depuración */
static Logger* logger;
/** Canal de comunicación remota MQTT */
static MQNetBridge* qnet;

static MQ::PublishCallback publ_cb;


// **************************************************************************
// *********** TEST  ********************************************************
// **************************************************************************

static void publCb(const char* name, int32_t){
}


//------------------------------------------------------------------------------------
static void subscCallback(const char* topic, void* msg, uint16_t msg_len){
    DEBUG_TRACE("Recibido mensaje MQTT del topic[%s] con mensaje[%s]\r\n", topic, msg);
    MQ::MQClient::publish("test/mqtt/stat/echo", msg, msg_len, &publ_cb);
}


//------------------------------------------------------------------------------------
void test_MQNetBridge(){
            
    publ_cb = callback(&publCb);
    
    // --------------------------------------
    // Inicia el canal de depuración
    //  - Pines USBTX, USBRX a 115200bps y 256 bytes para buffers
    //  - Configurado por defecto en modo texto
    logger = new Logger(USBTX, USBRX, 16, 115200);
    DEBUG_TRACE("\r\nIniciando test_MQNetBridge...\r\n");


    
    // --------------------------------------
    // Creo módulo NetBridge MQTT que escuchará en el topic local "mqnetbridge"
    DEBUG_TRACE("\r\nCreando NetBridge...");    
    qnet = new MQNetBridge("sys/cmd/mqnetbridge");
    qnet->setDebugChannel(logger);
    while(qnet->getStatus() != MQNetBridge::Ready){
        Thread::yield();
    }
    DEBUG_TRACE("OK!"); 

    // Configuro el acceso al servidor mqtt
    DEBUG_TRACE("\r\nConfigurando conexión...");            
    static char* mnb_cfg = "cli,usr,pass,192.168.254.29,1883,Invitado,11FF00DECA";
    MQ::MQClient::publish("sys/cmd/mqnetbridge/conn", mnb_cfg, strlen(mnb_cfg)+1, &publ_cb);
    while(qnet->getStatus() != MQNetBridge::Connected){
        Thread::yield();
        MQNetBridge::Status stat = qnet->getStatus();
        if(stat >= MQNetBridge::WifiError){
            char *zeromsg = "0";
            DEBUG_TRACE("\r\nERR_CONN %d. Desconectando...", (int)stat);      
            MQ::MQClient::publish("sys/cmd/mqnetbridge/disc", zeromsg, strlen(zeromsg)+1, &publ_cb);
            while(qnet->getStatus() != MQNetBridge::Ready){
                Thread::yield();
            }
            DEBUG_TRACE("\r\nReintentando conexión...");     
            MQ::MQClient::publish("sys/cmd/mqnetbridge/conn", mnb_cfg, strlen(mnb_cfg)+1, &publ_cb);
        }
    }
    DEBUG_TRACE("OK!");
    
    // Hago que escuche todos los topics del recurso "test/mqtt/cmd"
    static char* mnb_rsubtopic = "test/mqtt/cmd/#";
    DEBUG_TRACE("\r\nSuscribiendo a topic remoto: %s...", mnb_rsubtopic);
    MQ::MQClient::publish("sys/cmd/mqnetbridge/rsub", mnb_rsubtopic, strlen(mnb_rsubtopic)+1, &publ_cb);
    while(qnet->getStatus() != MQNetBridge::Connected){
        Thread::yield();        
    }
    DEBUG_TRACE("OK!");
    
    // Hago que escuche topics locales para redireccionarlos al exterior
    static char* mnb_lsubtopic0 = "test/mqtt/stat/#";
    DEBUG_TRACE("\r\nSuscribiendo a topic local: %s", mnb_lsubtopic0);    
    MQ::MQClient::publish("sys/cmd/mqnetbridge/lsub", mnb_lsubtopic0, strlen(mnb_lsubtopic0)+1, &publ_cb);
    
    // Se suscribe a los topics de test
    MQ::MQClient::subscribe("test/mqtt/cmd/#", new MQ::SubscribeCallback(&subscCallback));
    
    // Publico topic de notificación de estado
    MQ::MQClient::publish("test/mqtt/stat/conn", (void*)"Ready!", strlen("Ready!") + 1, &publ_cb);
    
    // --------------------------------------
    // Arranca el test
    DEBUG_TRACE("\r\n...................INICIO DEL TEST.........................\r\n");    
    DEBUG_TRACE("\r\n- Escucha en test/mqtt/cmd/# ");    
    DEBUG_TRACE("\r\n- Publica en test/mqtt/sta/...");    
}

