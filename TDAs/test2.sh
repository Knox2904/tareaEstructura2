#!/bin/bash
echo "Compilando tarea2.c..."
gcc tarea2.c -o tarea2

if [ $? -eq 0 ]; then
  echo "Compilación exitosa. Ejecutando..."
  ./tarea2
else
  echo "Error en la compilación."
fi
