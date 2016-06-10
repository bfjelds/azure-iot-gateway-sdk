@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0

rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-config=Debug
set build-platform=Win32
set CMAKE_run_e2e_tests=OFF
set CMAKE_enable_dotnet_binding=OFF

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--run-e2e-tests" goto arg-run-e2e-tests
if "%1" equ "--enable-dotnet-binding" goto arg-enable_dotnet_binding
call :usage && exit /b 1

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:arg-build-platform
shift
if "%1" equ "" call :usage && exit /b 1
set build-platform=%1
goto args-continue

:arg-run-e2e-tests
set CMAKE_run_e2e_tests=ON
goto args-continue

:arg-enable_dotnet_binding
set CMAKE_enable_dotnet_binding=ON
goto args-continue


:args-continue
shift
goto args-loop

:args-done

rem -----------------------------------------------------------------------------
rem -- build with CMAKE and run tests
rem -----------------------------------------------------------------------------

rem this is setting the cmake path in a quoted way
set cmake-root="%build-root%"\build

rem get the size of cmake path
set cmake-root-no-quotes=%build-root%\build
set size=0
:loop
if defined cmake-root-no-quotes (
    rem chop down one character
    set cmake-root-no-quotes=%cmake-root-no-quotes:~1%
    rem add it to the size
    set /A size += 1
    rem go to begin
    goto loop
)

rem echo %build-root%\build (%cmake-root%)is %size% characters long!
if %size% GTR 49 (
    @echo build.cmd cannot continue because the path %build-root%\build is too long!
    @echo try placing the repo in a shorter path. the path size can be at most 49 characters
    exit /b 1
)

rmdir /s/q %cmake-root%

mkdir %cmake-root%
rem no error checking

pushd %cmake-root%
cmake -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Denable_dotnet_binding:BOOL=%CMAKE_enable_dotnet_binding% "%build-root%" "%build-root%"
if not %errorlevel%==0 exit /b %errorlevel%

msbuild /m /p:Configuration="%build-config%" /p:Platform="%build-platform%" azure_iot_gateway_sdk.sln
if not %errorlevel%==0 exit /b %errorlevel%

ctest -C "debug" -V
if not %errorlevel%==0 exit /b %errorlevel%

popd
goto :eof

rem -----------------------------------------------------------------------------
rem -- subroutines
rem -----------------------------------------------------------------------------

:usage
echo build.cmd [options]
echo options:
echo  --config ^<value^>         [Debug] build configuration (e.g. Debug, Release)
echo  --platform ^<value^>       [Win32] build platform (e.g. Win32, x64, ...)
echo  --run-e2e-tests            run end-to-end tests
echo  --enable-dotnet-binding    enable dotnet binding (Windows Only)
goto :eof

