/*
 * MQLib.h
 *
 *  Created on: Sep 2017
 *      Author: raulMrello
 *
 *  MQLib es una librería que proporciona capacidades de publicación-suscripción de forma pasiva, sin necesidad de
 *	utilizar un thread dedicado y siempre corriendo en el contexto del publicador.
 *
 *	Consta de dos tipos de clases estáticas: MQBroker y MQClient.
 *
 *	MQBroker: se encarga de gestionar la lista de suscriptores y el paso de mensajes desde los publicadores a éstos.
 *	 Para ello necesita mantener una lista de topics y los suscriptores a cada uno de ellos.
 *
 *	MQClient: se encarga de hacer llegar los mensajes publicados al broker, de forma que éste los procese como corresponda.
 *
 *  Los Topics se identifican mediante una cadena de texto separada por tokens '/' que indican el nivel de profundidad
 *  del recurso al que se accede (ej: 'aaa/bbb/ccc/ddd' tiene 4 niveles de profundidad: aaa, bbb, ccc, ddd).
 *
 *  Además cada nivel se identifica mediante un identificador único. Así para el ejemplo anterior, el token 'aaa' tiene un
 *  identificador, 'bbb' tiene otro, y lo mismo para 'ccc' y 'ddd'.
 *
 *  Esta librería está limitada con unos parámetros por defecto, de forma que encajen en una gran variedad de aplicaciones
 *  sin necesidad de tocar esos parámetros. Sin embargo, podrían modificarse adaptándola a casos especiales. Las limitaciones
 *  son éstas:
 *
 *  Identificador de topic: Variable entera 64-bit (uint64_t)
 *
 *  Relación entre "Identificador de token" vs "Niveles de profundidad"
 *     TokenId_size vs  NivelesDeProfundidad   
 *      8bit_token  -> 8 niveles (máximo de 256 tokens)
 *      9bit_token  -> 7 niveles (máximo de 512 tokens)
 *      10bit_token -> 6 niveles (máximo de 1024 tokens)
 *      13bit_token -> 5 niveles (máximo de 8192 tokens)
 *      16bit_token -> 4 niveles (máximo de 65536 tokens)
 *
 *  La configuración se selecciona mediante el parámetro MQ_CONFIG_VALUE
 *
 *  Es posible publicar y suscribirse a topics dedicados relativos a un ámbito concreto que se sale de la norma de identificación
 *  de los topics. El wildcard es '@' de esta forma se genera un ámbito "scope" al que se dirige el mensaje. Dicho scope 
 *  está formado por un valor uint32_t.
 *
 *  Por ejemplo para dirigir mensajes concretos al grupo 3, el topic dedicado será de la forma "@group:3/..." O para mensajes
 *  a un dispositivo, por ejemplo el 8976: "@dev:8976/...". De igual forma, se podrán realizar suscripciones, por ejemplo a
 *  todos los topics con destino el grupo 78: "@group:78/#".
 *
 *  En caso de no utilizar el wildcard "scope", su valor será 0 y se considerará un topic general.
 *
 * 	Versión: 1.0.0-build-17-sep-2017
 */

#ifndef MQLIB_H_
#define MQLIB_H_


//------------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include "List.h"
#include "Heap.h"

//------------------------------------------------------------------------------------
/** Portabilidad a la API MBED 5.x */
#if __MBED__ == 1
#include "rtos.h"
#define MQ_MUTEX				Mutex
#define MQ_MUTEX_LOCK()    	 	_mutex.lock(osWaitForever)
#define MQ_MUTEX_UNLOCK()   	_mutex.unlock()

/** Portabilidad a cmsis_os utilizando un módulo Callback adaptado */
#else
#include "cmsis_os.h"
#include "FuncPtr.h"
#define MQ_MUTEX				osMutexId
#define MQ_MUTEX_LOCK()        	osMutexWait(_mutex, osWaitForever)
#define MQ_MUTEX_UNLOCK()      	osMutexRelease(_mutex)
MQ_MUTEX MQ_MUTEX_CREATE(void);

#endif


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

