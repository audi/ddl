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

#ifndef TRIGGER_BASE_HEADER
#define TRIGGER_BASE_HEADER

#include "a_util/result.h"

#include "target.h"

namespace mapping
{
namespace rt
{

/// base class representing a trigger
class TriggerBase
{
public:
    /**
    * Virtual DTOR
    */
    virtual ~TriggerBase();

    /**
    * Method to start trigger
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    * @retval ERR_FAILED       Error during runtime
    */
    virtual a_util::result::Result start() = 0;

    /**
    * Method to stop trigger
    *
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    virtual a_util::result::Result stop() = 0;

public: // Base implementations
    /**
    * Getter for the list of registered targets
    * @return the list of targets
    */
    TargetSet& getTargetList();

    /**
    * Method to register a new target
    * @param[in] target The target to add
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result addTarget(Target* target);

    /**
    * Method to deregister a target
    * @param[in] target The target to remove
    * @retval a_util::result::SUCCESS      Everything went fine
    */
    a_util::result::Result removeTarget(Target* target);

protected:
    /// nodoc
    TargetSet _targets;
};

typedef std::map<std::string, TriggerBase*> TriggerMap;

} // namespace rt
} // namespace mapping
#endif //TRIGGER_BASE_HEADER
