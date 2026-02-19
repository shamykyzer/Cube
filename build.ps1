param(
  [ValidateSet("clang","gcc","cl","auto")]
  [string]$Compiler = "auto",
  [switch]$Run
)

$ErrorActionPreference = "Stop"

Set-Location -Path $PSScriptRoot

function Resolve-Exe([string]$name) {
  $cmd = Get-Command $name -ErrorAction SilentlyContinue
  if ($null -ne $cmd) { return $cmd.Source }

  $candidates = @()
  switch ($name) {
    "clang" {
      $candidates += @(
        (Join-Path $env:ProgramFiles "LLVM\\bin\\clang.exe"),
        (Join-Path ${env:ProgramFiles(x86)} "LLVM\\bin\\clang.exe")
      )
    }
    "gcc" {
      $candidates += @(
        "C:\\msys64\\ucrt64\\bin\\gcc.exe",
        "C:\\msys64\\mingw64\\bin\\gcc.exe"
      )
    }
  }

  foreach ($p in $candidates) {
    if ($p -and (Test-Path $p)) { return $p }
  }
  return $null
}

function Pick-Compiler([string]$pref) {
  if ($pref -ne "auto") {
    $exe = Resolve-Exe $pref
    if ($null -eq $exe) { throw "Compiler '$pref' not found (PATH or common install locations)." }
    return @{ Name = $pref; Exe = $exe }
  }

  $isWin = ($env:OS -eq "Windows_NT")
  $order = if ($isWin) { @("gcc","clang","cl") } else { @("clang","gcc","cl") }
  foreach ($c in $order) {
    $exe = Resolve-Exe $c
    if ($null -ne $exe) { return @{ Name = $c; Exe = $exe } }
  }
  throw "No C compiler found (tried clang, gcc, cl). Install one, then re-run."
}

$choice = Pick-Compiler $Compiler
$cName = $choice.Name
$cExe = $choice.Exe
$out = "cube.exe"

Write-Host "Using compiler: $cName"

if ($cName -eq "cl") {
  & $cExe /nologo /O2 /W3 /Fe:$out cube.c
} elseif ($cName -eq "gcc") {
  # MSYS2 MinGW toolchains keep runtime DLLs in the toolchain bin dir.
  # When invoked from plain PowerShell, that dir may not be on PATH, causing cc1/ld to fail to start.
  $toolchainBin = Split-Path -Parent $cExe
  if ($toolchainBin -and (Test-Path $toolchainBin)) {
    $env:PATH = "$toolchainBin;$env:PATH"
  }
  & $cExe cube.c -O2 -std=c11 -o $out
} else {
  & $cExe cube.c -O2 -std=c11 -o $out
}

if ($LASTEXITCODE -ne 0) {
  throw "Build failed (exit code $LASTEXITCODE)."
}

Write-Host "Built $out"

if ($Run) {
  & (Join-Path $PSScriptRoot $out)
}
