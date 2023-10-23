@echo off

if not exist build (
    mkdir build
    pushd build
        cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/OX" ..
        copy compile_commands.json ..
    popd
)

pushd build
    cmake --build . || (
        echo "Build failed"
        exit /b 1
    )
    copy compile_commands.json ..
    
    set OS_NAME=
    set OS_VERSION=
    set CPU_NAME=

    REM Capture OS Name
    for /f "tokens=2*" %%i in ('systeminfo ^| findstr /B /C:"OS Name"') do (
        set OS_NAME=%%j
    )

    REM Capture OS Version
    for /f "tokens=2*" %%i in ('systeminfo ^| findstr /B /C:"OS Version"') do (
        set OS_VERSION=%%j
    )

    REM Capture CPU Name
    for /f "tokens=2 delims==" %%i in ('wmic cpu get name /value') do (
        set CPU_NAME=%%i
    )

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