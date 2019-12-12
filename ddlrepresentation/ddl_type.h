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

#ifndef DDL_TYPE_H_INCLUDED
#define DDL_TYPE_H_INCLUDED

#include "ddl_common.h"
#include "ddl_intf.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-49, ERR_OUT_OF_RANGE)

    /**
     * The abstract helper class for representation classes for DDL descriptions.
     */
    class DDL : public IDDL
    {
    public:

        virtual bool isInitialized() const;

        virtual bool isPredefined() const;

        virtual bool isOverwriteable() const;

        virtual int getCreationLevel() const;

        /**
         * Functor for use with \c std::transform() to delete all objects
         * where the elements of a vector point at.
         * @param[in] obj - Pointer to the object to delete
         * @return Pointer to the predefined object that was not deleted (see remarks).
         * @remarks Objects which are declarated as predefined are not deleted.
         */
        template<typename T>
        static T* deleteChild(T *obj)
        {
            if (NULL != obj)
            {
                if (obj->isPredefined())
                {
                    return obj;
                }
                else
                {
                    delete obj;
                }
            }
            return NULL;
        }

        /**
         * Method moves element within the list. 
         * @param[in] obj - Pointer to the list object
         * @param[in] from - Position of element in the list
         * @param[in] to - New position of element in the list
         * @return ERR_OUT_OF_RANGE - from and to parameters are out of range.
         * @return a_util::result::SUCCESS
         */
        template<typename T>
        static a_util::result::Result moveChild(T *obj, const int from, const int to)
        {
            if (NULL == obj) { return a_util::result::Result(-4); } //ERR_POINTER for hackers...

            if (-1 == from || -1 == to ||
                (int)obj->size() < from || (int)obj->size() < to)
            {
                return ERR_OUT_OF_RANGE;
            }

            if (from != to)
            {
                if (from < to)
                {
                    std::rotate(obj->begin() + from,
                        obj->begin() + from + 1,
                        obj->begin() + to + 1);
                }
                else
                {
                    std::rotate(obj->begin() + to,
                        obj->begin() + from,
                        obj->begin() + from + 1);
                }
            }
            return a_util::result::SUCCESS;
        }

        /**
         * Functor for use with \c std::transform() to clone the objects
         * where the elements of a vector point at.
         * @tparam T - Representation object type (e.g. \c Prefix)
         * @param[in] obj - Pointer to the object to clone
         * @return Pointer to cloned instance of T or \c NULL if there was
         * no original instance (obj == NULL).
         */
        template<typename T>
        static T* clone(T* obj)
        {
            if (NULL != obj)
            {
                if (obj->isPredefined())
                {
                    return ref<T>(obj);
                }
                return new T(*obj);
            }
            return NULL;
        }

        /**
         * Functor for use with \c std::transform() to ref the objects
         * where the elements of a vector point at, this is for the Always_there.
         * @tparam T - Representation object type (e.g. \c Prefix)
         * @param[in] obj - Pointer to the object to clone
         * @return Pointer to ref instance of T or \c NULL if there was
         * no original instance (obj == NULL).
         */
        template<typename T>
        static T* ref(T* obj)
        {
            if (NULL != obj)
            {
                return obj;
            }
            return NULL;
        }

        /**
         * Predicate to compare 2 DDL representation objects (for use with
         * \c std::unique()).
         * @param[in] lhs - Pointer to the object on left-hand side
         * @param[in] rhs - Pointer to the object on right-hand side
         * @retval true if the objects have the same name
         * @retval false else
         */
        static bool isEqual(IDDL* lhs, IDDL* rhs);

        /**
         * sort predicate to compare to 2 DDL representation objects (for use
         * with \c std::sort()).
         * @param[in] lhs - Pointer to the object on left-hand side
         * @param[in] rhs - Pointer to the object on right-hand side
         * @retval true if the left argument goes before the right one.
         * @retval false else
         */
        static bool isSorted(IDDL* lhs, IDDL* rhs);
    };

    /// functional pattern to use std find algorithms
    template<typename T = IDDL>
    struct DDLCompareFunctor
    {
        /// the name of the pattern
        const std::string& pattern;

        /**
         * CTR
         *
         * @param [in] pattern   the name of the pattern
         */
        DDLCompareFunctor(const std::string& pattern): pattern(pattern)
        {
        }

        /**
         * compare function for pattern
         *
         * @param [in] entry   pointer to the DDL object
         *
         * @return true if equal, false otherwise
         */
        bool operator()(const T* entry)
        {
            if (NULL == entry)
            {
                return false;
            }
            return pattern == entry->getName();
        }
    };

}   // namespace ddl

#endif  // DDL_TYPE_H_INCLUDED
