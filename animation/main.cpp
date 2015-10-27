#include "icg_common.h"
#include "Bat.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

Bat bat;

const int BAT_MOVEMENT = 10;
const int SPEED_FACTOR = 1;
const int BEZIER_SPEED = 5;

std::vector<vec3> bezierPts;

void init(){
    ///--- Sets background color
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0 );
    bat.init();
}

vec3 bezierMovement(std::vector<vec3> movement, float time)
{
    float t = time / BEZIER_SPEED;
    // set the values for both (1 - t) and t
    float omt1 = 1.0 - t;
    float omt2 = pow(omt1, 2);
    float omt3 = pow(omt1, 3);
    float t2 = pow(t, 2);
    float t3 = pow(t, 3);
    // get the point after the bezier curve calculation
    vec3 point = omt3 * movement.at(0) + 3.0 * t * omt2 * movement.at(1) + 3.0 * t2 * omt1 * movement.at(2) + t3 * movement.at(3);
    return point;
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    float time_s = glfwGetTime();
    float freq = M_PI*time_s*SPEED_FACTOR;
    // Bat size and movement
    float wingFlap = std::sin(time_s);
    float scale = 0.10;

    // bezier control points
    vec3 pt1(-0.583333, 0.992188, 0);
    vec3 pt2(-0.88, -0.71, 0);
    vec3 pt3(1.3, -0.8, 0);
    vec3 pt4(1.3099, 1.05729, 0);
    bezierPts.push_back(pt1);
    bezierPts.push_back(pt2);
    bezierPts.push_back(pt3);
    bezierPts.push_back(pt4);

    // bezier movement
    vec3 bezier_movement = bezierMovement(bezierPts, time_s);
    //TODO: Set up the transform hierarchies for the three objects!

    float x_bat_movement = bezier_movement[0];
    float y_bat_movement = bezier_movement[1];

    bat.draw(x_bat_movement, y_bat_movement, wingFlap, scale*time_s);
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
