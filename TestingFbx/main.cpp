#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>
#include <stack>
#include <string>
#include "Test.h"

std::ostream& operator<<(std::ostream& os, aiVector3D& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Note: Quaternions are more complex than this. I just printed out the coordinates.
std::ostream& operator<<(std::ostream& os, aiQuaterniont<float>& q) {
    os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")"; 
    return os;
}

// Helper function to create the indent for printing the dfs nodes
// Each hyphen indicates one level down from the root in the scene graph
std::string makeIndent(int depth) {
    std::string indent = "";
    for (unsigned int i = 0; i < depth; i++) {
        indent += "-";
    }
    return indent;
}

// Prints the scene nodes traversed by DFS
void printNodes(aiNode* rootNode) {
    std::stack<std::pair<aiNode*, int>> dfsStack;
    int startDepth = 0;
    dfsStack.push(std::make_pair(rootNode, startDepth));
    while (!dfsStack.empty()) {
        // Get the top of the stack and print the node
        std::pair<aiNode*, int> top = dfsStack.top();
        aiNode* node = top.first;
        int depth = top.second;
        std::string indent = makeIndent(depth);
        std::cerr << indent << node->mName.C_Str() << std::endl;

        // Push node's children onto stack
        dfsStack.pop();
        aiNode** children = node->mChildren;
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            dfsStack.push(std::make_pair(children[i], depth + 1));
        }
    }
}

// Prints the important information for a node animation
void printNodeAnimation(aiNodeAnim* nodeAnim) {
    std::cerr << "Associated node: " << nodeAnim->mNodeName.C_Str() << std::endl;
    std::cerr << "Pre-state: " << nodeAnim->mPreState << std::endl;
    std::cerr << "Post-state: " << nodeAnim->mPostState << std::endl;

    // An `aiNodeAnim` object consists of a sequence of position, rotation, and scaling "keys"
    // that can be combined to generate a transformation matrix at a specific time to apply to the associated node

    std::cerr << "\nPosition keys:\n";
    aiVectorKey* posKeys = nodeAnim->mPositionKeys;
    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
        std::cerr << "Time: " << posKeys[i].mTime << " | Value: " << posKeys[i].mValue << std::endl;
    }

    std::cerr << "\nRotation keys:\n";
    aiQuatKey* rotKeys = nodeAnim->mRotationKeys;
    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        std::cerr << "Time: " << rotKeys[i].mTime << " | Value: " << rotKeys[i].mValue << std::endl;
    }

    std::cerr << "\nScaling keys:\n";
    aiVectorKey* scaleKeys = nodeAnim->mScalingKeys;
    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        std::cerr << "Time: " << scaleKeys[i].mTime << " | Value: " << scaleKeys[i].mValue << std::endl;
    }
}


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
        exit(1);
    }

    // Print out the nodes in the scene
    aiNode* rootNode = scene->mRootNode;
    std::cerr << "Printing all nodes in scene ...\n";
    printNodes(rootNode);
    std::cerr << "Done printing all nodes in scene \n\n";

    // Get all animations from the scene
    aiAnimation** animations = scene->mAnimations;
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* anim = animations[i];
        std::cerr << "Animation name: " << anim->mName.C_Str() << std::endl;
        // For each animation, get its node-specific animations 
        aiNodeAnim** nodeAnimations = anim->mChannels;
        for (unsigned int j = 0; j < anim->mNumChannels; j++) {
            aiNodeAnim* nodeAnim = nodeAnimations[j];
            printNodeAnimation(nodeAnim);            
        }
    }
    
	return 0;
}