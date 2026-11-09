$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
$installation = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
if (!$installation) { throw 'Visual Studio C++ tools not found.' }
$msbuild = Join-Path $installation 'MSBuild/Current/Bin/MSBuild.exe'
$vcvars = Join-Path $installation 'VC/Auxiliary/Build/vcvars64.bat'
Push-Location $repoRoot
try {
    & $msbuild D2DGameFramework.sln /m /p:Configuration=Debug /p:Platform=x64 /nologo /verbosity:minimal
    if ($LASTEXITCODE -ne 0) { throw 'Game build failed.' }
    New-Item -ItemType Directory -Force artifacts | Out-Null
    $arguments = @('/nologo /std:c++20 /EHsc /MDd /D_DEBUG /utf-8 /I.',
        '../tests/FadeTransitionRegression.cpp',
        '/Fo../artifacts/FadeTransitionRegression.obj',
        '/Fe../x64/Debug/FadeTransitionRegression.exe')
    $arguments += Get-ChildItem D2DGameFramework/x64/Debug -Filter '*.obj' |
        Where-Object { $_.Name -ne 'main.obj' } |
        ForEach-Object { '"x64/Debug/' + $_.Name + '"' }
    $arguments += '/link /SUBSYSTEM:CONSOLE kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib windowscodecs.lib'
    Set-Content artifacts/FadeTransitionRegression.rsp -Value $arguments -Encoding utf8
    Set-Content artifacts/build-fade-regression.cmd -Encoding ascii -Value @(
        '@echo off', ('call "' + $vcvars + '" >nul'),
        'if errorlevel 1 exit /b 1', 'cl @..\artifacts\FadeTransitionRegression.rsp')
    Push-Location D2DGameFramework
    try {
        cmd /c ..\artifacts\build-fade-regression.cmd
        if ($LASTEXITCODE -ne 0) { throw 'Regression build failed.' }
        & ../x64/Debug/FadeTransitionRegression.exe
        if ($LASTEXITCODE -ne 0) { throw 'Regression failed.' }
    } finally { Pop-Location }
} finally { Pop-Location }
