#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include "Definitions.h"
#include <cstdint>

using namespace std;

// ---------- Parámetros globales ----------
void* g_pKeyHandle = nullptr;
unsigned short g_usNodeId = 1; // Se selecciona dinámicamente
char* pDeviceName = (char*)"EPOS4";
char* pProtocolStackName = (char*)"CANopen";
char* pInterfaceName = (char*)"CAN_gs_usb 0";
char* pPortName = (char*)"CAN0";
unsigned int pErrorCode = 0;

double g_countsPorGrado = 487.0;
const double ANGULO_MAX_MANUAL = 30.0;
const double ANGULO_MAX_CONFIG = 5.0;

// ---------- Funciones ----------
bool initEPOS4() {
    g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, &pErrorCode);
    if (!g_pKeyHandle) {
        cerr << "❌ Error al abrir el dispositivo. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "🔌 Nodo conectado: " << g_usNodeId << endl;

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

    return true;
}

bool setearHoming(int8_t homingMethod = 35) {
    if (!VCS_ActivateHomingMode(g_pKeyHandle, g_usNodeId, &pErrorCode)) {
        cerr << "❌ Error activando modo homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_SetHomingParameter(g_pKeyHandle, g_usNodeId, 500, 100, 50, 0, 0, 0, &pErrorCode)) {
        cerr << "❌ Error en parámetros de homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_FindHome(g_pKeyHandle, g_usNodeId, homingMethod, &pErrorCode)) {
        cerr << "❌ Error ejecutando homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_WaitForHomingAttained(g_pKeyHandle, g_usNodeId, 5000, &pErrorCode)) {
        cerr << "❌ Timeout esperando homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_DefinePosition(g_pKeyHandle, g_usNodeId, 0, &pErrorCode)) {
        cerr << "❌ No se pudo definir posición cero. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "✅ Posición cero seteada con homing." << endl;
    return true;
}

bool configurarPerfil(unsigned int v, unsigned int a, unsigned int d) {
    if (!VCS_ActivateProfilePositionMode(g_pKeyHandle, g_usNodeId, &pErrorCode)) {
        cerr << "❌ Error activando modo posición. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_SetPositionProfile(g_pKeyHandle, g_usNodeId, v, a, d, &pErrorCode)) {
        cerr << "❌ Error configurando perfil. Código: " << pErrorCode << endl;
        return false;
    }

    return true;
}

long gradosAcounts(double grados) {
    return static_cast<long>(grados * g_countsPorGrado);
}

void moverMotorSeguro(long posicionCounts, short corrienteMaxima = 6000) {
    if (!VCS_MoveToPosition(g_pKeyHandle, g_usNodeId, posicionCounts, true, false, &pErrorCode)) {
        cerr << "❌ Error al iniciar movimiento. Código: " << pErrorCode << endl;
        return;
    }

    const int intervaloMs = 50;
    short corriente = 0;
    int movimientoCompletado = 0;

    while (true) {
        // Verifica si ya terminó el movimiento
        if (!VCS_GetMovementState(g_pKeyHandle, g_usNodeId, &movimientoCompletado, &pErrorCode)) {
            cerr << "❌ Error al obtener estado de movimiento. Código: " << pErrorCode << endl;
            break;
        }

        // Verifica la corriente
        if (!VCS_GetCurrentIs(g_pKeyHandle, g_usNodeId, &corriente, &pErrorCode)) {
            cerr << "❌ Error al leer corriente. Código: " << pErrorCode << endl;
            break;
        }

        cout << "🔍 Corriente: " << corriente << " mA" << endl;

        // Si la corriente supera el límite, detener el motor
        if (abs(corriente) > corrienteMaxima) {
            cout << "⚠️ Corriente excedida (" << corriente << " mA). ¡Deteniendo motor por seguridad!" << endl;
            VCS_HaltPositionMovement(g_pKeyHandle, g_usNodeId, &pErrorCode);
            break;
        }

        // Si el movimiento ha finalizado, salir
        if (movimientoCompletado) {
            cout << "✅ Movimiento completado con éxito." << endl;
            break;
        }

        usleep(intervaloMs * 1000); // Espera entre chequeos
    }
}

void controlPorIncrementos(double maxIncrementoGrado) {
    double posicionActualGrados = 0.0;
    string input;

    while (true) {
        cout << "📍 Posición actual: " << posicionActualGrados << "°" << endl;
        cout << "🔧 Ingresa incremento (±" << maxIncrementoGrado << "° máx) o 'f' para volver: ";
        cin >> input;

        if (input == "f" || input == "F") break;

        double incremento = 0.0;
        try {
            incremento = stod(input);
        } catch (...) {
            cout << "⚠️ Entrada inválida." << endl;
            continue;
        }

        if (incremento < -maxIncrementoGrado || incremento > maxIncrementoGrado) {
            cout << "⚠️ El incremento debe estar entre -" << maxIncrementoGrado << "° y +" << maxIncrementoGrado << "°." << endl;
            continue;
        }

        posicionActualGrados += incremento;
        long nuevaPosicionCounts = gradosAcounts(posicionActualGrados);
        moverMotorSeguro(nuevaPosicionCounts);
    }
}


void controlManual(double maxAngulo) {
    double angulo = 0.0;
    string input;

    while (true) {
        cout << "🎯 Ingresa ángulo (-" << maxAngulo << "° a +" << maxAngulo << "°) o 'f' para volver: ";
        cin >> input;
        if (input == "f" || input == "F") break;

        try {
            angulo = stod(input);
        } catch (...) {
            cout << "⚠️ Entrada inválida." << endl;
            continue;
        }

        if (angulo < -maxAngulo || angulo > maxAngulo) {
            cout << "⚠️ Fuera de rango permitido." << endl;
            continue;
        }

        moverMotorSeguro(gradosAcounts(angulo));
    }
}
bool seleccionarNodo() {
    while (true) {
        string input;
        cout << "🔧 Ingrese el ID del nodo al que desea conectarse (1, 2, 3, etc.): ";
        cin >> input;

        try {
            int id = stoi(input);
            if (id <= 0) throw invalid_argument("ID debe ser positivo");
            g_usNodeId = static_cast<unsigned short>(id);

            // Establecer relación según el nodo
            if (g_usNodeId == 1)
                g_countsPorGrado = 487.0;
            else if (g_usNodeId == 2)
                g_countsPorGrado = 500.0;
            else
                g_countsPorGrado = 487.0; // Valor por defecto para nodos desconocidos


        } catch (...) {
            cout << "❌ ID inválido. Inténtalo de nuevo.\n" << endl;
            continue;
        }

        // Cerrar si ya hay una conexión previa
        if (g_pKeyHandle) {
            VCS_CloseDevice(g_pKeyHandle, &pErrorCode);
            g_pKeyHandle = nullptr;
        }

        // Intentar conectar con el nuevo nodo
        if (initEPOS4() && configurarPerfil(500, 500, 500)) {
            return true;
        } else {
            cout << "❌ Fallo al conectar con el nodo " << g_usNodeId << ". Intenta con otro ID.\n" << endl;
        }
    }
}





// ---------- Menú principal ----------
int main() {
    if (!seleccionarNodo()) return -1;

    while (true) {
        cout << "\n===== MENÚ PRINCIPAL (Nodo: " << g_usNodeId << ") =====" << endl;
        cout << "1. Setear posición en cero (homing)" << endl;
        cout << "2. Configurar posición (±5°)" << endl;
        cout << "3. Control manual (±30°)" << endl;
        cout << "4. Cambiar de nodo" << endl;
        cout << "5. Salir" << endl;
        cout << "Selecciona una opción: ";

        string opcion;
        cin >> opcion;

        if (opcion == "1") {
            setearHoming();
        } else if (opcion == "2") {
            if (!configurarPerfil(500, 500, 500)) return -1;
            controlPorIncrementos(ANGULO_MAX_CONFIG);
        } else if (opcion == "3") {
            if (!configurarPerfil(500, 500, 500)) return -1;
            controlManual(ANGULO_MAX_MANUAL);
        } else if (opcion == "4") {
            if (!seleccionarNodo()) break;
        } else if (opcion == "5") {
            break;
        } else {
            cout << "❌ Opción no válida." << endl;
        }
    }

    if (g_pKeyHandle)
        VCS_CloseDevice(g_pKeyHandle, &pErrorCode);

    cout << "🛑 Programa finalizado." << endl;
    return 0;
}