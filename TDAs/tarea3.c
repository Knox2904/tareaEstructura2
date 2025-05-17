#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define N 4
#define M 4
#define EspacioCelda 22

typedef struct {
    char *nombre; 
    int id ; 
    char *descripcion; 
    List *items ; 
    char* conexiones[4] ; 
    int esFinal ; 
    
}Escenario;

typedef struct {
    char *nombre ; 
    int peso ; 
    int valor ; 
}Item;

typedef struct {
    int idJugador ; 
    char *nombreJugador ;
    Escenario *posicion ; 

} Jugador  ;

Map* mapaEscenarios ; 
List* jugadores ;

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

void mostrarMenuPrincipal() {
    limpiarPantalla();
    puts("========================================");
    puts("               Graph Quest              ");
    puts("========================================");

    puts("1) Cargar CSV");
    puts("2) Empezar a jugar");
    puts("3) Salir de emergencia");
}


/**
 * Carga canciones desde un archivo CSV
 */
void leer_escenarios(Map* mapaEscenarios) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/graphquest.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV


  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {

    Escenario *escenaioAux = malloc(sizeof(Escenario)) ; 
    escenaioAux->items = list_create() ; 

    escenaioAux->id = atoi(campos[0]) ; 
    escenaioAux->nombre = strdup(campos[1]) ; 
    escenaioAux->descripcion = strdup(campos[2]); 


    List* items = split_string(campos[3], ";");


    for(char *item = list_first(items); item != NULL; 
          item = list_next(items)){

        List* values = split_string(item, ",");
        char* item_name = strdup(list_first(values));
        int item_value = atoi(list_next(values));
        int item_weight = atoi(list_next(values));
        
        Item* nuevoItem = malloc(sizeof(Item)) ; 
        nuevoItem->nombre = item_name;
        nuevoItem->valor = item_value;
        nuevoItem->peso = item_weight;
        
        list_pushBack(escenaioAux->items , nuevoItem) ; 

        list_clean(values);
        free(values);
    }

    escenaioAux->conexiones[0] = (atoi(campos[4]) != -1) ? strdup(campos[4]) : NULL; //ternarios •𐃷•
    escenaioAux->conexiones[1] = (atoi(campos[5]) != -1) ? strdup(campos[5]) : NULL; 
    escenaioAux->conexiones[2] = (atoi(campos[6]) != -1) ? strdup(campos[6]) : NULL; 
    escenaioAux->conexiones[3] = (atoi(campos[7]) != -1) ? strdup(campos[7]) : NULL; 

    escenaioAux->esFinal = atoi(campos[8]);
    int *key = malloc(sizeof(int));
    *key = escenaioAux->id;

    map_insert(mapaEscenarios , key , escenaioAux) ;

    list_clean(items);
    free(items);
    
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas

}


bool existeElJugador(List *jugadores , const char* nombre){

    if(list_first(jugadores) == NULL) return false ; 

    for (Jugador* j = list_first(jugadores); j != NULL; j = list_next(jugadores)) {
        if (strcmp(j->nombreJugador, nombre) == 0) {
            return true; 
        }
    }    
    return false ;
}


void ConseguirNombres(Map* mapaEscenarios , List* jugadores){
    limpiarPantalla();
    puts("========================================");
    puts("               Graph Quest              ");
    puts("========================================");

    int cantidadJugadores ; 
    printf("Ingrese la cantidad de jugadores : \n") ;
    scanf("%d" , &cantidadJugadores) ; 


    char* nombre ;
    nombre = malloc(51) ; //50 caracteres maximo 

    int idInicial = 0;
    char* idInicialStr = malloc(12) ; 
    sprintf(idInicialStr, "%d", idInicial);

    int i = 0 ; 
    MapPair* par = map_search(mapaEscenarios, idInicialStr);
    Escenario* escenarioInicial = par ? (Escenario*) par->value : NULL;

    do {
        printf("ingrese el nombre del jugador : \n") ;
        scanf(" %s" , nombre) ;

        if(existeElJugador(jugadores , nombre)){
            printf("El nombre del jugador ya esta en uso \n") ; 
            continue;
        }

        Jugador* jugadorTemp = malloc(sizeof(Jugador)) ;

        jugadorTemp->idJugador = i ; 
        jugadorTemp->nombreJugador = strdup(nombre) ; 

        MapPair* par2 = map_search(mapaEscenarios, idInicialStr);
        jugadorTemp->posicion = par2 ? (Escenario*) par2->value : NULL;

        list_pushBack(jugadores , jugadorTemp) ; 
        i++ ; 
    } while (i < cantidadJugadores);
    
    free(nombre);
    free(idInicialStr);

}


