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

#include <ddl.h>
#include "header_to_ddl_converter.h"
#include "header_header.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
        _MAKE_RESULT(-19, ERR_NOT_SUPPORTED);
        _MAKE_RESULT(-16, ERR_NOT_IMPL);
        _MAKE_RESULT(-38, ERR_FAILED);
    }
}

using namespace ddl_generator::oo;

using namespace ddl;

namespace ddl
{

    /**
     * This class tries to resolve a typedef down to its original type, following all consecutive typedefs.
     * start the process by visiting a typedef and if successfull, either the getStruct() or the getBasicType()
     * method will not return a NULL pointer, meaning that this is the found type the typedef is pointing to
     * in the end.
     * The class will not take ownership of anything. Well thats kinda obvious since it only accepts pointers
     * to const types.
     */
    class TypeResolver : public IHeaderVisitor {
    public:
        /**
         * CTOR
         */
        TypeResolver()
        {
            reset();
        }

        /**
         * The method Reset resets all internal states so that the class is fresh and shiny as new.
         *
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine
         */
        a_util::result::Result reset()
        {
            _basic_type = NULL;
            _struct = NULL;
            _enum = NULL;
            _visited_typedefs.clear();
            return ERR_NOERROR;
        }

        /**
         * The method getStruct returns the found struct.
         *
         * @returns  A pointer to the found struct if found, NULL otherwise.
         */
        const HeaderStruct* getStruct()
        {
            return _struct;
        }

        /**
         * The method getBasicType returns the found basic type.
         *
         * @returns  A pointer to the found basic tyoe if found, NULL otherwise.
         */
        const HeaderBasicType* getBasicType()
        {
            return _basic_type;
        }

        /**
        * The method getEnum returns the found enum.
        *
        * @returns  A pointer to the found Enum if found, NULL otherwise.
        */
        const HeaderEnum* getEnum()
        {
            return _enum;
        }

    public: // Implements IHeaderVisitor
        virtual a_util::result::Result visit(const Header* header)
        {
            return ERR_NOT_IMPL;
        }
        virtual a_util::result::Result visit(const HeaderBasicType* basic_type)
        {
            _basic_type = basic_type;
            return ERR_NOERROR;
        }
        virtual a_util::result::Result visit(const HeaderTypedef* type_def)
        {
            // Check if we have visited that typedef before
            if (std::find(_visited_typedefs.begin(), _visited_typedefs.end(), type_def) == _visited_typedefs.end())
            {
                // Store typedef, so we wont visit it again, in case of a circular connection
                _visited_typedefs.push_back(type_def);

                return type_def->getOriginalType()->accept(this);
            }
            return ERR_NOT_SUPPORTED;
        }
        virtual a_util::result::Result visit(const HeaderConstant* constant)
        {
            return ERR_NOT_IMPL;
        }

        virtual a_util::result::Result visit(const HeaderStruct* header_struct)
        {
            _struct = header_struct;
            return ERR_NOERROR;
        }
        virtual a_util::result::Result visit(const HeaderStructElement* struct_element)
        {
            return ERR_NOT_IMPL;
        }

        virtual a_util::result::Result visit(const HeaderEnum* header_enum)
        {
            _enum = header_enum;
            return ERR_NOERROR;
        }
    private:
        const HeaderBasicType *_basic_type;
        const HeaderStruct *_struct;
        const HeaderEnum *_enum;
        HeaderConstTypedefs _visited_typedefs;
    };

    HeaderToDDLConverter::HeaderToDDLConverter() : _description(NULL), _header(NULL)
    { }

    HeaderToDDLConverter::~HeaderToDDLConverter()
    { }


