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

#ifndef HEADER_MAP_CONFIGURATION_H
#define HEADER_MAP_CONFIGURATION_H

#include <vector>
#include "a_util/result.h"

#include "ddlrepresentation/ddldescription.h"
#include "ddlrepresentation/ddldatatype_intf.h"

#include "map_header.h"
#include "map_source.h"
#include "map_transformation.h"
#include "map_assignment.h"
#include "map_trigger.h"
#include "map_target.h"

namespace mapping
{
namespace oo
{
    typedef std::vector<std::string> MapErrorList;
/**
 * MapConfiguration is the central class of the mapping::oo namespace.
 * It represents a whole mapping configuration which can be imported from
 * a mapping configuration file.
 */
class MapConfiguration
{
public: // methods
    /// Mapping import flags (\ref loadFromFile, \ref loadFromDOM)
    enum MapConfigFlags
    {
        // replace the existing configuration
        mc_load_mapping    = 0x1 << 0,
        
        // merge any new configuration into the current one
        // (error on conflicting configuration)
        mc_merge_mapping = 0x1 << 1
    };

    /**
    * CTOR
    */
    MapConfiguration();

    /**
    * CTOR
    * @param [in] ddl The DDL information base used for type lookup
    */
    MapConfiguration(const ddl::DDLDescription* ddl);
    
    /**
    * DTOR
    */
    ~MapConfiguration();

    /**
    * C-CTOR
    */
    MapConfiguration(const MapConfiguration& other);

    /**
    * Assignment operator
    */
    MapConfiguration& operator=(const MapConfiguration& other);
    
    /**
    * Set the reference ddl description
    * 
    * @param [in] ddl The DDL information base used for type lookup
    * @note This call implies an internal Reset call, the configuration is reset
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setDescription(const ddl::DDLDescription* ddl);

    /**
    * Set the reference ddl description without consistency check
    *
    * @param [in] ddl The DDL information base used for type lookup
    * @retval a_util::result::SUCCESS      Everything went fine
     */
    a_util::result::Result setDescriptionWithoutConsistency(const ddl::DDLDescription* ddl);

    /**
    * Modify the reference ddl description
    * Set the new ddl description and check mapping consistency
    * 
    * @param [in] ddl The DDL information base used for type lookup
    * @note This call does not imply a internal Reset call    
    * @retval ERR_INVALID_TYPE Configuration is inconsistent with ddl, mapping validity updated
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result modifyDescription(const ddl::DDLDescription* ddl);

    /**
    * Get the reference ddl description
    */
    const ddl::DDLDescription* getDescription() const;

    /**
    * Resets the entire map configuration to its initial, empty state
    * \note The ddl description remains untouched by this
    * @returns Nothing but doxygen won't shut up if I dont write anything here
    */
    void reset();
    
    /**
    * Import mapping configuration from a file 
    *    Load file in DOM and call loadFromDOM()
    * 
    * @param [in] file_path The file path to the configuration file
    * @param [in] flags Options for loading. See \ref MapConfigFlags
    * 
    * @retval ERR_INVALID_FILE DOM Import from file failed
    *                    (see error list for details)
    * @retval see loadFromDOM()
    */
    a_util::result::Result loadFromFile(const std::string& file_path,
        uint32_t flags = mc_load_mapping);

