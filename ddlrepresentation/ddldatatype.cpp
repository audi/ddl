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

#include "ddldatatype.h"

namespace ddl
{
   //define all needed error types and values locally
   _MAKE_RESULT(-4, ERR_POINTER);

   DDLDataType::DDLDataType() :
       _language_version{DDLVersion::ddl_version_invalid},
       _name{},
       _num_bits{},
       _byte_size{},
       _aligned_size{},
       _unit{},
       _description{},
       _array_size{},
       _array_size_source{},
       _alignment{},
       _init_flag{},
       _level{1},
       _min_valid{},
       _min_val{},
       _max_valid{},
       _max_val{},
       _default_valid{},
       _default_val{}
   {
   }

   DDLDataType::DDLDataType(const std::string& name,
                              unsigned int const num_bits,
                              IDDLUnit* unit,
                              const std::string& description,
                              DDLAlignment::AlignmentType const alignment,
                              unsigned int const array_size,
                              int const creation_level,
                              const std::string& array_size_source,
                              bool const min_valid,
                              const std::string& min_value,
                              bool const max_valid,
                              const std::string& max_value) :
       _language_version(DDLVersion::ddl_version_invalid),
       _name(name),
       _num_bits(num_bits),
       _unit(unit),
       _description(description),
       _array_size(array_size),
       _array_size_source(array_size_source),
       _alignment(alignment),
       _init_flag(true),
       _level(creation_level),
       _min_valid(min_valid),
       _min_val(min_value),
       _max_valid(max_valid),
       _max_val(max_value)
   {
   }

   a_util::result::Result DDLDataType::accept(IDDLVisitor *visitor) const
   {
       return visitor->visit(this);
   }

   a_util::result::Result DDLDataType::accept(IDDLChangeVisitor *visitor)
   {
       return visitor->visit(this);
   }

   const std::string& DDLDataType::getName() const
   {
       return _name;
   }

   void DDLDataType::setName(const std::string& name)
   {
       _name = name;
   }

   bool DDLDataType::isInitialized() const
   {
       return _init_flag;
   }

   int DDLDataType::getCreationLevel() const
   {
       return _level;
   }

   a_util::result::Result DDLDataType::create(const std::string& name,
                                unsigned int const num_bits,
                                IDDLUnit* unit,
                                const std::string& description,
                                DDLAlignment::AlignmentType const alignment,
                                unsigned int const array_size,
                                int const creation_level,
                                const std::string& array_size_source,
                                bool const min_valid,
                                const std::string& min_value,
                                bool const max_valid,
                                const std::string& max_value)
   {
       if (!unit) { return ERR_POINTER; }
       _name = name;
       _num_bits = num_bits;
       _unit = unit;
       _description = description;
       _array_size = array_size;
       _init_flag = true;
       _level = creation_level;
       _alignment = alignment;
       _array_size_source = array_size_source;
       _language_version = DDLVersion::ddl_version_invalid;
       _min_valid = min_valid;
       _min_val = min_value;
       _max_valid = max_valid;
       _max_val = max_value;
       return a_util::result::SUCCESS;
   }

   unsigned int DDLDataType::getNumBits() const
   {
       return _num_bits;
   }

   void DDLDataType::setNumBits(unsigned int const num_bits)
   {
       _num_bits = num_bits;
   }

   void DDLDataType::setDescription(const std::string& description)
   {
       _description = description;
   }

   std::string DDLDataType::getDescription() const
   {
       return _description;
   }

   void DDLDataType::setArraysize(unsigned int const array_size)
   {
       if (array_size > 0)
       {
           _array_size = array_size;
       }
   }

   unsigned int DDLDataType::getArraysize() const
   {
       return _array_size;
   }

   void DDLDataType::setUnit(IDDLUnit* unit)
   {
       _unit = unit;
   }

   std::string DDLDataType::getUnit() const
   {
       if (NULL == _unit)
       {
           return a_util::strings::empty_string;
       }
       return _unit->getName();
   }

   const IDDLUnit* DDLDataType::getUnitObject() const
   {
       if (!_init_flag)
       {
           return NULL;
       }
       return _unit;
   }

  IDDLUnit* DDLDataType::getUnitObject()
   {
       if (!_init_flag)
       {
           return NULL;
       }
       return _unit;
   }

   bool DDLDataType::isPredefined() const
   {
       return _level == -1;   // cMediaManager::DL_AlwaysThere
   }

   bool DDLDataType::isOverwriteable() const
   {
       return _level > 0;
   }

   void DDLDataType::setAlignment(DDLAlignment::AlignmentType const alignment)
   {
       _alignment = alignment;
   }

   DDLAlignment::AlignmentType DDLDataType::getAlignment() const
   {
       return _alignment;
   }

   std::string DDLDataType::getArraySizeSource() const
   {
       return _array_size_source;
   }

   void DDLDataType::setArraySizeSource( const std::string& array_size_source )
   {
       _array_size_source = array_size_source;
   }

   bool DDLDataType::isDynamic() const
   {
       if (getArraysize() == 0 ||
       (!getArraySizeSource().empty() && !a_util::strings::isInt64(getArraySizeSource())))
       {
           return true;
       }

       return false;
   }

   DDLVersion DDLDataType::getDDLVersion() const
   {
       return _language_version;
   }

   a_util::result::Result DDLDataType::setDDLVersion(DDLVersion& language_version)
   {
       _language_version = language_version;
       return a_util::result::SUCCESS;
   }

   const std::string & DDLDataType::getMinValue () const
   {
       return _min_val;
   }

   a_util::result::Result DDLDataType::setMinValue (const std::string& min_value)
   {
       // TODO validate new min value
       _min_val = min_value;
       return a_util::result::SUCCESS;
   }

   const std::string & DDLDataType::getMaxValue () const
   {
       return _max_val;
   }

   a_util::result::Result DDLDataType::setMaxValue (const std::string& max_value)
   {
       // TODO validate new max value
       _max_val = max_value;
       return a_util::result::SUCCESS;
   }

   void DDLDataType::setMinValidity (bool const min_valid)
   {
       _min_valid = min_valid;
   }

   bool DDLDataType::isMinValid () const
   {
       return _min_valid;
   }

   void DDLDataType::setMaxValidity (bool const max_valid)
   {
       _max_valid = max_valid;
   }

   bool DDLDataType::isMaxValid () const
   {
       return _max_valid;
   }

}   // namespace ddl
