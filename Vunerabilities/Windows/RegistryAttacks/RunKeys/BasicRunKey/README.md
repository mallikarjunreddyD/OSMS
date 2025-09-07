# Registry Run Key Attack

## Overview
Registry Run Keys are a common persistence mechanism used by malware and attackers to maintain access to compromised systems. These registry keys automatically execute programs when a user logs in or the system starts.

## How It Works

### Registry Locations
- **HKCU\Software\Microsoft\Windows\CurrentVersion\Run** - Current user only
- **HKLM\Software\Microsoft\Windows\CurrentVersion\Run** - All users (requires admin)
- **HKCU\Software\Microsoft\Windows\CurrentVersion\RunOnce** - Run once then remove
- **HKLM\Software\Microsoft\Windows\CurrentVersion\RunOnce** - System-wide RunOnce

### Attack Process
1. **Gain Access** - Attacker gains access to the system
2. **Add Entry** - Add malicious executable path to Run key
3. **Persistence** - Malicious code runs automatically on login/startup
4. **Maintain Access** - Provides persistent access across reboots

## Files in This Directory

### `run_key_demo.ps1`
PowerShell script that demonstrates:
- How to view current Run key entries
- How attackers use Run keys for persistence
- Detection methods and mitigation strategies
- Safe demonstration without making actual changes

### `run_key_manipulation.cpp`
C++ program that demonstrates:
- Programmatic manipulation of Run keys
- Adding malicious entries (using notepad.exe for safety)
- Listing and removing entries
- Different Run key locations and their purposes

## Usage

### PowerShell Demo
```powershell
# Run the PowerShell demonstration
.\run_key_demo.ps1
```

### C++ Demo
```bash
# Compile the C++ program
g++ -o run_key_demo run_key_manipulation.cpp -ladvapi32

# Run the demonstration
./run_key_demo
```

## Detection Methods

### 1. Registry Monitoring
- Monitor Run key changes using tools like Sysmon
- Set up Windows Event Log monitoring
- Use SIEM solutions for automated detection

### 2. Process Monitoring
- Monitor processes started from Run keys
- Check for unusual process behavior
- Verify process signatures and file hashes

### 3. Manual Inspection
- Regularly check Run key entries
- Look for suspicious executable paths
- Verify legitimate entries

## Mitigation Strategies

### 1. Registry Permissions
- Restrict write access to Run keys using Group Policy
- Implement least privilege access controls
- Monitor registry permission changes

### 2. Application Whitelisting
- Use AppLocker or Windows Defender Application Control
- Block execution of unauthorized applications
- Implement code signing requirements

### 3. Monitoring and Alerting
- Implement SIEM solutions
- Set up alerts for Run key modifications
- Conduct regular security audits

## Real-World Examples

### Common Malicious Entries
- **Name**: "WindowsUpdate"
- **Value**: "C:\Windows\System32\malware.exe"
- **Disguise**: Appears as legitimate Windows Update process

### Advanced Techniques
- **Path Obfuscation**: Using environment variables or short paths
- **Legitimate Process Abuse**: Replacing legitimate executables
- **Multiple Persistence**: Using multiple Run key locations

## Security Considerations

### Why This Attack is Effective
- **Persistence**: Survives reboots and user logouts
- **Stealth**: Often disguised as legitimate processes
- **Accessibility**: HKCU Run key doesn't require admin rights
- **Reliability**: Built into Windows startup process

### Defense in Depth
- Combine multiple detection methods
- Implement layered security controls
- Regular security awareness training
- Continuous monitoring and response

## Educational Value

This demonstration helps understand:
- How Windows startup process works
- Registry structure and manipulation
- Persistence mechanisms used by malware
- Detection and mitigation strategies
- Programmatic registry manipulation

## Warning

⚠️ **This is for educational purposes only!**
- Do not use these techniques for malicious purposes
- Always test in isolated environments
- Respect applicable laws and regulations
- Use responsibly for security research and education
