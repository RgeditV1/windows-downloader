#!/usr/bin/env bash

set -e

# Definición de archivos y carpetas temporales
TMP_DIR=$(mktemp -d)
ARCHIVE_FILE="$TMP_DIR/windows-downloader-archive"
EXTRACT_PATH="$TMP_DIR/extracted"

mkdir -p "$EXTRACT_PATH"

# Limpieza automática al finalizar o interrumpir el script
cleanup() {
    rm -rf "$TMP_DIR"
}
trap cleanup EXIT

OS_TYPE="$(uname -s 2>/dev/null || echo "Unknown")"

if [[ "$OS_TYPE" == *"Linux"* ]]; then
    IS_LINUX=true
    URL="https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader-linux.tar.gz"
else
    IS_LINUX=false
    URL="https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader-windows.zip"
fi

# 1. Descargar el archivo
echo "Descargando paquete desde: $URL..."
if command -v curl >/dev/null 2>&1; then
    curl -fsSL "$URL" -o "$ARCHIVE_FILE"
elif command -v wget >/dev/null 2>&1; then
    wget -qO "$ARCHIVE_FILE" "$URL"
else
    echo "Error: Se requiere curl o wget para realizar la descarga." >&2
    exit 1
fi

# 2. Descomprimir según el tipo de archivo
echo "Extrayendo archivos..."
if [ "$IS_LINUX" = true ]; then
    tar -xzf "$ARCHIVE_FILE" -C "$EXTRACT_PATH"
else
    if command -v unzip >/dev/null 2>&1; then
        unzip -q "$ARCHIVE_FILE" -d "$EXTRACT_PATH"
    elif command -v 7z >/dev/null 2>&1; then
        7z x "$ARCHIVE_FILE" -o"$EXTRACT_PATH" >/dev/null
    else
        echo "Error: Se requiere 'unzip' o '7z' para descomprimir el archivo en Windows/Bash." >&2
        exit 1
    fi
fi

# 3. Buscar el ejecutable (windows-downloader o windows-downloader.exe)
EXE=$(find "$EXTRACT_PATH" -type f \( -name "windows-downloader" -o -name "windows-downloader.exe" \) | head -n 1)

if [ -z "$EXE" ]; then
    echo "Error: No se encontró el ejecutable 'windows-downloader' en el paquete descargado." >&2
    exit 1
fi

# 4. Ejecución
if [ "$IS_LINUX" = true ]; then
    chmod +x "$EXE"
    "$EXE"
else
    chmod +x "$EXE" 2>/dev/null || true
    "$EXE"
fi