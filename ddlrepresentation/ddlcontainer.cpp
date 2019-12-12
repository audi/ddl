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

#include "ddlcontainer.h"
#include "ddl_type.h"

#include "ddlunit.h"
#include "ddlbaseunit.h"
#include "ddlprefix.h"
#include "ddldatatype.h"
#include "ddlenum.h"
#include "ddlcomplex.h"
#include "ddlstream.h"
#include "ddlstreammetatype.h"

namespace ddl
{

template <typename T>
struct LessCompare
{
    bool operator()(const T* obj1, const T* obj2)
    {
      return obj1->getName() < obj2->getName();
    }

    bool operator()(const T* obj, const std::string& name)
    {
      return obj->getName() < name;
    }

    bool operator()(const std::string& name, const T* obj)
    {
      return name < obj->getName();
    }
};

template <typename T>
DDLContainerNoClone<T>::DDLContainerNoClone(bool sorted): _sorted(sorted)
{
}

template <typename T>
void DDLContainerNoClone<T>::insert(T* elem, int pos)
{
    if (_sorted)
    {
        _pointers.insert(std::lower_bound(_pointers.begin(), _pointers.end(), elem, LessCompare<T>()), elem);
    }
    else
    {
        const typename std::vector<T*>::size_type sz_pos =
            static_cast<typename std::vector<T*>::size_type>(pos);
        if (0 <= sz_pos && sz_pos < _pointers.size())
        {
            _pointers.insert(_pointers.begin() + sz_pos, elem);
        }
        else
        {
            _pointers.push_back(elem);
        }
    }
}

template <typename T>
typename DDLContainerNoClone<T>::iterator DDLContainerNoClone<T>::findIt(const std::string& name)
{
  if (_sorted)
  {
      iterator it = std::lower_bound(this->begin(), this->end(), name, LessCompare<T>());
      if (this->end() == it || name != (*it)->getName())
      {
          // not found
          return this->end();
      }
      return it;
  }
  else
  {
      iterator it = std::find_if(this->begin(), this->end(), DDLCompareFunctor<>(name));
      if (this->end() == it)
      {
          // not found
          return this->end();
      }
      return it;
  }
}

template <typename T>
const T* DDLContainerNoClone<T>::find(const std::string& name) const
{
    if (_sorted)
    {
        return find_sorted(name);
    }
    return find_unsorted(name);
}

template <typename T>
T* DDLContainerNoClone<T>::find(const std::string& name)
{
    if (_sorted)
    {
        return find_sorted(name);
    }
    return find_unsorted(name);
}

template <typename T>
const T* DDLContainerNoClone<T>::find_unsorted(const std::string& name) const
{

    const_iterator it = std::find_if(this->begin(), this->end(), DDLCompareFunctor<>(name));
    if (this->end() == it)
    {
        // not found
        return NULL;
    }
    return *it;
}

template <typename T>
const T* DDLContainerNoClone<T>::find_sorted(const std::string& name) const
{

    const_iterator it = std::lower_bound(this->begin(), this->end(), name, LessCompare<T>());
    if (this->end() == it || name != (*it)->getName())
    {
        // not found
        return find_unsorted(name);
    }
    return *it;
}

template <typename T>
T* DDLContainerNoClone<T>::find_unsorted(const std::string& name)
{

    iterator it = std::find_if(this->begin(), this->end(), DDLCompareFunctor<>(name));
    if (end() == it)
    {
        // not found
        return NULL;
    }
    return *it;
}

template <typename T>
T* DDLContainerNoClone<T>::find_sorted(const std::string& name)
{
    iterator it = std::lower_bound(this->begin(), this->end(), name, LessCompare<T>());
    if (end() == it || name != (*it)->getName())
    {
        // not found
        auto obj = find_unsorted(name);
        if (obj != nullptr)
        {
            sort();
        }
        return obj;
    }
    return *it;
}

template <typename T>
void DDLContainerNoClone<T>::copyFromRef(DDLContainerNoClone& other)
{
  _pointers.resize(other.size());
  std::copy(other.begin(), other.end(), _pointers.begin());
  if (_sorted && !other._sorted)
  {
      sort();
  }
}



template <typename T>
void DDLContainerNoClone<T>::deleteAll()
{
  for (iterator it = begin(); it != end(); ++it)
  {
      DDL::deleteChild<T>(*it);
  }
  clear();
}

