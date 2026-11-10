#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderClass.h"
#include "Camera.h"
#include "ModelGLTF.h"
#include "AnimatedModelGLTF.h"
#include "Iluminacion.h"
#include "Bezier.h"


int main()
{
    // =====================================================

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


    // =====================================================
    // 2. CONFIGURACION OPENGL
    // =====================================================

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);


    // =====================================================
    // 3. SHADER E ILUMINACION DEL CASTILLO
    // =====================================================

    Shader shaderProgram(
        "default.vert",
        "default.frag"
    );

    SunLight sun(shaderProgram.ID);


    // =====================================================
    // 4. CARGAR CASTILLO
    // =====================================================

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


    // =====================================================
    // 5. SHADER DEL DRAGON
    // =====================================================

    Shader dragonShader(
        "dragon.vert",
        "dragon.frag"
    );


    // =====================================================
    // 6. CARGAR MOUNTAIN DRAGON
    // =====================================================

    AnimatedModelGLTF dragon;

    std::cout << "Cargando Mountain Dragon..." << std::endl;

    if (!dragon.Load("models/MountainDragon/scene.gltf"))
    {
        std::cout << "ERROR: No se pudo cargar Mountain Dragon" << std::endl;

        modelGLTF.Delete();

        shaderProgram.Delete();
        dragonShader.Delete();

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    std::cout << "Mountain Dragon cargado correctamente" << std::endl;

    std::cout << "Animacion: "
        << dragon.GetAnimationName() << std::endl;

    std::cout << "Duracion: "
        << dragon.GetDuration() << " segundos" << std::endl;


    // =====================================================
    // 7. CAMARA
    // =====================================================

    Camera camera(
        800,
        800,
        glm::vec3(0.0f, 10.0f, 40.0f)
    );

    camera.SetupInputs(window);


    // =====================================================
    // 8. UNIFORMS DEL CASTILLO
    // =====================================================

    GLint tintLoc = glGetUniformLocation(
        shaderProgram.ID,
        "uColorTint"
    );

    GLint modelLoc = glGetUniformLocation(
        shaderProgram.ID,
        "uModel"
    );


    // =====================================================
    // 9. UNIFORMS DEL DRAGON
    // =====================================================

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


    // =====================================================
    // 10. CONTROL DE ANIMACION
    // =====================================================

    float lastFrame = 0.0f;

    bool previousP = false;
    bool previousR = false;
    bool previousLeftBracket = false;
    bool previousRightBracket = false;

    // Tiempos originales de Take 001

    const float startTime = 0.0f;
    const float takeoffTime = 21.07f;
    const float flightEndTime = 41.62f;


    // Iniciar directamente con la animacion de vuelo
    dragon.SetAnimationRange(
        21.07f,
        41.62f,
        true
    );

    // El dragon comienza volando
    bool flightStarted = true;

    float flightTime = 0.0f;

    // Duracion de una vuelta completa
    const float flightDuration = 20.55f;


    // =====================================================
    // 11. CONFIGURACION DE BEZIER
    // =====================================================

    // Centro aproximado del castillo

    glm::vec3 center(
        0.0f,
        0.0f,
        0.0f
    );

    // Radio y altura del vuelo

    const float flightRadius = 35.0f;
    const float flightHeight = 12.0f;

    const float k = 0.55228475f;

    float r = flightRadius;
    float h = flightHeight;


    // Curva 1: frente hacia derecha

    Bezier curve1(
        center + glm::vec3(0, h, r),
        center + glm::vec3(k * r, h, r),
        center + glm::vec3(r, h, k * r),
        center + glm::vec3(r, h, 0)
    );


    // Curva 2: derecha hacia atras

    Bezier curve2(
        center + glm::vec3(r, h, 0),
        center + glm::vec3(r, h, -k * r),
        center + glm::vec3(k * r, h, -r),
        center + glm::vec3(0, h, -r)
    );


    // Curva 3: atras hacia izquierda

    Bezier curve3(
        center + glm::vec3(0, h, -r),
        center + glm::vec3(-k * r, h, -r),
        center + glm::vec3(-r, h, -k * r),
        center + glm::vec3(-r, h, 0)
    );


    // Curva 4: izquierda hacia frente

    Bezier curve4(
        center + glm::vec3(-r, h, 0),
        center + glm::vec3(-r, h, k * r),
        center + glm::vec3(-k * r, h, r),
        center + glm::vec3(0, h, r)
    );


    // =====================================================
    // 12. BUCLE PRINCIPAL
    // =====================================================

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame =
            static_cast<float>(glfwGetTime());

        float deltaTime = currentFrame - lastFrame;

        lastFrame = currentFrame;


        // Actualizar camara

        camera.ProcessInputs(window, deltaTime);


        // =================================================
        // 13. CONTROLES MANUALES
        // =================================================

        bool currentP =
            glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;

        bool currentR =
            glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

        bool currentLeftBracket =
            glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;

        bool currentRightBracket =
            glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;


        // P: pausar o continuar

        if (currentP && !previousP)
        {
            dragon.SetPaused(!dragon.IsPaused());

            std::cout << "Animacion: "
                << (dragon.IsPaused() ? "PAUSADA" : "REPRODUCIENDO")
                << std::endl;
        }


        // R: reiniciar toda la secuencia

        if (currentR && !previousR)
        {
            flightTime = 0.0f;

            dragon.SetAnimationRange(
                21.07f,
                40.00f,
                true
            );

            std::cout << "Vuelo reiniciado" << std::endl;
        }


        // [: retroceder

        if (currentLeftBracket && !previousLeftBracket)
        {
            dragon.SetPaused(true);

            float newTime =
                dragon.GetAnimationTime() - 0.1f;

            dragon.SetAnimationTime(newTime);

            std::cout << "Tiempo: "
                << dragon.GetAnimationTime()
                << " segundos" << std::endl;
        }


        // ]: avanzar

        if (currentRightBracket && !previousRightBracket)
        {
            dragon.SetPaused(true);

            float newTime =
                dragon.GetAnimationTime() + 0.1f;

            dragon.SetAnimationTime(newTime);

            std::cout << "Tiempo: "
                << dragon.GetAnimationTime()
                << " segundos" << std::endl;
        }


        previousP = currentP;
        previousR = currentR;
        previousLeftBracket = currentLeftBracket;
        previousRightBracket = currentRightBracket;


        // =================================================
        // 14. ACTUALIZAR VENTANA
        // =================================================

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


        // Limpiar pantalla

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


        // =============================================
        // ANIMACION CONTINUA DEL VUELO
        // =============================================

        dragon.Update(deltaTime);

        // Actualizar trayectoria Bezier
        if (!dragon.IsPaused())
        {
            flightTime += deltaTime;
        }

        // =================================================
        // 16. TRANSFORMACION DEL CASTILLO
        // =================================================

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


        // =================================================
        // 17. RENDERIZAR CASTILLO
        // =================================================

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


        // =================================================
        // 18. MATRICES DE CAMARA
        // =================================================

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection =
            camera.GetProjectionMatrix(
                60.0f,
                0.1f,
                3000.0f
            );

        glm::mat4 viewProjection =
            projection * view;


        // =================================================
        // 19. TRANSFORMACION DEL DRAGON
        // =================================================

        glm::mat4 dragonModel(1.0f);

        if (!flightStarted)
        {
            // Posicion inicial del dragon

            dragonModel = glm::translate(
                dragonModel,
                glm::vec3(0.0f, 3.0f, 0.0f)
            );
        }
        else
        {
            // Tiempo normalizado del circuito

            float t = std::fmod(
                flightTime / flightDuration,
                1.0f
            );

            glm::vec3 position(0.0f);
            glm::vec3 tangent(0.0f);


            // Seleccionar curva correspondiente

            if (t < 0.25f)
            {
                float localT = t * 4.0f;

                position = curve1.GetPosition(localT);
                tangent = curve1.GetTangent(localT);
            }
            else if (t < 0.50f)
            {
                float localT = (t - 0.25f) * 4.0f;

                position = curve2.GetPosition(localT);
                tangent = curve2.GetTangent(localT);
            }
            else if (t < 0.75f)
            {
                float localT = (t - 0.50f) * 4.0f;

                position = curve3.GetPosition(localT);
                tangent = curve3.GetTangent(localT);
            }
            else
            {
                float localT = (t - 0.75f) * 4.0f;

                position = curve4.GetPosition(localT);
                tangent = curve4.GetTangent(localT);
            }


            // Orientacion horizontal

            float yaw = atan2(
                tangent.x,
                tangent.z
            );


            // Aplicar posicion

            dragonModel = glm::translate(
                dragonModel,
                position
            );


            // Aplicar orientacion

            dragonModel = glm::rotate(
                dragonModel,
                yaw,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        }


        // Escala del dragon

        dragonModel = glm::scale(
            dragonModel,
            glm::vec3(0.025f)
        );


        // =================================================
        // 20. RENDERIZAR MOUNTAIN DRAGON
        // =================================================

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


        // =================================================
        // 21. ACTUALIZAR VENTANA
        // =================================================

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // =====================================================
    // 22. LIBERAR RECURSOS
    // =====================================================

    dragon.Delete();

    modelGLTF.Delete();

    dragonShader.Delete();

    shaderProgram.Delete();


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
