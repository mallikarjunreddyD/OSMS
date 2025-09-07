# Registry Run Key Attack Demonstration
# This script demonstrates how malicious actors use Windows Registry Run keys for persistence
# WARNING: This is for educational purposes only!

Write-Host "=== Registry Run Key Attack Demonstration ===" -ForegroundColor Red
Write-Host "This demonstrates how Run Keys can be used for persistence" -ForegroundColor Yellow
Write-Host ""

# Function to demonstrate Run Key manipulation
function Show-RunKeyAttack {
    Write-Host "1. Current Run Key Entries:" -ForegroundColor Green
    Write-Host "   HKCU Run Key (Current User):" -ForegroundColor Cyan
    Get-ItemProperty -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run" -ErrorAction SilentlyContinue | 
        Format-Table -AutoSize
    
    Write-Host "   HKLM Run Key (Local Machine):" -ForegroundColor Cyan
    Get-ItemProperty -Path "HKLM:\Software\Microsoft\Windows\CurrentVersion\Run" -ErrorAction SilentlyContinue | 
        Format-Table -AutoSize
    
    Write-Host ""
    Write-Host "2. How Attackers Use Run Keys:" -ForegroundColor Green
    Write-Host "   - Add malicious executable to Run key" -ForegroundColor White
    Write-Host "   - Executable runs automatically on user login" -ForegroundColor White
    Write-Host "   - Provides persistence across reboots" -ForegroundColor White
    Write-Host "   - Often disguised as legitimate system processes" -ForegroundColor White
    
    Write-Host ""
    Write-Host "3. Common Run Key Locations:" -ForegroundColor Green
    Write-Host "   HKCU:\Software\Microsoft\Windows\CurrentVersion\Run" -ForegroundColor White
    Write-Host "   HKLM:\Software\Microsoft\Windows\CurrentVersion\Run" -ForegroundColor White
    Write-Host "   HKCU:\Software\Microsoft\Windows\CurrentVersion\RunOnce" -ForegroundColor White
    Write-Host "   HKLM:\Software\Microsoft\Windows\CurrentVersion\RunOnce" -ForegroundColor White
    
    Write-Host ""
    Write-Host "4. Example Malicious Entry (NOT EXECUTED):" -ForegroundColor Green
    Write-Host "   Name: 'WindowsUpdate'" -ForegroundColor White
    Write-Host "   Value: 'C:\Windows\System32\notepad.exe'" -ForegroundColor White
    Write-Host "   (Disguised as legitimate Windows Update process)" -ForegroundColor Gray
}

# Function to show detection methods
function Show-DetectionMethods {
    Write-Host ""
    Write-Host "=== Detection Methods ===" -ForegroundColor Red
    
    Write-Host "1. Registry Monitoring:" -ForegroundColor Green
    Write-Host "   - Monitor Run key changes" -ForegroundColor White
    Write-Host "   - Alert on suspicious entries" -ForegroundColor White
    Write-Host "   - Use tools like Sysmon or Windows Event Logs" -ForegroundColor White
    
    Write-Host ""
    Write-Host "2. Process Monitoring:" -ForegroundColor Green
    Write-Host "   - Monitor processes started from Run keys" -ForegroundColor White
    Write-Host "   - Check for unusual process behavior" -ForegroundColor White
    Write-Host "   - Verify process signatures" -ForegroundColor White
    
    Write-Host ""
    Write-Host "3. Manual Inspection:" -ForegroundColor Green
    Write-Host "   - Regularly check Run key entries" -ForegroundColor White
    Write-Host "   - Look for suspicious executable paths" -ForegroundColor White
    Write-Host "   - Verify legitimate entries" -ForegroundColor White
}

# Function to show mitigation strategies
function Show-MitigationStrategies {
    Write-Host ""
    Write-Host "=== Mitigation Strategies ===" -ForegroundColor Red
    
    Write-Host "1. Registry Permissions:" -ForegroundColor Green
    Write-Host "   - Restrict write access to Run keys" -ForegroundColor White
    Write-Host "   - Use Group Policy to control Run key access" -ForegroundColor White
    
    Write-Host ""
    Write-Host "2. Application Whitelisting:" -ForegroundColor Green
    Write-Host "   - Use AppLocker or Windows Defender Application Control" -ForegroundColor White
    Write-Host "   - Block execution of unauthorized applications" -ForegroundColor White
    
    Write-Host ""
    Write-Host "3. Monitoring and Alerting:" -ForegroundColor Green
    Write-Host "   - Implement SIEM solutions" -ForegroundColor White
    Write-Host "   - Set up alerts for Run key modifications" -ForegroundColor White
    Write-Host "   - Regular security audits" -ForegroundColor White
}

# Main execution
Show-RunKeyAttack
Show-DetectionMethods
Show-MitigationStrategies

Write-Host ""
Write-Host "=== Demonstration Complete ===" -ForegroundColor Red
Write-Host "This was a safe demonstration - no actual changes were made to your system" -ForegroundColor Green
