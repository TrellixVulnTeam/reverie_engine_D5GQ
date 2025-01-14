#pragma once

// Internal
#include "core/GManager.h"
#include "fortress/types/GLoadable.h"
#include "GPhysics.h"
#include "fortress/containers/GContainerExtensions.h"

namespace rev {

class CoreEngine;
class ProcessManager;
class PhysicsScene;
class PhysicsMaterial;
class PhysicsGeometry;
class PhysicsShapePrefab;

enum class QueryFlag {
    kStatic = physx::PxQueryFlag::eSTATIC, // 	Traverse static shapes. 
    kDynamic = physx::PxQueryFlag::eDYNAMIC, //	Traverse dynamic shapes
    kPreFilter = physx::PxQueryFlag::ePREFILTER, // Run the pre-intersection-test filter (see PxQueryFilterCallback::preFilter()). 
    kPostFilter = physx::PxQueryFlag::ePOSTFILTER, // Run the post-intersection-test filter (see PxQueryFilterCallback::postFilter()). 
    kAnyHit = physx::PxQueryFlag::eANY_HIT, // Abort traversal as soon as any hit is found and return it via callback.block. Helps query performance. Both eTOUCH and eBLOCK hitTypes are considered hits with this flag. 
    kNoBlock = physx::PxQueryFlag::eNO_BLOCK, // All hits are reported as touching. Overrides eBLOCK returned from user filters with eTOUCH. This is also an optimization hint that may improve query performance. 
    kReserved = physx::PxQueryFlag::eRESERVED // Reserved for physx internal use.
};
typedef QFlags<QueryFlag> QueryFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(QueryFlags)


/// @class Error callback
class ErrorCallback : public physx::PxErrorCallback{
public:
    ErrorCallback();
    ~ErrorCallback();

    virtual void reportError(physx::PxErrorCode::Enum code,
        const char* message, 
        const char* file,
        int line);
};

/// @class PhysicsManager
/// @brief Class representing the physics manager
/// See: https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/HelloWorld.html
/// @todo Make this a singleton
class PhysicsManager : public Manager {
public:

    /// @brief  Foundation SDK singleton class. 
    /// @details  Need to have an instance of this class to instance the higher level SDKs
    static physx::PxFoundation* Foundation() { return s_foundation; }

    /// @brief Abstract singleton factory class used for instancing objects in the Physics SDK.
    /// @details Can use PxPhysics to set global parameters which will effect all scenes, create triangle meshes.
    /// @note Can get an instance of physx::PxPhysics by calling PxCreatePhysics()
    static physx::PxPhysics* Physics() { return s_physics; }

    /// @brief The CpuDispatcher is an abstract Class the SDK uses for interfacing with the application's thread pool. 
    /// @details Typically, there will be one single CpuDispatcher for the entire application, 
    /// since there is rarely a need for more than one thread pool. 
    /// A CpuDispatcher instance may be shared by more than one TaskManager, 
    /// for example if multiple scenes are being used.
    static physx::PxDefaultCpuDispatcher* Dispatcher() { return s_dispatcher; }

    /// @brief Pointers to all the physics scenes
    static std::vector<std::shared_ptr<PhysicsScene>>& Scenes() { return s_scenes; }

    /// @brief Pointers to all the physics shapes
    /// @details Shapes are indexed by unique name
    static PhysicsShapePrefab* DefaultShape();
    static PhysicsShapePrefab* Shape(const GString& name);
    static void RemoveShape(PhysicsShapePrefab* prefab);
    static const std::vector<std::unique_ptr<PhysicsShapePrefab>>& ShapePrefabs() { return s_shapes; }

    static std::shared_ptr<PhysicsMaterial> DefaultMaterial();
    static std::shared_ptr<PhysicsMaterial> Material(const GString& name);

    ///// @brief Pointers to all baked physics geometry
    //static tsl::robin_map<GString, std::shared_ptr<PhysicsGeometry>>& geometry() { return s_geometry; }

    /// @brief Pointers to all physics materials
    static std::vector<std::shared_ptr<PhysicsMaterial>>& Materials() { return s_materials; }

    /// @brief Clear all geometry and materials
    static void Clear(bool clearDefault);

    /// @name Constructors/Destructor
    /// @{
    PhysicsManager(CoreEngine* core);
    ~PhysicsManager();
    /// @}

    /// @name Public Methods
    /// @{

    /// @brief Get scene object from its rigid body component


    /// @brief Step the physics simulation    void step(float dt);
    void step(float dt);

    /// @}

    /// @name Friend Functions
    /// @{

    /// @brief Convert from the given class type to JSON
    /// @note Actually defined in namespace outside of class, so this should be recognized by nlohmann JSON
    /// @param orJson The output JSON
    /// @param korObject The object to convert to JSON
    friend void to_json(nlohmann::json& orJson, const PhysicsManager& korObject);

    /// @brief Convert from JSON to the given class type
    /// @param korJson The input JSON
    /// @param orObject The object to be obtained from JSON
    friend void from_json(const nlohmann::json& korJson, PhysicsManager& orObject);


    /// @}

protected:
    /// @name Friends
    /// @{

    friend class PhysicsShapePrefab;

    /// @}

    /// @name Protected Methods
    /// @{

    /// @brief Initialize the manager
    void initialize();

    /// @brief Delete the manager
    void onDelete();

    /// @brief Get the index of the prefab with the given name/Uuid in the internal vector
    /// @return -1 if prefab not found, the internal index otherwise
    static int GetPrefabIndex(const GString& name);
    static int GetPrefabIndex(const Uuid& uuid);

    //static const GString& DefaultShapeKey() { return s_defaultShapeKey; }
    //static const GString& DefaultMaterialKey() { return s_defaultMaterialKey; }

    /// @}

    /// @name Protected Members
    /// @{

    /// @brief Abstract base class for an application defined memory allocator
    physx::PxDefaultAllocator m_allocator;

    /// @brief User defined interface class. Used by the library to emit debug information
    ErrorCallback m_errorCallback;

    /// @brief  Foundation SDK singleton class. 
    /// @details  Need to have an instance of this class to instance the higher level SDKs
    static physx::PxFoundation* s_foundation;

    /// @brief Abstract singleton factory class used for instancing objects in the Physics SDK.
    /// @details Can use PxPhysics to set global parameters which will effect all scenes, create triangle meshes.
    /// Can get an instance of this class by calling PxCreatePhysics()
    static physx::PxPhysics* s_physics;

    /// @brief A CpuDispatcher is responsible for scheduling the execution of tasks passed to it by the SDK
    static physx::PxDefaultCpuDispatcher* s_dispatcher;

    /// @brief Pointers to all the physics scenes
    static std::vector<std::shared_ptr<PhysicsScene>> s_scenes;

    /// @brief Pointers to all physics shapes
    static std::vector<std::unique_ptr<PhysicsShapePrefab>> s_shapes;

    ///// @brief Pointers to all baked physics geometry
    //static tsl::robin_map<GString, std::shared_ptr<PhysicsGeometry>> s_geometry;

    /// @brief Pointers to all physics materials
    static std::vector<std::shared_ptr<PhysicsMaterial>> s_materials;

    physx::PxPvd* m_pvd = nullptr;

    static GString s_defaultShapeKey;
    static GString s_defaultMaterialKey;

    /// @}

};


} // End namespaces
