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

#ifndef DDL_CONTAINER_H_INCLUDED
#define DDL_CONTAINER_H_INCLUDED

#include "ddl_common.h"
#include "ddl_intf.h"

namespace ddl
{
    class DDLUnit;
    class DDLBaseunit;
    class DDLPrefix;
    class DDLDataType;
    class DDLEnum;
    class DDLComplex;
    class DDLStream;
    class DDLStreamMetaType;

/**
 * Utility class that stores DDL entities
 */
template <typename T>
class DDLContainerNoClone
{
    private:
        std::vector<T*> _pointers;
        bool _sorted;

        #if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
            static T* _empty_dummy;
        #endif

    public:

        ///iterator for DDL container
        typedef T** iterator; // NOLINT
        
        ///const iterator for DDL container
        typedef T* const * const_iterator; // NOLINT

    public:

        /**
         * Constructor
         * @param [in] sorted whether the items should be sorted by name (this improves access times)
         */
        DDLContainerNoClone(bool sorted = true);

        /**
         * @brief insert
         * @param [in] elem The element to insert.
         * @param [in] pos  Position to add the element
         * @return void
         */
        void insert(T* elem, int pos = -1);

        /**
         * Finds an element by name
         * @param [in] name
         * @return pointer to the element or NULL
         */
        T* find(const std::string& name);

        /**
         * Finds an element by name
         * @param [in] name
         * @return iterator to the element or end()
         */
        iterator findIt(const std::string& name);

        /**
         * Finds an element by name
         * @param [in] name
         * @return pointer to the element or NULL
         */
        const T* find(const std::string& name) const;

        /**
         * Copies the pointers from the other container.
         * @param [in] other Thge other container
         * @return void
         */
        void copyFromRef(DDLContainerNoClone &other);

        /**
         * Deletes all stored items (by calling delete on the pointers)
         */
        void deleteAll();

        /**
         * sort the items by name.
         */
        void sort();

        /**
         * Returns Whether or not the container is sorted by name or not.
         * @return whether or not the container is sorted by name or not.
         */
        bool isSorted() const;

        /**
         * Returns an iterator to the first element.
         * @return an iterator to the first element.
         */
        iterator begin();

        /**
         * Returns an iterator to the first element.
         * @return an iterator to the first element.
         */
        const_iterator begin() const;

        /**
         * Returns an iterator to the element after the last element.
         * @return an iterator to the element after the last element.
         */
        iterator end();

        /**
         * Returns an iterator to the element after the last element.
         * @return an iterator to the element after the last element.
         */
        const_iterator end() const;

        /**
         * clears the container (does not destroy the elements)
         */
        void clear();

        /**
         * removes an element from the container
         * @param [in] pos The element which should be removed.
         * @return iterator to the element after the erased element.
         */
        iterator erase(iterator pos);

        /**
         * @brief removes a sequence of elements from the container
         * @param pos_first the first element to erase.
         * @param pos_last the first one that should not be erased.
         * @return iterator to the element after the erased elements.
         */
        iterator erase(iterator pos_first, iterator pos_last);

        /**
         * Returns whether the container is empty or not.
         * @return whether the container is empty or not.
         */
        bool empty() const;

        /**
         * Returns the size of the container.
         * @return the size of the container.
         */
        unsigned int size() const; // for compatibility reasons

        /**
         * random access operator.
         * @param [in] pos The index of the element.
         * @return The element.
         */
        T*& operator[] (unsigned int pos);

        /**
         * random access operator.
         * @param [in] pos The index of the element.
         * @return The element.
         */
        T* const& operator[] (unsigned int pos) const;

        /**
         * random access method.
         * @param [in] pos The index of the element.
         * @return The element.
         */
        T*& at(unsigned int pos);

        /**
         * random access method.
         * @param [in] pos The index of the element.
         * @return The element.
         */
        T* const& at(unsigned int pos) const;

    private:
        /**
        * Finds an sorted element by name
        * @param [in] name
        * @return pointer to the element or NULL
        */
        T* find_sorted(const std::string& name);

        /**
        * Finds an sorted element by name
        * @param [in] name
        * @return pointer to the element or NULL
        */
        const T* find_sorted(const std::string& name) const;


        /**
        * Finds an unsorted element by name
        * @param [in] name
        * @return pointer to the element or NULL
        */
        T* find_unsorted(const std::string& name);

        /**
        * Finds an unsorted element by name
        * @param [in] name
        * @return pointer to the element or NULL
        */
        const T* find_unsorted(const std::string& name) const;
};

/**
 * Utility class to store DDL elements that can be cloned.
 */
template <typename T>
class DDLContainer: public DDLContainerNoClone<T>
{
    public:
        /**
         * Constructor
         * @param [in] sorted Whether the container shall be sorted by name or not.
         */
        DDLContainer(bool sorted = true);

        /**
         * Clones all elements of a given container.
         * @param [in] other the container that should be cloned.
         * @return void
         */
        void cloneFrom(const DDLContainer& other);
};

/**
 * Container type of basic DDL objects
 */
typedef DDLContainerNoClone<IDDL> DDLVec;

/**
 * Container type of DDLUnit objects
 */
typedef DDLContainer<DDLUnit> DDLUnitVec;
/**
 * Container type of DDLBaseunit objects
 */
typedef DDLContainer<DDLBaseunit> DDLBaseunitVec;
/**
 * Container type of DDLPrefix objects
 */
typedef DDLContainer<DDLPrefix> DDLPrefixVec;
/**
 * Container type of DDLDataType objects
 */
typedef DDLContainer<DDLDataType> DDLDTVec;
/**
 * Container type of DDLEnum objects
 */
typedef DDLContainer<DDLEnum> DDLEnumVec;
/**
 * Container type of DDLComplex objects
 */
typedef DDLContainer<DDLComplex> DDLComplexVec;
/**
 * Container type of DDLStream objects
 */
typedef DDLContainer<DDLStream> DDLStreamVec;
/**
 * Container type of DDLStreamMetaType objects
 */
typedef DDLContainer<DDLStreamMetaType> DDLStreamMetaTypeVec;

/**
 * Iterator type for DDLUnitVec
 */
typedef DDLUnitVec::iterator DDLUnitIt;
/**
 * Iterator type for DDLBaseunitVec
 */
typedef DDLBaseunitVec::iterator DDLBaseunitIt;
/**
 * Iterator type for DDLPrefixVec
 */
typedef DDLPrefixVec::iterator DDLPrefixIt;
/**
 * Iterator type for DDLDTVec
 */
typedef DDLDTVec::iterator DDLDTIt;
/**
 * Iterator type for tDDLConstVec
 */
typedef DDLEnumVec::iterator DDLEnumIt;
/**
 * Iterator type for DDLComplexVec
 */
typedef DDLComplexVec::iterator DDLComplexIt;
/**
 * Iterator type for DDLStreamVec
 */
typedef DDLStreamVec::iterator DDLStreamIt;

/**
 * Associative container type for DDLStream objects
 */
typedef std::map<std::string, DDLStream*> DDLStreamMap;

/**
 * Iterator type for DDLStreamMap
 */
typedef DDLStreamMap::iterator DDLStreamMapIt;

}

#endif

