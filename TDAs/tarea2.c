

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
  char *artistas;
  char *album;
  char *nombreCancion;
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

//revamp
void cargar_canciones(Map *cancionesArtista, Map *cancionesGenero, Map *cancionesTempo, Map *cancionesID, List *listaGeneral) {
    static FILE *archivo = NULL; // para mantener abierto entre llamadas

    const char *datos = "data/song_dataset_.csv";

    if (archivo == NULL) {
        archivo = fopen(datos, "r");
        if (archivo == NULL) {
            perror("Error al abrir el archivo");
            return;
        }
        leer_linea_csv(archivo, ','); // saltar encabezado
        printf("Archivo abierto correctamente\n");
    }

    int cantidad_a_leer;
    printf("Cuantas canciones deseas cargar en esta sesion?: ");
    scanf("%d", &cantidad_a_leer);

    char **campos;
    int contador = 0;

    while (contador < cantidad_a_leer && (campos = leer_linea_csv(archivo, ',')) != NULL) {
        contador++;

        if (contador % 1000 == 0) printf("Procesando cancion #%d | ID: %s\n", contador, campos[0]);

        song *cancion = malloc(sizeof(song));
        if (cancion == NULL) {
            printf("Error al reservar memoria\n");
            continue;
        }

        cancion->id = atoi(campos[0]);
        cancion->artistas = strdup(campos[2]);
        cancion->album = strdup(campos[3]);
        cancion->nombreCancion = strdup(campos[4]);
        cancion->tempo = atof(campos[18]);
        cancion->genres = split_string(campos[20], ",");

        // --- Lista general ---
        list_pushBack(listaGeneral, cancion);

        // --- MAPA: Género ---
        char *genre = list_first(cancion->genres);
        MapPair *genre_pair = map_search(cancionesGenero, genre);
        List *genre_list = genre_pair ? genre_pair->value : NULL;
        if (genre_list == NULL) {
            genre_list = list_create();
            map_insert(cancionesGenero, genre, genre_list);
        }
        list_pushBack(genre_list, cancion);

        // --- MAPA: Artista ---
        MapPair *artista_pair = map_search(cancionesArtista, cancion->artistas);
        List *artista_list = artista_pair ? artista_pair->value : NULL;
        if (artista_list == NULL) {
            artista_list = list_create();
            map_insert(cancionesArtista, cancion->artistas, artista_list);
        }
        list_pushBack(artista_list, cancion);

        // --- MAPA: Tempo ---
        char *tempo_rango;
        if (cancion->tempo < 80)
            tempo_rango = "Lentas";
        else if (cancion->tempo <= 120)
            tempo_rango = "Moderadas";
        else
            tempo_rango = "Rapidas";

        MapPair *tempo_pair = map_search(cancionesTempo, tempo_rango);
        List *tempo_list = tempo_pair ? tempo_pair->value : NULL;
        if (tempo_list == NULL) {
            tempo_list = list_create();
            map_insert(cancionesTempo, tempo_rango, tempo_list);
        }
        list_pushBack(tempo_list, cancion);

        // --- MAPA: ID ---
        int *key = malloc(sizeof(int));
        *key = cancion->id;
        map_insert(cancionesID, key, cancion);
    }

    if (contador < cantidad_a_leer) {
        printf("Se llegó al final del archivo.\n");
        fclose(archivo);
        archivo = NULL;
    } else {
        printf("Se procesaron %d canciones en esta sesion.\n", contador);
    }
}



void crearListaReproducion(Map* mapaListasReproducion){
  List* aux = list_create() ; 
  if(aux == NULL) { printf("\033[1;31mFATAL ERROR SE CREO MAL LA LISTA \033[0m\n"); exit(EXIT_FAILURE) ; }

  printf("ingrese el nombre de la Lista(50 char Max) : \n") ;
  char nombre[51] ; 
  scanf(" %50[^\n]s" , nombre) ; 

  if (map_search(mapaListasReproducion , nombre) != NULL){
    printf("\033[1;31mFATAL ERROR NOMBRE EN USO \033[0m\n");
    return;
  }

  map_insert(mapaListasReproducion ,strdup(nombre) ,aux) ; 
  printf("se creo la lista exitosamente \n") ; 
  
}