namespace MQ{	
    
    
/** @struct MQ::MAX_TOKEN_LEVEL
 *  @brief Tipo definido para definir la profundidad máxima de los topics
 */
static const uint8_t MAX_TOKEN_LEVEL = 8;
   
    
/** @struct MQ::token_t
 *  @brief Tipo definido para definir el valor de un token
 */
typedef uint8_t token_t;
    
    
/** @struct MQ::Token
 *  @brief Tipo definido para la definición de tokens
 */
typedef const char* Token;
    
    
/** @type MQ::SubscribeCallback
 *  @brief Tipo definido para las callbacs de suscripción
 */
#if __MBED__ == 1    
typedef Callback<void(const char* name, void*, uint16_t)> SubscribeCallback;
#else    
typedef FuncPtr3<void, const char* name, void*, uint16_t> SubscribeCallback;
#endif
	
/** @type MQ::PublishCallback
 *  @brief Tipo definido para las callbacs de publicación
 */
#if __MBED__ == 1    
typedef Callback<void(const char* name, int32_t)> PublishCallback;
#else
typedef FuncPtr2<void, const char* name, int32_t> PublishCallback;
#endif	
	
/** @enum ErrorResult
 *  @brief Resultados de error generados por la librería
 */
enum ErrorResult{
	SUCCESS = 0,          	///< Operación correcta
	NULL_POINTER,           ///< Fallo por Puntero nulo
	DEINIT,                 ///< Fallo por no-inicialización
	OUT_OF_MEMORY,          ///< Fallo por falta de memoria
	EXISTS,           		///< Fallo por objeto existente
	NOT_FOUND,        		///< Fallo por objeto no existente
    OUT_OF_BOUNDS,          ///< Fallo por exceso de tamaño
};
	
/** @enum HierarchyLevels
 *  @brief Niveles de jerarquía en los topics
 */
enum HierarchyLevels{
	NumLevels_4 = 4,      	///< 4 niveles (-> _token_bits = 16)
	NumLevels_5,          	///< 5 niveles (-> _token_bits = 13)
	NumLevels_6,           	///< 6 niveles (-> _token_bits = 10)
	NumLevels_7,           	///< 7 niveles (-> _token_bits = 9)
	NumLevels_8,           	///< 8 niveles (-> _token_bits = 8)
};
   
    
/** @struct MQ::topic_t
 *  @brief Tipo definido para definir el identificador de un topic
 */
__packed struct topic_t{
    uint8_t tk[MQ::MAX_TOKEN_LEVEL+1];
};


/** @struct Topic
 *  @brief Estructura asociada los topics, formada por un nombre y una lista de suscriptores
 */
struct Topic{
    MQ::topic_t id;              					/// Identificador del topic
    const char* name;                               /// Nombre del name asociado a este nivel
	List<MQ::SubscribeCallback > *subscriber_list; 	/// Lista de suscriptores
};


	
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------



class MQBroker {
public:	    

    /** @fn start
     *  @brief Inicializa el broker MQ estableciendo el número máximo de jerarquías en el
     *          nombre de los topics. 
     *  @param token_list Lista predefinida de tokens
	 *	@param token_count Máximo número de tokens en la lista
     *  @param max_len_of_name Número de caracteres máximo que puede tener un topic (incluyendo '\0' final)
     *  @return Código de error
     */
    static int32_t start(const char** token_list, uint32_t token_count, uint8_t max_len_of_name) {
        // ajusto parámetros por defecto 
        _max_name_len = max_len_of_name-1;
        _token_provider = token_list;
        // copio el número de tokens y reservo los wildcard (n.a.,+,#) con valores 0,1,2
        _token_provider_count = token_count + WildcardCOUNT;
        
        // si hay un número de tokens mayor que el tamaño que lo puede alojar, devuelve error:
        // ej: token_count = 500 con token_t = uint8_t, que sólo puede codificar hasta 256 valores.
        if((_token_provider_count >> (8*sizeof(MQ::token_t))) > 0){
            return OUT_OF_BOUNDS;
        }
        
        // si no hay lista inicial, se crea...
        if(!_topic_list){
            #if !defined(__MBED__)
            _mutex = MQ_MUTEX_CREATE();
            #endif
            MQ_MUTEX_LOCK();
            _topic_list = new List<MQ::Topic>();
			if(!_topic_list){
				MQ_MUTEX_UNLOCK();
				return OUT_OF_MEMORY;
			}
			if(token_count > 0){
				_topic_list->setLimit(token_count);
			}
            MQ_MUTEX_UNLOCK();
			return SUCCESS;
        }
		return EXISTS;
    }


