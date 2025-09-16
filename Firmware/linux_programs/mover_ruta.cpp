#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include "Definitions.h"
#include <cstdint>
#include <vector>
#include <thread>

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

unsigned short ID_nodos[]    = {1,2,3};           // Tobillo, rodilla, cadera.    
double posicionesIniciales[] = {4.0, 3.0, 30.0};  // Posiciones iniciales en grados

// ---------- Funciones ----------
bool initEPOS4() {
    g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, &pErrorCode);
    if (!g_pKeyHandle) {
        cerr << "❌ Error al abrir el dispositivo. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "🔌 Dispositivo CAN abierto." << endl;

    unsigned short ID_nodos[] = {1, 2, 3}; // Tobillo, rodilla, cadera
    for (unsigned short ID_nodo : ID_nodos) {
        if (!VCS_SetEnableState(g_pKeyHandle, ID_nodo, &pErrorCode)) {
            cerr << "❌ Error al habilitar nodo " << ID_nodo << ". Código: " << pErrorCode << endl;
            return false;
        }
        cout << "✅ Nodo " << ID_nodo << " habilitado." << endl;
    }

    return true;
}

long grados2counts(int articulacion, double grados) {
    double countsPorGrado;

    if (articulacion == 1) {           // Tobillo
        countsPorGrado = 487.0;
    } 
    else if (articulacion == 2) {      // Rodilla
        countsPorGrado = 500.0;
    } 
    else if (articulacion == 3) {      // Cadera
        countsPorGrado = 1024.0;
    } 
    else {
        cerr << "❌ Articulación no reconocida: " << articulacion << endl;
        return 0;
    }

    return static_cast<long>(grados * countsPorGrado);
}

bool setearHoming(unsigned short ID_nodo, int8_t homingMethod = 35) {
    if (!VCS_ActivateHomingMode(g_pKeyHandle, ID_nodo, &pErrorCode)) {
        cerr << "❌ Error activando modo homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_SetHomingParameter(g_pKeyHandle, ID_nodo, 500, 100, 50, 0, 0, 0, &pErrorCode)) {
        cerr << "❌ Error en parámetros de homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_FindHome(g_pKeyHandle, ID_nodo, homingMethod, &pErrorCode)) {
        cerr << "❌ Error ejecutando homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_WaitForHomingAttained(g_pKeyHandle, ID_nodo, 5000, &pErrorCode)) {
        cerr << "❌ Timeout esperando homing. Código: " << pErrorCode << endl;
        return false;
    }

    if (!VCS_DefinePosition(g_pKeyHandle, ID_nodo, 0, &pErrorCode)) {
        cerr << "❌ No se pudo definir posición cero. Código: " << pErrorCode << endl;
        return false;
    }

    cout << "✅ Posición cero seteada con homing." << endl;
    return true;
}

bool configurarPerfil() {

    // Activar y configurar todos los nodos
    for (int i = 0; i < 3; i++) {
        unsigned short nodo = ID_nodos[i];

        if (!VCS_ActivateProfilePositionMode(g_pKeyHandle, nodo, &pErrorCode)) {
            cerr << "❌ Error activando Profile Position Mode en nodo " << nodo << endl;
            return false;
        }

        if (!VCS_SetPositionProfile(g_pKeyHandle, nodo, 500, 500, 500, &pErrorCode)) {
            cerr << "❌ Error configurando perfil en nodo " << nodo << endl;
            return false;
        }
    }

    cout << "✅ Todos los motores están en posición inicial." << endl;

    return true;
}

