#!/usr/bin/env bash

set -e

# Configuración de URLs y rutas temporales
REPO_URL="https://github.com/RgeditV1/windows-downloader"
RELEASE_URL="$REPO_URL/releases/latest/download/windows-downloader.zip"

TMP_DIR=$(mktemp -d)
ARCHIVE_FILE="$TMP_DIR/windows-downloader.zip"

# Limpieza automática al finalizar o en caso de error
cleanup() {
    rm -rf "$TMP_DIR"
}
trap cleanup EXIT

echo -e "\033[36m\033[1m=== Windows Downloader Installer ===\033[0m\n"

# 1. Descargar el paquete ejecutable
echo -e "\033[33mDescargando paquete de instalación para Linux...\033[0m"
if ! curl -fsSL "$RELEASE_URL" -o "$ARCHIVE_FILE"; then
    echo -e "\033[31mError: No se pudo descargar el archivo de la release.\033[0m" >&2
    exit 1
fi

# 2. Descomprimir el archivo
echo -e "\033[33mExtrayendo archivos...\033[0m"
if [[ "$ARCHIVE_FILE" == *.tar.gz ]]; then
    tar -xzf "$ARCHIVE_FILE" -C "$TMP_DIR"
elif [[ "$ARCHIVE_FILE" == *.zip ]]; then
    unzip -q "$ARCHIVE_FILE" -d "$TMP_DIR"
fi

EXE=$(find "$TMP_DIR" -type f -name "windows-downloader" | head -n 1)

if [ -z "$EXE" ]; then
    echo -e "\033[31mError: No se encontró el ejecutable 'windows-downloader' en el paquete.\033[0m" >&2
    exit 1
fi

# Dar permisos de ejecución
chmod +x "$EXE"

echo -e "\033[32m\033[1mIniciando aplicación...\033[0m\n"
"$EXE"