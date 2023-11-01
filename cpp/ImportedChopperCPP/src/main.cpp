// ---------------------------- Includes -------------------------- //
#include <stdlib.h>         // C++ standard library
#include <stdexcept>
#include <stack>            // We use the standard C++ stack implementation to create model matrix stacks
#include <vector>           // We use the standard C++ vector to store values in
#include <map>
#include <functional>
#include <cmath>

#include <algorithm>        // Sorting is in there
#include <unistd.h>         // Threading
#include <stdio.h>          // Input/Output
#include <iostream>
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library
#include <glm/gtx/rotate_vector.hpp>


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "shader_util.h"
#include "texture_util.h"
#include "geometry.h"

#define WEIGHTS_PER_VERT 4

//These will hold our hangar
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO;
GLuint cubeVAO, sphereVAO; //No need to use those.

// Here are the structs, where we hold our objects in.
// Of course, would be better to do classes instead.

/**
 * Bones are inside rigged meshes.
 */
struct Bone {
    std::string name;
    glm::mat4 model;
    glm::mat4 localTransform;  //Bone's local transformation
    glm::mat4 offsetTransform; //Model space -> this bone space
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 scale;
    std::string parentName;
    bool isRoot;
};

/**
 * Node animation has the keys for this node (bone)
 */
struct NodeAnimation {
    std::string name;
    std::map<float, glm::vec3> positionKeys;
    std::map<float, glm::quat> rotationKeys;
    std::map<float, glm::vec3> scaleKeys;
};

/**
 * Animation consists of a map of NodeAnimations.
 * Also other values specifying the speed.
 */
struct Animation {
    std::string name;
    std::map<std::string, NodeAnimation> nodeAnimations;
    float duration; //Total ticks
    float tps;      //Ticks per second
    float length;   //The total time length of this animation
    double time;    //How many ticks this animation has played
};

/**
 * This struct currently holds all our objects.
 */
struct Object3D {
    GLuint vao;
    int indexCount;
    glm::mat4 model;        //This is the base transform of the entire object. We only change it upon initialization.
    glm::vec3 rotation;     // rotation, position and scale are there to apply additional transformations.
    glm::vec3 position;
    glm::vec3 scale;
    std::map<std::string, Bone> bones;
    std::map<std::string, Animation> animations;
    std::vector<Object3D> children;
    glm::mat4 rigTransform;
    GLuint textureHandle;
};

/**
 * This should be familiar from the Projected Chopper task.
 */
struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    Camera(glm::mat4 in_projection, glm::mat4 in_view) {
        projection = in_projection;
        view = in_view;
    }
};
Camera* mainCamera;     //Pointer to our main (perspective) camera


Object3D chopperOBJ;        //We want to have these objects in our scene.
Object3D chopperCollada;
Object3D marine;

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog defaultShader("shaders/default.vert.glsl", "shaders/default.frag.glsl");
shader_prog skinnedShader("shaders/skinned.vert.glsl", "shaders/skinned.frag.glsl");

float screenWidth = 800;
float screenHeight = 450;

float speed = 0.0;      // This is the movement speed of our Marine
float rotSpeed = 0.0;   // This is the rotation speed of our Marine

/**
 * Init the hangar. Still just 5 quads.
 */
void initHangar() {
    printf("Init walls\n");
    defaultShader.activate();
    leftWallVAO = createQuad(glm::vec3(0.66, 0.66, 0.66), &defaultShader);
    rightWallVAO = createQuad(glm::vec3(0.4, 0.4, 0.4), &defaultShader);
    backWallVAO = createQuad(glm::vec3(1.0, 0.0, 0.0), &defaultShader);
    ceilingVAO = createQuad(glm::vec3(0.33, 0.33, 0.33), &defaultShader);
    floorVAO = createQuad(glm::vec3(0.22, 0.22, 0.22), &defaultShader);
}


/**
 * Drawing the hangar (each wall).
 */
