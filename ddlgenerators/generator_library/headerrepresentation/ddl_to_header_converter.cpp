/**
 * @file
 * Description is missing.
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


#include <ddl.h>
#include "ddl_to_header_converter.h"
#include "header_visitor_intf.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
        _MAKE_RESULT(-4, ERR_POINTER);
        _MAKE_RESULT(-5, ERR_INVALID_ARG);
        _MAKE_RESULT(-11, ERR_INVALID_FILE);
        _MAKE_RESULT(-19, ERR_NOT_SUPPORTED);
        _MAKE_RESULT(-20, ERR_NOT_FOUND);
        _MAKE_RESULT(-24, ERR_PATH_NOT_FOUND);
        _MAKE_RESULT(-38, ERR_FAILED);
    }
}

using namespace ddl;

namespace ddl
{

    using namespace ddl_generator::oo;
    DDLToHeaderConverter::DDLToHeaderConverter()
    {
        _header = NULL;
        _known_types = NULL;
    }

    DDLToHeaderConverter::~DDLToHeaderConverter()
    { }

    a_util::result::Result DDLToHeaderConverter::setKnownTypes(const HeaderTypesVec* types)
    {
        _known_types = types;
        return ERR_NOERROR;
    }

    const std::string& DDLToHeaderConverter::getError()
    {
        return _error_string;
    }

    a_util::result::Result DDLToHeaderConverter::visitDDL(const DDLDescription *description)
    {
        // visit EVERYTHING and let the individual visit() method decide what to do.
        DDLBaseunitVec base_units = description->getBaseunits();
        for (DDLBaseunitIt iter = base_units.begin();
            iter != base_units.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        DDLDTVec data_types = description->getDatatypes();
        for (DDLDTIt iter = data_types.begin();
            iter != data_types.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        DDLEnumVec enums = description->getEnums();
        for (DDLEnumIt iter = enums.begin();
            iter != enums.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        DDLPrefixVec prefixes = description->getPrefixes();
        description->getHeader()->accept(this);
        for (DDLPrefixIt iter = prefixes.begin();
            iter != prefixes.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        DDLStreamVec streams = description->getStreams();
        for (DDLStreamIt iter = streams.begin();
            iter != streams.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        DDLComplexVec structs = description->getStructs();
        for (DDLComplexIt iter = structs.begin();
            iter != structs.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        DDLUnitVec units = description->getUnits();
        for (DDLUnitIt iter = units.begin();
            iter != units.end();
            iter++)
        {
            if (isFailed(((*iter)->accept(this))))
            {
                return ERR_FAILED;
            }
        }
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLHeader *header)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLDataType *data_type)
    {
        // Make sure, that the header type exists
        if (findTypeByName(data_type->getName()) == NULL)
        {
            _error_string.append("Types that are not part of ADTF or stdint.h are not supported (");
            _error_string.append(data_type->getName());
            _error_string.append(").\n");
            return (ERR_NOT_FOUND);
        }
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLComplex *ddl_struct)
    {
        if (findTypeByName(ddl_struct->getName()) == NULL)
        {
            // Add the type, so if its referenced by one of its member, there will be no infinite loop.
            HeaderStruct* header_struct = new HeaderStruct(ddl_struct->getName());
            header_struct->setName(cleanUpName(ddl_struct->getName()));
            header_struct->setComment(ddl_struct->getComment());
            _structs.push_back(header_struct);
            // parse all elements of the struct
            DDLElementVec ddl_elements = ddl_struct->getElements();
            for (DDLElementIt iter = ddl_elements.begin();
                iter != ddl_elements.end();
                iter++)
            {
                (*iter)->accept(this);
                HeaderStructElement* header_element = new HeaderStructElement();
                if ((*iter)->isDynamic())
                {
                    // Not supported ATM
                    _error_string.append("Dynamic arrays are not supported.\n");
                    return ERR_NOT_SUPPORTED;
                }
                else
                {
                    header_element->setArraySize((*iter)->getArraysize());
                }
                header_element->setIsConst(false);
                header_element->setIsPointer(false);
                header_element->setIsStatic(false);
                header_element->setName(cleanUpName((*iter)->getName()));
                header_element->setType(findTypeByName(cleanUpName((*iter)->getTypeObject()->getName())));
                header_element->setDescription((*iter)->getDescription());
                header_element->setComment((*iter)->getComment());
                header_struct->addElement(header_element);
                // Alignment
                // Only two cases are supported at the moment:
                // either the alignment of the element is the same as the default alignment of its type
                // or the alignment of the element is smaller than the default alignment of its type and
                // is the same as the parent structs alignment.
                if (header_element->getType()->getPacking() == fromAlignment((*iter)->getAlignment()) ||
                    (header_element->getType()->getPacking() > fromAlignment((*iter)->getAlignment()) &&
                    (*iter)->getAlignment() == ddl_struct->getAlignment()))
                {
                    // It's Okay, nothing to do here
                }
                else
                {
                    _error_string.append("This alignment cannot be mapped to c-structs (");
                    _error_string.append(ddl_struct->getName());
                    _error_string.append(".");
                    _error_string.append((*iter)->getName());
                    _error_string.append(").\n");
                    _structs.pop_back();
                    delete header_struct;
                    return ERR_NOT_SUPPORTED;
                }
            }
            // Setting packing after adding elements, so packing wont be recalculated inside the object
            header_struct->setPacking(fromAlignment(ddl_struct->getAlignment()));
            // Move this struct to the end, so that all needed types are defined front up.
            _structs.erase(std::find(_structs.begin(), _structs.end(), header_struct));
            _structs.push_back(header_struct);
        }
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLStream *stream)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLUnit *unit)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLBaseunit *base_unit)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLExtDeclaration *ext_declaration)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLElement *element)
    {
        return (element->getTypeObject()->accept(this));
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLPrefix *prefix)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLRefUnit *ref_unit)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLStreamStruct *stream_struct)
    {
        return ERR_NOERROR;
    }


    a_util::result::Result DDLToHeaderConverter::visit(const DDLEnum *ddl_enum)
    {
        // Check the type
        if (isFailed((ddl_enum->getTypeObject()->accept(this))))
        {
            return ERR_FAILED;
        }

        bool is_integer_enum = isEnumIntegral(ddl_enum);

        if (is_integer_enum)
        { 
            if (findTypeByName(ddl_enum->getName()) == NULL)
            {
                HeaderEnum* new_enum = new HeaderEnum(cleanUpName(ddl_enum->getName()));

                for (auto value_iter = ddl_enum->getValues().begin(); value_iter != ddl_enum->getValues().end(); value_iter++)
                {
                    const std::string& name = value_iter->first;
                    const std::string& value = value_iter->second;
                    
                    new_enum->addValue(a_util::strings::toInt32(value), name);
                }

                _enums.push_back(new_enum);
            }
            return ERR_NOERROR;
        }
        else
        {
            // Non integer enums not possible in header. In that case we add the type as a typedef
            // and define the values as constants of that type

            // Add a typedef if not existing
            if (findTypeByName(ddl_enum->getName()) == NULL)
            {
                _typedefs.push_back(new HeaderTypedef(cleanUpName(ddl_enum->getName()),
                    findTypeByName(ddl_enum->getType())));
            }
            const HeaderType* enum_type = findTypeByName(ddl_enum->getName());

            for (auto value_iter = ddl_enum->getValues().begin(); value_iter != ddl_enum->getValues().end(); value_iter++)
            {
                const std::string& name = value_iter->first;
                const std::string& value = value_iter->second;
                HeaderConstant* constant = new HeaderConstant();
                constant->setName(name);
                constant->setType(enum_type);
                constant->reset(value.c_str());
                _constants.push_back(constant);
            }
            return ERR_NOERROR;
        }
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLProperty* ddl_property)
    {
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::visit(const DDLStreamMetaType* stream_meta_type)
    {
        return ERR_NOERROR;
    }

    Header* DDLToHeaderConverter::getHeader()
    {
        return _header;
    }

    a_util::result::Result DDLToHeaderConverter::createNew()
    {
        // This instance has no responsibility for the object.
        _header = new Header();
        _error_string = "";
        BuildTypedefs();
        BuildConstants();
        buildStructs();
        buildEnums();
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::DestroyHeader()
    {
        if (_header != NULL)
        {
            delete _header;
        }

        for (HeaderTypedefs::iterator iter = _typedefs.begin();
            iter != _typedefs.end();
            iter++)
        {
            delete *iter;
        }
        _typedefs.clear();

        for (HeaderConstants::iterator iter = _constants.begin();
            iter != _constants.end();
            iter++)
        {
            delete *iter;
        }
        _constants.clear();

        for (HeaderStructs::iterator iter = _structs.begin();
            iter != _structs.end();
            iter++)
        {
            delete *iter;
        }
        _structs.clear();

        for (auto iter = _enums.begin();
            iter != _enums.end();
            iter++)
        {
            delete *iter;
        }
        _enums.clear();
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::BuildTypedefs()
    {
        for (HeaderTypedefs::iterator iter = _typedefs.begin();
            iter != _typedefs.end();
            iter++)
        {
            _header->addTypedef(*iter);
        }
        _typedefs.clear();
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::BuildConstants()
    {
        for (HeaderConstants::iterator iter = _constants.begin();
            iter != _constants.end();
            iter++)
        {
            _header->addConstant(*iter);
        }
        _constants.clear();
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::buildStructs()
    {
        for (HeaderStructs::iterator iter = _structs.begin();
            iter != _structs.end();
            iter++)
        {
            _header->addStruct(*iter);
        }
        _structs.clear();
        return ERR_NOERROR;
    }

    a_util::result::Result DDLToHeaderConverter::buildEnums()
    {
        for (auto iter = _enums.begin();
            iter != _enums.end();
            iter++)
        {
            _header->addEnum(*iter);
        }
        _enums.clear();
        return ERR_NOERROR;
    }

    const HeaderType* DDLToHeaderConverter::findTypeByName(const std::string &name)
    {
        const HeaderType* result = NULL;
        if (_known_types != NULL)
        {
            result = findTypeByName(*_known_types, name);
        }
        if (result == NULL)
        {
            result = findTypeByName(_typedefs, name);
        }
        if (result == NULL)
        {
            result = findTypeByName(_structs, name);
        }
        if (result == NULL)
        {
            result = findTypeByName(_enums, name);
        }
        return result;
    }

    template <class C>
    const HeaderType* DDLToHeaderConverter::findTypeByName(const std::vector<C> &vec,
        const std::string &name)
    {
        const HeaderType* result = NULL;
        for (typename std::vector<C>::const_iterator iter = vec.begin();
            iter != vec.end();
            iter++)
        {
            if ((*iter)->getName().compare(name) == 0)
            {
                result = *iter;
                break;
            }
        }
        return result;
    }


    size_t DDLToHeaderConverter::fromAlignment(DDLAlignment::AlignmentType alignment)
    {
        size_t result = 0;
        switch (alignment)
        {
        case DDLAlignment::e_invalid:
            result = 1;
            break;
        case DDLAlignment::e1:
            result = 1;
            break;
        case DDLAlignment::e2:
            result = 2;
            break;
        case DDLAlignment::e4:
            result = 4;
            break;
        case DDLAlignment::e8:
            result = 8;
            break;
        case DDLAlignment::e16:
            result = 16;
            break;
        case DDLAlignment::e32:
            result = 32;
            break;
        case DDLAlignment::e64:
            result = 64;
            break;
        }
        return result;
    }

    std::string DDLToHeaderConverter::cleanUpName(const std::string &name)
    {
        std::string result = name;
        if (_displace.length() > 0)
        {
            if (result.find(_displace) == 0)
            {
                result.erase(0, _displace.length());
            }
        }
        const std::string allowed_characters =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890";
        
        for (size_t idx = 0; idx < result.size(); idx++)
        {
            if (allowed_characters.find(result.at(idx)) == std::string::npos)
            {
                result[idx] = '_';
            }
        }
        return result;
    }

    bool DDLToHeaderConverter::isEnumIntegral(const DDLEnum *ddl_enum) const
    {
        for (auto iter = ddl_enum->getValues().begin(); iter != ddl_enum->getValues().end(); iter++)
        {
            if (!a_util::strings::isInt32(iter->second))
            {
                return false;
            }
        }
        return true;
    }

    void DDLToHeaderConverter::setDisplaceableString(std::string displace)
    {
        _displace = displace;
    }

}
