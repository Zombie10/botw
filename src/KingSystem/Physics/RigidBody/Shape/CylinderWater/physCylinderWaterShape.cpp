#include "KingSystem/Physics/RigidBody/Shape/CylinderWater/physCylinderWaterShape.h"
#include <Havok/Physics2012/Collide/Shape/HeightField/hkpHeightFieldShape.h>
#include "KingSystem/Utils/HeapUtil.h"
#include "KingSystem/Utils/SafeDelete.h"

namespace ksys::phys {

class alignas(16) HavokCylinderWaterShape : public hkpHeightFieldShape {
public:
    HK_DECLARE_CLASS_ALLOCATOR(HavokCylinderWaterShape)

    HavokCylinderWaterShape() : hkpHeightFieldShape(hkcdShapeType::HEIGHT_FIELD) {}

    void setRadius(float radius) { m_radius = radius; }

    void getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& aabbOut) const override;

    hkBool castRay(const hkpShapeRayCastInput& input, hkpShapeRayCastOutput& output) const override;

    void castRayWithCollector(const hkpShapeRayCastInput& input, const hkpCdBody& cdBody,
                              hkpRayHitCollector& collector) const override;

    void collideSpheres(const CollideSpheresInput& input,
                        SphereCollisionOutput* outputArray) const override;

    void castSphere(const hkpSphereCastInput& input, const hkpCdBody& cdBody,
                    hkpRayHitCollector& collector) const override {}

    float m_height = 1.0;
    float m_radius = 1.0;
};

CylinderWaterShape* CylinderWaterShape::make(const CylinderShapeParam& param, sead::Heap* heap) {
    void* storage = util::allocStorage<HavokCylinderWaterShape>(heap);
    if (!storage)
        return nullptr;

    auto* hk_shape = new (storage) HavokCylinderWaterShape;
    return new (heap) CylinderWaterShape(param, hk_shape);
}

CylinderWaterShape::CylinderWaterShape(const CylinderShapeParam& param,
                                       HavokCylinderWaterShape* shape)
    : mMaterialMask(param.common.getMaterialMask()), mHavokShape(shape) {
    if (param.common.item_code_disable_stick)
        mMaterialMask.getData().setCustomFlag(MaterialMaskData::CustomFlag::_0);

    setMaterialMask(mMaterialMask);
    mHavokShape->setRadius(param.radius);
    mHavokShape->m_height = param.vertex_a.x;
}

CylinderWaterShape::~CylinderWaterShape() {
    util::deallocateObjectUnsafe(mHavokShape);
}

void CylinderWaterShape::setMaterialMask(const MaterialMask& mask) {
    mMaterialMask = mask;
    if (mHavokShape)
        mHavokShape->setUserData(mask.getRawData());
}

bool CylinderWaterShape::setRadius(float radius) {
    if (!mHavokShape)
        return false;

    if (mHavokShape->m_radius == radius)
        return false;

    mHavokShape->m_radius = radius;
    mFlags.set(Flag::Dirty);
    return true;
}

bool CylinderWaterShape::setHeight(float height) {
    if (mHavokShape->m_height == height)
        return false;

    mHavokShape->m_height = height;
    mFlags.set(Flag::Dirty);
    return true;
}

// NON_MATCHING: useless store to param.vertex_a.x
CylinderWaterShape* CylinderWaterShape::clone(sead::Heap* heap) const {
    CylinderShapeParam param(-sead::Vector3f::ey, sead::Vector3f::ey);
    param.radius = getRadius();
    param.vertex_a.x = getHeight();
    auto* cloned = make(param, heap);
    cloned->setMaterialMask(mMaterialMask);
    return cloned;
}

float CylinderWaterShape::getRadius() const {
    return mHavokShape->m_radius;
}

float CylinderWaterShape::getHeight() const {
    return mHavokShape->m_height;
}

float CylinderWaterShape::getVolume() const {
    return getHeight() * (sead::Mathf::piHalf() * getRadius() * getRadius());
}

hkpShape* CylinderWaterShape::getHavokShape() {
    return mHavokShape;
}

const hkpShape* CylinderWaterShape::getHavokShape() const {
    return mHavokShape;
}

const hkpShape* CylinderWaterShape::updateHavokShape() {
    if (mFlags.isOn(Flag::Dirty)) {
        // Nothing to do.
        mFlags.reset(Flag::Dirty);
    }
    return nullptr;
}

void CylinderWaterShape::setScale(float scale) {
    setRadius(getRadius() * scale);
    setHeight(getHeight() * scale);
}

}  // namespace ksys::phys
