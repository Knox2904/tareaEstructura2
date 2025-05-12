# Gestión de Canciones y Listas de Reproducción - Tarea Estructura 2

## Descripción

Este programa implementa un sistema para gestionar canciones y listas de reproducción. Permite a los usuarios cargar canciones desde un archivo CSV, organizarlas por género, artista, tempo e ID, y crear listas de reproducción personalizadas. Además, incluye funcionalidades para buscar canciones y mostrarlas según diferentes criterios.


✅ principales funciones del programa:
leer determinado n° de canciones, 114000 max, para luego guardar los datos en listas que estan en mapas
Mostrar las canciones ya sea por genero, por artista y por tempo.
crear listas de reproduccion.
Mostrar las listas de reproduccion creadas por el usuario.


---

## Cómo Compilar y Ejecutar

### Requisitos Previos
- Tener instalado un compilador de C, como GCC.
- Un editor de texto o IDE para trabajar con el código fuente.

### Pasos para Compilar

1. **Clonar el Repositorio**:
2) Para compilar y ejecutar el programa, sigue estos pasos:

    1) Asegúrate de tener un compilador de C instalado en tu sistema (como GCC).
    2) Descarga los archivos del proyecto y colócalos en un mismo directorio.
    3) Compila el programa utilizando el siguiente comando en terminal git bash:
        ejm : cd "/c/Users/fcast/Desktop/tarea 2/tareaEstructura2/TDAs"
        para hacer eso hay que ver en que carpeta se coloco el programa, entrar en la carpeta y entrar en TDAs, y luego copiar la direccion y hacer los cambios respectivos a la direccion, ya que originalmente es asi : 
     C:\Users\fcast\Desktop\tarea 2 2.0\tareaEstructura2\TDAs y deberias ser asi : /c/Users/fcast/Desktop/tarea 2tareaEstructura2TDAs.
    4) Ejecuta el programa con: ./tarea2.sh


📂 Estructura del Proyecto
El proyecto incluye los siguientes archivos principales:

tarea2.c: Archivo principal que contiene la lógica del programa y el menú interactivo.
list.c: Implementación de una lista enlazada genérica para gestionar las canciones.
list.h: Cabecera que define las funciones y estructuras utilizadas en list.c.
map.c: Implementación de un mapa genérico que se usa para guardar y gestionar las listas de reproducción(listas).
map.h: Cabecera que define las funciones y estructuras utilizadas en map.c.
extra.c: implementacione de funciones que hacen capaces la lectura de lineas de archivo y ayudar con la lectura de datos.

🗂️Estructuras y Tdas utilizados:

Estructura `song`:
```c
typedef struct {
  int id;                // Identificador único de la canción
  char *artistas;        // Artista(s) de la canción
  char *album;           // Nombre del álbum
  char *nombreCancion;   // Nombre de la canción
  float tempo;           // Tempo de la canción
  List *genres;          // Lista de géneros de la canción
} song;
```
TDA Lista Enlazada
```c
typedef struct Node {
  void *data;            // Apunta al dato almacenado
  struct Node *next;     // Apunta al siguiente nodo
} Node;

typedef struct List {
  Node *head;            // Primer nodo de la lista
  Node *tail;            // Último nodo de la lista
  Node *current;         // Nodo actual para iteraciones
  int size;              // Tamaño de la lista
} List
```
TDA Mapa
```c
typedef struct {
  void *key;             // Clave del par (ejemplo: género, artista)
  void *value;           // Valor asociado a la clave (ejemplo: lista de canciones)
} MapPair;

typedef struct Map {
  int (*lower_than)(void *key1, void *key2);  // Función para comparar claves (ordenar)
  int (*is_equal)(void *key1, void *key2);   // Función para verificar igualdad de claves
  List *ls;                                  // Lista enlazada de pares clave-valor
};
```

## Características Principales

