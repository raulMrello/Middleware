# MQLib

MQLib es una librería que proporciona un framework de publicación-suscripción, sin necesidad de correr en un
thread dedicado, es decir, siempre corre en el contexto del publicador.

Para ella cuenta con un "mutex" que garantiza el acceso secuencial al módulo de publicación y notificación de
actualizaciones a los suscriptores.

Para su funcionamiento cuenta con dos utilidades básicas:

- Módulo List: que permite crear listas doblemente enlazadas de items (donde cada item es un puntero a un objeto).
- Módulo Heap: que permite gestionar diferentes tipos de memoria dinámica (malloc, memoryPool, etc...)

- Versión 2.0.0
  
## Changelog

----------------------------------------------------------------------------------------------
##### 02.02.2018 ->commit:"Versión 2.0.0"
- [x] Realizo las siguientes mejoras y correcciones de bugs:
	- Permite no usar lista de tokens predefinida.
	- Añade trazas de depuración
	- Compatibilidad con MBED-OS y ESP-IDF
	- Corrige bug en <matchIds>
	- Corrige bugs de cálculo de fin de tabla
	- Corrige otros bugs menores.
      
----------------------------------------------------------------------------------------------
##### 17.10.2017 ->commit:"Incluyo identificador como topic_t y límite de niveles"
- [x] Incluyo límite de niveles con MAX_TOKEN_LEVEL y objetos MQ::topic_t para los identificadores.
- [ ] Falta implementar chequeo de verificación de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripción.

----------------------------------------------------------------------------------------------
##### 09.10.2017 ->commit:"Incluyo límite de caracteres en topics"
- [x] Incluyo límite de caracteres en topics.
- [ ] Falta implementar chequeo de verificación de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripción.

----------------------------------------------------------------------------------------------
##### 09.10.2017 ->commit:"Ids tipo uint64"
- [x] Modifico identificadores a uint64 con posiblidad de configuración de niveles de profundidad.
- [ ] Falta implementar chequeo de verificación de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripción.

----------------------------------------------------------------------------------------------
##### 25.09.2017 ->commit:"Incluyo chequeo de topics"
- [x] Añado función para chequear topic por nombre e id.
- [ ] Falta implementar chequeo de verificación de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripción.

----------------------------------------------------------------------------------------------
##### 22.09.2017 ->commit:"Añado gestión de topics por nombre"
- [x] Añado funciones para gestión de topics por nombre.
- [ ] Falta implementar chequeo de verificación de nombre con wildcards especiales como # y +
      a la hora de obtener el topic_id durante el proceso de suscripción.

----------------------------------------------------------------------------------------------
##### 21.09.2017 ->commit:"Modifico callbacks para que utilicen topics uin32_t"
- [x] Cambio callbacks con parámetros uint16_t a a uint32_t
- [ ] 
----------------------------------------------------------------------------------------------
##### 19.09.2017 ->commit:"Incluyo módulo FuncPtr para CMSIS_OS"
- [x] Incluyo módulo FuncPtr para las callbacks
- [ ] 
----------------------------------------------------------------------------------------------
##### 18.09.2017 ->commit:"Librería compatible con la API de MBED 5.x y con CMSIS_OS RTOSv1"
- [x] Cambio código válido para cmsis o mbed5.
- [ ] Incluir parámetros de configuración (thread, mutex, etc) en función del entorno de desarrollo utilizado:
		> Por ejemplo para mbed-os < 5x, cmsis-os, cmsis-os2,...

----------------------------------------------------------------------------------------------
##### 18.09.2017 ->commit:"Compatibilizo con cmsis y mbed5"
- [x] Cambio código válido para cmsis o mbed2.
- [ ] Incluir parámetros de configuración (thread, mutex, etc) en función del entorno de desarrollo utilizado:
		> Por ejemplo para mbed-os < 5x, cmsis-os, cmsis-os2,...

----------------------------------------------------------------------------------------------
##### 17.09.2017 ->commit:"Versión 1.0.0-build-17-sep-2017"
- [x] Funciones básicas creadas para su funcionamiento con mbed-os 5.x
- [ ] Incluir parámetros de configuración (thread, mutex, etc) en función del entorno de desarrollo utilizado:
		> Por ejemplo para mbed-os < 5x, cmsis-os, cmsis-os2,...

