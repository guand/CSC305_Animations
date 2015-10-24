#include "icg_common.h"

#include "Triangle/Triangle.h"
#include "Quad/Quad.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

Quad head;
Quad body;

Triangle wingR;
Triangle wingL;

const int BAT_MOVEMENT = 10;
const int SPEED_FACTOR = 1;

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

    float freq = M_PI*time_s*SPEED_FACTOR;
    float wingFlap = std::sin(time_s);

    //TODO: Set up the transform hierarchies for the three objects!

    float x_bat_movement = std::cos(-freq/BAT_MOVEMENT);
    float y_bat_movement = std::sin(-freq/BAT_MOVEMENT);


    Transform body_M = Transform::Identity();
    body_M *= Eigen::Translation3f(x_bat_movement, y_bat_movement, 0.0);
    body_M *= Eigen::Translation3f(0.0, 0.0 + wingFlap * 0.75, 0.0);
    body.draw(body_M.matrix());

    Transform head_M = Transform::Identity();
    head_M *= Eigen::Translation3f(x_bat_movement, y_bat_movement, 0.0);
    head_M *= Eigen::Translation3f(0.0, 1.5, 0.0);
    head_M *= Eigen::Translation3f(0.0, 0.0 + wingFlap * 0.75, 0.0);
    head_M *= Eigen::AlignedScaling3f(.5, .5, 1.0);

    head.draw(head_M.matrix());
    Transform wingR_M = Transform::Identity();
    wingR_M *= Eigen::Translation3f(x_bat_movement, y_bat_movement, 0.0);
    wingR_M *= Eigen::AlignedScaling3f(.75, .75, 1.0);
    wingR_M *= Eigen::Translation3f(1.3, 1.33, 0.0);
    wingR_M *= Eigen::Translation3f(0.0, 0.0 + wingFlap, 0.0);
    wingR_M *= Eigen::Affine3f(Eigen::AngleAxisf(M_PI/2.0, vec3::UnitZ())).matrix();
    wingR_M *= Eigen::Affine3f(Eigen::AngleAxisf(wingFlap, vec3::UnitZ())).matrix();
    wingR_M *= Eigen::Translation3f(0.0, -1.0, 0.0);
    wingR.draw(wingR_M.matrix());
    Transform wingL_M = Transform::Identity();
    wingL_M *= Eigen::Translation3f(x_bat_movement, y_bat_movement, 0.0);
    wingL_M *= Eigen::AlignedScaling3f(.75, .75, 1.0);
    wingL_M *= Eigen::Translation3f(-1.3, 1.33, 0.0);
    wingL_M *= Eigen::Translation3f(0.0, 0.0 + wingFlap, 0.0);
    wingL_M *= Eigen::Affine3f(Eigen::AngleAxisf(M_PI/-2.0, vec3::UnitZ())).matrix();
    wingL_M *= Eigen::Affine3f(Eigen::AngleAxisf(-wingFlap, vec3::UnitZ())).matrix();
    wingL_M *= Eigen::Translation3f(0.0, -1.0, 0.0);;
    wingL.draw(wingL_M.matrix());

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
