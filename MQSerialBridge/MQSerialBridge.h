/*
 * MQSerialBridge.h
 *
 *  Created on: Nov 2017
 *      Author: raulMrello
 *
 *  MQSerialBridge es el m�dulo C++ que proporciona un puente de comunicaciones MQ a trav�s de un puerto serie, haciendo
 *  uso de un componente SerialTerminal.
 *  Este m�dulo recibir� publicaciones a trav�s de un enlace serie y las insertar� como si las hubiera publicado su propio
 *  cliente.
 *  Por otro lado, los topics a los que est� suscrito, los replicar� hacia el enlace serie.
 *  El protocolo ser� una mezcla de modo texto y binario, utilizando un espacio en blanco (BREAK) como delimitador de trama.
 *  El formato de los mensajes ser� el siguiente:
 *  
 *      TOPIC \n TAMA�O_DATOS \n CRC32_DATOS \n\0 DATOS[n] 
 * 
 *  El topic, el tama�o y el crc vendr�n en modo texto y los datos en binario como un array de bytes. Se chequear� que tanto 
 *  el tama�o como el crc concuerdan, para darlo por bueno.
 *
 *  Adem�s este m�dulo escucha publicaciones en el topic por defecto ($cfg_topic = "mqserialbridge") aunque puede cambiarse en 
 *  el constructor, de forma que desde el enlace serie se podr� configurar su funcionamiento, por ejemplo para realizar las 
 *  siguientes funciones.
 *
 *  Suscripci�n a topics: 
 *      TOPIC = $cfg_topic/suscr
 *      SIZE  = strlen(DATA)+1
 *      CRC32 = crc32(DATA, strlen(DATA)+1)
 *      DATA  = "topic_a_suscribirse\nTama�o_del_mensaje_asociado\n\0"   (DATA_ARGS = 2)
 *
 *  Publicaci�n remota a topic: 
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
     *  @param tx L�nea tx del Puerto serie asignado
     *  @param rx L�nea rx del Puerto serie asignado
     *  @param baud Velocidad del puerto serie
     *  @param recv_buf_size Tama�o a reservar para el buffer de recepci�n
     *  @param cfg_topic Topic de configuraci�n, para ajuste de par�metros propios
     *  @param token Caracter de separaci�n de argumentos. Por defecto '\n'
     */
    MQSerialBridge(PinName tx, PinName rx, uint32_t baud, uint16_t recv_buf_size, const char* cfg_topic = "mqserialbridge", const char* token = "\n");


    /** addSubscription()
     *  A�ade una suscripci�n a un topic concreto
     *  @param topic Topic al que se suscribe
     *  @param msg_size Tama�o del mensaje asociado al topic
     */
    void addSubscription(const char* topic, uint32_t msg_size);

      
protected:

    /** M�ximo n�mero de argumentos que pueden ir asociados a una publicaci�n desde el enlace serie */
    static const uint8_t MaxNumArguments = 4;


    /** task()
     *  Hilo de ejecuci�n asociado para el procesado de las comunicaciones serie
     */
    void task(); 

                
	/** subscriptionCb()
     *  Callback invocada al recibir una actualizaci�n de un topic al que est� suscrito
     *  @param topic Identificador del topic
     *  @param msg Mensaje recibido
     *  @param msg_len Tama�o del mensaje
     */    
    void subscriptionCb(const char* topic, void* msg, uint16_t msg_len);

        
	/** publicationCb()
     *  Callback invocada al finalizar una publicaci�n
     *  @param topic Identificador del topic
     *  @param result Resultado de la publicaci�n
     */    
     void publicationCb(const char* topic, int32_t result);


    /** onRxComplete()
     *  Manejador ISR de datos recibidos v�a serie
     */
    void onRxComplete();


    /** onRxTimeout()
     *  Manejador ISR de timeout en la recepci�n serie
     */
    void onRxTimeout();


    /** onRxOvf()
     *  Manejador ISR de buffer overflow en la recepci�n serie
     */
    void onRxOvf();


    /** onTxComplete()
     *  Manejador ISR de datos enviados v�a serie
     */
    void onTxComplete();


    /** onRxData()
     *  Procesamiento dedicado de los bytes recibidos.
     *  @param buf Buffer de datos recibidos
     *  @param size N�mero de dato recibidos hasta el momento
     *  @return Indica si el procesado tiene una trama v�lida (true) o no (false)
     */
    bool onRxData(uint8_t* buf, uint16_t size);   


    /** Flags de tarea (asociados a la m�quina de estados) */
    enum SigEventFlags{
        ReceivedData   = (1<<0),
        SentData       = (1<<1),
        TimeoutOnRecv  = (1<<2),
        OverflowOnRecv = (1<<3),
    };
      
    
    Thread      _th;                            /// Manejador del thread
    uint32_t    _timeout;                       /// Manejador de timming en la tarea
         
    MQ::SubscribeCallback   _subscriptionCb;    /// Callback de suscripci�n a topics
    MQ::PublishCallback     _publicationCb;     /// Callback de publicaci�n en topics
    
    char* _tbuf;                                /// Buffer para el env�o de datos
    char* _rbuf;                                /// Buffer para la recepci�n de datos
    uint16_t _rbufsize;                         /// Tama�o del buffer
    char* _token;                               /// Caracter de separaci�n de argumentos
    char* _cfg_topic;                           /// Topic de configuraci�n       

};

#endif  /** _MQSERIALBRIDGE_H */

