$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
$out = Join-Path $root 'build_vs\Release'
New-Item -ItemType Directory -Force -Path $out | Out-Null
cmd.exe /d /s /c "call `"$vs`" && cl /nologo /std:c++17 /EHsc /O2 /utf-8 /DUNICODE /D_UNICODE `"$root\src\main.cpp`" `"$root\src\battle_rules.cpp`" `"$root\src\status_rules.cpp`" `"$root\src\battle_state.cpp`" `"$root\src\collection_state.cpp`" `"$root\src\data_registry.cpp`" `"$root\src\battle_ui.cpp`" `"$root\src\save_rules.cpp`" `"$root\src\skill_state.cpp`" `"$root\src\world_state.cpp`" /Fe:`"$out\YinYangChronicle.exe`" user32.lib gdi32.lib"
if ($LASTEXITCODE -ne 0) { throw 'MSVC build failed' }
Get-Item (Join-Path $out 'YinYangChronicle.exe') | Select-Object FullName, Length
