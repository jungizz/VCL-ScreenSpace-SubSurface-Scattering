//
//  Created by Song Jungeun on 2024/1/4
//

#pragma once

#ifndef objLoader_h
#define objLoader_h


#include <glm/glm.hpp>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<glm::vec2> texcoords;
std::vector<glm::u32vec3> triangles;


void processMesh(aiMesh* mesh, const aiScene* scene) 
{
    // mesh's vertex
    vertices.resize(mesh->mNumVertices);
    normals.resize(mesh->mNumVertices);
    texcoords.resize(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        glm::vec3 vector;

        // position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertices[i] = vector;

        // normal
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            normals[i] = vector;
        }

        // texture coordinate
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            texcoords[i] = vec;
        }
        else texcoords.push_back(glm::vec2(0.0f, 0.0f));

    }

    // mesh's face (triangle)
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        triangles.push_back(glm::u32vec3(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
    }
}

void processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

bool loadObj(const std::string& filePath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return 0;
    }
    processNode(scene->mRootNode, scene);
}

#endif