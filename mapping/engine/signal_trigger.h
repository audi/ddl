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

#ifndef SIGNAL_TRIGGER_HEADER
#define SIGNAL_TRIGGER_HEADER

#include "a_util/result.h"
#include "trigger.h"

namespace mapping
{
namespace rt
{

/// SignalTrigger implements a concrete periodic trigger in the runtime
class SignalTrigger : public TriggerBase
{
public:
    /**
    * CTOR
    * @param [in] env The mapping environment
    * @param [in] trigger_name The name of the trigger
    * @param [in] fPeriod The period of the trigger
    */
    SignalTrigger(IMappingEnvironment& env,
        const std::string& trigger_name);

    /**
    * DTOR
    */
    ~SignalTrigger();

    /**
    * Method to start trigger
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error during runtime
    */
    a_util::result::Result start();

    /**
    * Method to stop trigger
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result stop();

    /**
    * Method to transmit targets
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result transmit();

private:
    /// @cond nodoc
    IMappingEnvironment& _env;
    std::string _name;
    bool _is_running;
    /// @endcond
};

} // namespace rt
} // namespace mapping
#endif //SIGNAL_TRIGGER_HEADER
