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

#include "ddlcompare.h"
#include <memory>   //std::unique_ptr<>
#include "../codec/struct_layout.h"
#include "a_util/result/error_def.h"
#include "legacy_error_macros.h"
#include "ddl_error.h"

#include "ddldescription.h"
#include "ddlunit.h"
#include "ddlbaseunit.h"
#include "ddlprefix.h"
#include "ddldatatype.h"
#include "ddlenum.h"
#include "ddlcomplex.h"
#include "ddlstream.h"
#include "ddlstreammetatype.h"
#include "ddlheader.h"
#include "ddlextdeclaration.h"
#include "ddlrefunit.h"
#include "ddlelement.h"
#include "ddlstreamstruct.h"

#include "ddlimporter.h"



namespace ddl
{

//define all needed error types and values locally
_MAKE_RESULT(-3, ERR_UNEXPECTED)
_MAKE_RESULT(-5, ERR_INVALID_ARG)
_MAKE_RESULT(-20, ERR_NOT_FOUND)
_MAKE_RESULT(-38, ERR_FAILED)

#define COMPARE(__name, __method) __name##1->__method() != __name##2->__method()
#define COMPARE_VERSION(__name, __method, __flag_prefix) ((flags & DDLCompare::__flag_prefix ## _versions) && (__name##1->__method() != __name##2->__method()))
#define COMPARE_DESCRIPTION(__name, __flag_prefix) ((flags & DDLCompare::__flag_prefix ## _descriptions) && (__name##1->getDescription() != __name##2->getDescription()))
#define COMPARE_COMMENT(__name, __flag_prefix) ((flags & DDLCompare::__flag_prefix ## _comments) && (__name##1->getComment() != __name##2->getComment()))
#define CHECK_NAMES(__var, __kind) \
if (flags & DDLCompare::icf_names && \
    __var##1->getName() != __var##2->getName()) \
{ \
    RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The " __kind " '%s' has a different name than '%s'.", __var##1->getName().c_str(), __var##2->getName().c_str())); \
} \

#define CHECK_OPTIONAL(__name, __attr) \
(COMPARE(__name, is##__attr##Valid) || \
 (__name##1->is##__attr##Valid() && COMPARE(__name, get##__attr##Value)))

class DDLImporterWrapper
{
    private:
        DDLImporter _importer;

    public:

        ~DDLImporterWrapper()
        {
            _importer.destroyDDL();
        }

        a_util::result::Result create(const std::string& desc, const DDLDescription* ref_desc)
        {
            _importer.setMergeDefaults(false);
            _importer.setPreferReferenceEntities(false);
            RETURN_DDLERROR_IF_FAILED_DESC(_importer.setXML(desc), _importer.getErrorDesc());
            if (ref_desc)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(_importer.createPartial(ref_desc, DDLVersion::ddl_version_invalid), _importer.getErrorDesc());
            }
            else
            {
                RETURN_DDLERROR_IF_FAILED_DESC(_importer.createNew(DDLVersion::ddl_version_invalid), _importer.getErrorDesc());
            }

            return a_util::result::SUCCESS;
        }

        const DDLDescription* getDDL()
        {
            return _importer.getDDL();
        }

};

template<typename T>
static a_util::result::Result CompareSizes(const T& vec1, const T& vec2, uint32_t flags, const std::string& name, uint32_t subset_flags)
{
    if (flags & subset_flags)
    {
        if (vec1.size() > vec2.size())
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, "The first description has more " + name + " then the second.");
        }
    }
    else
    {
        if (vec1.size() != vec2.size())
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, "The descriptions have different amounts of " + name + ".");
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isBinaryEqual(const std::string& type1, const std::string& desc1, const std::string& type2, const std::string& desc2, bool is_subset)
{
    std::unique_ptr<DDLDescription> ref_desc(DDLDescription::createDefault());
    DDLImporterWrapper importer1;
    RETURN_IF_FAILED(importer1.create(desc1, ref_desc.get()));
    DDLImporterWrapper importer2;
    RETURN_IF_FAILED(importer2.create(desc2, ref_desc.get()));

    return isBinaryEqual(type1, importer1.getDDL(), type2, importer2.getDDL(), is_subset);
}

static a_util::result::Result CompareStaticElements(const std::vector<StructLayoutElement>& elements1,
                                     const std::vector<StructLayoutElement>& elements2)
{
    for (size_t n_element = 0; n_element < elements1.size(); ++n_element)
    {
        const StructLayoutElement& element1 = elements1[n_element];
        const StructLayoutElement& element2 = elements2[n_element];
        if (element1.deserialized.bit_offset != element2.deserialized.bit_offset ||
            element1.deserialized.bit_size != element2.deserialized.bit_size)
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The elements '%s' and '%s' differ in size or address.",
                              element1.name.c_str(), element2.name.c_str());
        }
        if (element1.type != element2.type)
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The elements '%s' and '%s' have different types.",
                              element1.name.c_str(), element2.name.c_str());
        }
    }

    return a_util::result::SUCCESS;
}