void drawHangar(shader_prog* shader) {
    shader->activate();

    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

    //Because of the correct use of the matrix stack, I can scale the entire hangar here! :)
    ms.top() = glm::scale(ms.top(), glm::vec3(2.0, 2.0, 2.0));
    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 5.0, 0.0));

    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(-10.0, 0.0, 10.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        ms.top() = glm::scale(ms.top(), glm::vec3(2.0, 1.0, 1.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(leftWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(10.0, 0.0, 10.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        ms.top() = glm::scale(ms.top(), glm::vec3(2.0, 1.0, 1.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(rightWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, -10.0, 10.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        ms.top() = glm::scale(ms.top(), glm::vec3(1.0, 2.0, 1.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 10.0, 10.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        ms.top() = glm::scale(ms.top(), glm::vec3(1.0, 2.0, 1.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(ceilingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(backWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
}

/**
 * Recursive drawing for object hierarchies.
 */
void drawObjectRec(Object3D* object, std::stack<glm::mat4>* ms, shader_prog* shader) {
    ms->push(ms->top());
        ms->top() = glm::translate(ms->top(), object->position);
        ms->top() = glm::rotate(ms->top(), object->rotation.x, glm::vec3(1.0, 0.0, 0.0));
        ms->top() = glm::rotate(ms->top(), object->rotation.y, glm::vec3(0.0, 1.0, 0.0));
        ms->top() = glm::rotate(ms->top(), object->rotation.z, glm::vec3(0.0, 0.0, 1.0));
        ms->top() = glm::scale(ms->top(), object->scale);
        ms->top() = ms->top() * object->model;

        shader->uniformMatrix4fv("modelMatrix", ms->top());
        glBindVertexArray(object->vao);
        glDrawElements(GL_TRIANGLES, object->indexCount, GL_UNSIGNED_INT, 0);

        if (object->textureHandle > 0) {
            glActiveTexture(GL_TEXTURE0); //We activate the first texture uniform in the shader
            glBindTexture(GL_TEXTURE_2D, object->textureHandle); //Bind specific textue to it
        }

        for (unsigned int i = 0; i < object->children.size(); i++) {
            drawObjectRec(&object->children[i], ms, shader);
        }
    ms->pop();
}

/**
 * Draw one Object3D.
 */
void drawObject(Object3D* object, shader_prog* shader) {
    shader->activate();

    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0));
        drawObjectRec(object, &ms, shader);
    ms.pop();
}

/**
 * Import a model with AssImp
 * http://assimp.sourceforge.net/lib_html/usage.html
 */
bool DoTheImportThing(const std::string& pFile, std::function<Object3D(const aiScene*)> callback, Object3D &object) {

  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile( pFile,
        aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType            |
        aiProcess_GenSmoothNormals
    );

    if(!scene) {
        printf("Error importing a file: %s\n", importer.GetErrorString());
        return false;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        printf("Loaded mesh: %s \n", scene->mMeshes[i][0].mName.C_Str());
        printf("Vertex count: %d \n", scene->mMeshes[i][0].mNumVertices);
        std::cout << scene->mMeshes[i][0].mName.data << std::endl;
    }

    //Next we print some data. Just for logging the hierarchi of what was loaded.
    struct Node {
        aiNode* node;
        int level;
    };

    printf("Loaded hierarchy: \n");
    std::stack< std::pair<aiNode*, int> > nodes = std::stack< std::pair<aiNode*, int> >();
    nodes.push(std::make_pair(scene->mRootNode, 0));
    while (!nodes.empty()) {
        std::pair<aiNode*, int> node = nodes.top(); nodes.pop();
        for (int i = 0; i <= node.second; i++) {
            printf("-");
        }
        printf(" %s \n", node.first->mName.C_Str());
        for (unsigned int i = 0; i < node.first->mNumChildren; i++) {
            nodes.push(std::make_pair(node.first->mChildren[i], node.second + 1));
        }
    }

  // This callback will initialize the loaded object.
  object = callback(scene);

  return true; // We're done. Everything will be cleaned up by the importer destructor.
}

/**
 * Initializes a skinned object.
 * Not very optimal, sorry.
 */
Object3D initSkinnedObject(const aiScene* scene, char* name) {
    aiNode* node = scene->mRootNode->FindNode(name);
    if (node == NULL) { //We create a new node, if no node with such a name
        printf("Did not found node with name \"%s\". Created empty node.\n", name);
        Object3D object = Object3D();
        object.vao = 0;
        object.indexCount = 0;
        object.model = glm::mat4(1.0);
        object.rotation = glm::vec3(0.0);
        object.position = glm::vec3(0.0);
        object.scale = glm::vec3(1.0);

        return object;
    }
    Object3D object = Object3D();

    printf("Initializing node \"%s\".\n", name);
    if (node->mNumMeshes > 1) {
        printf("Multiple meshes in one node not supported!\n");
    }

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    std::vector<glm::vec3> positions = std::vector<glm::vec3>();
    std::vector<glm::vec3> normals = std::vector<glm::vec3>();
    std::vector<glm::vec3> colors = std::vector<glm::vec3>();
    std::vector<glm::vec2> uvs = std::vector<glm::vec2>();

    std::vector<int> boneIds = std::vector<int>();
    std::vector<float> boneWeights = std::vector<float>();
    std::map<int, std::vector<std::pair<int, float> > > boneMap = std::map<int, std::vector<std::pair<int, float> > >();
    std::vector<glm::mat4> boneMatrices = std::vector<glm::mat4>();
    std::map<std::string, Bone> bones = std::map<std::string, Bone>();

    std::vector<unsigned int> indices = std::vector<unsigned int>();


    printf("Meshes: %d\n", node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        printf("Bones: %d\n", mesh->mNumBones);

        for (unsigned int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];

            Bone boneStruct = Bone(); //Create new bone structs
            boneStruct.name = std::string(bone->mName.C_Str());
            boneStruct.offsetTransform = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));

            bones.insert(std::make_pair(std::string(bone->mName.C_Str()), boneStruct));
        }
        for (unsigned int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];

            for (unsigned int k = 0; k < bone->mNumWeights; k++) { //We have to map vertices to bones, populate bone map
                int index = distance(bones.begin(), bones.find(std::string(bone->mName.C_Str())));
                boneMap[bone->mWeights[k].mVertexId].push_back(std::pair<int, float>(index, bone->mWeights[k].mWeight));
            }
        }

        for (std::map<std::string, Bone>::iterator it = bones.begin(); it != bones.end(); it++) {
            Bone* bone = &(it->second);
            bone->isRoot = true;
            aiNode* boneNode = scene->mRootNode->FindNode(bone->name.c_str());
            bone->model = glm::transpose(glm::make_mat4(&(boneNode->mTransformation.a1)));
            std::string parentName = std::string(boneNode->mParent->mName.C_Str());

            std::map<std::string, Bone>::iterator parentIt = bones.find(parentName);
            if (parentIt != bones.end()) {
                bone->parentName = parentIt->first;
                bone->isRoot = false;
            } else {
                printf("Root: %s\n", bone->name.c_str());
            }
        }
        skinnedShader.uniformVecMat4("boneMatrices", boneMatrices); //Send the bone matrices to skinnedShader

        printf("Vertices: %d\n", mesh->mNumVertices);
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D diffuse; material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse); //Get the diffuse color

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) { //We have a texture for the diffuse color
            aiString textureName;
            scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);
            printf("Texture file: %s\n", textureName.C_Str());
            std::string file = std::string("data/") + std::string(textureName.C_Str());
            object.textureHandle = load_texture(GL_TEXTURE_2D, GL_SRGB, file.c_str());
        }

        for (unsigned int j = 0; j < mesh->mNumVertices; j++) { // j-th vertex inside this mesh

            positions.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
            normals.push_back(glm::normalize(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z)));
            colors.push_back(glm::vec3(diffuse.r, diffuse.g, diffuse.b));

            if (object.textureHandle > 0) { // We have a texture, send UV-s (only supports one texture currently)
                uvs.push_back(glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y));
            }

            if (boneMap[j].size() > WEIGHTS_PER_VERT) { //This can happen (happens a lot with the Marine)
                //printf("WARNING, too many bones for one vertes: %d\n", boneMap[j].size());
            }

            //vector of <ID, weight>  - for vertex j
            std::vector<std::pair<int, float> > weights = boneMap[j];
            std::sort(weights.begin(), weights.end(), [](const std::pair<int, float> a, const std::pair<int, float> b) { return a.second > b.second; });


            for (unsigned int k = 0; k < WEIGHTS_PER_VERT; k++) {
                if (k < weights.size() && weights[k].second > 0.0) {
                    boneIds.push_back(weights[k].first);        //Assign bone ID
                    boneWeights.push_back(weights[k].second);   //Assign weight for that bone
                    if (weights[k].first > 56) {
                        printf("Too large index, not enough matrices! %d", weights[k].first);
                    }
                } else {
                    boneIds.push_back(0);
                    boneWeights.push_back(0.0f);
                }
            }
        }
        printf("Faces: %d\n", mesh->mNumFaces);
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) { //Populate the indices
            for (unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
                if (mesh->mFaces[j].mNumIndices != 3) {
                    printf("Untringualated faces (%d indices) not supported!\n", mesh->mFaces[j].mNumIndices);
                }
                indices.push_back(mesh->mFaces[j].mIndices[k]);
            }
        }
        printf("Elements: %d\n", indices.size());
    }

    std::map<std::string, Animation> animations = std::map<std::string, Animation>();
    printf("Animations count: %d\n", scene->mNumAnimations);
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) { //We have animations, populate them as well
        float sampleRate = 2.0;

        Animation animation = Animation();
        animation.name = std::string(scene->mAnimations[i]->mName.C_Str());
        animation.duration = (float)scene->mAnimations[i]->mDuration + sampleRate;
        animation.tps = (float)scene->mAnimations[i]->mTicksPerSecond;
        animation.length = animation.duration / animation.tps;
        animation.time = 0.0;

        printf("Loaded animation: %s\n", animation.name.c_str());
        for (unsigned int j = 0; j < scene->mAnimations[i]->mNumChannels; j++) {
            aiNodeAnim* channel = scene->mAnimations[i]->mChannels[j];

            NodeAnimation nodeAnimation = NodeAnimation();
            nodeAnimation.name = std::string(channel->mNodeName.C_Str());

            unsigned int k;
            //Position keys
            for (k = 0; k < channel->mNumPositionKeys; k++) {
                glm::vec3 posValue = glm::vec3(channel->mPositionKeys[k].mValue.x, channel->mPositionKeys[k].mValue.y, channel->mPositionKeys[k].mValue.z);
                nodeAnimation.positionKeys.insert(std::make_pair(channel->mPositionKeys[k].mTime, posValue));
            }
            k = channel->mNumPositionKeys - 1;
            glm::vec3 posValue = glm::vec3(channel->mPositionKeys[0].mValue.x, channel->mPositionKeys[0].mValue.y, channel->mPositionKeys[0].mValue.z);
            nodeAnimation.positionKeys.insert(std::make_pair(channel->mPositionKeys[k].mTime + sampleRate, posValue));


            //Rotation keys
            for (k = 0; k < channel->mNumRotationKeys; k++) {
                glm::quat rotValue = glm::quat(channel->mRotationKeys[k].mValue.w, channel->mRotationKeys[k].mValue.x, channel->mRotationKeys[k].mValue.y, channel->mRotationKeys[k].mValue.z);
                nodeAnimation.rotationKeys.insert(std::make_pair(channel->mPositionKeys[k].mTime, rotValue));
            }
            k = channel->mNumRotationKeys - 1;
            glm::quat rotValue = glm::quat(channel->mRotationKeys[0].mValue.w, channel->mRotationKeys[0].mValue.x, channel->mRotationKeys[0].mValue.y, channel->mRotationKeys[0].mValue.z);
            nodeAnimation.rotationKeys.insert(std::make_pair(channel->mPositionKeys[k].mTime + sampleRate, rotValue));

            //Scale keys
            for (k = 0; k < channel->mNumScalingKeys; k++) {
                glm::vec3 scaleValue = glm::vec3(channel->mScalingKeys[k].mValue.x, channel->mScalingKeys[k].mValue.y, channel->mScalingKeys[k].mValue.z);
                nodeAnimation.scaleKeys.insert(std::make_pair(channel->mScalingKeys[k].mTime, scaleValue));
            }
            k = channel->mNumScalingKeys - 1;
            glm::vec3 scaleValue = glm::vec3(channel->mScalingKeys[0].mValue.x, channel->mScalingKeys[0].mValue.y, channel->mScalingKeys[0].mValue.z);
            nodeAnimation.scaleKeys.insert(std::make_pair(channel->mScalingKeys[k].mTime + sampleRate, scaleValue));

            animation.nodeAnimations.insert(std::make_pair(nodeAnimation.name, nodeAnimation));
        }
        animations.insert(std::make_pair(animation.name, animation));
    }

    if (WEIGHTS_PER_VERT == 4) { //Send the bone ids and weights for vertices
        skinnedShader.attributeVectorInt("boneIds", boneIds);
        skinnedShader.attributeVectorFloat("boneWeights", boneWeights);
    }
    skinnedShader.attributePosColNom(positions, colors, normals, indices); //The usual
    if (uvs.size() > 0) { //Also send UV-s
        skinnedShader.attributeVectorVec2("uv", uvs);
    }

    glBindVertexArray(0);

    object.vao = vertexArrayHandle;
    object.indexCount = indices.size();

    object.model = glm::transpose(glm::make_mat4(&node->mTransformation.a1));
    object.rotation = glm::vec3(0.0);
    object.position = glm::vec3(0.0);
    object.scale = glm::vec3(1.0);
    object.bones = bones;
    object.animations = animations;

    return object;
}

