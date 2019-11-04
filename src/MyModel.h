#include "Mesh.h"
#include <vector>

class MyModel
{
public:
    MyModel(string path);
    void draw(Shader shader, glm::vec3 scale);
    void drawMeshByIndex(Shader shader, int index, glm::vec3 scale);

private:
    vector<Mesh> meshes;
    string directory;
    string objCode;
    string mtlCode;
    vector<Texture> textures_loaded;

    void loadModel(string path);
    void processMesh(string meshCode, std::vector<glm::vec3> &, std::vector<glm::vec2> &, std::vector<glm::vec3> &);
    Material processMtl(string mtlId);
    Texture parseTexture(string newmtl,string type);
    unsigned int TextureFromFile(string path);
};