void MostrarMapa(Map* mapaEscenarios, List* jugadores) { //no funciona aun , hay que revisar las conexiones 
    Escenario* matriz[N][M] = {NULL};

    // Llenamos la matriz con los escenarios según su ID
    MapPair* par = map_first(mapaEscenarios);
    while (par) {
        Escenario* escenario = (Escenario*) par->value;
        int fila = (escenario->id - 1) / M;
        int columna = (escenario->id - 1) % M;
        if (fila < N && columna < M)
            matriz[fila][columna] = escenario;
        par = map_next(mapaEscenarios);
    }

    for (int i = 0; i < N; i++) {
        // Línea 1: imprimir nombres e IDs
        for (int j = 0; j < M; j++) {
            Escenario* esc = matriz[i][j];
            if (esc) {
                printf("| %2d: %-*s", esc->id, EspacioCelda - 6, esc->nombre); // 6 = "| %2d: "
            } else {
                printf("| %-*s", EspacioCelda, "");
            }
        }
        printf("|\n");

        // Línea 2: conexiones horizontales (izquierda <--> derecha)
        for (int j = 0; j < M; j++) {
            Escenario* esc = matriz[i][j];
            Escenario* derecha = (j < M - 1) ? matriz[i][j + 1] : NULL;

            if (esc && derecha) {
                // Verificamos si hay conexión a la derecha
                int hayConexion = 0;
                if (esc->conexiones[3] && strcmp(esc->conexiones[3], derecha->nombre) == 0)
                    hayConexion = 1;

                if (hayConexion)
                    printf("       <-->       ");
                else
                    printf("                  ");
            } else {
                printf("                  ");
            }
        }
        printf("\n");

        // Línea 3: conexiones verticales (abajo)
        if (i < N - 1) {
            for (int j = 0; j < M; j++) {
                Escenario* esc = matriz[i][j];
                Escenario* abajo = matriz[i + 1][j];

                if (esc && abajo) {
                    int hayConexion = 0;
                    if (esc->conexiones[1] && strcmp(esc->conexiones[1], abajo->nombre) == 0)
                        hayConexion = 1;

                    if (hayConexion)
                        printf("        |         ");
                    else
                        printf("                  ");
                } else {
                    printf("                  ");
                }
            }
            printf("\n");
        }
    }
}




int main() {
    mapaEscenarios = map_create(is_equal_str) ; //inicio mnapa
    jugadores = list_create();
    char opcion ; 


    do{
        mostrarMenuPrincipal() ; 
        printf("Que desea hacer : \n") ; 
        scanf(" %c" , &opcion) ; 


        switch (opcion){
            case '1' :
                printf("cargando escenario \n") ; 
                leer_escenarios(mapaEscenarios);
                printf("cargando carga exitosa! \n") ; 
            break;

            case '2' : 

                printf("buena suerte : \n") ;
                ConseguirNombres(mapaEscenarios , jugadores) ; //revisar si se leyo el csv
                MostrarMapa(mapaEscenarios , jugadores) ; 
            break;

            case '3' :
            break;

            default :
            printf("opcion  no valida , selecione una opcion valida (1 o 2)\n") ;
            break; 
        }
        presioneTeclaParaContinuar();

    }while(opcion != '3') ; 
    
    return 0;
}