/**
 * Initializes a simple object.
 * Same as initSkinnedObject, but without bones, animations and diffuse texture.
 */
Object3D initObject(const aiScene* scene, char* name) {
    aiNode* node = scene->mRootNode->FindNode(name);
    if (node == NULL) { //Create an empty node if none was imported
        printf("Did not found node with name \"%s\". Created empty node.\n", name);
        Object3D object = Object3D();
        object.vao = 0;
        object.indexCount = 0;
        object.model = glm::mat4(1.0);
        object.rotation = glm::vec3(0.0);
        object.position = glm::vec3(0.0);
        object.scale = glm::vec3(1.0);

        return object;
    }
    Object3D object = Object3D();

    printf("Initializing node \"%s\".\n", name);
    if (node->mNumMeshes > 1) {
        printf("Multiple meshes in one node not supported!\n");
    }

    GLuint vertexArrayHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    std::vector<glm::vec3> positions = std::vector<glm::vec3>();
    std::vector<glm::vec3> normals = std::vector<glm::vec3>();
    std::vector<glm::vec3> colors = std::vector<glm::vec3>();

    std::vector<unsigned int> indices = std::vector<unsigned int>();

    printf("Meshes: %d\n", node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        printf("Vertices: %d\n", mesh->mNumVertices);
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D diffuse; material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse); //Get the diffuse color

        for (unsigned int j = 0; j < mesh->mNumVertices; j++) { // j-th vertex inside this mesh
            positions.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
            normals.push_back(glm::normalize(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z)));
            colors.push_back(glm::vec3(diffuse.r, diffuse.g, diffuse.b));
        }

        printf("Faces: %d\n", mesh->mNumFaces);
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            for (unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
                if (mesh->mFaces[j].mNumIndices != 3) {
                    printf("Untringualated faces (%d indices) not supported!\n", mesh->mFaces[j].mNumIndices);
                }
                indices.push_back(mesh->mFaces[j].mIndices[k]);
            }
        }
        printf("Elements: %d\n", indices.size());
    }

    defaultShader.attributePosColNom(positions, colors, normals, indices);
    glBindVertexArray(0);

    object.vao = vertexArrayHandle;
    object.indexCount = indices.size();

    object.model = glm::transpose(glm::make_mat4(&node->mTransformation.a1));
    object.rotation = glm::vec3(0.0);
    object.position = glm::vec3(0.0);
    object.scale = glm::vec3(1.0);
    return object;
}

