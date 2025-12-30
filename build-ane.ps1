# Build Discord ANE
# This script compiles the C++ native extensions, ActionScript bridge, and packages the ANE

Write-Host "=== Building Discord ANE ===" -ForegroundColor Cyan

# Step 1: Build x64 DLL
Write-Host "`n[1/5] Building x64 DLL..." -ForegroundColor Yellow
Push-Location "ane-c++-sdk"
New-Item -ItemType Directory -Force -Path ".\build\x86-64" | Out-Null
cmd /c '"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cl.exe /LD /Zi /EHsc /nologo /FS /MP /Fe.\build\x86-64\DiscordSDK64.dll /Fo.\build\x86-64\ /Idiscord-files /IMPLIB:.\build\x86-64\DiscordSDK.lib lib\x86-64\discord_game_sdk.dll.lib lib\x86-64\FlashRuntimeExtensions.lib DiscordSDK.cpp discord-files\*.cpp'
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: x64 DLL compilation failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}
Write-Host "x64 DLL compiled successfully" -ForegroundColor Green

# Step 2: Build x86 DLL
Write-Host "`n[2/5] Building x86 DLL..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force -Path ".\build\x86" | Out-Null
cmd /c '"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" && cl.exe /LD /Zi /EHsc /nologo /FS /MP /Fe.\build\x86\DiscordSDK.dll /Fo.\build\x86\ /Idiscord-files /IMPLIB:.\build\x86\DiscordSDK.lib lib\x86\discord_game_sdk.dll.lib lib\x86\FlashRuntimeExtensions.lib DiscordSDK.cpp discord-files\*.cpp'
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: x86 DLL compilation failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}
Write-Host "x86 DLL compiled successfully" -ForegroundColor Green
Pop-Location

# Step 3: Build ActionScript SWC
Write-Host "`n[3/5] Building ActionScript SWC..." -ForegroundColor Yellow
Push-Location "ane-as3-bridge"
compc +configname=air -source-path src -include-sources src -output bin/DiscordGameSDK.swc
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: SWC compilation failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}
Write-Host "SWC compiled successfully" -ForegroundColor Green
Pop-Location

# Step 4: Copy files to package directory
Write-Host "`n[4/5] Copying files to package directory..." -ForegroundColor Yellow
Copy-Item "ane-as3-bridge\bin\DiscordGameSDK.swc" "ane-package\DiscordGameSDK.swc" -Force
Copy-Item "ane-c++-sdk\build\x86\DiscordSDK.dll" "ane-package\Windows-x86\DiscordSDK.dll" -Force
Copy-Item "ane-c++-sdk\build\x86-64\DiscordSDK64.dll" "ane-package\Windows-x86-64\DiscordSDK64.dll" -Force

# Copy Discord SDK runtime DLLs (required at runtime)
if (Test-Path "ane-c++-sdk\lib\x86\discord_game_sdk.dll") {
    Copy-Item "ane-c++-sdk\lib\x86\discord_game_sdk.dll" "ane-package\Windows-x86\discord_game_sdk.dll" -Force
    Write-Host "Copied x86 discord_game_sdk.dll" -ForegroundColor Green
} else {
    Write-Host "WARNING: x86 discord_game_sdk.dll not found in lib folder" -ForegroundColor Yellow
}

if (Test-Path "ane-c++-sdk\lib\x86-64\discord_game_sdk.dll") {
    Copy-Item "ane-c++-sdk\lib\x86-64\discord_game_sdk.dll" "ane-package\Windows-x86-64\discord_game_sdk.dll" -Force
    Write-Host "Copied x64 discord_game_sdk.dll" -ForegroundColor Green
} else {
    Write-Host "WARNING: x64 discord_game_sdk.dll not found in lib folder" -ForegroundColor Yellow
}

Write-Host "Files copied successfully" -ForegroundColor Green

# Step 5: Package ANE
Write-Host "`n[5/5] Packaging ANE..." -ForegroundColor Yellow
Push-Location "ane-package"
adt -package -target ane DiscordANE.ane extension.xml -swc DiscordGameSDK.swc -platform Windows-x86-64 -C Windows-x86-64 . -platform Windows-x86 -C Windows-x86 .
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: ANE packaging failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}
Write-Host "ANE packaged successfully" -ForegroundColor Green

# Copy to test application
Copy-Item "DiscordANE.ane" "..\test-application\DiscordANE.ane" -Force
Write-Host "ANE copied to test-application" -ForegroundColor Green
Pop-Location

Write-Host "`n=== Build Complete! ===" -ForegroundColor Cyan
Write-Host "ANE location: ane-package\DiscordANE.ane" -ForegroundColor Green
