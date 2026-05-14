
#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// ─── Prototypes ───────────────────────────────────────────────────────────────
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

// ─── Window ───────────────────────────────────────────────────────────────────
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// ─── Camera ───────────────────────────────────────────────────────────────────
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool    keys[1024];
bool    firstMouse = true;

// Camera mode: 1 = first-person,  2 = third-person (follows Simon)
int cameraMode = 1;

// Third-person orbit parameters (controlled with mouse in 3rd-person mode)
float tpYaw = 0.0f;
float tpPitch = 30.0f;
float tpDistance = 6.0f;

// ─── Timing ───────────────────────────────────────────────────────────────────
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ─── Lighting ─────────────────────────────────────────────────────────────────
bool      active = false;
glm::vec3 Light1 = glm::vec3(0.0f);

// 4 point lights placed inside the castle
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f, 3.0f,  0.0f),
    glm::vec3(2.0f, 2.0f,  3.0f),
    glm::vec3(-2.0f, 2.0f, -3.0f),
    glm::vec3(0.0f, 2.0f, -5.0f)
};

// Cube geometry for rendering the light-source cubes
float vertices[] = {
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
     0.5f,-0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
    -0.5f, 0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f,-1.0f,

    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, 0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f,-0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f, 0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,-1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f,-0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f,-0.5f,-0.5f, 0.0f,-1.0f, 0.0f,
     0.5f,-0.5f,-0.5f, 0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f, 0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f, 0.0f,-1.0f, 0.0f,
    -0.5f,-0.5f, 0.5f, 0.0f,-1.0f, 0.0f,
    -0.5f,-0.5f,-0.5f, 0.0f,-1.0f, 0.0f,

    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f
};

// ─── Simon Belmont (personaje principal, animado por keyframes) ───────────────
float rotSimon = 0.0f;
float simonPosX = 0.0f;
float simonPosY = 0.0f;
float simonPosZ = 2.0f;

// ─── Limites del cuarto (ajusta si Simon choca antes o atraviesa paredes) ─────
const float ROOM_MIN_X = -5.5f, ROOM_MAX_X = 5.5f;
const float ROOM_MIN_Z = -12.0f, ROOM_MAX_Z = 9.5f;

// ─── Dracula Servant (enemigo, estatico en el trono) ──────────────────────────
// Ajusta estas constantes para moverlo al trono exacto
const float SERVANT_X = 0.09f;
const float SERVANT_Y = 0.0f;
const float SERVANT_Z = 8.3f;
const float SERVANT_ROT = 0.0f;    // cara hacia el jugador; ajustar en +90 si es necesario

// ─── KeyFrame animation (para Simon Belmont) ──────────────────────────────────
#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;

typedef struct _frame {
    float rot;
    float rotInc;
    float posX;
    float posY;
    float posZ;
    float incX;
    float incY;
    float incZ;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int  FrameIndex = 0;
bool play = false;
int  playIndex = 0;

void saveFrame()
{
    printf("Keyframe [%d] guardado: pos=(%.2f, %.2f, %.2f) rot=%.1f\n",
        FrameIndex, simonPosX, simonPosY, simonPosZ, rotSimon);
    KeyFrame[FrameIndex].posX = simonPosX;
    KeyFrame[FrameIndex].posY = simonPosY;
    KeyFrame[FrameIndex].posZ = simonPosZ;
    KeyFrame[FrameIndex].rot = rotSimon;
    FrameIndex++;
}

void resetElements()
{
    simonPosX = KeyFrame[0].posX;
    simonPosY = KeyFrame[0].posY;
    simonPosZ = KeyFrame[0].posZ;
    rotSimon = KeyFrame[0].rot;
}

void interpolation()
{
    KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
    KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
    KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
    KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rot - KeyFrame[playIndex].rot) / i_max_steps;
}

