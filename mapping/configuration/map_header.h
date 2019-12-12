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

#ifndef HEADER_MAP_HEADER_H
#define HEADER_MAP_HEADER_H

#include "a_util/result.h"
#include "a_util/xml.h"

namespace mapping
{
namespace oo
{

    typedef std::vector<std::string> MapErrorList;
    
/**
 * MapHeader contains all information from the header-Tag of a mapping configuration
 */
class MapHeader
{
public:    
    /**
    * CTOR
    */
    MapHeader();

    /**
    * Returns the description from the <description> header tag or an empty string
    */
    const std::string& getDescription() const;

    /**
    * Returns the list of dll paths from the <ddl> header tag or an empty string
    */
    const std::string& getDllPaths() const;
  
    /**
    * Returns the version from the language_version-header tag or an empty string
    */
    const std::string& getLanguageVersion() const;
    
    /**
    * Returns the author from the author-header tag or an empty string
    */
    const std::string& getAuthor() const;
    
    /**
    * Returns the date from the date_creation-header tag or an empty string
    */
    const std::string& getCreationDate() const;
    
    /**
    * Returns the date from the date_change-header tag or an empty string
    */
    const std::string& getModificationDate() const;
    
private:
    /**
    * Resets the instance to its initial state
    */
    void reset();

    /**
    * Initializes the header from a header-dom element
    * @param [in] dom_element The dom element from which to import
    * @param [out] errors The error list for debug
    * @retval ERR_INVALID_ARG  Field missing
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result loadFromDOM(const a_util::xml::DOMElement& header, MapErrorList& errors);

    /**
    * Export header to a XML dom element
    * 
    * @param [out] dom_element The dom element to be written
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result writeToDOM(a_util::xml::DOMElement dom_element);

    /**
    * Set description
    * @param [in] description The description
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result setDescription(const std::string& description);

    /**
    * Set ddl paths
    * @param [in] ddl_paths The string containing ddl paths
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result setDdlPaths(const std::string& ddl_paths);
    
private:
    /// @cond nodoc
    friend class MapConfiguration;
    std::string _lang_version;
    std::string _author;
    std::string _desc;
    std::string _ddl;
    std::string _creation_date;
    std::string _mod_date;
    /// @endcond
};


} // namespace oo
} // namespace mapping

#endif // HEADER_MAP_HEADER_H
