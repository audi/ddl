/**
* @file
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

namespace ddl
{
    DDL2HeaderCommandLine::DDL2HeaderCommandLine(): CommandLine()
    {
        _cli |= getNamespaceOpt();
        _cli |= getDisplaceableStringOpt();
    }

    std::string DDL2HeaderCommandLine::getNamespace()
    {
        return _opt_namespace;
    }

    std::string DDL2HeaderCommandLine::getDisplaceableString()
    {
        return _opt_displaceable_string;
    }

    void DDL2HeaderCommandLine::printExamples()
    {
        std::cout << std::endl << "If the target header file exists already the descriptions will be merged." << std::endl;
        std::cout << "This can lead to data loss in the existing header file." << std::endl << std::endl;
        std::cout << "examples: " << std::endl;
        std::cout << "  --headerfile=c:/myHeaderFile.h " <<
            "--descriptionfile=c:/myDescriptionFile.description";
        std::cout << std::endl << "  or" << std::endl;
        std::cout << "  --headerfile=c:/myHeaderFile.h " <<
            "--descriptionfile=c:/myDescriptionFile.description ";
        std::cout << "-struct=tMyStruct" << std::endl;
    }

    clara::Opt DDL2HeaderCommandLine::getNamespaceOpt()
    {
        return clara::Opt(_opt_namespace, "name")
            ["-n"]["--namespace"]
            ("[Optional] Place all generated elements in this namespace");
    }

    clara::Opt DDL2HeaderCommandLine::getDisplaceableStringOpt()
    {
        return clara::Opt(_opt_displaceable_string, "string")
            ["--displace"]
            (" [Optional] Remove this string from beginning of all element names.");

    }

}