// ─── Helpers ──────────────────────────────────────────────────────────────────
void SetPointLights(Shader& shader, bool torchOn)
{
    // Warm base that is always visible; extra intensity when torch is active
    float base = torchOn ? 1.0f : 0.4f;
    float flick = torchOn ? (0.8f + 0.2f * sinf((float)glfwGetTime() * 7.0f)) : 1.0f;

    glm::vec3 diff = glm::vec3(1.0f, 0.75f, 0.3f) * base * flick;
    glm::vec3 amb = diff * 0.25f;

    for (int i = 0; i < 4; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "pointLights[%d].position", i);
        glUniform3f(glGetUniformLocation(shader.Program, buf),
            pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);

        snprintf(buf, sizeof(buf), "pointLights[%d].ambient", i);
        glUniform3f(glGetUniformLocation(shader.Program, buf), amb.x, amb.y, amb.z);

        snprintf(buf, sizeof(buf), "pointLights[%d].diffuse", i);
        glUniform3f(glGetUniformLocation(shader.Program, buf), diff.x, diff.y, diff.z);

        snprintf(buf, sizeof(buf), "pointLights[%d].specular", i);
        glUniform3f(glGetUniformLocation(shader.Program, buf), 0.5f, 0.4f, 0.1f);

        snprintf(buf, sizeof(buf), "pointLights[%d].constant", i);
        glUniform1f(glGetUniformLocation(shader.Program, buf), 1.0f);

        snprintf(buf, sizeof(buf), "pointLights[%d].linear", i);
        glUniform1f(glGetUniformLocation(shader.Program, buf), 0.14f);

        snprintf(buf, sizeof(buf), "pointLights[%d].quadratic", i);
        glUniform1f(glGetUniformLocation(shader.Program, buf), 0.07f);
    }
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT,
        "Castlevania 3D  |  F1=1ra Persona  F2=3ra Persona  K=Keyframe  L=Play  SPACE=Antorchas  ESC=Salir",
        nullptr, nullptr);

    if (!window) {
        std::cout << "Error al crear ventana GLFW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cout << "Error al inicializar GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ── Shaders ───────────────────────────────────────────────────────────────
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // ── Modelos del proyecto (GLB en la raiz del repositorio) ─────────────────
    // Los archivos GLB estan en c:\...\Proyecto\, un nivel arriba de Project1\

    Model Escenario((char*)"Models/castle_escenario.obj");
    Model Simon((char*)"Models/castle_simon_belmont.obj");
    Model Servant((char*)"Models/castle_dracula_servant.obj");

    // ── Init keyframes ────────────────────────────────────────────────────────
    for (int i = 0; i < MAX_FRAMES; i++) {
        KeyFrame[i] = { 0,0,0,0,0,0,0,0 };
    }

    // ── VAO/VBO para los cubos de luz ─────────────────────────────────────────
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

    // Far plane amplio para ver toda la escena
    glm::mat4 projection = glm::perspective(
        camera.GetZoom(),
        (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT,
        0.05f, 500.0f);

    // ── Game loop ─────────────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();
        Animation();

        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        lightingShader.Use();
        glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

        // ── Vista: primera o tercera persona ─────────────────────────────────
        glm::mat4 view;
        glm::vec3 viewerPos;

        if (cameraMode == 1)
        {
            view = camera.GetViewMatrix();
            viewerPos = camera.GetPosition();
        }
        else
        {
            // Camara orbita detras/encima de Simon Belmont
            glm::vec3 simonCenter(simonPosX, simonPosY + 0.8f, simonPosZ);
            float yawRad = glm::radians(rotSimon + tpYaw);
            float pitchRad = glm::radians(tpPitch);

            glm::vec3 offset(
                -sinf(yawRad) * tpDistance * cosf(pitchRad),
                sinf(pitchRad) * tpDistance + 0.3f,
                -cosf(yawRad) * tpDistance * cosf(pitchRad)
            );
            viewerPos = simonCenter + offset;
            view = glm::lookAt(viewerPos, simonCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"),
            viewerPos.x, viewerPos.y, viewerPos.z);

        // ── Luz direccional (iluminacion base, siempre visible) ───────────────
        // Valores altos para garantizar que la escena sea legible
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.3f, -1.0f, -0.5f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.50f, 0.45f, 0.40f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.80f, 0.75f, 0.65f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.30f, 0.28f, 0.20f);

        // ── Luces puntuales (antorchas del castillo) ──────────────────────────
        SetPointLights(lightingShader, active);

        // ── Spotlight (linterna/luz del jugador) ──────────────────────────────
        glm::vec3 spPos, spDir;
        if (cameraMode == 1) {
            spPos = camera.GetPosition();
            spDir = camera.GetFront();
        }
        else {
            spPos = glm::vec3(simonPosX, simonPosY + 0.8f, simonPosZ);
            spDir = glm::vec3(sinf(glm::radians(rotSimon)), 0.0f, cosf(glm::radians(rotSimon)));
        }
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), spPos.x, spPos.y, spPos.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), spDir.x, spDir.y, spDir.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 0.95f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(17.5f)));

        // Material
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

        // Matrices
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model(1.0f);

        // ── Escenario del castillo ────────────────────────────────────────────
        // Si la escena aparece muy grande o muy pequena ajusta el scale aqui
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        Escenario.Draw(lightingShader);

        // ── Simon Belmont: solo visible en 3ra persona ───────────────────────
        if (cameraMode == 2)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(simonPosX, simonPosY, simonPosZ));
            model = glm::rotate(model, glm::radians(rotSimon), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Simon.Draw(lightingShader);
        }

        // ── Dracula Servant (estatico en el trono) ────────────────────────────
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(SERVANT_X, SERVANT_Y, SERVANT_Z));
        model = glm::rotate(model, glm::radians(SERVANT_ROT), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        Servant.Draw(lightingShader);

        // ── Cubos de luz (lamp shader) ────────────────────────────────────────
        lampShader.Use();
        GLint lModelLoc = glGetUniformLocation(lampShader.Program, "model");
        GLint lViewLoc = glGetUniformLocation(lampShader.Program, "view");
        GLint lProjLoc = glGetUniformLocation(lampShader.Program, "projection");
        glUniformMatrix4fv(lViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(lProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        for (int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            glUniformMatrix4fv(lModelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ─── DoMovement ───────────────────────────────────────────────────────────────
void DoMovement()
{
    if (cameraMode == 1)
    {
        // Primera persona: WASD mueve la camara
        if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])   camera.ProcessKeyboard(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
        if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

        // Limites del cuarto para la camara en primera persona
        glm::vec3 pos = camera.GetPosition();
        pos.x = glm::clamp(pos.x, ROOM_MIN_X, ROOM_MAX_X);
        pos.z = glm::clamp(pos.z, ROOM_MIN_Z, ROOM_MAX_Z);
        pos.y = glm::clamp(pos.y, 0.5f, 3.0f);
        camera.SetPosition(pos);

        // Sincronizar Simon con la camara para que quede en la misma posicion al cambiar a 3ra persona
        simonPosX = pos.x;
        simonPosZ = pos.z;
        glm::vec3 f = camera.GetFront();
        rotSimon = glm::degrees(atan2f(f.x, f.z));
    }
    else
    {
        // Tercera persona: WASD mueve y rota a Simon Belmont
        const float speed = 2.5f * deltaTime;
        const float rotSpeed = 90.0f * deltaTime;

        if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
            simonPosX += sinf(glm::radians(rotSimon)) * speed;
            simonPosZ += cosf(glm::radians(rotSimon)) * speed;
        }
        if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
            simonPosX -= sinf(glm::radians(rotSimon)) * speed;
            simonPosZ -= cosf(glm::radians(rotSimon)) * speed;
        }
        if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  rotSimon += rotSpeed;
        if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])  rotSimon -= rotSpeed;

        // Limites del cuarto para Simon en tercera persona
        simonPosX = glm::clamp(simonPosX, ROOM_MIN_X, ROOM_MAX_X);
        simonPosZ = glm::clamp(simonPosZ, ROOM_MIN_Z, ROOM_MAX_Z);
    }

    // Mover las luces puntuales manualmente (siempre disponible)
    if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.05f;
    if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.05f;
    if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.05f;
    if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.05f;
    if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.05f;
    if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.05f;
}

