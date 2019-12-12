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

#include "ddlversion.h"


namespace ddl
{

    const DDLVersion DDLVersion::ddl_version_invalid = DDLVersion(0, 0);
    const DDLVersion DDLVersion::ddl_version_10 = DDLVersion(1, 0);
    const DDLVersion DDLVersion::ddl_version_11 = DDLVersion(1, 1);
    const DDLVersion DDLVersion::ddl_version_12 = DDLVersion(1, 2);
    const DDLVersion DDLVersion::ddl_version_20 = DDLVersion(2, 0);
    const DDLVersion DDLVersion::ddl_version_30 = DDLVersion(3, 0);
    const DDLVersion DDLVersion::ddl_version_40 = DDLVersion(4, 0);
    const DDLVersion DDLVersion::ddl_version_41 = DDLVersion(4, 1);
    const DDLVersion DDLVersion::ddl_version_current = ddl_version_41;

    DDLVersion::DDLVersion(): DDLVersion(DDLVersion::getDefaultVersion())
    {

    }

    DDLVersion::DDLVersion(uint32_t major, uint32_t minor) :
        _major(major), _minor(minor)
    {

    }

    uint32_t DDLVersion::getMajor() const
    {
        return _major;
    }

    uint32_t DDLVersion::getMinor() const
    {
        return _minor;
    }

    std::string DDLVersion::toString() const
    {
        if (*this == DDLVersion(1, 1))
        {
            return std::string("1.0+");
        }
        return a_util::strings::format("%u.%02u", _major, _minor);
    }

    DDLVersion DDLVersion::fromString(const std::string& version)
    {
        if (version.size() == 0)
        {
            return ddl_version_invalid;
        }
        auto tokens = a_util::strings::split(version, ".");

        uint32_t major = 0, minor = 0;

        if (a_util::strings::isUInt32(tokens.at(0)))
        {
            major = a_util::strings::toUInt32(tokens.at(0));
        }
        else
        {
            return ddl_version_invalid;
        }
        
        if (tokens.size() == 2)
        {
            //special case 1.0+
            if (tokens.at(1) == "0+" || tokens.at(1) == "00+")
            {
                minor = 1;
            }
            // standard case
            else if (a_util::strings::isUInt32(tokens.at(1)))
            {
                minor = a_util::strings::toUInt32(tokens.at(1));
            }
        }
        return DDLVersion(major, minor);
    }

    bool DDLVersion::isValidVersion() const
    {
        if (*this < ddl_version_10)
        {
            return false;
        }

        if (*this > ddl_version_current)
        {
            return false;
        }

        return true;
    }

    bool DDLVersion::operator ==(const DDLVersion& other) const
    {
        if (_major == other._major && _minor == other._minor)
        {
            return true;
        }
        return false;
    }

    bool DDLVersion::operator !=(const DDLVersion& other) const
    {
        return !(*this == other);
    }

    bool DDLVersion::operator >(const DDLVersion& other) const
    {
        if (_major > other._major)
        {
            return true;
        }
        else if (_major < other._major)
        {
            return false;
        }

        if (_minor > other._minor)
        {
            return true;
        }
        return false;
    }

    bool DDLVersion::operator <(const DDLVersion& other) const
    {
        if (_major < other._major)
        {
            return true;
        }
        else if (_major > other._major)
        {
            return false;
        }

        if (_minor < other._minor)
        {
            return true;
        }
        return false;
    }

    bool DDLVersion::operator <=(const DDLVersion& other) const
    {
        return !(*this > other);
    }

    bool DDLVersion::operator >=(const DDLVersion& other) const
    {
        return !(*this < other);
    }
}