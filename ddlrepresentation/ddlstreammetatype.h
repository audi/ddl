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

#ifndef DDL_STREAM_META_TYPE_H_INCLUDED
#define DDL_STREAM_META_TYPE_H_INCLUDED

#include "ddl_common.h"
#include "ddl_type.h"
#include "ddlproperty.h"

namespace ddl
{
    typedef std::vector<DDLProperty*> DDLPropertyVec;

    /**
     * Representation of a stream inside a DDL description.
     */
    class DDLStreamMetaType : public DDL
    {
    public:
        /**
         * Default CTOR
         */
        DDLStreamMetaType() = default;

        /**
         * CTOR
         * @param[in] name - Name of the stream meta type
         * @param[in] version - Version of the stream meta type
         * @param[in] properties - The properties.
         * @param[in] parent - the stream meta type parent (optional)
         */
        DDLStreamMetaType(const std::string& name,
                           const std::string& version,
                           const DDLPropertyVec& properties = DDLPropertyVec(),
                           const DDLStreamMetaType* parent = NULL);

        /**
        * Copy CTOR
        * @param[in] other - Reference to stream meta type object to copy
        */
        DDLStreamMetaType(const DDLStreamMetaType& other);

        /**
         * Assignment operator (either copies or moves)
         * @param[in] other DDL stream meta type object to copy from
         * @return @c *this
         */
        DDLStreamMetaType& operator=(DDLStreamMetaType other);

        /**
         * Move CTOR
         * @param[in,out] other Stream meta type object to move from - empty but valid when finished
         */
        DDLStreamMetaType(DDLStreamMetaType&& other);

        /// DTOR
        virtual ~DDLStreamMetaType();

        a_util::result::Result accept(IDDLVisitor *visitor) const;
        a_util::result::Result accept(IDDLChangeVisitor *visitor);

        bool isInitialized() const;

        int getCreationLevel() const;

        /**
         * Getter for the name.
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Setter for the name.
         * @param[in] name - Name of the stream
         * @return void
         */
        void setName(const std::string& name);

        /**
         * Getter for the version.
         * @return the version
         */
        const std::string& getVersion() const;

        /**
         * Setter for the contained structs.
         * @param[in] structs - Vector of structures (complex data types)
         * @return void
         */
        void cloneProperties(const DDLPropertyVec& structs);

        /**
         * Getter for the contained properties.
         * @return the vector of properties
         */
        DDLPropertyVec& getProperties();

        /**
         * Getter for the contained properties.
         * @return the vector of properties
         */
        const DDLPropertyVec& getProperties() const;

        /**
         * Pointer to the parent stream meta type.
         * @return Pointer to the parent stream meta type.
         */
        const DDLStreamMetaType* getParentObject() const;

        /**
         * Getter for the name of the parent.
         * @return The name of the parent.
         */
        const std::string& getParent() const;

        /**
         * Add swap functionality, also enabling the copy-swap-idiom
         * @param[in,out] lhs Left-hand side ddl type
         * @param[in,out] rhs Right-hand side ddl type
         */
        friend void swap(DDLStreamMetaType& lhs, DDLStreamMetaType& rhs) noexcept;

    private:
        std::string _name;
        std::string _version;
        DDLPropertyVec _properties;
        const DDLStreamMetaType* _parent;
        bool _init_flag;
    };

}   // namespace ddl

#endif // DDL_STREAM_META_TYPE_H_INCLUDED