// ─── KeyCallback ──────────────────────────────────────────────────────────────
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)   keys[key] = true;
        if (action == GLFW_RELEASE) keys[key] = false;
    }

    // Cambio de camara
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        cameraMode = 1;
        printf("Camara: Primera Persona\n");
    }
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        cameraMode = 2;
        printf("Camara: Tercera Persona (sigue a Simon Belmont)\n");
    }

    // Antorchas (punto de luz con parpadeo)
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        active = !active;
        printf("Antorchas: %s\n", active ? "ENCENDIDAS (efecto parpadeo)" : "APAGADAS");
    }

    // Guardar keyframe de Simon
    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        if (FrameIndex < MAX_FRAMES)
            saveFrame();
        else
            printf("Maximo de keyframes alcanzado (%d)\n", MAX_FRAMES);
    }

    // Reproducir animacion keyframe
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if (!play && FrameIndex > 1) {
            resetElements();
            interpolation();
            play = true;
            playIndex = 0;
            i_curr_steps = 0;
            printf("Reproduciendo animacion (%d keyframes)\n", FrameIndex);
        }
        else {
            play = false;
            printf("Animacion detenida\n");
        }
    }
}

// ─── Animation ────────────────────────────────────────────────────────────────
void Animation()
{
    // ── Keyframe playback para Simon Belmont ──────────────────────────────────
    if (!play) return;

    if (i_curr_steps >= i_max_steps)
    {
        playIndex++;
        if (playIndex > FrameIndex - 2) {
            printf("Animacion terminada\n");
            playIndex = 0;
            play = false;
        }
        else {
            i_curr_steps = 0;
            interpolation();
        }
    }
    else
    {
        simonPosX += KeyFrame[playIndex].incX;
        simonPosY += KeyFrame[playIndex].incY;
        simonPosZ += KeyFrame[playIndex].incZ;
        rotSimon += KeyFrame[playIndex].rotInc;
        i_curr_steps++;
    }
}

// ─── MouseCallback ────────────────────────────────────────────────────────────
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse) {
        lastX = (GLfloat)xPos;
        lastY = (GLfloat)yPos;
        firstMouse = false;
    }

    GLfloat xOffset = (GLfloat)xPos - lastX;
    GLfloat yOffset = lastY - (GLfloat)yPos;
    lastX = (GLfloat)xPos;
    lastY = (GLfloat)yPos;

    if (cameraMode == 1) {
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
    else {
        // En 3ra persona: mouse orbita la camara alrededor de Simon
        tpYaw += xOffset * 0.25f;
        tpPitch += yOffset * 0.20f;
        if (tpPitch > 80.0f) tpPitch = 80.0f;
        if (tpPitch < -15.0f) tpPitch = -15.0f;
    }
}
