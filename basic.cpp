/*
  Draw a simple full screen quad

  Li-Yi Wei
  November 5, 2017

 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

#include <stdlib.h>

#include <GLXW/glxw.h>
#include <GLFW/glfw3.h>

class ShaderProgram
{
public:

    ShaderProgram(void): _shader_program(0), _vertex_shader(0), _fragment_shader(0)
    {
        // nothing else to do
    }

    ~ShaderProgram(void)
    {
        if(Valid())
        {
            glDetachShader(_shader_program, _vertex_shader);	
            glDetachShader(_shader_program, _fragment_shader);
            glDeleteShader(_vertex_shader);
            glDeleteShader(_fragment_shader);
            glDeleteProgram(_shader_program);
        }
    }

    string Build(const string & vertex_source, const string & fragment_source)
    {
        // we need these to properly pass the strings
        const char *source;
        int length;
        string message;

        // create and compiler vertex shader
        _vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        source = vertex_source.c_str();
        length = vertex_source.size();
        glShaderSource(_vertex_shader, 1, &source, &length); 
        glCompileShader(_vertex_shader);

        if((message = CheckShaderCompileStatus(_vertex_shader)) != "")
        {
            return "cannot compile vertex shader: " + message;       
        }
 
        // create and compiler fragment shader
        _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        source = fragment_source.c_str();
        length = fragment_source.size();
        glShaderSource(_fragment_shader, 1, &source, &length);   
        glCompileShader(_fragment_shader);
        if((message = CheckShaderCompileStatus(_fragment_shader)) != "")
        {
            return "cannot compile fragment shader: " + message;
        }
    
        // create program
        _shader_program = glCreateProgram();
    
        // attach shaders
        glAttachShader(_shader_program, _vertex_shader);
        glAttachShader(_shader_program, _fragment_shader);

        // link the program and check for errors
        glLinkProgram(_shader_program);
        if((message = CheckProgramLinkStatus(_shader_program)) != "")
        {
            return "cannot link shader program: " + message;
        }

        return "";
    }

    void Use(void) const
    {
        glUseProgram(_shader_program);
    }
 
    bool Valid(void) const
    {
        return (CheckProgramLinkStatus(_shader_program) == "");
    }

    string CheckShaderCompileStatus(GLuint obj) const
    {
        GLint status;
        glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE) 
        {
            GLint length;
            glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
            vector<char> log(length);
            glGetShaderInfoLog(obj, length, &length, &log[0]);
        
            return string(log.begin(), log.end());
        }
        else
        {
            return "";
        }
    }

    string CheckProgramLinkStatus(GLuint obj) const
    {
        GLint status;
        glGetProgramiv(obj, GL_LINK_STATUS, &status);
        if(status == GL_FALSE) 
        {
            GLint length;
            glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
            vector<char> log(length);
            glGetProgramInfoLog(obj, length, &length, &log[0]);
            return string(log.begin(), log.end());
        }
        else
        {
            return "";
        }
    }

protected:

    GLuint _shader_program, _vertex_shader, _fragment_shader; 
};

string BuildProgram(ShaderProgram & program)
{
    // shader source code
    std::string vertex_source =
        "#version 330\n"
        "layout(location = 0) in vec4 vposition;\n"
        "layout(location = 1) in vec4 vcolor;\n"
        "out vec4 fcolor;\n"
        "void main() {\n"
        "   fcolor = vcolor;\n"
        "   gl_Position = vposition;\n"
        "}\n";
        
    std::string fragment_source =
        "#version 330\n"
        "in vec4 fcolor;\n"
        "layout(location = 0) out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = fcolor;\n"
        "}\n";

    return program.Build(vertex_source, fragment_source);
};

class Scene
{
public:
    Scene(void): _vao(0), _vbo(0)
    {
        // generate and bind the vao
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
    
        // generate and bind the buffer object
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            
        // data for a fullscreen quad
        GLfloat vertexData[] = {
            //  X     Y     Z           R     G     B
            1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f, // vertex 0
            -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f, // vertex 1
            1.0f,-1.0f, 0.0f,       0.0f, 0.0f, 1.0f, // vertex 2
            1.0f,-1.0f, 0.0f,       0.0f, 0.0f, 1.0f, // vertex 3
            -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f, // vertex 4
            -1.0f,-1.0f, 0.0f,       1.0f, 0.0f, 0.0f, // vertex 5
        }; // 6 vertices with 6 components (floats) each
    
        // fill with data
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*6, vertexData, GL_STATIC_DRAW);
                    
        // set up generic attrib pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));
    }

    ~Scene(void)
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
    }

    void Draw(void) const
    {
        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

protected:
    GLuint _vao, _vbo;
};

int Main(int argc, char **argv)
{
    const int width = 640;
    const int height = 480;

    if(glfwInit() == GL_FALSE) 
    {
        cerr << "failed to init GLFW" << endl;
        return 1;
    }

    // select opengl version 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // create a window
    GLFWwindow *window;
    if((window = glfwCreateWindow(width, height, "triangle", 0, 0)) == 0) 
    {
        cerr << "failed to open window" << endl;
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);

    if(glxwInit())
    {
        cerr << "failed to init GL3W" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // scope for program and scene objects
    {
        // shader program
        ShaderProgram program;

        string message = BuildProgram(program);
        if(message != "")
        {
            cerr << "Error: " << message << endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        Scene scene;

        while(!glfwWindowShouldClose(window)) 
        {
            glfwPollEvents();

            // drawing etc goes here
            glClear(GL_COLOR_BUFFER_BIT);

            program.Use();

            scene.Draw();

            // check for errors
            GLenum error = glGetError();
            if(error != GL_NO_ERROR) 
            {
                cerr << "Error: " << error << endl;
                break;
            }

            // finally swap buffers
            glfwSwapBuffers(window);
        }
    }

    // cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

int main(int argc, char **argv)
{
    return Main(argc, argv);
}
