#!/bin/bash

gcc minishell.c -o minishell
gcc crearDirectorio.c -o crearDirectorio
gcc eliminarDirectorio.c -o eliminarDirectorio
gcc ayuda.c -o ayuda
gcc crearArchivo.c -o crearArchivo
gcc listarContenidoDirectorio.c -o listarContenidoDirectorio
gcc mostrarContenidoArchivo.c -o mostrarContenidoArchivo
gcc modificarPermisos.c -o modificarPermisos
./minishell