    /**
    * Import mapping configuration from a XML dom instance
    * 
    * @param [in] dom The dom instance from which to import
    * @param [in] flags Options for loading. See \ref MapConfigFlags
    * 
    * @retval ERR_INVALID_FLAGS Invalid flag combination
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_FAILED Configuration file is invalid
    *                    (see error list for details)
    * @retval ERR_INVALID_TYPE Configuration uses unknown DDL types
    *                          (see error list for details)
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result loadFromDOM(a_util::xml::DOM& dom,
        uint32_t flags = mc_load_mapping);

    /**
    * Import mapping configuration from a XML dom instance without checking for DDL consistency
    *
    * @param [in] dom The dom instance from which to import
    * @param [in] flags Options for loading. See \ref MapConfigFlags
    *
    * @retval ERR_INVALID_FLAGS Invalid flag combination
    * @retval ERR_FAILED Configuration file is invalid
    *                    (see error list for details)
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result loadFromDOMWithoutDDLConsistency(a_util::xml::DOM& dom,
        uint32_t flags = mc_load_mapping);

    /**
    * Import mapping configuration from a file
    *    Load file in DOM and call loadPartiallyFromDOM()
    * Modell is loaded even if it is not consistent with the DDL description
    * Each element has a validity flag representing its consistency
    * 
    * @param [in] file_path The file path to the configuration file
    * @param [in] flags Options for loading. See \ref MapConfigFlags
    * 
    * @retval ERR_INVALID_FILE DOM Import from file failed
    *                    (see error list for details)
    * @retval see loadPartiallyFromDOM()
    */
    a_util::result::Result loadPartiallyFromFile(const std::string& file_path,
        uint32_t flags = mc_load_mapping);

    /**
    * Import mapping configuration from a XML dom instance
    * Modell is laoding even if it is not consistent with the DDL description
    * Each element has a validity flag representing its consistency
    * 
    * @param [in] dom The dom instance from which to import
    * @param [in] flags Options for loading. See \ref MapConfigFlags
    * 
    * @retval ERR_INVALID_FLAGS Invalid flag combination
    * @retval ERR_FAILED Configuration file is invalid
    *                    (see error list for details)
    * @retval ERR_INVALID_TYPE Configuration uses unknown DDL types
    *                          (see error list for details)
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result loadPartiallyFromDOM(a_util::xml::DOM& dom,
        uint32_t flags = mc_load_mapping);

    /**
    * Export mapping configuration to a file
    *    Call writeToDOM() and wirte DOM to file
    * 
    * @param [in] file_path The file path to the configuration file
    * @retval ERR_INVALID_FILE Can not write in file
    *                    (see error list for details)
    * @retval see writeToDOM()
    */
    a_util::result::Result writeToFile(const std::string& file_path);

    /**
    * Export mapping configuration to a XML dom instance
    * 
    * @param [in] dom The dom instance from which to import
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_FAILED Configuration is invalid
    *                    (see error list for details)
    * @retval ERR_INVALID_TYPE Configuration uses unknown DDL types
    *                          (see error list for details)
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOM& dom);
    
    /**
    * Returns the mapping header of the configuration
    */
    const MapHeader& getHeader() const;

    /**
    * Set the description in configuration header
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result setHeaderDescription(const std::string& desc);

    /**
    * Set the ddl paths attribute in configuration header
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result setHeaderDdlPaths(const std::string& ddl_path);

    /**
    * Get the ddl paths attribute from configuration header
    */
    const std::string& getHeaderDdlPaths() const;
    
    /**
    * Returns all transformations contained in the configuration
    */
    const MapTransformationList& getTransformationList() const;
    
    /**
    * Returns a specific transformation from the configuration
    *
    * @param [in] transformation_name The name of the transformation
    *
    * @retval The requested transformation base instance pointer
    * @retval NULL if no transformation instance was found with that name
    */
    const MapTransformationBase* getTransformation(
        const std::string& transformation_name) const;

    /**
    * Returns a specific transformation from the configuration
    *
    * @param [in] transformation_name The name of the transformation
    *
    * @retval The requested transformation base instance pointer
    * @retval NULL if no transformation instance was found with that name
    */
    MapTransformationBase* getTransformation(
        const std::string& transformation_name);

    /**
    * Add a transformation to use in mapping
    * @param [in] transformation_name The transformation name
    * @param [in] transformation_type The transformation type
    *    can be "Periodic", "Signal" or "Data"
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_TYPE Inconsistency with ddl
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addTransformation(const std::string& transformation_name, const std::string& transformation_type);

    /**
    * Remove a transformation
    * @param [in] transformation_name The transformation name
    * @retval ERR_NOT_FOUND      Transformation not found
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeTransformation(const std::string& transformation_name);
    
    /**
    * Returns a specific mapping source from the configuration
    *
    * @param [in] source_name The name of the source
    *
    * @retval The requested source instance pointer
    * @retval NULL if no source instance was found with that name
    */
    const MapSource* getSource(const std::string& source_name) const;

