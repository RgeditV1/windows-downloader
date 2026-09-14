$url = "https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader.zip"

$zip = "$env:TEMP\windows-downloader.zip"
$extractPath = "$env:TEMP\windows-downloader"

try {
    # Descargar el ZIP
    Invoke-RestMethod -Uri $url -OutFile $zip

    # Crear carpeta de extracción
    if (Test-Path $extractPath) {
        Remove-Item $extractPath -Recurse -Force
    }

    New-Item -ItemType Directory -Path $extractPath | Out-Null

    # Descomprimir
    Expand-Archive -Path $zip -DestinationPath $extractPath -Force

    # Buscar el ejecutable
    $exe = Get-ChildItem -Path $extractPath -Filter "windows-downloader.exe" -Recurse | Select-Object -First 1

    if ($null -eq $exe) {
        Write-Error "No se encontró windows-downloader.exe dentro del ZIP."
        exit 1
    }

    Start-Process -FilePath $exe.FullName -Wait
}
finally {

    if (Test-Path $zip) {
        Remove-Item $zip -Force -ErrorAction SilentlyContinue
    }

    if (Test-Path $extractPath) {
        Remove-Item $extractPath -Recurse -Force -ErrorAction SilentlyContinue
    }
}