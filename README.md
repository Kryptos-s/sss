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

## Build prerequisites
1. Install [vcpkg](https://github.com/microsoft/vcpkg) and set `VCPKG_ROOT`.
2. Bootstrap vcpkg:
   ```powershell
   .\vcpkg\bootstrap-vcpkg.bat
   ```

## Configure + build
```powershell
cmake --preset debug
cmake --build --preset build-debug
ctest --preset test-debug
```

Release:
```powershell
cmake --preset release
cmake --build --preset build-release
```

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
