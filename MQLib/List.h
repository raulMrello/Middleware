/*
 * List.h
 *
 *  Created on: Apr 2015
 *  Author:     raulMrello
 *
 * 	List es una librería que permite manejar listas de objetos doblemente enlazadas. La lista puede ser limitada en 
 *	tamaño mediante el método "setLimit" de forma que no crezca de forma descontrolada.
 *	Los objetos son punteros a cualquier tipo de datos, ya que la lista se define como un template.
 */

#ifndef __LIST_H
#define __LIST_H

#include <stdint.h>



template<typename T>
class List {
public:
	
	/** Errores generados por la librería */
    enum Exception {
		SUCCESS = 0,          	///< No exceptions raised
		NULL_POINTER,         	///< Null pointer in operation
		LIMIT_EXCEEDED,         ///< Max number of items reached
		OUT_OF_MEMORY,          ///< No more dynamic memory
		ITEM_NOT_FOUND,			///< Item not found in list
	};        

	
    /** @fn List
     *  @brief Constructor que crea una lista vacía
     */
    List();

	
    /** @fn ~List
     *  @brief Destructor por defecto
     */
    ~List();

	
    /** @fn setLimit
     *  @brief Fija un nº máximo de items en la lista
     *  @param numItems Máximo nº de items permitidos
     *  @result Resultado 
     */
    int32_t setLimit(uint32_t numItems);

    /** @fn addItem
     *  @brief Añade un objeto a la lista
     *  @param item Objeto a añadir
     *  @return Resultado
     */
    int32_t addItem(T* item);

    /** @fn insertItem
     *  @brief Inserta un objeto en la lista
     *  @param item Objeto a añadir
     *  @return Resultado
     */
    int32_t insertItem(T* item);

    /** @fn removeItem
     *  @brief Elimina un objeto de la lista
     *  @param item Objeto a eliminar
     *  @param Resultado
     */
    int32_t removeItem(T* item);


    /** @fn removeAll
     *  @brief Borra todas las entradas de la lista
     */
    void removeAll();

    /** @fn getItemCount
     *  @brie Obtiene el nº de objetos en la lista
     *  @return Nº de objetos
     */
    uint32_t getItemCount();

    /** @fn getFirstItem
     *  @brief Obtiene el primer objeto de la lista
     *  @return Puntero al primer objeto, o NULL si está vacía
     */
    T* getFirstItem();

    /** @fn getLastItem
     *  @brief Obtiene el último objeto de la lista
     *  @return Puntero al último objeto, o NULL si está vacía
     */
    T* getLastItem();

    /** @fn getNextItem
     *  @brief Obtiene el siguiente objeto de la lista
     *  @return Puntero al siguiente objeto, o NULL si está vacía
     */
    T* getNextItem();

    /** @fn getPrevItem
     *  @brief Obtiene el anterior objeto de la lista
     *  @return Puntero al anterior objeto, o NULL si no hay más
     */
    T* getPrevItem();

    /** @fn getCurrentItem
     *  @brief Obtiene el objeto actualmente apuntado por el índice de búsqueda _search
     *  @return Puntero al objeto actual
     */
    T* getCurrentItem();

    /** @fn searchItem
     *  @brief Busca un objeto en la lista
     *  @param item Objeto a buscar
     *  @return Puntero al objeto encontrado, o NULL si no lo encuentra
     */
    T* searchItem(T* item);

private:
	
    /** Estructura de los items a insertar en la lista */
    struct ListItem{
        struct ListItem *prev;      ///< Puntero al anterior
        struct ListItem *next;      ///< Puntero al siguiente
        T * item;                   ///< Objeto insertado
    };

    ListItem *_first;       ///< Puntero al primer elemento
    ListItem *_last;        ///< Puntero al último elemento
    ListItem *_search;      ///< Puntero de búsqueda
    uint32_t  _count;       ///< Número de objetos insertados
    uint32_t  _numItems;    ///< Máximo número de objetos permitidos


    /** @fn removeListItem
     *  @brief Elimina una entrada de la lista
     *  @param listitem Registro a borrar
	 *	@return Resultado
     */
    int32_t removeListItem(ListItem * listitem);

};

#include "List_tpp.h"

#endif
