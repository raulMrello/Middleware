/*
 * MQSerialBridge.cpp
 *
 *  Created on: Nov 2017
 *      Author: raulMrello
 */


#include "MQSerialBridge.h"


//------------------------------------------------------------------------------------
//- STATIC ---------------------------------------------------------------------------
//------------------------------------------------------------------------------------

static uint16_t getCRC(void* data, uint32_t size){
    uint16_t crc = 0;
    uint8_t* udata = (uint8_t*)data;
    for(int i=0;i<size;i++){
        if((i&1 == 0)){
            crc ^= (((uint16_t)udata[i]) & 0x00ff);
        }
        else{
            crc ^= ((((uint16_t)udata[i]) << 8) & 0xff00);
        }
    }
    return crc;
}    
    
//------------------------------------------------------------------------------------
//-- PUBLIC METHODS IMPLEMENTATION ---------------------------------------------------
//------------------------------------------------------------------------------------


MQSerialBridge::MQSerialBridge(PinName tx, PinName rx, uint32_t baud, uint16_t recv_buf_size, const char* cfg_topic, const char* token) 
                    : SerialTerminal(tx, rx, recv_buf_size, baud, SerialTerminal::ReceiveAfterBreakTime) {    
    _rbufsize = recv_buf_size;
    _token = (char*)token;
    _cfg_topic = (char*)cfg_topic;
    
    if(_rbufsize){
        _tbuf = (char*)Heap::memAlloc(_rbufsize);
        _rbuf = (char*)Heap::memAlloc(_rbufsize);
        if(_tbuf && _rbuf){
            // prepara buffers
            memset(_tbuf, 0, _rbufsize);
            memset(_rbuf, 0, _rbufsize);
            
            // Carga callbacks estáticas de publicación/suscripción
            _subscriptionCb = callback(this, &MQSerialBridge::subscriptionCb);
            _publicationCb = callback(this, &MQSerialBridge::publicationCb);   
            
            // Prepara terminal serie con un breaktime de 4 * tbyte y un mínimo de 500us
            uint32_t break_time = 4 * (10000000/baud);
            break_time = (break_time < 500)? 500 : break_time;
            SerialTerminal::config(callback(this, &MQSerialBridge::onRxComplete), callback(this, &MQSerialBridge::onRxTimeout), callback(this, &MQSerialBridge::onRxOvf), break_time, 0);

            // Inicializa parámetros del hilo de ejecución propio
            _th.start(callback(this, &MQSerialBridge::task));
        }
    }
}
 

    
//------------------------------------------------------------------------------------
//-- PROTECTED METHODS IMPLEMENTATION ------------------------------------------------
//------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
void MQSerialBridge::onRxComplete(){
    _th.signal_set(ReceivedData);
}

//---------------------------------------------------------------------------------
void MQSerialBridge::onRxTimeout(){
    _th.signal_set(TimeoutOnRecv);
}

//---------------------------------------------------------------------------------
void MQSerialBridge::onRxOvf(){
    _th.signal_set(OverflowOnRecv);
}

//---------------------------------------------------------------------------------
void MQSerialBridge::onTxComplete(){
}


//---------------------------------------------------------------------------------
void MQSerialBridge::task(){    
    
    // se suscribe a todos los topics de configuración
    char* all_cfg_topics = (char*)Heap::memAlloc(MQ::MQClient::getMaxTopicLen());
    if(all_cfg_topics){
        sprintf(all_cfg_topics, "%s/#", _cfg_topic);
        MQ::MQClient::subscribe(all_cfg_topics, &_subscriptionCb);
        Heap::memFree(all_cfg_topics);
    }
    
    // Inicializo el terminal de recepción    
    SerialTerminal::startReceiver();
        
    for(;;){        
        osEvent evt = _th.signal_wait(0, _timeout);
        if(evt.status == osEventSignal){   
            uint32_t sig = evt.value.signals;
            if((sig & (TimeoutOnRecv | OverflowOnRecv))!=0){
                // descarto la trama recibida
                SerialTerminal::recv(0, 0);
            }
            if((sig & ReceivedData)!=0){
                // se lee la trama recibida
                uint16_t bufsize = SerialTerminal::recv(_rbuf, _rbufsize);
                
                // se extraen los tokens en modo texto para obtener el cuerpo del mensaje
                int32_t data_size = bufsize - (strlen(_rbuf) + 1);
                uint8_t* data = (uint8_t*)(_rbuf + strlen(_rbuf) + 1);
                char* topic = strtok(_rbuf, (const char*)_token);
                char* msg_size = strtok(0, (const char*)_token);
                char* msg_crc = strtok(0, (const char*)_token);
                
                // en primer lugar se comprueba si hay datos coherentes
                if(topic && msg_size && msg_crc && data_size >= 0){
                    // en segundo lugar se comprueba si el tamaño de los datos coincide
                    if(atoi(msg_size) == data_size){
                        // a continuación se verifica si el crc de los datos es correcto
                        if(atoi(msg_crc) == getCRC(data, data_size)){
                            // por último se publica el mensaje
                            MQ::MQClient::publish(topic, data, data_size, &_publicationCb);
                        }
                    }
                }
            }
        }
    }
}


//------------------------------------------------------------------------------------
void MQSerialBridge::subscriptionCb(const char* topic, void* msg, uint16_t msg_len){
    // en primer lugar chequea qué tipo de mensaje es
    // si no es de configuración, entonces lo envía al enlace serie
    if(strncmp(topic, _cfg_topic, strlen(_cfg_topic)) != 0){
        uint32_t msg_crc = getCRC(msg, msg_len);
        while(SerialTerminal::busy()){
            Thread::yield();
        }
        sprintf(_tbuf, "%s\n%d\n%d\n", topic, msg_len, msg_crc);
        char* data = (char*)(_tbuf + strlen(_tbuf) + 1);
        memcpy(data, msg, msg_len);
        SerialTerminal::send(_tbuf, msg_len + strlen(_tbuf) + 1, _cb_tx);
        return;
    }
    
    // si es una publicación en el topic de configuración propio, entonces la decodifica.
    // si es una suscripción nueva
    if(strstr(topic, "/suscr") != 0){
        char* susc_topic = strtok((char*)msg, (const char*)_token);
        char* susc_msg_size = strtok(0, (const char*)_token);
        if(susc_topic && susc_msg_size){
            MQ::MQClient::subscribe(susc_topic, &_subscriptionCb);
        }        
    }    
}


//------------------------------------------------------------------------------------
void MQSerialBridge::publicationCb(const char* topic, int32_t result){
    // Hacer algo si es necesario...
}  