 template <typename T>
void DDLContainerNoClone<T>::sort()
{
  std::sort(_pointers.begin(), _pointers.end(), LessCompare<T>());
}

template <typename T>
bool DDLContainerNoClone<T>::isSorted() const
{
  return _sorted;
}

// Note on our iterators:
// we do not want to expose the std::vector and std::vector::iterator implementation to the user
// so we use the simplest kind of iterator, a pointer.
template <typename T>
typename DDLContainerNoClone<T>::iterator DDLContainerNoClone<T>::begin()
{
#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
    // have a look at http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#464 and
    // http://stackoverflow.com/questions/3829788/using-operator-on-empty-stdvector
    // to find out the reason for this special implementation (VS2008 and lower)
    if (_pointers.empty())
    {
        return &_empty_dummy;
    }
    else
    {
        return &*_pointers.begin();
    }
#else
    return &*_pointers.begin();
#endif
}

template <typename T>
typename DDLContainerNoClone<T>::const_iterator DDLContainerNoClone<T>::begin() const
{
#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
    // have a look at http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#464 and
    // http://stackoverflow.com/questions/3829788/using-operator-on-empty-stdvector
    // to find out the reason for this special implementation (VS2008 and lower)
    if (_pointers.empty())
    {
        return &_empty_dummy;
    }
    else
    {
        return &*_pointers.begin();
    }
#else
    return &*_pointers.begin();
#endif
}

template <typename T>
typename DDLContainerNoClone<T>::iterator DDLContainerNoClone<T>::end()
{
#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
    // have a look at http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#464 and
    // http://stackoverflow.com/questions/3829788/using-operator-on-empty-stdvector
    // to find out the reason for this special implementation (VS2008 and lower)
    if (_pointers.empty())
    {
        return &_empty_dummy;
    }
    else
    {
        return &*_pointers.begin() + _pointers.size();
    }
#else
    return &*_pointers.end();
#endif
}

template <typename T>
typename DDLContainerNoClone<T>::const_iterator DDLContainerNoClone<T>::end() const
{
#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
    // have a look at http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#464 and
    // http://stackoverflow.com/questions/3829788/using-operator-on-empty-stdvector
    // to find out the reason for this special implementation (VS2008 and lower)
    if (_pointers.empty())
    {
        return &_empty_dummy;
    }
    else
    {
        return &*_pointers.begin() + _pointers.size();
    }
#else
    return &*_pointers.end();
#endif
}

template <typename T>
void DDLContainerNoClone<T>::clear()
{
    _pointers.clear();
}

template <typename T>
typename DDLContainerNoClone<T>::iterator DDLContainerNoClone<T>::erase(iterator it)
{
    typename std::vector<T*>::iterator it_helper = _pointers.erase(_pointers.begin() + (it - begin()));
#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
    // have a look at http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#464 and
    // http://stackoverflow.com/questions/3829788/using-operator-on-empty-stdvector
    // to find out the reason for this special implementation (VS2008 and lower)
    if (it_helper == _pointers.end())
    {
        return &_empty_dummy;
    }
    else
    {
        return &*it_helper;
    }
#else
    return &*it_helper;
#endif
}

template <typename T>
typename DDLContainerNoClone<T>::iterator DDLContainerNoClone<T>::erase(iterator pos_first, iterator pos_last)
{
    if (pos_first == pos_last)
    {
        return end();
    }

#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
    // have a look at http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#464 and
    // http://stackoverflow.com/questions/3829788/using-operator-on-empty-stdvector
    // to find out the reason for this special implementation (VS2008 and lower)

    // another bug in MSVS Debug http://connect.microsoft.com/VisualStudio/feedback/details/557029/visual-c-iterator-debugging-incorrectly-raises-assertion-on-correct-use-of-return-value-of-std-vector-erase
    // prevents us from using the erase(start, end) version.
    typename std::vector<T*>::iterator it_helper = _pointers.begin() + (pos_first - begin());
    for (uint64_t nCount = pos_last - pos_first; nCount > 0; --nCount)
    {
        it_helper = _pointers.erase(it_helper);
    }

    if (it_helper == _pointers.end())
    {
        return &_empty_dummy;
    }
    else
    {
        return &*it_helper;
    }
#else
    typename std::vector<T*>::iterator it_helper = _pointers.erase(_pointers.begin() + (pos_first - begin()),
                                                                      _pointers.begin() + (pos_last - begin()));
    return &*it_helper;
#endif
}

template <typename T>
bool DDLContainerNoClone<T>::empty() const
{
    return _pointers.empty();
}

template <typename T>
unsigned int DDLContainerNoClone<T>::size() const
{
    return static_cast<unsigned int>(_pointers.size());
}

template <typename T>
T*& DDLContainerNoClone<T>::operator[] (unsigned int pos)
{
    return _pointers[pos];
}

template <typename T>
T* const& DDLContainerNoClone<T>::operator[] (unsigned int pos) const
{
    return _pointers[pos];
}

template <typename T>
T*& DDLContainerNoClone<T>::at(unsigned int pos)
{
    return _pointers.at(pos);
}

template <typename T>
T* const& DDLContainerNoClone<T>::at(unsigned int pos) const
{
    return _pointers.at(pos);
}

template <typename T>
DDLContainer<T>::DDLContainer(bool sorted): DDLContainerNoClone<T>(sorted)
{
}

template <typename T>
void DDLContainer<T>::cloneFrom(const DDLContainer& other)
{
    this->clear();
    for (typename DDLContainer::const_iterator it = other.begin(); it != other.end(); ++it)
    {
        this->insert(DDL::clone<T>(*it));
    }
}

template class DDLContainerNoClone<IDDL>;
template class DDLContainerNoClone<DDLUnit>;
template class DDLContainerNoClone<DDLBaseunit>;
template class DDLContainerNoClone<DDLPrefix>;
template class DDLContainerNoClone<DDLDataType>;
template class DDLContainerNoClone<DDLEnum>;
template class DDLContainerNoClone<DDLComplex>;
template class DDLContainerNoClone<DDLStream>;
template class DDLContainerNoClone<DDLStreamMetaType>;

#if defined(WIN32) && ((_MSC_VER < 1600) || defined(_DEBUG))
IDDL* DDLContainerNoClone<IDDL>::_empty_dummy = NULL;
DDLUnit* DDLContainerNoClone<DDLUnit>::_empty_dummy = NULL;
DDLBaseunit* DDLContainerNoClone<DDLBaseunit>::_empty_dummy = NULL;
DDLPrefix* DDLContainerNoClone<DDLPrefix>::_empty_dummy = NULL;
DDLDataType* DDLContainerNoClone<DDLDataType>::_empty_dummy = NULL;
DDLEnum* DDLContainerNoClone<DDLEnum>::_empty_dummy = NULL;
DDLComplex* DDLContainerNoClone<DDLComplex>::_empty_dummy = NULL;
DDLStream* DDLContainerNoClone<DDLStream>::_empty_dummy = NULL;
DDLStreamMetaType* DDLContainerNoClone<DDLStreamMetaType>::_empty_dummy = NULL;
#endif

template class DDLContainer<DDLUnit>;
template class DDLContainer<DDLBaseunit>;
template class DDLContainer<DDLPrefix>;
template class DDLContainer<DDLDataType>;
template class DDLContainer<DDLEnum>;
template class DDLContainer<DDLComplex>;
template class DDLContainer<DDLStream>;
template class DDLContainer<DDLStreamMetaType>;

}
