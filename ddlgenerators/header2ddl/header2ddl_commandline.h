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

#ifndef _HEADER2DDL_COMMAND_LINE_H_
#define _HEADER2DDL_COMMAND_LINE_H_

#include <ddl.h>
#include "commandline.h"

namespace ddl
{
    class Header2DDLCommandLine : public ddl::CommandLine
    {
    public:
        Header2DDLCommandLine();

        DDLVersion getDDLVersion();
        std::string getHeaderFileSet();

        void setHeaderFile(std::string headerFile);

    protected:
        void printExamples() override;

        clara::Opt getDDLVersionOpt();
        clara::Opt getHeaderFileSetOpt();

        std::string _opt_DDL_version = "4.0";
        std::string _opt_header_file_set;
    };
}
#endif

