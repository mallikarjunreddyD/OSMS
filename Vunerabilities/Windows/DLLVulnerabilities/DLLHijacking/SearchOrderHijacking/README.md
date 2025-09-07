# DLL Search Order Hijacking

## Overview
DLL Search Order Hijacking is a technique where attackers place malicious DLLs in locations that Windows searches before finding the legitimate DLL. This allows malicious code to be executed when applications load DLLs.

## How It Works

### Windows DLL Search Order
Windows searches for DLLs in this specific order:
1. **Application directory** (where .exe is located)
2. **System directory** (C:\Windows\System32)
3. **16-bit System directory** (C:\Windows\System)
4. **Windows directory** (C:\Windows)
5. **Current directory**
6. **Directories in PATH environment variable**

### Attack Process
1. **Identify Target** - Find application that loads specific DLLs
2. **Create Malicious DLL** - Develop DLL with same exports as legitimate one
3. **Place DLL** - Put malicious DLL in higher priority location
4. **Execute** - Run target application to trigger DLL loading
5. **Persistence** - Malicious code executes with application privileges

## Files in This Directory

### `dll_hijack_demo.cpp`
Malicious DLL that demonstrates:
- DLL entry point (DllMain) execution
- Function export and interception
- Parameter manipulation
- Educational message boxes (safe demonstration)

### `target_application.cpp`
Target application that:
- Loads a common DLL (version.dll)
- Calls functions from the loaded DLL
- Shows DLL information and loaded modules
- Demonstrates the hijacking process

## Usage

### Step 1: Compile the Malicious DLL
```bash
g++ -shared -o malicious.dll dll_hijack_demo.cpp
```

### Step 2: Compile the Target Application
```bash
g++ -o target_app target_application.cpp -lpsapi
```

### Step 3: Set Up the Hijacking
```bash
# Rename the malicious DLL to match what the target loads
copy malicious.dll version.dll

# Place it in the same directory as the target application
# (The target application directory)
```

### Step 4: Run the Demonstration
```bash
# Run the target application
./target_app.exe
```

## Expected Results

### Without Hijacking
- Application fails to load version.dll
- Error message about missing DLL
- Instructions for setting up hijacking

### With Hijacking
- Message box appears: "DLL HIJACKED!"
- Malicious DLL executes instead of legitimate one
- Function calls are intercepted and modified
- Educational information about the attack

## Common DLL Hijacking Targets

### High-Value Targets
- **version.dll** - Version information (loaded by many apps)
- **winmm.dll** - Windows multimedia
- **winspool.drv** - Print spooler
- **wtsapi32.dll** - Terminal services
- **userenv.dll** - User environment
- **cryptsp.dll** - Cryptographic service provider
- **dpapi.dll** - Data protection API
- **netapi32.dll** - Network API

### Why These Are Targeted
- Loaded by many applications
- Often not digitally signed
- Located in system directories
- Provide useful functionality for attackers

## Attack Techniques

### 1. Search Order Hijacking
- Place malicious DLL in application directory
- Exploit search order priority
- Most common technique

### 2. Side-Loading
- Place malicious DLL alongside legitimate executable
- Use same name as legitimate DLL
- Often used with legitimate software

### 3. DLL Proxying
- Create proxy DLL that calls original functions
- Add malicious code while maintaining functionality
- Harder to detect

### 4. DLL Replacement
- Replace legitimate DLL with malicious version
- Requires higher privileges
- Most persistent but detectable

## Detection Methods

### 1. DLL Monitoring
- Monitor DLL loading events using Process Monitor
- Check DLL file hashes and digital signatures
- Use Windows Event Logs for DLL loading events

### 2. File System Monitoring
- Monitor for new DLL files in application directories
- Alert on suspicious DLL modifications
- Use Windows File System Filter drivers

### 3. Process Monitoring
- Monitor process behavior after DLL loading
- Check for unusual network activity
- Monitor registry and file system changes

### 4. Memory Analysis
- Analyze loaded DLLs in memory
- Check for code injection or modification
- Use tools like Volatility or WinDbg

## Mitigation Strategies

### 1. DLL Signing
- Require all DLLs to be digitally signed
- Use Windows Defender Application Control
- Implement code integrity policies

### 2. Path Restrictions
- Restrict DLL loading to specific directories
- Use AppLocker or similar tools
- Implement least privilege access controls

### 3. Monitoring and Alerting
- Implement real-time DLL monitoring
- Set up alerts for suspicious DLL activity
- Use SIEM solutions for correlation

### 4. Application Hardening
- Use static linking where possible
- Implement DLL load verification
- Regular security updates and patches

## Real-World Examples

### Common Attack Scenarios
- **Malware Persistence** - Maintaining access across reboots
- **Privilege Escalation** - Gaining higher privileges
- **Credential Theft** - Intercepting authentication
- **Data Exfiltration** - Stealing sensitive information
- **Backdoor Installation** - Creating persistent access

### Advanced Techniques
- **DLL Unhooking** - Bypassing security hooks
- **Reflective DLL Loading** - Loading DLLs from memory
- **DLL Hollowing** - Replacing legitimate DLLs in memory
- **Process Injection** - Injecting DLLs into running processes

## Security Considerations

### Why This Attack is Effective
- **Stealth** - Often undetected by antivirus
- **Persistence** - Survives reboots and updates
- **Privilege** - Runs with application privileges
- **Reliability** - Built into Windows DLL loading

### Defense in Depth
- Combine multiple detection methods
- Implement layered security controls
- Regular security awareness training
- Continuous monitoring and response

## Educational Value

This demonstration helps understand:
- Windows DLL loading mechanism
- Search order vulnerabilities
- Function interception techniques
- Detection and mitigation strategies
- Programmatic DLL manipulation

## Warning

⚠️ **This is for educational purposes only!**
- Do not use these techniques for malicious purposes
- Always test in isolated environments
- Respect applicable laws and regulations
- Use responsibly for security research and education
