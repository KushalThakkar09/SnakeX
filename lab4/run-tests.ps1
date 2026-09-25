param([ValidateSet('baseline', 'seam', 'all')][string]$Suite = 'all')
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$buildRoot = Join-Path $PSScriptRoot 'build'
$utf8 = New-Object System.Text.UTF8Encoding($false)

function Invoke-Checked([string]$program, [string[]]$arguments) {
    & $program @arguments
    if ($LASTEXITCODE -ne 0) { throw "$program failed with exit code $LASTEXITCODE" }
}

function Run-Suite([string]$name) {
    $targetDir = Join-Path $buildRoot $name
    New-Item -ItemType Directory -Force $targetDir | Out-Null
    # Remove only coverage counters from this exact suite directory, so reruns do not accumulate.
    Get-ChildItem -LiteralPath $targetDir -Filter '*.gcda' | Remove-Item
    Push-Location $repoRoot
    try {
        if ($name -eq 'baseline') {
            $baseLines = @(git -c "safe.directory=$($repoRoot.Replace('\', '/'))" show 'lab4-base:code/game.cpp')
            if ($LASTEXITCODE -ne 0) { throw 'Cannot read lab4-base:code/game.cpp' }
            $baseFile = Join-Path $targetDir 'base-game.cpp'
            [IO.File]::WriteAllLines($baseFile, [string[]]$baseLines, $utf8)
            if ($baseLines[298] -ne 'class Snake {' -or $baseLines[370] -ne 'class GameBoard {' -or $baseLines[429] -ne 'class Game {') {
                throw 'Unexpected baseline layout; review the component boundaries before continuing.'
            }
            # No rewritten logic: concatenate exact source ranges and retain original line numbers.
            # The broken unused multiplayer definition and interactive Game/main are excluded.
            $coveragePath = $baseFile.Replace('\', '/')
            $header = @("#line 1 `"$coveragePath`"") + $baseLines[0..353] +
                      @("#line 371 `"$coveragePath`"") + $baseLines[370..427]
            [IO.File]::WriteAllLines((Join-Path $targetDir 'baseline_components.h'), [string[]]$header, $utf8)
        }
    } finally { Pop-Location }
    Push-Location $targetDir
    try {
        $testFile = Join-Path $PSScriptRoot "tests/$name.cpp"
        Invoke-Checked 'g++' @('-std=c++17', '--coverage', '-O0', '-g', '-Wall', '-Wextra', '-I.', '-c', $testFile, '-o', 'tests.o')
        Invoke-Checked 'g++' @('--coverage', 'tests.o', '-o', 'tests.exe')
        & ./tests.exe 2>&1 | Tee-Object 'test-results.txt'
        if ($LASTEXITCODE -ne 0) { throw "$name tests failed" }
        & gcov -b -c tests.gcno 2>&1 | Tee-Object 'coverage-summary.txt'
        if ($LASTEXITCODE -ne 0) { throw "$name coverage failed" }
    } finally { Pop-Location }
}

if ($Suite -eq 'all') { Run-Suite 'baseline'; Run-Suite 'seam' }
else { Run-Suite $Suite }
