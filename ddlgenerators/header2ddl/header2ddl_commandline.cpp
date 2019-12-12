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
#include "header2ddl_commandline.h"

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

namespace ddl
{
    Header2DDLCommandLine::Header2DDLCommandLine() : CommandLine()
    {
        _cli |= getDDLVersionOpt();
        _cli |= getHeaderFileSetOpt();
    }

    ddl::DDLVersion Header2DDLCommandLine::getDDLVersion()
    {
        return DDLVersion::fromString(_opt_DDL_version);
    }
    std::string  Header2DDLCommandLine::getHeaderFileSet()
    {
        return _opt_header_file_set;
    }

    void Header2DDLCommandLine::setHeaderFile(std::string header_file)
    {
        _opt_header_file = header_file;
    }

    void Header2DDLCommandLine::printExamples()
    {
        std::cout << "examples: " << std::endl;
        std::cout << "  --headerfile=c:/myHeaderFile.h " <<
            "--descriptionfile=c:/myDescriptionFile.description";
        std::cout << std::endl << "  or" << std::endl;
        std::cout << "  --headerfile=c:/myHeaderFile.h " <<
            "--descriptionfile=c:/myDescriptionFile.description ";
        std::cout << "-struct=tMyStruct" << std::endl;
    }

    clara::Opt Header2DDLCommandLine::getDDLVersionOpt()
    {
        return clara::Opt(_opt_DDL_version, "version")
            ["-v"]["--ddlversion"]
            ("[Optional] Default value is 4.0. Supported formats are 3.0 and 4.0");
    }

    clara::Opt Header2DDLCommandLine::getHeaderFileSetOpt()
    {
        return clara::Opt(_opt_header_file_set, "list,of,files")
            ["--headerfileset"]
            ("Can be used instead of the headerfile option. List of comma separated "
            "headerfiles (no spaces!) to be merged into a single descriptionfile.");
    }

}
