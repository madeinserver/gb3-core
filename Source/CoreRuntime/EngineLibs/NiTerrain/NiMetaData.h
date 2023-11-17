// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef NIMETADATA_H
#define NIMETADATA_H

#include <NiTStringMap.h>
#include <NiFixedString.h>
#include <NiTSet.h>

#include "NiTerrainLibType.h"

namespace efd
{
class TiXmlElement;
}

/**
    The MetaData object is used to store extra data associated with an object.

    @note Currently only supports INTEGER, STRING and FLOAT data-types.
 */
class NITERRAIN_ENTRY NiMetaData : public NiMemObject
{
public:

    /**
        Meta-data data-types.

        These are explicitly defined so the compiler does not change them.
        Remember, these are used during load/save operations.
    */
    enum KeyType
    {
        /// Single integer value.
        INTEGER = 2,

        /// Single floating point value.
        FLOAT = 3,

        /// Arbitrary string value.
        STRING = 4,

        /// Single integer value that will blend with other
        /// meta-data objects
        INTEGER_BLENDED = 5,

        /// Single floating point value that will blend with other
        /// meta-data objects
        FLOAT_BLENDED = 6,

        /// Error type used when a problem occurs when blending. Value
        /// is a string containing a possible error message
        ERROR_TYPE = 7
    };

    /// @cond EMERGENT_INTERNAL
    class MetaDataValue : public NiMemObject
    {
    public:
        /// The type of the data.
        KeyType eType;

        /// The string value, if this is a string type.
        NiFixedString kStringValue;

        /// The weight for the data.
        float fWeight;

        /// Int or float values.
        union
        {
            NiInt32 iValue;
            float fValue;
        };

        /// An operator to implement a "Zero-Element".
        const MetaDataValue& operator=(const int& iParam)
        {
            eType = INTEGER;
            iValue = iParam;
            return *this;
        }
    };
    /// @endcond

    /// @name Constructors
    //@{
    /**
        Copy constructor for storing meta-data in a list/map.

        @param kMetaData A valid meta data which this meta object
            shall use to manage its keys
    */
    NiMetaData(const NiMetaData& kMetaData);

    /// Initialize a new meta-data object.
    NiMetaData();
    //@}

    /// Default Destructor.
    ~NiMetaData();

    /// Assignment operator.
    NiMetaData& operator=(const NiMetaData& kMetaData);

    /**
        Set an int value of meta-data corresponding to the given key.

        If no meta-data previous existed with the given key, it will be
        created.

        @param kKey Key corresponding to the meta-data to be set
        @param iValue Value to give the meta-data.
        @param eType The type of the key as enumerated by the KeyType
            enumeration.
        @param fWeight Weight assigned to the meta-data entry.
        @return True if the value was successfully set.
    */
    bool Set(const NiFixedString& kKey, NiInt32 iValue,
        KeyType eType = INTEGER, float fWeight = 1.0f);

    /**
        Set a float value of meta-data corresponding to the given key.

        If no meta-data previous existed with the given key, it will be
        created.

        @param kKey Key corresponding to the meta-data to be set
        @param fValue Value to give the meta-data.
        @param eType The type of the key as enumerated by the KeyType
            enumeration.
        @param fWeight Weight assigned to the meta-data entry.
        @return True if the value was successfully set.
    */
    bool Set(const NiFixedString& kKey, float fValue,
        KeyType eType = FLOAT, float fWeight = 1.0f);

    /**
        Set a string value of meta-data corresponding to the given key.

        If no meta-data previous existed with the given key, it will be
        created.

        @param kKey Key corresponding to the meta-data to be set
        @param kValue Value to give the meta-data.
        @param eType The type of the key as enumerated by the KeyType
            enumeration.
        @param fWeight Weight assigned to the meta-data entry.
        @return True if the value was successfully set.
    */
    bool Set(const NiFixedString& kKey, const NiFixedString& kValue,
        KeyType eType = STRING, float fWeight = 1.0f);

