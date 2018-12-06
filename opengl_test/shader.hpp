//
//  shader.h
//  opengl_test
//
//  Created by Алексей Кочкарёв on 05.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#ifndef shader_h
#define shader_h

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
public:
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    GLuint Program;
    
    void Use();
};

#endif /* shader_h */
