\# 🏰 Castillo y Mountain Dragon



Proyecto desarrollado en \*\*C++ y OpenGL\*\* que consiste en una escena 3D interactiva compuesta por un castillo y un dragón animado. El proyecto implementa carga de modelos mediante glTF, animaciones esqueléticas, curvas de Bézier para generar trayectorias circulares y una interfaz gráfica interactiva mediante Dear ImGui.



\## 📌 Descripción del proyecto



El proyecto permite visualizar un escenario 3D en el que un Mountain Dragon realiza un recorrido circular alrededor de un castillo, siguiendo una trayectoria construida mediante curvas cúbicas de Bézier.



El dragón cuenta con una animación glTF que permite controlar el movimiento de sus alas de manera independiente al desplazamiento por la trayectoria.



También incorpora una cámara interactiva para navegar por la escena y una interfaz gráfica que permite modificar diferentes parámetros durante la ejecución.



\## 🚀 Funcionalidades principales



\- Carga y renderizado de modelos 3D en formato glTF.

\- Animación del Mountain Dragon mediante animaciones glTF.

\- Implementación de cuatro curvas cúbicas de Bézier para construir una trayectoria circular.

\- Movimiento del dragón siguiendo la trayectoria.

\- Orientación del modelo según la dirección de la curva.

\- Visualización de la trayectoria con profundidad respecto al escenario.

\- Cámara 3D con movimiento y rotación mediante teclado y mouse.

\- Interfaz gráfica desarrollada con Dear ImGui.

\- Control independiente del vuelo y la animación de las alas.

\- Ajuste de velocidad y escala del dragón.

\- Iluminación de la escena 3D.



\## 🛠️ Tecnologías y herramientas



| Tecnología | Utilización |

|---|---|

| C++ | Lenguaje de programación |

| OpenGL 4.5 | Renderizado gráfico |

| GLFW | Creación y gestión de ventanas y entradas |

| GLAD | Carga de funciones de OpenGL |

| GLM | Operaciones matemáticas y matrices |

| TinyGLTF | Lectura de modelos glTF |

| Dear ImGui | Interfaz gráfica interactiva |

| GLSL | Programación de shaders |

| Visual Studio | Entorno de desarrollo |



\## 📂 Organización del código



El proyecto utiliza una arquitectura modular en la que cada clase tiene una responsabilidad específica.



| Archivo | Responsabilidad |

|---|---|

| `Main.cpp` | Inicialización y coordinación general de la escena. |

| `Camera.h / Camera.cpp` | Movimiento y control de la cámara 3D. |

| `Bezier.h / Bezier.cpp` | Cálculo y generación de las curvas de Bézier. |

| `DragonController.h / DragonController.cpp` | Control del movimiento, velocidad y escala del dragón. |

| `Interfaz.h / Interfaz.cpp` | Gestión de la interfaz gráfica Dear ImGui. |

| `AnimatedModelGLTF.h / AnimatedModelGLTF.cpp` | Carga y reproducción de animaciones glTF. |

| `ModelGLTF.h / ModelGLTF.cpp` | Carga y renderizado del castillo. |

| `Iluminacion.h / Iluminacion.cpp` | Configuración y aplicación de iluminación. |

| `ShaderClass.h / ShaderClass.cpp` | Gestión de los shaders. |



\## 🎮 Controles



\### Cámara 3D



| Tecla | Función |

|---|---|

| TAB | Alternar entre cámara 3D e interfaz. |

| W | Avanzar. |

| S | Retroceder. |

| A | Desplazarse a la izquierda. |

| D | Desplazarse a la derecha. |

| SPACE | Subir. |

| SHIFT | Bajar. |

| Mouse | Rotar la cámara cuando está activo el modo 3D. |



\### Control del dragón



| Tecla | Función |

|---|---|

| P | Pausar o reanudar el vuelo y las alas. |

| R | Reiniciar el recorrido. |

| \[ | Retroceder la animación glTF. |

| ] | Avanzar la animación glTF. |



\### Interfaz Dear ImGui



La interfaz permite:



\- Pausar y reanudar el vuelo.

\- Pausar y activar la animación de las alas.

\- Modificar la velocidad del vuelo.

\- Ajustar el tamaño del dragón.

\- Mostrar u ocultar la trayectoria de Bézier.

\- Consultar el tiempo de vuelo.

\- Reiniciar el recorrido.



\## 📐 Implementación de curvas de Bézier



La trayectoria del dragón se construye mediante cuatro curvas cúbicas de Bézier conectadas entre sí.



Cada curva utiliza cuatro puntos de control:



\- P0: punto inicial.

\- P1: primer punto de control.

\- P2: segundo punto de control.

\- P3: punto final.



La posición se calcula mediante la ecuación:



\\\[

B(t)=(1-t)^3P\_0+3(1-t)^2tP\_1+3(1-t)t^2P\_2+t^3P\_3

\\]



Donde \\(t\\) representa el progreso sobre la curva, entre 0 y 1.



La tangente permite obtener la dirección del recorrido y orientar el modelo del dragón durante su desplazamiento.



\## ⚙️ Instalación y ejecución



\### Requisitos



\- Windows.

\- Visual Studio con herramientas de desarrollo de C++.

\- OpenGL 4.5 compatible.

\- GLFW.

\- GLAD.

\- GLM.

\- TinyGLTF.

\- Dear ImGui.



\### Pasos



1\. Clonar el repositorio:



&#x20;  ```bash

&#x20;  git clone URL\_DEL\_REPOSITORIO

&#x20;  ```



2\. Abrir la solución `.sln` en Visual Studio.



3\. Verificar que las bibliotecas GLFW, GLAD, GLM, TinyGLTF y Dear ImGui estén instaladas y correctamente configuradas.



4\. Comprobar los directorios de inclusión y las dependencias de enlazado.



5\. Verificar que las carpetas `models` y los shaders estén ubicados correctamente.



6\. Seleccionar la configuración de compilación correspondiente (x64).



7\. Compilar y ejecutar el proyecto.



\*\*Nota:\*\* Las bibliotecas externas y los recursos 3D deben estar disponibles en las rutas utilizadas por el proyecto. Si no están incluidos en el repositorio, deben configurarse por separado.



\## 👨‍💻 Desarrollo



Este proyecto fue desarrollado como ejercicio académico de computación gráfica 3D, aplicando conceptos de:



\- Transformaciones geométricas.

\- Matrices de modelo, vista y proyección.

\- Renderizado mediante shaders.

\- Animación de modelos 3D.

\- Interpolación mediante curvas de Bézier.

\- Manejo de entradas de teclado y mouse.

\- Interfaces gráficas interactivas.



\---



\*\*Proyecto:\*\* Castillo y Mountain Dragon  

\*\*Lenguaje:\*\* C++  

\*\*Motor gráfico:\*\* OpenGL 4.5

