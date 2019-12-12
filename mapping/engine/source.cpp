/**
 * @file
 *
 * @copyright
 * @verbatim
   Copyright @ 2017 Audi Electronics Venture GmbH. All rights reserved.

       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.

   You may add additional accurate notices of copyright ownership.
   @endverbatim
*/

#include "source.h"

#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"

#include "codec/access_element.h"

#include "mapping/configuration/map_configuration.h"

#include "data_trigger.h"
#include "signal_trigger.h"

namespace mapping
{
namespace rt
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-42, ERR_INVALID_TYPE)
}
}

using namespace mapping;
using namespace mapping::rt;

Source::Source(IMappingEnvironment& oEnv) : _env(oEnv), _handle(0)
{
    _type_map["tUInt8"] =   e_uint8;
    _type_map["tUInt16"] =  e_uint16;
    _type_map["tUInt32"] =  e_uint32;
    _type_map["tUInt64"] =  e_uint64;
    _type_map["tInt8"] =    e_int8;
    _type_map["tInt16"] =   e_int16;
    _type_map["tInt32"] =   e_int32;
    _type_map["tInt64"] =   e_int64;
    _type_map["tFloat32"] = e_float32;
    _type_map["tFloat64"] = e_float64;
    _type_map["tBool"] =    e_bool;
    _type_map["tChar"] =    e_char;
}

Source::~Source()
{
    if (_handle)
    {
        _env.unregisterSource(_handle);
    }
}

a_util::result::Result Source::create(const oo::MapSource& oMapSource, const std::string& strTypeDescription)
{
    _name = oMapSource.getName();
    _type = oMapSource.getType();
    _type_description = strTypeDescription;

    _codec_factory.reset(new ddl::CodecFactory(_type.c_str(), _type_description.c_str()));
    if (isOk(_codec_factory->isValid()))
    {
        return _env.registerSource(_name.c_str(), _type.c_str(), this, _handle);
    }

    return ERR_INVALID_TYPE;
}

a_util::result::Result Source::addTrigger(const oo::MapConfiguration& oMapConfig, TriggerBase* oTrigger)
{
    Triggers::iterator it = _triggers.begin();
    for(; it != _triggers.end(); ++it)
    {
        if (it->first == oTrigger) break;
    }

    if(it == _triggers.end())
    {
        AssignmentStruct oStruct;
        DataTrigger* pDataTrigger = dynamic_cast<DataTrigger*>(oTrigger);

        if(pDataTrigger)
        {
            // Get structure from DDL
            const ddl::DDLDescription* pDescription = oMapConfig.getDescription();
            if (!pDescription) { return ERR_POINTER; }
            const ddl::DDLComplex* pStruct = pDescription->getStructByName(_type);
            if (!pStruct) { return ERR_POINTER; }

            const ddl::IDDLDataType* pDataType = NULL;
            // Get Element from DDL Struct
            const ddl::DDLElement* pElem = NULL;
            bool bIsArrayElement = false;
            RETURN_IF_FAILED(DDLHelper::LookupElement(*pStruct, pDataTrigger->getVariable(),
                pElem, bIsArrayElement));   

            // Get ID in DDL
            ddl::CodecFactory oFac(pStruct);
            ddl::StaticDecoder oDecoder = _codec_factory->makeStaticDecoderFor(NULL, oFac.getStaticBufferSize());
            size_t nIdx = 0;
            RETURN_IF_FAILED(ddl::access_element::find_index(oDecoder, pDataTrigger->getVariable(), nIdx));

            // Get element pointer offset
            oStruct.element_ptr_offset = (uintptr_t)oDecoder.getElementAddress(nIdx);

            pDataType = pElem->getTypeObject();
            // Get Type from TypeMap
            TypeMap::const_iterator it = _type_map.find(pDataType->getName());
            if(it != _type_map.end())
            {
                oStruct.type32 = it->second;
            }

            // find element size
            const ddl::DDLDataType* pSourceType = dynamic_cast<const ddl::DDLDataType*>(pDataType);
            if(pSourceType)
            {
                oStruct.buffer_size = pSourceType->getNumBits()/8;
            } 
            else 
            {
                return ERR_INVALID_TYPE;
            }
        }

        _triggers.push_back(std::make_pair(oTrigger, oStruct));
    }

    return a_util::result::SUCCESS;
}

const std::string& Source::getType() const
{
    return _type;
}

