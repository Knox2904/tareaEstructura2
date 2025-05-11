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
void cargar_canciones(Map *cancionesArtista, Map *cancionesGenero, Map *cancionesTempo, Map *cancionesID) {
    const char *datos = "data/song_dataset_.csv";
    FILE *archivo = fopen(datos, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    printf("Archivo abierto correctamente\n");

    char **campos;
    campos = leer_linea_csv(archivo, ','); // Saltamos el encabezado

    int contador = 0; // Para contar cuántas canciones se han procesado
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        contador++;
        if (contador % 1000 == 0) printf("Procesando cancion #%d | ID: %s\n", contador, campos[0]);


        song *cancion = (song*)malloc(sizeof(song));
        if (cancion == NULL) {
            printf("Error al reservar memoria para la cancion\n");
            continue;
        }

        cancion->id = atoi(campos[0]); 
        cancion->artistas = strdup(campos[2]); 
        cancion->album = strdup(campos[3]); 
        cancion->nombreCancion = strdup(campos[4]); 
        cancion->tempo = atof(campos[18]); 
        cancion->genres = split_string(campos[20], ",");

        

        // --- MAPA: Géneros ---
        char *genre = list_first(cancion->genres);
        MapPair *genre_pair = map_search(cancionesGenero, genre);
        List *genre_list = (genre_pair != NULL) ? (List*)genre_pair->value : NULL;

        if (genre_list == NULL) {
            genre_list = list_create();
            map_insert(cancionesGenero, genre, genre_list);
        }
        list_pushBack(genre_list, cancion);

        // --- MAPA: Artistas ---
        MapPair *artista_pair = map_search(cancionesArtista, cancion->artistas);
        List *artista_list = (artista_pair != NULL) ? (List*)artista_pair->value : NULL;

        if (artista_list == NULL) {
            artista_list = list_create();
            map_insert(cancionesArtista, cancion->artistas, artista_list);
        }
        list_pushBack(artista_list, cancion);

        // --- MAPA: Tempo ---
        //Asignamos el rango de tempo
        char *tempo_rango;
        if (cancion->tempo < 80) 
            tempo_rango = "Lentas";
        else if (cancion->tempo <= 120) 
            tempo_rango = "Moderadas";
        else 
            tempo_rango = "Rápidas";



        MapPair *tempo_pair = map_search(cancionesTempo, tempo_rango);
        List *tempo_list = (tempo_pair != NULL) ? (List*)tempo_pair->value : NULL;

        if (tempo_list == NULL) {
            tempo_list = list_create();
            map_insert(cancionesTempo, tempo_rango, tempo_list);
        }
        list_pushBack(tempo_list, cancion);

        // --- MAPA: ID ---
        map_insert(cancionesID, &cancion->id, cancion);
    }

    printf("Se han procesado un total de %d canciones.\n", contador);
    fclose(archivo);
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


void AgreagarCancionLista(Map* mapaListasReproducion, Map* cancionesID) {
  if (mapaListasReproducion == NULL || map_first(mapaListasReproducion) == NULL) {
    printf("El mapa esta vacio o no se inicializo bien. Favor revisar la creacion del mapa.\n");
    return;
  }

  char nombreLista[51];
  int id;

  printf("Ingrese el nombre de la lista en la que desea ingresar (50 char max):\n");
  scanf(" %50[^\n]s", nombreLista);

  MapPair *lista_pair = map_search(mapaListasReproducion, nombreLista);
  if (lista_pair == NULL) {
      printf("\033[1;31mFATAL ERROR: La lista no existe.\033[0m\n");
     return;
  }

  printf("Ingrese el ID de la canción:\n");
  scanf("%d", &id);

  Map *cancionesListasCreadas = (Map *)lista_pair->value;


  if (map_search(cancionesListasCreadas, &id) != NULL) {
    printf("\033[1;31mFATAL ERROR: La canción ya está en la lista.\033[0m\n");
    return;
  }

  song* cancion = (song*) map_search(cancionesID, &id);
  if (cancion == NULL) {
    printf("\033[1;31mFATAL ERROR: La canción no existe.Presione una tecla para continuar \033[0m\n");
    getchar() ; 
    return;
  }

  int* key = malloc(sizeof(int));
  *key = id;
  map_insert(cancionesListasCreadas, key, cancion);

  printf("Se insertó correctamente.\n");
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
  printf("+------------------------------+------------------------------+\n");
  printf("|          Nombre              |           Artista           |\n");
  printf("+------------------------------+------------------------------+\n");

  while (cancion != NULL) {
    printf("| %-28s | %-28s |\n", cancion->nombreCancion, cancion->artistas);
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
  scanf( "%50[^\n]s" , Lista);

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
    //aqui se concatenan los generos en una cadena por si es que la cancion tiene mas de un genero
    char generosC[200] = "";
    char *genero = list_first(cancion->genres);
    while (genero != NULL)
    {
      strcat(generosC , genero);
      genero = list_next(cancion -> genres);
      if (genero != NULL) strcat(generosC , ", ");

    }

    printf("| %-5d | %-30s | %-20s |\n", cancion->id, cancion->nombreCancion, cancion->album);
    printf("| %-5.1f | %-30s | %-20s |\n", cancion->tempo, cancion->artistas, generosC);
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

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opcion: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      printf("Cargando canciones...\n");
      cargar_canciones(cancionesArtista, cancionesGenero , cancionesTempo , cancionesID); // semi-Terminado (Felipe) , modificado para poder agregar al tercer mapa y cuarto(Gabriel) // testeado funciona 
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
      AgreagarCancionLista(mapaListasReproducion , cancionesID) ; // terminado (Gabriel) ->sin testear<- 
      break;
    case '7':
      MostarCancionesLista() ; // terminado (Felipe)  ->sin testear<-
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}
