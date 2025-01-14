#ifndef GB_PHYSICS_QUERY_H
#define GB_PHYSICS_QUERY_H

/////////////////////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////////////////
// Standard
#include <type_traits>

// QT

// Internal
#include "fortress/layer/framework/GFlags.h"
#include "GPhysics.h"
#include "GPhysicsManager.h"
#include "fortress/containers/math/GVector.h"
#include "GPhysicsScene.h"
#include "GPhysicsActor.h"
#include "core/converters/GPhysxConverter.h"

namespace rev {

//////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
//////////////////////////////////////////////////////////////////////////////////
//class PhysicsActor;
class Scene;
//class PxRigidActor;
class PxShape;

//////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Class definitions
/////////////////////////////////////////////////////////////////////////////////////////////


/// @class QueryFilterData
// TODO: Implement data filtering
class QueryFilterData: public physx::PxQueryFilterData {
};


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

struct HitEnum {
    enum PxHitFlags : uint16_t
    {
        kPOSITION = (1 << 0),	//!< "position" member of #PxQueryHit is valid
        kNORMAL = (1 << 1),	//!< "normal" member of #PxQueryHit is valid
        kUV = (1 << 3),	//!< "u" and "v" barycentric coordinates of #PxQueryHit are valid. Not applicable to sweep queries.
        kASSUME_NO_INITIAL_OVERLAP = (1 << 4),	//!< Performance hint flag for sweeps when it is known upfront there's no initial overlap.
        //!< NOTE: using this flag may cause undefined results if shapes are initially overlapping.
        kMESH_MULTIPLE = (1 << 5),	//!< Report all hits for meshes rather than just the first. Not applicable to sweep queries.
        kMESH_ANY = (1 << 6),	//!< Report any first hit for meshes. If neither eMESH_MULTIPLE nor eMESH_ANY is specified,
        //!< a single closest hit will be reported for meshes.
        kMESH_BOTH_SIDES = (1 << 7),	//!< Report hits with back faces of mesh triangles. Also report hits for raycast
        //!< originating on mesh surface and facing away from the surface normal. Not applicable to sweep queries.
        //!< Please refer to the user guide for heightfield-specific differences.
        kPRECISE_SWEEP = (1 << 8),	//!< Use more accurate but slower narrow phase sweep tests.
        //!< May provide better compatibility with PhysX 3.2 sweep behavior.
        kMTD = (1 << 9),	//!< Report the minimum translation depth, normal and contact point.
        kFACE_INDEX = (1 << 10),	//!< "face index" member of #PxQueryHit is valid

        kDEFAULT = kPOSITION | kNORMAL | kFACE_INDEX,

        /** \brief Only this subset of flags can be modified by pre-filter. Other modifications will be discarded. */
        kMODIFIABLE_FLAGS = kMESH_MULTIPLE | kMESH_BOTH_SIDES | kASSUME_NO_INITIAL_OVERLAP | kPRECISE_SWEEP
    };
};
typedef QFlags<HitEnum::PxHitFlags> HitFlags;
Q_DECLARE_OPERATORS_FOR_FLAGS(HitFlags)

/// @brief Template class representing a hit
template <typename HitType>
class PhysicsHit {
public:
    static_assert((std::is_base_of<physx::PxRaycastHit, HitType>::value ||
        std::is_base_of<physx::PxSweepHit, HitType>::value ||
        std::is_base_of<physx::PxOverlapHit, HitType>::value), "Invalid hit type");

    static bool constexpr IsRaycastOrSweep = std::is_base_of<physx::PxRaycastHit, HitType>::value || std::is_base_of<physx::PxSweepHit, HitType>::value;
    
    //--------------------------------------------------------------------------------------------
    /// @name Constructor/Destructor
    /// @{

    PhysicsHit(const HitType& hit) : m_hit(hit){
    }
    ~PhysicsHit() {
    }

    /// @}

    //--------------------------------------------------------------------------------------------
    /// @name PxActorShape
    /// @{

    physx::PxRigidActor* actor() const { return m_hit.actor; }
    physx::PxShape* shape() const { return m_hit.shape; }
    SceneObject* sceneObject() const {
        return (SceneObject*)m_hit.actor->userData;
    }

    /// @}

    //--------------------------------------------------------------------------------------------
    /// @name PxQueryHit
    /// @{

    int faceIndex() const { return m_hit.faceIndex; }

    /// @}

    //--------------------------------------------------------------------------------------------
    /// @name PxLocationHit 
    /// @{

