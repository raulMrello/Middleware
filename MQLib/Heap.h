/*
 * MsgBroker.h
 *
 *  Created on: 20/4/2015
 *      Author: raulMrello
 *
 *  MsgBroker library provides an extremely simple mechanism to add publish/subscription semantics to any C/C++ project.
 *  Publish/subscribe mechanisms is formed by:
 *  - Topics: messages exchanged between software agents: publishers and subscribers.
 *  - Publishers: agents which update data topics and notify those topic updates.
 *  - Subscribers: agents which attach to topic updates and act according their new values each time the get updated.
 */

#ifndef HEAP_H
#define HEAP_H
#include <stddef.h>
#include <stdlib.h>


class Heap{
public:
	static void* memAlloc(size_t size){
        void *ptr = malloc(size);
        if(!ptr){
            volatile int i = 0;
            while(i==0){
            }
        }
        return ptr;
    }
    static void memFree(void* ptr){
        free(ptr);
    }
};



#endif 
