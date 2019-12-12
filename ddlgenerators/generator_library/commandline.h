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

#ifndef _DDL_COMMAND_LINE_H_
#define _DDL_COMMAND_LINE_H_

#include <a_util/result.h>
#include <clara.hpp>

namespace ddl
{
    class CommandLine
    {
    public:
        CommandLine();
        virtual ~CommandLine();

        a_util::result::Result parseArgs(int argc, char* argv[]);
        bool isHelpRequested();
        void printHelp();
        virtual a_util::result::Result checkMandatoryArguments();

        std::string getHeaderFile();
        std::string getDescriptionFile();
        std::string getStruct();

    protected:

        virtual void printExamples() = 0;

        clara::Opt getHeaderFileOpt();
        clara::Opt getDescriptionFileOpt();
        clara::Opt getStructOpt();


        clara::Parser _cli;
        bool _showhelp = false;
        std::string _opt_header_file;
        std::string _opt_description_file;
        std::string _opt_struct;
    };
}
#endif

