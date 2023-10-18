@echo off

if not exist build (
    mkdir build
    pushd build
        cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
        copy compile_commands.json ..
    popd
)

@REM set OS_NAME=
@REM set OS_VERSION=
@REM set CPU_NAME=

@REM REM Capture OS Name
@REM for /f "tokens=2*" %%i in ('systeminfo ^| findstr /B /C:"OS Name"') do (
@REM     set OS_NAME=%%j
@REM )

@REM REM Capture OS Version
@REM for /f "tokens=2*" %%i in ('systeminfo ^| findstr /B /C:"OS Version"') do (
@REM     set OS_VERSION=%%j
@REM )

@REM REM Capture CPU Name
@REM for /f "tokens=2 delims==" %%i in ('wmic cpu get name /value') do (
@REM     set CPU_NAME=%%i
@REM )

pushd build
    cmake --build . || (
        echo "Build failed"
        exit /b 1
    )
    copy compile_commands.json ..
    


    echo # Benchmark Results > BENCHMARKS.md
    echo. >> BENCHMARKS.md
    
    echo ## System Info >> BENCHMARKS.md
    echo CPU: %CPU_NAME% >> BENCHMARKS.md
    echo OS: %OS_NAME% >> BENCHMARKS.md
    echo OS Version: %OS_VERSION% >> BENCHMARKS.md

    echo. >> BENCHMARKS.md

    echo ## SSE 4.2 >> BENCHMARKS.md
    vectormathbench_sse42.exe >> BENCHMARKS.md

    echo. >> BENCHMARKS.md
    echo ## AVX >> BENCHMARKS.md
    vectormathbench_avx.exe >> BENCHMARKS.md

    echo. >> BENCHMARKS.md
    echo ## AVX2 >> BENCHMARKS.md
    vectormathbench_avx2.exe >> BENCHMARKS.md

popd