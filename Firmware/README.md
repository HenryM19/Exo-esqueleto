# 💻 Firmware del Exoesqueleto

En esta sección se presenta el firmware para manejar el exoesqueleto mediante **USB** y **CAN**, compatible con controladores **EPOS4** de Maxon.  

---

## 🖥️ Windows  
Control directo de un **único módulo EPOS4** a través de **USB**, usando **Visual Studio** y las librerías proporcionadas por Maxon.  

📂 **Ubicación de librerías:** [`Drivers`](../Drivers)  

**Características:**
- Control individual de un EPOS4.
- Comunicación directa USB.
- Útil para pruebas de banco o calibración de un solo motor.

---

## 🐧 Ubuntu / Linux 
Mediante Linux actualmente se puede controlar los EPOS4 y sus motores mediante una red CAN, debido a que la librería proporcionada por Maxon para Linux es compatible. 

Control de los EPOS4 mediante **red CAN**, usando la librería oficial de Maxon para Linux.  

**Hardware compatible:**
- **PC con Ubuntu** + adaptador USB a CAN (ej. USB2CAN de Innomaker).
- **Raspberry Pi** + módulo MCP2515 (requiere adaptación de niveles de voltaje para SPI-CAN).

---

### 📦 Instalación de la librería en Linux
1. Descargar la librería oficial desde:  
   [EPOS4 50/8 - Página de producto Maxon](https://www.maxongroup.com/maxon/view/product/control/Positionierung/EPOS-4/504384)  
   En la sección **Descargas**, buscar el archivo **EPOS-Linux-Library**.

2. Descomprimir el archivo `.zip`.

3. Acceder a la carpeta **EPOS_Linux_Library**.

4. Ejecutar el script de instalación:
   ```bash
   sudo bash ./install.sh
    ```

   <p align="center">
     <img src="src/instalar.png" alt="Descarga Librería" width="400"/>
   </p>


## Programas Linux  

En cuestión se ha desarrollado algunos programas hasta el momento para el manejo de los motores, ubicados en la carpeta programs, entre ellos se encuentran: 


#### Windows


#### Linux

- **listar_interfaces.cpp**  
  Detecta las interfaces CAN disponibles y muestra los nombres compatibles con la librería EPOS.  

- **control_manual_CAN.cpp**  
  Permite conectarse a un motor mediante la red CAN. 

- **menu_control_OneMotor.cpp**  
  Permite controlar un EPOS4 a la vez mediante una red CAN, permite seleccionar el nodo del EPOS4 que se desea controlar. Luego se abre un menú que permite configurar la posición actual como la posición cero. otra opción permite mover el motor +- 5 grados, esta es para calibrar la posición 0, mientras que otra opción permite manejar a +-30 grados el motor (Considerando el engranaje).