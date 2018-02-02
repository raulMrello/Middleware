/*
 * ActiveModule.h
 *
 *  Created on: Ene 2018
 *      Author: raulMrello
 *
 *	ActiveModule es un interfaz que proporciona caracter�sticas comunes de funcionamiento relativas a los m�dulos de
 *	gesti�n de alto nivel.
 *
 *	Incluyen su propio hilo de ejecuci�n, su topic_base de publicaci�n y de suscripci�n, m�todos comunes de acceso a
 *	datos almacenados en memoria NV.
 */
 
#ifndef __ActiveModule__H
#define __ActiveModule__H

#include "mbed.h"
#include "StateMachine.h"
#include "MQLib.h"
#include "FSManager.h"

class ActiveModule : public StateMachine {
  public:
              
    /** Constructor, que asocia un nombre, as� como el tama�o de stack necesario para el thread
     *  @param name Nombre del m�dulo
     *  @param priority Prioridad del thread asociado
     *  @param stack_size Tama�o de stack asociado al thread
     */
    ActiveModule(const char* name, osPriority priority=osPriorityNormal, uint32_t stack_size = OS_STACK_SIZE, FSManager* fs = NULL, bool defdbg = false);


    /** Destructor
     */
    virtual ~ActiveModule(){}


    /** Chequea si el m�dulo est� preparado, es decir su thread est� corriendo.
     * 	@return True: thread corriendo, False: Inicializando
     */
    bool ready() { return _ready; }


    /** Chequea si el m�dulo tiene las trazas de depuraci�n activadas
     * 	@return True: activadas, False: desactivadas
     */
    bool degugActive() { return _defdbg; }
  
  
    /** Configura el topic base para la publicaci�n de mensajes
     *  @param pub_topic_base Topic base para la publicaci�n
     */
    void setPublicationBase(const char* pub_topic_base){
    	_pub_topic_base = pub_topic_base;
    }
    

    /** Configura el topic base para la suscripci�n de mensajes
     *  @param sub_topic_base Topic base para la suscripci�n
     */
    void setSubscriptionBase(const char* sub_topic_base){
    	_sub_topic_base = sub_topic_base;
    }


    /** Interfaz para postear un mensaje de la m�quina de estados en el Mailbox de la clase heredera
     *  @param msg Mensaje a postear
     */
    virtual void putMessage(State::Msg *msg) = 0;


  protected:

    const char* _pub_topic_base;				/// Nombre del topic base para las publicaciones
    const char* _sub_topic_base;				/// Nombre del topic base para las suscripciones
    bool _defdbg;								/// Flag para depuraci�n por defecto
    MQ::SubscribeCallback   _subscriptionCb;    /// Callback de suscripci�n a topics
    MQ::PublishCallback     _publicationCb;     /// Callback de publicaci�n en topics

  private:

    static const uint8_t MaxNameLength = 16;	/// Tama�o del nombre
    Thread* _th;								/// Thread asociado al m�dulo
    bool _ready;								/// Flag para indicar el estado del m�dulo a nivel de thread
    char _name[MaxNameLength+1];				/// Nombre del m�dulo (ej. "[Name]..........")

    FSManager* _fs;								/// Gestor del sistema de backup en memoria NVS

    State _stInit;								/// Variable de estado para stInit



  protected:

    /** Rutina de entrada a la m�quina de estados (gestionada por la clase heredera)
     */
    virtual State::StateResult Init_EventHandler(State::StateEvent* se) = 0;


    /** Interfaz para obtener un evento osEvent de la clase heredera
     *  @param msg Mensaje a postear
     */
    virtual osEvent getOsEvent() = 0;


	/** Callback invocada al recibir una actualizaci�n de un topic local al que est� suscrito
     *  @param topic Identificador del topic
     *  @param msg Mensaje recibido
     *  @param msg_len Tama�o del mensaje
     */
    virtual void subscriptionCb(const char* topic, void* msg, uint16_t msg_len) = 0;


	/** Callback invocada al finalizar una publicaci�n local
     *  @param topic Identificador del topic
     *  @param result Resultado de la publicaci�n
     */
    virtual void publicationCb(const char* topic, int32_t result) = 0;


	/** Graba un par�metro en la memoria NV
	 * 	@param param_id Identificador del par�metro
	 * 	@param data Datos asociados
	 * 	@param size Tama�o de los datos
	 * 	@param type Tipo de los datos
	 * 	@return True: �xito, False: no se pudo salvar
	 */
	virtual bool saveParameter(const char* param_id, void* data, size_t size, NVSInterface::KeyValueType type);


	/** Recupera un par�metro de la memoria NV
	 * 	@param param_id Identificador del par�metro
	 * 	@param data Receptor de los datos asociados
	 * 	@param size Tama�o de los datos a recibir
	 * 	@param type Tipo de los datos
	 * 	@return True: �xito, False: no se pudo recuperar
	 */
	virtual bool restoreParameter(const char* param_id, void* data, size_t size, NVSInterface::KeyValueType type);


  private:

    /** Hilo de ejecuci�n propio.
     */
    void task();

};
     
#endif /*__ActiveModule__H */

/**** END OF FILE ****/