/**
 * Initialize the OBJ Chopper
 */
Object3D initChopperOBJ(const aiScene* scene) {
    Object3D chopper = initObject(scene, "Chopper");

    /**
     * --Task--
     * Finish building the chopper's hierarchy.
     * 1) Make the chopper.model rotate 90 degrees around Y.
     * 2) Call initObject(scene, "Body") and push it to the Chopper's children
     * 3) Do the same for Blades
     * 4) Same for Blade1 and Blade2, but push them to the Blade's children.
     */

    return chopper;
}

/**
 * Initialize the Collada Chopper
 */
Object3D initChopperCollada(const aiScene* scene) {
    Object3D chopper = initObject(scene, "Chopper");

    /**
     * --Task--
     * Finish loading the Collada Chopper.
     * Although Collada does store the hierarchy, currently this base code wants you to still reconstruct it.
     * For automatic loading we would need to do an initHierarchicalObject() function. Not done yet, sorry.
     * So, basically just do the same things you did in the initObjChopper function.
     *
     * Exception is that for the model we apply:
     * 1) Rotation of 90 degrees around Y
     * 2) Rotation of -90 degrees around X
     * For some reason Blender's Collada exporter seems incapable of converting to our Y=up, -Z=forward system.
     */

    return chopper;
}

/**
 * Initialize the Marine FBX
 */
