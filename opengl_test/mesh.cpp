//
//  cube.cpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 13.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "mesh.hpp"

Cube::Cube(Shader *shader, Shader *reflShader) {
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); // vertex coords
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // texture coords
    glBindVertexArray(0); // Unbind VAO
    
    _shader = shader;
    _reflShader = reflShader;
    
    _shader->Use();
    glUniform3f(glGetUniformLocation(_shader->Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(_shader->Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(_shader->Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(_shader->Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
    
    glUniform1f(glGetUniformLocation(_shader->Program, "material.shininess"), 32.0f);
    
    glUniform3f(glGetUniformLocation(_shader->Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(_shader->Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(_shader->Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(_shader->Program, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(_shader->Program, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(_shader->Program, "pointLights[0].quadratic"), 0.032f);
    
}

void Cube::Draw(GLuint texture, glm::mat4 projection, glm::mat4 view, glm::vec3 camera, Shader depthShader) { // reflecting cube
    
    _reflShader->Use();
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glUniformMatrix4fv(glGetUniformLocation(_reflShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(_reflShader->Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(_reflShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(_reflShader->Program, "cameraPos"), camera.x, camera.y, camera.z);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
}

void Cube::Draw(GLuint diffuseMap, GLuint specMap, glm::mat4 viewMatrix, glm::mat4 projection, glm::vec3 camPos, glm::vec3 lightPos, Shader depthShader) {
    
    _shader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glUniform1i(glGetUniformLocation(_shader->Program, "material.diffuse"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specMap);
    glUniform1i(glGetUniformLocation(_shader->Program, "material.specular"), 1);
    
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians((GLfloat)glfwGetTime() * 50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(_shader->Program, "viewPos"), camPos.x, camPos.y, camPos.z);
    
    glUniform3f(glGetUniformLocation(_shader->Program, "pointLights[0].position"), lightPos.x, lightPos.y, lightPos.z);
    
    glBindVertexArray(VAO);
    for (int i = 0; i < 4; i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = i * 50.0f;
        //if (i % 3 == 0) angle = glfwGetTime() * 50.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
}

Plane::Plane(Shader *shader) {
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0); // vertex coords
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1); // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // texture coords
    
    _shader = shader;
}

void Plane::Draw(GLuint diffuseMap, GLuint specMap, Shader depthShader) {
    _shader->Use();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glUniform1i(glGetUniformLocation(_shader->Program, "material.diffuse"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specMap);
    glUniform1i(glGetUniformLocation(_shader->Program, "material.specular"), 1);
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

SkyBox::SkyBox(Shader *shader) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0); // skybox coords
    glBindVertexArray(0);
    
    _shader = shader;
}

void SkyBox::Draw(GLuint texture, glm::mat4 view, glm::mat4 projection) {
    glDepthFunc(GL_LEQUAL);
    _shader->Use();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glUniform1i(glGetUniformLocation(_shader->Program, "skybox"), 0);
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    glBindVertexArray(0);
}

Lamp::Lamp(Shader *shader) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    _shader = shader;
}

void Lamp::Draw(glm::vec3 lightPos, glm::mat4 view, glm::mat4 projection, Shader depthShader) {
    _shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "project"), 1, GL_FALSE, glm::value_ptr(projection));
    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    glUniformMatrix4fv(glGetUniformLocation(_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
