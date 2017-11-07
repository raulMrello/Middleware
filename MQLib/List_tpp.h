/*
 * List.tpp
 *
 *  Created on: Apr 2015
 *  Author:     raulMrello
 *
 *  Implementación de la librería List.
 */

/** Archivo de cabecera para abstraer las reservas de memoria del Heap */
#include "Heap.h"



//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

template<typename T>
List<T>::List(){
    _count = 0;
    _first = 0;
    _last = 0;
    _search = 0;
    _numItems = (uint32_t)-1;
}

//------------------------------------------------------------------------------------
template<typename T>
List<T>::~List(){
    // elimina todos
    while(_first){
        removeListItem(_last);
    }
}

//------------------------------------------------------------------------------------
template<typename T>
int32_t List<T>::setLimit(uint32_t numItems){
    if(!numItems){
        return (NULL_POINTER);
    }
    _numItems = numItems;
	return SUCCESS;
}

//------------------------------------------------------------------------------------
template<typename T>
int32_t List<T>::addItem(T* item){
    if(!item){
        return(NULL_POINTER);
    }
    if(_count >= _numItems){
        return(LIMIT_EXCEEDED);
    }
    ListItem* listitem = (ListItem*)Heap::memAlloc(sizeof(ListItem));
    if(!listitem){
        return(OUT_OF_MEMORY);
    }
    listitem->item = item;
    // si es el primero, inicializa los punteros
    if(!_count){
        _first = listitem;
        _last = listitem;
        listitem->prev = 0;
        listitem->next = 0;
    }
    // sino, lo añade al final
    else{
        ListItem* last = _last;
        last->next = listitem;
        listitem->prev = last;
        listitem->next = 0;
        _last = listitem;
    }
    _count++;
	return SUCCESS;
}

//------------------------------------------------------------------------------------
template<typename T>
int32_t List<T>::insertItem(T* item){
    if(!item){
        return(NULL_POINTER);
    }
    if(_count >= _numItems){
        return(LIMIT_EXCEEDED);
    }
    ListItem* listitem = (ListItem*)Heap::memAlloc(sizeof(ListItem));
    if(!listitem){
        return(OUT_OF_MEMORY);
    }
    listitem->item = item;
    // si es el primero, inicializa punteros
    if(!_search){
        _first = listitem;
        _last = listitem;
        _search = listitem;
        listitem->prev = 0;
        listitem->next = 0;
    }
    else if(_search == _first){
        listitem->prev = 0;
        listitem->next = _search;
        _search->prev = listitem;
        _search = listitem;
        _first = listitem;
    }
    else if(_search == _last){
        listitem->prev = _search->prev;
        listitem->next = _search;
        _search->prev->next = listitem;
        _search->prev = listitem;
        _search = listitem;
    }
    
    else{
        listitem->prev = _search->prev;
        listitem->next = _search;
        _search->prev->next = listitem;
        _search->prev = listitem;
        _search = listitem;
    }
    _count++;
	return SUCCESS;
}

//------------------------------------------------------------------------------------
template<typename T>
int32_t List<T>::removeItem(T* item){
    if(!item){
        return(NULL_POINTER);
    }
    ListItem* listitem = _first;
    do{
        if(listitem->item == item){
            //apunta al siguiente
            getNextItem();
            removeListItem(listitem);
            return SUCCESS;
        }
        listitem = listitem->next;
    }while(listitem);
    return(ITEM_NOT_FOUND);
}


//------------------------------------------------------------------------------------
template<typename T>
void List<T>::removeAll(){
    // remove all items
    while(_first){
        removeListItem(_last);
    }
}


//------------------------------------------------------------------------------------
template<typename T>
uint32_t List<T>::getItemCount(){
    return _count;
}

//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getFirstItem(){
    _search = _first;
    if(!_search){
        return (T*)0;
    }
    return _search->item;

}

//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getLastItem(){
    _search = _last;
    if(!_search){
        return (T*)0;
    }
    return _search->item;

}
//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getNextItem(){
    if(!_search){
        return (T*)0;
    }
    _search = _search->next;
    if(!_search){
        return (T*)0;
    }
    return _search->item;
}
//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getPrevItem(){
    if(!_search){
        return (T*)0;
    }
    _search = _search->prev;
    if(!_search){
        return (T*)0;
    }
    return _search->item;
}

//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getCurrentItem(){
    if(!_search){
        return (T*)0;
    }
    return _search->item;
}

//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::searchItem(T* item){
    if(!item){
        return 0;
    }
    T* i = getFirstItem();
    if(!i){
        return 0;
    }
    for(;;){
        if(i == item){
            return i;
        }
        i = getNextItem();
        if(!i){
            break;
        }
    }
    return 0;
}

//------------------------------------------------------------------
template<typename T>
int32_t List<T>::removeListItem(ListItem *listitem){
    // si es el primero, apunta al siguiente
    if(!listitem->prev){
        // si el siguiente no existe, es que está vacía.
        if(!listitem->next){
            _first = 0;
            _last = 0;
            _count = 0;
            _search = 0;
            Heap::memFree(listitem);
            return SUCCESS;
        }
        _first = listitem->next;
        listitem->next->prev = 0;
        _count--;
         Heap::memFree(listitem);
         return SUCCESS;
    }
    // si no es el primero, lo suprime
    listitem->prev->next = listitem->next;
    if(listitem->next){
        listitem->next->prev = listitem->prev;
	}
    else{
        _last = listitem->prev;
	}
    _count--;
    Heap::memFree(listitem); 
	return SUCCESS;
}
