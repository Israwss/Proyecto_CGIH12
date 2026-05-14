# ACTA DEL PROYECTO FINAL — CASTLEVANIA 3D

---

**UNIVERSIDAD NACIONAL AUTÓNOMA DE MÉXICO**  
**Facultad de Ingeniería**  
**División de Ingeniería Eléctrica**  
**Ingeniería en Computación**  
**Computación Gráfica e Interacción Humano Computadora**

**Semestre:** 2026-2  
**Fecha de entrega:** 14 de mayo de 2026

---

## 1. INTEGRANTES DEL EQUIPO

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
| Caballero Martínez Sergio Jair | 318074748 | sergio_jair4@hotmail.com |
| Martínez Jiménez Israel | 312032184 | paranoid12w@gmail.com |

---

## 2. VIDEOJUEGO SELECCIONADO

**Castlevania** — Konami, 1986, Nintendo Entertainment System (NES)

---

## 3. JUSTIFICACIÓN DEL VIDEOJUEGO ELEGIDO

*Castlevania* fue seleccionado como base del proyecto por cinco razones principales:

1. **Riqueza escenográfica aplicable al curso.** La arquitectura gótica y medieval del castillo de Drácula constituye un entorno ideal para aplicar de manera integral los temas del curso: iluminación local (Phong multifuente), texturizado con mapeo UV y mapas normales, transformaciones geométricas 3D, sistema de cámaras y animación por keyframes. Cada elemento del escenario obliga al equipo a tomar decisiones técnicas concretas y documentables.

2. **Importancia histórica dentro de la industria.** Desde su lanzamiento en 1986, *Castlevania* se consolidó como uno de los títulos más influyentes de la historia de los videojuegos, dando origen a un género propio (Metroidvania). Esta relevancia cultural aporta contexto y motivación académica sólidos.

3. **Viabilidad técnica para el equipo.** El diseño 2D original libera al equipo de restricciones de fidelidad absoluta y permite una interpretación artística 3D propia, concentrando el esfuerzo en la implementación gráfica en lugar de la reproducción exacta de assets.

4. **Diferenciación respecto a títulos habituales.** A diferencia de proyectos académicos recurrentes (mapa de ciudad, habitación genérica), la ambientación de castillo ofrece elementos visuales complejos: escaleras, candelabros, vitrales, alfombras, pilares y efectos de antorcha. Esto obliga al equipo a resolver problemas de iluminación dinámica, transparencias y composición de escena que otros escenarios no plantean.

5. **Conocimiento previo del equipo.** Ambos integrantes cuentan con familiaridad con el título, facilitando las decisiones de diseño y garantizando coherencia estética durante el desarrollo.

---

## 4. DESCRIPCIÓN GENERAL DEL ESCENARIO

### 4.1 Composición del escenario

El escenario recrea el **salón del trono del castillo de Drácula** en tres dimensiones. Está compuesto por los siguientes assets:

| Asset | Descripción |
|---|---|
| **Paredes** | Muros de piedra que delimitan el salón principal |
| **Piso** | Suelo empedrado con textura de piedra |
| **Techo** | Bóveda del salón |
| **Escaleras** | Acceso elevado hacia el área del trono |
| **Pilares** | Columnas decorativas |
| **Alfombra** | Alfombra roja extendida desde la entrada hasta el trono |
| **Candelabros (Chandelier)** | Iluminación de techo |
| **Puerta** | Acceso principal al salón |
| **Corredor** | Pasillo lateral con ventanas enrejadas |
| **Muros auxiliares (Mini Walls)** | Elementos de separación interna |
| **Ventana vidriera (Stained Window)** | Ventana circular de vidrio emplomado, efecto emisivo |
| **Trono** | Silla principal del salón |
| **Simon Belmont** | Personaje protagonista, controlable |
| **Sirviente de Drácula** | NPC estático ubicado junto al trono |

### 4.2 Dimensiones del escenario

Las dimensiones del modelo exportado (`castle_escenario.obj`) son:

| Eje | Mínimo | Máximo | Rango |
|---|---|---|---|
| X (ancho) | −0.604 | 1.041 | ~1.65 unidades |
| Y (alto) | −0.022 | 2.847 | ~2.87 unidades |
| Z (profundidad) | −2.069 | 4.856 | ~6.93 unidades |

### 4.3 Sistema de iluminación

Se implementó un modelo de iluminación **Phong multifuente** mediante shaders GLSL con tres tipos de luces simultáneas:

| Tipo | Cantidad | Descripción |
|---|---|---|
| **Luz direccional** | 1 | Iluminación ambiental base; simula luz difusa del entorno |
| **Luces puntuales** | 4 | Representan antorchas; distribución interior del salón; activables con efecto de parpadeo (*flicker*) mediante SPACE |
| **Spotlight** | 1 | Ligado a la cámara del jugador; simula linterna o foco de visión personal |

