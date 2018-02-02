/*
 * MQLib.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "MQLib.h"

/** Mutex para MQBroker */

// portabilidad a mbed-os
#if __MBED__ == 1 
MQ_MUTEX MQ::MQBroker::_mutex;

// portabilidad a esp-idf
#elif ESP_PLATFORM == 1
MQ_MUTEX MQ::MQBroker::_mutex;

// portabilidad a cmsis-rtos
#else 
osMutexDef (_mutdef);
MQ_MUTEX MQ_MUTEX_CREATE(void){
    return osMutexCreate(osMutex(_mutdef));
}
#endif


/** Lista de topics */
List<MQ::Topic> * MQ::MQBroker::_topic_list = 0;

/** Variables para el control de tokens y topics */
bool MQ::MQBroker::_tokenlist_internal = false;
const char** MQ::MQBroker::_token_provider = 0;
uint32_t MQ::MQBroker::_token_provider_count = 0;
uint8_t MQ::MQBroker::_token_bits = 0;
uint8_t MQ::MQBroker::_max_name_len = 0;
bool MQ::MQBroker::_defdbg = false;

