#include "icg_common.h"
#include "Bat.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

Bat bat;

const int BAT_MOVEMENT = 10;
const int SPEED_FACTOR = 1;

void init(){
    ///--- Sets background color
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0 );
    bat.init();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    float time_s = glfwGetTime();

    float freq = M_PI*time_s*SPEED_FACTOR;
    float wingFlap = std::sin(time_s);

    //TODO: Set up the transform hierarchies for the three objects!

    float x_bat_movement = std::cos(-freq/BAT_MOVEMENT);
    float y_bat_movement = std::sin(-freq/BAT_MOVEMENT);

    bat.draw(x_bat_movement, y_bat_movement, wingFlap);


}

int main(int, char**){
    glfwInitWindowSize(748, 748);
    glfwCreateWindow("bat");
    glfwDisplayFunc(display);
    init();
    glfwMainLoop();
    bat.cleanup();
    return EXIT_SUCCESS;
}
