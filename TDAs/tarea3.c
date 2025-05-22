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
    char* conexiones[4] ; //arriba , abajo , izq , der 
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

    int cantidadJugadores;
    printf("Ingrese la cantidad de jugadores: \n");
    scanf("%d", &cantidadJugadores);

    char* nombre = malloc(51);  // 50 caracteres maximo + '\0'
    int idInicial = 1;
    char idInicialStr[12]; 
    sprintf(idInicialStr, "%d", idInicial);

    int i = 0;
    do {
        printf("Ingrese el nombre del jugador: \n");
        scanf(" %50s", nombre);

        if (existeElJugador(jugadores, nombre)) {
            printf("El nombre del jugador ya está en uso.\n");
            continue;
        }

        Jugador* jugadorTemp = malloc(sizeof(Jugador));
        jugadorTemp->idJugador = i;
        jugadorTemp->nombreJugador = strdup(nombre);

        MapPair* par2 = map_search(mapaEscenarios, &idInicial) ; 
        if (!par2) {
            printf("Error: No se encontró el escenario inicial.\n");
            free(jugadorTemp);
            continue;
        }
        jugadorTemp->posicion = (Escenario*) par2->value;

        list_pushBack(jugadores, jugadorTemp);
        i++;
    } while (i < cantidadJugadores);
    
    free(nombre);
}


void MostrarMapa(Map* mapaEscenarios, List* jugadores) {
    Escenario* matriz[N][M] = {NULL};

    // Llenar la matriz con escenarios
    MapPair* par = map_first(mapaEscenarios);
    while (par) {
        Escenario* escenario = (Escenario*) par->value;
        int fila = (escenario->id - 1) / M;
        int col = (escenario->id - 1) % M;
        if (fila < N && col < M)
            matriz[fila][col] = escenario;
        par = map_next(mapaEscenarios);
    }

    // Verificar si la lista de jugadores está inicializada
    if (!jugadores) {
        printf("Error: La lista de jugadores no está inicializada.\n");
        return;
    }
    if (list_size(jugadores) == 0) {
        printf("Error: No hay jugadores en la lista.\n");
        return;
    }

    for (int i = 0; i < N; i++) {
        // Línea 1: Escenarios + conexión horizontal
        for (int j = 0; j < M; j++) {
            Escenario* esc = matriz[i][j];
            if (esc) {
                // Verificar si hay jugadores en este escenario
                char jugadoresEnEscenario[EspacioCelda] = "";
                Jugador* jugador = list_first(jugadores);
                
                while (jugador) {
                    if (jugador->posicion) {
                        if (jugador->posicion->id == esc->id) {
                            strcat(jugadoresEnEscenario, jugador->nombreJugador);
                            strcat(jugadoresEnEscenario, ", ");
                        }
                    } else {
                        printf("Advertencia: El jugador %s no tiene una posición definida.\n", jugador->nombreJugador);
                    }

                    jugador = list_next(jugadores);
                }

                // Eliminar la última coma y espacio para evitar formato incorrecto
                if (strlen(jugadoresEnEscenario) > 0) {
                    jugadoresEnEscenario[strlen(jugadoresEnEscenario) - 2] = '\0';
                }

                // Mostrar el escenario con los jugadores si hay alguno presente
                if (strlen(jugadoresEnEscenario) > 0)
                    printf("| %2d: %-*s [%s]", esc->id, EspacioCelda - 6, esc->nombre, jugadoresEnEscenario);
                else
                    printf("| %2d: %-*s", esc->id, EspacioCelda - 6, esc->nombre);
            } else {
                printf("| %-*s", EspacioCelda, "");
            }

            // Conexión a la derecha
            if (j < M - 1) {
                Escenario* derecha = matriz[i][j + 1];
                int hayConexion = 0;
                if (esc && derecha && esc->conexiones[3] && atoi(esc->conexiones[3]) == derecha->id)
                    hayConexion = 1;

                if (hayConexion)
                    printf(" <-----> ");
                else
                    printf("         ");
            }
        }
        printf("|\n");

        // Línea 2: Conexiones verticales
        if (i < N - 1) {
            for (int j = 0; j < M; j++) {
                Escenario* esc = matriz[i][j];
                Escenario* abajo = matriz[i + 1][j];
                if (esc && abajo && esc->conexiones[1] && atoi(esc->conexiones[1]) == abajo->id)
                    printf("%*s", EspacioCelda - 3, "||");
                else
                    printf("%*s", EspacioCelda - 3, " ");

                // Espacio entre columnas (compensar <----->)
                if (j < M - 1)
                    printf("         ");
            }
            printf("\n");
        }
    }
}


void acionesJugador(Map* mapaEscenarios, List* jugadores){

    puts("========================================");
    puts("               Graph Quest              ");
    puts("========================================");
    puts("que desea hacer ? : ") ; 

    puts("1) recoger item");//falta , revisar conflictos 
    puts("2) descartar item");//falta 
    puts("3) Avanzar en una direccion");//falta 
    puts("4) reiniciar partida ");//falta 
    puts("5) Salir del juego "); //falta 
}



void jugar(Map* mapaEscenarios , List* jugadores){

    int tiempo = 10 ; // cambia la logica de esto a la formula que esta en el Notion
    do{
        MostrarMapa(mapaEscenarios , jugadores) ; 
        mostrarEstadoJugador(mapaEscenarios , jugadores);//falta 
        acionesJugador(mapaEscenarios , jugadores);//falta  
        tiempo--;
    } while (tiempo != 0 );
    
    
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
                jugar(mapaEscenarios , jugadores) ; 
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