    /**
        Retrieve int valued meta-data stored within this object,
        specified by the given key.

        This method will fail if no meta-data exists by the given key, or it
        is of the wrong type.

        @param kKey Key corresponding to the meta-data to be set retrieved.
        @param iValue Variable within which to place the value of the
            meta-data.
        @param fWeight Current weight of the meta-data object.  This will be
            meaningless for meta-data passed in to a visitor functor, use the
            fWeight parameter passed to the visitor instead.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, NiInt32& iValue, float& fWeight) const;

    /**
        Retrieve float valued meta-data stored within this object,
        specified by the given key.

        This method will fail if no meta-data exists by the given key, or
        it is of the wrong type.

        @param kKey Key corresponding to the meta-data to be set retrieved.
        @param fValue Variable within which to place the value of the
            meta-data.
        @param fWeight Current weight of the meta-data object.  This will be
            meaningless for meta-data passed in to a visitor functor, use the
            fWeight parameter passed to the visitor instead.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, float& fValue, float& fWeight) const;

    /**
        Retrieve string valued meta-data stored within this object, specified
        by the given key.

        This method will fail if no meta-data exists by the
        given key, or it is of the wrong type.

        @param kKey Key corresponding to the meta-data to be set retrieved.
        @param kValue Variable within which to place the value of the
            meta-data.
        @param fWeight Current weight of the meta-data object.  This will be
            meaningless for meta-data passed in to a visitor functor, use the
            fWeight parameter passed to the visitor instead.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, NiFixedString& kValue,
        float& fWeight) const;

    /**
        Retrieve int valued meta-data stored within this object,
        specified by the given key.

        This method will fail if no meta-data exists by the given key, or it
        is of the wrong type.

        @param kKey Key corresponding to the meta-data to be set retrieved.
        @param iValue Variable within which to place the value of the
            meta-data.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, NiInt32& iValue) const;

    /**
        Retrieve float valued meta-data stored within this object,
        specified by the given key.

        This method will fail if no meta-data exists by the given key, or
        it is of the wrong type.

        @param kKey Key corresponding to the meta-data to be set retrieved.
        @param fValue Variable within which to place the value of the
            meta-data.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, float& fValue) const;

    /**
        Retrieve string valued meta-data stored within this object, specified
        by the given key.

        This method will fail if no meta-data exists by the
        given key, or it is of the wrong type.

        @param kKey Key corresponding to the meta-data to be set retrieved.
        @param kValue Variable within which to place the value of the
            meta-data.
        @return true if the value was successfully retrieved.
    */
    bool Get(const NiFixedString& kKey, NiFixedString& kValue) const;

    /// @name Key Removal
    //@{

    /**
        Remove the value stored for a key.

        @param kKey the key to remove.
        @return true if the value was found and removed, false otherwise.
    */
    inline bool RemoveKey(const NiFixedString &kKey);

    /// Remove all keys stored by this meta-data object.
    inline void RemoveAllKeys();

    /**
        Function that checks if a given key is present in the meta-data.
        It returns true if a key was found and false otherwise.
    */
    inline bool HasKey(const NiFixedString& kMetaDataKey);
    //@}

    /// @name Key Information
    //@{

    /**
        Retrieve the expected type for a particular key.

        @note This is a delegate function to the MetaDataStore object
            referenced by this MetaData object.

        @param kKey the key to retrieve the type for
        @param kKeyType the variable to store the type inside.
        @return true if the type was successfully retrieved.
    */
    bool GetKeyType(const NiFixedString& kKey, KeyType& kKeyType) const;
    //@}

    /// @name Iteration
    //@{

    /**
        Populate an ObjectSet with the sting names of all the keys stored
        in this meta-data object.

        This function is useful for iteration of all the data stored in this
        object.

        @param kKeyNames The list with which to insert the current keys.
    */
    void GetKeys(NiTObjectSet<NiFixedString>& kKeyNames) const;
    
    /// Set the weights of all the meta-data values.
    ///
    /// @param fWeight New weight.
    void UpdateWeights(float fWeight) const;

    /**
        Returns the weight of the meta-data.  This will be meaningless for
        meta-data passed in to a visitor functor, use the fWeight parameter
        passed to the visitor instead.
    */
    float GetWeight() const;

    /**
        Blend this meta-data object with another meta-data object.

        Any keys that do not already exist will be added.
        Any keys that already exist will be blended according to
        their type. INTEGER_BLENDED and FLOAT_BLENDED will be
        blended together using the weighting provided. For all
        other types the value will remain the value of the
        meta-data with the highest weight.

        @param pkMetaData A pointer to the meta-data to blend with.
        @param fWeight The weighting of this meta-data's values.
    */
    void Blend(const NiMetaData* pkMetaData, float fWeight);

    /// Save the data in this MetaData object into a DOM format.
    void Save(efd::TiXmlElement* pkRootElement) const;

    /// Load the date stored in the NiDOMTool into this MetaData object.
    ///
    /// Any existing entries in this object shall be removed.
    void Load(const efd::TiXmlElement* pkRootElement);
    //@}

private:

    /// A StringMap mapping keys to their values
    NiTStringPointerMap<MetaDataValue*>* m_pkValueMap;
};

#include "NiMetaData.inl"

#endif // NIMETADATA_H
