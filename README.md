# 🏰 Castillo y Mountain Dragon

**Proyecto de Computación Gráfica | C++ · OpenGL 4.5**

Proyecto de visualización 3D interactiva desarrollado en C++ utilizando OpenGL. Consiste en una escena compuesta por un castillo y un Mountain Dragon animado, cuyo desplazamiento se realiza mediante una trayectoria circular construida con curvas cúbicas de Bézier.

El proyecto integra modelado 3D, transformaciones geométricas, animación glTF, iluminación, shaders y una interfaz gráfica desarrollada con Dear ImGui.

---

## 📸 Vista del proyecto

<!-- Agregar aquí capturas de pantalla del proyecto -->

| Escena 3D | Trayectoria Bézier |
|:---:|:---:|
| Captura del castillo y el dragón | Captura de la trayectoria circular |

| Interfaz Dear ImGui |
|:---:|
| Captura del panel de control |

---

## 📖 Descripción general

El objetivo del proyecto es desarrollar una escena 3D interactiva en la que un Mountain Dragon realiza un recorrido alrededor de un castillo.

Para conseguirlo, se implementaron cuatro curvas cúbicas de Bézier conectadas entre sí, formando una trayectoria circular. El dragón utiliza la posición y la tangente de estas curvas para desplazarse y orientar su modelo durante el vuelo.

Además, se implementó un sistema de animación glTF que permite controlar independientemente el movimiento de las alas y el desplazamiento del personaje.

La escena cuenta con una cámara 3D y una interfaz gráfica que permite modificar diferentes parámetros durante la ejecución.

---

## ✨ Funcionalidades

- Carga y visualización de modelos 3D mediante archivos glTF.
- Reproducción de animaciones del Mountain Dragon.
- Trayectoria circular mediante cuatro curvas cúbicas de Bézier.
- Movimiento automático del dragón sobre la trayectoria.
- Orientación del dragón según la dirección del recorrido.
- Visualización de las curvas respetando la profundidad de la escena.
- Cámara 3D con movimiento mediante teclado y mouse.
- Iluminación dinámica del escenario.
- Interfaz gráfica interactiva con Dear ImGui.
- Control independiente del vuelo y la animación de las alas.
- Modificación de la velocidad y escala del dragón.
- Visualización y ocultamiento de la trayectoria.

---

## 🛠️ Tecnologías utilizadas

| Tecnología | Aplicación |
|---|---|
| C++ | Desarrollo de la aplicación |
| OpenGL 4.5 | Renderizado 3D |
| GLFW | Ventana y gestión de entradas |
| GLAD | Carga de funciones OpenGL |
| GLM | Matemáticas y transformaciones 3D |
| TinyGLTF | Lectura de modelos glTF |
| Dear ImGui | Interfaz gráfica |
| GLSL | Programación de shaders |
| Visual Studio | Entorno de desarrollo |

---

## 🧩 Arquitectura del proyecto

El código está organizado mediante clases independientes para facilitar su mantenimiento y comprensión.

| Archivo | Descripción |
|---|---|
| `Main.cpp` | Inicialización y coordinación general del proyecto. |
| `Camera.h / Camera.cpp` | Control de la cámara y navegación 3D. |
| `Bezier.h / Bezier.cpp` | Cálculo matemático de las curvas y construcción de la trayectoria. |
| `DragonController.h / DragonController.cpp` | Control del movimiento, velocidad, escala y animación del dragón. |
| `Interfaz.h / Interfaz.cpp` | Gestión de la interfaz gráfica mediante Dear ImGui. |
| `AnimatedModelGLTF.h / AnimatedModelGLTF.cpp` | Carga y reproducción de animaciones glTF. |
| `ModelGLTF.h / ModelGLTF.cpp` | Carga y renderizado del castillo. |
| `Iluminacion.h / Iluminacion.cpp` | Configuración y aplicación de iluminación. |
| `ShaderClass.h / ShaderClass.cpp` | Administración de shaders. |

