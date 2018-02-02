# MQLib

MQLib es una librer�a que proporciona un framework de publicaci�n-suscripci�n, sin necesidad de correr en un
thread dedicado, es decir, siempre corre en el contexto del publicador.

Para ella cuenta con un "mutex" que garantiza el acceso secuencial al m�dulo de publicaci�n y notificaci�n de
actualizaciones a los suscriptores.

Para su funcionamiento cuenta con dos utilidades b�sicas:

- M�dulo List: que permite crear listas doblemente enlazadas de items (donde cada item es un puntero a un objeto).
- M�dulo Heap: que permite gestionar diferentes tipos de memoria din�mica (malloc, memoryPool, etc...)

- Versi�n 2.0.0
  
## Changelog

----------------------------------------------------------------------------------------------
##### 02.02.2018 ->commit:"Versi�n 2.0.0"
- [x] Realizo las siguientes mejoras y correcciones de bugs:
	- Permite no usar lista de tokens predefinida.
	- A�ade trazas de depuraci�n
	- Compatibilidad con MBED-OS y ESP-IDF
	- Corrige bug en <matchIds>
	- Corrige bugs de c�lculo de fin de tabla
	- Corrige otros bugs menores.
      
----------------------------------------------------------------------------------------------
##### 17.10.2017 ->commit:"Incluyo identificador como topic_t y l�mite de niveles"
- [x] Incluyo l�mite de niveles con MAX_TOKEN_LEVEL y objetos MQ::topic_t para los identificadores.
- [ ] Falta implementar chequeo de verificaci�n de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripci�n.

----------------------------------------------------------------------------------------------
##### 09.10.2017 ->commit:"Incluyo l�mite de caracteres en topics"
- [x] Incluyo l�mite de caracteres en topics.
- [ ] Falta implementar chequeo de verificaci�n de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripci�n.

----------------------------------------------------------------------------------------------
##### 09.10.2017 ->commit:"Ids tipo uint64"
- [x] Modifico identificadores a uint64 con posiblidad de configuraci�n de niveles de profundidad.
- [ ] Falta implementar chequeo de verificaci�n de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripci�n.

----------------------------------------------------------------------------------------------
##### 25.09.2017 ->commit:"Incluyo chequeo de topics"
- [x] A�ado funci�n para chequear topic por nombre e id.
- [ ] Falta implementar chequeo de verificaci�n de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripci�n.

----------------------------------------------------------------------------------------------
##### 22.09.2017 ->commit:"A�ado gesti�n de topics por nombre"
- [x] A�ado funciones para gesti�n de topics por nombre.
- [ ] Falta implementar chequeo de verificaci�n de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripci�n.

----------------------------------------------------------------------------------------------
##### 21.09.2017 ->commit:"Modifico callbacks para que utilicen topics uin32_t"
- [x] Cambio callbacks con par�metros uint16_t a a uint32_t
- [ ] 
----------------------------------------------------------------------------------------------
##### 19.09.2017 ->commit:"Incluyo m�dulo FuncPtr para CMSIS_OS"
- [x] Incluyo m�dulo FuncPtr para las callbacks
- [ ] 
----------------------------------------------------------------------------------------------
##### 18.09.2017 ->commit:"Librer�a compatible con la API de MBED 5.x y con CMSIS_OS RTOSv1"
- [x] Cambio c�digo v�lido para cmsis o mbed5.
- [ ] Incluir par�metros de configuraci�n (thread, mutex, etc) en funci�n del entorno de desarrollo utilizado:
		> Por ejemplo para mbed-os < 5x, cmsis-os, cmsis-os2,...

----------------------------------------------------------------------------------------------
##### 18.09.2017 ->commit:"Compatibilizo con cmsis y mbed5"
- [x] Cambio c�digo v�lido para cmsis o mbed2.
- [ ] Incluir par�metros de configuraci�n (thread, mutex, etc) en funci�n del entorno de desarrollo utilizado:
		> Por ejemplo para mbed-os < 5x, cmsis-os, cmsis-os2,...

----------------------------------------------------------------------------------------------
##### 17.09.2017 ->commit:"Versi�n 1.0.0-build-17-sep-2017"
- [x] Funciones b�sicas creadas para su funcionamiento con mbed-os 5.x
- [ ] Incluir par�metros de configuraci�n (thread, mutex, etc) en funci�n del entorno de desarrollo utilizado:
		> Por ejemplo para mbed-os < 5x, cmsis-os, cmsis-os2,...