Object3D initMarine(const aiScene* scene) {
    Object3D marine = initSkinnedObject(scene, "male_marine");

    glm::mat4 rootM = glm::transpose(glm::make_mat4(&(scene->mRootNode->mTransformation.a1)));
    glm::mat4 rigM = glm::transpose(glm::make_mat4(&(scene->mRootNode->FindNode("marine_rig")->mTransformation.a1)));
    glm::mat4 fbxM = glm::transpose(glm::make_mat4(&(scene->mRootNode->FindNode("Fbx01")->mTransformation.a1)));

    marine.rigTransform = glm::inverse(marine.model) * rigM * fbxM;

    /**
     * --Task--
     * 1) Scale the Marine by 0.0003. It is modelled very large.
     * 2) Apply a translation -2 units in Z
     * 3) Put the initial position (marine.position now, not the model matrix) at (0, 0, 40).
     */

    return marine;
}

/**
 * Update the bone's local transformation based on the animation and it's weight
 */
void updateBone(Bone* bone, Animation* animation, float animW) {

    float time = fmod(animation->time * animation->tps, animation->duration);

    std::map<std::string, NodeAnimation>::iterator nodeAnimPair = animation->nodeAnimations.find(bone->name);
    if (nodeAnimPair != animation->nodeAnimations.end()) {
        NodeAnimation nodeAnim = nodeAnimPair->second;

        float w = 0.0f; //Interpolate the position
        glm::vec3 pos1 = glm::vec3(0.0);
        glm::vec3 pos2 = glm::vec3(0.0);
        for (std::map<float, glm::vec3>::iterator it = nodeAnim.positionKeys.begin(); it != nodeAnim.positionKeys.end(); it++) {
            if (it->first > time) {
                w = float(it->first - time) / (it->first - w);
                pos2 = glm::mix(it->second, pos1, w);
                break;
            }
            pos1 = it->second;
            w = it->first;
        }
        bone->position = pos2;

        w = 0.0; //Interpolate the rotation
        glm::quat rot1 = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::quat rot2 = glm::quat(1.0, 0.0, 0.0, 0.0);
        for (std::map<float, glm::quat>::iterator it = nodeAnim.rotationKeys.begin(); it != nodeAnim.rotationKeys.end(); it++) {
            if (it->first > time) {
                w = float(it->first - time) / (it->first - w);
                rot2 = glm::slerp(it->second, rot1, w);
                break;
            }
            rot1 = it->second;
            w = it->first;
        }
        bone->rotation = rot2;

        w = 0.0; //Interpolate the scale
        glm::vec3 sca1 = glm::vec3(1.0);
        glm::vec3 sca2 = glm::vec3(1.0);
        for (std::map<float, glm::vec3>::iterator it = nodeAnim.scaleKeys.begin(); it != nodeAnim.scaleKeys.end(); it++) {
            if (it->first > time) {
                w = float(it->first - time) / (it->first - w);
                sca2 = glm::mix(it->second, sca1, w);
                break;
            }
            sca1 = it->second;
            w = it->first;
        }
        bone->scale = sca2;

        //Bone's local transformation is a weighted Pos * Rot * Scale.
        bone->localTransform += animW * glm::translate(glm::mat4(1.0), bone->position) * glm::mat4_cast(bone->rotation) * glm::scale(glm::mat4(1.0), bone->scale);
    }
}

