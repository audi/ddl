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

#include "periodic_trigger.h"

using namespace mapping::rt;

PeriodicTrigger::PeriodicTrigger(IMappingEnvironment& oEnv,
    const std::string& strTriggerName, double fPeriod) : _env(oEnv),
    _name(strTriggerName), _period(fPeriod), _running(false)
{
}

PeriodicTrigger::~PeriodicTrigger()
{
    _env.unregisterPeriodicTimer((timestamp_t)_period * 1000, this);
}

a_util::result::Result PeriodicTrigger::create()
{
    return _env.registerPeriodicTimer((timestamp_t)_period * 1000, this);
}

a_util::result::Result PeriodicTrigger::start()
{
    _running = true;
    return a_util::result::SUCCESS;
}

a_util::result::Result PeriodicTrigger::stop()
{
    _running = false;
    return a_util::result::SUCCESS;
}

void PeriodicTrigger::onTimer(timestamp_t tmNow)
{
    if (_running)
    {
        for(TargetSet::iterator it = _targets.begin(); it != _targets.end(); ++it)
        {
            const void* pBuffer = NULL;
            size_t szBuffer = 0;
            (*it)->aquireReadLock();
            (*it)->updateTriggerFunctionValues();
            if (isOk((*it)->getBufferRef(pBuffer, szBuffer)))
            {
                _env.sendTarget((handle_t)*it, pBuffer, szBuffer, tmNow);
            }
            (*it)->releaseReadLock();
        }
    }
}
