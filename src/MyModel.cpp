#include "MyModel.h"

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

MyModel::MyModel(string path)
{
    directory = path.substr(0, path.find_last_of('/'));
    loadModel(path);
}

void MyModel::loadModel(string path)
{
    // 1. 读取文件
    std::ifstream objFile;
    objFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        objFile.open(path);
        std::stringstream objStream;
        // 读取文件的缓冲内容到数据流中
        objStream << objFile.rdbuf();
        // 关闭文件处理器
        objFile.close();
        // 转换数据流到string
        objCode = objStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    // 读取mtl文件
    int mtlPathStart = objCode.find("mtllib ");
    int mtlPathEnd = objCode.find("\n", mtlPathStart);
    string mtlPath = directory + '/' + split(objCode.substr(mtlPathStart, mtlPathEnd - mtlPathStart), ' ')[1];
    std::ifstream mtlFile;
    mtlFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        mtlFile.open(mtlPath);
        std::stringstream mtlStream;
        // 读取文件的缓冲内容到数据流中
        mtlStream << mtlFile.rdbuf();
        // 关闭文件处理器
        mtlFile.close();
        // 转换数据流到string
        mtlCode = mtlStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::MTL::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    // 找出单个mesh
    int pos1 = objCode.find("# object");
    int pos2 = objCode.find("triangles");
    while (string::npos != pos2)
    {
        string meshCode = objCode.substr(pos1, pos2 - pos1);
        processMesh(meshCode, vertices, texCoords, normals);
        pos1 = objCode.find("# object", pos2);
        pos2 = objCode.find("triangles", pos1);
    }
}

void MyModel::processMesh(string meshCode, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &texCoords, std::vector<glm::vec3> &normals)
{
    std::vector<Vertex> meshVertices;
    Material meshTexs;
    std::vector<unsigned int> indices;
    glm::vec3 center = glm::vec3(0);

    std::vector<string> lines = split(meshCode, '\n');
    // 处理mesh的所有行
    for (int i = 0; i < lines.size(); i++)
    {
        std::vector<string> lineBreak = split(lines[i], ' ');
        if (lineBreak.size() < 2)
            continue;
        if (lineBreak[0] == "v")
            vertices.push_back(glm::vec3(atof(lineBreak[2].c_str()), atof(lineBreak[3].c_str()), atof(lineBreak[4].c_str())));
        else if (lineBreak[0] == "vn")
            normals.push_back(glm::vec3(atof(lineBreak[1].c_str()), atof(lineBreak[2].c_str()), atof(lineBreak[3].c_str())));
        else if (lineBreak[0] == "vt")
            texCoords.push_back(glm::vec2(atof(lineBreak[1].c_str()), atof(lineBreak[2].c_str())));
        else if (lineBreak[0] == "f")
        {
            // 处理面的顶点数组
            for (int j = 1; j < lineBreak.size(); j++)
            {
                std::vector<string> pointBreak = split(lineBreak[j], '/');
                Vertex meshVertex;
                meshVertex.position = vertices[atof(pointBreak[0].c_str()) - 1];
                center += vertices[atof(pointBreak[0].c_str()) - 1];
                meshVertex.texCoord = texCoords[atof(pointBreak[1].c_str()) - 1];
                meshVertex.normal = normals[atof(pointBreak[2].c_str()) - 1];
                meshVertices.push_back(meshVertex);
                if (j == 4)
                {
                    indices.push_back(meshVertices.size() - 4);
                    indices.push_back(meshVertices.size() - 2);
                    indices.push_back(meshVertices.size() - 1);
                }
                else
                {
                    indices.push_back(meshVertices.size() - 1);
                }
            }
        }
        else if (lineBreak[0] == "usemtl")
        {
            meshTexs = processMtl(lineBreak[1]);
        }
    }

    center /= meshVertices.size();
    meshes.push_back(Mesh(meshVertices, indices, meshTexs, center));
}

Material MyModel::processMtl(string mtlId)
{
    Material material;

    int newmtlStart = mtlCode.find("newmtl " + mtlId);
    int newmtlEnd = mtlCode.find("\n\n", newmtlStart);
    string newmtl = mtlCode.substr(newmtlStart, newmtlEnd - newmtlStart);

    Texture ambient = parseTexture(newmtl, "ambient");
    Texture diffuse = parseTexture(newmtl, "diffuse");
    Texture specular = parseTexture(newmtl, "specular");

    std::vector<Texture> textures;
    textures.push_back(ambient);
    textures.push_back(diffuse);
    textures.push_back(specular);

    material.textures = textures;

    int shininessStart = newmtl.find("Ns");
    int shininessEnd = newmtl.find("\n", shininessStart);
    material.shininess = atof(split(newmtl.substr(shininessStart, shininessEnd - shininessStart),' ')[1].c_str());
    return material;
}

Texture MyModel::parseTexture(string newmtl, string type)
{
    Texture texture;
    int start = 0;
    if (type == "ambient")
        start = newmtl.find("map_Ka");
    else if (type == "diffuse")
        start = newmtl.find("map_Kd");
    else if (type == "specular")
        start = newmtl.find("map_Ks");
    bool skip = false;
    if (start == string::npos)
    {
        texture.path = "../default.png";
    }
    else
    {
        int end = newmtl.find("\n", start);
        string filename = directory + '/' + split(newmtl.substr(start, end - start), ' ')[1];
        texture.path = filename;
    }
    for (unsigned int j = 0; j < textures_loaded.size(); j++)
    {
        if (strcmp(textures_loaded[j].path.c_str(), texture.path.c_str()) == 0)
        {
            texture.id = textures_loaded[j].id;
            skip = true;
            break;
        }
    }
    texture.type = type;
    if (!skip)
    {
        texture.id = TextureFromFile(texture.path);
        textures_loaded.push_back(texture);
    }
    return texture;
}

unsigned int MyModel::TextureFromFile(string filename)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // 生成多级纹理
        glGenerateMipmap(GL_TEXTURE_2D);

        // 设置S轴与T轴的纹理环绕方式为重复
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // 为放大缩小设置纹理过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // 放大并不会产生多级渐远纹理
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        cout << "Texture failed to load at path: " << filename << endl;
        stbi_image_free(data);
    }

    return textureID;
}

void MyModel::draw(Shader shader, glm::vec3 scale)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].draw(shader, false, scale);
    }
}

void MyModel::drawMeshByIndex(Shader shader, int index, glm::vec3 scale)
{
    meshes[index].draw(shader, true, scale);
}