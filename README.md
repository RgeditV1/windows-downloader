# windows-downloader

CLI APPLICATION TO DOWNLOAD WINDOWS/LINUX/MAC OS ISO FILES

![screenshoot](/capture.png "Windows-Downloader Screenshoot")

### Requirements

- Install `VCPKG` for dependency management. You can use `git submodule`.
- Install `Libcurl` and `nlohmann_json` using `vcpkg`.

### Installing Requirements with VCPKG

```powershell
cd 3rdparty/vcpkg

.\vcpkg install curl

.\vcpkg install nlohmann-json:x64-windows
```