### 4.4 Sistema de cámara

| Modo | Activación | Descripción |
|---|---|---|
| **Primera persona** | F1 | Cámara libre, el jugador explora el escenario directamente |
| **Tercera persona** | F2 | Cámara orbital alrededor de Simon Belmont; controlable con el mouse |

### 4.5 Controles

| Tecla | Función |
|---|---|
| W / ↑ | Avanzar |
| S / ↓ | Retroceder |
| A / ← | Girar izquierda (3ª persona) / desplazar izquierda (1ª persona) |
| D / → | Girar derecha (3ª persona) / desplazar derecha (1ª persona) |
| F1 | Cámara primera persona |
| F2 | Cámara tercera persona |
| SPACE | Encender / apagar antorchas |
| K | Guardar keyframe de Simon Belmont |
| L | Reproducir animación keyframe |
| ESC | Salir |

### 4.6 Stack tecnológico

| Componente | Tecnología |
|---|---|
| Lenguaje | C++17 |
| API gráfica | OpenGL 3.3 Core Profile |
| Ventana y entrada | GLFW 3.3 |
| Carga de extensiones | GLEW |
| Matemáticas 3D | GLM (OpenGL Mathematics) |
| Carga de modelos | Assimp 5.x |
| Carga de texturas | SOIL2, stb_image |
| Shaders | GLSL (Vertex + Fragment Shader) |
| Modelado | Blender 4.x (exportación OBJ/MTL) |
| IDE / compilador | Visual Studio 2022, MSVC v143 |

---

## 5. METODOLOGÍA DE DESARROLLO

### 5.1 Metodología seleccionada: Iterativa Incremental (adaptada a equipo reducido)

Se adoptó una **metodología iterativa incremental** con ciclos de trabajo semanales, inspirada en los principios del Manifiesto Ágil y ajustada a las restricciones propias de un proyecto académico con dos integrantes y fecha de entrega fija.

Cada iteración produjo un incremento funcional y demostrable del software, lo que permitió al equipo recibir retroalimentación visual continua (fundamental en un proyecto de gráficas) y ajustar prioridades conforme se avanzó en los temas del curso.

#### Ciclos de desarrollo

| Iteración | Semanas | Contenido | Entregable verificable |
|---|---|---|---|
| 1 | 1–2 | Configuración del entorno, estructura del proyecto Visual Studio, carga y renderizado básico de modelos OBJ con Assimp | Modelos visibles sin iluminación |
| 2 | 3–4 | Implementación del modelo de iluminación Phong: luz direccional, luces puntuales y spotlight; shaders GLSL | Escena con iluminación correcta |
| 3 | 5–6 | Texturizado: carga de mapas difusos y de normales, samplers en shaders, corrección UV | Modelos completamente texturizados |
| 4 | 7–8 | Sistema de cámara de primera y tercera persona; rotación orbital con mouse; sincronización con el personaje | Cambio de cámara funcional (F1/F2) |
| 5 | 9–10 | Animación de Simon Belmont por keyframes: guardado, interpolación lineal y reproducción | Animación de trayectoria reproducible |
| 6 | 11–12 | Ajuste de límites de movimiento, posicionamiento correcto de assets, pruebas de calidad y documentación | Proyecto integrado y entregable |

### 5.2 Comparativa con otras metodologías

| Metodología | Ventajas en este contexto | Desventajas en este contexto | Motivo de descarte |
|---|---|---|---|
| **Cascada (Waterfall)** | Simple, bien documentada, adecuada para requisitos completamente fijos desde el inicio | Los requisitos visuales evolucionan con el aprendizaje del curso; errores de diseño gráfico detectados en fases tardías son costosos de corregir | No es flexible ante cambios visuales iterativos |
| **Scrum** | Sprints de duración fija, roles definidos (PO, SM, Dev Team), retrospectivas formales | El overhead administrativo (Daily Scrum, Sprint Review, Backlog Refinement) es desproporcionado para un equipo de dos personas sin cliente externo | Exceso de ceremonias para el tamaño del equipo |
| **Kanban** | Flujo continuo, tablero de trabajo visible, sin iteraciones forzadas | La ausencia de sprints dificulta la planificación con fechas de entrega académicas fijas; el WIP ilimitado puede causar acumulación de trabajo sin cierre formal | No garantiza entregas parciales en fechas específicas del curso |
| **Extreme Programming (XP)** | Énfasis en calidad del código, pair programming como práctica central | Prácticas como TDD no se aplican naturalmente a gráficas 3D; diseñado para equipos mayores con cliente activo | Las prácticas XP no son compatibles con el dominio gráfico del proyecto |
| **Iterativa Incremental ✓** | Ciclos cortos con verificación visual, mínima burocracia, fácil coordinación entre dos personas, permite priorización dinámica por temas del curso | Requiere disciplina para no saltar iteraciones; sin retrospectivas formales, los problemas pueden acumularse | **Seleccionada** |

