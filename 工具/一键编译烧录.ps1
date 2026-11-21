# 一键编译 + 烧录（作业四）
# 用法：右键这个文件 -> 使用 PowerShell 运行；或在 PowerShell 里执行 .\一键编译烧录.ps1
$ErrorActionPreference = 'Stop'

$ROOT   = Split-Path -Parent (Split-Path -Parent $PSCommandPath)   # diankong4\
$BUILD  = Join-Path $ROOT 'build\debug'
$ELF    = Join-Path $BUILD 'diankong4.elf'

$GCC    = 'D:\apps\diankong-tools\gcc\bin'
$CMAKE  = 'D:\apps\diankong-tools\cmake\bin\cmake.exe'
$NINJA  = 'D:\apps\diankong-tools\ninja'
$OCD    = 'D:\apps\diankong-tools\openocd\bin\openocd.exe'

$env:PATH = "$GCC;$NINJA;" + $env:PATH

Write-Host '=== 1/3 配置 CMake ===' -ForegroundColor Cyan
& $CMAKE -S $ROOT -B $BUILD -G Ninja -DCMAKE_BUILD_TYPE=Debug `
         "-DCMAKE_TOOLCHAIN_FILE=$ROOT\cmake\gcc-arm-none-eabi.cmake" | Out-Null

Write-Host '=== 2/3 编译 ===' -ForegroundColor Cyan
& $CMAKE --build $BUILD
if ($LASTEXITCODE -ne 0) { Write-Host '编译失败，先别看板子，回去看报错。' -ForegroundColor Red; exit 1 }
if (-not (Test-Path $ELF)) { Write-Host "找不到 $ELF" -ForegroundColor Red; exit 1 }

Write-Host '=== 3/3 烧录（OpenOCD + ST-Link）===' -ForegroundColor Cyan
$elfFwd = $ELF -replace '\\', '/'
& $OCD -f interface/stlink.cfg -f target/stm32f1x.cfg `
       -c "program $elfFwd verify reset exit" 2>&1 |
    Select-String -Pattern 'Programming|Verified|Error|error|flash size' |
    ForEach-Object { $_.Line }

Write-Host ''
Write-Host '完成。如果看到 Verified OK，说明烧进去了。' -ForegroundColor Green
Write-Host '接着打开「串口助手（电控作业）」，选 COM5、115200、8N1，就能看到 Target/CCR/Pulse 三行。' -ForegroundColor Green
