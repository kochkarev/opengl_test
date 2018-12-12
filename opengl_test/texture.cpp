//
//  texture.cpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 06.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#include "texture.hpp"
#include <iostream>

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

Texture::Texture(std::vector<std::string> faces) {
    
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    
    for (int i = 0; i < faces.size(); i++) {
        
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(faces[i].c_str(), 0);
        FIBITMAP* image = FreeImage_Load(format, faces[i].c_str());
        FIBITMAP* temp = image;
        image = FreeImage_ConvertTo32Bits(image);
        FreeImage_Unload(temp);
        
        int w = FreeImage_GetWidth(image);
        int h = FreeImage_GetHeight(image);
        
        GLubyte* bits = (GLubyte*)FreeImage_GetBits(image);
        
        if (bits) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)bits);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

