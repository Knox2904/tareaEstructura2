#!/bin/bash
echo "Compilando tarea2.c..."
# Usa gcc de MinGW para compilar en Windows
gcc tarea2.c TDAs/*.c -o tarea2.exe

# Verifica si la compilación fue exitosa
if [ $? -eq 0 ]; then
  echo "Compilación exitosa. Ejecutando..."
  # Ejecuta el archivo .exe generado
  ./tarea2.exe
else
  echo "Error en la compilación."
fi
