#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/stack.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char nombre[100] ; 
    char descripcion[100] ; 
    List *items ; 
    char* conexiones[4] ; 
    int esFinal ; 
    
}Escenario;

typedef struct {
    char nombre[50] ; 
    int peso ; 
    int valor ; 
}Item;

Map* mapaEscenarios ; 

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
void leer_escenarios() {
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
    printf("ID: %d\n", atoi(campos[0]));
    printf("Nombre: %s\n", campos[1]);
    printf("Descripción: %s\n", campos[2]);

    List* items = split_string(campos[3], ";");

    printf("Items: \n");
    for(char *item = list_first(items); item != NULL; 
          item = list_next(items)){

        List* values = split_string(item, ",");
        char* item_name = list_first(values);
        int item_value = atoi(list_next(values));
        int item_weight = atoi(list_next(values));
        printf("  - %s (%d pts, %d kg)\n", item_name, item_value, item_weight);
        list_clean(values);
        free(values);
    }

    int arriba = atoi(campos[4]);
    int abajo = atoi(campos[5]);
    int izquierda = atoi(campos[6]);
    int derecha = atoi(campos[7]);

    if (arriba != -1) printf("Arriba: %d\n", arriba);
    if (abajo != -1) printf("Abajo: %d\n", abajo);
    if (izquierda != -1) printf("Izquierda: %d\n", izquierda);
    if (derecha != -1) printf("Derecha: %d\n", derecha);

    
    int is_final = atoi(campos[8]);
    if (is_final) printf("Es final\n");

    list_clean(items);
    free(items);
    
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas

}

void jugar(){

    printf("no hay nada aun :P \n") ; 

}




int main() {
    mapaEscenarios = map_create(is_equal_str) ; //inicio mnapa
 
    char opcion ; 


    do{
        mostrarMenuPrincipal() ; 
        printf("Que desea hacer : \n") ; 
        scanf(" %c" , &opcion) ; 


        switch (opcion){
            case '1' :
            printf("cargando escenario \n") ; 
            leer_escenarios(mapaEscenarios);
            break;

            case '2' : 

                printf("buena suerte : \n") ;
                jugar() ; //revisar si se leyo el csv
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