void AgregarCancionLista(Map* mapaListasReproducion, Map* cancionesID) {
  if (mapaListasReproducion == NULL || map_first(mapaListasReproducion) == NULL) {
    printf("El mapa de listas de reproduccion está vacio o no se inicializo correctamente.\n");
    return;
  }

  char nombreLista[51];
  int id;

  printf("Ingrese el nombre de la lista en la que desea ingresar (50 char max):\n");
  scanf(" %50[^\n]s", nombreLista);

  // ahora Buscamos la lista de reproducción
  MapPair *listaPair = map_search(mapaListasReproducion, nombreLista);
  if (listaPair == NULL) {
    printf("La lista '%s' no existe.\n", nombreLista);
    return;
  }

  printf("Ingrese el ID de la cancion:\n");
  scanf("%d", &id);

  // se obtiene la lista de canciones asociada a la lista de reproducción
  List *listaCanciones = (List *)listaPair->value;

  // se Verifica si la canción ya está en la lista
  song *cancionExistente = list_first(listaCanciones);
  while (cancionExistente != NULL) {
    if (cancionExistente->id == id) {
      printf("La canción con ID %d ya está en la lista '%s'.\n", id, nombreLista);
      return;
    }
    cancionExistente = list_next(listaCanciones);
  }

  // Buscar la canción por ID
  MapPair *cancionPair = map_search(cancionesID, &id);
  if (cancionPair == NULL) {
    printf("La cancion con ID %d no existe.\n", id);
    return;
  }

  song *cancion = (song *)cancionPair->value;

  //finalmente se agrega la canción a la lista
  list_pushBack(listaCanciones, cancion);

  printf("Se agrego la cancion '%s' a la lista '%s'.\n", cancion->nombreCancion, nombreLista);
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
    printf("El mapa esta vacio o no se inicio correctamente. Por favor, revisar la carga del archivo CSV.\n");
    return;
  }

  printf("Seleccione el rango de tempo para buscar canciones:\n");
  printf("1. Lentas (Tempo < 80 BPM)\n");
  printf("2. Moderadas (80 <= Tempo <= 120 BPM)\n");
  printf("3. Rapidas (Tempo > 120 BPM)\n");
  printf("Ingrese su opcion (1-3): ");

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
  printf("|----------------------------------------------------------------------------|\n");
  printf("| %-7s | %-30s | %-20s |\n", "ID", "Nombre", "Álbum");
  printf("| %-7s | %-30s | %-20s |\n", "Tempo", "Artista", "Género");
  printf("|----------------------------------------------------------------------------|\n");

  while (cancion != NULL) {
    printf("| %-5d | %-30s | %-20s |\n", cancion->id, cancion->nombreCancion, cancion->album);
    printf("| %-5.1f | %-30s | %-20s |\n", cancion->tempo, cancion->artistas, cancion->genres);
    printf("|----------------------------------------------------------------------------|\n");
    cancion = list_next(listaCanciones);
  }

  printf("+------------------------------+------------------------------+\n");
}

