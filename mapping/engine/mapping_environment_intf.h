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

#ifndef MAPPINGENVIRONMENT_HEADER
#define MAPPINGENVIRONMENT_HEADER

#include "a_util/result.h"

namespace mapping
{
namespace rt
{

/// Signal listener interface class
class ISignalListener
{
public:
    /// DTOR
    virtual ~ISignalListener();

    /**
    * \c onSampleReceived is to be called by the mapping environment
    * upon receiving a sample on a registered source signal.
    *
    * @param [in] data The data contained in the sample
    * @param [in] size The size of the data
    * @returns Standard result
    */
    virtual a_util::result::Result onSampleReceived(const void* data, size_t size) = 0;
};

/// Timer interface class
class IPeriodicListener
{
public:
    /// DTOR
    virtual ~IPeriodicListener();

    /**
    * \c onTimer is to be called by the mapping environment periodically.
    *
    * @param [in] now The current time
    * @returns Nothing
    */
    virtual void onTimer(timestamp_t now) = 0;
};

/// Mapping environment interface class
/// This in combination with ISignalListener forms the functionality that
/// the environment using the mapping package must provide.
class IMappingEnvironment
{
public:
    /// DTOR
    virtual ~IMappingEnvironment();

    /**
    * Method to register a source signal. \c registerSource is called for all
    * source signals inside a mapped target signal.
    *
    * @param [in] source_name The name of the source signal
    * @param [in] type_name The type of the source signal
    * @param [in] listener The signal listener instance
    * @param [in] handle The output parameter for the source handle
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    virtual a_util::result::Result registerSource(const char* source_name,
        const char* type_name, ISignalListener* listener, handle_t& handle) = 0;

    /**
    * Method to unregister a source signal. Used to unregister any source
    * signals upon unmapping of a target signal.
    *
    * @param [in] handle The handle of the source signal
    * @retval a_util::result::SUCCESS Everything went fine
    */
    virtual a_util::result::Result unregisterSource(handle_t handle) = 0;

    /**
    * Method to send a target signal. It is called upon
    *
    * @param [in] target The target handle
    * @param [in] data The pointer referencing the target buffer
    * @param [in] size The size of the target buffer
    * @param [in] time_stamp The timestamp of the target
    * @retval a_util::result::SUCCESS Everything went fine
    */
    virtual a_util::result::Result sendTarget(handle_t target, const void* data,
        size_t size, timestamp_t time_stamp) = 0;

    /**
    * \c targetMapped is invoked by the mapping engine upon the creation of a mapping.
    * The callback is supposed to give the mapping environment a chance to preallocate
    * any sample buffers to be used during runtime.
    *
    * @param [in] target_name The target name
    * @param [in] target_type The target type
    * @param [in] target The handle under which the target is refered to in \c sendTarget
    * @param [in] target_size The target type size (size of a sample in bytes)
    * @retval a_util::result::SUCCESS Everything went fine
    */
    virtual a_util::result::Result targetMapped(const char* target_name, const char* target_type,
        handle_t target, size_t target_size) = 0;
    
    /**
    * \c targetUnmapped is invoked by the mapping engine upon the destruction of a mapping.
    * The callback is supposed to give the mapping environment a chance to cleanup
    * any sample buffers associated with that target.
    *
    * @param [in] target_name The target name
    * @param [in] target The target handle
    * @retval a_util::result::SUCCESS Everything went fine
    */
    virtual a_util::result::Result targetUnmapped(const char* target_name, handle_t target) = 0;

    /**
    * Method to get the ddl description for a type
    * @param [in] type_name The name of the type to be resolved
    * @param [out] type_description Output paramter for the description for the type
    * @retval a_util::result::SUCCESS Everything went fine
    */
    virtual a_util::result::Result resolveType(const char* type_name, const char*& type_description) = 0;

    /**
    * \c getTime is used by the mapping engine to get the current time. Since only the
    * application of mapping can now what time domain should be used, this method has to be
    * implementied by the environment.
    * @returns The current timestamp in microseconds
    */
    virtual timestamp_t getTime() const = 0;

    /**
    * \c registerPeriodicTimer is used by the mapping engine to register periodic timers
    * @param [in] period_us The period
    * @param [in] listener The listener to be called periodically
    * @returns Standard result
    */
    virtual a_util::result::Result registerPeriodicTimer(timestamp_t period_us, IPeriodicListener* listener) = 0;

    /**
    * \c registerPeriodicTimer is used by the mapping engine to unregister periodic timers
    * @param [in] period_us The period
    * @param [in] listener The listener
    * @returns Standard result
    */
    virtual a_util::result::Result unregisterPeriodicTimer(timestamp_t period_us, IPeriodicListener* listener) = 0;
};

} // namespace rt
} // namespace mapping
#endif //MAPPINGENVIRONMENT_HEADER
