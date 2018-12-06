//
//  texture.cpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 06.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#include "texture.hpp"

#include <GLFW/glfw3.h>
#include<FreeImage.h>

Texture::Texture(const char *path) {
    
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path, 0);
    FIBITMAP* image = FreeImage_Load(format, path);
    FIBITMAP* temp = image;
    image = FreeImage_ConvertTo32Bits(image);
    FreeImage_Unload(temp);
    
    int w = FreeImage_GetWidth(image);
    int h = FreeImage_GetHeight(image);
    
    GLubyte* bits = (GLubyte*)FreeImage_GetBits(image);
    
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FreeImage_Unload(image);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
}
