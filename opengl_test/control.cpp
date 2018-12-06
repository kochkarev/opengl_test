//
//  control.cpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 04.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#include <stdlib.h>

/*#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif*/

#include <GLFW/glfw3.h>

#include "control.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
