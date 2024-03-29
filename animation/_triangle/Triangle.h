#pragma once
#include "icg_common.h"

class Triangle{
private:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID 
    GLuint _vbo; ///< memory buffer
public:
    void init(){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("_triangle/vshader.glsl", "_triangle/fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);       
        glUseProgram(_pid);
        
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        ///--- Vertex Buffer
        const GLfloat vpoint[] = { /*V1*/-1.0f, -1.0f, 0.0f, 
                                   /*V2*/ 1.0f, -1.0f, 0.0f, 
                                   /*V3*/ 0.0f,  1.0f, 0.0f};
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
    
        ///--- vpoint shader attribute
        GLuint position = glGetAttribLocation(_pid, "vpoint"); ///< Fetch Attribute ID for Vertex Positions
        glEnableVertexAttribArray(position); /// Enable it
        glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
                      
        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
       
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &_vbo);
        glDeleteProgram(_pid);
        glDeleteVertexArrays(1, &_vao);
    }
        
    void draw(const mat4& M){
        glUseProgram(_pid);
        glBindVertexArray(_vao);       
            ///--- Set transformation uniform
            /// @see http://eigen.tuxfamily.org/dox/classEigen_1_1AngleAxis.html#details

            GLuint M_id = glGetUniformLocation(_pid, "M");
            glUniformMatrix4fv(M_id, 1, GL_FALSE, M.data());
            ///--- Draw
            glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};
