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

#include <iostream>
#include "commandline.h"

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

namespace ddl
{

    CommandLine::CommandLine()
    {
        _cli |= clara::Help(_showhelp);
        _cli |= getHeaderFileOpt();
        _cli |= getDescriptionFileOpt();
        _cli |= getStructOpt();
    }


    CommandLine::~CommandLine()
    {
    }

    a_util::result::Result CommandLine::parseArgs(int argc, char* argv[])
    {
        auto result = _cli.parse(clara::Args(argc, argv));
        if (!result) 
        {
            std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
            return ERR_INVALID_ARG;
        }
        return ERR_NOERROR;
    }

    bool CommandLine::isHelpRequested()
    {
        return _showhelp;
    }

    void CommandLine::printHelp()
    {
        std::cout << _cli << std::endl;
        printExamples();
    }

    a_util::result::Result CommandLine::checkMandatoryArguments()
    {
        a_util::result::Result result = ERR_NOERROR;
        if (_opt_header_file.empty())
        {
            std::cerr << "Error: No option 'headerfile' is set. " <<
                "Please use option '--help' for further information." << std::endl;
            result = ERR_INVALID_ARG;
        }
        if (_opt_description_file.empty())
        {
            std::cerr << "Error: No option 'descriptionfile' is set. " <<
                "Please use option '--help' for further information." << std::endl;
            result = ERR_INVALID_ARG;
        }
        return result;
    }

    std::string CommandLine::getHeaderFile()
    {
        return _opt_header_file;
    }

    std::string CommandLine::getDescriptionFile()
    {
        return _opt_description_file;
    }

    std::string CommandLine::getStruct()
    {
        return _opt_struct;
    }

    clara::Opt CommandLine::getHeaderFileOpt()
    {
        return clara::Opt(_opt_header_file, "file")
            ["-f"]["--headerfile"]
            ("[Mandatory] The path to the header file.");
    }

    clara::Opt CommandLine::getDescriptionFileOpt()
    {
        return clara::Opt(_opt_description_file, "file")
            ["-d"]["--descriptionfile"]
            ("[Mandatory] The path to the description file.");
    }

    clara::Opt CommandLine::getStructOpt()
    {
        return clara::Opt(_opt_struct, "structName")
            ["-s"]["--struct"]
            ("[Optional] Just create the description file for the given struct "
                "of the header file.");
    }
}
