$runningOnLinux = $IsLinux -or ($env:OS -ne "Windows_NT")

if ($runningOnLinux) {
    $url = "https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader.zip"
    $zip = "/tmp/windows-downloader.zip"
    $extractPath = "/tmp/windows-downloader"
} else {
    $url = "https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader.zip"
    $zip = "$env:TEMP\windows-downloader.zip"
    $extractPath = "$env:TEMP\windows-downloader"
}

try {
    # Descargar el archivo
    Invoke-RestMethod -Uri $url -OutFile $zip

    if (Test-Path $extractPath) {
        Remove-Item $extractPath -Recurse -Force
    }

    New-Item -ItemType Directory -Path $extractPath | Out-Null

    # Descomprimir
    Expand-Archive -Path $zip -DestinationPath $extractPath -Force

    $exe = Get-ChildItem -Path $extractPath -Recurse | Where-Object { $_.Name -match "^windows-downloader(\.exe)?$" } | Select-Object -First 1

    if ($null -eq $exe) {
        Write-Error "No se encontró el ejecutable dentro del paquete."
        exit 1
    }

    if ($runningOnLinux) {
        # Asignar permisos de ejecución en Linux
        chmod +x $exe.FullName
        & $exe.FullName
    } else {
        Start-Process -FilePath $exe.FullName -Wait
    }
}
finally {
    if (Test-Path $zip) {
        Remove-Item $zip -Force -ErrorAction SilentlyContinue
    }
    if (Test-Path $extractPath) {
        Remove-Item $extractPath -Recurse -Force -ErrorAction SilentlyContinue
    }
}