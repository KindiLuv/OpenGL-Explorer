//#define GLEW_STATIC 1
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "../common/GLShader.h"

#include "tiny_obj_loader.h"

struct Vertex {
    float position[3];  // x, y, z
    float normal[3];     // nx, ny, nz
    float uv[2]; // u,v
};

#include "DragonData.h"
#include "stb_image.h"


using std::vector;

GLShader g_triangleShader;
Vertex coords[50000] = {};
int cpt = 0;
unsigned int texID;
void Initialize()
{
    g_triangleShader.LoadVertexShader("triangle.vs");
    g_triangleShader.LoadFragmentShader("triangle.fs");
    g_triangleShader.Create();
    glGenTextures(1, &texID);
}

void Terminate()
{
    glDeleteTextures(1, &texID);
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

    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE,
                          stride, coords->position);
    glEnableVertexAttribArray(loc_position);

    const auto loc_color = glGetAttribLocation(program,
                                                  "a_color");
    glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE,
                          stride, coords->normal);
    glEnableVertexAttribArray(loc_color);

    const auto loc_uv = glGetAttribLocation(program,
                                            "a_uv");
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, stride, coords->uv);
    glEnableVertexAttribArray(loc_uv);

    auto loc_tex = glGetUniformLocation(program, "u_sampler");
    glUniform1f(loc_tex, 0);

    const auto loc_time = glGetUniformLocation(program,
                                               "u_time");
    glUniform1f(loc_time, time);

    // une matrice OpenGL est definie en COLONNE
    float rotationMatrix[16] = {
            cosf(time), 0.0f, sinf(time),  0.0f,
            0.f, 1, 0.0f, 0.0f,
            -sinf(time), 0.0f, cosf(time), 0.0f,
            0.0f, 0.0f, -5.0f, 1.0f
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
    glDrawArrays(GL_TRIANGLES, 0, cpt);

    //const uint16_t indexCount = cpt;
    //glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, DragonIndices);
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int w,h;
    unsigned char *data = stbi_load("../Sting_Base_Color.png" , &w,&h,nullptr,STBI_rgb_alpha);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    std::string inputfile = "C:/Users/pierr/Documents/GitHub/OpenGL-Explorer/objects/Triangulate_chickin.obj";
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files

    tinyobj::ObjReader reader;
    reader_config.triangulate = true;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }


    auto& attrib = reader.GetAttrib();
    //Collect faces
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();


    for(size_t i = 0; i < shapes.size(); i++) {
        size_t index_offset = 0;
        // For each face
        for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
            for(int k = 0; k < 3; k++){

            tinyobj::index_t idx = shapes[i].mesh.indices[index_offset];

            float vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            float vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            float vz = attrib.vertices[3*size_t(idx.vertex_index)+2];


            float nx = attrib.normals[3*size_t(idx.normal_index)+0];
            float ny = attrib.normals[3*size_t(idx.normal_index)+1];
            float nz = attrib.normals[3*size_t(idx.normal_index)+2];


            float tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
            float ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];


            coords[cpt]=Vertex{
                    {vx,vy,vz},
                    {nx,ny,nz},
                    {tx,ty}
            };
            cpt++;
            index_offset++;
            }
        }
    }

    std::cout << cpt << std::endl;
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