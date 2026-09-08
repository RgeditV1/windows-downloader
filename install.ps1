$url = "https://github.com/RgeditV1/windows-downloader/releases/latest/download/windows-downloader.exe"
$output = "$env:TEMP\windows-downloader.exe"

# Descargar el .exe
Invoke-RestMethod -Uri $url -OutFile $output

# Ejecutar la aplicación
Start-Process -FilePath $output -Wait