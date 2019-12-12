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

#ifndef HEADER_MAP_SOURCE_H
#define HEADER_MAP_SOURCE_H

#include <vector>
#include <set>

#include "a_util/result.h"
#include "a_util/xml.h"

namespace mapping
{
namespace oo
{
    class MapConfiguration;

/**
 * MapSource represents a mapping source from the configuration file
 */
class MapSource
{
public:    
    /**
    * CTOR
    */
    MapSource(MapConfiguration* config);
    /**
    * CTOR
    * @param [in] name signal name
    * @param [in] type signal type
    */
    MapSource(MapConfiguration* config, std::string name, std::string type);

    /**
    * Returns the name of the mapping source
    */
    const std::string& getName() const;
    
    /** 
    * Set name
    * @param [in] new_name The new name of the target
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setName(const std::string& new_name);

    /**
    * Returns the DDL typename of the mapping source
    */
    const std::string& getType() const;

    /** 
    * Set type
    * @param [in] _type The type of the target
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval ERR_INVALID_TYPE Inconsistency with ddl
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setType(const std::string& type);

    /**
    * Returns the validity for the current description
    */
    bool isValid() const;
    
private:
    /**
    * Initializes the source from a source-dom element
    * @param [in] dom_element The dom element from which to import
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG  Missing name or type
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDOM(const a_util::xml::DOMElement& source);

    /**
    * Export source to a XML dom element
    * 
    * @param [in] dom_element The dom element to be written
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;

private:
    /// @cond nodoc
    friend class MapConfiguration;
    friend class MapTarget;
    MapConfiguration* _config;
    std::string _name;
    std::string _type;
    bool _is_valid;
    /// @endcond
};

inline bool operator==(const MapSource& a, const MapSource& b)
{
    return a.getName() == b.getName() && a.getType() == b.getType();
}

inline bool operator!=(const MapSource& a, const MapSource& b)
{
    return !(a == b);
}

/// Public composite types used in the mapping::oo namespace
typedef std::vector<MapSource> MapSourceList;
typedef std::set<std::string> MapSourceNameList;

} // namespace oo
} // namespace mapping

#endif // HEADER_MAP_SOURCE_H
