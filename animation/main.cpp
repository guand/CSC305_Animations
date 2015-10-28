#include "icg_common.h"
#include "FrameBuffer.h"
#include "_quad/Quad.h"
#include "_screenquad/ScreenQuad.h"
#include "Bat.h"

int width=720, height=720;
const int BAT_MOVEMENT = 10;
const int SPEED_FACTOR = 20;
const int BEZIER_SPEED = 1;
typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
std::vector<vec3> bezierPts;

Quad background;
Quad moon;
Bat bat;


static const float SpeedFactor = 1;
// TODO: declare Framebuffer + ScreenQuad (see slides)
FrameBuffer fb(width, height);
ScreenQuad squad;

/**
 * @brief bezierMovement
 * @param movement
 * @param time
 * @return vector3f
 * Creates the curve for the movement
 */
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


void init(){
    glClearColor(1,1,1, /*solid*/1.0 );
    background.init("_quad/background.tga");
    moon.init("_quad/moon.tga");
    bat.init();

    // TODO: initialize framebuffer (see slides)
    // TODO: initialize fullscreen quad (see slides)
    GLuint fb_tex = fb.init();
    squad.init(fb_tex);


}

void drawScene(float timeCount)
{
    //TODO: Draw the animation you want
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float t = timeCount * SpeedFactor;
    float bTime = fmod(t, 3);
    float wingFlap = std::sin(t * SPEED_FACTOR);
    float scale = 0.15;
//    Transform TRS = Transform::Identity();
//    background.draw(TRS.matrix());
    vec3 pt1(-0.583333, 0.992188, 0);
    vec3 pt2(-0.88, -0.71, 0);
    vec3 pt3(1.3, -0.8, 0);
    vec3 pt4(1.3099, 1.05729, 0);
    bezierPts.push_back(pt1);
    bezierPts.push_back(pt2);
    bezierPts.push_back(pt3);
    bezierPts.push_back(pt4);
    vec3 bezier_movement = bezierMovement(bezierPts, bTime);
    float x_bat_movement = bezier_movement[0];
    float y_bat_movement = bezier_movement[1];
    bat.draw(x_bat_movement, y_bat_movement, wingFlap, bTime);
    glDisable(GL_BLEND);
}

void drawMoon()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Transform TRS = Transform::Identity();
    TRS *= Eigen::Translation3f(-0.75, 0.75, 0.0);
    TRS *= Eigen::AlignedScaling3f(0.35, 0.35, 1.0);

    moon.draw(TRS.matrix());
    glDisable(GL_BLEND);
}

void drawMoonPassTwo()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Transform TRS = Transform::Identity();
    TRS *= Eigen::Translation3f(-0.75, 0.75, 0.0);
    TRS *= Eigen::AlignedScaling3f(0.20, 0.20, 1.0);
    moon.draw(TRS.matrix());
    glDisable(GL_BLEND);
}

void display(){
    opengp::update_title_fps("FrameBuffer");
    glViewport(0,0,width,height);

    // TODO: First draw the scene onto framebuffer,
    // then use screen quad to add effects!
    ///--- Render to FB
    fb.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawMoon();
    fb.unbind();
    // fb.display_color_attachment("FB - Color"); ///< debug

    ///--- Render to Window
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    squad.draw();
    drawScene(glfwGetTime());
    drawMoonPassTwo();
}

int main(int, char**){
    /// GLFW Initialization
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return EXIT_FAILURE;
    }

    /// Hint GLFW that we would like an OpenGL 3 context (at least)
    /// http://www.glfw.org/faq.html#41__how_do_i_create_an_opengl_30_context
    /// This may cause problems on a laptop Intel on-board graphics card
    /// When you have a dedicated graphics cards, enable these lines
    /// to have more powerful features of openGL!!
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /// Attempt to open the window: fails if required version unavailable
    /// @note some Intel GPUs do not support OpenGL 3.2
    /// @note update the driver of your graphic card
    if( !glfwOpenWindow(width, height, 0,0,0,0, 32,0, GLFW_WINDOW ) ){
        fprintf( stderr, "Failed to open OpenGL 3.2 GLFW window.\n" );
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /// GLEW Initialization (must have a context)
    glewExperimental = true; ///<
    if( glewInit() != GLEW_NO_ERROR ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    /// Set window title
    glfwSetWindowTitle("Loading Framebuffer....");
    std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;

    /// Initialize our OpenGL program
    init();

    /// Render loop & keyboard input
    while(glfwGetKey(GLFW_KEY_ESC)!=GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED)){
        display();
        glfwSwapBuffers();
    }

    /// Close OpenGL window and terminate GLFW
    glfwTerminate();
    exit( EXIT_SUCCESS );
}