    /** @fn ready
     *  @brief Chequea si el broker está listo para ser usado
	 *	@return True:listo, False:pendiente
     */
    static bool ready() {
		return ((_topic_list)? true : false);
	}

    
    /** @fn subscribeReq
     *  @brief Recibe una solicitud de suscripción a un topic
     *  @param name Nombre del topic
     *  @param subscriber Manejador de las actualizaciones del topic
     *  @return Resultado
     */
    static int32_t subscribeReq(const char* name, MQ::SubscribeCallback *subscriber){
        int32_t err;
        if(!_topic_list){
            return DEINIT;
        }
        // si el nombre excede el tamaño máximo, no lo permite
        if(strlen(name) > _max_name_len){
            return OUT_OF_BOUNDS;
        }
        // Inicia la búsqueda del topic para ver si ya existe
        MQ_MUTEX_LOCK();
        MQ::Topic * topic = findTopicByName(name);		
		// si lo encuentra...
        if(topic){
			// Chequea si el suscriptor ya existe...
			MQ::SubscribeCallback *sbc = topic->subscriber_list->searchItem(subscriber);
			// si no existe, lo añade
			if(!sbc){
				err = topic->subscriber_list->addItem(subscriber);
				MQ_MUTEX_UNLOCK();
				return err;
			}
			// si existe, devuelve el error
			MQ_MUTEX_UNLOCK();
            return (EXISTS);
        }
		
		// si el topic no existe, lo crea reservarvando espacio para el topic
        topic = (MQ::Topic*)Heap::memAlloc(sizeof(MQ::Topic));
        if(!topic){
            MQ_MUTEX_UNLOCK();
            return(OUT_OF_MEMORY);
        }
        
        // se fijan los parámetros del token (delimitadores, id, nombre)
        topic->name = name;
        createTopicId(&topic->id, name);
        
        // se crea la lista de suscriptores
        topic->subscriber_list = new List<MQ::SubscribeCallback>();
        if(!topic->subscriber_list){
            MQ_MUTEX_UNLOCK();
            return(OUT_OF_MEMORY);
        }
        
        // y se añade el suscriptor
        if(topic->subscriber_list->addItem(subscriber) != SUCCESS){
            MQ_MUTEX_UNLOCK();
            return(OUT_OF_MEMORY);
        }
        
        // se inserta en el árbol de topics
        err = _topic_list->addItem(topic);
        MQ_MUTEX_UNLOCK();
        return err;
    }

	
    /** @fn unsubscribeReq
     *  @brief Recibe una solicitud de cancelación de suscripción a un topic
     *  @param name Nombre del topic
     *  @param subscriber Suscriptor a eliminar de la lista de suscripción
     *  @return Resultado
     */
    static int32_t unsubscribeReq (const char* name, MQ::SubscribeCallback *subscriber){
		int32_t err;
		if(!_topic_list){
            return DEINIT;
        }
        // si el nombre excede el tamaño máximo, no lo permite
        if(strlen(name) > _max_name_len){
            return OUT_OF_BOUNDS;
        }
        
        MQ_MUTEX_LOCK();
        MQ::Topic * topic = findTopicByName(name);
        if(!topic){
            MQ_MUTEX_UNLOCK();
            return NOT_FOUND;
        }
		
        MQ::SubscribeCallback *sbc = topic->subscriber_list->searchItem(subscriber);
        if(!sbc){
            MQ_MUTEX_UNLOCK();
            return NOT_FOUND;
        }
		
        err = topic->subscriber_list->removeItem(sbc);
        MQ_MUTEX_UNLOCK();
		return err;
    }
	
	
    /** @fn publishReq
     *  @brief Recibe una solicitud de publicación a un topic
     *  @param name Nombre del topic
     *  @param data Mensaje
     *  @param datasize Tamaño del mensaje
     *  @param publisher Callback de notificación de la publicación
	 *	@return Resultado
     */
    static int32_t publishReq (const char* name, void *data, uint32_t datasize, MQ::PublishCallback *publisher){	
		if(!_topic_list){
            return DEINIT;
        }
        // si el nombre excede el tamaño máximo, no lo permite
        if(strlen(name) > _max_name_len){
            return OUT_OF_BOUNDS;
        }

		// obtiene el identificador del topic a publicar
        MQ::topic_t topic_id;
        createTopicId(&topic_id, name);
        
        // recorre la lista de topics buscando aquellos que coincidan, teniendo en cuenta el tamaño del token_id
        // dado por _token_bits
        MQ_MUTEX_LOCK();
        MQ::Topic* topic = _topic_list->getFirstItem();
        while(topic){
            // comprueba si el id coincide o si no se usa (=0)
            if(matchIds(&topic_id, &topic->id)){
                // si coinciden, se invoca a todos los suscriptores
                MQ::SubscribeCallback *sbc = topic->subscriber_list->getFirstItem();
                while(sbc){
                    sbc->call(name, data, datasize);
                    sbc = topic->subscriber_list->getNextItem();
                }                    
            }
            topic = _topic_list->getNextItem();
        }
        publisher->call(name, SUCCESS);
        MQ_MUTEX_UNLOCK();
		return SUCCESS;
    }

    
    /** @fn getTopicIdReq 
     *  @brief Obtiene el identificador del topic dado su nombre
     *  @param id Recibe el Identificador del topic or (0) si no existe
     *  @param name Nombre del topic
     */
    static void getTopicIdReq(MQ::topic_t* id, const char* name){        
        createTopicId(id, name);
    }    

    
    /** @fn getTopicNameReq 
     *  @brief Obtiene el nombre de un topic dado su id y su scope
     *  @param name Recibe el nombre asociado al id
     *  @param len Tamaño máximo aceptado para el nombre
     *  @param id Identificador del topic
     */
    static void getTopicNameReq(char* name, uint8_t len, MQ::topic_t* id){
        strcpy(name, "");
		if(!_token_provider || _token_provider_count == 0){
            return;
        }

        // recorre campo a campo verificando los tokens
        for(int i=0;i<MQ::MAX_TOKEN_LEVEL;i++){
            uint32_t idex;
            // si es un campo de tamaño normal token_t
            if(i != AddrField){
                // obtiene el token_id
                idex = id->tk[i];
                // si ha llegado al final del topic, termina
                if(idex == WildcardNotUsed){
                    // una vez finalizado, debe borrar el último caracter '/' insertado
                    name[strlen(name)-1] = 0;                        
                    return;
                }
                // si son wildcards, los escribe
                else if(idex == WildcardAny){
                    strcat(name, "+/");
                }
                else if(idex == WildcardAll){
                    strcat(name, "#/");
                }
                // en otro caso, escribe el token correspondiente
                else{
                    strcat(name, _token_provider[idex]);
                    strcat(name, "/");
                }
            }
            // si es el campo de tamaño ampliado
            else{
                // calcula el identificador extendido
                idex = (((uint32_t)id->tk[i]) << (8 * sizeof(MQ::token_t))) + id->tk[i+1];       
                // comprueba si el nombre contiene el wildcard de direccionamiento
                if(strchr(name, WildcardScope) != 0){
                    // en ese caso, añado el nombre numérico
                    char num[16];
                    sprintf(num, "%d/", idex);
                    strcat(name, num);
                }
                // si no contiene el wildcard de direccionamiento, comprueba que si está fuera de rango
                else if(idex >= (_token_provider_count - WildcardCOUNT)){
                    strcpy(name, "");
                    return;
                }
                // sino, devuelve el texto correspondiente
                else{
                    // si ha llegado al final del topic, termina
                    if(idex == WildcardNotUsed){
                        // una vez finalizado, debe borrar el último caracter '/' insertado
                        name[strlen(name)-1] = 0;                        
                        return;
                    }
                    // si son wildcards, los escribe
                    else if(idex == WildcardAny){
                        strcat(name, "+/");
                    }
                    else if(idex == WildcardAll){
                        strcat(name, "#/");
                    }
                    // en otro caso, escribe el token correspondiente
                    else{
                        strcat(name, _token_provider[idex]);
                        strcat(name, "/");
                    }                    
                }                                
                // descarto el campo siguiente
                i++;
            }
        }
        // una vez finalizado, debe borrar el último caracter '/' insertado
        name[strlen(name)-1] = 0;
    }        

    
    /** @fn getMaxTopicLenReq 
     *  @brief Obtiene el número máximo de caracteres que puede tener un topic
     *  @return Número de caracteres, incluyendo el '\0' final.
     */
    static uint8_t getMaxTopicLenReq(){
        return _max_name_len;
    }               

protected:
	
