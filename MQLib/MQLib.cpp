/*
 * MQLib.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "MQLib.h"

/** Mutex para MQBroker */
#if (!defined(__MBED__) || (__MBED__ != 1))
osMutexDef (_mutdef);
MQ_MUTEX MQ_MUTEX_CREATE(void){
    return osMutexCreate(osMutex(_mutdef));
}
#endif
MQ_MUTEX MQ::MQBroker::_mutex;

/** Lista de topics */
List<MQ::Topic> * MQ::MQBroker::_topic_list = 0;

/** Registro del elemento proporcionador de los topics */
const char** MQ::MQBroker::_token_provider = 0;
uint32_t MQ::MQBroker::_token_provider_count = 0;
uint8_t MQ::MQBroker::_token_bits = 0;
uint8_t MQ::MQBroker::_max_name_len = 0;

