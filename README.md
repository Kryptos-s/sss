# Activator (C++20 + Dear ImGui)

Windows-first desktop activator built from scratch with Dear ImGui.

## Features
- License login via KeyAuth (`init` + `license` flow).
- Stable Windows HWID source derivation (`MachineGuid|VolumeSerial|CPUBrand`) and SHA-256 hash.
- Backend API integration for entitlement verification and extension build requests.
- Server-authoritative cooldown support (`cooldownUntil`) with client-side UX lockout.
- Runtime ZIP download, SHA-256 verification, and unzip into `%APPDATA%/<AppName>/downloads/<buildId>/`.
- Secure session storage via Windows DPAPI (`session.dat`).
- Sanitized logging and copyable diagnostics.

## Build prerequisites (Windows)
1. Install **Visual Studio 2022** with **Desktop development with C++**.
2. Install [CMake](https://cmake.org/download/) (3.21+).
3. Install [vcpkg](https://github.com/microsoft/vcpkg), then bootstrap:
   ```powershell
   .\vcpkg\bootstrap-vcpkg.bat
   ```
4. Set `VCPKG_ROOT` to your vcpkg path:
   ```powershell
   setx VCPKG_ROOT "C:\path\to\vcpkg"
   ```
   Restart terminal after `setx`.

## Recommended Windows build (no Ninja required)
From a normal `cmd` or PowerShell terminal:
```powershell
cmake --preset windows-msvc-debug
cmake --build --preset build-windows-msvc-debug
ctest --preset test-windows-msvc-debug
```

Release:
```powershell
cmake --preset windows-msvc-release
cmake --build --preset build-windows-msvc-release
```

## Optional Ninja workflow
Use only if Ninja and compiler environment are available:
```powershell
cmake --preset debug
cmake --build --preset build-debug
ctest --preset test-debug
```

## Why you saw `No CMAKE_CXX_COMPILER could be found`
This usually happens when using the Ninja preset without a compiler toolchain available in PATH.

Fix options:
- Use the **Visual Studio presets** above (`windows-msvc-*`) so CMake uses VS generator directly.
- Or open **x64 Native Tools Command Prompt for VS 2022** and then run Ninja presets.
- Ensure C++ workload is installed in Visual Studio Installer.

## Environment variables
Copy `.env.example` and set:
- `ACTIVATOR_BACKEND_URL`
- `KEYAUTH_APP_NAME`
- `KEYAUTH_OWNER_ID`
- `KEYAUTH_APP_SECRET`
- `KEYAUTH_APP_VERSION`
- `APP_VERSION`

The app reads environment first, then `%APPDATA%/<AppName>/config.json` from Settings.

## Security notes
- TLS verification is always enabled.
- No secrets are logged.
- License key is not persisted by default.
- Session is protected with DPAPI at `%APPDATA%/<AppName>/session.dat`.

## KeyAuth endpoint notes
`src/keyauth/KeyAuthClient.cpp` currently targets `https://keyauth.win/api/1.2/` and sends JSON parameters for `init`, `license`, and `fetchOnline`. If your KeyAuth deployment has different routes/parameter requirements, update this class accordingly.
