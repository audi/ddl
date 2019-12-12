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

#ifndef HEADER_MAP_ASSIGNMENT_H
#define HEADER_MAP_ASSIGNMENT_H

#include "a_util/result.h"
#include "a_util/xml.h"

namespace mapping
{
namespace oo
{ 
    typedef std::vector<std::string> MapErrorList;
/**
 * MapAssignment 
 */
class MapAssignment
{
public: 
    /**
    * CTOR
    */
    MapAssignment();

    /**
    * CTOR
    * @param [in] name Name of assigned signal or element
    */
    MapAssignment(std::string to);

    /**
    * Returns the validity for the current description
    */
    bool isValid() const;

    /**
    * Returns the source element name of the assignment (or an empty string)
    */
    const std::string& getFrom() const;
    
    /**
    * Returns the source name of the assignment (or an empty string)
    */
    const std::string& getSource() const;

    /**
    * Connects the target element or signal with a source
    * @param [in] source_element_path The complete path to the source element
    *        it is constructed with [SignalName].[ElementName]
    * @retval ERR_INVALID_ARG      Source is empty
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result connect(const std::string& source_element_path);

    /**
    * Returns the constant of the assignment (or an empty string)
    */
    const std::string& getConstant() const;

    /**
    * Setter for the constant of the assignment
    * @param [in] constant The constant to assign
    * @retval ERR_INVALID_ARG      Constant is empty
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setConstant(const std::string& constant);

    /**
    * Returns the constant of the assignment (or an empty string)
    */
    const std::string& getFunction() const;

    /**
    * Setter for the SimulationTime assignment
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setSimulationTimeFunction();

    /**
    * Setter for the TriggerCounter assignment
    * @param [in] modulo The modulo for the function
    *    it represents the maximal value that can be assigned before going back to 0
    *    if left empty, the limit will be the maximal value of Float64
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setTriggerCounterFunction(const std::string& modulo);

    /**
    * Setter for the Received assignment
    * @param [in] source The name of the source to consider
    * @retval ERR_INVALID_ARG      Source is empty
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setReceivedFunction(const std::string& source);

    /**
    * Returns the constant of the assignment (or an empty string)
    */
    const std::string& getModulo() const;
    
    /**
    * Returns the name of the target element that is assigned
    */
    const std::string& getTo() const;
    
    /**
    * Returns the associated transformation of this assignment (or an empty string)
    */
    const std::string& getTransformation() const;
    
    /**
    * Associates the transformation to this assignment
    * @param [in] transformation_name The name of the transformation to add
    * @retval ERR_INVALID_ARG      Source or from is empty (no signal connected)
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setTransformation(const std::string& transformation_name);

    /**
    * Remove the transformation from this assignment
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeTransformation();

private:
    /**
    * Initializes the assignment from a assignment-dom element
    * @param [in] dom_element The dom element from which to import
    * @param [out] errors The error list for debug
    * @retval ERR_INVALID_ARG  Missing name or type
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDOM(const a_util::xml::DOMElement& assignment, MapErrorList& errors);

    /**
    * Export assignment to a XML dom element
    * 
    * @param [in] dom_element The dom element from which to import
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;
    
private:
    /// @cond nodoc
    friend class MapConfiguration;
    friend class MapTarget;
    std::string _from;
    std::string _source;
    std::string _constant;
    std::string _function; // "simulation_time" -> no args, "trigger_counter" -> arg in modulo, "received" -> arg in from
    std::string _modulo;
    std::string _to;
    std::string _transform;
    bool _is_valid;
    /// @endcond
};

inline bool operator==(const MapAssignment& a, const MapAssignment& b)
{
    return a.getConstant() == b.getConstant() && a.getFrom() == b.getFrom() &&
        a.getSource() == b.getSource() && a.getTo() == b.getTo() &&
        a.getFunction() == b.getFunction() && a.getModulo() == b.getModulo() &&
        a.getTransformation() == b.getTransformation();
}

inline bool operator!=(const MapAssignment& a, const MapAssignment& b)
{
    return !(a == b);
}

/// Public composite types used in the mapping::oo namespace
typedef std::vector<MapAssignment> MapAssignmentList;

} // namespace oo
} // namespace mapping

#endif // HEADER_MAP_ASSIGNMENT_H
