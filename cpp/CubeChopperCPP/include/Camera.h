#ifndef ALITCH_CAMERA_H
#define ALITCH_CAMERA_H
#include <glm/glm.hpp>
#include <memory>

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

extern std::shared_ptr<Camera> mainCamera;     // Smart pointer to main camera
extern std::shared_ptr<Camera> secondaryCamera; // Smart pointer to secondary camera


#endif