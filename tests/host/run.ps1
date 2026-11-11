param([string]$Compiler = 'gcc')
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
Push-Location $projectRoot
try {
    New-Item -ItemType Directory -Path 'build/host-tests' -Force | Out-Null
    & $Compiler -std=c11 -Wall -Wextra -Wno-unused-variable -Itests/host/stubs -ISystem/AngleLoop -ISystem/PID_Control -IHardWare/MPU6050 -IHardWare/Motor -IHardWare/BlueTooth tests/host/test_balance.c System/AngleLoop/AngleLoop.c HardWare/MPU6050/MPU6050.c HardWare/Motor/Motor.c HardWare/BlueTooth/BlueTooth.c -o build/host-tests/test_balance.exe -lm
    if ($LASTEXITCODE -ne 0) { throw 'Host compile failed' }
    & ./build/host-tests/test_balance.exe
    if ($LASTEXITCODE -ne 0) { throw 'Host tests failed' }
} finally { Pop-Location }

