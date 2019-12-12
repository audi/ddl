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

#ifndef _DDL2HEADER_COMMAND_LINE_H_
#define _DDL2HEADER_COMMAND_LINE_H_

#include "commandline.h"

namespace ddl
{
    class DDL2HeaderCommandLine: public ddl::CommandLine
    {

    public:
        DDL2HeaderCommandLine();

        std::string getNamespace();
        std::string getDisplaceableString();

    protected:
        void printExamples();

        clara::Opt getNamespaceOpt();
        clara::Opt getDisplaceableStringOpt();

        std::string _opt_namespace;
        std::string _opt_displaceable_string;
       
    };
}
#endif

