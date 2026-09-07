# windows-downloader
CLI APLICATION TO DOWNLOAD WINDOWS/LINUX/MAC OS

###  Requerimientos
- Instala ``VCPKG`` para el manejo de dependencias, puedes usar ``git submodule``
- Con ``vcpkg`` instala ``Libcurl`` y ``nlohmann_json``

### Instalacion de Requerimientos con VCPKG
```powershell

cd 3rdparty/vcpkg
.\vcpkg install curl
.\vcpkg install nlohmann-json:x64-windows
```