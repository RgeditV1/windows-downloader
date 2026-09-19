$runningOnLinux = $IsLinux -or ($env:OS -ne "Windows_NT")

if ($runningOnLinux) {
    $url = "https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader-linux.tar.gz"
    $archive = "/tmp/windows-downloader.tar.gz"
    $extractPath = "/tmp/windows-downloader"
} else {
    $url = "https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader-windows.zip"
    $archive = "$env:TEMP\windows-downloader.zip"
    $extractPath = "$env:TEMP\windows-downloader"
}

try {
    # Descargar el paquete correspondiente
    Invoke-RestMethod -Uri $url -OutFile $archive

    # Crear/limpiar carpeta de extracción
    if (Test-Path $extractPath) {
        Remove-Item $extractPath -Recurse -Force
    }

    New-Item -ItemType Directory -Path $extractPath | Out-Null

    # Descomprimir según la extensión
    if ($runningOnLinux) {
        tar -xzf $archive -C $extractPath
    } else {
        Expand-Archive -Path $archive -DestinationPath $extractPath -Force
    }

    # Buscar el ejecutable (.exe en Windows o binario nativo en Linux)
    $exe = Get-ChildItem -Path $extractPath -Recurse | Where-Object { 
        $_.Name -eq "windows-downloader" -or $_.Name -eq "windows-downloader.exe" 
    } | Select-Object -First 1

    if ($null -eq $exe) {
        Write-Error "No se encontró el ejecutable 'windows-downloader' en el paquete descargado."
        exit 1
    }

    if ($runningOnLinux) {
        # Permisos y ejecución en Linux
        chmod +x $exe.FullName
        & $exe.FullName
    } else {
        # Ejecución en Windows
        Start-Process -FilePath $exe.FullName -Wait
    }
}
finally {
    # Limpieza de archivos temporales
    if (Test-Path $archive) {
        Remove-Item $archive -Force -ErrorAction SilentlyContinue
    }

    if (Test-Path $extractPath) {
        Remove-Item $extractPath -Recurse -Force -ErrorAction SilentlyContinue
    }
}