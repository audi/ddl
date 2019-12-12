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

#include "signal_trigger.h"

using namespace mapping::rt;

SignalTrigger::SignalTrigger(IMappingEnvironment& oEnv,
    const std::string& strTriggerName) : _env(oEnv),
    _name(strTriggerName), _is_running(false)
{
}

SignalTrigger::~SignalTrigger()
{
    stop();
}

a_util::result::Result SignalTrigger::start()
{
    _is_running = true;
    return a_util::result::SUCCESS;
}

a_util::result::Result SignalTrigger::stop()
{
    _is_running = false;
    return a_util::result::SUCCESS;
}

a_util::result::Result SignalTrigger::transmit()
{
    if(_is_running)
    {
        for(TargetSet::iterator it = _targets.begin(); it != _targets.end(); ++it)
        {
            const void* pBuffer = NULL;
            size_t szBuffer = 0;
            (*it)->aquireReadLock();
            (*it)->updateTriggerFunctionValues();
            if (isOk((*it)->getBufferRef(pBuffer, szBuffer)))
            {
                _env.sendTarget((handle_t)*it, pBuffer, szBuffer, 0);
            }
            (*it)->releaseReadLock();
        }
    }

    return a_util::result::SUCCESS;
}