/**
 * Blend and update the Marine's animations.
 */
void updateMarineAnimation(double dt) {

    Animation* run = &marine.animations.find(std::string("marine_rig|run"))->second;
    Animation* walk = &marine.animations.find(std::string("marine_rig|walk"))->second;
    Animation* idle = &marine.animations.find(std::string("marine_rig|idle"))->second;

    /**
     * --Task--
     * Use the Bernstein basis polynomials to interpolate between all 3 animations.
     * https://en.wikipedia.org/wiki/Bernstein_polynomial
     *
     * This means find w1, w2 and w3.
     */
    float w = speed / 4.0f;
    float w1 = 0.0;
    float w2 = 0.0;
    float w3 = 0.0;

    /**
     * --Task--
     * We need to normalize the playback speeds. The run->length, walk->length and idle->length are all different.
     * In order to blend the animations correctly, we need to scale all of them up / down to one length.
     * Find that one length (based on the w1, w2 and w3) and use that to find, how much each animation should advance.
     * Then advance the animations (run->time += ..., walk->time += ... and idle->time += ...)
     */


    //Next we update the individual bone transformations based on the times you just assigned.
    //After that new matrices are found and sent to the shaders.
    std::vector<glm::mat4> boneMatrices = std::vector<glm::mat4>();
    for (std::map<std::string, Bone>::iterator boneIt = marine.bones.begin(); boneIt != marine.bones.end(); boneIt++) {
        Bone* bone = &boneIt->second;

        bone->localTransform = glm::mat4(0.0);

        updateBone(bone, idle, w1);
        updateBone(bone, walk, w2);
        updateBone(bone, run, w3);
    }

    //We need to find the chained transformations for each bone to update the matrices.
    for (std::map<std::string, Bone>::iterator it = marine.bones.begin(); it != marine.bones.end(); it++) {
        Bone* bone = &it->second;
        glm::mat4 m = glm::mat4(1.0);
        do {
            m = bone->localTransform * m; //Local transformations move up a level to the parent bone
            if (bone->isRoot) break;
            bone = &marine.bones.at(bone->parentName);
        } while (true);

        bone = &it->second;
        m = marine.rigTransform * m * bone->offsetTransform; //Offset matrix: local space -> current bone space

        boneMatrices.push_back(m);
    }

    skinnedShader.activate(); //Send the updated matrices
    skinnedShader.uniformVecMat4("boneMatrices", boneMatrices);
}

