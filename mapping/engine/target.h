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
 *
 * QNX support Copyright (c) 2019 by dSPACE GmbH, Paderborn, Germany. All Rights Reserved
 */


#ifndef TARGET_HEADER
#define TARGET_HEADER

#include <vector>
#include "a_util/result.h"

#include "codec/static_codec.h"

#include "element.h"
#include "source.h"
#include "mapping_environment_intf.h"
#include "mapping/configuration/map_target.h"
#include "mapping/configuration/map_source.h"

namespace mapping
{
namespace rt
{

/// Target represents a mapped target signal in the runtime api
class Target
{
public: // types

#if defined(__GNUC__) && (__GNUC__ == 5) && defined(__QNX__)
#pragma GCC diagnostic ignored "-Wattributes" // standard type attributes are ignored when used in templates
#endif

    /// Assignment container
    typedef std::vector<std::pair<std::string, TargetElement*> > Assignments;
    /// Trigger counter container
    typedef std::vector<std::pair<uint64_t, TargetElement*> > TriggerCounters;
    /// Constant container
    typedef std::vector<std::pair<std::string, TargetElement*> > Constants;
    /// Generic byte-buffer
    typedef std::vector<uint8_t> MemoryBuffer;

#if defined(__GNUC__) && (__GNUC__ == 5) && defined(__QNX__)
#pragma GCC diagnostic warning "-Wattributes" // standard type attributes are ignored when used in templates
#endif

public:
    /**
    * CTOR
    * @param [in] env The environment
    */
    Target(IMappingEnvironment& env);

    /**
    * DTOR
    */
    ~Target();

    /**
    * Creation method to fill the object with data.
    * @param[in] map_config - The Configuration instance
    * @param[in] oo::MapTarget - target representation from mapping configuration 
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error while filing the object
    */
    a_util::result::Result create(const oo::MapConfiguration& map_config, const oo::MapTarget& map_target,
        const std::string& target_description, SourceMap& sources);

    /**
    * Reset target Buffers
    * @param[in] map_config - The Configuration instance
    * @return error code
    */
    a_util::result::Result reset(const oo::MapConfiguration& map_config);

    /**
    * Getter for the target name
    * @return the target name
    */
    const std::string& getName() const;

    /**
    * Getter for the target type
    * @return the target type
    */
    const std::string& getType() const;

    /**
    * Getter for the element list
    * @return the element list
    */
    const TargetElementList& getElementList() const;    

    /**
    * Method to get buffer size
    *
    * @retval buffer size
    */
    size_t getSize() const;

    /**
    * Method to get a copy of the current target buffer
    *
    * @param [in] target_buffer Pointer referencing the target buffer
    * @param [in] target_buffer_size The size of the target buffer
    * @retval a_util::result::SUCCESS Everything went fine
    * @retval ERR_MEMORY Target buffer size not large enough
    */
    a_util::result::Result getCurrentBuffer(void* target_buffer, size_t target_buffer_size);

    
    /**
    * Method to get access to the current buffer without copying it
    *
    * @param [in] buffer Destination pointer
    * @param [out] target_buffer_size Destination parameter for the size of the buffer
    * @retval a_util::result::SUCCESS Everything went fine
    */
    a_util::result::Result getBufferRef(const void*& buffer, size_t& target_buffer_size);

    /**
    * Method to update all dynamic values that are to be updates during buffer access
    * (i.e. simulation time)
    */
    a_util::result::Result updateAccessFunctionValues();

    /**
    * Method to update all dynamic values that are to be updates during a trigger
    * (i.e. trigger counters)
    */
    a_util::result::Result updateTriggerFunctionValues();

private:
    /// @cond nodoc
    std::string _name;
    std::string _type;
    TargetElementList _target_elements;     
    std::vector<TargetElement*> _simulation_time_elements;
    TriggerCounters _counter_elements;
    Constants _constant_elements;
    uint64_t _counter;
    a_util::memory::unique_ptr<ddl::StaticCodec> _codec;
    MemoryBuffer _buffer;
    mutable a_util::concurrency::shared_mutex _buffer_mutex;
    IMappingEnvironment& _env;
    /// @nodoc
public:
    /// Lock the buffer for a source update
    inline void aquireWriteLock() const { _buffer_mutex.lock_shared(); }

    /// Unlock the buffer after a source update
    inline void releaseWriteLock() const { _buffer_mutex.unlock_shared(); }

    /// Lock the buffer for a buffer read
    inline void aquireReadLock() const { _buffer_mutex.lock(); }

    /// Unlock the buffer after a buffer read
    inline void releaseReadLock() const { _buffer_mutex.unlock(); }
};

/// Public composite types used in the mapping::rt namespace
typedef std::map<std::string, Target*> TargetMap;
typedef std::set<Target*> TargetSet;

} // namespace rt
} // namespace mapping
#endif //TARGET_HEADER
