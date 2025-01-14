/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GB_MODEL_READER_H
#define GB_MODEL_READER_H

// External
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

// QT
#include <QObject>
#include <QOpenGLBuffer>

// Internal
#include "core/rendering/geometry/GVertexData.h"
#include "core/rendering/materials/GMaterial.h"
#include "heave/collisions/GCollisions.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
/////////////////////////////////////////////////////////////////////////////////////////////
namespace rev {  

class ResourceHandle;
class ResourceCache;
class Mesh;
class Model;
class VertexArrayData;
class Bone;
class Animation;
class SkeletonJoint;

/////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
/////////////////////////////////////////////////////////////////////////////////////////////
/// @struct ModelChunkData
struct ModelChunkData {
    //Uuid m_meshHandleID;
    //Uuid m_matHandleID;
    uint32_t m_meshHandleIndex; // index in model reader's list of mesh handles
    uint32_t m_matHandleIndex; // index in model reader's list oof material handles
    AABBData m_boundingBox;
};


/// @class ModelReader
/// @brief For loading in 3D model data from a .obj file
/// @note See:
/// https://www.ics.com/blog/qt-and-opengl-loading-3d-model-open-asset-import-library-assimp
/// https://learnopengl.com/Model-Loading/Assimp
/// https://learnopengl.com/Model-Loading/Mesh
/// https://learnopengl.com/Model-Loading/Model
/// https://github.com/Jas03x/GL_GameKit/blob/436a7921f0c342de681732f6dd850277626fc03a/src/ColladaLoader.cpp
class ModelReader {
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Static
    /// @{    

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructor/Destructpr
    /// @{    

    ModelReader(ResourceCache* cache, ResourceHandle& handle);
    ModelReader(const ModelReader& reader);
    ~ModelReader();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{ 

    const aiScene* scene() const { return m_scene; }

    std::vector<ModelChunkData>& chunks() { return m_chunks; }


    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public methods
    /// @{    

    /// @brief Load in a file
    void loadFile();
    
    /// @brief Retrieve a model at the given filepath (loading only if not loaded)
    Model* loadModel();

    /// @}   

protected:

    friend class Model;

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Private methods
    /// @{    

    /// @brief Load model using assimp
    void loadAssimp();

    /// @brief Load materials into the resource cache
    void loadMaterials();

    /// @brief Construct mesh data after parsing from a file
    void loadMeshes();

    /// @brief load animations
    void loadAnimations();

    /// @brief Process mesh data from a assimp mesh
    const Mesh& processMesh(aiMesh* mesh);

    /// @brief Process material from ASSIMP data
    MaterialData processMaterial(aiMaterial* mat);

    /// @brief Create material from material data
    Material* createMaterial(const MaterialData& data);

    /// @brief Calculate the object-space transform of the node
    Matrix4x4g calculateGlobalNodeTransform(const aiNode* root);

    /// @brief Process texture type from material
    void processTexture(aiMaterial* mat, MaterialData& outMaterial, aiTextureType type);

    /// @brief Process nodes for metadata
    void processNodes(const aiNode* node);

    /// @brief Recursive method for walking aiMesh to construct output
    void parseNodeHierarchy(aiNode* aiNode, uint32_t currentNodeIndex);

    /// @brief Add animation data to skeleton if necessary
    void postProcessSkeleton();
    void postProcessSkeleton(SkeletonJoint& joint);

    /// @brief Finalize loading of model
    void postProcessModel();

    /// @brief Convert an assimp matrix to a custom matrix
    Matrix4x4g toMatrix(const aiMatrix4x4& mat);

    Model* model();

    /// @}   

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Private Members
    /// @{    

    /// @brief The resource handle that the model is being loaded into
    ResourceHandle* m_handle;

    /// @brief Resource cache
    ResourceCache* m_resourceCache;

    /// @brief Assimp importer
    Assimp::Importer m_importer;

    /// @brief Assimp scene
    const aiScene* m_scene{nullptr};

    /// @brief Vector of node names
    std::vector<GString> m_nodeNames;

    /// @brief Vector of model chunk data
    std::vector<ModelChunkData> m_chunks;

    /// @brief Loaded mesh handles
    std::vector<std::shared_ptr<ResourceHandle>> m_meshHandles;

    /// @brief Loaded material handles
    std::vector<std::shared_ptr<ResourceHandle>> m_matHandles;

    /// @brief Map of bones, according to node/bone name
    tsl::robin_map<GString, Bone*> m_boneMapping;

    ///// @brief Vector of meshes, by name and index in scene mesh list
    //std::vector<GString> m_meshNames;

    /// @brief Vector of material data
    std::vector<MaterialData> m_materialData;

    /// @brief Count of animated joints
    int m_animatedJointCount = 0;

    /// @brief Vector of animations
    std::vector<Animation*> m_animations;

    bool m_processedSkeleton = false;

    /// @brief Global inverse transformation
    Matrix4x4g m_globalInverseTransform;

    GString m_filePath; ///< Filepath to the model

    /// @}   
};

        
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif