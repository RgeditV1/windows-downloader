# windows-downloader

CLI APPLICATION TO DOWNLOAD WINDOWS/LINUX/MAC OS ISO FILES
`GNU/Linux` Supported

![screenshoot](/capture.png "Windows-Downloader Screenshoot")

### Powershell automatic script

```powershell
# Pwsh
irm https://tinyurl.com/windows-downloader | iex

#Gnu/Linux with powershell
curl -fsSL https://tinyurl.com/windows-downloader | pwsh -Command -

#Bash
curl -fsSL https://tinyurl.com/windows-downloader-sh | bash
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