void buscaPorArtista(Map*cancionesArtista) {

  if(cancionesArtista == NULL || map_first(cancionesArtista) == NULL) {
    printf("el mapa de las caniciones esta vacio , favor revisar la lectura del .CSV \n") ; 
    return;
  }

  printf("ingrese el nombre del artista que desea buscar \n") ; 
  char nombreArtista [100]; 
  scanf(" %[^\n]s" , nombreArtista) ; 

  MapPair *par = map_search(cancionesArtista , nombreArtista) ;
  if(par == NULL){
    printf("el artista no se encuentra en el mapa ") ; 
    return;
  }

  List* compiladoDeCanciones = (List*)par->value ; 
  song* cancion = list_first(compiladoDeCanciones) ;
  //tabla cortesia de la profe laura 
  printf("|----------------------------------------------------------------------------|\n");
  printf("| %-7s | %-30s | %-20s |\n", "ID", "Nombre", "Álbum");
  printf("| %-7s | %-30s | %-20s |\n", "Tempo", "Artista", "Género");
  printf("|----------------------------------------------------------------------------|\n");

  while (cancion != NULL) {
    printf("| %-5d | %-30s | %-20s |\n", cancion->id, cancion->nombreCancion, cancion->album);
    printf("| %-5.1f | %-30s | %-20s |\n", cancion->tempo, cancion->artistas, cancion->genres);
    printf("|----------------------------------------------------------------------------|\n");

    cancion = list_next(compiladoDeCanciones);
  }
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

    char *generoCSV = parActual->key ; //guardamos el genero en una variable para poder usarlo mas facil 
    List* listaCanciones = parActual->value ; // lo mismo de arriba pero ahora con las canciones en lista 

    if(strcmp(generoUsuario , generoCSV) == 0){
      song* cancion = list_first(listaCanciones) ; //lo de arriba pero ahora una variable (una cancion no todas como arriba)
      //gracias profe laura por la tabla :D
      printf("|----------------------------------------------------------------------------|\n");
      printf("| %-7s | %-30s | %-20s |\n", "ID", "Nombre", "Álbum");
      printf("| %-7s | %-30s | %-20s |\n", "Tempo", "Artista", "Género");
      printf("|----------------------------------------------------------------------------|\n");

      while (cancion != NULL) {
        printf("| %-5d | %-30s | %-20s |\n", cancion->id, cancion->nombreCancion, cancion->album);
        printf("| %-5.1f | %-30s | %-20s |\n", cancion->tempo, cancion->artistas, cancion->genres);
        printf("|----------------------------------------------------------------------------|\n");

        cancion = list_next(listaCanciones);
      }

      printf("+------------------------------+------------------------------+\n");
      bandera = 1 ; 
    }

    parActual = map_next(cancionesGenero) ; 
  }
  if(!bandera){
    printf("no se encontraron canciones con el genero dado \n") ; 
  }
}

void MostrarCancionesLista(Map *mapaListasReproducion)
{
  if (mapaListasReproducion == NULL || map_first(mapaListasReproducion) == NULL) 
  {
    printf("El mapa de listas de reproduccion esta vacio o no se inicio bien , por favor revisar la lectura del csv.\n");
    return;
  }

  
  
  printf("Ingrese el nombre de la lista que desea ver :\n");
  char Lista[51];
  scanf(" %50[^\n]s" , Lista);

  MapPair *ListaP = map_search(mapaListasReproducion , Lista);
  if (ListaP == NULL)
  {
    printf("la lista '%s' no esta registrada.\n", Lista);
    return;
  }

  List *cancionesL = (List *)ListaP -> value;
  song * cancion = list_first(cancionesL);

  if (cancion == NULL)
  { 
    printf("la lista '%s' esta vacia.\n", Lista);
    return;
  } 

  printf("|----------------------------------------------------------------------------|\n");
  printf("| %-7s | %-30s | %-20s |\n", "ID", "Nombre", "Álbum");
  printf("| %-7s | %-30s | %-20s |\n", "Tempo", "Artista", "Género");
  printf("|----------------------------------------------------------------------------|\n");

  while (cancion != NULL) 
  {
    printf("| %-5d | %-30s | %-20s |\n", cancion->id, cancion->nombreCancion, cancion->album);
    printf("| %-5.1f | %-30s | %-20s |\n", cancion->tempo, cancion->artistas, cancion->genres);
    printf("|----------------------------------------------------------------------------|\n");

    cancion = list_next(cancionesL);
  }

  printf("+------------------------------------------------------------------------------------+\n");
  
}

