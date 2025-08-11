# 🔧 Configuración

## 🛠️ Conexión de Hardware

1. **Conectar el EPOS4 50/8** a la placa de conexión.  

2. **Conectar el motor** que se desea configurar a la placa de conexión mediante el cable **MOLEX** al conector correspondiente.  
   ![Conexión motor](docs/images/conexion_motor.jpg)  

3. **Conectar el encoder** (interno o externo):  
   - **Encoder interno**: Conectar al puerto indicado en la siguiente figura.  
     ![Puerto encoder interno](docs/images/encoder_interno.jpg)  
   - **Encoder externo incremental**: Conectar al puerto indicado para este tipo de encoder.  
     ![Puerto encoder externo](docs/images/encoder_externo.jpg)  

4. **Conectar el cable de alimentación**:  
   - El EPOS4 cuenta con **dos puertos de alimentación**:  
     - **Entrada**: Desde la fuente de alimentación.  
     - **Salida**: Permite alimentar en serie otro EPOS4.  
   ⚠️ **Precaución**: Conectar mal estos puertos puede provocar un **cortocircuito**.  
   ![Conexión alimentación](docs/images/conexion_alimentacion.jpg)  

5. **Fuente de 5V**: Necesaria para desbloquear el movimiento del sistema. *(Pendiente especificar el mecanismo exacto que se desbloquea)*.  

6. **Encendido y diagnóstico por LED**:  
   - **Verde** ✅ → Funcionamiento correcto (generalmente después de la sintonización).  
   - **Rojo** ❌ → Error en configuración o incompatibilidad entre motor y parámetros del EPOS4.  

7. **Conectar el EPOS4 al PC** mediante cable **Micro-USB**.  

---

## ⚙️ Configuración del Controlador

1. **Instalar EPOS Studio** en el PC.  

2. **Crear un nuevo proyecto** y seleccionar el modelo **EPOS4**.  

3. En el apartado **Communication**, hacer clic derecho y seleccionar **Connect**.  
   ![Conexión en EPOS Studio](docs/images/epos_studio_connect.jpg)  

4. Ir a **Wizards → Startup** para realizar el proceso de configuración, donde se definen las características del motor que controlará el EPOS4.  

---

### 🛠️ Proceso de Configuración del Motor

#### **1. Drive System / Motor**  
- **Nominal current** → Corriente nominal del motor.  
- **Thermal time constant winding**  
- **Max speed** → Según *datasheet*.  
- **Number of pole pairs**  

#### **2. Drive System / Sensors**  
- **X4**: Sensores Hall.  
- **Encoder interno** (X5):  
  - Tipo: *Digital incremental encoder 1*.  
  - Configurar como en la figura:  
    ![Config encoder interno](docs/images/config_encoder_interno.jpg)  
- **Encoder externo** (X6):  
  - Introducir el número de pulsos según el encoder AMT.  
  - Usar **AMT Viewpoint** para obtener la cantidad de pulsos.  
  - Dirección: **Inverted**.  
  - Configurar como en la figura:  
    ![Config encoder externo](docs/images/config_encoder_externo.jpg)  

#### **3. Controller / Communication**  
- Seleccionar sensores a usar: solo Hall o Hall + Incremental (recomendado).  
- Configuración como en la figura:  
  ![Config comunicación](docs/images/config_communication.jpg)  

#### **4. Controller / Regulation**  
- Seleccionar el tipo de control: **Corriente**, **Velocidad** o **Posición**.  
- Figura de ejemplo:  
  ![Config regulación](docs/images/config_regulation.jpg)  

#### **5. Controller / Limits**  
- **Max output current** → Corriente nominal × 3  
- **Max acceleration** → Según requerimientos mecánicos.  
- **Max profile velocity** → Velocidad máxima según *datasheet* (limitada por número de polos).  
- **Following error window** → Según tolerancia de la aplicación.  

---

✅ **Finalizar** el asistente y guardar configuración.  