/**
 * Update the Marine's position and the primary camera based on the speed.
 */
void updateMarinePosition(double dt) {

    /**
     * --Task--
     * Make the marine move and the camera to follow him in a TPS view.
     * For that:
     * 1) Find the "forward" vector of the marine by rotating the [0, 0, -1] by the Marine's Y rotation
     *    See: http://glm.g-truc.net/0.9.3/api/a00199.html
     * 2) Advance marine's position by the "forward" vector.
     *    I used speed * speed * dt as a coefficient for the forward vector. Seemed to produce ok result. You can try other coefficients.
     * 3) Assign a new view matrix for the main camera.
     * 3.1) Position of the camera is somewhere behind and up from the marine. You can re-use the "forward" vector
     * 3.2) Find out, where you want the camera to look at
     * 3.3) Up vector can stay the same.
     */


    if (speed > 0) { //Dampen the speed, you can try something else here as well.
        speed -= 0.08 * speed * dt;
    }
    marine.rotation.y += rotSpeed * dt;
}

/**
 * Draws the scene.
 * Hangar with the default shader, particles with the particle shader.
 */
void drawScene() {
    drawHangar(&defaultShader);
    drawObject(&chopperOBJ, &defaultShader);
    drawObject(&chopperCollada, &defaultShader);
    drawObject(&marine, &skinnedShader);
}


// ---------------------------- Keyboard and Input -------------------------- //

std::map<int, bool> keyboard = std::map<int, bool>();

void initKeyboard() {
    keyboard.insert(std::make_pair(GLFW_KEY_UP, false));
    keyboard.insert(std::make_pair(GLFW_KEY_DOWN, false));
    keyboard.insert(std::make_pair(GLFW_KEY_LEFT, false));
    keyboard.insert(std::make_pair(GLFW_KEY_RIGHT, false));
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS or action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
    if (action == GLFW_PRESS) {                         // Here is a good way to read and parse input
         if (keyboard.find(key) != keyboard.end()) {    // Dictionary holds info if a key is currently down or not
            keyboard.at(key) = true;                    // Each main loop iteration we check the keys (input(dt))
        }                                               // If correct keys are down, increase speed by accounting correct dt
    }
    if (action == GLFW_RELEASE) {
        if (keyboard.find(key) != keyboard.end()) {
            keyboard.at(key) = false;
        }
    }
}

