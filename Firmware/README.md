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
   En la sección **Descargas**, buscar el archivo mostrado en la imagen:  
   <p align="center">
     <img src="src/startup.png" alt="Descarga Librería" width="400"/>
   </p>

2. Descomprimir el archivo `.zip`.

3. Acceder a la carpeta **EPOS_Linux_Library**.

4. Ejecutar el script de instalación:
   ```bash
   sudo bash ./install.sh
