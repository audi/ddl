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

#ifndef HEADER_MAP_TRIGGER_H
#define HEADER_MAP_TRIGGER_H

#include "a_util/result.h"
#include "a_util/xml.h"

namespace mapping
{
namespace oo
{
    class MapConfiguration;
/**
 * cMapTrigger forms a base class for a trigger contained in the configuration
 */
class MapTriggerBase
{
public:
    /**
    * CTOR
    */
    MapTriggerBase(MapConfiguration* config);

    /**
    * DTOR
    */
    virtual ~MapTriggerBase();

    /**
    * Returns the validity for the current description
    */
    bool isValid() const;

    /**
    * returns an optional dependency on a source name
    */
    virtual std::string getSourceDependency() const;

    /**
    * Polymorphic comparison method
    */
    virtual bool isEqual(const MapTriggerBase& other) const = 0;

    /** 
    * Check Trigger validity and set validity flag
    * @retval validity flag
    **/
    bool checkValidity();

        /**
    * Checks the configuration for consistency of a new trigger
    * @param [in] _name The name of the target
    * @param [in] _type The type of the target
    * @retval ERR_INVALID_ARG  Reference not found in Configuration
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result checkTriggerReferences() const;
    
protected:
    /**
    * Polymorphic clone method
    */
    virtual MapTriggerBase* clone() const = 0;

private:    
    /**
    * creates a polymorphic trigger instance from a trigger-dom element
    * @param [in] oConfig The configuration
    * @param [in] dom_element The dom element from which to import
    * @param [out] destination The Trigger object to fill
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    static a_util::result::Result createFromDOM(MapConfiguration* config, const a_util::xml::DOMElement& dom_element,
        MapTriggerBase*& destination);    

    /**
    * Export trigger to a XML dom element
    * 
    * @param [in] dom_element The dom element from which to import
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;

    /** 
    * Change name for source signal
    **/
    a_util::result::Result setSourceDependency(const std::string& new_name);

protected:
    /// @cond nodoc
    friend class MapConfiguration;
    friend class MapTarget;
    MapConfiguration* _config;
    bool _is_valid;
    /// @endcond
};

/// implementation of a periodic trigger in the configuration api
class MapPeriodicTrigger : public MapTriggerBase
{
public:
    /**
    * CTOR
    * @param [in] oConfig The configuration
    */
    MapPeriodicTrigger(MapConfiguration* config);

    /**
    * Returns the period of the trigger in ms
    */
    double getPeriod() const;

    /** 
    * Set the period
    * @param [in] period The period as integer
    * @param [in] unit The unit ("us", "ms" or "s")
    **/
    a_util::result::Result setPeriod(const std::string& period, 
const std::string& unit);

    /**
    * Polymorphic comparison method (impl. from MapTriggerBase)
    * @param [in] other The other instance
    * @returns True if both are equal
    */
    bool isEqual(const MapTriggerBase& other) const;

protected: 
    // implements MapTriggerBase
    MapTriggerBase* clone() const;

private:
    /**
    * creates a trigger instance from a trigger-dom element
    * @param [in] dom_element The dom element from which to import
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDom(const a_util::xml::DOMElement& dom_element);

    /**
    * Export trigger to a XML dom element
    * 
    * @param [in] dom_element The dom element from which to import
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;

private:
    /// @cond nodoc
    friend class MapTriggerBase;
    double _period;
    /// @endcond
};

/// implementation of a signal trigger in the configuration api
class MapSignalTrigger : public MapTriggerBase
{
public:
    /**
    * CTOR
    * @param [in] oConfig The configuration
    */
    MapSignalTrigger(MapConfiguration* config);

    /**
    * Overrides MapTriggerBase
    */
    virtual std::string getSourceDependency() const;

    /**
    * Returns the source signal
    */
    const std::string& getVariable() const;

    /** 
    * Set the source signal
    * @param [in] signal_name The source signal name
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    a_util::result::Result setVariable(const std::string& signal_name);

    /**
    * Polymorphic comparison method (impl. from MapTriggerBase)
    * @param [in] other The other instance
    * @returns True if both are equal
    */
    bool isEqual(const MapTriggerBase& other) const;

protected: 
    // implements MapTriggerBase
    MapTriggerBase* clone() const;

private:
    /**
    * creates a trigger instance from a trigger-dom element
    * @param [in] dom_element The dom element from which to import
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDom(const a_util::xml::DOMElement& dom_element);

    /**
    * Export trigger to a XML dom element
    * 
    * @param [in] dom_element The dom element from which to import
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;

    /** 
    * Set the source signal
    * @param [in] signal_name The source signal name
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    a_util::result::Result setVariableNoTypeCheck(const std::string& signal_name);

private:
    /// @cond nodoc
    friend class MapConfiguration;
    friend class MapTriggerBase;
    std::string _variable;
    bool _is_valid;
    /// @endcond
};

/// implementation of a data trigger in the configuration api
class MapDataTrigger : public MapTriggerBase
{
public:
    /**
    * CTOR
    * @param [in] oConfig The configuration
    */
    MapDataTrigger(MapConfiguration* config);

    /**
    * Overrides MapTriggerBase
    */
    virtual std::string getSourceDependency() const;

    /**
    * Returns the variable to compare
    */
    const std::string& getVariable() const;

    /**
    * Returns the source signal
    */
    const std::string& getSource() const;

    /**
    * Returns the operator for the comparison
    */
    const std::string& getOperator() const;

    /**
    * Returns the value to compare to
    */
    double getValue() const;

    /** 
    * Set the comparison
    * @param [in] source_element_path The path to the element ([SourceSignal].[Element])
    * @param [in] comp_operator The operator for the comparison
    * @param [in] value The value to compare to
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval ERR_INVALID_TYPE Definition is inconsistent with ddl
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    a_util::result::Result setComparison(const std::string& source_element_path, 
            const std::string& comp_operator, 
            const std::string& value);

    

    /**
    * Polymorphic comparison method (impl. from MapTriggerBase)
    * @param [in] other The other instance
    * @returns True if both are equal
    */
    bool isEqual(const MapTriggerBase& other) const;

protected: // implements MapTriggerBase
    MapTriggerBase* clone() const;

private:
    /**
    * creates a trigger instance from a trigger-dom element
    * @param [in] trigger_element The dom element from which to import
    * @param [out] lstErrors The error list for debug
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDom(const a_util::xml::DOMElement& trigger_element);

    /**
    * Export trigger to a XML dom element
    * 
    * @param [in] dom_element The dom element from which to import
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement& dom_element) const;

    /** 
    * Set the comparison
    * @param [in] source_element_path The path to the element ([SourceSignal].[Element])
    * @param [in] comp_operator The operator for the comparison
    * @param [in] value The value to compare to
    * @retval ERR_INVALID_ARG  Missing attribute
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    a_util::result::Result setComparisonNoTypeCheck(const std::string& source_element_path, 
            const std::string& comp_operator, 
            const std::string& value);

    /** 
    * Set the source
    * @param [in] source The source signal
    * @retval a_util::result::SUCCESS      Everything went fine
    **/
    a_util::result::Result setSourceNoTypeCheck(const std::string& source);

private:
    /// @cond nodoc
    friend class MapConfiguration;
    friend class MapTriggerBase;
    std::string _variable;
    std::string _source;
    std::string _operator;
    double _value;    
    /// @endcond
};

/// Public composite types used in the mapping::oo namespace
typedef std::vector<MapTriggerBase*> MapTriggerList;

} // namespace oo
} // namespace mapping

#endif // HEADER_MAP_TRIGGER_H
