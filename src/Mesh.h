#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include <vector>
#include <string>
#include "stb_image.h"
using namespace std;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture
{
    unsigned int id;
    string type;
    string path; // 储存纹理路径以防止重复加载
};

struct Material
{
    vector<Texture> textures;
    int shininess;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    Material material;
    glm::vec3 center;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material, glm::vec3 center);
    void draw(Shader shader,bool center,glm::vec3 scale);

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif // !MESH_H