### Funcionalidades Implementadas:
1. **Cargar Canciones**:
   se le solicita al usuario cuantas canciones va a cargar, luego de eso se lee el archivo csv mediante una variable campos, que ocupa una funcion que va leyendo el archivo linea por linea, con esto sacamos la informacion que queremos de la cancion, id , nombre de la cancion, nombre del album, genero  y tempo, luego de eso se forman listas para guardar cada uno de los datos, ya sea id , tempo, genero , artista, para luego ser guardadas en mapas.

2. **Buscar Canciones**:
   - Búsqueda por género, artista o rango de tempo.
   todas muy similares, se ocupa su respectivo mapa y luego se accede a este segun las especificaciones que se digan, ya sea tipo de tempo, nombre del artista y genero, luego se muestra lo que el usuario pidio 

3. **CrearListaReproducción**:
   se pide al usuario que se diga el nombre que desea ponerle a la lista de reproduccion deseada para luego crearla, guardandola en una lista, para luego ser agregada en un mapa con todas las listas de reproduccion.

4 **agregarCancionLista**
   - se le pide al usuario decir el nombre de la lista que desea acceder para luego de verificar el nombre, si no esta se le avisa con un print, en caso contrario se le pide que escriba el id de la cancion, si el id no exite lo devuelve al menu, si existe se agrega y se avisa se agrega a la lista para luego ser guardada en un mapa.

4. **MostrarCancionesLista**:
   se pide el nombre de la lista, se verifica y luego se ve dentro del mapa que contiene las listas, si esta se accede a esta y se muestra su contenido, si no te devuelve al menu

Funciones de la Lista Enlazada :

    -list_create: Crea una nueva lista enlazada.

    -list_sortedInsert: Inserta un nodo en una posición ordenada según una función de comparación.

    -list_clean:limina todos los nodos de la lista, liberando la memoria asociada a cada uno.

    -list_size:Devuelve el tamaño de la lista (número de elementos almacenados).

    -list_first: Elimina todos los nodos de la lista.

    -list_popCurrent:Elimina el nodo actualmente seleccionado (current) de la lista y devuelve un puntero al dato almacenado en ese nodo.

    -list_pushCurrent:nserta un nuevo elemento después del nodo actual (current) de la lista.

    -list_pushFront: Inserta un nodo al inicio de la lista.

    -list_popFront:Elimina el primer elemento de la lista y devuelve un puntero al dato almacenado en ese nodo.

    -list_pushBack: Inserta un nodo al final de la lista.

    -list_popBack:Elimina el último elemento de la lista y devuelve un puntero al dato almacenado en ese nodo.

Funciones del Mapa

    -map_create: Crea un mapa no ordenado que utiliza una función de igualdad para comparar claves.

    -sorted_map_create: Crea un mapa ordenado que utiliza una función para comparar claves y establecer el orden.

    -map_insert: Inserta un nuevo par clave-valor en el mapa. Si la clave ya existe, no realiza la inserción.

    -map_search: Busca un par clave-valor en el mapa utilizando la clave proporcionada. Devuelve el par si lo encuentra o NULL si no existe.

    -map_remove: Elimina un par clave-valor del mapa usando la clave proporcionada. Devuelve el par eliminado o NULL si la clave no existe.

    -map_first: Devuelve el primer par clave-valor del mapa para comenzar la iteración.

    -map_next: Devuelve el siguiente par clave-valor en el mapa durante una iteración.

    -map_clean: Limpia el contenido del mapa, eliminando todos los pares clave-valor y liberando la memoria utilizada.

## ⚠️ Problemas Conocidos
1. **Caracteres especiales**:
   - cuando se encuentra un caracter especial o letras de otros idiomas como arabe o japones, se tiende a ver raro, incluso creemos que esta afectando directamente al programa por que funciones de buscar muestran los datos raros, incluso afecta a generos.
   
2. **Poca eficiencia a la hora de cargar datos**:
   - gracias a la cantidad de datos, estos pueden ser una gran carga para el computador, es por eso que pusimos la opcion de cargar por partes, ya que cargar todos los datos dependiendo del computador, puede llegar a tomar 10 min en el peor de los casos(pc con poca ram y muy vieja).


---

