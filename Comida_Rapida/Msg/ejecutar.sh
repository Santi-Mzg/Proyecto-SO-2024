#!/bin/bash

# Compila el archivo C
gcc -o "fastfood_msg" "fastfood_msg.c" -pthread

./"fastfood_msg"
