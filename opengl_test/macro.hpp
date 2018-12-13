//
//  macro.hpp
//  opengl_test
//
//  Created by Алексей Кочкарёв on 13.12.2018.
//  Copyright © 2018 Алексей Кочкарёв. All rights reserved.
//

#ifndef macro_h
#define macro_h

#define CUBES_RENDER(depthShader) \
glm::mat4 view(1.0f);\
view = camera.GetViewMatrix();\
glm::mat4 projection(1.0f);\
projection = glm::perspective(45.0f, (float)(width / height), 0.1f, 100.0f);\
cube.Draw(texture1.ID, texture2.ID, view, projection, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z), glm::vec3(lightPos.x, lightPos.y, lightPos.z), depthShader);\

#define LAMP_RENDER(depthShader) \
lamp.Draw(lightPos, view, projection, depthShader);

#define PLANE_RENDER(depthShader) \
plane.Draw(floorTexture.ID, floorTexture.ID, depthShader);

#define REFL_CUBE_RENDER(depthShader) \
cube.Draw(skyboxTexture.ID, projection, view, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z), depthShader);

#define SKYBOX_RENDER \
view = glm::mat4(glm::mat3(camera.GetViewMatrix())); \
skybox.Draw(skyboxTexture.ID, view, projection); \

#define RENDER_SCENE(depthShader) \
CUBES_RENDER(depthShader) \
LAMP_RENDER(depthShader) \
PLANE_RENDER(depthShader) \
REFL_CUBE_RENDER(depthShader) \
SKYBOX_RENDER

#endif /* macro_h */