    a_util::result::Result HeaderToDDLConverter::visit(const Header* header)
    {
        _header = header;
        for (HeaderTypedefs::const_iterator iter = header->getTypedefs().begin(); iter != header->getTypedefs().end(); iter++)
        {
            (*iter)->accept(this);
        }
        for (HeaderStructs::const_iterator iter = header->getStructs().begin(); iter != header->getStructs().end(); iter++)
        {
            (*iter)->accept(this);
        }
        for (HeaderConstants::const_iterator iter = header->getConstants().begin(); iter != header->getConstants().end(); iter++)
        {
            (*iter)->accept(this);
        }
        for (auto iter = header->getEnums().begin(); iter != header->getEnums().end(); iter++)
        {
            (*iter)->accept(this);
        }
        return ERR_NOERROR;
    }


    a_util::result::Result HeaderToDDLConverter::visit(const HeaderBasicType* basic_type)
    {
        _basic_types.insert(basic_type);
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::visit(const HeaderTypedef* type_def)
    {
        if (std::find(_typedefs.begin(), _typedefs.end(), type_def) == _typedefs.end())
        {
            _typedefs.push_back(type_def);
            type_def->getOriginalType()->accept(this);
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::visit(const HeaderConstant* constant)
    {
        if (std::find(_constants.begin(), _constants.end(), constant) == _constants.end())
        {
            _constants.push_back(constant);
            // Also add the type of the constant
            constant->getType()->accept(this);
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::visit(const HeaderStruct* header_struct)
    {
        if (std::find(_structs.begin(), _structs.end(), header_struct) == _structs.end())
        {
            // Add struct to vector, so if its referenced within other types used in the struct, there wont be an infinite loop.
            _structs.push_back(header_struct);
            // Take care of the struct elements
            for (HeaderStructElementVec::const_iterator iter = header_struct->getElements().begin(); iter != header_struct->getElements().end(); iter++)
            {
                (*iter)->accept(this);
            }
            // Now move the struct to the end, so all needed types will be defined before the struct
            _structs.erase(std::find(_structs.begin(), _structs.end(), header_struct));
            _structs.push_back(header_struct);

        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::visit(const HeaderStructElement* struct_element)
    {
        struct_element->getType()->accept(this);
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::visit(const HeaderEnum* header_enum)
    {
        if (std::find(_enums.begin(), _enums.end(), header_enum) == _enums.end())
        {
            _enums.push_back(header_enum);
        }
        
        return ERR_NOERROR;
    }

    DDLDescription * HeaderToDDLConverter::getDDL() const
    {
        return _description;
    }

    a_util::result::Result HeaderToDDLConverter::createNew(const ddl::DDLVersion& version /*= 0*/)
    {
        _description = DDLDescription::createDefault(version, 4, false);
        buildHeader();
        buildUnits();
        buildDatatypes();
        buildEnums();
        buildStructs();
        buildStreams();
        

        ddl::DDLInspector inspector(true);
        if (isFailed((inspector.visitDDL(_description))))
        {
            LOG_ERROR(inspector.getLastErrorDesc().c_str());
            return ERR_FAILED;
        }

        return ERR_NOERROR;
    }


    void HeaderToDDLConverter::destroyDDL()
    {
        if (_description != NULL)
        {
            delete _description;
            _description = NULL;
        }
    }

    a_util::result::Result HeaderToDDLConverter::buildHeader()
    {
        DDLHeader* header = _description->getHeader();
        a_util::datetime::Date now = a_util::datetime::getCurrentLocalDate();
        header->setAuthor(a_util::system::getCurrentUserName());
        header->setDateCreation(now);
        header->setDateChange(now);
        header->setDescription(std::string("Generated by DDL_Editor "));
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::buildUnits()
    {
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::buildDatatypes()
    {
        for (HeaderConstBasicTypes::iterator iter = _basic_types.begin(); iter != _basic_types.end(); iter++)
        {
            if (_description->getDataTypeByName((*iter)->getName()) == NULL)
            {
                _description->addDatatype(new DDLDataType((*iter)->getName(), static_cast<unsigned int>((*iter)->getBitsize_t())));
            }
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::buildStructs()
    {
        // First all structs will be created but without any members.
        // This way, all types are already defined.
        // This approach is comparable to a forward declaration known from C++
        for (HeaderConstStructs::iterator struct_iter = _structs.begin(); struct_iter != _structs.end(); struct_iter++)
        {
            DDLComplex* complex = new DDLComplex((*struct_iter)->getName(), 0, "", toAlignment((*struct_iter)->getPacking()));
            _description->addStruct(complex);
        }
        // Second all structs will be filled with elements.
        for (HeaderConstStructs::iterator struct_iter = _structs.begin(); struct_iter != _structs.end(); struct_iter++)
        {
            DDLComplex* complex = _description->getStructByName((*struct_iter)->getName());
            for (HeaderStructElementVec::const_iterator element_iter = (*struct_iter)->getElements().begin(); 
                element_iter != (*struct_iter)->getElements().end(); element_iter++)
            {
                // Resolve the type of the element. Please refer to ticket #19830, comment 48 if you want to know why this is done.
                TypeResolver resolver;
                if (isFailed(((*element_iter)->getType()->accept(&resolver))))
                {
                    return ERR_FAILED;
                }
                IDDLDataType* ddl_data_type = NULL;
                if (resolver.getBasicType() != NULL)
                {
                    ddl_data_type = _description->getDataTypeByName(resolver.getBasicType()->getName());
                }
                else if (resolver.getStruct() != NULL)
                {
                    ddl_data_type = _description->getStructByName(resolver.getStruct()->getName());
                }
                else if (resolver.getEnum() != NULL)
                {
                    ddl_data_type = _description->getEnumByName(resolver.getEnum()->getName());
                }
                else
                {
                    // Something really went wrong here
                    return (ERR_FAILED);
                }
                DDLElement* element = new DDLElement(ddl_data_type,
                    (*element_iter)->getName(),
                    0,                                  // See bytepos related comment below
                    static_cast<unsigned int>((*element_iter)->getArraySize()),
                    DDLByteorder::e_le,
                    DDLAlignment::e0);                 // Alignment will be determined later

                // Take the alignment of the whole struct into account.
                if ((*element_iter)->getType()->getPacking() > (*struct_iter)->getPacking())
                {
                    element->setAlignment(toAlignment((*struct_iter)->getPacking()));
                }
                else
                {
                    element->setAlignment(toAlignment((*element_iter)->getType()->getPacking()));
                }

                // Bytepos will not be set here (to anything but zero), the byte position calculation
                // method of the DDL Inspector will be used later.
                complex->addElement(element);
            }
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::buildStreams()
    {
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::buildEnums()
    {
        for (auto iter = _enums.begin(); iter != _enums.end(); iter++)
        {
            const HeaderEnum* current_enum = *iter;
            if (NULL != _description->getEnumByName(current_enum->getName()))
            {
                continue;
            }
            EnumNameValueVec enum_values;
            for (auto value_iter = current_enum->getValues().begin(); value_iter != current_enum->getValues().end(); value_iter++)
            {
                std::pair<std::string, std::string> pair(value_iter->second, a_util::strings::format("%d", value_iter->first));
                enum_values.push_back(pair);
            }
            DDLDataType* data_type = _description->getDataTypeByName("tInt32");
            DDLEnum* ddl_enum = new DDLEnum(data_type, current_enum->getName(), enum_values);
            
            _description->addEnum(ddl_enum);
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderToDDLConverter::buildStreamMetaTypes()
    {
        return ERR_NOERROR;
    }

    ddl::DDLAlignment::AlignmentType HeaderToDDLConverter::toAlignment(size_t packing)
    {
        DDLAlignment::AlignmentType result = DDLAlignment::e1;
        switch (packing)
        {
        case 0:
        case 1:
            result = DDLAlignment::e1;
            break;
        case 2:
            result = DDLAlignment::e2;
            break;
        case 4:
            result = DDLAlignment::e4;
            break;
        case 8:
            result = DDLAlignment::e8;
            break;
        case 16:
            result = DDLAlignment::e16;
            break;
        case 32:
            result = DDLAlignment::e32;
            break;
        case 64:
            result = DDLAlignment::e64;
            break;
        }
        return result;
    }

}