const Source::Assignments& Source::getAssigmentList() const
{
    return _assignments;
}

a_util::result::Result Source::addAssignment(const oo::MapConfiguration& oMapConfig, 
    const std::string& strSourceElement, TargetElement* pTargetElement)
{
    if(strSourceElement == "received()")
    {
        _received_elements.push_back(pTargetElement);
    }
    else
    {
        AssignmentStruct oStruct;

        // Get structure from DDL
        const ddl::DDLDescription* pDescription = oMapConfig.getDescription();
        if (!pDescription) { return ERR_POINTER; }
        const ddl::DDLComplex* pStruct = pDescription->getStructByName(_type);
        if (!pStruct) { return ERR_POINTER; }

        unsigned int szArraySize = 1;

        // If strSourceElement is empty, the whole Structure will be copied
        if(strSourceElement.empty())
        {
            oStruct.buffer_size = _codec_factory->getStaticBufferSize();
            oStruct.element_ptr_offset = 0;
        }
        else
        {
            // Get Element from DDL Struct
            const ddl::DDLElement* pElem = NULL;
            bool bIsArrayElement = false;
            
            RETURN_IF_FAILED(DDLHelper::LookupElement(*pStruct, strSourceElement,
                pElem, bIsArrayElement));

            const ddl::IDDLDataType* pDataType = pElem->getTypeObject();

            // Get ID in DDL Coder
            if(!bIsArrayElement)
            {
                szArraySize = pElem->getArraysize();
            }   

            // Get Type from TypeMap
            TypeMap::const_iterator it = _type_map.find(pDataType->getName());
            if(it != _type_map.end())
            {
                oStruct.type32 = it->second;
            }

            // find element size
            const ddl::DDLDataType* pSourceType = dynamic_cast<const ddl::DDLDataType*>(pDataType);
            const ddl::DDLComplex* pSourceStruct = dynamic_cast<const ddl::DDLComplex*>(pDataType);
            const ddl::DDLEnum* pSourceEnum = dynamic_cast<const ddl::DDLEnum*>(pDataType);

            if(pSourceType)
            {
                oStruct.buffer_size = pSourceType->getNumBits() * szArraySize/8;
            } 
            else if(pSourceStruct)
            {
                // TODO: check if this struct can be a different one than the one in _codec_factory
                ddl::CodecFactory oFactory(pSourceStruct->getName().c_str(),
                    _type_description.c_str());

                oStruct.buffer_size = oFactory.getStaticBufferSize();
            } 
            else if(pSourceEnum)
            {
                const ddl::DDLDataType* pEnumType = dynamic_cast<const ddl::DDLDataType*>(pSourceEnum->getTypeObject());
                oStruct.buffer_size = pEnumType->getNumBits() * szArraySize/8;

                // we need to set the interpretation type to the underlying type here
                it = _type_map.find(pEnumType->getName());
                if(it != _type_map.end())
                {
                    oStruct.type32 = it->second;
                }
            }
            else 
            {
                return ERR_INVALID_TYPE;
            }

            std::string strPath = strSourceElement;
            if (szArraySize > 1)
            {
                strPath.append("[0]");
            }

            ddl::CodecFactory oFac(pStruct);
            ddl::StaticDecoder oDecoder = _codec_factory->makeStaticDecoderFor(NULL, oFac.getStaticBufferSize());
            size_t nIdx = 0;
            RETURN_IF_FAILED(ddl::access_element::find_index(oDecoder, strPath, nIdx));

            // Get element pointer offset
            oStruct.element_ptr_offset = (uintptr_t)oDecoder.getElementAddress(nIdx);
        }
    
        Assignments::iterator itAssigns = _assignments.end();
        for (itAssigns = _assignments.begin(); itAssigns != _assignments.end(); ++itAssigns)
        {
            if (itAssigns->first == oStruct)
            {
                break;
            }
        }

        if (itAssigns == _assignments.end())
        {
            _assignments.push_back(std::make_pair(oStruct, TargetElementList()));
            _assignments.back().second.push_back(pTargetElement);
        }
        else
        {
            itAssigns->second.push_back(pTargetElement);
        }
    }

    _targets.insert(pTargetElement->getTarget());

    return a_util::result::SUCCESS;
}