static a_util::result::Result CompareDynamicElements(const std::vector<DynamicStructLayoutElement>& elements1,
                                      const std::vector<DynamicStructLayoutElement>& elements2)
{
    for (size_t n_element = 0; n_element < elements1.size(); ++n_element)
    {
        const DynamicStructLayoutElement& element1 = elements1[n_element];
        const DynamicStructLayoutElement& element2 = elements2[n_element];

        if (element1.alignment != element2.alignment)
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The elements '%s' and '%s' have different alignment.",
                              element1.name.c_str(), element2.name.c_str());
        }

        if (element1.static_elements.size() != element2.static_elements.size())
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The elements '%s' and '%s' have different amounts of child elements.",
                              element1.name.c_str(), element2.name.c_str());
        }

        RETURN_IF_FAILED(CompareStaticElements(element1.static_elements,
                                               element2.static_elements));

        if (element1.dynamic_elements.size() != element2.dynamic_elements.size())
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The elements '%s' and '%s' have different amounts of dynamic elements.",
                              element1.name.c_str(), element2.name.c_str());
        }

        RETURN_IF_FAILED(CompareDynamicElements(element1.dynamic_elements,
                                                element2.dynamic_elements));
    }
    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isBinaryEqual(const std::string& type1, const DDLDescription* desc1, const std::string& type2, const DDLDescription* des2, bool is_subset)
{
    const DDLComplex* struct1 = desc1->getStructByName(type1);
    if (!struct1)
    {
        RETURN_ERROR_DESCRIPTION(ERR_NOT_FOUND, ("Unable to find definitions for struct " + type1).c_str());
    }
    const DDLComplex* struct2 = des2->getStructByName(type2);
    if (!struct2)
    {
        RETURN_ERROR_DESCRIPTION(ERR_NOT_FOUND, ("Unable to find definitions for struct " + type2).c_str());
    }

    StructLayout layout1(struct1);
    StructLayout layout2(struct2);

    RETURN_IF_FAILED(layout1.isValid());
    RETURN_IF_FAILED(layout2.isValid());

    if (is_subset)
    {
        if (layout1.getStaticElements().size() > layout2.getStaticElements().size())
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The struct '%s' has more elements than '%s'.", type1.c_str(), type2.c_str());
        }
    }
    else
    {
        if (layout1.getStaticElements().size() != layout2.getStaticElements().size())
        {
            RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The structs '%s' and '%s' have different amounts of elements.", type1.c_str(), type2.c_str());
        }
    }

    RETURN_IF_FAILED(CompareStaticElements(layout1.getStaticElements(), layout2.getStaticElements()));


    // check dynamic stuff
    if (layout1.getDynamicElements().size() != layout2.getDynamicElements().size())
    {
        RETURN_ERROR_DESCRIPTION(ERR_FAILED, "The structs '%s' and '%s' have different amounts of dynamic elements.", type1.c_str(), type2.c_str());
    }

    if (layout1.hasDynamicElements())
    {
        RETURN_IF_FAILED(CompareDynamicElements(layout1.getDynamicElements(), layout2.getDynamicElements()));
    }

    return a_util::result::SUCCESS;
}

static a_util::result::Result CompareDataTypes(const DDLDataType* dt1,
                                const DDLDataType* dt2,
                                uint32_t flags)
{
    // Data types have to have the same name
    if (COMPARE(dt, getName))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The datatypes '%s' and '%s' do not match.", dt1->getName().c_str(), dt2->getName().c_str()));
    }

    if (COMPARE_DESCRIPTION(dt, icf) ||
        COMPARE(dt, getArraysize) ||
        COMPARE(dt, getUnit) ||
        COMPARE(dt, getAlignment) ||
        COMPARE(dt, getNumBits) ||
        COMPARE(dt, getArraySizeSource))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The datatype '%s' is different in the second description.", dt1->getName().c_str()));
    }

    if (flags & DDLCompare::icf_visualizations_attributes)
    {
        if (CHECK_OPTIONAL(dt, Min) ||
            CHECK_OPTIONAL(dt, Max))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The datatype min/max attributes of '%s' are different in the second description.", dt1->getName().c_str()));
        }
    }

    return a_util::result::SUCCESS;
}

static a_util::result::Result CompareEnums(const DDLEnum* p_enum1,
                            const DDLEnum* p_enum2,
                            uint32_t flags)
{
    CHECK_NAMES(p_enum, "enum")

    if (COMPARE(p_enum, getType))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The enum '%s' has a different type in the second description.", p_enum1->getName().c_str()));
    }

    if (!(flags & DDLCompare::icf_no_enum_values_check))
    {
        if (COMPARE(p_enum, getValues))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The values of enum '%s' are different in the second description.", p_enum1->getName().c_str()));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLPrefix* prefix1, const DDLPrefix* prefix2,
                             uint32_t flags)
{
    CHECK_NAMES(prefix, "prefix")

    if (COMPARE(prefix, getPower) ||
        COMPARE(prefix, getSymbol))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The prefixes '%s' and '%s' do not match.",
                                                     prefix1->getName().c_str(), prefix2->getName().c_str()));
    }

    return a_util::result::SUCCESS;
}

static a_util::result::Result CompareBaseUnits(const DDLBaseunit* baseunit1,
                                const DDLBaseunit* baseunit2,
                                uint32_t flags)
{
    CHECK_NAMES(baseunit, "base unit")

    if (COMPARE_DESCRIPTION(baseunit, icf) ||
        COMPARE(baseunit, getSymbol))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The baseunits '%s' and '%s' do not match.",
                                                     baseunit1->getName().c_str(), baseunit2->getName().c_str()));
    }

    return a_util::result::SUCCESS;
}

