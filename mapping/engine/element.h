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

#ifndef ELEMENT_HEADER
#define ELEMENT_HEADER

#include "a_util/result.h"

#include "ddlrepresentation/ddldatatype_intf.h"
#include "ddlrepresentation/ddldatatype.h"
#include "ddlrepresentation/ddlcomplex.h"
#include "ddlrepresentation/ddlenum.h"

#include "mapping/configuration/map_transformation.h"
#include "mapping/ddl_helper.h"

namespace mapping
{
namespace rt
{

class Target;

/// TargetElement represents a single signal element in the target
class TargetElement
{
public:
    /**
    * CTOR
    * @param[in] target The target containing this element
    */
    TargetElement(Target* target);

    /**
    * DTOR
    */
    ~TargetElement();

    /**
    * Creation method to initialize the object
    * @param [in] element_ptr The Pointer referencing the element in target buffer
    * @param [in] pDatatType The element datatype
    * @param [in] array_size The array size of the element
    * @param [in] element_name The name of the element in Assignment, for debug only
    * @returns Standard result
    */
    a_util::result::Result create(void* element_ptr, ddl::IDDLDataType* data_type,
        const unsigned int array_size, const std::string& element_name);

    /**
    * Setter for transformation
    * Write constant from Mapping or default value from DDL or 0
    * @param [in] transformation The transformation to apply for to element
    * @returns Standard result
    */
    a_util::result::Result setTransformation(const oo::MapTransformationBase* transformation);

    /**
    * Method to give a default value to the element
    * @param [in] default_value The default value as a string
    * @returns Standard result
    */
    a_util::result::Result setDefaultValue(const std::string& default_value);

    /**
    * Setter value to the element
    * @param [in] data The pointer referencing the source buffer
    * @param [in] src_type The datatype of the source element
    * @param [in] mem_size The size of the source buffer
    * @returns Standard result
    */
    a_util::result::Result setValue(const void* data, uint32_t src_type, size_t mem_size);

    /**
    * Getter for the parent target reference
    * @returns The target
    */
    Target* getTarget();

private:
    /// @cond nodoc
    Target* _target;
    void* _element_ptr;
    TypeMap _type_map;
    uint32_t _type_int;
    ddl::DDLDataType* _type;
    ddl::DDLComplex* _struct;
    ddl::DDLEnum* _enum;
    unsigned int _array_size;
    const oo::MapTransformationBase* _transformation;
    // For Debug only
    std::string _element_name;
    /// @endcond
};

typedef std::vector<TargetElement*> TargetElementList;

} // namespace rt
} // namespace mapping
#endif //ELEMENT_HEADER
