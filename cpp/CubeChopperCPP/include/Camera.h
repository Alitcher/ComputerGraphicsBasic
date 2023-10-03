#ifndef ALITCH_CAMERA_H
#define ALITCH_CAMERA_H
#include <glm/glm.hpp>

class Camera
{
private:
    glm::mat4 m_projectonMatrix;
    glm::mat4 m_viewMatrix;

public:
    Camera(glm::mat4 in_projection, glm::mat4 in_view);
    ~Camera();

    glm::mat4 getProjection() const;
    void setProjection(const glm::mat4& in_projection);
    glm::mat4 getView() const;
    void setView(const glm::mat4& in_view);
};

extern Camera* mainCamera;     // Pointer to our main (perspective) camera
extern Camera* secondaryCamera; // Pointer to our secondary (orthographic) camera

#endif