static a_util::result::Result CompareUnits(const DDLUnit* unit1, const DDLUnit* unit2,
                            uint32_t flags)
{
    CHECK_NAMES(unit, "unit")

    if (COMPARE(unit, getDenominator) ||
        COMPARE(unit, getNumerator) ||
        COMPARE(unit, getOffset))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The units '%s' and '%s' do not match.",
                                                     unit1->getName().c_str(), unit2->getName().c_str()));
    }

    if (!(flags & DDLCompare::icf_no_recursion))
    {
        const DDLRefUnitVec& ref_units1 = unit1->getRefUnits();
        const DDLRefUnitVec& ref_units2 = unit2->getRefUnits();

        RETURN_IF_FAILED(CompareSizes(ref_units1, ref_units2, flags, "refunits", DDLCompare::icf_subset));

        for (DDLRefUnitVec::const_iterator it1 = ref_units1.begin(); it1 != ref_units1.end(); ++it1)
        {
            DDLRefUnit* ref_unit1 = *it1;

            DDLRefUnitVec::const_iterator it2 = std::find_if(ref_units2.begin(), ref_units2.end(), DDLCompareFunctor<DDLRefUnit>(ref_unit1->getName()));
            if (it2 == ref_units2.end())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The reference unit '%s' of unit '%s' is not available in the second description.",
                                                                ref_unit1->getName().c_str(), unit1->getName().c_str()));
            }

            DDLRefUnit* ref_unit2 = *it2;

            RETURN_IF_FAILED(DDLCompare::isEqual(ref_unit1, ref_unit2, flags));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const IDDLUnit* i_unit1, const IDDLUnit* i_unit2,
                             uint32_t flags)
{
    const DDLBaseunit* baseunit1 = dynamic_cast<const DDLBaseunit*>(i_unit1);
    const DDLBaseunit* baseunit2 = dynamic_cast<const DDLBaseunit*>(i_unit2);
    const DDLUnit* unit1 = dynamic_cast<const DDLUnit*>(i_unit1);
    const DDLUnit* unit2 = dynamic_cast<const DDLUnit*>(i_unit2);
    if (baseunit1 && baseunit2)
    {
        return CompareBaseUnits(baseunit1, baseunit2, flags);
    }
    else if (unit1 && unit2)
    {
        return CompareUnits(unit1, unit2, flags);
    }
    else
    {
        if (baseunit1)
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The first unit '%s' is a base unit and the second '%s' is a unit",
                                                             i_unit1->getName().c_str(), i_unit2->getName().c_str()));
        }
        else
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The first unit  '%s' is a unit and the second '%s' is a base unit",
                                                             i_unit1->getName().c_str(), i_unit2->getName().c_str()));
        }
        return a_util::result::SUCCESS;
    }
}


