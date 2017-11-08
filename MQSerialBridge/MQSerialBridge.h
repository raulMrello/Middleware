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
 *
 *  El protocolo podr� configurarse en modo texto (con espacios) o modo mixto con (\n). Por ejemplo:
 *  
 *  Modo Texto: topic/to/pub DATO0 DATO1 DATO2 DATON[ \0]
 *  Modo Mixto: topic/to/pub[\n]data_size[\n]data_crc16[\n]data...[\n\0] 
 * 
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
    
    /** ModeType 
     *  Listado de los diferentes modos de funcionamiento
     */
    enum ModeType{
        TextMode,   /// Modo texto (token separador es el espacio ' ')
        MixMode,    /// Modo mixto (token separador es el caracter '\n')
    };
    
    /** MQSerialBridge()
     *  Crea el objeto asignando un puerto serie para la interfaz con el equipo digital
     *  @param tx L�nea tx del Puerto serie asignado
     *  @param rx L�nea rx del Puerto serie asignado
     *  @param baud Velocidad del puerto serie
     *  @param recv_buf_size Tama�o a reservar para el buffer de recepci�n
     *  @param cfg_topic Topic de configuraci�n, para ajuste de par�metros propios
     *  @param token Caracter de separaci�n de argumentos. Por defecto '\n'
     */
    MQSerialBridge(PinName tx, PinName rx, uint32_t baud, uint16_t recv_buf_size, const char* cfg_topic = "mqserialbridge", ModeType mode = TextMode);


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
    ModeType _mode;                             /// Modo de funcionamiento

};

#endif  /** _MQSERIALBRIDGE_H */