a_util::result::Result Source::removeAssignmentsFor(const Target* pTarget)
{
    for (Assignments::iterator itAssignments = _assignments.begin();
        itAssignments != _assignments.end();)
    {
        TargetElementList& oAssignedElements = itAssignments->second;
        for (TargetElementList::iterator itElements = oAssignedElements.begin();
            itElements != oAssignedElements.end();)
        {
            if ((*itElements)->getTarget() == pTarget)
            {
                // element is owned and deleted by target
                itElements = oAssignedElements.erase(itElements);
            }
            else
            {
                ++itElements;
            }
        }

        if (itAssignments->second.empty())
        {
            itAssignments = _assignments.erase(itAssignments);
        }
        else
        {
            ++itAssignments;
        }
    }

    _targets.erase(pTarget);

    return a_util::result::SUCCESS;
}

a_util::result::Result Source::onSampleReceived(const void* pData, size_t szSize)
{
    if (!pData) { return ERR_POINTER; }

    // lock all currently mapped target buffers
    // note: theres some optimization potential here:
    //   -- rework the assignment container to be able to sort the assignments by target
    //   -- lock only the current target during iteration over all assignments
    // for now it is fast enough since the number of mapped targets isn't to high
    for (TargetRefList::const_iterator it = _targets.begin();
        it != _targets.end(); ++it)
    {
        (*it)->aquireWriteLock();
    }

    // write true into all received(<this_signal>) assignments
    bool bValue = true;
    for(TargetElementList::iterator it = _received_elements.begin(); it != _received_elements.end(); it++)
    {
        (*it)->setValue(&bValue, e_bool, sizeof(bValue));
    }

    // write all assignments that stem from this source
    for (Assignments::iterator itAssign = _assignments.begin();
        itAssign != _assignments.end(); ++itAssign)
    {
        void* pValue = (void*)((uintptr_t)pData + itAssign->first.element_ptr_offset);

        const std::vector<TargetElement*>& vecElements = itAssign->second;
        uint32_t type32 = itAssign->first.type32;
        size_t buffer_size = itAssign->first.buffer_size;

        for (size_t idx = 0; idx < vecElements.size(); ++idx)
        {
            vecElements[idx]->setValue(pValue, type32, buffer_size);
        }
    }

    // unlock all currently mapped target buffers
    for (TargetRefList::const_iterator it = _targets.begin();
        it != _targets.end(); ++it)
    {
        (*it)->releaseWriteLock();
    }

    // call signal triggers
    for(Triggers::iterator it = _triggers.begin();
        it != _triggers.end(); it++)
    {
        SignalTrigger* pSigTrigger = dynamic_cast<SignalTrigger*>(it->first);
        DataTrigger* pDataTrigger = dynamic_cast<DataTrigger*>(it->first);
        if(pSigTrigger)
        {
            pSigTrigger->transmit();
        }
        else if(pDataTrigger)
        {
            // read the current value and cast it into a float64
            double f64Val = 0;
            void* pValue = (void*)((uintptr_t)pData + it->second.element_ptr_offset);
            switch (it->second.type32)
            {
            case e_uint8:
                f64Val = *(static_cast<uint8_t*>(pValue));
                break;
            case e_uint16:
                f64Val = *(static_cast<uint16_t*>(pValue));
                break;
            case e_uint32:
                f64Val = *(static_cast<uint32_t*>(pValue));
                break;
            case e_uint64:
                f64Val = static_cast<double>(*(static_cast<uint64_t*>(pValue)));
                break;
            case e_int8:
                f64Val = *(static_cast<int8_t*>(pValue));
                break;
            case e_int16:
                f64Val = *(static_cast<int16_t*>(pValue));
                break;
            case e_int32:
                f64Val = *(static_cast<int32_t*>(pValue));
                break;
            case e_int64:
                f64Val = static_cast<double>(*(static_cast<int64_t*>(pValue)));
                break;
            case e_float32:
                f64Val = *(static_cast<float*>(pValue));
                break;
            case e_float64:
                f64Val = *(static_cast<double*>(pValue));
                break;
            case e_bool:
                f64Val = *(static_cast<bool*>(pValue));
                break;
            case e_char:
                f64Val = *(static_cast<char*>(pValue));
                break;
            default:
                assert(false);
                break;
            }

            // compare/interpret the operator comparison
            if(pDataTrigger->compare(f64Val))
            {
                pDataTrigger->transmit();
            }
        }
    }

    return a_util::result::SUCCESS;
}
