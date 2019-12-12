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

#ifndef HEADER_MAP_TARGET_H
#define HEADER_MAP_TARGET_H

#include "a_util/result.h"
#include "a_util/xml.h"

#include "map_assignment.h"
#include "map_trigger.h"
#include "map_source.h"

namespace mapping
{
namespace oo
{
    class MapConfiguration;

/**
 * MapTarget represents a mapped target from the mapping configuration 
 */
class MapTarget
{
public:
    /**
    * CTOR
    */
    MapTarget(MapConfiguration* config);

    /**
    * CTOR
    * @param [in] name The signal name
    * @param [in] type The signal type
    */
    MapTarget(MapConfiguration* config, std::string name, std::string type);

    /**
    * DTOR
    */
    ~MapTarget();

    /**
    * CCTOR
    */
    MapTarget(const MapTarget& other);

    /**
    * Assignment operator
    */
    MapTarget& operator=(const MapTarget& other);

    /**
    * Returns the name of the mapped target
    */
    const std::string& getName() const;

    /** 
    * Set name
    * @param [in] new_name The new name of the target
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setName(const std::string& name);
    
    /**
    * Returns the DDL typename of the mapped target
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

    /**
    * Add an assignment
    * @param [in] assignment The assignment to add
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_ARG  Inconsistency with current configuration
    * @retval ERR_INVALID_TYPE Inconsistency with ddl
    * @retval ERR_NOT_SUPPORTED Assignment overlap with existing assignments
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addAssignment(const MapAssignment& assignment); 

    /**
    * Remove an assignment 
    * @param [in] element_name The name of the assigned element
    * @retval ERR_NOT_FOUND    Assignement not found
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeAssignment(const std::string& element_name); 

    /**
    * Returns all assignments configured for this target
    */
    const MapAssignmentList& getAssignmentList() const;
    
    /**
    * Add a trigger
    * @param [in] trigger The trigger to add
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_NOT_FOUND    Target not found
    * @retval ERR_INVALID_ARG  Inconsistency with current configuration
    * @retval ERR_INVALID_TYPE Inconsistency with ddl
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addTrigger(MapTriggerBase* trigger); 

    /**
    * Remove a trigger 
    * @param [in] trigger The trigger to remove
    * @retval ERR_NOT_FOUND    Trigger not found
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeTrigger(MapTriggerBase* trigger); 

    /**
    * Returns all triggers configured for this target
    */
    const MapTriggerList& getTriggerList() const;

    /**
    * Returns all referenced sources for this target.
    */
    const MapSourceNameList& getReferencedSources() const;

private:
    /**
    * Initializes the target from a target-dom element
    * @param [in] dom_element The dom element from which to import
    * @param [in] lstSources The list of existing sources in mapping
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG  Missing name or type
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDOM(const a_util::xml::DOMElement& target);

    /**
    * Export target to a XML dom element
    * 
    * @param [in] dom_element The dom element to be written
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;

    /**
    * Checks if the assignments overlap
    * @retval ERR_NOT_SUPPORTED   Assignments overlap
    * @retval a_util::result::SUCCESS         Everything went fine
    */
    a_util::result::Result checkDoubleAssignments();

    /**
    * Add an assignment to this target
    * @param [in] assignment The assignment to add
    * @param [in] lstSources The list of existing sources in mapping
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG     Missing or erroneous attribute
    * @retval a_util::result::SUCCESS         Everything went fine
    */
    a_util::result::Result addAssignmentNoTypeCheck(const MapAssignment& assignment);
    
    /**
    * Remove an assignment from this target
    * @param [in] strTo The name of the target element to unassign
    * @retval ERR_NOT_FOUND      Assignement not found
    * @retval a_util::result::SUCCESS         Everything went fine
    */
    a_util::result::Result removeAssignmentWithoutClear(const std::string& element_name);

    /**
    * Add a trigger to this target
    * @param [in] trigger The trigger to add
    * @param [in] lstSources The list of existing sources in mapping
    * @retval a_util::result::SUCCESS         Everything went fine
    */
    a_util::result::Result addTriggerNoTypeCheck(MapTriggerBase* trigger);

    /**
    * Modify a source signal name
    * @param [in] _name The name of the target
    * @param [in] new_name The new name of the target
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result modifySourceName(const std::string& name, const std::string& new_name);
    
    /**
    * swap implementation
    */
    void swap(MapTarget& other);

private:
    /// @cond nodoc
    friend class MapConfiguration;
    friend class MapSource;
    MapConfiguration* _config;
    std::string _name;
    std::string _type;
    MapSourceNameList _sources;
    MapAssignmentList _assignments;
    MapTriggerList _triggers;
    bool _is_valid;
    /// @endcond
};

bool operator==(const MapTarget& a, const MapTarget& b);

inline bool operator!=(const MapTarget& a, const MapTarget& b)
{
    return !(a == b);
}

/// Public composite types used in the mapping::oo namespace
typedef std::vector<MapTarget> MapTargetList;

} // namespace oo
} // namespace mapping

#endif // HEADER_MAP_TARGET_H
