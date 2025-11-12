# Raspberry Pi SD Card Preparation Script
# Run this script as Administrator

# Configuration
$SD_DRIVE = "D:"  # CHANGE THIS to your SD card drive letter
$TEMP_DIR = "$env:TEMP\rpi-boot"

# Verify SD card drive exists
if (-not (Test-Path $SD_DRIVE)) {
    Write-Host "ERROR: Drive $SD_DRIVE not found!" -ForegroundColor Red
    Write-Host "Please change the SD_DRIVE variable to match your SD card drive letter" -ForegroundColor Yellow
    pause
    exit 1
}

# Warning message
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "WARNING: This will format drive $SD_DRIVE" -ForegroundColor Red
Write-Host "All data on this drive will be ERASED!" -ForegroundColor Red
Write-Host "========================================" -ForegroundColor Yellow
Write-Host ""
$confirm = Read-Host "Type 'YES' to continue"
if ($confirm -ne "YES") {
    Write-Host "Operation cancelled." -ForegroundColor Yellow
    exit 0
}

# Format SD card
Write-Host "`nFormatting SD card as FAT32..." -ForegroundColor Cyan
try {
    $driveLetter = $SD_DRIVE.TrimEnd(':')
    Format-Volume -DriveLetter $driveLetter -FileSystem FAT32 -NewFileSystemLabel "BOOT" -Force -Confirm:$false
    Write-Host "[OK] SD card formatted successfully" -ForegroundColor Green
} catch {
    Write-Host "ERROR: Failed to format SD card: $_" -ForegroundColor Red
    pause
    exit 1
}

# Create temporary directory
Write-Host "`nCreating temporary directory..." -ForegroundColor Cyan
if (-not (Test-Path $TEMP_DIR)) {
    New-Item -ItemType Directory -Path $TEMP_DIR -Force | Out-Null
}

# Download firmware files
Write-Host "`nDownloading Raspberry Pi firmware files..." -ForegroundColor Cyan
$firmwareFiles = @{
    "bootcode.bin" = "https://github.com/raspberrypi/firmware/raw/master/boot/bootcode.bin"
    "start.elf" = "https://github.com/raspberrypi/firmware/raw/master/boot/start.elf"
    "fixup.dat" = "https://github.com/raspberrypi/firmware/raw/master/boot/fixup.dat"
}

foreach ($file in $firmwareFiles.GetEnumerator()) {
    Write-Host "  Downloading $($file.Key)..." -ForegroundColor Gray
    try {
        $output = Join-Path $TEMP_DIR $file.Key
        Invoke-WebRequest -Uri $file.Value -OutFile $output -UseBasicParsing
        Write-Host "  [OK] $($file.Key)" -ForegroundColor Green
    } catch {
        Write-Host "  ERROR: Failed to download $($file.Key): $_" -ForegroundColor Red
        pause
        exit 1
    }
}

# Copy firmware files to SD card
Write-Host "`nCopying firmware files to SD card..." -ForegroundColor Cyan
foreach ($file in $firmwareFiles.Keys) {
    $source = Join-Path $TEMP_DIR $file
    $destination = Join-Path $SD_DRIVE $file
    Copy-Item -Path $source -Destination $destination -Force
    Write-Host "  [OK] Copied $file" -ForegroundColor Green
}

# Create config.txt
Write-Host "`nCreating config.txt..." -ForegroundColor Cyan
$configPath = Join-Path $SD_DRIVE "config.txt"
$configContent = @"
arm_64bit=1
kernel=kernel8.img
"@
Set-Content -Path $configPath -Value $configContent
Write-Host "[OK] config.txt created" -ForegroundColor Green

# Copy kernel8.img if it exists in current directory
$kernelSource = ".\kernel8.img"
if (Test-Path $kernelSource) {
    Write-Host "`nCopying kernel8.img..." -ForegroundColor Cyan
    $kernelDest = Join-Path $SD_DRIVE "kernel8.img"
    Copy-Item -Path $kernelSource -Destination $kernelDest -Force
    Write-Host "[OK] kernel8.img copied" -ForegroundColor Green
} else {
    Write-Host "`nWARNING: kernel8.img not found in current directory!" -ForegroundColor Yellow
    Write-Host "Please copy your kernel8.img to $SD_DRIVE manually" -ForegroundColor Yellow
}

# Clean up temporary files
Write-Host "`nCleaning up temporary files..." -ForegroundColor Cyan
Remove-Item -Path $TEMP_DIR -Recurse -Force
Write-Host "[OK] Cleanup complete" -ForegroundColor Green

# Display SD card contents
Write-Host "`n========================================" -ForegroundColor Green
Write-Host "SD Card is ready!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "`nContents of $SD_DRIVE :" -ForegroundColor Cyan
Get-ChildItem -Path $SD_DRIVE | Format-Table Name, Length -AutoSize

Write-Host "`nYou can now:" -ForegroundColor Yellow
Write-Host "1. Safely eject the SD card" -ForegroundColor White
Write-Host "2. Insert it into your Raspberry Pi 3" -ForegroundColor White
Write-Host "3. Connect your LEDs and power on" -ForegroundColor White

pause
