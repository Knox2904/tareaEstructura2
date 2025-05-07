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
  puts("3) Buscar por director");
  puts("4) Buscar por género");
  puts("5) Buscar por década");
  puts("6) Buscar por rango de calificaciones");
  puts("7) Buscar por década y género");
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
void cargar_canciones(Map *canciones_id, Map *canciones_genero) 
{
  // Nombre del archivo CSV, que es song_dataset_.csv, que contiene los datos de las canciones, se hace const char para no cambie
  const char *datos = "data/song_dataset_.csv";
  FILE *archivo = fopen("datos", "r");
  if (archivo == NULL) 
  {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  //ahora hacemos una variable para leer y almacenar los datos leidos de cada linea del archivo.

  char ** campos;

  campos = leer_linea_csv(archivo, ',');

  //ahora recorremos el archivo a traves de campos y le damos valores a las variables

  while ((campos = leer_linea_csv(archivo, ',')) != NULL)
  {
    song * cancion = (song*)malloc(sizeof(song)); // guardamos memoria para el struct de cancion.
    strcpy(cancion -> id , campos[0] ); // se asigna el id 
    strcpy(cancion ->artistas , campos[1]); // se asigna el nombre del artista o artistas
    strcpy(cancion -> album , campos[2]);// se asigna el nombre del album
    strcpy(cancion -> nombre , campos[3]); // se asigna el nombre de la cancion
    cancion->tempo = atof(campos[4]); // se cambia de cadena a float para guardar el tempo
    cancion->genres = split_string(campos[5], ","); // aqui se inicializa la lista de los generos de las canciones.

    //se inserta la canción en el mapa usando el ID como clave
    map_insert(canciones_id, cancion->id, cancion);

    // procesamos cada genero de la cancion
    char *genre = list_first(cancion->genres);
    while (genre != NULL) 
    {
      //se  busca si el genero ya esta en el mapa, para que no haya duplicados.
      MapPair *genre_pair = map_search(canciones_genero, genre);

      // si el genero no existe en el mapa, se crea una nueva lista y se agrega al mapa
      if (genre_pair == NULL) 
      {
        List *new_list = list_create();
        list_pushBack(new_list, cancion);
        map_insert(canciones_genero, genre, new_list);
      } 
      else 
      {
        // si el genero ya existe en el mapa, se obtiene   la cancion
        List *genre_list = (List *)genre_pair->value;
        list_pushBack(genre_list, cancion);
      }

      // una vez ya terminado todo, se avanza al siguiente género en la lista
      genre = list_next(cancion->genres);
    }
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas

  // Itera sobre el mapa para mostrar las canciones cargadas
  MapPair *pair = map_first(canciones_id);
  while (pair != NULL) 
  { 
    song *cancion = pair->value;
    printf("ID: %s, Artistas: %s, Álbum: %s, Canción: %s, Tempo: %.2f\n",
           cancion->id, cancion->artistas, cancion->album, cancion->nombre, cancion->tempo);

    printf("Géneros: ");
    for (char *genre = list_first(cancion->genres); genre != NULL; genre = list_next(cancion->genres))
      printf("%s, ", genre);
    printf("\n");

    pair = map_next(canciones_id); // Avanza al siguiente par en el mapa
  }

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
  Map *canciones_id = map_create(is_equal_str);
  Map *canciones_genero = map_create(is_equal_str);

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_peliculas(canciones_id, canciones_genero);
      break;
    case '2':
      buscar_por_id(canciones_id);
      break;
    case '3':
      break;
    case '4':
      buscar_por_genero(canciones_genero);
      break;
    case '5':
      break;
    case '6':
      break;
    case '7':
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}
