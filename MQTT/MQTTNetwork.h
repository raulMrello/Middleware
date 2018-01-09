#ifndef _MQTTNETWORK_H_
#define _MQTTNETWORK_H_

#include "NetworkInterface.h"

class MQTTNetwork {
public:
    MQTTNetwork(NetworkInterface* aNetwork) : network(aNetwork) {
        socket = new TCPSocket();
    }

    ~MQTTNetwork() {
        delete socket;
    }

    int read(unsigned char* buffer, int len, int timeout) {
        int rc = socket->recv(buffer, len);
        if(rc == NSAPI_ERROR_WOULD_BLOCK){
            rc = 0;
        }
        return rc;
    }

    int write(unsigned char* buffer, int len, int timeout) {
        return socket->send(buffer, len);
    }

    int connect(const char* hostname, int port) {
        socket->open(network);
        return socket->connect(hostname, port);
    }

    int disconnect() {
        return socket->close();
    }
    
    /** Set blocking or non-blocking mode of the socket
     *
     *  Initially all sockets are in blocking mode. In non-blocking mode
     *  blocking operations such as send/recv/accept return
     *  NSAPI_ERROR_WOULD_BLOCK if they can not continue.
     *
     *  set_blocking(false) is equivalent to set_timeout(-1)
     *  set_blocking(true) is equivalent to set_timeout(0)
     *
     *  @param blocking true for blocking mode, false for non-blocking mode.
     */
    void set_blocking(bool blocking){ socket->set_blocking(blocking); }
    
private:
    NetworkInterface* network;
    TCPSocket* socket;
};

#endif