    /** Identificador de wildcards */
    enum Wildcards{
        WildcardNotUsed = 0,
        WildcardAny,
        WildcardAll,
        WildcardCOUNT,
    };    
    
    /** Posición del campo de tamaño especial uint16_t */
    static const uint8_t AddrField = 1;
    
    /** Wildcard asociado a un token de ámbito o direccionamiento seguido de un campo numérico */
    static const int WildcardScope = '@';
	
    /** Lista de topics registrados */
    static List<MQ::Topic> * _topic_list;

    /** Puntero a la lista de topics proporcionados */
    static const char** _token_provider;
    static uint32_t _token_provider_count;
    static uint8_t _token_bits;

	/** Mutex */
    static MQ_MUTEX _mutex;

    /** Límite de tamaño en nombres de topcis */
    static uint8_t _max_name_len;
 

    /** @fn findTopicByName 
     *  @brief Busca un topic por medio de su nombre, descendiendo por la jerarquía hasta
     *         llegar a un topic final.
     *  @param name nombre
     *  @return Pointer to the topic or NULL if not found
     */
    static MQ::Topic * findTopicByName(const char* name){
        MQ::Topic* topic = _topic_list->getFirstItem();
        while(topic){
            if(strcmp(name, topic->name)==0){
                return topic;
            }
            topic = _topic_list->getNextItem();
        }
        return NULL;
    }

 