a_util::result::Result CompareStructs(const DDLComplex* ddl_struct1, const DDLComplex* ddl_struct2, uint32_t flags)
{
    CHECK_NAMES(ddl_struct, "struct")

    if ((flags & DDLCompare::icf_versions) &&
        ddl_struct1->getVersion() != ddl_struct2->getVersion())
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The versions of '%s' and '%s' do not match.", ddl_struct1->getName().c_str(), ddl_struct2->getName().c_str()));
    }

    if ((flags & DDLCompare::icf_comments) &&
        ddl_struct1->getComment() != ddl_struct2->getComment())
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The comments of '%s' and '%s' do not match.", ddl_struct1->getName().c_str(), ddl_struct2->getName().c_str()));
    }

    if (flags & DDLCompare::icf_memory)
    {
        if (ddl_struct1->getAlignment() != ddl_struct2->getAlignment())
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The alignments of '%s' and '%s' do not match.", ddl_struct1->getName().c_str(), ddl_struct2->getName().c_str()));
        }
    }

    if (!(flags & DDLCompare::icf_no_recursion))
    {
        const DDLElementVec& elements1 = ddl_struct1->getElements();
        const DDLElementVec& elements2 = ddl_struct2->getElements();

        if (flags & DDLCompare::icf_subset)
        {
            if (elements1.size() > elements2.size())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The there are more elements in '%s' than in '%s'.", ddl_struct1->getName().c_str(), ddl_struct2->getName().c_str()));
            }
        }
        else
        {
            if (elements1.size() != elements2.size())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The amount of elements in '%s' and '%s' is not equal.", ddl_struct1->getName().c_str(), ddl_struct2->getName().c_str()));
            }
        }

        DDLElementVec::const_iterator it_element1 = elements1.begin();
        DDLElementVec::const_iterator it_element2 = elements2.begin();
        for (; it_element1 != elements1.end(); ++it_element1, ++it_element2)
        {
            const DDLElement* element1 = *it_element1;
            const DDLElement* element2 = *it_element2;

            // we do this special checks here, sinc the specialized isEqual method does nto have access to the other elements.

            if (element1->isDynamic() != element2->isDynamic())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("One of the elements '%s.%s' and '%s.%s' is dynamic the other not.",
                                                             ddl_struct1->getName().c_str(), element1->getName().c_str(), ddl_struct2->getName().c_str(), element2->getName().c_str()));
            }

            if (element1->isDynamic())
            {
                // find element positions
                DDLElementVec::const_iterator it_size_element1 = std::find_if(elements1.begin(), it_element1,
                                                                            DDLCompareFunctor<DDLElement>(element1->getArraySizeSource()));
                if (it_size_element1 == elements1.end())
                {
                    RETURN_DDLERROR_IF_FAILED_DESC(ERR_INVALID_ARG, a_util::strings::format("The array size element specified in '%s.%s' could not be found",
                                     ddl_struct1->getName().c_str(), element1->getName().c_str()));
                }

                DDLElementVec::const_iterator it_size_element2 = std::find_if(elements2.begin(), it_element2,
                                                                            DDLCompareFunctor<DDLElement>(element2->getArraySizeSource()));
                if (it_size_element2 == elements2.end())
                {
                    RETURN_DDLERROR_IF_FAILED_DESC(ERR_INVALID_ARG, a_util::strings::format("The array size element specified in '%s.%s' could not be found",
                                     ddl_struct2->getName().c_str(), element2->getName().c_str()));
                }

                if (it_size_element1 - elements1.begin() != it_size_element2 - elements2.begin())
                {
                    RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The referenced dynamic array size element of the elements '%s.%s' and '%s.%s' do not match.",
                                                                 ddl_struct1->getName().c_str(), element1->getName().c_str(), ddl_struct2->getName().c_str(), element2->getName().c_str()));
                }
            }

            // only the last element is allowed to be a subset
            uint32_t sub_flags = flags;
            if ((flags & DDLCompare::icf_subset) &&
                (it_element1 +  1 != elements1.end()))
            {
                sub_flags = flags & !DDLCompare::icf_subset;
            }

            RETURN_IF_FAILED(DDLCompare::isEqual(element1, element2, sub_flags));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const IDDLDataType* type1, const IDDLDataType* type2,
                             uint32_t flags)
{
    const DDLDataType* dt1 = dynamic_cast<const DDLDataType*>(type1);
    const DDLDataType* dt2 = dynamic_cast<const DDLDataType*>(type2);
    const DDLComplex* ddl_struct1 = dynamic_cast<const DDLComplex*>(type1);
    const DDLComplex* ddl_struct2 = dynamic_cast<const DDLComplex*>(type2);
    const DDLEnum* enum1 = dynamic_cast<const DDLEnum*>(type1);
    const DDLEnum* enum2 = dynamic_cast<const DDLEnum*>(type2);

    if (dt1 && dt2)
    {
        return CompareDataTypes(dt1, dt2, flags);
    }
    else if (ddl_struct1 && ddl_struct2)
    {
        return CompareStructs(ddl_struct1, ddl_struct2, flags);
    }
    else if (enum1 && enum2)
    {
        return CompareEnums(enum1, enum2, flags);
    }
    else
    {
        std::string kind1 = "basic data type";
        if (ddl_struct1)
        {
            kind1 = "struct";
        }
        else if (enum1)
        {
            kind1 = "enum";
        }

        std::string kind2 = "basic data type";
        if (ddl_struct2)
        {
            kind2 = "struct";
        }
        else if (enum2)
        {
            kind2 = "enum";
        }

        RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The first type '%s' is a %s and the second '%s' is a %s.",
                                                         type1->getName().c_str(), kind1.c_str(), type2->getName().c_str(), kind2.c_str()));

    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLStream* stream1, const DDLStream* stream2,
                             uint32_t flags)
{
    CHECK_NAMES(stream, "stream")

    if (COMPARE_VERSION(stream, getDDLVersion, icf) ||
        COMPARE_DESCRIPTION(stream, icf))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The stream '%s' is different in the second description", stream1->getName().c_str()));
    }

    RETURN_IF_FAILED(isEqual(stream1->getTypeObject(), stream2->getTypeObject(), flags));

    if (!(flags & icf_no_recursion))
    {
        const DDLStreamStructVec& structs1 = stream1->getStructs();
        const DDLStreamStructVec& structs2 = stream2->getStructs();

        if (flags & DDLCompare::icf_subset)
        {
            if (structs1.size() > structs2.size())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The there are more structs in '%s' in the second description", stream1->getName().c_str()));
            }
        }
        else
        {
            if (structs1.size() != structs2.size())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The stream '%s' has a different amount of structs in the second description.", stream1->getName().c_str()));
            }
        }

        DDLStreamStructVec::const_iterator it_struct1 = structs1.begin();
        DDLStreamStructVec::const_iterator it_struct2 = structs2.begin();

        for (; it_struct1 != structs1.end(); ++it_struct1, ++it_struct2)
        {
            const DDLStreamStruct* ddl_struct1 = *it_struct1;
            const DDLStreamStruct* ddl_struct2 = *it_struct2;

            // only the last element is allowed to be a subset
            uint32_t sub_flags = flags;
            if ((flags & DDLCompare::icf_subset) &&
                (it_struct1 +  1 != structs1.end()))
            {
                sub_flags = flags & !DDLCompare::icf_subset;
            }

            RETURN_IF_FAILED(DDLCompare::isEqual(ddl_struct1, ddl_struct2, sub_flags));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLExtDeclaration* ext1, const DDLExtDeclaration* ext2,
                             uint32_t flags)
{
    CHECK_NAMES(ext, "ext declaration") // name == key

    if (COMPARE(ext, getValue))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The external declaration '%s' is different in the second description.",
                                                        ext1->getName().c_str()));
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLRefUnit* ref_unit1, const DDLRefUnit* ref_unit2,
                             uint32_t flags)
{
    CHECK_NAMES(ref_unit, "ref unit")

    if (COMPARE(ref_unit, getPower))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The reference unit '%s' is different in the second description.",
                                                        ref_unit1->getName().c_str()));
    }

    if (!(flags & icf_no_recursion))
    {
        RETURN_IF_FAILED(DDLCompare::isEqual(ref_unit1->getUnitObject(), ref_unit2->getUnitObject(), flags));
        RETURN_IF_FAILED(DDLCompare::isEqual(ref_unit1->getPrefixObject(), ref_unit2->getPrefixObject(), flags));
    }
    else if (flags & icf_names)
    {
        if (COMPARE(ref_unit, getPrefix))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The prefix of the reference unit '%s' is different in the second description.",
                                                         ref_unit1->getName().c_str()));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLElement* element1, const DDLElement* element2,
                             uint32_t flags)
{
    if (flags & DDLCompare::icf_memory)
    {
        if (COMPARE(element, getAlignment))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The alignments of the elements '%s' and '%s' do not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }
    }

    if (COMPARE(element, getArraysize))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The arraysizes of the elements '%s' and '%s' do not match.",
                                                     element1->getName().c_str(), element2->getName().c_str()));
    }

    if (COMPARE(element, getConstantValue))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The constant values of the elements '%s' and '%s' do not match.",
                                                     element1->getName().c_str(), element2->getName().c_str()));
    }

    if (flags & icf_visualizations_attributes)
    {
        if (CHECK_OPTIONAL(element, Default))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The default values of the elements '%s' and '%s' do not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }

        if (CHECK_OPTIONAL(element, Scale))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The scale values of the elements '%s' and '%s' do not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }

        if (CHECK_OPTIONAL(element, Offset))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The scale values of the elements '%s' and '%s' do not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }

        if (CHECK_OPTIONAL(element, Min))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The min values of the elements '%s' and '%s' do not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }

        if (CHECK_OPTIONAL(element, Max))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The max values of the elements '%s' and '%s' do not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }
    }

    if (COMPARE(element, isDynamic))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("One of the elements '%s' and '%s' is dynamic the other not.",
                                                     element1->getName().c_str(), element2->getName().c_str()));
    }

    if (element1->isDynamic())
    {
        // here all we can do is compare the names
        if (flags & DDLCompare::icf_names &&
            COMPARE(element, getArraySizeSource))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The element '%s' has a different arraysize specifier than '%s'.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }
    }

    if (flags & DDLCompare::icf_serialized)
    {
        if (COMPARE(element, getBitpos))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The bitpos of the elements '%s' and '%s' does not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }
        if (COMPARE(element, getBytepos))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The bitpos of the elements '%s' and '%s' does not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }
        if (COMPARE(element, getByteorder))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The bitpos of the elements '%s' and '%s' does not match.",
                                                         element1->getName().c_str(), element2->getName().c_str()));
        }
    }

    CHECK_NAMES(element, "element")

    if (!(flags & icf_no_recursion))
    {
        if (flags & DDLCompare::icf_units)
        {
            IDDLUnit* unit1 = element1->getUnitObject();
            IDDLUnit* unit2 = element2->getUnitObject();

            if ((unit1 == NULL) != (unit2 == NULL))
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The units of the elements '%s' and '%s' do not match.",
                                                             element1->getName().c_str(), element2->getName().c_str()));
            }

            if (unit1)
            {
                RETURN_IF_FAILED(DDLCompare::isEqual(unit1, unit2, flags));
            }
        }

        // we handle the special case of basic data types in order to provide a meaningfull errror message
        DDLDataType* dt1 = dynamic_cast<DDLDataType*>(element1->getTypeObject());
        DDLDataType* dt2 = dynamic_cast<DDLDataType*>(element2->getTypeObject());
        if (dt1 && dt2)
        {
            if (COMPARE(dt, getName))
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The data types of the elements '%s(%s)' and '%s(%s)' do not match.",
                                                             element1->getName().c_str(), dt1->getName().c_str(),
                                                             element2->getName().c_str(), dt2->getName().c_str()));
            }
        }

        RETURN_IF_FAILED(DDLCompare::isEqual(element1->getTypeObject(), element2->getTypeObject(), flags));
    }
    else if (flags & icf_names)
    {
        // compare the names of the types only.
        if (COMPARE(element, getType))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The data types of the elements '%s(%s)' and '%s(%s)' do not match.",
                                                         element1->getName().c_str(), element1->getType().c_str(),
                                                         element2->getName().c_str(), element2->getType().c_str()));
        }

        if (flags & DDLCompare::icf_units)
        {
            if (COMPARE(element, getUnit))
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The units of the elements '%s(%s)' and '%s(%s)' do not match.",
                                                             element1->getName().c_str(), element1->getUnit().c_str(),
                                                             element2->getName().c_str(), element2->getUnit().c_str()));
            }
        }
    }


    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLStreamStruct* ddl_struct1, const DDLStreamStruct* ddl_struct2,
                             uint32_t flags)
{
    if (((flags & DDLCompare::icf_serialized) && COMPARE(ddl_struct, getBytepos)) ||
        ((flags & DDLCompare::icf_names) && COMPARE(ddl_struct, getName)))
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The stream structs '%s' and '%s' are not equal",
                                                     ddl_struct1->getName().c_str(),
                                                     ddl_struct2->getName().c_str()));
    }

    if (!(flags & icf_no_recursion))
    {
        RETURN_IF_FAILED(DDLCompare::isEqual(ddl_struct1->getTypeObject(), ddl_struct2->getTypeObject(), flags));
    }
    else if (flags & icf_names)
    {
        // compare the names of the types only.
        if (COMPARE(ddl_struct, getType))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The types of the stream structs '%s(%s)' and '%s(%s)' do not match.",
                                                         ddl_struct1->getName().c_str(), ddl_struct1->getType().c_str(),
                                                         ddl_struct2->getName().c_str(), ddl_struct2->getType().c_str()));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqual(const DDLDescription* desc1,
                             const DDLDescription* des2,
                             uint32_t flags)
{
    // build flags for the item based comparisions
    uint32_t item_flags = icf_memory | icf_serialized | icf_names;
    if (flags & dcf_versions)
    {
        item_flags |= icf_versions;
    }
    if (flags & dcf_descriptions)
    {
        item_flags |= icf_descriptions;
    }
    if (flags & dcf_comments)
    {
        item_flags |= icf_comments;
    }
    if (flags & dcf_units || flags & dcf_base_units)
    {
        item_flags |= icf_units;
    }
    if (flags & dcf_visualization_attributes)
    {
        item_flags |= icf_visualizations_attributes;
    }
    if (flags & dcf_no_enum_values_check)
    {
        item_flags |= icf_no_enum_values_check;
    }
    if (flags & dcf_no_recursion)
    {
        item_flags |= icf_no_recursion;
    }

    if (flags & dcf_header)
    {
        const DDLHeader* pHeader1 = desc1->getHeader();
        const DDLHeader* pHeader2 = des2->getHeader();

        if (COMPARE(pHeader, getAuthor) ||
            COMPARE_DESCRIPTION(pHeader, dcf) ||
            COMPARE_VERSION(pHeader, getLanguageVersion, dcf) ||
            COMPARE(pHeader, getName))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, "The headers are different.");
        }

        // only check dates if dcf_no_header_dates is not set
        if ((flags & dcf_no_header_dates) == 0 &&
            (COMPARE(pHeader, getDateChange) || COMPARE(pHeader, getDateCreation)))
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, "The headers are different.");
        }

        const DDLExtDeclarationVec& exts1 = pHeader1->getExtDeclarations();
        const DDLExtDeclarationVec& exts2 = pHeader2->getExtDeclarations();

        RETURN_IF_FAILED(CompareSizes(exts1, exts2, flags, "external declarations", dcf_subset));

        for (DDLExtDeclarationVec::const_iterator it_ext1 = exts1.begin(); it_ext1 != exts1.end(); ++it_ext1)
        {
            const DDLExtDeclaration* ext1 = *it_ext1;
            DDLExtDeclarationVec::const_iterator it_ext2 = std::find_if(exts2.begin(), exts2.end(),
                                                                        DDLCompareFunctor<DDLExtDeclaration>(ext1->getName())); // key = name
            if (it_ext2 == exts2.end())
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The external declaration '%s' is not available in the second description.",
                                                                ext1->getName().c_str()));
            }

            const DDLExtDeclaration* ext2 = *it_ext2;

            RETURN_IF_FAILED(isEqual(ext1, ext2, flags));
        }

    }

    if (flags & dcf_data_types)
    {
        const DDLDTVec& dts1 = desc1->getDatatypes();
        const DDLDTVec& dts2 = des2->getDatatypes();

        RETURN_IF_FAILED(CompareSizes(dts1, dts2, flags, "datatypes", dcf_subset));

        for (DDLDTVec::const_iterator it = dts1.begin(); it != dts1.end(); ++it)
        {
            const DDLDataType* dt1 = *it;
            const DDLDataType* dt2 = des2->getDataTypeByName(dt1->getName());
            if (!dt2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The data type '%s' is not available in the second description.", (*it)->getName().c_str()));
            }

            RETURN_IF_FAILED(isEqual(dt1, dt2, item_flags));
        }
    }

    if (flags & dcf_enums)
    {
        const DDLEnumVec& enums1 = desc1->getEnums();
        const DDLEnumVec& enums2 = des2->getEnums();

        RETURN_IF_FAILED(CompareSizes(enums1, enums2, flags, "enums", dcf_subset));

        for (DDLEnumVec::const_iterator it = enums1.begin(); it != enums1.end(); ++it)
        {
            const DDLEnum* enum1 = *it;
            const DDLEnum* enum2 = des2->getEnumByName(enum1->getName());
            if (!enum2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The enum '%s' is not available in the second description.", enum1->getName().c_str()));
            }

            RETURN_IF_FAILED(isEqual(enum1, enum2, item_flags));
        }
    }

    if (flags & dcf_base_units)
    {
        const DDLBaseunitVec& units1 = desc1->getBaseunits();
        const DDLBaseunitVec& units2 = des2->getBaseunits();

        RETURN_IF_FAILED(CompareSizes(units1, units2, flags, "baseunits", dcf_subset));

        for (DDLBaseunitVec::const_iterator it_unit = units1.begin(); it_unit != units1.end(); ++it_unit)
        {
            const DDLBaseunit* unit1 = *it_unit;
            const DDLBaseunit* unit2 = des2->getBaseunitByName(unit1->getName());
            if (!unit2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The baseunit '%s' is not available in the second description.", unit1->getName().c_str()));
            }

            RETURN_IF_FAILED(CompareBaseUnits(unit1, unit2, item_flags));
        }
    }

    if (flags & dcf_units)
    {
        const DDLUnitVec& units1 = desc1->getUnits();
        const DDLUnitVec& units2 = des2->getUnits();

        RETURN_IF_FAILED(CompareSizes(units1, units2, flags, "units", dcf_subset));

        for (DDLUnitVec::const_iterator it_unit = units1.begin(); it_unit != units1.end(); ++it_unit)
        {
            const DDLUnit* unit1 = *it_unit;
            const DDLUnit* unit2 = des2->getUnitByName(unit1->getName());
            if (!unit2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The unit '%s' is not available in the second description.", unit1->getName().c_str()));
            }

            uint32_t unit_flags = item_flags;
            if (flags & dcf_subset)
            {
                unit_flags |= icf_subset;
            }
            RETURN_IF_FAILED(isEqual(unit1, unit2, unit_flags));
        }
    }

    if (flags & dcf_prefixes)
    {
        const DDLPrefixVec& prefixes1 = desc1->getPrefixes();
        const DDLPrefixVec& prefixes2 = des2->getPrefixes();

        RETURN_IF_FAILED(CompareSizes(prefixes1, prefixes2, flags, "prefixes", dcf_subset));

        for (DDLPrefixVec::const_iterator it = prefixes1.begin(); it != prefixes1.end(); ++it)
        {
            const DDLPrefix* prefix1 = *it;
            const DDLPrefix* prefix2 = des2->getPrefixByName(prefix1->getName());
            if (!prefix2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The prefix '%s' is not available in the second description.", prefix1->getName().c_str()));
            }

            RETURN_IF_FAILED(isEqual(prefix1, prefix2, item_flags));
        }
    }

    if (flags & dcf_structs)
    {
        const DDLComplexVec& structs1 = desc1->getStructs();
        const DDLComplexVec& structs2 = des2->getStructs();

        RETURN_IF_FAILED(CompareSizes(structs1, structs2, flags, "structs", dcf_subset));

        for (DDLComplexVec::const_iterator it_struct1 = structs1.begin();
             it_struct1 != structs1.end(); ++it_struct1)
        {
            const DDLComplex* ddl_struct1 = *it_struct1;
            const DDLComplex* ddl_struct2 = des2->getStructByName(ddl_struct1->getName());
            if (!ddl_struct2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The struct '%s' is not available in the second description.", ddl_struct1->getName().c_str()));
            }

            RETURN_IF_FAILED(isEqual(ddl_struct1, ddl_struct2, item_flags));
        }
    }

    if (flags & dcf_streams)
    {
        const DDLStreamVec& streams1= desc1->getStreams();
        const DDLStreamVec& streams2= des2->getStreams();

        RETURN_IF_FAILED(CompareSizes(streams1, streams2, flags, "streams", dcf_subset));

        DDLStreamVec::const_iterator it_stream1 = streams1.begin();
        DDLStreamVec::const_iterator it_stream2 = streams2.begin();
        for (; it_stream1 != streams1.end(); ++it_stream1, ++it_stream2)
        {
            const DDLStream* stream1 = *it_stream1;
            const DDLStream* stream2 = des2->getStreamByName(stream1->getName());
            if (!stream2)
            {
                RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The stream '%s' is not available in the second description.", stream1->getName().c_str()));
            }

            RETURN_IF_FAILED(isEqual(stream1, stream2, item_flags));
        }
    }

    return a_util::result::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// string based methods
