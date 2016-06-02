$ErrorActionPreference = "Stop"

$packageName = "dide_mod"
$packageVersion = &git rev-parse --short HEAD

Add-Type -Assembly System.IO.Compression.FileSystem

$tempRootDir = "./ReleasePackageTemp"
$tempWorkDir = Join-Path -Path $tempRootDir -ChildPath "work"
$tempOutputDir = Join-Path -Path $tempRootDir -ChildPath "output"
if (-not (Test-Path -Path $tempWorkDir)) {
    New-Item -ItemType Directory -Path $tempWorkDir
}

if (-not (Test-Path -Path $tempOutputDir)) {
    New-Item -ItemType Directory -Path $tempOutputDir
}

Remove-Item -Path "${tempOutputDir}/*"
Remove-Item -Path "${tempWorkDir}/*"


$packageOutputPath = Join-Path -Path $tempOutputDir -ChildPath "${packageName}-${packageVersion}-bin.zip"
Copy-Item -Path "bin/Release/*" -Include ("*.dll", "*.exe") -Destination $tempWorkDir
Copy-Item -Path "bundled/*" -Destination $tempWorkDir
[System.IO.Compression.ZipFile]::CreateFromDirectory($tempWorkDir, $packageOutputPath)
Remove-Item -Path "${tempWorkDir}/*"


$packageOutputPath = Join-Path -Path $tempOutputDir -ChildPath "${packageName}-${packageVersion}-artifacts.zip"
Copy-Item -Path "bin/Release/*" -Destination $tempWorkDir
[System.IO.Compression.ZipFile]::CreateFromDirectory($tempWorkDir, $packageOutputPath)
Remove-Item -Path "${tempWorkDir}/*"