**Justificación de la elección:** La naturaleza visual del proyecto exige *feedback* inmediato ante cualquier cambio. Un ajuste en la posición de una luz, la escala de un modelo o el comportamiento de una cámara debe verse y validarse en tiempo real. La metodología iterativa incremental facilita exactamente este flujo de trabajo, con una carga administrativa mínima y sin sacrificar la trazabilidad de los avances.

### 5.3 Roles asignados

#### Caballero Martínez Sergio Jair — *Artista Técnico / Diseñador 3D*

| Área | Responsabilidades específicas |
|---|---|
| Modelado | Creación y refinamiento de assets 3D en Blender (paredes, escaleras, trono, pilares, candelabros, ventana, puerta, alfombra, corredor, personajes) |
| Exportación | Configuración de escala, origen y orientación de los modelos para su correcta carga con Assimp; generación de archivos OBJ/MTL y mapas UV |
| Materiales y texturas | Definición de materiales en Blender; bake de mapas de normales y emisivos; selección y asignación de texturas |
| Layout del escenario | Composición y distribución de assets dentro del salón para coherencia visual con el referente original |
| Integración visual | Verificación de que los modelos exportados se despliegan correctamente dentro del motor; ajuste de escala relativa entre assets |

#### Martínez Jiménez Israel — *Desarrollador Gráfico / Programador*

| Área | Responsabilidades específicas |
|---|---|
| Pipeline OpenGL | Configuración del contexto OpenGL, GLFW, GLEW; ciclo de renderizado (*game loop*); gestión de VAO/VBO |
| Shaders GLSL | Implementación del Vertex Shader y Fragment Shader; modelo de iluminación Phong con luz direccional, luces puntuales (atenuación cuadrática) y spotlight |
| Sistema de cámara | Cámara de primera persona con procesamiento de teclado y mouse; cámara orbital de tercera persona alrededor del personaje |
| Animación | Sistema de keyframes para Simon Belmont: captura de posición/rotación, interpolación lineal, reproducción con control de pasos |
| Control y movimiento | Manejo de entrada de teclado y mouse (GLFW callbacks); movimiento del personaje con restricción de límites del escenario |
| Integración de bibliotecas | Configuración de Assimp, SOIL2, stb_image, GLM dentro del proyecto Visual Studio |

> Ambos integrantes participaron en pruebas funcionales, depuración y toma de decisiones de diseño a lo largo de todas las iteraciones.

---

## 6. BIBLIOGRAFÍA

De Vries, J. (2020). *LearnOpenGL: Learn modern OpenGL graphics programming in a step-by-step fashion*. Kendall & Welling. Recuperado de https://learnopengl.com

Sellers, G., Wright, R. S., & Haemel, N. (2015). *OpenGL SuperBible: Comprehensive Tutorial and Reference* (7.ª ed.). Addison-Wesley Professional.

Akenine-Möller, T., Haines, E., Hoffman, N., Pesce, A., Iwanicki, M., & Hillaire, S. (2018). *Real-Time Rendering* (4.ª ed.). A K Peters / CRC Press.

Shirley, P., & Marschner, S. (2009). *Fundamentals of Computer Graphics* (3.ª ed.). A K Peters / CRC Press.

Foley, J. D., van Dam, A., Feiner, S. K., & Hughes, J. F. (1996). *Computer Graphics: Principles and Practice* (2.ª ed.). Addison-Wesley.

Lengyel, E. (2016). *Foundations of Game Engine Development, Volume 1: Mathematics*. Terathon Software.

Blender Foundation. (2024). *Blender 4.x Reference Manual*. Recuperado de https://docs.blender.org

GLFW Development Team. (2023). *GLFW Documentation — An OpenGL library*. Recuperado de https://www.glfw.org/docs/latest/

Assimp Development Team. (2023). *Open Asset Import Library (Assimp) — Documentation*. Recuperado de https://assimp-docs.readthedocs.io

Beck, K., Beedle, M., van Bennekum, A., Cockburn, A., Cunningham, W., Fowler, M., … Thomas, D. (2001). *Manifesto for Agile Software Development*. Recuperado de https://agilemanifesto.org

Pressman, R. S., & Maxim, B. R. (2015). *Ingeniería del Software: Un enfoque práctico* (8.ª ed.). McGraw-Hill Education.

---

*Documento generado para la materia Computación Gráfica e Interacción Humano Computadora — Facultad de Ingeniería, UNAM — Semestre 2026-2.*
