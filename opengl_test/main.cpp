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
#include "mesh.hpp"

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
        
    //////// CUSTOM FRAMEBUFFER FOR POST-PROCESSING /////////
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
    
    //////// BUFFER OBJECT /////////
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
    
    //////// DEPTH MAP /////////
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //////// SCREEN QUAD /////////
    GLuint quadVAO, quadVBO;
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
    
    Cube cube(&cubeShader, &reflectShader);
    Plane plane(&cubeShader);
    SkyBox skybox(&skyboxShader);
    Lamp lamp(&lightShader);
    
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
        
        /*if (postProc) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glEnable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }*/
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        lightPos.x = sin(glfwGetTime() / 7.0) * 2.7f;
        lightPos.z = cos(glfwGetTime() / 7.0) * 2.7f;

        //////// CUBES RENDER /////////
        glm::mat4 view(1.0f);
        view = camera.GetViewMatrix();
        glm::mat4 projection(1.0f);
        projection = glm::perspective(45.0f, (float)(width / height), 0.1f, 100.0f);
        cube.Draw(texture1.ID, texture2.ID, view, projection, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z), glm::vec3(lightPos.x, lightPos.y, lightPos.z));
        
        //////// LAMP RENDER /////////
        lamp.Draw(lightPos, view, projection);
        
        //////// PLANE RENDER /////////
        plane.Draw(floorTexture.ID, floorTexture.ID);
        
        //////// REEFLECTING CUBE RENDER /////////
        cube.Draw(skyboxTexture.ID, projection, view, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
        
        //////// SKYBOX RENDER /////////
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skybox.Draw(skyboxTexture.ID, view, projection);
        
        
        /*if (postProc) {
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
        }*/
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return 0;
}

void renderScene(Shader cubeShader, Shader lightShader, Shader reflectShader, Shader skyboxShader) {
    
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
