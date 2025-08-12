#include <iostream>
#include "Definitions.h"
#include <cstdint>

int main() {
    const uint16_t maxStrSize = 100;
    char* deviceName = (char*)"EPOS4";
    char* protocolStackName = (char*)"CANopen";
    char interfaceName[maxStrSize];
    int endOfSelection = 0;  // 🔧 cambiado de bool a int
    uint32_t errorCode = 0;

    std::cout << "Interfaces disponibles:\n";

    if (VCS_GetInterfaceNameSelection(deviceName, protocolStackName, 1, interfaceName, maxStrSize, &endOfSelection, &errorCode)) {
        std::cout << "- " << interfaceName << std::endl;

        while (endOfSelection == 0) {
            if (VCS_GetInterfaceNameSelection(deviceName, protocolStackName, 0, interfaceName, maxStrSize, &endOfSelection, &errorCode)) {
                std::cout << "- " << interfaceName << std::endl;
            } else {
                std::cerr << "Error: " << errorCode << std::endl;
                break;
            }
        }
    } else {
        std::cerr << "Error inicial: " << errorCode << std::endl;
    }

    return 0;
}