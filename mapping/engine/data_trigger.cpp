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

#include "data_trigger.h"

using namespace mapping::rt;

DataTrigger::DataTrigger(IMappingEnvironment& oEnv,
        const std::string& strTriggerName, 
        const std::string& strVariableName, 
        const std::string& strOperator, 
        const double& f64Value) : _env(oEnv), 
        _name(strTriggerName), _variable_name(strVariableName), 
        _value(f64Value), _is_running(false)
{
    if(strOperator == "equal")
    {
        _operator = e_equal;
    }
    else if(strOperator == "not_equal")
    {
        _operator = e_not_equal;
    }
    else if(strOperator == "less_than")
    {
        _operator = e_less_than;
    }
    else if(strOperator == "greater_than")
    {
        _operator = e_greater_than;
    }
    else if(strOperator == "less_than_equal")
    {
        _operator = e_less_than_equal;
    }
    else if(strOperator == "greater_than_equal")
    {
        _operator = e_greater_than_equal;
    }
}

DataTrigger::~DataTrigger()
{
    stop();
}

a_util::result::Result DataTrigger::start()
{
    _is_running = true;
    return a_util::result::SUCCESS;
}

a_util::result::Result DataTrigger::stop()
{
    _is_running = false;
    return a_util::result::SUCCESS;
}

bool DataTrigger::compare(double f64Val)
{
    switch(_operator)
    {
    case e_equal:
        return f64Val == _value;
    case e_not_equal:
        return f64Val != _value;
    case e_less_than:
        return f64Val < _value;
    case e_greater_than:
        return f64Val > _value;
    case e_less_than_equal:
        return f64Val <= _value;
    case e_greater_than_equal:
        return f64Val >= _value;
    default:
        assert(false);
    }

    return false;
}

a_util::result::Result DataTrigger::transmit()
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

const std::string& DataTrigger::getVariable() const
{
    return _variable_name;
}
