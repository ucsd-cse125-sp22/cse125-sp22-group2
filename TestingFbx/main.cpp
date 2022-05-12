#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>
int main(void) {
    // Create an instance of the Importer class
    const char* animationFile = "../animations/BasicAnimation.fbx";
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(animationFile,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (!scene)
    {
        std::cerr << importer.GetErrorString() << std::endl;
    }
    else {
        const aiNode* rootNode = scene->mRootNode;
        std::cerr << "Root node name: " << rootNode->mName.C_Str() << std::endl;
        std::cerr << "Number of children: " << rootNode->mNumChildren << std::endl;

        for (unsigned int i = 0; i < rootNode->mNumChildren; i++) {
            const aiNode* node = rootNode->mChildren[i];
            std::cerr << "Child: " << node->mName.C_Str() << std::endl;
        }


        const unsigned int numAnimations = scene->mNumAnimations;
        std::cerr << "Number of animations: " << numAnimations << std::endl;
        

        aiAnimation** animations = scene->mAnimations;
        for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
            aiAnimation* anim = animations[i];
            std::cerr << "Animation name: " << anim->mName.C_Str() << std::endl;
        }

    }
	return 0;
}