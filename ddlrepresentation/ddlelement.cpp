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

#include "ddlelement.h"
#include <ddl.h>


namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-4, ERR_POINTER)
    _MAKE_RESULT(-37, ERR_NOT_INITIALIZED)

    DDLElement::DDLElement(IDDLDataType* poType,
                             const std::string& name,
                             unsigned int const uiBytepos,
                             unsigned int const uiArraysize,
                             DDLByteorder::Byteorder const eByteorder,
                             DDLAlignment::AlignmentType const eAlignment,
                             IDDLUnit* poUnit,
                             unsigned int const uiBitpos,
                             unsigned int const uiNumBits,
                             const std::string& strDescription,
                             const std::string& strComment,
                             const std::string& strArraySizeSource,
                             const std::string& strValue,
                             bool const bMinValid,
                             const std::string& strMinValue,
                             bool const bMaxValid,
                             const std::string& strMaxValue,
                             bool const bDefaultValid,
                             const std::string& strDefaultValue,
                             bool const bScaleValid,
                             const std::string& strScaleValue,
                             bool const bOffsetValid,
                             const std::string& strOffsetValue) :
        _type(poType),
        _name(name),
        _bytes_pos(uiBytepos),
        _bit_pos(uiBitpos),
        _num_bits(uiNumBits),
        _description(strDescription),
        _array_size(uiArraysize),
        _array_size_source(strArraySizeSource),
        _constant_value(strValue),
        _unit(poUnit),
        _byteorder(eByteorder),
        _alignment(eAlignment),
        _comment(strComment),
        _init_flag(NULL != _type),
        _min_valid(bMinValid),
        _min_value(strMinValue),
        _max_valid(bMaxValid),
        _max_value(strMaxValue),
        _default_valid(bDefaultValid),
        _default_value(strDefaultValue),
        _scale_valid(bScaleValid),
        _scale_value(strScaleValue),
        _offset_valid(bOffsetValid),
        _offset_value(strOffsetValue)
    {
    }

    a_util::result::Result DDLElement::accept(IDDLVisitor *poVisitor) const
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    a_util::result::Result DDLElement::accept(IDDLChangeVisitor *poVisitor)
    {
        if (!_init_flag)
        {
            return ERR_NOT_INITIALIZED;
        }
        return poVisitor->visit(this);
    }

    a_util::result::Result DDLElement::create(IDDLDataType* poType,
                                const std::string& name,
                                unsigned int const uiBytepos,
                                unsigned int const uiArraysize,
                                DDLByteorder::Byteorder const eByteorder,
                                DDLAlignment::AlignmentType const eAlignment,
                                IDDLUnit* poUnit,
                                unsigned int const uiBitpos,
                                unsigned int const uiNumBits,
                                const std::string& strDescription,
                                const std::string& strComment,
                                const std::string& strArraySizeSource,
                                const std::string& strValue,
                                bool const bMinValid,
                                const std::string& strMinValue,
                                bool const bMaxValid,
                                const std::string& strMaxValue,
                                bool const bDefaultValid,
                                const std::string& strDefaultValue,
                                bool const bScaleValid,
                                const std::string& strScaleValue,
                                bool const bOffsetValid,
                                const std::string& strOffsetValue)
    {
        if (!poType) { return ERR_POINTER; }
        _type = poType;
        _name = name;
        _bytes_pos = uiBytepos;
        _array_size = uiArraysize;
        _byteorder = eByteorder;
        _alignment = eAlignment;
        _unit = poUnit;
        _bit_pos = uiBitpos;
        _num_bits = uiNumBits;
        _description = strDescription;
        _comment = strComment;
        _array_size_source = strArraySizeSource;
        _init_flag = true;
        _constant_value = strValue;
        _min_valid = bMinValid;
        _min_value = strMinValue;
        _max_valid = bMaxValid;
        _max_value = strMaxValue;
        _default_valid = bDefaultValid;
        _default_value = strDefaultValue;
        _scale_valid = bScaleValid;
        _scale_value = strScaleValue;
        _offset_valid = bOffsetValid;
        _offset_value = strOffsetValue;
        return a_util::result::SUCCESS;
    }

    bool DDLElement::isPredefined() const
    {
        return false;
    }

    int DDLElement::getCreationLevel() const
    {
        return 99;  // cMediaManager::DL_Internal => error
    }

    const std::string& DDLElement::getType() const
    {
        if (!_init_flag)
        {
            return a_util::strings::empty_string;
        }
        return _type->getName();
    }

    void DDLElement::setType(IDDLDataType* const pType)
    {
        _type = pType;
        _init_flag = (_type != NULL);
    }


    IDDLDataType * DDLElement::getTypeObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _type;
    }

    const std::string& DDLElement::getName() const
    {
        return _name;
    }

    void DDLElement::setName(const std::string& name)
    {
        _name = name;
    }

    unsigned int DDLElement::getBytepos() const
    {
        return _bytes_pos;
    }

    void DDLElement::setBytepos(unsigned int const uiBytePos)
    {
        _bytes_pos = uiBytePos;
    }

    unsigned int DDLElement::getArraysize() const
    {
        return _array_size;
    }

    void  DDLElement::setArraysize(unsigned int const uiArraySize )
    {
        _array_size = uiArraySize;
    }


    DDLByteorder::Byteorder DDLElement::getByteorder() const
    {
        return _byteorder;
    }

    void DDLElement::setByteorder(DDLByteorder::Byteorder const eByteOrder)
    {
        _byteorder = eByteOrder;
    }

    DDLAlignment::AlignmentType DDLElement::getAlignment() const
    {
        return _alignment;
    }

    void DDLElement::setAlignment(DDLAlignment::AlignmentType const eAlign)
    {
        _alignment = eAlign;
    }

    void DDLElement::setBitpos(unsigned int const uiBitpos)
    {
        if (uiBitpos >= 0 && uiBitpos <= 7)
        {
            _bit_pos = uiBitpos;
        }
    }

    unsigned int DDLElement::getBitpos() const
    {
        return _bit_pos;
    }

    void DDLElement::setNumBits(unsigned int const uiNumBits)
    {
        _num_bits = uiNumBits;
    }

    unsigned int DDLElement::getNumBits() const
    {
        return _num_bits;
    }

    void DDLElement::setDescription(const std::string& strDescription)
    {
        _description = strDescription;
    }

    const std::string& DDLElement::getDescription() const
    {
        return _description;
    }

    void DDLElement::setUnit(IDDLUnit* poUnit)
    {
        _unit = poUnit;
    }

    const std::string& DDLElement::getUnit() const
    {
        if (NULL == _unit)
        {
            // no unit reference defined
            return a_util::strings::empty_string;
        }
        return _unit->getName();
    }

    IDDLUnit * DDLElement::getUnitObject() const
    {
        if (!_init_flag)
        {
            return NULL;
        }
        return _unit;
    }

    void DDLElement::setComment(const std::string& strComment)
    {
        _comment = strComment;
    }

    const std::string& DDLElement::getComment() const
    {
        return _comment;
    }

    bool DDLElement::isInitialized() const
    {
        return _init_flag;
    }

    const std::string& DDLElement::getArraySizeSource() const
    {
        return _array_size_source;
    }

    void DDLElement::setArraySizeSource( const std::string& strArraySizeSource )
    {
        _array_size_source = strArraySizeSource;
    }

    const std::string& DDLElement::getConstantValue() const
    {
        return _constant_value;
    }

    void DDLElement::setConstantValue( const std::string& strConstantValue )
    {
        _constant_value = strConstantValue;
    }

    bool DDLElement::isDynamic() const
    {
        if (!getArraySizeSource().empty())
        {
            return true;
        }

        return false;
    }

    void DDLElement::setMinValidity (bool const bMinValid)
    {
        _min_valid = bMinValid;
    }

    bool DDLElement::isMinValid () const
    {
        return _min_valid;
    }

    const std::string & DDLElement::getMinValue () const
    {
        if (_min_valid)
        {
            return _min_value;
        }
        else if (_init_flag)
        {
            DDLDataType *poDT = dynamic_cast<DDLDataType*>(_type);
            if (NULL != poDT && poDT->isMinValid())
            {
                // inherit minimum value from primitive data type
                return poDT->getMinValue();
            }
        }
        // no type information available or complex type or no min value valid
        return a_util::strings::empty_string;
    }

    a_util::result::Result DDLElement::setMinValue (const std::string& strMinVal)
    {
        // TODO validate new minimum value
        _min_value = strMinVal;
        return a_util::result::SUCCESS;
    }

    void DDLElement::setMaxValidity (bool const bMaxValid)
    {
        _max_valid = bMaxValid;
    }

    bool DDLElement::isMaxValid () const
    {
        return _max_valid;
    }

    const std::string & DDLElement::getMaxValue () const
    {
        if (_max_valid)
        {
            return _max_value;
        }
        else if (_init_flag)
        {
            DDLDataType *poDT = dynamic_cast<DDLDataType*>(_type);
            if (NULL != poDT && poDT->isMaxValid())
            {
                // inherit maximum value from primitive type
                return poDT->getMaxValue();
            }
        }
        // no type information available or complex type or no min value valid
        return a_util::strings::empty_string;
    }

    a_util::result::Result DDLElement::setMaxValue (const std::string& strMaxVal)
    {
        // TODO validate new maximum value
        _max_value = strMaxVal;
        return a_util::result::SUCCESS;
    }

    void DDLElement::setDefaultValidity (bool const bDefaultValid)
    {
        _default_valid = bDefaultValid;
    }

    bool DDLElement::isDefaultValid () const
    {
        return _default_valid;
    }

    const std::string & DDLElement::getDefaultValue () const
    {
        return _default_value;
    }

    a_util::result::Result DDLElement::setDefaultValue (const std::string& strDefaultVal)
    {
        // TODO validate new default value
        _default_value = strDefaultVal;
        return a_util::result::SUCCESS;
    }

    void DDLElement::setScaleValidity (bool const bScaleValid)
    {
        _scale_valid = bScaleValid;
    }

    bool DDLElement::isScaleValid () const
    {
        return _scale_valid;
    }

    const std::string & DDLElement::getScaleValue () const
    {
        return _scale_value;
    }

    a_util::result::Result DDLElement::setScaleValue (const std::string& strScaleValue)
    {
        // TODO validate new scaling value
        _scale_value = strScaleValue;
        return a_util::result::SUCCESS;
    }

    void DDLElement::setOffsetValidity (bool const bOffsetValid)
    {
        _offset_valid = bOffsetValid;
    }

    bool DDLElement::isOffsetValid () const
    {
        return _offset_valid;
    }

    const std::string & DDLElement::getOffsetValue () const
    {
        return _offset_value;
    }

    a_util::result::Result DDLElement::setOffsetValue (const std::string& strOffsetValue)
    {
        // TODO validate new offset value
        _offset_value = strOffsetValue;
        return a_util::result::SUCCESS;
    }
}   // namespace ddl