//////////////////////////////////////////////////////////////////////////////////////////////////////////

a_util::result::Result DDLCompare::isEqualPrefix(const std::string& prefix_str1, const std::string& desc1,
                                   const std::string& prefix_str2, const std::string& desc2,
                                   uint32_t flags)
{
    a_util::memory::unique_ptr<DDLDescription> ref_desc(DDLDescription::createDefault());
    DDLImporterWrapper importer1;
    RETURN_IF_FAILED(importer1.create(desc1, ref_desc.get()));
    DDLImporterWrapper importer2;
    RETURN_IF_FAILED(importer2.create(desc2, ref_desc.get()));

    const DDLPrefix* prefix1 = importer1.getDDL()->getPrefixByName(prefix_str1);
    if (!prefix1)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The prefix '%s' is not definied within the first DDL.", prefix_str1.c_str()));
    }

    const DDLPrefix* prefix2 = importer2.getDDL()->getPrefixByName(prefix_str2);
    if (!prefix2)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The prefix '%s' is not definied within the second DDL.", prefix_str2.c_str()));
    }

    return isEqual(prefix1, prefix2, flags);
}

a_util::result::Result DDLCompare::isEqualUnit(const std::string& unit_str1, const std::string& desc1,
                                 const std::string& unit_str2, const std::string& desc2,
                                 uint32_t flags)
{
    if (unit_str1 != unit_str2)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_FAILED, a_util::strings::format("The units '%s' and '%s' do not match.",
                                                     unit_str1.c_str(), unit_str1.c_str()));
    }

    a_util::memory::unique_ptr<DDLDescription> ref_desc(DDLDescription::createDefault());
    DDLImporterWrapper importer1;
    RETURN_IF_FAILED(importer1.create(desc1, ref_desc.get()));
    DDLImporterWrapper importer2;
    RETURN_IF_FAILED(importer2.create(desc2, ref_desc.get()));

    const DDLBaseunit* baseunit1 = importer1.getDDL()->getBaseunitByName(unit_str1);
    const DDLBaseunit* baseunit2 = importer2.getDDL()->getBaseunitByName(unit_str2);
    const DDLUnit* unit1 = importer1.getDDL()->getUnitByName(unit_str1);
    const DDLUnit* unit2 = importer2.getDDL()->getUnitByName(unit_str2);

    if (!baseunit1 && !unit1)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The units '%s' is not defined in the first description.",
                                                         unit_str1.c_str()));
    }
    if (!baseunit2 && !unit2)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The units '%s' is not defined in the second description.",
                                                         unit_str2.c_str()));
    }

    if (baseunit1 && baseunit2)
    {
        return CompareBaseUnits(baseunit1, baseunit2, flags);
    }
    else if (unit1 && unit2)
    {
        return CompareUnits(unit1, unit2, flags);
    }
    else
    {
        if (baseunit1)
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The units '%s' is a base unit in the first description and the unit '%s' is a unit in the second.",
                                                             unit_str1.c_str(), unit_str2.c_str()));
        }
        else
        {
            RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The units '%s' is a unit in the first description and the unit '%s' is a base unit in the second.",
                                                             unit_str1.c_str(), unit_str2.c_str()));
        }
    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqualType(const std::string& type1, const std::string& desc1,
                                 const std::string& type2, const std::string& desc2,
                                 uint32_t flags)
{
    a_util::memory::unique_ptr<DDLDescription> ref_desc(DDLDescription::createDefault());
    DDLImporterWrapper importer1;
    RETURN_IF_FAILED(importer1.create(desc1, ref_desc.get()));
    DDLImporterWrapper importer2;
    RETURN_IF_FAILED(importer2.create(desc2, ref_desc.get()));

    const DDLDataType* dt1 = importer1.getDDL()->getDataTypeByName(type1);
    const DDLDataType* dt2 = importer2.getDDL()->getDataTypeByName(type2);
    const DDLComplex* ddl_struct1 = importer1.getDDL()->getStructByName(type1);
    const DDLComplex* ddl_struct2 = importer2.getDDL()->getStructByName(type2);
    const DDLEnum* enum1 = importer1.getDDL()->getEnumByName(type1);
    const DDLEnum* enum2 = importer2.getDDL()->getEnumByName(type2);

    if (!dt1 && !ddl_struct1 && !enum1)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The type '%s' is not defined in the first description.",
                                                         type1.c_str()));
    }
    if (!dt2 && !ddl_struct2 && !enum2)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The type '%s' is not defined in the second description.",
                                                         type2.c_str()));
    }

    if (dt1 && dt2)
    {
        return CompareDataTypes(dt1, dt2, flags);
    }
    else if (ddl_struct1 && ddl_struct2)
    {
        return CompareStructs(ddl_struct1, ddl_struct2, flags);
    }
    else if (enum1 && enum2)
    {
        return CompareEnums(enum1, enum2, flags);
    }
    else
    {
        std::string kind1 = "basic data type";
        if (ddl_struct1)
        {
            kind1 = "struct";
        }
        else if (enum1)
        {
            kind1 = "enum";
        }

        std::string kind2 = "basic data type";
        if (ddl_struct2)
        {
            kind2 = "struct";
        }
        else if (enum2)
        {
            kind2 = "enum";
        }

        RETURN_DDLERROR_IF_FAILED_DESC(ERR_UNEXPECTED, a_util::strings::format("The first type '%s' is a %s and the second '%s' is a %s.",
                                                         type1.c_str(), kind1.c_str(), type2.c_str(), kind2.c_str()));

    }

    return a_util::result::SUCCESS;
}

