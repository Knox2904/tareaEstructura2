/*
ve el nuevo archivo porfa , puedes borrar este mensaje cuando lo hagas 
*/

#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct 
{
  int id;
  char artistas[100];
  char album[100];
  char nombreCancion[100];
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

// Comparador de floats con una tolerancia , cortesia GPT , para poder mantener el como se crean los mapas
int is_equal_float(void *key1, void *key2) {
  float val1 = *(float *)key1;
  float val2 = *(float *)key2;
  float epsilon = 0.0001;  // Tolerancia 

  return fabs(val1 - val2) < epsilon;  // Si la diferencia es menor que epsilon, se consideran iguales
}

/*
  Carga canciones desde un archivo CSV y las almacena en un mapa por ID.
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
  printf("Archivo abierto correctamente\n");

  // Ahora hacemos una variable para leer y almacenar los datos leidos de cada linea del archivo.
  char ** campos;

  campos = leer_linea_csv(archivo, ','); // Salta encabezado

  // Recorremos el archivo a traves de campos y le damos valores a las variables
  while ((campos = leer_linea_csv(archivo, ',')) != NULL)
  {
    printf("Leyendo cancion ID: %s\n", campos[0]);
    song * cancion = (song*)malloc(sizeof(song)); // Guardamos memoria para el struct de canción.
    cancion->id = atoi(campos[0]); // Se asigna el id
    strcpy(cancion->artistas, campos[2]); // Se asigna el nombre del artista o artistas
    strcpy(cancion->album, campos[3]); // Se asigna el nombre del álbum
    strcpy(cancion->nombreCancion, campos[4]); // Se asigna el nombre de la canción
    cancion->tempo = atof(campos[18]); // Se guarda el tempo
    cancion->genres = split_string(campos[20], ","); // Aquí se inicializa la lista de los géneros de las canciones.

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

void buscarPorGenero( Map* cancionesGenero){
  if(cancionesGenero == NULL || map_first(cancionesGenero) == NULL){
    printf("el mapa esta vacio o no se inicio bien , favor revisar la lectura del .csv \n") ; 
    return;
  }

  printf("ingrese el genero que se desea buscar (sin espacios) \n") ; 
  char generoUsuario[100] ; 
  scanf("%99s" , generoUsuario) ; 

  MapPair *parActual = map_first(cancionesGenero) ; 
  int bandera = 0 ; 

  while(parActual != NULL){ //iteraciones en el mapa 

    char *generoCSV = parActual->key ; //guardamos el genero en una varable para poder usarlo mas facil 
    List* listaCanciones = parActual->value ; // lo mismo de arriba pero ahora con las canciones en lista 

    if(strcmp(generoUsuario , generoCSV) == 0){
      song* cancion = list_first(listaCanciones) ; //lo de arriba pero ahoara una variable (una cancion no todas como arriba)
      //gracias profe laura por la tabla :D
      printf("+------------------------------+------------------------------+\n");
      printf("|          Nombre              |           Artista           |\n");
      printf("+------------------------------+------------------------------+\n");

      while (cancion != NULL) {
        printf("| %-28s | %-28s |\n", cancion->nombreCancion, cancion->artistas);
        cancion = list_next(listaCanciones);
      }

      printf("+------------------------------+------------------------------+\n");
      bandera = 1 ; 
    }

    parActual = map_next(cancionesGenero) ; 
  }
  if(!bandera){
    printf("no se encontraron canciones con el genro dado \n") ; 
  }
}



/*
esta es la funcion buscarPorTempo, ya que en la funcion cargar_canciones se categorizo los tempos según Lentas, Moderadas y Rápidas, lo
unico que hace esta funcion es buscar según el tipo de tempo, en este caso usando una variable llamada rtempo, que se ocupara
en un switch para los distintos casos, 4 en particular, los tipos de tempos y el default que seria ninguno de esos.
*/
void buscarPorTempo(Map *cancionesTempo) 
{
  if (cancionesTempo == NULL || map_first(cancionesTempo) == NULL) 
  {
    printf("El mapa está vacío o no se inicio correctamente. Por favor, revisar la carga del archivo CSV.\n");
    return;
  }

  printf("Seleccione el rango de tempo para buscar canciones:\n");
  printf("1. Lentas (Tempo < 80 BPM)\n");
  printf("2. Moderadas (80 <= Tempo <= 120 BPM)\n");
  printf("3. Rápidas (Tempo > 120 BPM)\n");
  printf("Ingrese su opción (1-3): ");

  int opcion;
  scanf("%d", &opcion);

  //ahora se usan cases para que no se llene de ifs y sea más eficiente el codigo y mas amigable de leer.
  //se usan los cases con una variable que toma las distintas situaciones de tempo, ya sea Lentas, Moderadas o Rápidas.
  char *rTempo;
  switch (opcion) {
    case 1:
      rTempo = "Lentas";
      break;
    case 2:
      rTempo = "Moderadas";
      break;
    case 3:
      rTempo = "Rápidas";
      break;
    default:
      printf("Opción no válida.\n");
      return;
  }

  //  ahora buscamos el rango de tempo en el mapa, se mostrara un mensaje en la pantalla que dira la situacion, ya sea 
  //si es que se encontro o en el peor caso de que no exista cancion con ese tipo de tempo se avisara la situacion.
  MapPair *tempoPair = map_search(cancionesTempo, rTempo);
  if (tempoPair == NULL) {
    printf("No se encontraron canciones en el rango de tempo descrito (%s).\n", rTempo);
    return;
  }

  // se obtiene la lista de canciones asociada al rango de tempo
  List *listaCanciones = (List *)tempoPair->value;
  song *cancion = list_first(listaCanciones);

  //  ahora se muestra las canciones del rango de tempo seleccionado
  printf("+------------------------------+------------------------------+\n");
  printf("|          Nombre              |           Artista           |\n");
  printf("+------------------------------+------------------------------+\n");

  while (cancion != NULL) {
    printf("| %-28s | %-28s |\n", cancion->nombreCancion, cancion->artistas);
    cancion = list_next(listaCanciones);
  }

  printf("+------------------------------+------------------------------+\n");
}



int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *cancionesGenero = map_create(is_equal_str);
  Map *cancionesArtista = map_create(is_equal_str);
  Map *cancionesTempo = map_create(is_equal_float) ; 

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opcion: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      printf("Cargando canciones...\n");
      cargar_canciones(cancionesArtista, cancionesGenero , cancionesTempo); // semi-Terminado (Felipe) , modificado para poder agregar al tercer mapa (Gabriel)
      printf("Canciones cargadas correctamente.\n");
      break;
    case '2':
      buscarPorGenero(cancionesGenero); // terminado (Gabriel) ->sin testear<-
      break;
    case '3':
      //buscaPorArtista(cancionesArtista); // pendiente
      break;
    case '4':
      buscarPorTempo(cancionesTempo) ; // terminado(Felipe) ->sin testear<-
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
