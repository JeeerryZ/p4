# ---------------------------------------------------------------------------
# P4 Windows Installer
# Builds P4 from source and assembles a self-contained dist\ directory.
#
# Maple scripts and help files are embedded in p4.exe via Qt resources
# and extracted automatically at runtime — no separate files needed.
#
# Prerequisites — the following must be on your PATH before running:
#   qmake.exe        (Qt 6.x MinGW)
#   mingw32-make.exe (MinGW)
#   windeployqt.exe  (Qt 6.x MinGW — for DLL deployment)
#
# Example (adjust to your Qt install location):
#   $env:PATH = "E:\Qt\Tools\mingw1310_64\bin;E:\Qt\6.9.3\mingw_64\bin;$env:PATH"
#
# Usage:
#   .\install_win.ps1             # build + assemble dist\
#   .\install_win.ps1 -Clean      # remove build\ and dist\
# ---------------------------------------------------------------------------
param([switch]$Clean)

$Root      = $PSScriptRoot
$BuildDir  = "$Root\build"
$DistDir   = "$Root\dist"

function Write-Step  { param($msg) Write-Host "  * $msg" -ForegroundColor Cyan }
function Write-Ok    { param($msg) Write-Host "  OK  $msg" -ForegroundColor Green }
function Write-Fail  { param($msg) Write-Host "  FAIL  $msg" -ForegroundColor Red; exit 1 }

Write-Host ""
Write-Host "  P4 Windows Installer" -ForegroundColor Cyan
Write-Host "  ====================" -ForegroundColor Cyan
Write-Host ""

if ($Clean) {
    Write-Step "Cleaning build\ and dist\ ..."
    Remove-Item -Recurse -Force $BuildDir, $DistDir -ErrorAction SilentlyContinue
    Write-Ok "Done."
    exit 0
}

# ── Check tools ───────────────────────────────────────────────────────────────
Write-Step "Checking qmake"
if (-not (Get-Command qmake -ErrorAction SilentlyContinue)) { Write-Fail "qmake not found. Check QtBin path in this script." }
Write-Ok "qmake  $((qmake --version) -join ' ')"

Write-Step "Checking mingw32-make"
if (-not (Get-Command mingw32-make -ErrorAction SilentlyContinue)) { Write-Fail "mingw32-make not found. Check MinGWBin path in this script." }
Write-Ok "mingw32-make found"

# ── Build ─────────────────────────────────────────────────────────────────────
Write-Host ""
Write-Host "  [ Phase 1/2 ]  Building" -ForegroundColor Cyan

Write-Step "Running qmake"
Set-Location $Root
& qmake -r P4.pro 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) { Write-Fail "qmake failed" }
Write-Ok "qmake configured"

# windres only depends on p4.rc, not the .ico files it references, so an
# icon-only change wouldn't otherwise trigger a rebuild of the embedded
# resource. Force it by removing any stale compiled resource object.
Remove-Item -Force -ErrorAction SilentlyContinue `
    "$Root\src-gui\p4\release\p4_res.o", "$Root\src-gui\p4\debug\p4_res.o"

$Jobs = [Environment]::ProcessorCount
Write-Step "Compiling ($Jobs jobs)"
$buildLog = "$BuildDir\make.log"
& mingw32-make "-j$Jobs" 2>&1 | Tee-Object $buildLog | Where-Object { $_ -match "error:" } | ForEach-Object { Write-Host $_ -ForegroundColor Red }
$makeExit = $LASTEXITCODE
if ($makeExit -ne 0) { Write-Fail "Build failed -- see $buildLog" }
Write-Ok "Build complete"

# ── Assemble dist\ ────────────────────────────────────────────────────────────
Write-Host ""
Write-Host "  [ Phase 2/2 ]  Assembling dist\" -ForegroundColor Cyan

New-Item -ItemType Directory -Force $DistDir | Out-Null

foreach ($bin in @("p4", "lyapunov", "lyapunov_mpf", "separatrice")) {
    $src = "$BuildDir\$bin\$bin.exe"
    $dst = "$DistDir\$bin.exe"
    Write-Step "Copying $bin.exe"
    if (-not (Test-Path $src)) { Write-Fail "$src not found -- did compilation succeed?" }

    # Windows locks a running executable, so copying over it fails.  Without
    # this check the script reported success and left dist\ holding the
    # previous build, which is indistinguishable from a build that did nothing.
    try {
        Copy-Item $src $dst -Force -ErrorAction Stop
    } catch {
        $running = Get-Process -Name $bin -ErrorAction SilentlyContinue
        if ($running) {
            Write-Fail "$bin.exe is running (PID $($running.Id -join ', ')) -- close it and re-run"
        }
        Write-Fail "could not copy $bin.exe to dist\: $($_.Exception.Message)"
    }

    # Copy-Item can report success and still leave an older file behind, so
    # compare what actually landed in dist\.
    if ((Get-Item $dst).LastWriteTimeUtc -lt (Get-Item $src).LastWriteTimeUtc) {
        Write-Fail "$bin.exe in dist\ is older than the one just built -- copy did not take effect"
    }
    Write-Ok "$bin.exe"
}

# lyapunov(.exe) caches its summation tables (sum1.tab, sum2.tab, ...) here and
# generates them on first use.  Without the directory the table write fails, the
# binary aborts leaving a 0-byte result file, and Maple reports
# "Error (readlyapunovresult) invalid subscript selector".
Write-Step "Creating sumtables\"
New-Item -ItemType Directory -Force "$DistDir\sumtables" | Out-Null
Write-Ok "sumtables\"

# Run windeployqt to copy Qt DLLs
Write-Step "Running windeployqt"
$WinDeploy = (Get-Command windeployqt -ErrorAction SilentlyContinue)?.Source
if ($WinDeploy) {
    & $WinDeploy "$DistDir\p4.exe" --no-translations 2>&1 | Out-Null
    Write-Ok "Qt DLLs deployed"
} else {
    Write-Host "  WARN  windeployqt not found on PATH -- copy Qt DLLs manually" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "  ======================================" -ForegroundColor Green
Write-Host "  P4 installed -> $DistDir"              -ForegroundColor Green
Write-Host "  p4.exe  lyapunov.exe  lyapunov_mpf.exe  separatrice.exe" -ForegroundColor Green
Write-Host "  ======================================" -ForegroundColor Green
Write-Host ""
