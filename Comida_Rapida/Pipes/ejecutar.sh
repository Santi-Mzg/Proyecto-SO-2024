#!/bin/bash

# Compila el archivo C
gcc -o "fastfood_pipe" "fastfood_pipe.c" -pthread

./"fastfood_pipe"