    /**
    * Returns a specific mapping source from the configuration
    *
    * @param [in] source_name The name of the source
    *
    * @retval The requested source instance pointer
    * @retval NULL if no source instance was found with that name
    */
    MapSource* getSource(const std::string& source_name);
    
    /**
    * Returns all mapping sources contained in the configuration
    */
    const MapSourceList& getSourceList() const;

    /**
    * Add a source signal to use in mapping
    * @param [in] name The name of the source
    * @param [in] type The type of the source
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_TYPE Inconsistency with ddl
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addSource(const std::string& name, const std::string& type);

    /**
    * Remove a source signal from the configuration
    * All connections using this source will be deleted
    * @param [in] name The name of the target
    * @retval ERR_NOT_FOUND      Source not found
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeSource(const std::string& name);
    
    /**
    * Returns a specific mapping target from the configuration
    *
    * @param [in] target_name The name of the target
    *
    * @retval The requested target instance pointer
    * @retval NULL if no target instance was found with that name
    */
    const MapTarget* getTarget(const std::string& target_name) const;

    /**
    * Returns a specific mapping target from the configuration
    *
    * @param [in] target_name The name of the target
    *
    * @retval The requested target instance pointer
    * @retval NULL if no target instance was found with that name
    */
    MapTarget* getTarget(const std::string& target_name);

    /**
    * Add a target signal to use in mapping
    * @param [in] name The name of the target
    * @param [in] type The type of the target
    * @retval ERR_INVALID_STATE No description found
    * @retval ERR_INVALID_TYPE Inconsistency with ddl
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addTarget(const std::string& name, const std::string& type);

    /**
    * Remove a target signal from the configuration
    * All connections using this source will be deleted
    * @param [in] name The name of the target
    * @retval ERR_NOT_FOUND      Target not found
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeTarget(const std::string& name);
    
    /**
    * Returns all mapping targets contained in the configuration
    */
    const MapTargetList& getTargetList() const;

    /**
    * Returns the error list, containing human readable error messages.
    * @note The error list is filled by \ref loadFromFile and \ref loadFromDOM
    *       and is reset on every call
    * @returns Const reference to the error list
    */
    const MapErrorList& getErrorList() const;

    /** 
    * Clear error list and verify DDL Description exists
    * @retval ERR_INVALID_STATE No description found
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    a_util::result::Result resetErrors();

    /** 
    * Append error to error list
    **/
    void appendError(const std::string& error_string) const;

    /**
     * Returns bool flag indicating whether consistency of mapping with its DDL was already checked
     * @return Bool flag indicating whether consistency was checked
     */
    bool isConsistencyChecked();

    /**
     * Returns bool flag indicating consistency of mapping with its DDL
     * @return Bool flag indicating consistency
     */
    bool isConsistent();

    /**
    * Checks the configuration for consistency with ddl representation
    *   (all types, transformations etc. are known and present & many other problems)
    * @retval ERR_INVALID_TYPE Configuration is inconsistent with ddl,
    *    error list is populated and validity flags are set
    * @retval a_util::result::SUCCESS Everything went fine, configuration is consistent
    */
    a_util::result::Result checkDDLConsistency();
    
private:
    /**
    * swap implementation for use in the assignment op
    */
    void swap(MapConfiguration& other);

    /**
    * Set reference to new configuration
    * @param [in] config The configuration to reference
    */
    void repairConfigReferences(MapConfiguration& config);

