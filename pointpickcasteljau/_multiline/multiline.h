#include "icg_common.h"

const static Scalar H = .7;
const static Scalar R = 2;

class MultiLine{
private:
    class Hull{
    public:
        Hull() {
            _p1 = _p2 = _p3 = _p4 = _p5 = _p6 = vec3::Zero();
        }

        vec3& p1(){ return _p1; }
        vec3& p2(){ return _p2; }
        vec3& p3(){ return _p3; }
        vec3& p4(){ return _p4; }
        vec3& p5(){ return _p5; }
        vec3& p6(){ return _p6; }

    private:
        vec3 _p1;
        vec3 _p2;
        vec3 _p3;
        vec3 _p4;
        vec3 _p5;
        vec3 _p6;
    };
    
private:
    Hull _hull;                  ///< control points
    std::vector<vec3> _vertices; ///< multiline points
    GLuint _vao;                 ///< Vertex array objects
    GLuint _pid;          ///< GLSL program ID
    GLuint _vbo;
    
private:

    void multiline(Hull & p)
    {
        _vertices.push_back(p.p1());
        _vertices.push_back(p.p2());
        _vertices.push_back(p.p3());
        _vertices.push_back(p.p4());
        _vertices.push_back(p.p5());
        _vertices.push_back(p.p6());
    }

public:
    void init(GLuint pid){
        ///--- Set the (compiled) shaders
        _pid = pid;
        
        /// Generate the vertex array
        glGenVertexArrays(ONE, &_vao);
        glBindVertexArray(_vao);
        
        /// Generate one buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(ONE, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }
    void set_points(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec3& p5, const vec3& p6) {
        _vertices.clear();

        ///--- initialize data
        _hull.p1() = p1;
        _hull.p2() = p2;
        _hull.p3() = p3;
        _hull.p4() = p4;
        _hull.p5() = p5;
        _hull.p6() = p6;

        ///--- create the multiline
        multiline(_hull);
    }

    void draw(const mat4& model, const mat4& view, const mat4& projection){
        if (_vertices.empty()) return;

        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Vertex Positions
        GLuint position = glGetAttribLocation(_pid, "position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        ///--- vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_vertices.size(), &_vertices[0], GL_STATIC_DRAW);

        ///--- setup view matrices
        GLuint projection_id = glGetUniformLocation(_pid, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, projection.data());
        mat4 MV = view*model;
        GLuint model_view_id = glGetUniformLocation(_pid, "model_view");
        glUniformMatrix4fv(model_view_id, ONE, DONT_TRANSPOSE, MV.data());
        check_error_gl();

        glDrawArrays(GL_LINE_STRIP, 0, _vertices.size());
        glDisableVertexAttribArray(position);
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
        ///--- setup view matrices for bezier curve
    }
};
