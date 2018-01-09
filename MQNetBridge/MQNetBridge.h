/*
 * MQNetBridge.h
 *
 *  Created on: Nov 2017
 *      Author: raulMrello
 *
 *  MQNetBridge es el módulo C++ que proporciona un puente de comunicaciones MQ a través de un enlace de red MQTT.
 *  Este módulo recibirá publicaciones a través del protocolo MQTT y las insertará como si las hubiera publicado su propio
 *  cliente. Por otro lado, los topics a los que esté suscrito, los replicará hacia el enlace mqtt.
 *
 *  Por defecto este módulo se registra en MQLib escuchando en el topic base $(base)="mqnetbridge", de forma que 
 *  pueda ser configurado. Las configuraciones básicas que permite son las siguientes:
 *
 *  CONECTAR
 *  $(base)conn Cli,Usr,UsrPass,Host,Port,Essid,Passwd" 
 *      Solicita la conexión, configurando los parámetros necesarios.
 * 
 *  DESCONECTAR
 *  $(base)/disc 0" 
 *      Solicita la desconexión
 * 
 *  TIEMOUT MQTT_YIELD
 *  $(base)/yield MILLIS" 
 *      Solicita cambiar la temporización de espera de mqtt_yield
 *
 *  SUSCRIPCION LOCAL (MQLIB)
 *  $(base)/lsub TOPIC" 
 *      Permite suscribirse al topic local (MQLib) TOPIC y redirigir las actualizaciones recibidas al mismo topic mqtt.
 *
 *  SUSCRIPCION REMOTA
 *  $(base)/rsub TOPIC" 
 *      Permite suscribirse al topic remoto (MQTT) TOPIC y redirigir las actualizaciones al mismo topic mqlib. 
 * 
 *  QUITAR SUSCRIPCION REMOTA
 *  $(base)/runs TOPIC" 
 *      Permite quitar la suscribirse al topic remoto (MQTT) TOPIC.
 *
 *  ACTIVAR ESCUCHA MQTT
 *  $(base)/listen 0" 
 *      Permite suscribirse al topic remoto (MQTT) TOPIC y redirigir las actualizaciones al mismo topic mqlib. 
 * 
 */
 
 
#ifndef _MQNETBRIDGE_H
#define _MQNETBRIDGE_H


#include "mbed.h"
#include "Logger.h"
#include "MQLib.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
  
  
  
//---------------------------------------------------------------------------------
//- class MQNetBridge ----------------------------------------------------------
//---------------------------------------------------------------------------------


class MQNetBridge {

public:
        
    enum Status{
        Unknown,        /// No iniciado
        Ready,          /// Iniciado pero sin configurar la conexión
        Disconnected,   /// Configurado pero no conectado
        Connected,      /// Configurado y conectado
        WifiError,      /// Error al conectar con la red wifi
        SockError,      /// Error al abrir el socket con el servidor mqtt
        MqttError,      /// Error al conectar el cliente mqtt
    };
    
    /** MQNetBridge()
     *  Crea el objeto asignando un puerto serie para la interfaz con el equipo digital
     *  @param base_topic Topic base, utilizado para poder ser configurado
     */
    MQNetBridge(const char* base_topic, uint32_t mqtt_yield_millis = 10);
    
  
	/** setDebugChannel()
     *  Instala canal de depuración
     *  @param dbg Logger
     */
    void setDebugChannel(Logger* dbg){ _debug = dbg; }
    
                
	/** notifyRmoteSubscription()
     *  Callback invocada de forma estática al recibir una actualización de un topic remoto al que está suscrito
     *  @param md Referencia del mensaje recibido
     */    
    void notifyRemoteSubscription(MQTT::MessageData& md);


    /** getStatus()
     *  Obtiene el estado del módulo
     *  @return Estado del módulo
     */
    Status getStatus() { return _stat; }


    /** changeYieldTimeout()
     *  Modifica el timeout de mqtt yield
     *  @param millis Timeout en milisegundos
     */
    void changeYieldTimeout(uint32_t millis) { _yield_millis = millis; }    
    
      
protected:

    /** Número de items para la cola de mensajes entrantes */
    static const uint8_t MaxQueueEntries = 8;
    static const uint8_t RetriesOnError = 3;

    /** Flags de tarea (asociados a la máquina de estados) */
    enum SigEventFlags{
        ConnectSig              = (1<<0),   /// Flag para solicitar un inicio de conexión o reconexión
        DisconnectSig           = (1<<1),   /// Flag para solicitar una desconexión
        RemoteSubscriptionSig   = (1<<2),   /// Flag para solicitar una suscripción a un topic mqtt
        RemoteUnsubscriptionSig = (1<<3),   /// Flag para solicitar la no suscripción a un topic mqtt
        RemotePublishSig        = (1<<4),   /// Flag para solicitar la publicación a un topic mqtt
    };
    
    /** Estructura de datos de las solicitudes insertadas en la cola de proceso */
    struct RequestOperation_t{
        SigEventFlags id;                   /// Identificador de la operación a realizar
        char* data;                         /// Datos asociados a la operación
    };
      
    
    Thread  _th;                                        /// Manejador del thread
    uint32_t _timeout;                                  /// Timeout de espera en el Mailbox
    Status  _stat;                                      /// Estado del módulo
    Logger* _debug;                                     /// Canal de depuración
    Queue<RequestOperation_t, MaxQueueEntries> _queue;  /// Request queue    
    
    NetworkInterface* _network;
    MQTTNetwork *_net;                              /// Conexión MQTT
    MQTT::Client<MQTTNetwork, Countdown> *_client;  /// Cliente MQTT
    MQTTPacket_connectData _data;
         
    MQ::SubscribeCallback   _subscriptionCb;        /// Callback de suscripción a topics
    MQ::PublishCallback     _publicationCb;         /// Callback de publicación en topics
   
    
    char* _base_topic;                              /// Topic de configuración    
    char* _client_id;                               /// Ciente id MQTT
    char* _user;                                    /// Nombre de usuario MQTT
    char* _userpass;                                /// Clave usuario MQTT
    char* _host;                                    /// Servidor MQTT
    int   _port;                                    /// Puerto MQTT
    char* _essid;                                   /// Red wifi
    char* _passwd;                                  /// Clave wifi
    char* _gw;                                      /// Gateway IP
    uint32_t _yield_millis;                         /// Tiemout para mqtt yield

    /** task()
     *  Hilo de ejecución asociado para el procesado de las comunicaciones serie
     */
    void task(); 

                
	/** localSubscriptionCb()
     *  Callback invocada al recibir una actualización de un topic local al que está suscrito
     *  @param topic Identificador del topic
     *  @param msg Mensaje recibido
     *  @param msg_len Tamaño del mensaje
     */    
    void localSubscriptionCb(const char* topic, void* msg, uint16_t msg_len);
    

	/** localPublicationCb()
     *  Callback invocada al finalizar una publicación local
     *  @param topic Identificador del topic
     *  @param result Resultado de la publicación
     */    
     void localPublicationCb(const char* topic, int32_t result);
    

	/** connect()
     *  Inicia el interfaz de red wifi, conecta socket tcp y conecta client mqtt
     *  @return Código de error, o 0 si Success.
     */    
     int connect();
        

	/** disconnect()
     *  Desconecta el cliente mqtt, cierra el socket mqtt y finaliza el interfaz de red wifi
     */    
     void disconnect();
    

	/** reconnect()
     *  Desconecta y vuelve a conectar 
     *  @return Código de error, o 0 si Success.
     */    
     int reconnect(){ disconnect(); return(connect()); }


};

#endif  /** _MQNETBRIDGE_H */