void input(float dt) {
    if (keyboard.at(GLFW_KEY_UP)) {
        speed += 2.0 * dt;
        speed = std::min(4.0f, speed);
    }
    if (keyboard.at(GLFW_KEY_DOWN)) {
        speed -= 2.0 * dt;
        speed = std::max(0.0f, speed);
    }
    if (keyboard.at(GLFW_KEY_LEFT)) {
        rotSpeed += glm::radians(60.0f) * dt;
    }
    if (keyboard.at(GLFW_KEY_RIGHT)) {
        rotSpeed -= glm::radians(60.0f) * dt;
    }
    if (!keyboard.at(GLFW_KEY_LEFT) && !keyboard.at(GLFW_KEY_RIGHT)) {
        rotSpeed -= 5.0 * rotSpeed * dt;
    }
}
// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;
    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(screenWidth, screenHeight, "Imported Chopper", NULL, NULL);
    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    glewInit();

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(win);
    glewExperimental = GL_TRUE;

    GLenum status = glewInit();
    if(status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
    }

    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
    GLint maxVertexUniComp; glGetIntegerv (GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniComp);
    printf("Max vertex uniform components: %d\n", maxVertexUniComp);

    glfwSetKeyCallback(win, key_callback);
    initKeyboard();

    defaultShader.use();
    skinnedShader.use();

    initHangar();

    mainCamera = new Camera(
        glm::perspective(glm::radians(80.0f), screenWidth / screenHeight, 0.1f, 300.f),
        glm::lookAt(
            glm::vec3(0.0, 4.0, 30.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        )
    );

    glm::vec3 lightPosition;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);

    // -------------- Create objects ------------- //
    DoTheImportThing("data/chopper.obj", *initChopperOBJ, chopperOBJ);         //This is a chopper from Timo Kallaste
    DoTheImportThing("data/chopper.dae", *initChopperCollada, chopperCollada); //You can also try chopper-mat, which is the one Ats did (I added some colors).
    DoTheImportThing("data/marine.fbx", *initMarine, marine);       //Seems that Blender's Collada exporter can only export 1 animation. This is why we use FBX here.

    double dt = 0.0;             // We need to calculate the deltaTime each frame
    double currentTime = 0.0;    // Othewise we can't change the animation speeds correctly.
    double lastTime = 0.0;       // Marine's movement would also be no all that "correct".

    printf("Starting rendering loop...");
    while (!glfwWindowShouldClose(win)) {
        currentTime = glfwGetTime();
        dt = (currentTime - lastTime);
        lastTime = currentTime;

        input(dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPosition.x = 7.0f * sin(glfwGetTime()); //Move our light on a trajectory
        lightPosition.y = 7.0f * cos(glfwGetTime());
        lightPosition.z = -20.0f * sin(glfwGetTime()) + 5.0;

        defaultShader.activate(); // Send the updated values to the shaders
        defaultShader.uniformVec3("lightPosition", glm::vec3(mainCamera->view * glm::vec4(lightPosition, 1.0)));
        defaultShader.uniformMatrix4fv("projectionMatrix", mainCamera->projection);
        defaultShader.uniformMatrix4fv("viewMatrix", mainCamera->view);

        skinnedShader.activate();
        skinnedShader.uniformVec3("lightPosition", glm::vec3(mainCamera->view * glm::vec4(lightPosition, 1.0)));
        skinnedShader.uniformMatrix4fv("projectionMatrix", mainCamera->projection);
        skinnedShader.uniformMatrix4fv("viewMatrix", mainCamera->view);

        /**
         * --Task--
         * For both chopperOBJ and chopperCollada make:
         * - Them move up-down a bit based on cosine
         * - Make the blades rotate ;)
         */


        //Update the marine position and animation
        updateMarinePosition(dt);
        updateMarineAnimation(dt);

        drawScene();

        glfwSwapBuffers(win);

        usleep(100);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
