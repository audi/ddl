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

#ifndef HEADER_FACTORY_H_INCLUDED
#define HEADER_FACTORY_H_INCLUDED

namespace ddl
{
    class Header;

    /**
     * Abstract base class/interface for Factory Method design-pattern.
     */
    class IHeaderFactory
    {
    public:
        /**
         * Getter for the header object.
         * @return the header object
         * @attention The caller/user has the responsibility for the created
         * header object! Especially take this aspect into consideration in
         * matters of the deallocation of memory!
         */
        virtual Header* getHeader() = 0;

        /**
         * Method to build up a new header hierarchy.
         * This will internally create a new header. An existing old header
         * will not be deleted. So make sure to have taken responsibility of all
         * previously created headers by calling getHeader().
         * @retval ERR_NOT_INITIALISED Not yet initialized
         * @retval ERR_UNKNOWN_FORMAT Expected header hierarchy not found
         * @retval ERR_FAILED Some other error occurred.
         * was not found.
         */
        virtual a_util::result::Result createNew() = 0;

        /**
         * The method DestroyHeader destroys the header object and all contained objects.
         * 
         * @returns  Standard result code.
         * @retval ERR_NOERROR  Everything went fine
         */
        virtual a_util::result::Result DestroyHeader() = 0;

        /**
         * Method to build a typedef object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected header hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable
         */
        virtual a_util::result::Result BuildTypedefs() = 0;

        /**
         * Method to build a constants object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected header hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. struct)
         */
        virtual a_util::result::Result BuildConstants() = 0;

        /**
         * Method to build a structs object hierarchy.
         * @retval ERR_UNKNOWN_FORMAT Expected header hierarchy not found
         * @retval ERR_NO_CLASS Cross reference not resolvable (e.g. base type)
         * @retval ERR_UNKNOWN Not all firstly unknown structs have been
         * resolved
         */
        virtual a_util::result::Result buildStructs() = 0;

        /**
        * Method to build a enum object hierarchy.
        * @retval ERR_UNKNOWN_FORMAT Expected header hierarchy not found
        * resolved
        */
        virtual a_util::result::Result buildEnums() = 0;
    };

}   // namespace ddl

#endif  // HEADER_FACTORY_H_INCLUDED
