/**
 * @file
 * Launcher.
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

#include <a_util/result.h>
#include <ddl_generator_core.h>
#include "ddl2header_commandline.h"

namespace ddl_generator
{
namespace oo
{
    //define all needed error types and values locally
    _MAKE_RESULT(0, ERR_NOERROR)
    _MAKE_RESULT(-5, ERR_INVALID_ARG)
}
}
using namespace ddl_generator::oo;


int main(int argc, char* argv[])
{
    ddl::DDL2HeaderCommandLine cmdLine;
    if (a_util::result::isFailed(cmdLine.parseArgs(argc, argv)))
    {
        return ERR_INVALID_ARG.getCode();
    }

    if (cmdLine.isHelpRequested())
    {
        cmdLine.printHelp();
        return ERR_NOERROR.getCode();
    }

    if (a_util::result::isFailed(cmdLine.checkMandatoryArguments()))
    {
        return ERR_INVALID_ARG.getCode();
    }

    DDLUtilsCore core;
    a_util::result::Result res = core.generateHeaderFile(cmdLine.getDescriptionFile(), 
        cmdLine.getHeaderFile(), cmdLine.getStruct(), cmdLine.getNamespace(), 
        cmdLine.getDisplaceableString());
    if (a_util::result::isFailed(res))
    {
        LOG_ERROR("Error: An error occured during generating the header file.");
    }
    return res.getErrorCode();
}