    /** @fn createTopicId 
     *  @brief Crea el identificador del topic. Los wildcards se sustituyen por el valor 0
     *  @param id Recibe el Identificador 
     *  @param name Nombre completo del topic
     */
    static void createTopicId(MQ::topic_t* id, const char* name){
        uint8_t from = 0, to = 0;
        bool is_final = false;
        int pos = 0; 
        // Inicializo el contenido del identificador para marcar como no usado
        for(int i=0;i<MQ::MAX_TOKEN_LEVEL;i++){
            id->tk[i] = WildcardNotUsed;
        }
        
        // obtiene los delimitadores para buscar tokens
        getNextDelimiter(name, &from, &to, &is_final);
        while(from < to){
            uint32_t token = WildcardNotUsed;
            // chequea si es un campo extendido
            if(pos == AddrField){
                // chequea si es un wildcard
                if(strncmp(&name[from], "+", to-from)==0){
                    token = WildcardAny;
                }            
                else if(strncmp(&name[from], "#", to-from)==0){
                    token = WildcardAll;
                } 
                // en caso contrario...
                else{
                    bool match = false;
                    // comprueba si viene precedido del wildcard de direccionamiento
                    for(int j=0;j<to;j++){
                        if(name[j] == WildcardScope){
                            match = true;
                            break;
                        }
                    }
                    // si es un campo numérico...
                    if(match){
                        // aplica el número
                        char num[16];
                        strncpy(num, &name[from], to-from);
                        token = atoi(num);                        
                    }
                    // sino, busca el token
                    else{
                        for(int i=0;i<_token_provider_count;i++){                
                            // si encuentra el token... actualiza el id
                            if(strncmp(_token_provider[i], &name[from], to-from)==0){
                                token  = i + WildcardCOUNT;
                                break;
                            }
                        }                          
                    }
                }                    
                // actualiza el token id
                id->tk[pos] = (MQ::token_t)(token >> (8*sizeof(MQ::token_t)));
                id->tk[pos+1] = (MQ::token_t)token;
                // salto al siguiente token
                pos++;
            } 
            // si no es un campo extendido, busca el texto
            else{
                for(int i=0;i<_token_provider_count;i++){                
                    // si encuentra el token... actualiza el id
                    if(strncmp(_token_provider[i], &name[from], to-from)==0){
                        token  = i + WildcardCOUNT;
                        break;
                    }
                }                  
                id->tk[pos] = (MQ::token_t)(token);
            }
            // pasa al siguiente campo
            from = to+1;
            getNextDelimiter(name, &from, &to, &is_final);            
            pos ++;                        
        }
    }    
       
    
    /** @fn getNextDelimiter
     *  @brief Extrae delimitadores del nombre, token a token, desde la posición "from"
     *  @param name Nombre del topic a evaluar
     *  @param from Recibe el delimitador inicial y se usa como punto inicial de búsqueda
     *  @param to Recibe el delimitador final
     *  @param is_final Recibe el flag si es el subtopic final
     */
    static void getNextDelimiter(const char* name, uint8_t* from, uint8_t* to, bool* is_final){
        // se obtiene el tamaño total del nombre
        int len = strlen(name);
        // si el rango es incorrecto, no hace nada
        if(*from >= len){
            *from = len;
            *to = len;
            *is_final = true;
            return;
        }
        // se inicia la búsqueda, si parte de un espaciador, lo descarta
        if(name[*from] == '/'){
            *from++;
        }
        *to = *from;
        for(int i = *from; i <= len; i++){
            // si encuentra fin de trama, marca como final
            if(name[i] == 0){
                *to = i;
                break;
            }
            // si encuentra espaciador de tokens, marca como final
            else if(name[i] == '/'){
                *to = i;
                break;
            }
        }
        // si el punto final coincide con el tamaño de trama, marca como punto final
        if(*to >= len){
            *is_final = true;            
        }
    }      
       
    
    /** @fn matchIds
     *  @brief Compara dos identificadores. El de búsqueda con el encontrado. Si el encontrado
     *         contiene wildcards, habrá que tenerlos en cuenta.
     *  @param found_id Identificador encontrado
     *  @param search_id Identificador de búsqueda
     *  @return True si encajan, False si no encajan 
     */
    static bool matchIds(MQ::topic_t* found_id, MQ::topic_t* search_id){
        for(int i=0;i<MQ::MAX_TOKEN_LEVEL;i++){
            // si ha llegado al final de la cadena de búsqueda sin errores, es que coincide...
            if(search_id->tk[i] == WildcardNotUsed){
                return true;
            }
            // si ha encontrado un wildcard All, es que coincide
            if(found_id->tk[i] == WildcardAll){
                return true;
            }
            
            // si no coinciden ni hay wildcards '+' involucrados, no hay coincidencia
            if(found_id->tk[i] != WildcardAny && found_id->tk[i] != search_id->tk[i]){
                return false;
            }
            // en otro caso, es que coinciden y por lo tanto sigue analizando siguientes elementos
        }        
        // si llega a este punto es que coinciden todos los niveles
        return true;
    }         
};




//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------




class MQClient{
public:
    