bool calibrarNodoManual() {
    configurarPerfil();

    for (int i = 0; i < 3; i++) {
        unsigned short nodo = ID_nodos[i];

        // Calibración manual
        string input;
        while (true) {
            cout << "👉 Ingresa grados a mover (ej: 5, -2), 'set' para fijar cero, 'exit' para paro de emergencia: ";
            cin >> input;

            if (input == "exit") {
                cout << "🛑 Paro de emergencia activado. Calibración detenida." << endl;
                return false;
            } 
            else if (input == "set") {
                if (!setearHoming(nodo)) {  // Usamos la función setearHoming con el ID del nodo
                    cerr << "❌ Error seteando posición cero en nodo " << nodo << endl;
                    return false;
                }
                cout << "✅ Nodo " << nodo << " calibrado en cero mediante homing." << endl;
                break; // Pasar al siguiente nodo
            }
            else {
                try {
                    double grados = stod(input);
                    long target = grados2counts(nodo, grados);

                    if (!VCS_MoveToPosition(g_pKeyHandle, nodo, target, true, true, &pErrorCode)) {
                        cerr << "❌ Error moviendo nodo " << nodo << " a " << grados << " grados." << endl;
                        return false;
                    }
                    cout << "➡️ Nodo " << nodo << " movido a " << grados << " grados." << endl;
                } 
                catch (...) {
                    cerr << "⚠️ Entrada no válida." << endl;
                }
            }
        }
    }

    cout << "\n✅ Todos los nodos han sido calibrados correctamente." << endl;
    return true;
}

void moverMotorSeguro(unsigned short nodo, long posicionCounts, short corrienteMaxima = 6000) {
    if (!VCS_MoveToPosition(g_pKeyHandle, nodo, posicionCounts, true, false, &pErrorCode)) {
        cerr << "❌ Error al iniciar movimiento. Código: " << pErrorCode << endl;
        return;
    }

    const int intervaloMs = 50;
    short corriente = 0;
    int movimientoCompletado = 0;

    while (true) {
        if (!VCS_GetMovementState(g_pKeyHandle, nodo, &movimientoCompletado, &pErrorCode)) {
            cerr << "❌ Error al obtener estado de movimiento. Código: " << pErrorCode << endl;
            break;
        }

        if (!VCS_GetCurrentIs(g_pKeyHandle, nodo, &corriente, &pErrorCode)) {
            cerr << "❌ Error al leer corriente. Código: " << pErrorCode << endl;
            break;
        }

        cout << "Nodo " << nodo << " | Corriente: " << corriente << " mA" << endl;

        if (abs(corriente) > corrienteMaxima) {
            cout << "⚠️ Corriente excedida (" << corriente << " mA). Deteniendo motor " << nodo << endl;
            VCS_HaltPositionMovement(g_pKeyHandle, nodo, &pErrorCode);
            break;
        }

        if (movimientoCompletado) {
            cout << "✅ Nodo " << nodo << " completó movimiento." << endl;
            break;
        }

        usleep(intervaloMs * 1000);
    }
}

void moverMotorVector(unsigned short nodo, std::vector<double>& posicionesGrados, short corrienteMaxima){
    for (double grados : posicionesGrados) {
        long targetCounts = grados2counts(nodo, grados);
        cout << "\n🔹 Nodo " << nodo << " moviéndose a " << grados << " grados." << endl;
        moverMotorSeguro(nodo, targetCounts, corrienteMaxima);
    }
    cout << "✅ Nodo " << nodo << " completó todo el vector de posiciones." << endl;
}

// ---------- Menú principal ----------
int main() {
    initEPOS4();
    calibrarNodoManual();
    configurarPerfil();

    std::vector<double> posicionesPrueba = {10.0, -10.0, 10};

    std::thread hiloTobillo(moverMotorVector, ID_nodos[0], std::ref(posicionesPrueba),6000);
    std::thread hiloRodilla(moverMotorVector, ID_nodos[1], std::ref(posicionesPrueba),6000);
    std::thread hiloCadera(moverMotorVector, ID_nodos[2], std::ref(posicionesPrueba),6000);

    hiloTobillo.join();
    hiloRodilla.join();
    hiloCadera.join();

    cout << "✅ Movimiento de prueba completado en los tres nodos." << endl;
}