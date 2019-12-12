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

#ifndef DDL_BUILDER_H_INCLUDED
#define DDL_BUILDER_H_INCLUDED

#include "ddl_common.h"
#include "ddlversion.h"

namespace ddl
{
    class DDLDescription;

    /**
     * Abstract base class/interface for Factory Method design-pattern.
     */
    class IDDLFactoryMethod
    {
    public:
        /**
         * Getter for the DDL object.
         * @return the DDL object
         * @attention The caller/user has the responsibility for the created
         * DDL object! Especially take this aspect into consideration in
         * matters of the deallocation of memory!
         */
        virtual DDLDescription * getDDL() const = 0;

        /**
         * Method to build up a new DDL hierarchy.
         * @param[in] version  The version of the DDL hierarchy to be created. 0 Means newest version.
         * @retval ERR_NOT_INITIALIZED Not yet initialized (see setter methods
         * in concrete classes, e.g. \c DDLImporter::setFile())
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. refUnit)
         * @retval ERR_UNKNOWN Cross reference has not been resolved
         * @retval ERR_NOT_FOUND At least one mandatory element (e.g. header)
         * was not found.
         */
        virtual a_util::result::Result createNew(const DDLVersion& version = DDLVersion::ddl_version_invalid) = 0;

        /**
         * Method to destroy the DDL object and all contained objects.
         */
        virtual void destroyDDL() = 0;

        /**
         * Method to build a header object.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         */
        virtual a_util::result::Result buildHeader() = 0;

        /**
         * Method to build a units object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. refUnit)
         * @retval ERR_UNKNOWN Not all firstly unknown units have been resolved
         */
        virtual a_util::result::Result buildUnits() = 0;

        /**
         * Method to build a datatypes object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. unit)
         */
        virtual a_util::result::Result buildDatatypes() = 0;

        /**
         * Method to build a enums object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. refEnums)
         * @retval ERR_UNKNOWN Not all firstly unknown structs have been
         * resolved
         */
        virtual a_util::result::Result buildEnums() = 0;

       /**
         * Method to build a structs object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. datatype)
         * @retval ERR_UNKNOWN Not all firstly unknown structs have been
         * resolved
         */
        virtual a_util::result::Result buildStructs() = 0;

        /**
         * Method to build a streams object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. struct)
         */
        virtual a_util::result::Result buildStreams() = 0;

        /**
         * Method to build a streams object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected XML hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. struct)
         */
        virtual a_util::result::Result buildStreamMetaTypes() = 0;

        /**
         * DTOR
         */
        virtual ~IDDLFactoryMethod(){}
    };

}   // namespace ddl

#endif  // DDL_BUILDER_H_INCLUDED
