//
//  texture.hpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 06.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#ifndef texture_hpp
#define texture_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include<FreeImage.h>

class Texture {
public:
    Texture(const char *path);
    Texture(std::vector<std::string> faces);
    GLuint ID;
};

#endif /* texture_hpp */
