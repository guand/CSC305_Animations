#include "icg_common.h"
#include "FrameBuffer.h"
#include "_quad/Quad.h"
#include "_screenquad/ScreenQuad.h"

int width=720, height=720;
typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

Quad background;
Quad foreground;
static const float SpeedFactor = 1;
// TODO: declare Framebuffer + ScreenQuad (see slides)
FrameBuffer fb(width, height);
ScreenQuad squad;

void init(){
    glClearColor(1,1,1, /*solid*/1.0 );
    foreground.init("_quad/nyancat.tga");
    background.init("_quad/background.tga");

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
    Transform TRS = Transform::Identity();
    background.draw(TRS.matrix());
    float xcord = 0.7*std::cos(t);
    float ycord = 0.7*std::sin(t);

    TRS *= Eigen::Translation3f(xcord, ycord, 0);
    TRS *= Eigen::AngleAxisf(t + M_PI / 2, Eigen::Vector3f::UnitZ());
    TRS *= Eigen::AlignedScaling3f(0.285, 0.2, 1);
    foreground.draw(TRS.matrix());
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
        drawScene(glfwGetTime());
    fb.unbind();
    // fb.display_color_attachment("FB - Color"); ///< debug

    ///--- Render to Window
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    squad.draw();
    drawScene(glfwGetTime());
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