    /// @brief Whether or not the shape that was hit overlaps the raycast/sweep origin
    /// @note Only valid for raycasts and sweeps
    bool hadInitialOverlap() const {
        static_assert(IsRaycastOrSweep, "Invalid hit type");
        m_hit.hadInitialOverlap();
    }

    /// @note Only valid for raycasts and sweeps
    HitFlags flags() const {
        static_assert(IsRaycastOrSweep, "Invalid hit type");
        uint16_t pxFlags = (uint16_t)m_hit.flags;
        // Workaround for lack of |= integer operator
        HitFlags flags = Flags::toFlags<HitEnum::PxHitFlags, uint16_t>(pxFlags);
        auto flagInt = (uint16_t)flags;
        if (flagInt != pxFlags) Logger::Throw("Error, bad time");
        return flags;
    }

    /// @note Only valid for raycasts and sweeps
    Vector3 position() const {
        static_assert(IsRaycastOrSweep, "Invalid hit type");
        return PhysxConverter::FromPhysX(m_hit.position); 
    }

    /// @note Only valid for raycasts and sweeps
    Vector3 normal() const { 
        static_assert(IsRaycastOrSweep, "Invalid hit type");
        return PhysxConverter::FromPhysX(m_hit.normal);
    }

    /// @note Only valid for raycasts and sweeps
    float distance() const { 
        static_assert(IsRaycastOrSweep, "Invalid hit type");
        return m_hit.distance; 
    }

    /// @}

    /// @brief Reference to the wrapped physx hit
    const HitType& m_hit;
};
typedef PhysicsHit<physx::PxRaycastHit> PhysicsRaycastHit;
typedef PhysicsHit<physx::PxSweepHit> SweepHit; // TODO: Implement in scene
typedef PhysicsHit<physx::PxOverlapHit> OverlapHit; // TODO: Implement in scene


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Template class representing a set of hits of any type
/// @details Can be used for raycasts, sweeps, or overlap queries
/// A hit can be touching (querying continues after the hit), or blocking (querying ends once the blocking hit occurs)
/// Hits are assumed to be blocking by default
template <typename HitType>
class PhysicsHits{
public:

    /// @brief Returns the closest blocking hit result, invalid if hasBlock is false
    PhysicsHit<HitType> block() const { return m_hitBuffer.block; }

    /// @brief Whether or not there is a blocking hit
    bool hasBlock() const { return m_hitBuffer.hasBlock; }

    /// @brief Return the total number of hits, both blocking and touching
    uint32_t getNumHits() const{
        return m_hitBuffer.getNbAnyHits();
    }

    /// @brief Return the total number of touch hits
    uint32_t getNumTouches() const {
        return m_hitBuffer.getNbTouches();
    }

    /// @brief Return a vector of all touches
    void getTouches(std::vector<PhysicsHit<HitType>>& touches) const{
        uint32_t numTouches = getNumTouches();
        touches.reserve(numTouches);
        for (uint32_t i = 0; i < numTouches; i++) {
            touches.push_back(m_hitBuffer.getTouch(i));
        }
    }

    /// @brief Return a vector of all hits
    void getHits(std::vector<PhysicsHit<HitType>>& hits) const {
        uint32_t numHits = getNumHits();
        hits.reserve(numHits);
        for (uint32_t i = 0; i < numHits; i++) {
            hits.push_back(getHit(i));
        }
    }

    /// @brief Return any touch from its index in the touch array
    PhysicsHit<HitType> getTouch(uint32_t index) const {
        return PhysicsHit<HitType>(m_hitBuffer.getTouch(index));
    }

    /// @brief Return any hit from an index
    PhysicsHit<HitType> getHit(uint32_t index) const {
        return PhysicsHit<HitType>(m_hitBuffer.getAnyHit(index));
    }

    physx::PxHitBuffer<HitType> m_hitBuffer;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/// @class PhysicsRaycast
class PhysicsRaycast {
public:

    bool hadHit() const {
        return m_hits.getNumHits() != 0;
    }

    PhysicsRaycastHit firstHit() const {
        return m_hits.getHit(0);
    }

    //Real_t m_maxDistance = std::numeric_limits<Real_t>::infinity();
    Real_t m_maxDistance = (float)1e30;

    Vector3 m_origin;

    /// @brief Unit direction of the raycast
    Vector3 m_direction;

    /// @brief The raycast results
    PhysicsHits<physx::PxRaycastHit> m_hits;
};




//////////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces

#endif