$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$mutationRoot = Join-Path $PSScriptRoot 'build/mutations'
New-Item -ItemType Directory -Force $mutationRoot | Out-Null
$utf8 = New-Object System.Text.UTF8Encoding($false)
$source = [IO.File]::ReadAllText((Join-Path $repoRoot 'code/game.cpp'))
$test = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'tests/seam.cpp'))
$checks = (Join-Path $PSScriptRoot 'tests/check.h').Replace('\', '/')
$mutations = @(
    @{Name='missing-score'; From='progress.score++;'; To='/* score increment removed */'},
    @{Name='double-score'; From='progress.score++;'; To='progress.score += 2;'},
    @{Name='missing-growth'; From='        snake.grow();'; To='        /* growth removed */'},
    @{Name='missing-game-over'; From='progress.gameOver = true;'; To='progress.gameOver = false;'},
    @{Name='missing-delay-clamp'; From='max(minSpeedMs, progress.speedMs - speedStep)'; To='progress.speedMs - speedStep'},
    @{Name='missing-high-score'; From='if (progress.score > progress.highScore) progress.highScore = progress.score;'; To='/* high score update removed */'}
)
$results = @()
foreach ($mutation in $mutations) {
    if (-not $source.Contains($mutation.From)) { throw "Missing mutation site: $($mutation.Name)" }
    $mutantSource = Join-Path $mutationRoot "$($mutation.Name).cpp"
    $mutantTest = Join-Path $mutationRoot "$($mutation.Name)-test.cpp"
    $exe = Join-Path $mutationRoot "$($mutation.Name).exe"
    [IO.File]::WriteAllText($mutantSource, $source.Replace($mutation.From, $mutation.To), $utf8)
    $mutantText = $test.Replace('../../code/game.cpp', $mutantSource.Replace('\', '/')).Replace('"check.h"', ('"' + $checks + '"'))
    [IO.File]::WriteAllText($mutantTest, $mutantText, $utf8)
    & g++ -std=c++17 -O0 $mutantTest -o $exe
    if ($LASTEXITCODE -ne 0) { throw "Mutation failed to compile (not counted as killed): $($mutation.Name)" }
    # Windows PowerShell wraps expected native stderr in ErrorRecords. Capture it
    # without terminating the script, then judge only the exit code and FAIL lines.
    $previousErrorPreference = $ErrorActionPreference
    try {
        $ErrorActionPreference = 'Continue'
        $output = @(& $exe 2>&1)
        $status = $LASTEXITCODE
    } finally { $ErrorActionPreference = $previousErrorPreference }
    $output | Set-Content (Join-Path $mutationRoot "$($mutation.Name).txt")
    if ($status -ne 1 -or -not ($output -match '^FAIL ')) { throw "Mutant survived or crashed: $($mutation.Name), exit $status" }
    $results += "KILLED $($mutation.Name) (compiled successfully; behavioral assertion failed)"
}
$results | Tee-Object (Join-Path $mutationRoot 'results.txt')