    /** 
    * Import mapping configuration from a XML dom instance
    * 
    * @param [in] dom The dom instance from which to import
    * @param [out] tmp_config The temporary configuration to create
    * @retval ERR_INVALID_ARG  DOM does not contains a valid mapping
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    static a_util::result::Result loadMappingFromDOM(a_util::xml::DOM& dom, MapConfiguration& tmp_config);

    /**
    * Add a target signal to configuration
    * @param [in] target The target signal
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addTarget(const MapTarget& target);

    /**
    * Add a source signal to configuration
    * @param [in] source The source signal
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addSource(const MapSource& source);

    /**
    * Checks the mapping for consistency
    *   All referenced Sources and Transformations must exist in Mapping
    * @retval ERR_INVALID_ARG  Unknown source or transformation is used
    * @retval a_util::result::SUCCESS      Everything went fine, configuration is consistent
    */
    a_util::result::Result checkMappingConsistency(); 

    /**
    * merge another configuration into this one
    * @param [in] other The other instance to be merged
    * @retval ERR_INVALID_ARG Type conflict
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result merge(const MapConfiguration& other);

    /**
    * find a DDL data type within a struct given a path to the element
    *
    * @param [in] struct_name Name of the struct type
    * @param [in] path Path to be found
    * @param [out] type Destination type parameter
    * @param [out] array_size Destination array size parameter
    * @retval ERR_NOT_FOUND The path is invalid or the type was not found
    * @retval ERR_INVALID_TYPE Uses dynamic type
    * @retval a_util::result::SUCCESS The type was found, type and array_size are filled
    */
    a_util::result::Result findTypeObject(const std::string& struct_name, const std::string&
        path, const ddl::IDDLDataType*& type, int& array_size) const;

    /**
    * @overload
    */
    a_util::result::Result findTypeObject(const ddl::DDLComplex& struct_name, const std::string&
        path, const ddl::IDDLDataType*& type, int& array_size) const;

    /**
    * Checks whether the target datatype is compatible with the source datatype
    * @param [in] source Datatype 1
    * @param [in] source_array_size Arraysize 1
    * @param [in] target Datatype 2
    * @param [in] target_array_size Arraysize 2
    * @retval true or false
    */
    bool checkTypeCompatibility(const ddl::IDDLDataType& source, int source_array_size,
        const ddl::IDDLDataType& target, int target_array_size) const;

    /**
    * Checks the configuration for consistency of a new signal
    * @param [in] name The signal name
    * @retval ERR_INVALID_ARG  Signal name already used in configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result checkSignalName(const std::string& name) const;

    /**
    * Checks the configuration for consistency of a new signal
    * @param [in] type The type of the signal
    * @retval ERR_INVALID_TYPE Type not found in DDL
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result checkSignalType(const std::string& type) const;

    /**
    * Checks the configuration for consistency of a new assignment
    * @param [in] assignment The assignment to check
    * @retval ERR_INVALID_ARG  Reference not found in Configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result checkAssignmentReferences(const MapAssignment& assignment) const;

    /**
    * Checks the configuration for consistency of a new assignment
    * @param [in] signal_name The name of the target signal (for error list)
    * @param [in] signal_struct The ddl structure for the signal
    * @param [in] assignment The assignment to check
    * @retval ERR_INVALID_TYPE Definition is inconsistent with ddl
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result checkAssignmentType(const std::string& signal_name, 
        const ddl::DDLComplex& signal_struct, const MapAssignment& assignment) const;

    /**
    * Checks the configuration for consistency of a new trigger
    * @param [in] trigger The trigger to check
    * @retval ERR_INVALID_TYPE Definition is inconsistent with ddl
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result checkTriggerType(const MapTriggerBase* trigger) const;

private:
    /// @cond nodoc
    friend class MapSource;
    friend class MapTarget;
    friend class MapTriggerBase;
    friend class MapDataTrigger;
    MapHeader _header;
    MapSourceList _sources;
    MapTargetList _targets;
    MapTransformationList _transforms;
    // mutable because its not part of the object state
    mutable MapErrorList _errors;
    const ddl::DDLDescription* _ddl_ref;
    bool _checked_for_consistency;
    bool _is_consistent;
    /// @endcond
};

} // namespace oo
} // namespace mapping

#endif // HEADER_MAP_CONFIGURATION_H
