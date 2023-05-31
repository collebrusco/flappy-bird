////
////  Camera.h
////  opengl-tools
////
////  Created by Frank Collebrusco on 12/27/22.
////
////
//
//#ifndef Camera_h
//#define Camera_h
//#include <glm/glm.hpp>
//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/ext/matrix_clip_space.hpp>
//
//class Camera {
//public://protected:
//    glm::vec3 pos, look, up;
//    glm::mat4 view;
//    glm::mat4 proj;
//    float near, far;
//public: //TODO: detect & update only when needed
//    virtual ~Camera() = default;
//    virtual void update(); //default calls up v & p, add more if needed
//    virtual void setMouseControl(bool);
//    glm::mat4 updateView();
//    virtual glm::mat4 updateProj() = 0;
//    glm::mat4 View();
//    glm::mat4 Proj();
//};
//
//class OrthoCamera : public Camera {
//    constexpr const static glm::mat2x4 lrbtMat = { glm::vec4(-0.5f, 0.5f, 0, 0),
//                                                   glm::vec4(0, 0, -0.5f, 0.5f)};
//    float viewWidth; //in game coords
//public:
//    OrthoCamera();
//    OrthoCamera(const OrthoCamera&) = default;
//    OrthoCamera(glm::vec3 p, glm::vec3 l, glm::vec3 u, float n, float f, float w);
//    void setViewWidth(float vw);
//    float getViewWidth();
//    glm::mat4 updateProj() override ;
//};
//
//class PerspectiveCamera : public Camera {
//    float fov;
//    bool mouseControlled;
//public:
//    float phi, theta;
//    PerspectiveCamera();
//    PerspectiveCamera(const PerspectiveCamera&) = default;
//    PerspectiveCamera(glm::vec3 p, glm::vec3 l, glm::vec3 u, float n, float f, float fv);
//    void setFOV(float fv);
//    glm::mat4 updateProj() override;
//    void setMouseControl(bool) override;
//    void update() override;
//};
//
//#endif /* Camera_h */
