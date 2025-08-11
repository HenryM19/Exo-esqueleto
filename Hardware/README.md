## ⚙️ Hardware del Exoesqueleto  

El sistema está compuesto por diversos elementos mecánicos y electrónicos que trabajan en conjunto para el control y movimiento del exoesqueleto.  

---

### 🌀 Motores Maxon  
- **Descripción:** Se utilizan motores Maxon de distintos modelos (no todos son iguales).  
- **Documentación:** Los *datasheets* y especificaciones técnicas de cada motor se encuentran en la carpeta [`Docs/`](./Docs).  
- **Notas:** Cada motor tiene diferentes características de torque, velocidad y consumo.  

**Imagen sugerida:**  
![Ejemplo Motor Maxon](docs/images/motor_maxon.jpg)  

---

### 🎛️ Controladores EPOS4 50/8  
- **Función:** Controlador dedicado para cada motor, es decir, **un EPOS4 50/8 por motor**.  
- **Configuración:** Se ajustan mediante el software **EPOS Studio**, donde se sintonizan los parámetros PID para cada motor.  
- **Documentación:** Manuales y guías en [`Docs/`](./Docs).  

**Imagen sugerida:**  
![EPOS4 50/8](docs/images/epos4.jpg)  

---

### 🔌 Placa de Conexión  
- **Origen:** Diseñada por estudiantes de la Universidad de Cuenca como parte de su tesis de grado.  
- **Funciones:** Facilita la interconexión entre motores, controladores y sensores.  
- **Extras:** Se incluye un **pinout detallado** para facilitar la comprensión del funcionamiento.  
- **Documentación:** Archivos y esquemas en [`Docs/`](./Docs).  

**Imagen sugerida:**  
![Placa de Conexión](docs/images/placa_conexion.jpg)  

---

### ⚡ Fuente de Alimentación  
- **Descripción:** Conjunto de dos fuentes de poder para suministrar energía al sistema.  
- **Salidas:**  
  - **5V** (para circuitos lógicos y controladores)  
  - **12V o 24V** (para motores, dependiendo de la configuración)  
- **Nota:** Las salidas disponibles dependen de si una o ambas fuentes están encendidas.  

**Imagen sugerida:**  
![Fuente de Alimentación](docs/images/fuente_poder.jpg)  

---

📌 **Recomendaciones:**  
- Agregar un **diagrama general de conexión** que muestre cómo interactúan estos componentes en el sistema.  
- Mantener todas las imágenes reales en la carpeta `docs/images` para centralizar los recursos visuales.  