	/** @fn subscribe
     *  @brief Se suscribe a un tipo de topic, realizando una petición al broker
     *  @param name Nombre del topic
     *  @param subscriber Manejador de las actualizaciones del topic
     *  @return Resultado
     */
    static int32_t subscribe(const char* name, MQ::SubscribeCallback *subscriber){
		return MQBroker::subscribeReq(name, subscriber);
    }

	
    /** @fn unsubscribe
     *  @brief Finaliza la suscripción a un topic, realizando una petición al broker
     *  @param name Nombre del topic
     *  @param subscriber Suscriptor a eliminar de la lista de suscripción
     *  @return Resultado
     */
    static int32_t unsubscribe (const char* name, MQ::SubscribeCallback *subscriber){
		return MQBroker::unsubscribeReq(name, subscriber);
    }
	
		
    /** @fn publish 
     *  @brief Publica una actualización de un topic, realizando una petición al broker
     *  @param name Nombre del topic
     *  @param data Mensaje
     *  @param datasize Tamaño del mensaje
     *  @param publisher Callback de notificación de la publicación
	 *	@return Resultado
     */
    static int32_t publish (const char* name, void *data, uint32_t datasize, MQ::PublishCallback *publisher){
        return MQBroker::publishReq(name, data, datasize, publisher);
    }  

    
    /** @fn getTopicName 
     *  @brief Obtiene el nombre de un topic dado su id
     *  @param name Recibe el nombre asociado al id
     *  @param len Tamaño máximo aceptado para el nombre
     *  @param id Identificador del topic
     */
    static void getTopicName(char*name, uint8_t len, MQ::topic_t* id){
        MQBroker::getTopicNameReq(name, len, id);
    }        

    
    /** @fn getTopicId 
     *  @brief Obtiene el id de un topic dado su nombre
     *  @return Recibe el Identificador del topic
     *  @param name Nombre del topic
     */
    static void getTopicId(MQ::topic_t* id, const char* name){
        MQBroker::getTopicIdReq(id, name);
    }   

    
    /** @fn getMaxTopicLen 
     *  @brief Obtiene el número máximo de caracteres que puede tener un topic
     *  @return Número de caracteres, incluyendo el '\0' final.
     */
    static uint8_t getMaxTopicLen(){
        return MQBroker::getMaxTopicLenReq();
    }        
};

} /* End of namespace MQ */

#endif /* MSGBROKER_H_ */