##  📈 Mejoras Planeadas
- Implementar un mejor sistema de lectura para que aumente su eficiencia.
- hacer que el programa pueda leer caracteres especiales y letras de otros idiomas especiales como arabe o japones.
- Optimizar las búsquedas y el ordenamiento de canciones.
- hacer posible la busqueda por paginas segun la pantalla del usuario para que este no se encuentre confundido por tanta informacion    en la pantalla
- hacer posible la busqueda por paginas, para no sobrecargar tanto al computador del usuario.

---

## 👥 Aportes(en general, cambios en los días)

Recordatorio: el archivo se lee de arriba para abajo, ya que el orden de cambios van del más antiguo al más nuevo.
#---------------------------------------------------
Fecha: 7/5 
Autor: Felipe
Cambios realizados:
- Añadir el archivo de lectura csv.
- Crear la funcion para leer el archivo, funcion cargar_canciones.
- Cambios en el main sobre el nombre de algunas variables.

#---------------------------------------------------
Fecha: 8/5
Autor: Gabriel
Cambios realizados:
- Creación de un ejecutable .sh para facilitar la ejecución.
- Limpieza del menú para mostrar la información correcta.
- Modificación de la función de lectura del CSV para soportar un tercer mapa (por tempo).
- Cambio de los nombres de los mapas para que correspondan a los campos requeridos: Artista, BPM (Tempo), Género.
- Creación de la función de búsqueda por género (terminada, sin testear).

#---------------------------------------------------

Fecha: 8/5
Autor: Felipe
Cambios realizados:
- Creación de la función buscarPorTempo.
- arreglos minimos de main y funcion cargar_canciones.


#---------------------------------------------------

Fecha: 9/5
Autor: Gabriel
Cambios realizados:
- Creación de la funcion buscar por artista.
- Optimizacion de la lectura del archivo CSV.
- Modificación de la función de lectura del CSV para soportar un cuarto mapa (por ID).
- Corregidos errores de ortografia varios y legiblidad del codigo en general 
- Creación de la funcion crear lista de reproduccion .
- Creación de la funcion agregar a lista de reproduccion.

#---------------------------------------------------

Fecha: 9/5
Autor: Felipe
Cambios realizados:
- empezar el README.md.
- revisar funciones para ver sus funcionamientos y posibles errores.
- revision de cargar_canciones para ver diferentes tipos de carga y opciones para mostrar las diferentes funciones de mostrar.

#---------------------------------------------------

Fecha: 10/5
Autor: Felipe
Cambios realizados:
- Creación de la función mostrarCancionesLista.
- encontrar errores y buscar posibles soluciones para funciones(todas en general).

#---------------------------------------------------

Fecha: 11/5
Autor: Felipe
Cambios realizados:
- revision general del programa.
- terminar README.md.

#---------------------------------------------------

en general: aporte de ambos Excelente 10/10.

**importante recalcar que se aprecia la participacion de parte de gpt y copilot por mencionar errores y ideas de como implementar funciones, ademas de ayudar con aspectos de ver como leer correctamente el csv, con la idea de revisarlo directamente.**

## Ejemplo de Uso

```bash
$ ./tarea2.exe
Bienvenido al sistema de gestión de canciones y listas de reproducción.
1. Cargar canciones
2. Buscar canciones por genero
3. Buscar canciones por artista
4. Buscar canciones por tempo
5. Crear lista de reproduccion
6. Agregar cancion a lista
7. Mostrar canciones de la lista
8. Salir

Ingrese una opción: 1
Cargando canciones...
Ingrese cuántas canciones desea cargar: 10
Canciones cargadas correctamente.

Ingrese una opción: 5
Ingrese el nombre de la lista: Mi Lista Favorita
Lista creada con éxito.

Ingrese una opción: 6
Ingrese el nombre de la lista: Mi Lista Favorita
Ingrese el ID de la canción: 12345
Se insertó correctamente.

Ingrese una opción: 7
Ingrese el nombre de la lista: Mi Lista Favorita
| id    | cancion                   | album   |
| tempo | artista                   | genero  |
|----------------------------------------------------------------------------|
