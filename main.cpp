//#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "../common/GLShader.h"

struct Vertex {
    float position[3];  // x, y, z
    float normal[3];     // nx, ny, nz
    float uv[2]; // u,v
};

#include "DragonData.h"

GLShader g_triangleShader;

void Initialize()
{
    g_triangleShader.LoadVertexShader("triangle.vs");
    g_triangleShader.LoadFragmentShader("triangle.fs");
    g_triangleShader.Create();
}

void Terminate()
{
    g_triangleShader.Destroy();
}


void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 1.0f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = glfwGetTime();

    const auto program = g_triangleShader.GetProgram();
    glUseProgram(program);

    const int stride = sizeof(Vertex);
    const auto loc_position = glGetAttribLocation(program,
                                                  "a_position");

    const Vertex* mesh = (Vertex*)DragonVertices;
    const size_t tailleDragonOctet = sizeof(DragonVertices);
    //const size_t floatCount = _countof(DragonVertices);
    const uint32_t vertexCount = tailleDragonOctet/sizeof(Vertex);

    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE,
                          stride, mesh->position);
    glEnableVertexAttribArray(loc_position);

    const auto loc_color = glGetAttribLocation(program,
                                                  "a_color");
    glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE,
                          stride, mesh->normal);
    glEnableVertexAttribArray(loc_color);
    //glVertexAttrib3fv(loc_color, triangle[0].color);

    const auto loc_time = glGetUniformLocation(program,
                                               "u_time");
    glUniform1f(loc_time, time);

    // une matrice OpenGL est definie en COLONNE
    float rotationMatrix[16] = {
            cosf(time), 0.0f, sinf(time),  0.0f,
            0.f, 1, 0.0f, 0.0f,
            -sinf(time), 0.0f, cosf(time), 0.0f,
            0.0f, 0.0f, -40.0f, 1.0f
    };

    const auto loc_rotz_mat = glGetUniformLocation(
            program, "u_rotationMatrix");
    glUniformMatrix4fv(loc_rotz_mat, 1, GL_FALSE, rotationMatrix);

    const float aspectRatio = float(width)/float(height);
    const float zNear = 0.1f, zFar = 100.0f;
    const float fovy = 45.f * M_PI/180.f;
    const float cot = 1.f / tanf(fovy / 2.f);
    float projectionMatrix[] = {
          cot/aspectRatio, 0.f, 0.f, 0.f, // 1ere colonne
          0.f, cot, 0.f, 0.f,
          0.f, 0.f, -zFar/(zFar-zNear), -1.f,
          0.f, 0.f, -zFar*zNear/(zFar-zNear), 0.f
    };

    const auto loc_proj_mat = glGetUniformLocation(
            program, "u_projectionMatrix");
    glUniformMatrix4fv(loc_proj_mat, 1, GL_FALSE, projectionMatrix);

    //glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    const uint16_t indexCount = sizeof(DragonIndices)/ sizeof(uint16_t);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, DragonIndices);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    GLenum error = glewInit();
    if (error != GL_NO_ERROR) {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
    }

    Initialize();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        Render(window);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    Terminate();

    glfwTerminate();
    return 0;
}