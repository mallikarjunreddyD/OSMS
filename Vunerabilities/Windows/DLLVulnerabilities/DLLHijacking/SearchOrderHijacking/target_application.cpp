#include <windows.h>
#include <iostream>
#include <string>

// Target Application for DLL Hijacking Demonstration
// This application loads a DLL that can be hijacked
// WARNING: This is for educational purposes only!

// Function pointer for the DLL function we want to call
typedef void (*LegitimateFunctionPtr)();
typedef int (*AnotherFunctionPtr)(int);

int main() {
    std::cout << "=== Target Application for DLL Hijacking Demo ===" << std::endl;
    std::cout << "This application will attempt to load a DLL" << std::endl;
    std::cout << "If a malicious DLL is present, it will be loaded instead!" << std::endl;
    
    // Try to load the DLL
    HMODULE hDll = LoadLibraryA("version.dll"); // Common DLL that can be hijacked
    
    if (hDll == NULL) {
        std::cout << "\n❌ Failed to load version.dll" << std::endl;
        std::cout << "Error: " << GetLastError() << std::endl;
        std::cout << "\nThis is normal if the DLL doesn't exist." << std::endl;
        std::cout << "To test DLL hijacking:" << std::endl;
        std::cout << "1. Compile the malicious DLL" << std::endl;
        std::cout << "2. Rename it to version.dll" << std::endl;
        std::cout << "3. Place it in this directory" << std::endl;
        std::cout << "4. Run this application again" << std::endl;
        return 1;
    }
    
    std::cout << "\n✅ Successfully loaded version.dll!" << std::endl;
    std::cout << "DLL Handle: " << hDll << std::endl;
    
    // Try to get function addresses
    LegitimateFunctionPtr legitFunc = (LegitimateFunctionPtr)GetProcAddress(hDll, "LegitimateFunction");
    AnotherFunctionPtr anotherFunc = (AnotherFunctionPtr)GetProcAddress(hDll, "AnotherFunction");
    
    if (legitFunc != NULL) {
        std::cout << "\n📞 Calling LegitimateFunction..." << std::endl;
        legitFunc();
    } else {
        std::cout << "\n⚠️  LegitimateFunction not found in DLL" << std::endl;
    }
    
    if (anotherFunc != NULL) {
        std::cout << "\n📞 Calling AnotherFunction with parameter 42..." << std::endl;
        int result = anotherFunc(42);
        std::cout << "Function returned: " << result << std::endl;
    } else {
        std::cout << "\n⚠️  AnotherFunction not found in DLL" << std::endl;
    }
    
    // Show DLL information
    std::cout << "\n=== DLL Information ===" << std::endl;
    char dllPath[MAX_PATH];
    if (GetModuleFileNameA(hDll, dllPath, MAX_PATH)) {
        std::cout << "DLL Path: " << dllPath << std::endl;
    }
    
    // Show loaded modules
    std::cout << "\n=== Loaded Modules ===" << std::endl;
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            char modName[MAX_PATH];
            if (GetModuleFileNameA(hMods[i], modName, sizeof(modName))) {
                std::cout << "  " << modName << std::endl;
            }
        }
    }
    
    // Cleanup
    FreeLibrary(hDll);
    std::cout << "\n✅ DLL unloaded successfully" << std::endl;
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    std::cout << "If you saw message boxes, the DLL hijacking worked!" << std::endl;
    
    return 0;
}

/*
Compilation instructions:
g++ -o target_app target_application.cpp -lpsapi

Usage:
1. Compile this target application
2. Compile the malicious DLL: g++ -shared -o malicious.dll dll_hijack_demo.cpp
3. Rename malicious.dll to version.dll
4. Place version.dll in the same directory as target_app.exe
5. Run target_app.exe to see the hijacking in action

The target application will load version.dll, but if your malicious DLL is present
with the same name, it will be loaded instead!
*/
