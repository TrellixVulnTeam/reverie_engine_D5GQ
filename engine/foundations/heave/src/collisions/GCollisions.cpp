#include "heave/collisions/GCollisions.h"

#include <algorithm>
#include "fortress/containers/math/GVector.h"
#include "heave/kinematics/GTransform.h"
#include "heave/collisions/GFrustum.h"
#include "logging/GLogger.h"

namespace rev{


// CollidingGeometry

CollidingGeometry::CollidingGeometry(GeometryType type):
    m_geometryType(type)
{
}

CollidingGeometry::~CollidingGeometry()
{
}




// AABBData

AABBData::AABBData()
{
}

AABBData::~AABBData()
{
}

AABBData & AABBData::operator=(const AABBData & other)
{
    m_min = other.m_min;
    m_max = other.m_max;
    return *this;
}

Vector3 AABBData::getDimensions() const
{
    return Vector3(maxX() - minX(), maxY() - minY(), maxZ() - minZ());
}

Vector3 AABBData::getOrigin() const
{
    return Vector3((maxX() + minX()) / 2, (maxY() + minY()) / 2, (maxZ() + minZ()) / 2);
}



// AABB

AABB::AABB(): CollidingGeometry(kAABB)
{
}

AABB::AABB(const AABBData & data):
    CollidingGeometry(kAABB),
    m_boxData(data)
{
}

AABB::~AABB()
{
}

AABB & AABB::operator=(const AABB & other)
{
    m_geometryType = other.m_geometryType;
    m_boxData = other.m_boxData;
    return *this;
}

bool AABB::intersects(const CollidingGeometry & other) const
{
    bool hit;
    switch (other.geometryType()) {
    case kAABB:
    {
        const AABB& aabb = *static_cast<const AABB*>(&other);
        hit = (m_boxData.minX() <= aabb.m_boxData.maxX() && m_boxData.maxX() >= aabb.m_boxData.minX()) &&
              (m_boxData.minY() <= aabb.m_boxData.maxY() && m_boxData.maxY() >= aabb.m_boxData.minY()) &&
              (m_boxData.minZ() <= aabb.m_boxData.maxZ() && m_boxData.maxZ() >= aabb.m_boxData.minZ());
        break;
    }
    case kBoundingSphere:
    {
        const BoundingSphere& sphere = *static_cast<const BoundingSphere*>(&other);
        float rad = sphere.radius();

        // Get box closest point to sphere center by clamping        
        Vector3 closestPoint = { std::max(m_boxData.minX(), std::min(sphere.origin().x(), m_boxData.maxX())),
            std::max(m_boxData.minY(), std::min(sphere.origin().y(), m_boxData.maxY())),
            std::max(m_boxData.minZ(), std::min(sphere.origin().z(), m_boxData.maxZ()))
        };

        float distanceSquared = (closestPoint - sphere.origin()).lengthSquared();

        hit = distanceSquared <= (rad * rad);
        break;
    }
    case kPoint:
    {
        const CollidingPoint& point = *static_cast<const CollidingPoint*>(&other);
        hit = (point.point().x() >= m_boxData.minX() && point.point().x() <= m_boxData.maxX()) &&
              (point.point().y() >= m_boxData.minY() && point.point().y() <= m_boxData.maxY()) &&
              (point.point().z() >= m_boxData.minZ() && point.point().z() <= m_boxData.maxZ());
        break;
    }
    case kFrustum:
    {
        const Frustum& frustum = *static_cast<const Frustum*>(&other);
        hit = frustum.intersects(*this);
        break;
    }
    default:
        Logger::Throw("Invalid");
        break;
    }
    return hit;
}

void AABB::recalculateBounds(const TransformInterface& transform, CollidingGeometry & out) const
{
    recalculateBounds(transform.worldMatrix(), out);
}

void AABB::recalculateBounds(const Matrix4x4g & transform, CollidingGeometry & out) const
{
    /// \see https://stackoverflow.com/questions/6053522/how-to-recalculate-axis-aligned-bounding-box-after-translate-rotate
    // A more performant recalculation, see Jim Arvo's Graphics Gems chapter (1990)
    // Split the transform into a translation vector(T) and a 3x3 rotation(M).
    const Vector4& translation = transform.getColumn(3);

    AABB& aabbOut = static_cast<AABB&>(out);
    AABBData& transformedBounds = aabbOut.m_boxData;
    transformedBounds.m_min = translation;
    transformedBounds.m_max = translation;
    Real_t a;
    Real_t b;
    for (uint32_t j = 0; j < 3; j++) {
        for (uint32_t i = 0; i < 3; i++) {
            a = transform(i, j) * m_boxData.m_min[j];
            b = transform(i, j) * m_boxData.m_max[j];
            transformedBounds.m_min[i] += a < b ? a : b;
            transformedBounds.m_max[i] += a < b ? b : a;
        }
    }

    //std::vector<Vector4> outPoints;
    //m_boxData.getPoints(outPoints);
    //for (Vector4& point : outPoints) {
    //    point = transform * point;
    //}
    //AABB& outCast = static_cast<AABB&>(out);
    //outCast.resize(outPoints);
}






// Bounding Sphere

BoundingSphere::BoundingSphere() : CollidingGeometry(kBoundingSphere)
{
}

BoundingSphere::~BoundingSphere()
{
}

bool BoundingSphere::intersects(const CollidingGeometry & other) const
{
    bool hit;
    switch (other.geometryType()) {
    case kAABB:
    {
        const AABB& aabb = *static_cast<const AABB*>(&other);

        // Get box closest point to sphere center by clamping        
        Vector3 closestPoint = { std::max(aabb.boxData().minX(), std::min(m_data.m_origin.x(), aabb.boxData().maxX())),
            std::max(aabb.boxData().minY(), std::min(m_data.m_origin.y(), aabb.boxData().maxY())),
            std::max(aabb.boxData().minZ(), std::min(m_data.m_origin.z(), aabb.boxData().maxZ()))
        };

        float distanceSquared = (closestPoint - m_data.m_origin).lengthSquared();

        hit = distanceSquared <= (m_data.m_radius * m_data.m_radius);
        break;
    }
    case kBoundingSphere:
    {
        const BoundingSphere& sphere = *static_cast<const BoundingSphere*>(&other);
        float distanceSquared = (m_data.m_origin - sphere.m_data.m_origin).lengthSquared();
        float radiusSquared = m_data.m_radius + sphere.m_data.m_radius;
        radiusSquared *= radiusSquared;
        hit = distanceSquared <= radiusSquared;
        break;
    }
    case kPoint:
    {
        const CollidingPoint& point = *static_cast<const CollidingPoint*>(&other);
        float distanceSquared = (m_data.m_origin - point.point()).lengthSquared();
        hit = distanceSquared <= (m_data.m_radius * m_data.m_radius);
        break;
    }    
    case kFrustum:
    {
        const Frustum& frustum = *static_cast<const Frustum*>(&other);
        hit = frustum.intersects(*this);
        break;
    }
    default:
        Logger::Throw("Invalid");
        break;
    }
    return hit;
}

void BoundingSphere::recalculateBounds(const TransformInterface& transform, CollidingGeometry & out) const
{
    BoundingSphere& sphere = dynamic_cast<BoundingSphere&>(out);
    sphere.m_data.m_origin += transform.getPosition().asReal();
    sphere.m_data.m_radius *= std::max(std::max(transform.getScaleVec()[0], transform.getScaleVec()[1]), transform.getScaleVec()[2]);
}


// Colliding Point

CollidingPoint::CollidingPoint() : CollidingGeometry(kPoint)
{
}

CollidingPoint::~CollidingPoint()
{
}

bool CollidingPoint::intersects(const CollidingGeometry & other) const
{
    bool hit;
    switch (other.geometryType()) {
    case kAABB:
    {
        const AABB& aabb = *static_cast<const AABB*>(&other);
        hit = (m_point.x() >= aabb.boxData().minX() && m_point.x() <= aabb.boxData().maxX()) &&
              (m_point.y() >= aabb.boxData().minY() && m_point.y() <= aabb.boxData().maxY()) &&
              (m_point.z() >= aabb.boxData().minZ() && m_point.z() <= aabb.boxData().maxZ());
        break;
    }
    case kBoundingSphere:
    {
        const BoundingSphere& sphere = *static_cast<const BoundingSphere*>(&other);
        float distanceSquared = (m_point - sphere.origin()).lengthSquared();
        hit = distanceSquared <= (sphere.radius() * sphere.radius());
        break;
    }
    case kPoint:
    {
        const CollidingPoint& point = *static_cast<const CollidingPoint*>(&other);
        hit = m_point == point.point();
        break;
    }
    case kFrustum:
    {
        const Frustum& frustum = *static_cast<const Frustum*>(&other);
        hit = frustum.intersects(*this);
        break;
    }
    default:
        Logger::Throw("Invalid");
        break;
    }
    return hit;
}

void CollidingPoint::recalculateBounds(const TransformInterface & transform, CollidingGeometry & out) const
{
    CollidingPoint& point = dynamic_cast<CollidingPoint&>(out);
    Vector3 newPoint = transform.worldMatrix().multPoint(point.point().asReal());
    std::swap(point.point(), newPoint);
}

// Bounding Plane

BoundingPlane::BoundingPlane(): CollidingGeometry(kPlane)
{
}

BoundingPlane::~BoundingPlane()
{
}

bool BoundingPlane::intersects(const CollidingGeometry & other) const
{
    bool hit;
    switch (other.geometryType()) {
    case kAABB:
    {
        hit = classifyGeometry(static_cast<const AABB&>(other)) == kIntersects;
        break;
    }
    case kBoundingSphere:
    {
        hit = classifyGeometry(static_cast<const BoundingSphere&>(other)) == kIntersects;
        break;
    }
    case kPoint:
    {
        hit = classifyGeometry(static_cast<const CollidingPoint&>(other).point()) == kIntersects;
        break;
    }
    case kFrustum:
    {
        const Frustum& frustum = *static_cast<const Frustum*>(&other);
        hit = frustum.intersects(*this);
        break;
    }
    default:
        Logger::Throw("Invalid");
        break;
    }
    return hit;
}

void BoundingPlane::recalculateBounds(const TransformInterface & transform, CollidingGeometry & out) const
{
    /// \see https://community.khronos.org/t/clever-way-to-transform-plane-by-matrix/49570/3
    BoundingPlane& plane = dynamic_cast<BoundingPlane&>(out);
    Vector4 planeEq = Vector4(m_normal);
    planeEq[3] = m_d;
    
    // Transform plane equation
    planeEq = transform.worldMatrix().inversed().transposed() * planeEq;
    plane.m_normal = Vector3(planeEq);
    plane.m_d = planeEq[3];
}

BoundingPlane::Halfspace BoundingPlane::classifyGeometry(const Vector3 & pt) const
{
    float dist = m_normal.dot(pt) + m_d;
    if (dist < 0) {
        return kNegative;
    }
    else if (dist > 0) {
        return kPositive;
    }
    return kIntersects;
}

BoundingPlane::Halfspace BoundingPlane::classifyGeometry(const BoundingSphere & sphere) const
{
    // Assumes normalized plane
    float dist = m_normal.dot(sphere.origin()) + m_d;
    if (dist < -sphere.radius()) {
        return kNegative;
    }
    else if (dist < sphere.radius()) {
        return kIntersects;
    }
    else {
        return kPositive;
    }
}

BoundingPlane::Halfspace BoundingPlane::classifyGeometry(const AABB & aabb) const
{
    Vector3 p = Vector3::EmptyVector();
    Vector3 n = Vector3::EmptyVector();
    findNP(aabb, n, p);

    // Is the most positive vertex position outside?
    if (classifyGeometry(p) == kNegative) {
        return kNegative;
    }

    // Is the most negative vertex outside?
    if (classifyGeometry(n) == kNegative) {
        // Positive vertex is inside, but negative vertex is outside
        return kIntersects;
    }

    return kPositive;
}

float BoundingPlane::distanceToPoint(const Vector3 & point)
{
    if (!isNormalized()) { 
        normalize(); 
    }
    return m_normal.dot(point) + m_d; // m_normal.dot(point) is equivalent to ax + by + cz
}

void BoundingPlane::normalize()
{
    float lengthSquared = m_normal.lengthSquared();
    if (lengthSquared != 1) {
        m_normal.normalize();
        m_d /= std::sqrt(lengthSquared);
    }
}

void BoundingPlane::findNP(const AABB & aabb, Vector3 & n, Vector3 & p) const
{
    // Note: This relies on the assumption that the box is axis-aligned
    // If the box is not axis-aligned, then need to convert normal to box space and use that:
    // nb = (bx . n, by . n, bz . n), where bx, by, and bz are the box axes

    // Find p and n
    p = Vector3(aabb.boxData().minX(), aabb.boxData().minY(), aabb.boxData().minZ());
    n = Vector3(aabb.boxData().maxX(), aabb.boxData().maxY(), aabb.boxData().maxZ());
    if (m_normal.x() >= 0) {
        p[0] = aabb.boxData().maxX();
        n[0] = aabb.boxData().minX();
    }
    if (m_normal.y() >= 0) {
        p[1] = aabb.boxData().maxY();
        n[1] = aabb.boxData().minY();
    }
    if (m_normal.z() >= 0) {
        p[2] = aabb.boxData().maxZ();
        n[2] = aabb.boxData().minZ();
    }
}




} // rev