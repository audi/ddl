/**
 * @file
 * Launcher
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

#include <iostream>
#include <ddl_generator_core.h>
#include "header2ddl_commandline.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR);
        _MAKE_RESULT(-5, ERR_INVALID_ARG);
    }
}
using namespace ddl_generator::oo;

a_util::result::Result HandleInputFileset(std::string strHeaderFileSet, std::string& strMergedHeaderFile)
{
    std::vector<std::string> vecHeaderFiles = a_util::strings::split(strHeaderFileSet, ",");
    if (vecHeaderFiles.size() == 0)
    {
        LOG_ERROR("ERROR: Empty header file list");
        return ERR_INVALID_ARG.getCode();
    }

    std::string strMergedHeaderContent;
    for (std::string strHeaderpath : vecHeaderFiles)
    {
        std::string strHeaderContent;
        if (!a_util::filesystem::exists(strHeaderpath) || a_util::result::isFailed(a_util::filesystem::readTextFile(strHeaderpath, strHeaderContent)))
        {
            LOG_ERROR(a_util::strings::format("ERROR: Could not read header file '%s'", strHeaderpath.c_str()).c_str());
            return ERR_INVALID_ARG.getCode();
        }

        strMergedHeaderContent.append(strHeaderContent);
    }

    a_util::filesystem::Path mergedHeader = a_util::filesystem::getTempDirectory().append("MergedHeader.h");
    a_util::filesystem::writeTextFile(mergedHeader, strMergedHeaderContent);
    strMergedHeaderFile = mergedHeader.toString();
    return ERR_NOERROR.getCode();
}


int main(int argc, char* argv[])
{
    ddl::Header2DDLCommandLine cmd_line;
    if (a_util::result::isFailed(cmd_line.parseArgs(argc, argv)))
    {
        return ERR_INVALID_ARG.getCode();
    }

    if (cmd_line.isHelpRequested())
    {
        cmd_line.printHelp();
        return ERR_NOERROR.getCode();
    }

    std::string strMergedHeader;
    if (!cmd_line.getHeaderFileSet().empty())
    {
        a_util::result::Result res = HandleInputFileset(cmd_line.getHeaderFileSet(), strMergedHeader);
        if (a_util::result::isFailed(res))
        {
            return res.getErrorCode();
        }
        cmd_line.setHeaderFile(strMergedHeader);
    }
    if (a_util::result::isFailed(cmd_line.checkMandatoryArguments()))
    {
        return ERR_INVALID_ARG.getCode();
    }

    DDLUtilsCore core;
    a_util::result::Result res = core.generateDescriptionFile(cmd_line.getHeaderFile(),
        cmd_line.getDescriptionFile(), cmd_line.getDDLVersion(), cmd_line.getStruct());
    if (a_util::result::isFailed(res))
    {
        std::cerr << "Error: An error occured during generating of the description file.";
    }

    // delete temp file
    if (!strMergedHeader.empty())
    {
        a_util::filesystem::remove(strMergedHeader);
    }

    return res.getErrorCode();

}
