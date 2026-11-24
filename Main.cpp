#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Interfaz.h"
#include "ShaderClass.h"
#include "Camera.h"
#include "ModelGLTF.h"
#include "AnimatedModelGLTF.h"
#include "Iluminacion.h"
#include "Bezier.h"
#include "DragonController.h"

int main()
{
    // =============================================
    // 1. INICIALIZAR GLFW
    // =============================================

    if (!glfwInit())
    {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        2000, 900,
        "Castillo y Mountain Dragon",
        NULL, NULL
    );

    if (!window)
    {
        std::cout << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        std::cout << "Error al inicializar GLAD" << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    std::cout << "OpenGL inicializado correctamente" << std::endl;


    // =============================================
    // 2. CONFIGURACION OPENGL
    // =============================================

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);


    // =============================================
    // 3. SHADER E ILUMINACION DEL CASTILLO
    // =============================================

    Shader shaderProgram(
        "default.vert",
        "default.frag"
    );

    SunLight sun(shaderProgram.ID);


    // =============================================
    // 4. CARGAR CASTILLO
    // =============================================

    ModelGLTF modelGLTF;

    std::cout << "Cargando castillo..." << std::endl;

    if (!modelGLTF.Load("models/scene.gltf"))
    {
        std::cout << "ERROR: No se pudo cargar el castillo" << std::endl;

        shaderProgram.Delete();

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    std::cout << "Castillo cargado correctamente" << std::endl;


    // =============================================
    // 5. SHADERS DEL DRAGON Y BEZIER
    // =============================================

    Shader dragonShader(
        "dragon.vert",
        "dragon.frag"
    );

    Shader curveShader(
        "curve.vert",
        "curve.frag"
    );


    // =============================================
    // 6. CARGAR MOUNTAIN DRAGON
    // =============================================

    AnimatedModelGLTF dragon;

    std::cout << "Cargando Mountain Dragon..." << std::endl;

    if (!dragon.Load("models/MountainDragon/scene.gltf"))
    {
        std::cout << "ERROR: No se pudo cargar Mountain Dragon" << std::endl;

        modelGLTF.Delete();

        shaderProgram.Delete();
        dragonShader.Delete();
        curveShader.Delete();

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    std::cout << "Mountain Dragon cargado correctamente" << std::endl;

    std::cout << "Animacion: "
        << dragon.GetAnimationName() << std::endl;

    std::cout << "Duracion: "
        << dragon.GetDuration() << " segundos" << std::endl;


    // =============================================
    // 7. CAMARA
    // =============================================

    Camera camera(
        800,
        800,
        glm::vec3(0.0f, 10.0f, 40.0f)
    );

    camera.SetupInputs(window);

    bool cameraMode = true;
    bool previousTab = false;


    // =============================================
    // 8. INICIALIZAR INTERFAZ
    // =============================================

    Interfaz interfaz;

    interfaz.Initialize(window);


    // =============================================
    // 9. UNIFORMS DEL CASTILLO
    // =============================================

    GLint tintLoc = glGetUniformLocation(
        shaderProgram.ID,
        "uColorTint"
    );

    GLint modelLoc = glGetUniformLocation(
        shaderProgram.ID,
        "uModel"
    );


    // =============================================
    // 10. UNIFORMS DEL DRAGON
    // =============================================

    GLint dragonSunDirection = glGetUniformLocation(
        dragonShader.ID,
        "sunDirection"
    );

    GLint dragonSunColor = glGetUniformLocation(
        dragonShader.ID,
        "sunColor"
    );

    GLint dragonAmbient = glGetUniformLocation(
        dragonShader.ID,
        "ambientStrength"
    );


    // =============================================
    // 11. CONFIGURAR ANIMACION
    // =============================================

    float lastFrame = 0.0f;

    bool previousP = false;
    bool previousR = false;
    bool previousLeftBracket = false;
    bool previousRightBracket = false;

    const float takeoffTime = 21.07f;
    const float flightEndTime = 41.00f;
    const float flightDuration = 20.55f;


    // =============================================
    // 12. CREAR TRAYECTORIA BEZIER
    // =============================================

    glm::vec3 center(0.0f, 0.0f, 0.0f);

    BezierPath dragonPath(
        center,
        35.0f,
        12.0f,
        80
    );


    // =============================================
    // 13. CREAR CONTROLADOR DEL DRAGON
    // =============================================

    DragonController dragonController(
        dragon,
        dragonPath,
        takeoffTime,
        flightEndTime,
        flightDuration,
        0.025f
    );


    // =============================================
    // 14. BUCLE PRINCIPAL
    // =============================================

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame =
            static_cast<float>(glfwGetTime());

        float deltaTime = currentFrame - lastFrame;

        lastFrame = currentFrame;


        // =========================================
        // 15. INICIAR FRAME DE INTERFAZ
        // =========================================

        interfaz.BeginFrame();


        // =========================================
        // 16. DIBUJAR PANEL DEL DRAGON
        // =========================================

        interfaz.Draw(dragonController);


        // =========================================
        // 17. CAMBIAR MODO DE CAMARA CON TAB
        // =========================================

        bool currentTab =
            glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

        if (currentTab && !previousTab)
        {
            cameraMode = !cameraMode;

            camera.SetCameraMode(window, cameraMode);

            std::cout << "Modo actual: "
                << (cameraMode ? "CAMARA 3D" : "INTERFAZ IMGUI")
                << std::endl;
        }

        previousTab = currentTab;


        // =========================================
        // 18. ACTUALIZAR CAMARA
        // =========================================

        if (cameraMode &&
            !interfaz.WantsCaptureKeyboard() &&
            !interfaz.WantsCaptureMouse())
        {
            camera.ProcessInputs(window, deltaTime);
        }


        // =========================================
        // 19. CONTROLES DE TECLADO
        // =========================================

        bool currentP =
            !interfaz.WantsCaptureKeyboard() &&
            glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;

        bool currentR =
            !interfaz.WantsCaptureKeyboard() &&
            glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

        bool currentLeftBracket =
            !interfaz.WantsCaptureKeyboard() &&
            glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;

        bool currentRightBracket =
            !interfaz.WantsCaptureKeyboard() &&
            glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;


        // P: pausar o continuar vuelo y alas

        if (currentP && !previousP)
        {
            bool pause = !dragonController.AreWingsPaused();

            dragonController.SetWingsPaused(pause);
            dragonController.SetFlightPaused(pause);

            std::cout << "Animacion: "
                << (pause ? "PAUSADA" : "REPRODUCIENDO")
                << std::endl;
        }


        // R: reiniciar recorrido

        if (currentR && !previousR)
        {
            dragonController.Reset();

            std::cout << "Vuelo reiniciado" << std::endl;
        }


        // [: retroceder animacion

        if (currentLeftBracket && !previousLeftBracket)
        {
            dragonController.SetWingsPaused(true);
            dragonController.SetFlightPaused(true);

            float newTime = dragon.GetAnimationTime() - 0.1f;

            dragon.SetAnimationTime(newTime);

            std::cout << "Tiempo: "
                << dragon.GetAnimationTime()
                << " segundos" << std::endl;
        }


        // ]: avanzar animacion

        if (currentRightBracket && !previousRightBracket)
        {
            dragonController.SetWingsPaused(true);
            dragonController.SetFlightPaused(true);

            float newTime = dragon.GetAnimationTime() + 0.1f;

            dragon.SetAnimationTime(newTime);

            std::cout << "Tiempo: "
                << dragon.GetAnimationTime()
                << " segundos" << std::endl;
        }


        // Guardar estado de las teclas

        previousP = currentP;
        previousR = currentR;
        previousLeftBracket = currentLeftBracket;
        previousRightBracket = currentRightBracket;


        // =========================================
        // 20. ACTUALIZAR VENTANA
        // =========================================

        int width, height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        if (height == 0)
            height = 1;

        camera.width = width;
        camera.height = height;

        glViewport(0, 0, width, height);


        // =========================================
        // 21. LIMPIAR PANTALLA
        // =========================================

        glClearColor(
            0.58f,
            0.68f,
            0.78f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // =========================================
        // 22. ACTUALIZAR CONTROLADOR DEL DRAGON
        // =========================================

        dragonController.Update(deltaTime);


        // =========================================
        // 23. TRANSFORMACION DEL CASTILLO
        // =========================================

        glm::mat4 model(1.0f);

        model = glm::rotate(
            model,
            glm::radians(0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(220.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        model = glm::scale(
            model,
            glm::vec3(0.1f)
        );


        // =========================================
        // 24. RENDERIZAR CASTILLO
        // =========================================

        shaderProgram.Activate();

        camera.UpdateMatrix(
            shaderProgram,
            "uMVP",
            model,
            60.0f,
            0.1f,
            3000.0f
        );

        sun.Update(deltaTime);
        sun.Apply();

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniform4f(
            tintLoc,
            1.0f, 1.0f, 1.0f, 1.0f
        );

        glDepthMask(GL_TRUE);

        modelGLTF.Draw(shaderProgram);


        // =========================================
        // 25. MATRICES DE CAMARA
        // =========================================

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection =
            camera.GetProjectionMatrix(
                60.0f,
                0.1f,
                3000.0f
            );

        glm::mat4 viewProjection = projection * view;


        // =========================================
        // 26. DIBUJAR TRAYECTORIA BEZIER
        // =========================================

        dragonPath.Draw(
            curveShader.ID,
            viewProjection,
            interfaz.GetShowTrajectory()
        );


        // =========================================
        // 27. TRANSFORMACION DEL DRAGON
        // =========================================

        glm::mat4 dragonModel =
            dragonController.GetModelMatrix();


        // =========================================
        // 28. RENDERIZAR MOUNTAIN DRAGON
        // =========================================

        dragonShader.Activate();

        glUniform3f(
            dragonSunDirection,
            0.0f, 1.0f, 0.3f
        );

        glUniform3f(
            dragonSunColor,
            1.0f, 0.95f, 0.85f
        );

        glUniform1f(
            dragonAmbient,
            0.55f
        );

        glDepthMask(GL_TRUE);

        dragon.Draw(
            dragonShader.ID,
            viewProjection,
            dragonModel
        );


        // =========================================
        // 29. RENDERIZAR INTERFAZ
        // =========================================

        interfaz.Render();


        // =========================================
        // 30. ACTUALIZAR VENTANA
        // =========================================

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // =============================================
    // 31. LIBERAR RECURSOS
    // =============================================

    interfaz.Shutdown();

    dragonPath.Delete();

    curveShader.Delete();

    dragon.Delete();

    modelGLTF.Delete();

    dragonShader.Delete();

    shaderProgram.Delete();


    // =============================================
    // 32. FINALIZAR GLFW
    // =============================================

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
