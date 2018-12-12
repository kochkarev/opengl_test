//
//  main.cpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 20.11.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <streambuf>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FreeImage.h>

#include "control.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "coordinates.hpp"

#define CUBES 4

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;
bool postProc = false;
bool blinnLight = false;

int main(int argc, char *argv[]) {

    //////////// GLWF WINDOW INITIALIZING /////////////
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(720, 720, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    std::cout << glfwGetVersionString() << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //////////// GLWF WINDOW INITIALIZING FINISH /////////////
    
    //////// SHADERS /////////
    Shader cubeShader("shaders/cube.vert", "shaders/cube.frag");
    Shader lightShader("shaders/light.vert", "shaders/light.frag");
    Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
    Shader reflectShader("shaders/reflect.vert", "shaders/reflect.frag");
    
    //////// CUBES /////////
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); // vertex coords
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // texture coords
    glBindVertexArray(0); // Unbind VAO
    
    //////// SKYBOX /////////
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0); // skybox coords
    glBindVertexArray(0);
    
    //////// LAMP /////////
    GLuint light_VAO;
    glGenVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    //////// PLANE /////////
    GLuint planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0); // vertex coords
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1); // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // texture coords
    
    //////// CUSTOM FRAMEBUFFER /////////
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLuint texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    
    //////// REDER BUFFER OBJECT /////////
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //////// SCREEN QUAD /////////
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // vertex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1); // texture coords
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render only lines
    
    //////// TEXTURES /////////
    Texture texture1("textures/container2.png");
    Texture texture2("textures/container2_specular.png");
    Texture floorTexture("textures/floor.bmp");
    Texture skyboxTexture(faces);
    
    //////// SHADERS CONFIGURING /////////
    cubeShader.Use();
    glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
    
    glUniform1f(glGetUniformLocation(cubeShader.Program, "material.shininess"), 32.0f);
    
    glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].quadratic"), 0.032f);
    
    //////// GAME CYCLE /////////
    while(!glfwWindowShouldClose(window))
    {
        
        cubeShader.Use();
        glUniform1i(glGetUniformLocation(cubeShader.Program, "blinn"), blinnLight);
        //glEnable(GL_FRAMEBUFFER_SRGB); // gamma - correction
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
        
        Do_Movement(); // checking for key pressed or mouse moved
        
        if (postProc) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glEnable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //////// CUBES RENDER /////////
        cubeShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1.ID);
        glUniform1i(glGetUniformLocation(cubeShader.Program, "material.diffuse"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2.ID);
        glUniform1i(glGetUniformLocation(cubeShader.Program, "material.specular"), 1);
        
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians((GLfloat)glfwGetTime() * 50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        
        glm::mat4 view(1.0f);
        view = camera.GetViewMatrix();
        
        glm::mat4 projection(1.0f);
        projection = glm::perspective(45.0f, (float)(width / height), 0.1f, 100.0f);
        
        glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(cubeShader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        
        lightPos.x = sin(glfwGetTime() / 7.0) * 2.7f;
        lightPos.z = cos(glfwGetTime() / 7.0) * 2.7f;
        glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].position"), lightPos.x, lightPos.y, lightPos.z);
        
        glBindVertexArray(VAO);
        for(GLuint i = 0; i < CUBES; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = i * 50.0f;
            //if (i % 3 == 0) angle = glfwGetTime() * 50.0f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        
        //////// LAMP RENDER /////////
        lightShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(light_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        //////// PLANE RENDER /////////
        cubeShader.Use();
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture.ID);
        glUniform1i(glGetUniformLocation(cubeShader.Program, "material.diffuse"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorTexture.ID);
        glUniform1i(glGetUniformLocation(cubeShader.Program, "material.specular"), 1);
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        //////// REEFLECTING CUBE RENDER /////////
        reflectShader.Use();
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        glBindVertexArray(VAO);
        glUniformMatrix4fv(glGetUniformLocation(reflectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(reflectShader.Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(reflectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(reflectShader.Program, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.ID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        //////// SKYBOX RENDER /////////
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();
        glBindVertexArray(skyboxVAO);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.ID);
        glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.ID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        glBindVertexArray(0);
        
        if (postProc) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            screenShader.Use();
            glUniform1i(glGetUniformLocation(screenShader.Program, "screenTexture"), 0);
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if(action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
     }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void Do_Movement()
{
    if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_P]) postProc = !postProc;
    if (keys[GLFW_KEY_B]) {
        blinnLight = !blinnLight;
        if (blinnLight) std::cout << "Blinn" << std::endl;
        else std::cout << "Fong" << std::endl;
    }
}