---

## 🎮 Controles del proyecto

### Cámara 3D

| Tecla | Acción |
|---|---|
| `TAB` | Alternar entre modo cámara e interfaz. |
| `W` | Avanzar. |
| `S` | Retroceder. |
| `A` | Desplazarse a la izquierda. |
| `D` | Desplazarse a la derecha. |
| `SPACE` | Subir. |
| `SHIFT` | Bajar. |
| Mouse | Rotar la cámara en modo 3D. |

### Control del Mountain Dragon

| Tecla | Acción |
|---|---|
| `P` | Pausar o reanudar el vuelo y las alas. |
| `R` | Reiniciar el recorrido. |
| `[` | Retroceder la animación. |
| `]` | Avanzar la animación. |

### Panel Dear ImGui

La interfaz permite realizar las siguientes acciones:

- Pausar y reanudar el vuelo.
- Activar y pausar la animación de las alas.
- Modificar la velocidad del vuelo.
- Ajustar el tamaño del dragón.
- Mostrar u ocultar la trayectoria Bézier.
- Consultar el tiempo de vuelo.
- Reiniciar el recorrido.

---

## 📐 Implementación matemática: curvas de Bézier

La trayectoria del dragón se construye mediante cuatro curvas cúbicas de Bézier.

Cada curva utiliza cuatro puntos:

- **P0:** punto inicial.
- **P1:** primer punto de control.
- **P2:** segundo punto de control.
- **P3:** punto final.

La posición se obtiene mediante la siguiente expresión:

\[
B(t)=(1-t)^3P_0+3(1-t)^2tP_1+3(1-t)t^2P_2+t^3P_3
\]

Donde \(t\) representa el progreso sobre la curva y toma valores entre 0 y 1.

También se calcula la tangente de la curva para determinar la dirección del movimiento y orientar el modelo durante el recorrido.

Las cuatro curvas se conectan para formar una trayectoria circular alrededor del escenario.

---

## ⚙️ Instalación y ejecución

### Requisitos

- Windows.
- Visual Studio con herramientas de desarrollo C++.
- Compatibilidad con OpenGL 4.5.
- GLFW.
- GLAD.
- GLM.
- TinyGLTF.
- Dear ImGui.

### Procedimiento

**1. Clonar el repositorio**

```bash
git clone URL_DEL_REPOSITORIO
```

**2. Abrir el proyecto**

Abrir el archivo de solución `.sln` desde Visual Studio.

**3. Configurar las bibliotecas**

Verificar que las dependencias GLFW, GLAD, GLM, TinyGLTF y Dear ImGui estén disponibles y correctamente configuradas.

**4. Verificar los recursos**

Comprobar que los modelos glTF, las texturas y los shaders estén ubicados en las rutas utilizadas por el código.

**5. Compilar y ejecutar**

Seleccionar la plataforma `x64`, compilar la solución y ejecutar el proyecto.

> **Nota:** Las dependencias externas y los recursos que no estén incluidos en el repositorio deben obtenerse y configurarse por separado.

---

## 📚 Conceptos aplicados

Durante el desarrollo se trabajaron los siguientes conceptos de computación gráfica:

- Sistemas de coordenadas 3D.
- Transformaciones de traslación, rotación y escala.
- Matrices de modelo, vista y proyección.
- Renderizado mediante shaders.
- Iluminación de escenas 3D.
- Carga y animación de modelos glTF.
- Interpolación mediante curvas de Bézier.
- Manejo de entradas de teclado y mouse.
- Interfaces gráficas interactivas.

---

## 👨‍💻 Información del proyecto

**Nombre:** Castillo y Mountain Dragon

**Lenguaje:** C++

**API gráfica:** OpenGL 4.5

**Entorno de desarrollo:** Visual Studio

**Área:** Computación Gráfica

---

*Proyecto académico desarrollado como ejercicio de programación gráfica 3D.*
