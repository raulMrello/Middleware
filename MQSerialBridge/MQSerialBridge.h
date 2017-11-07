/*
 * MQSerialBridge.h
 *
 *  Created on: Nov 2017
 *      Author: raulMrello
 *
 *  MQSerialBridge es el módulo C++ que proporciona un puente de comunicaciones MQ a través de un puerto serie, haciendo
 *  uso de un componente SerialTerminal.
 *  Este módulo recibirá publicaciones a través de un enlace serie y las insertará como si las hubiera publicado su propio
 *  cliente.
 *  Por otro lado, los topics a los que esté suscrito, los replicará hacia el enlace serie.
 *  El protocolo será una mezcla de modo texto y binario, utilizando un espacio en blanco (BREAK) como delimitador de trama.
 *  El formato de los mensajes será el siguiente:
 *  
 *      TOPIC \n TAMAÑO_DATOS \n CRC32_DATOS \n\0 DATOS[n] 
 * 
 *  El topic, el tamaño y el crc vendrán en modo texto y los datos en binario como un array de bytes. Se chequeará que tanto 
 *  el tamaño como el crc concuerdan, para darlo por bueno.
 *
 *  Además este módulo escucha publicaciones en el topic por defecto ($cfg_topic = "mqserialbridge") aunque puede cambiarse en 
 *  el constructor, de forma que desde el enlace serie se podrá configurar su funcionamiento, por ejemplo para realizar las 
 *  siguientes funciones.
 *
 *  Suscripción a topics: 
 *      TOPIC = $cfg_topic/suscr
 *      SIZE  = strlen(DATA)+1
 *      CRC32 = crc32(DATA, strlen(DATA)+1)
 *      DATA  = "topic_a_suscribirse\nTamaño_del_mensaje_asociado\n\0"   (DATA_ARGS = 2)
 *
 *  Publicación remota a topic: 
 *      TOPIC = topic/a/publicar
 *      SIZE  = sizeof(DATA)
 *      CRC32 = crc32(DATA, sizeof(DATA))
 *      DATA  = ...data...   (DATA_ARGS = 1)
 */
 
 
#ifndef _MQSERIALBRIDGE_H
#define _MQSERIALBRIDGE_H


#include "mbed.h"
#include "SerialTerminal.h"
#include "MQLib.h"
  
  
  
//---------------------------------------------------------------------------------
//- class MQSerialBridge ----------------------------------------------------------
//---------------------------------------------------------------------------------


class MQSerialBridge : public SerialTerminal {

public:
    /** MQSerialBridge()
     *  Crea el objeto asignando un puerto serie para la interfaz con el equipo digital
     *  @param tx Línea tx del Puerto serie asignado
     *  @param rx Línea rx del Puerto serie asignado
     *  @param baud Velocidad del puerto serie
     *  @param recv_buf_size Tamaño a reservar para el buffer de recepción
     *  @param cfg_topic Topic de configuración, para ajuste de parámetros propios
     *  @param token Caracter de separación de argumentos. Por defecto '\n'
     */
    MQSerialBridge(PinName tx, PinName rx, uint32_t baud, uint16_t recv_buf_size, const char* cfg_topic = "mqserialbridge", const char* token = "\n");


    /** addSubscription()
     *  Añade una suscripción a un topic concreto
     *  @param topic Topic al que se suscribe
     *  @param msg_size Tamaño del mensaje asociado al topic
     */
    void addSubscription(const char* topic, uint32_t msg_size);

      
protected:

    /** Máximo número de argumentos que pueden ir asociados a una publicación desde el enlace serie */
    static const uint8_t MaxNumArguments = 4;


    /** task()
     *  Hilo de ejecución asociado para el procesado de las comunicaciones serie
     */
    void task(); 

                
	/** subscriptionCb()
     *  Callback invocada al recibir una actualización de un topic al que está suscrito
     *  @param topic Identificador del topic
     *  @param msg Mensaje recibido
     *  @param msg_len Tamaño del mensaje
     */    
    void subscriptionCb(const char* topic, void* msg, uint16_t msg_len);

        
	/** publicationCb()
     *  Callback invocada al finalizar una publicación
     *  @param topic Identificador del topic
     *  @param result Resultado de la publicación
     */    
     void publicationCb(const char* topic, int32_t result);


    /** onRxComplete()
     *  Manejador ISR de datos recibidos vía serie
     */
    void onRxComplete();


    /** onRxTimeout()
     *  Manejador ISR de timeout en la recepción serie
     */
    void onRxTimeout();


    /** onRxOvf()
     *  Manejador ISR de buffer overflow en la recepción serie
     */
    void onRxOvf();


    /** onTxComplete()
     *  Manejador ISR de datos enviados vía serie
     */
    void onTxComplete();


    /** onRxData()
     *  Procesamiento dedicado de los bytes recibidos.
     *  @param buf Buffer de datos recibidos
     *  @param size Número de dato recibidos hasta el momento
     *  @return Indica si el procesado tiene una trama válida (true) o no (false)
     */
    bool onRxData(uint8_t* buf, uint16_t size);   


    /** Flags de tarea (asociados a la máquina de estados) */
    enum SigEventFlags{
        ReceivedData   = (1<<0),
        SentData       = (1<<1),
        TimeoutOnRecv  = (1<<2),
        OverflowOnRecv = (1<<3),
    };
      
    
    Thread      _th;                            /// Manejador del thread
    uint32_t    _timeout;                       /// Manejador de timming en la tarea
         
    MQ::SubscribeCallback   _subscriptionCb;    /// Callback de suscripción a topics
    MQ::PublishCallback     _publicationCb;     /// Callback de publicación en topics
    
    char* _tbuf;                                /// Buffer para el envío de datos
    char* _rbuf;                                /// Buffer para la recepción de datos
    uint16_t _rbufsize;                         /// Tamaño del buffer
    char* _token;                               /// Caracter de separación de argumentos
    char* _cfg_topic;                           /// Topic de configuración       

};

#endif  /** _MQSERIALBRIDGE_H */

