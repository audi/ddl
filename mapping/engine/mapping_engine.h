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

/*! Mapping Engine
 *
 * \section intro_sec Introduction
 *
 * This is the Mapping Engine documentation.
 */

#ifndef MAPPINGENGINE_HEADER
#define MAPPINGENGINE_HEADER

#include "mapping/configuration/map_configuration.h"

#include "mapping_environment_intf.h"
#include "element.h"
#include "source.h"
#include "target.h"
#include "trigger.h"
#include "periodic_trigger.h"
#include "data_trigger.h"
#include "signal_trigger.h"

namespace mapping
{
namespace rt
{

class MappingEngine
{
public:
    /**
    * CTOR
    * @param[in] env - Mapping environment, used as interface
    */
    MappingEngine(IMappingEnvironment& env);

    /**
    * DTOR
    */
    ~MappingEngine();

    /**
    * Setter for the current mapping configuration
    * @param[in] config - The Configuration instance
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result setConfiguration(const oo::MapConfiguration& config);

    /**
    * Method to instanciate or expand the mapping structure for one particular target
    * @param [in] target_name The target name
    * @param [out] mapped_signal - Destination parameter for the signal handle
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_INVALID_ARG  Error target already mapped or not found in mapping configuration
    * @retval ERR_FAILED       Error during creation of the mapping
    */
    a_util::result::Result Map(const std::string& target_name, handle_t& mapped_signal);

    /**
    * Method to reset target buffers
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error during runtime
    */
    a_util::result::Result reset();

    /**
    * Method to start mapping
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error during runtime
    */
    a_util::result::Result start();

    /**
    * Method to stop mapping
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result stop();

    /**
    * Method to remove a target from the existing mapping structure
    * @param [in] mapped_signal - Target handle
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_INVALID_ARG  Error target not found in mapping structure
    * @retval ERR_FAILED       Error during removal
    */
    a_util::result::Result unmap(handle_t mapped_signal);

    /**
    * Method to know if signal has triggers or not
    *
    * @param [in] mapped_signal The target handle
    * @retval true Signal has triggers
    * @retval false Signal has no triggers
    */
    bool hasTriggers(handle_t mapped_signal) const;

    /**
    * Method to send current data
    *
    * @param [in] mapped_signal The target handle
    * @param [in] target_buffer The pointer referencing the target buffer
    * @param [in] target_buffer_size The size of the target buffer
    * @retval a_util::result::SUCCESS Everything went fine
    * @retval ERR_INVALID_ARG Invalid target handle or target buffer is NULL
    * @retval ERR_MEMORY      Target buffer not large enough
    */
    a_util::result::Result getCurrentData(handle_t mapped_signal, void* target_buffer, size_t target_buffer_size) const;

    /**
    * Method to reinitialize the existing mapping structure
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error during reinitialization
    */
    a_util::result::Result unmapAll();

private:
    /**
    * Method to give an initial value to all targets
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error during initialization
    */
    a_util::result::Result initializeModel();

private:
    IMappingEnvironment& _env;
    bool _running;

    oo::MapConfiguration _map_config;
    TargetMap _targets;
    SourceMap _sources;
    TriggerMap _triggers;
};

} // namespace rt
} // namespace mapping
#endif //MAPPINGENGINE_HEADER
