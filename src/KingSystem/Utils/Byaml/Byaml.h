#pragma once

/// BYML / BYAML utilities which seem to have been imported from Super Mario Odyssey.

#include <basis/seadTypes.h>
#include <math/seadVector.h>
#include <prim/seadEndian.h>
#include <prim/seadSizedEnum.h>
#include "KingSystem/Utils/Types.h"

namespace al {

struct ByamlData;
struct ByamlHashPair;

enum class ByamlType {
    Invalid = 0,

    String = 0xa0,
    Binary = 0xa1,

    Array = 0xc0,
    Hash = 0xc1,
    StringTable = 0xc2,

    Bool = 0xd0,
    Int = 0xd1,
    Float = 0xd2,
    UInt = 0xd3,
    Int64 = 0xd4,
    UInt64 = 0xd5,
    Double = 0xd6,

    Null = 0xff,
};

class ByamlIter {
public:
    ByamlIter();
    explicit ByamlIter(const u8* data);
    ByamlIter(const u8* data, const u8* root_node);
    ByamlIter(const ByamlIter& other);

    ByamlIter& operator=(const ByamlIter& rhs) = default;
    void Set(const ByamlIter& other);

    bool isValid() const;
    bool isTypeHash() const;
    bool isTypeArray() const;
    bool isTypeContainer() const;

    bool isExistKey(const char* key) const;
    s32 getKeyIndex(const char* key) const;

    s32 getSize() const;

    ByamlIter getIterByIndex(s32 index) const;
    ByamlIter getIterByKey(const char* key) const;
    ByamlIter getIterFromData(const ByamlData& data) const;
    bool getByamlDataByIndex(ByamlData* data, s32 index) const;
    bool getByamlDataByKey(ByamlData* data, const char* key) const;
    bool getByamlDataAndKeyNameByIndex(ByamlData* data, const char** key, s32 index) const;

    bool getKeyName(const char** key, s32 index) const;

    bool tryGetIterByIndex(ByamlIter* iter, s32 index) const;
    bool tryGetIterByKey(ByamlIter* iter, const char* key) const;
    bool tryGetIterAndKeyNameByIndex(ByamlIter* iter, const char** key, s32 index) const;

    bool tryGetStringByIndex(const char** value, s32 index) const;
    bool tryGetStringByKey(const char** value, const char* key) const;
    bool tryConvertString(const char** value, const ByamlData* data) const;

    bool tryGetIntByIndex(s32* value, s32 index) const;
    bool tryGetIntByKey(s32* value, const char* key) const;
    bool tryConvertInt(s32* value, const ByamlData* data) const;

    bool tryGetUIntByIndex(u32* value, s32 index) const;
    bool tryGetUIntByKey(u32* value, const char* key) const;
    bool tryConvertUInt(u32* value, const ByamlData* data) const;

    bool tryGetFloatByIndex(f32* value, s32 index) const;
    bool tryGetFloatByKey(f32* value, const char* key) const;
    bool tryConvertFloat(f32* value, const ByamlData* data) const;

    bool tryGetBoolByIndex(bool* value, s32 index) const;
    bool tryGetBoolByKey(bool* value, const char* key) const;
    bool tryConvertBool(bool* value, const ByamlData* data) const;

    bool isEqualData(const ByamlIter& other) const;

private:
    const u8* mData = nullptr;
    const u8* mRootNode = nullptr;
};

struct ByamlHeader {
    u16 getTag() const;
    bool isInvertHeader() const;
    u16 getVersion() const { return version; };
    u32 getHashKeyTableOffset() const { return hash_key_table_offset; };
    u32 getStringTableOffset() const { return string_table_offset; };
    u32 getDataOffset() const { return data_offset; };

    u16 magic;
    u16 version;
    u32 hash_key_table_offset;
    u32 string_table_offset;
    u32 data_offset;
};
KSYS_CHECK_SIZE_NX150(ByamlHeader, 0x10);

struct ByamlContainerHeader {
    ByamlType getType() const {
        // Type is only the first byte
        return ByamlType(*reinterpret_cast<const u8*>(&type_and_size));
    }

    u32 getCount() const { return type_and_size >> 8; };
    u32 type_and_size;
};

bool tryGetByamlS32(s32* value, const ByamlIter& iter, const char* key);
bool tryGetByamlU32(u32* value, const ByamlIter& iter, const char* key);
bool tryGetByamlF32(f32* value, const ByamlIter& iter, const char* key);
bool tryGetByamlV3f(sead::Vector3f* value, const ByamlIter& iter, const char* key);

}  // namespace al