a_util::result::Result DDLCompare::isEqualStream(const std::string& stream_str1, const std::string& desc1,
                                   const std::string& stream_str2, const std::string& desc2,
                                   uint32_t flags)
{
    a_util::memory::unique_ptr<DDLDescription> ref_desc(DDLDescription::createDefault());
    DDLImporterWrapper importer1;
    RETURN_IF_FAILED(importer1.create(desc1, ref_desc.get()));
    DDLImporterWrapper importer2;
    RETURN_IF_FAILED(importer2.create(desc2, ref_desc.get()));

    const DDLStream* stream1 = importer1.getDDL()->getStreamByName(stream_str1);
    if (!stream1)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The stream '%s' is not definied within the first DDL.", stream_str1.c_str()));
    }

    const DDLStream* stream2 = importer2.getDDL()->getStreamByName(stream_str2);
    if (!stream2)
    {
        RETURN_DDLERROR_IF_FAILED_DESC(ERR_NOT_FOUND, a_util::strings::format("The stream '%s' is not definied within the second DDL.", stream_str2.c_str()));
    }

    return isEqual(stream1, stream2, flags);
}

a_util::result::Result DDLCompare::isEqual(const std::string& desc1,
                             const std::string& desc2,
                             uint32_t flags)
{
    DDLImporterWrapper importer1;
    RETURN_IF_FAILED(importer1.create(desc1, NULL));
    DDLImporterWrapper importer2;
    RETURN_IF_FAILED(importer2.create(desc2, NULL));

    return isEqual(importer1.getDDL(), importer2.getDDL(), flags);
}


}


