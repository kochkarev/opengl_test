//
//  coordinates.hpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 12.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#ifndef coordinates_h
#define coordinates_h

#include <vector>
#include <string>

// quad for visualizing post-processed texture
float quadVertices[] = { // vertex(2), texture(2)
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    
    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

std::vector<std::string> faces
{
    "textures/skybox_right.png",
    "textures/skybox_left.png",
    "textures/skybox_up.png",
    "textures/skybox_down.png",
    "textures/skybox_front.png",
    "textures/skybox_back.png"
};

#endif /* coordinates_h */
