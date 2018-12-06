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

#include <GLFW/glfw3.h>
#include<FreeImage.h>

class Texture {
public:
    Texture(const char *path);
    GLuint ID;
};

#endif /* texture_hpp */
