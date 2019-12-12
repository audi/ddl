/**
 * @file
 * Package header for DDL
 * This package provides the DDL Represantations and Coder.
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

#ifndef _PACKAGE_DDL_REPRESENTATION_HEADER_
#define _PACKAGE_DDL_REPRESENTATION_HEADER_
    
    // forward declarations
    // very ugly hack, but linearizing all dependencies is not an option right now
namespace ddl {
    class DDL;
    class DDLError;
    class IDDL;
    class DDLAlignment;
    class DDLBaseunit;
    class IDDLUnit;
    class DDLByteorder;
    class DDLCloner;
    class DDLCompare;
    class DDLComplex;
    class IDDLDataType;
    class DDLDataType;
    class DDLDescription;
    class DDLElement;
    class DDLEnum;
    class DDLExtDeclaration;
    class IDDLFactoryMethod;
    class DDLHeader;
    class DDLImporter;
    class DDLInspector;
    class DDLPrefix;
    class DDLPrinter;
    class DDLProperty;
    class DDLRefUnit;
    class DDLRepair;
    class DDLResolver;
    class DDLStream;
    class DDLStreamMetaType;
    class DDLStreamStruct;
    class DDLUnit;
    class IDDLUnit;
    class IDDLVisitor;
}


    #include <stack>
    #include <algorithm>

    //common errordefintion
    #include "ddl_error.h"
    //common supported versions
    #include "ddlversion.h"

    // OO-DDL interfaces
    #include "ddl_intf.h"
    #include "ddl_type.h"
    #include "ddlalignment.h"

    #include "ddlserializable_intf.h"
    #include "ddlunit_intf.h"
    #include "ddldatatype_intf.h"
    #include "ddlvisitor_intf.h"
    #include "ddlfactorymethod_intf.h"

    // DDL object representation (OO-DDL)
    #include "ddlbyteorder.h"
    #include "ddlcomplex.h"
    #include "ddldatatype.h"
    #include "ddlheader.h"
    #include "ddlstream.h"
    #include "ddlenum.h"
    #include "ddlcontainer.h"
    #include "ddldescription.h"
    #include "ddlunit.h"
    #include "ddlrefunit.h"
    #include "ddlprefix.h"
    #include "ddlextdeclaration.h"
    #include "ddlelement.h"
    #include "ddlbaseunit.h"
    #include "ddlstreamstruct.h"
    #include "ddlstreammetatype.h"
    #include "ddlproperty.h"
    #include "ddlprinter.h"
    #include "ddlimporter.h"
    #include "ddlcloner.h"
    #include "ddlresolver.h"
    #include "ddlrepair.h"
    #include "ddlinspector.h"
    #include "ddlcompare.h"

#endif // _PACKAGE_DDL_REPRESENTATION_HEADER_

