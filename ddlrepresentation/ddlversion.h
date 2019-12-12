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

#ifndef DDLVERSION_H_INCLUDED
#define DDLVERSION_H_INCLUDED

#include "ddl_common.h"

namespace ddl
{
    class DDLVersion
    {
    public:
        DDLVersion();

        DDLVersion(uint32_t major, uint32_t minor);

        uint32_t getMajor() const;
        uint32_t getMinor() const;

        std::string toString() const;

        bool isValidVersion() const;

        static DDLVersion fromString(const std::string& version);

        static const DDLVersion& getDefaultVersion()
        {
            return ddl_version_current;
        }

        /* ignore patch version for all comparisons */
        bool operator ==(const DDLVersion& other) const;
        bool operator !=(const DDLVersion& other) const;
        bool operator >(const DDLVersion& other) const;
        bool operator <(const DDLVersion& other) const;
        bool operator <=(const DDLVersion& other) const;
        bool operator >=(const DDLVersion& other) const;

        // known Versions
        static const DDLVersion ddl_version_invalid;
        static const DDLVersion ddl_version_10;
        static const DDLVersion ddl_version_11;
        static const DDLVersion ddl_version_12;
        static const DDLVersion ddl_version_20;
        static const DDLVersion ddl_version_30;
        static const DDLVersion ddl_version_40;
        static const DDLVersion ddl_version_41;
        static const DDLVersion ddl_version_current;


    private:
        uint32_t _major;
        uint32_t _minor;
    };

}

#endif