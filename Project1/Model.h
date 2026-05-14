#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SOIL2/SOIL2.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

using namespace std;

// scene pointer needed to resolve embedded textures in GLB files ("*0", "*1", ...)
GLint TextureFromFile(const char *path, string directory, const aiScene *scene = nullptr);

class Model
{
public:
    Model(GLchar *path)
    {
        this->loadModel(path);
    }

    void Draw(Shader shader)
    {
        for (GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(shader);
    }

    size_t meshCount() const { return meshes.size(); }

private:
    vector<Mesh>    meshes;
    string          directory;
    vector<Texture> textures_loaded;

    void loadModel(string path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // Derive directory — handles both / and \ separators (Windows)
        size_t slashPos = path.find_last_of("/\\");
        this->directory = (slashPos != string::npos) ? path.substr(0, slashPos) : ".";

        this->processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        for (GLuint i = 0; i < node->mNumChildren; i++)
            this->processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex>  vertices;
        vector<GLuint>  indices;
        vector<Texture> textures;

        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex    vertex;
            glm::vec3 v;

            v.x = mesh->mVertices[i].x;
            v.y = mesh->mVertices[i].y;
            v.z = mesh->mVertices[i].z;
            vertex.Position = v;

            v.x = mesh->mNormals[i].x;
            v.y = mesh->mNormals[i].y;
            v.z = mesh->mNormals[i].z;
            vertex.Normal = v;

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 uv;
                uv.x = mesh->mTextureCoords[0][i].x;
                uv.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = uv;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            // Diffuse; if absent (e.g. Blender exports PBR as map_Ke) fall back to emissive
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
            if (diffuseMaps.empty())
                diffuseMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_diffuse", scene);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular
            vector<Texture> specMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
            textures.insert(textures.end(), specMaps.begin(), specMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         string typeName, const aiScene *scene)
    {
        vector<Texture> textures;

        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            GLboolean skip = false;
            for (GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if (textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                Texture texture;
                texture.id   = TextureFromFile(str.C_Str(), this->directory, scene);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }

        return textures;
    }
};

GLint TextureFromFile(const char *path, string directory, const aiScene *scene)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    // ── Embedded texture (GLB / binary GLTF) ─────────────────────────────────
    // Assimp signals embedded textures with paths "*0", "*1", "*2", etc.
    if (path[0] == '*' && scene != nullptr)
    {
        int idx = atoi(path + 1);

        if (idx >= 0 && idx < (int)scene->mNumTextures)
        {
            const aiTexture *tex = scene->mTextures[idx];

            if (tex->mHeight == 0)
            {
                // Compressed image (PNG / JPEG) — mWidth is the byte size of pcData
                GLuint id = SOIL_load_OGL_texture_from_memory(
                    reinterpret_cast<const unsigned char *>(tex->pcData),
                    (int)tex->mWidth,
                    SOIL_LOAD_AUTO,
                    textureID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
                );
                if (id == 0)
                    cout << "SOIL: fallo textura embebida GLB [" << idx << "]" << endl;
                return id > 0 ? id : textureID;
            }
            else
            {
                // Uncompressed raw ARGB8888 — aiTexel is stored as B G R A
                int w = (int)tex->mWidth;
                int h = (int)tex->mHeight;
                vector<unsigned char> rgba(w * h * 4);
                for (int p = 0; p < w * h; p++)
                {
                    rgba[p * 4 + 0] = tex->pcData[p].r;
                    rgba[p * 4 + 1] = tex->pcData[p].g;
                    rgba[p * 4 + 2] = tex->pcData[p].b;
                    rgba[p * 4 + 3] = tex->pcData[p].a;
                }
                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                return textureID;
            }
        }

        cout << "ASSIMP: indice de textura embebida fuera de rango: " << idx << endl;
        return textureID;
    }

    // ── External file texture (OBJ / external GLTF) ───────────────────────────
    string filename = directory + '/' + string(path);

    int width = 0, height = 0;
    unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

    if (image && width > 0 && height > 0)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        cout << "SOIL: no se pudo cargar: " << filename << endl;
        // 1x1 white fallback so meshes without textures render lit instead of random color
        unsigned char white[3] = {255, 255, 255};
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    SOIL_free_image_data(image);
    return textureID;
}
