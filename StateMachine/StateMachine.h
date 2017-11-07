/*
    Copyright (c) 2016 raulMrello
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
 
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    
    @file          StateMachine.h 
    @purpose       Módulo de gestión de una máquina de estados jerárquica
    @version       1
    @date          Jul 2017
    @author        raulMrello
*/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H


#include "mbed.h"
  
//---------------------------------------------------------------------------------
//- class State -------------------------------------------------------------------
//---------------------------------------------------------------------------------


class State{
public:

    /** Lista de resultados que puede devolver un manejador de estado */
    typedef enum{
        HANDLED,
        IGNORED,
        TRANSITION
    }StateResult;

    /** Lista de eventos básicos en un estado */
    typedef enum{
        EV_ENTRY,
        EV_EXIT,
        EV_TIMED,
        EV_MESSAGE,
        EV_RESERVED_USER
    }Event_type;
    
    /** Estructura de eventos soportado por los estados */
    typedef struct {
        Event_type evt;
        osEvent*   oe;
    }StateEvent;
  
    /** definición de un manejador de eventos como un puntero a función */
    typedef Callback<State::StateResult(State::StateEvent*)> EventHandler;

    /** Constructor
     *  Asigna un número de manejadores de eventos máximo (por defecto 3) sin contar
     *  los manejadores de eventos Entry, Exit, Timed.
     *  @param num_hnd Número máximo de manejadores de eventos
     */
    State(){
        _handler = callback(this, &State::defaultHandler);
    }
    
    /** setHandler()
     *  Inserta un nuevo manejador de eventos
     *  @param evhnd Manejador a instalar
     */
	void setHandler(EventHandler evhnd){
        _handler = evhnd;
    }
    
    /** getHandler()
     *  Obtiene el manejador del evento especificado
     *  @return manejador instalado, NULL si no instalado
     */
	EventHandler* getHandler(){
        return &_handler;    
    }      
            
protected:
    EventHandler _handler;
    State::StateResult defaultHandler(State::StateEvent*){ return IGNORED; }
};





//---------------------------------------------------------------------------------
//- class StateMachine ------------------------------------------------------------
//---------------------------------------------------------------------------------

#define NullState   (State*)0

class StateMachine {

public:
	  
    /** StateMachine()
    *  Constructor por defecto
    *  @param init Estado inicial
    *  @param parent Estado padre
    */
    StateMachine(){
        _curr = NullState;
        _next = NullState;
        _parent = NullState;
    }
    
    /** run()
     *  Ejecuta la máquina de estados
     */
    void run(osEvent* oe){
        State::StateEvent se;
        se.oe = oe;
        if(oe->status == osEventTimeout){
            se.evt = State::EV_TIMED;
        }
        else if(oe->status == osEventMail){
            se.evt = State::EV_MESSAGE;
        }
        else if(oe->status == osEventSignal){   
            se.evt = (State::Event_type)0;
            uint32_t mask = 1;
            uint32_t sig = oe->value.signals;
            do{
                bool skip_parent = false;
                if((sig & mask) != 0){
                    State::EventHandler* hnd;
                    if(_curr != NullState){
                        hnd = _curr->getHandler();
                        if(hnd->call(&se) != State::IGNORED){
                            skip_parent = true;
                        }
                    }
                    if(!skip_parent && _parent != NullState){
                        hnd = _parent->getHandler();
                        hnd->call(&se);                        
                    }                                    
                }
                // borro el flag procesado
                sig &= ~(mask);
                // paso al siguiente evento
                mask = (mask << 1);
                se.evt = (State::Event_type)(se.evt+1);
            }while (sig != 0); 
        }         
    }    
     
    /** initState()
     *  Inicia la máquina de estados a un estado por defecto
     *  @param st Estado al que conmutar
     *  @parm tid Contexto thread sobre el que notificar
     *
     */
    void initState(State* st, osThreadId tid = 0){
        if(!tid){
            tid = osThreadGetId();
        }
        _curr = st;
        _next = NullState;
        raiseEvent(State::EV_ENTRY, tid);        
    }
     
    /** tranState()
     *  Cambia de estado (transición). En caso de ser el primero, fuerza su inicio
     *  @param st Estado al que conmutar
     *  @parm tid Contexto thread sobre el que notificar
     *
     */
    void tranState(State* st, osThreadId tid = 0){
        if(!tid){
            tid = osThreadGetId();
        }
        if(_curr == NullState){
            _curr = st;
            _next = NullState;
            raiseEvent(State::EV_ENTRY, tid);
        }
        else{
            _next = st;
            raiseEvent(State::EV_EXIT, tid);            
        }
    }
     
    /** nextState()
     *  Cambia al siguiente estado
     *  @param True cambio efectuado
     */
    bool nextState(){
        if(_next == NullState){
            return false;
        }
        _curr = NullState;
        tranState(_next); 
        return true;
    }
     
    /** setParent()
     *  Cambia de estado padre
     *  @param st Estado padre
     */
    void setParent(State* st){
        _parent = st;
    }
     
    /** raiseEvent()
     *  Lanza Evento
     *  @param evt Evento
     *  @parm tid Contexto thread sobre el que notificar
     */
       void raiseEvent(uint32_t evt, osThreadId tid = 0){
        if(!tid){
            tid = osThreadGetId();
        }
        osSignalSet(tid, (1 << evt));
    } 
protected:  
      
  State* _curr;
  State* _next;
  State* _parent;
        
};



#endif