int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *cancionesID = map_create(is_equal_str) ; 
  Map *cancionesGenero = map_create(is_equal_str);
  Map *cancionesArtista = map_create(is_equal_str);
  Map *cancionesTempo = map_create(is_equal_float) ;
  Map *mapaListasReproducion = map_create(is_equal_str) ; 
  List* listaGeneral = list_create() ; 
  

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opcion: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      printf("Cargando canciones...\n");
      cargar_canciones(cancionesArtista, cancionesGenero , cancionesTempo , cancionesID , listaGeneral); // semi-Terminado (Felipe) , modificado para poder agregar al tercer mapa y cuarto(Gabriel) // testeado funciona 
      printf("Canciones cargadas correctamente.\n");
      break;
    case '2':
      buscarPorGenero(cancionesGenero); // terminado (Gabriel) ->sin testear<- // testeado funciona 
      break;
    case '3':
      buscaPorArtista(cancionesArtista); // terminado (Gabriel) ->sin testear<- // testeado funciona 
      break;
    case '4':
      buscarPorTempo(cancionesTempo) ; // terminado(Felipe) ->sin testear<- // testeado funciona 
      break;
    case '5':
      crearListaReproducion(mapaListasReproducion) ;  // terminado (Gabriel) // testeado funciona 
      break;
    case '6':
      AgregarCancionLista(mapaListasReproducion , cancionesID) ; // terminado (Gabriel) ->sin testear<- //testeado funciona
      break;
    case '7':
      MostrarCancionesLista(mapaListasReproducion) ; // terminado (Felipe)  ->sin testear<- //testeado funciona
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  song *cancion = list_first(listaGeneral);
  while (cancion != NULL) 
  {
    
    free(cancion->artistas);
    free(cancion->album);
    free(cancion->nombreCancion);

    
    char *genero = list_first(cancion->genres);
    while (genero != NULL) 
    {
      free(genero);
      genero = list_next(cancion->genres);
    }
    list_clean(cancion->genres);
    free(cancion->genres);

    
    free(cancion);

    
    cancion = list_next(listaGeneral);
  }
  list_clean(listaGeneral);
  free(listaGeneral);

  
  MapPair *par = map_first(cancionesArtista);
  while (par != NULL) 
  {
    free(par->key); 
    
    List *lista = (List *)par->value;
    song *cancion = list_first(lista);
    while (cancion != NULL) 
    {
      free(cancion); 
      cancion = list_next(lista);
    }
    list_clean(lista);
    free(lista);

    free(par); 
    par = map_next(cancionesArtista);
  }
  map_clean(cancionesArtista);
  free(cancionesArtista);

  
  par = map_first(cancionesGenero);
  while (par != NULL) 
  {
    free(par->key); 
    List *lista = (List *)par->value;
    list_clean(lista);
    free(lista);

    free(par); 
    par = map_next(cancionesGenero);
  }
  map_clean(cancionesGenero);
  free(cancionesGenero);

  
  par = map_first(cancionesTempo);
  while (par != NULL) 
  {
    free(par->key); 
    List *lista = (List *)par->value;
    list_clean(lista);
    free(lista);

    free(par); 
    par = map_next(cancionesTempo);
  }
  map_clean(cancionesTempo);
  free(cancionesTempo);

  
  par = map_first(cancionesID);
  while (par != NULL) 
  {
    free(par->key); 
    free(par->value); 
    free(par); 
    par = map_next(cancionesID);
  }
  map_clean(cancionesID);
  free(cancionesID);

  
  par = map_first(mapaListasReproducion);
  while (par != NULL) 
  {
    free(par->key); 
    List *lista = (List *)par->value;
    list_clean(lista);
    free(lista);

    free(par); 
    par = map_next(mapaListasReproducion);
  }
  map_clean(mapaListasReproducion);
  free(mapaListasReproducion);


  return 0;
}
