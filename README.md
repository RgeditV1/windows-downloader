# windows-downloader

CLI APPLICATION TO DOWNLOAD WINDOWS/LINUX/MAC OS ISO FILES
`GNU/Linux` Supported

![screenshoot](/capture.png "Windows-Downloader Screenshoot")

### Powershell automatic script

```powershell
# Pwsh/linux powershell too
irm https://tinyurl.com/windows-downloader | iex

#Bash
bash <(curl -fsSL https://tinyurl.com/windows-downloader-sh)
```

### Requirements

- Install `VCPKG` for dependency management. You can use `git submodule`.
- Install `Libcurl` and `nlohmann_json` using `vcpkg`.

### Installing Requirements with VCPKG

```powershell
cd 3rdparty/vcpkg

.\vcpkg install curl

.\vcpkg install nlohmann-json:x64-windows #x64-Linux
```