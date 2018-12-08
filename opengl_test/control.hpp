//
//  control.hpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 04.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#ifndef control_hpp
#define control_hpp

enum ScreenSize { HEIGHT = 720, WIDTH = 720};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void Do_Movement();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

#endif /* control_hpp */
