#!/bin/bash
echo "Compilando tarea3.c..."
# Usa gcc de MinGW para compilar en Windows
gcc tarea3.c TDAs/*.c -o tarea3.exe

# Verifica si la compilación fue exitosa
if [ $? -eq 0 ]; then
  echo "Compilación exitosa. Ejecutando..."
  # Ejecuta el archivo .exe generado
  ./tarea3.exe
else
  echo "Error en la compilación."
fi