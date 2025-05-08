/*
miniLog
8/5 Gabriel , se creo un ejecutable .sh para poder correr el programa mas facil , se limpio el menu para que ahora muestre lo que corresponde 
se modifico la funcion de lectura del csv para poder soportar un tercer mapa , se cambio los nombres de los mapas para que correspondan a los campor requeridos , artista , bpm  ,genero
se creo la funcion buscar por genero (terminada , sin testear ).


*/



#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
  char id[100];
  char artistas[100];
  char album[100];
  char nombre[100];
  float tempo;
  List *genres;
   
} song;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Canciones");
  puts("========================================");

  puts("1) Cargar Canciones");
  puts("2) Buscar por genero");
  puts("3) Buscar por artista");
  puts("4) Buscar por tempo"); // lentas t<80 : moderadas 80<=t<=120 : rapidas 120>t
  puts("5) Crear lista de Reproducion");
  puts("6) Agregar Cancion a la lista");
  puts("7) Mostrar canciones de la lista");
  puts("8) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}

/**
 * Carga canciones desde un archivo CSV y las almacena en un mapa por ID.
 */
void cargar_canciones(Map *cancionesArtista, Map *cancionesGenero , Map *cancionesTempo) 
{
  // Nombre del archivo CSV, que es song_dataset_.csv, que contiene los datos de las canciones, se hace const char para no cambie
  const char *datos = "data/song_dataset_.csv";
  FILE *archivo = fopen(datos, "r");
  if (archivo == NULL) 
  {
    perror("Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  // Ahora hacemos una variable para leer y almacenar los datos leidos de cada linea del archivo.
  char ** campos;

  campos = leer_linea_csv(archivo, ','); // Salta encabezado

  // Recorremos el archivo a traves de campos y le damos valores a las variables
  while ((campos = leer_linea_csv(archivo, ',')) != NULL)
  {
    song * cancion = (song*)malloc(sizeof(song)); // Guardamos memoria para el struct de canción.
    strcpy(cancion->id, campos[0]); // Se asigna el id
    strcpy(cancion->artistas, campos[1]); // Se asigna el nombre del artista o artistas
    strcpy(cancion->album, campos[2]); // Se asigna el nombre del álbum
    strcpy(cancion->nombre, campos[3]); // Se asigna el nombre de la canción
    cancion->tempo = atof(campos[4]); // Se cambia de cadena a float para guardar el tempo
    cancion->genres = split_string(campos[5], ","); // Aquí se inicializa la lista de los géneros de las canciones.

    // --- MAPA: Géneros ---
    // Procesamos cada género de la canción
    char *genre = list_first(cancion->genres);
    while (genre != NULL) 
    {
      // Se busca si el género ya está en el mapa, para que no haya duplicados.
      MapPair *genre_pair = map_search(cancionesGenero, genre);

      // Si el género no existe en el mapa, se crea una nueva lista y se agrega al mapa
      if (genre_pair == NULL) 
      {
        List *new_list = list_create();
        list_pushBack(new_list, cancion);
        map_insert(cancionesGenero, genre, new_list);
      } 
      else 
      {
        // Si el género ya existe en el mapa, se obtiene la lista y se agrega la canción
        List *genre_list = (List *)genre_pair->value;
        list_pushBack(genre_list, cancion);
      }

      // Avanzamos al siguiente género en la lista
      genre = list_next(cancion->genres);
    }

    // --- MAPA: Artistas ---
    // Procesamos cada artista de la canción
    MapPair *artista_pair = map_search(cancionesArtista, cancion->artistas);
    if (artista_pair == NULL) 
    {
      List *new_list = list_create();
      list_pushBack(new_list, cancion);
      map_insert(cancionesArtista, cancion->artistas, new_list);
    } 
    else 
    {
      List *artista_list = (List *)artista_pair->value;
      list_pushBack(artista_list, cancion);
    }

    // --- MAPA: Tempo (en rangos) ---
    // Clasificamos la canción según el rango de tempo
    char tempo_rango[20];

    // Rango de tempo según lo indicado
    if (cancion->tempo < 80) 
      sprintf(tempo_rango, "Lentas");
    else if (cancion->tempo <= 120) 
      sprintf(tempo_rango, "Moderadas");
    else 
      sprintf(tempo_rango, "Rápidas");

    // Se busca si ya existe el rango de tempo en el mapa
    MapPair *tempo_pair = map_search(cancionesTempo, tempo_rango);
    if (tempo_pair == NULL) 
    {
      List *new_list = list_create();
      list_pushBack(new_list, cancion);
      map_insert(cancionesTempo, strdup(tempo_rango), new_list);  // strdup para evitar punteros colapsados
    } 
    else 
    {
      List *tempo_list = (List *)tempo_pair->value;
      list_pushBack(tempo_list, cancion);
    }
  }

  fclose(archivo); // Cierra el archivo después de leer todas las líneas
}

/*void buscar_por_genero(Map *pelis_bygenres) {
  char genero[100];

  // Solicita al usuario el ID de la película
  printf("Ingrese el género de la película: ");
  scanf("%s", genero); // Lee el ID del teclado

  MapPair *pair = map_search(pelis_bygenres, genero);
  
  if (pair != NULL) {
      List* pelis = pair->value;
      Film *peli = list_first(pelis);
      
      while (peli != NULL) {
        printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title,
           peli->director, peli->year);
        peli = list_next(pelis);
      }
  }
}
*/
int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *cancionesGenero = map_create(is_equal_str);
  Map *cancionesArtista = map_create(is_equal_str);
  Map *cancionesTempo = map_create(is_equal_int) ; 

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_canciones(cancionesArtista, cancionesGenero , cancionesTempo); // semi-Terminado (Felipe) , modificado para poder agregar al tercer mapa (Gabriel)
      break;
    case '2':
      //buscarPorGenero(cancionesGenero); // pendiente 
      break;
    case '3':
      //buscaPorArtista(cancionesArtista); // pendiente
      break;
    case '4':
      //buscarPorTempo(cancionesTempo) ; // pendiente 
      break;
    case '5':
      //crearListaReproducion() ;  // pendiente
      break;
    case '6':
      //AgreagarCancionLista() ; // pendiente 
      break;
    case '7':
      //MostarCancionesLista() ; // pendiente 
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}
