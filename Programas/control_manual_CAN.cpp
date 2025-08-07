#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>  // Para usleep
#include "Definitions.h"

using namespace std;

// Parámetros globales
void* g_pKeyHandle = nullptr;
unsigned short g_usNodeId = 1;
char* pDeviceName = (char*)"EPOS4";
char* pProtocolStackName = (char*)"CANopen";
char* pInterfaceName = (char*)"CAN_mcp251x 0";
char* pPortName = (char*)"CAN0";
unsigned int pErrorCode = 0;

// Parámetros de control
const double countsPorGrado = 487.0;
const double ANGULO_MIN = -30.0;
const double ANGULO_MAX = 30.0;

// ---------- Inicialización ----------
bool initEPOS4() {
    g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, &pErrorCode);
    if (!g_pKeyHandle) {
        cerr << "❌ Error al abrir el dispositivo. Código: " << pErrorCode << endl;
        return false;
    }

    int isInFault = 0;
    if (VCS_GetFaultState(g_pKeyHandle, g_usNodeId, &isInFault, &pErrorCode)) {
        if (isInFault) {
            if (!VCS_ClearFault(g_pKeyHandle, g_usNodeId, &pErrorCode)) {
                cerr << "❌ Error al limpiar fallo. Código: " << pErrorCode << endl;
                return false;
            }
        }
    } else {
        cerr << "❌ Error al consultar estado de fallo. Código: " << pErrorCode << endl;
        return false;
    }

    int isEnabled = 0;
    if (VCS_GetEnableState(g_pKeyHandle, g_usNodeId, &isEnabled, &pErrorCode)) {
        if (!isEnabled) {
            if (!VCS_SetEnableState(g_pKeyHandle, g_usNodeId, &pErrorCode)) {
                cerr << "❌ Error al habilitar nodo. Código: " << pErrorCode << endl;
                return false;
            }
        }
    } else {
        cerr << "❌ Error al verificar estado habilitado. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "✅ Nodo habilitado correctamente." << endl;
    return true;
}

// ---------- Ejecutar homing ----------
bool ejecutarHoming(int8_t homingMethod = 35) {
    if (!VCS_ActivateHomingMode(g_pKeyHandle, g_usNodeId, &pErrorCode)) {
        cerr << "❌ Error activando modo homing. Código: " << pErrorCode << endl;
        return false;
    }

    uint32_t homingAccel = 500;
    uint32_t speedSwitch = 100;
    uint32_t speedIndex = 50;
    long offset = 0;
    uint16_t currentThreshold = 0;
    long homePosition = 0;

    if (!VCS_SetHomingParameter(g_pKeyHandle, g_usNodeId,
                                 homingAccel, speedSwitch, speedIndex,
                                 offset, currentThreshold, homePosition, &pErrorCode)) {
        cerr << "❌ Error configurando parámetros homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_FindHome(g_pKeyHandle, g_usNodeId, homingMethod, &pErrorCode)) {
        cerr << "❌ Error ejecutando homing. Código: " << pErrorCode << endl;
        return false;
    }

    uint32_t timeoutMs = 5000;
    if (!VCS_WaitForHomingAttained(g_pKeyHandle, g_usNodeId, timeoutMs, &pErrorCode)) {
        cerr << "❌ Timeout esperando homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_DefinePosition(g_pKeyHandle, g_usNodeId, 0, &pErrorCode)) {
        cerr << "❌ No se pudo definir posición cero. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "✅ Homing exitoso. Posición cero definida." << endl;
    return true;
}

// ---------- Configurar perfil ----------
bool configurarPerfil(unsigned int velocity, unsigned int acceleration, unsigned int deceleration) {
    if (!VCS_ActivateProfilePositionMode(g_pKeyHandle, g_usNodeId, &pErrorCode)) {
        cerr << "❌ Error al activar modo posición. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_SetPositionProfile(g_pKeyHandle, g_usNodeId, velocity, acceleration, deceleration, &pErrorCode)) {
        cerr << "❌ Error al configurar perfil. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "✅ Perfil de movimiento configurado." << endl;
    return true;
}

// ---------- Convertir grados a counts ----------
long gradosAcounts(double grados) {
    return static_cast<long>(grados * countsPorGrado);
}

// ---------- Mover a posición ----------
void moverMotor(long posicionCounts) {
    if (!VCS_MoveToPosition(g_pKeyHandle, g_usNodeId, posicionCounts, true, true, &pErrorCode)) {
        cerr << "❌ Error al mover motor. Código: " << pErrorCode << endl;
    } else {
        cout << "✅ Movido a " << posicionCounts << " counts." << endl;
    }
}

// ---------- Principal ----------
int main() {
    if (!initEPOS4()) return -1;
    if (!ejecutarHoming()) return -1;
    if (!configurarPerfil(500, 500, 500)) return -1;

    double angulo = 0.0;

    cout << "🔄 Sistema iniciado. Ángulo actual: 0 grados (posición inicial)." << endl;

    while (true) {
        cout << "\n👉 Ingresa ángulo deseado (" << ANGULO_MIN << "° a " << ANGULO_MAX << "° o 'f' para salir): ";
        string input;
        cin >> input;

        if (input == "f" || input == "F") break;

        try {
            angulo = stod(input);
        } catch (...) {
            cout << "⚠️ Entrada inválida. Intenta de nuevo." << endl;
            continue;
        }

        if (angulo < ANGULO_MIN || angulo > ANGULO_MAX) {
            cout << "⚠️ Ángulo fuera de rango permitido." << endl;
            continue;
        }

        moverMotor(gradosAcounts(angulo));

        // Leer corriente
        short corriente = 0;
        if (VCS_GetCurrentIs(g_pKeyHandle, g_usNodeId, &corriente, &pErrorCode)) {
            cout << "🔌 Corriente actual: " << corriente << " mA" << endl;
        } else {
            cout << "❌ Error leyendo corriente. Código: " << pErrorCode << endl;
        }
    }

    VCS_CloseDevice(g_pKeyHandle, &pErrorCode);
    cout << "🛑 Simulación finalizada." << endl;
    return 0;
}