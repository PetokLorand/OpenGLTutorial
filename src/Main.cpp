#include <cmath>
#include <optional>
#include <string_view>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.hpp"
#include "Model.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "Camera.hpp"


uint32_t g_width = 800;
uint32_t g_height = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = g_width / 2;
float lastY = g_height / 2;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void frameBufferSizeCallback(GLFWwindow* window, int widthIn, int heightIn)
{
    static_cast<void>(window);
    g_width = widthIn;
    g_height = heightIn;
    glViewport(0, 0, widthIn, heightIn);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    static_cast<void>(window);

    static bool firstMouse = true;

    const float xpos = static_cast<float>(xposIn);
    const float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    static_cast<void>(window);
    static_cast<void>(xoffset);

    camera.processMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        static bool isFullscreen = false;

        if (isFullscreen)
        {

            glfwSetWindowMonitor(window, nullptr, 100, 100, g_width, g_height, 0);
            isFullscreen = false;
        }
        else
        {
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, g_width, g_height, 60);

            isFullscreen = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        static bool isCursorDisabled = false;

        if (isCursorDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isCursorDisabled = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isCursorDisabled = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Movement::Up, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Movement::Down, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Movement::Forward, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Movement::Backward, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Movement::Left, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Movement::Right, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Looking::Left, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Looking::Right, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Looking::Up, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera.processKeyboard(Camera::Looking::Down, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        static bool isWireframe = false;

        if (isWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            isWireframe = false;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            isWireframe = true;
        }
    }
}

int main()
{
    if (glfwInit() != GLFW_TRUE)
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", nullptr, nullptr);
    
    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, g_width, g_height);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    int result = glewInit();
    if (result != GLEW_OK)
    {
        log("[Error] GLEW initialization failed: {}", (char*)glewGetErrorString(result));
        return -1;
    }

    std::vector<Vertex> vertices
    {
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
        { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) }
    };

    std::vector<uint32_t> indices
    {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20
    };

    // auto diffuseMap = Texture::load("container2.jpg", "textures/", Texture::Type::Diffuse);
    // auto specularMap = Texture::load("container2_specular.png", "textures/", Texture::Type::Specular);

    // if (!diffuseMap || !specularMap)
    // {
    //     log("[Error] Texture loading failed");
    //     return -1;
    // }

    // std::vector<Texture> textures
    // {
    //     { diffuseMap->id, Texture::Type::Diffuse },
    //     { specularMap->id, Texture::Type::Specular }
    // };  

    // Mesh cubeMesh = Mesh::create(vertices, indices, textures);
    Mesh lightMesh = Mesh::create(vertices, indices, {});

    // glm::vec3 cubePositions[] =
    // {
    //     glm::vec3( 0.0f,  0.0f,  0.0f), 
    //     glm::vec3( 2.0f,  5.0f, -15.0f), 
    //     glm::vec3(-1.5f, -2.2f, -2.5f),  
    //     glm::vec3(-3.8f, -2.0f, -12.3f),  
    //     glm::vec3( 2.4f, -0.4f, -3.5f),  
    //     glm::vec3(-1.7f,  3.0f, -7.5f),  
    //     glm::vec3( 1.3f, -2.0f, -2.5f),  
    //     glm::vec3( 1.5f,  2.0f, -2.5f), 
    //     glm::vec3( 1.5f,  0.2f, -1.5f), 
    //     glm::vec3(-1.3f,  1.0f, -1.5f)  
    // };

    // // const float phi = (1 + std::sqrt(5)) / 2;

    // // float vertices[60] =
    // // {
    // //     phi, 1, 0, 0, 0,
    // //     -phi, 1, 0, 0, 0,
    // //     phi, -1, 0, 0, 0,
    // //     -phi, -1, 0, 0, 0,
    // //     1, 0, phi, 0, 0,
    // //     1, 0, -phi, 0, 0,
    // //     -1, 0, phi, 0, 0,
    // //     -1, 0, -phi, 0, 0,
    // //     0, phi, 1, 0, 0,
    // //     0, -phi, 1, 0, 0,
    // //     0, phi, -1, 0, 0,
    // //     0, -phi, -1, 0, 0
    // // };

    // // uint32_t indices[60] =
    // // {
    // //     0, 8, 4,
    // //     0, 5, 10,
    // //     2, 4, 9,
    // //     2, 11, 5,
    // //     1, 6, 8,
    // //     1, 10, 7,
    // //     3, 9, 6,
    // //     3, 7, 11,
    // //     0, 10, 8,
    // //     1, 8, 10,
    // //     2, 9, 11,
    // //     3, 9, 11,
    // //     4, 2, 0,
    // //     5, 0, 2,
    // //     6, 1, 3,
    // //     7, 3, 1,
    // //     8, 6, 4,
    // //     9, 4, 6,
    // //     10, 5, 7,
    // //     11, 7, 5
    // // };

    // auto shaderOpt = Shader::create("shaders/Cube.vs", "shaders/Cube.fs");

    // if (!shaderOpt)
    // {
    //     log("[Error] Shader program creation failed");
    //     return -1;
    // }

    // Shader cubeShader = std::move(*shaderOpt);

    // const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    // const glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);

    // cubeShader.use();
    // cubeShader.setVec3("objectColor", objectColor);

    // cubeShader.setInt("material.diffuse", 0);
    // cubeShader.setInt("material.specular", 1);
    // cubeShader.setFloat("material.shininess", 32.0f);

    // cubeShader.setVec3("light.position", lightPos);

    // cubeShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    // cubeShader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    // cubeShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    // cubeShader.setFloat("light.constant", 1.0f);
    // cubeShader.setFloat("light.linear", 0.09f);
    // cubeShader.setFloat("light.quadratic", 0.032f);

    auto shaderOpt = Shader::create("shaders/Cube.vs", "shaders/Light.fs");
    if (!shaderOpt)
    {
        log("[Error] Shader program creation failed");
        return -1;
    }

    Shader lightShader = std::move(*shaderOpt);

    shaderOpt = Shader::create("shaders/ModelWithLight.vs", "shaders/ModelWithLight.fs");
    if (!shaderOpt)
    {
        log("[Error] Shader program creation failed");
        return -1;
    }

    Shader modelShader = std::move(*shaderOpt);

    const glm::vec3 lightPos(1.2f, 1.0f, 15.0f);

    modelShader.use();

    modelShader.setVec3("light.position", lightPos);
    
    modelShader.setFloat("shininess", 32.0f);

    modelShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    modelShader.setVec3("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
    modelShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    
    modelShader.setFloat("light.constant", 1.0f);
    // modelShader.setFloat("light.linear", 0.09f);
    // modelShader.setFloat("light.quadratic", 0.032f);
    modelShader.setFloat("light.linear", 0.00f);
    modelShader.setFloat("light.quadratic", 0.0f);


    // auto modelOpt = Model::create("assets/backpack/backpack.obj");
    auto modelOpt = Model::create("assets/globe/globe.obj");
    if (!modelOpt)
    {
        log("[Error] Model loading failed");
        return -1;
    }

    Model backpackModel = std::move(*modelOpt);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)g_width / (float)g_height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        modelShader.use();
        modelShader.setVec3("viewPos", camera.getPosition());
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("model", model);

        backpackModel.draw(modelShader);

        // cubeShader.use();
        // cubeShader.setVec3("viewPos", camera.getPosition());
        // cubeShader.setMat4("projection", projection); 
        // cubeShader.setMat4("view", view);

        // for (uint32_t i = 0; i < 10; ++i)
        // {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubePositions[i]);
        //     float angle = 20.0f * i;
        //     model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        //     cubeShader.setMat4("model", model);

        //     cubeMesh.draw(cubeShader);
        // }

        lightShader.use();

        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightShader.setMat4("model", model);

        lightMesh.draw(lightShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}