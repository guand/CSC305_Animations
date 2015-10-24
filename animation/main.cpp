#include "icg_common.h"

#include "Triangle/Triangle.h"
#include "Quad/Quad.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

Quad head;
Quad body;

Triangle wingR;
Triangle wingL;

void init(){
    ///--- Sets background color
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0 );
    head.init();
    body.init();
    wingR.init();
    wingL.init();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    float time_s = glfwGetTime();


    //TODO: Set up the transform hierarchies for the three objects!
    Transform head_M = Transform::Identity();
    head_M *= Eigen::Translation3f(0.0, 1.5, 0.0);
    head_M *= Eigen::AlignedScaling3f(.5, .5, 1.0);

    head.draw(head_M.matrix());

    Transform body_M = Transform::Identity();
    body.draw(body_M.matrix());
    Transform wingR_M = Transform::Identity();
    wingR_M *= Eigen::Translation3f(1.5, 1.0, 0.0);
    wingR_M *= Eigen::AlignedScaling3f(.5, .5, 1.0);
    wingR_M *= Eigen::Affine3f(Eigen::AngleAxisf(M_PI/2.0, vec3::UnitZ())).matrix();
    wingR.draw(wingR_M.matrix());


}

int main(int, char**){
    glfwInitWindowSize(748, 748);
    glfwCreateWindow("bat");
    glfwDisplayFunc(display);
    init();
    glfwMainLoop();
    head.cleanup();
    body.cleanup();
    wingR.cleanup();
    return EXIT_SUCCESS;
}
