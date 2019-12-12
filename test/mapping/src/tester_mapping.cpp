/**
 * @file 
 * Implementation of the tester for Mapping
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

#include <memory>   //std::unique_ptr<>
#include <ddl.h>
#include "a_util/result/error_def.h"
#include <gtest/gtest.h>
#include "../../_common/adtf_compat.h"
#include "../../_common/compat.h"

using namespace mapping::oo;
using namespace mapping::rt;
using namespace ddl;

_MAKE_RESULT(-5, ERR_INVALID_ARG)
    _MAKE_RESULT(-11, ERR_INVALID_FILE)
    _MAKE_RESULT(-20, ERR_NOT_FOUND)
    _MAKE_RESULT(-40, ERR_INVALID_STATE)
    _MAKE_RESULT(-42, ERR_INVALID_TYPE)

    ddl::DDLDescription* LoadDDL(const std::string& strDDLFile)
{
    std::string strDDL;

    if (a_util::filesystem::readTextFile(strDDLFile, strDDL) != a_util::filesystem::OK)
    {
        return NULL;
    }

    ddl::DDLImporter oImp;
    if (a_util::result::isFailed(oImp.setXML(strDDL)))
    {
        return NULL;
    }

    if (a_util::result::isFailed(oImp.createNew()))
    {
        return NULL;
    }

    return oImp.getDDL();
}

/**
* @detail This test creates an empty configuration.
*/
TEST(cTesterMapping,
    TestEmptyConfig)
{
    TEST_REQ("FEPSDK-283");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    ASSERT_EQ(oConfig.getHeader().getAuthor() , a_util::system::getCurrentUserName());
    ASSERT_EQ(oConfig.getHeader().getCreationDate() , oConfig.getHeader().getModificationDate());
    ASSERT_EQ(oConfig.getHeader().getLanguageVersion() , "1.00");

    ASSERT_TRUE(oConfig.getSourceList().empty());
    ASSERT_TRUE(oConfig.getTargetList().empty());
    ASSERT_TRUE(oConfig.getTransformationList().empty());

    // test setDescription for reset
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.setDescription(NULL));

    // test load from not existing file
    ASSERT_EQ(oConfig.loadFromFile("blubb") , ERR_INVALID_FILE);
}

/**
* @detail This test creates a configuration and loads a map file.
*/
TEST(cTesterMapping,
    TestLoadMapSuccessConfig)
{
    TEST_REQ("FEPSDK-283");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    a_util::xml::DOM oDom;
    ASSERT_TRUE(oDom.load("files/base.map"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.loadFromDOM(oDom));

    // Verify header
    ASSERT_EQ(oConfig.getHeader().getAuthor() , "ASAP");
    ASSERT_EQ(oConfig.getHeader().getCreationDate() , "2016-Mar-22");
    ASSERT_EQ(oConfig.getHeader().getLanguageVersion() , "1.00");
    ASSERT_EQ(oConfig.getHeader().getModificationDate() , "2016-Mar-22");    

    // Verify source list
    ASSERT_EQ(oConfig.getSourceList().size() , 4);
    ASSERT_TRUE(oConfig.getSource("Light"));
    ASSERT_TRUE(oConfig.getSource("Wiper"));
    ASSERT_TRUE(oConfig.getSource("AALA"));
    ASSERT_TRUE(oConfig.getSource("Additional"));
    ASSERT_EQ(oConfig.getSource("Light")->getType() , "tFEP_Driver_LightControl");
    ASSERT_EQ(oConfig.getSource("Wiper")->getType() , "tFEP_Driver_WiperControl");
    ASSERT_EQ(oConfig.getSource("AALA")->getType() , "tFEP_Driver_AalaControl");
    ASSERT_EQ(oConfig.getSource("Additional")->getType() , "tFEP_Driver_AdditionalControl");

    // Verify target list
    ASSERT_EQ(oConfig.getTargetList().size() , 1);
    ASSERT_TRUE(oConfig.getTarget("Signal"));
    ASSERT_EQ(oConfig.getTarget("Signal")->getType() , "tFEP_Driver_DriverCtrl");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList().size() , 13);

    // Verify assignment list
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[0].getConstant() , "42");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[0].getTo() , "f64SimTime");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[0].getSource().empty());
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[0].getFrom().empty());
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[0].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[1].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[1].getTo() , "sAalaControl");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[1].getFrom().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[1].getSource() , "AALA");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[1].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[2].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[2].getTo() , "sWiperControl");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[2].getFrom().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[2].getSource() , "Wiper");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[2].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[3].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[3].getTo() , "ui8Spare1[0]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[3].getFrom() , "ui8Spare1[3]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[3].getSource() , "Light");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[3].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[4].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[4].getTo() , "ui8Spare1[1]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[4].getFrom() , "ui8Spare1[2]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[4].getSource() , "Light");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[4].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[5].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[5].getTo() , "ui8Spare1[2]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[5].getFrom() , "ui8Spare1[1]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[5].getSource() , "Light");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[5].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[6].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[6].getTo() , "ui8Spare1[3]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[6].getFrom() , "ui8Spare1[0]");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[6].getSource() , "Light");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[6].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[7].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[7].getTo() , "ui32ValidityMask");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[7].getFrom() , "ui32ValidityMask");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[7].getSource() , "Additional");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[7].getTransformation() , "polynom2");

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[8].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[8].getTo() , "sLightControl.eLightState");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[8].getFrom() , "eLightState");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[8].getSource() , "Light");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[8].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[9].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[9].getTo() , "ui32VehicleId");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[9].getFunction() , "simulation_time");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[9].getSource().empty());
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[9].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[10].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[10].getTo() , "sLongitudinal.f32ThrottlePedal");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[10].getFunction() , "trigger_counter");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[10].getModulo() , "10");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[10].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[11].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[11].getTo() , "sLightControl.bIndicatorLeft");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[11].getFunction() , "received");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[11].getSource() , "Light");
    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[11].getTransformation().empty());

    ASSERT_TRUE(oConfig.getTarget("Signal")->getAssignmentList()[12].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[12].getTo() , "eGear");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[12].getFrom() , "eFrontState");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[12].getSource() , "Wiper");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList()[12].getTransformation() , "table1");

    // Verify referenced sources list
    ASSERT_EQ(oConfig.getTarget("Signal")->getReferencedSources().size() , 4);

    // Verify trigger list
    ASSERT_EQ(oConfig.getTarget("Signal")->getTriggerList().size() , 3);
    ASSERT_TRUE(oConfig.getTarget("Signal")->getTriggerList()[0]);
    const MapPeriodicTrigger* p =
        dynamic_cast<const MapPeriodicTrigger*>(oConfig.getTarget("Signal")->getTriggerList()[0]);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->getPeriod() , 10);

    ASSERT_TRUE(oConfig.getTarget("Signal")->getTriggerList()[1]);
    const MapSignalTrigger* s =
        dynamic_cast<const MapSignalTrigger*>(oConfig.getTarget("Signal")->getTriggerList()[1]);
    ASSERT_TRUE(s);
    ASSERT_EQ(s->getVariable() , "Light");

    ASSERT_TRUE(oConfig.getTarget("Signal")->getTriggerList()[2]);
    const MapDataTrigger* d =
        dynamic_cast<const MapDataTrigger*>(oConfig.getTarget("Signal")->getTriggerList()[2]);
    ASSERT_TRUE(d);
    ASSERT_EQ(d->getSource() , "Light");
    ASSERT_EQ(d->getVariable() , "ui8Spare1[3]");
    ASSERT_EQ(d->getOperator() , "less_than");
    ASSERT_EQ(d->getValue() , 2);

    // Verify transformation list
    ASSERT_EQ(oConfig.getTransformationList().size() , 3);
    ASSERT_TRUE(oConfig.getTransformation("polynom1"));
    const MapPolynomTransformation* t =
        dynamic_cast<const MapPolynomTransformation*>(oConfig.getTransformation("polynom1"));
    ASSERT_TRUE(t);
    ASSERT_EQ(t->getA() , 1);
    ASSERT_EQ(t->getB() , 1.7);
    ASSERT_EQ(t->getC() , 2);
    ASSERT_EQ(t->getD() , 1.1);
    ASSERT_EQ(t->getE() , 0);

    ASSERT_TRUE(oConfig.getTransformation("polynom2"));
    t = dynamic_cast<const MapPolynomTransformation*>(oConfig.getTransformation("polynom2"));
    ASSERT_TRUE(t);
    ASSERT_EQ(t->getA() , 2);
    ASSERT_EQ(t->getB() , 1);
    ASSERT_EQ(t->getC() , 0);
    ASSERT_EQ(t->getD() , 0);
    ASSERT_EQ(t->getE() , 0);

    ASSERT_TRUE(oConfig.getTransformation("table1"));
    const MapEnumTableTransformation* tEnum =
        dynamic_cast<const MapEnumTableTransformation*>(oConfig.getTransformation("table1"));
    ASSERT_TRUE(tEnum);
    ASSERT_EQ(tEnum->getDefault() , 3);
    std::map< int64_t, int64_t > oConvList = tEnum->getConversions();
    ASSERT_EQ(oConvList.size() , 5);
    ASSERT_EQ(oConvList[0] , 4);
    ASSERT_EQ(oConvList[1] , 5);
    ASSERT_EQ(oConvList[2] , 6);
    ASSERT_EQ(oConvList[3] , 7);
    ASSERT_EQ(oConvList[4] , 4);
}

/**
* @detail Test Configuration to load erroneous Map Files.
*/
TEST(cTesterMapping,
    TestLoadMapFailConfig)
{
    TEST_REQ("FEPSDK-283");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    a_util::xml::DOM oDom;
    for (int i = 0; i <= 40; ++i)
    {
        oDom = a_util::xml::DOM();
        ASSERT_TRUE(oDom.load(a_util::strings::format("files/nok%d.map", i)));
        a_util::result::Result nRes = oConfig.loadFromDOM(oDom);
        //tMapErrorList list = oConfig.getErrorList();
        //LOG_INFO(list.Join(','));
        ASSERT_TRUE(a_util::result::isFailed(nRes)) << 
            a_util::strings::format("%d should have failed but didn't", i).c_str();
    }
    // nok0.map -> <from> and <constant> attributes set at the same time for <assignment> to 'f64SimTime'
    // nok1.map -> Missing 'from' or 'constant' attribute for assignment element
    // nok2.map -> Missing 'to' attribute for <assignment> element
    // nok3.map -> Missing <language_version>, <author>, <date_creation>, <date_change> or <description> element in header
    // nok4.map -> Missing or empty name attribute for <source> element
    // nok5.map -> Missing or empty type attribute for <source> element
    // nok6.map -> Missing name attribute for <target> element
    // nok7.map -> Missing type attribute for <target> element
    // nok8.map -> Missing <mapping> root element
    // nok9.map -> Missing <header> element
    // nok10.map -> Redefinition of source 'Light'
    // nok11.map -> Unknown source type 'tBullshit'
    // nok12.map -> Redefinition of target or name reuse of source 'Signal' 
    // nok13.map -> Unknown target type 'tBullshit'
    // nok14.map ->  Empty or non-numeric constant 'true'
    // nok15.map -> Target element referenced by 'to' unknown or invalid: 'bullshit'
    // nok16.map -> Assignment references unknown source: 'Bullshit'
    // nok17.map -> Source element referenced by 'from' unknown or invalid: 'AALA.nBullshit' 
    // nok18.map -> Destination element is assigned repeatedly: ui8Spare1[0]
    // nok19.map -> Structure <assignment> 'sLightControl' overwrites previous element <assignment> in <target> 'Signal'
    // nok20.map -> Element <assignment> 'sLightControl.eLightState' overwrites previous structure <assignment> in <target> 'Signal'
    // nok21.map -> Unknown transformation: 'bullshit'
    // nok22.map -> Incompatible types in <assignment>: struct->other struct 
    // nok23.map -> Incompatible types in <assignment>: struct->primitive
    // nok24.map -> Incompatible types in <assignment>: primitive->struct
    // nok25.map -> Incompatible types in <assignment>: array->array with other size
    // nok26.map -> Missing <to> attribute for an <assignment>
    // nok27.map -> <from> attribute empty for <assignment> to 'f64SimTime'
    // nok28.map -> Missing <trigger> type attribute
    // nok29.map -> Invalid period attribute for periodic trigger (expected positive integer)
    // nok30.map -> Missing unit attribute for periodic trigger
    // nok31.map -> Invalid element type in <target> 'Signal': 'tigger'
    // nok32.map -> Invalid coefficient attribute 'b' for polynomial transformation 'polynom2' (expected number),
    // Invalid coefficient attribute 'd' for polynomial transformation 'polynom2' (expected number)
    // nok33.map -> Dynamic target elements are not allowed: 'f64DynamicArray' in <target> 'DynSignal'
    // nok34.map -> Incompatible types for <assignment> to 'ui8Spare1[0]' in <target> 'Signal'
    // nok35.map -> <function> should be of type 'simulation_time()', 'trigger_counter([Modulo])' or 'received([Signal])' for <assignment> to 'ui32VehicleId'
    // nok36.map -> <function> of type 'simulation_time()' takes no argument for <assignment> to 'ui32VehicleId'
    // nok37.map -> <function> of type'received([Signal])' has no argument for <assignment> to 'ui32VehicleId'
    // nok38.map -> Missing variable attribute for signal trigger
    // nok39.map -> Variable attribute for data trigger should be a signal element (expected [Signal].[Element]),Invalid operator attribute for data trigger
    //(expected less_than, greater_than, less_than_equal, greater_than_equal, equal or not_equal),Invalid value attribute for data trigger (expected floating point value)
    // nok40.map -> <to> attribute 'GS' for <conversion> in <transformation> 'table1' should be a element of 'tFEP_Driver_GearSelection' <enumeration>
}

/**
* @detail Test Configuration to merge Map Files
*/
TEST(cTesterMapping,
    TestMergeMapsSuccessConfig)
{
    TEST_REQ("FEPSDK-283");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    a_util::xml::DOM oDom;
    ASSERT_TRUE(oDom.load("files/base.map"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.loadFromDOM(oDom));

    oDom = a_util::xml::DOM();
    ASSERT_TRUE(oDom.load("files/merge.map"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.loadFromDOM(oDom, MapConfiguration::mc_merge_mapping));

    // Verify that new signal was added in target list
    ASSERT_EQ(oConfig.getTargetList().size() , 2);
    ASSERT_TRUE(oConfig.getTarget("Signal2"));
    ASSERT_EQ(oConfig.getTarget("Signal2")->getType() , "tFEP_Driver_LateralControl");
    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList().size() , 2);

    ASSERT_TRUE(oConfig.getTarget("Signal2")->getAssignmentList()[0].getConstant().empty());
    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList()[0].getFrom() , "ui32ValidityMask");
    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList()[0].getSource() , "Light");
    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList()[0].getTo() , "f32SteeringWheel");
    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList()[0].getTransformation() , "polynom2");

    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList()[1].getConstant() , "21");
    ASSERT_TRUE(oConfig.getTarget("Signal2")->getAssignmentList()[1].getFrom().empty());
    ASSERT_TRUE(oConfig.getTarget("Signal2")->getAssignmentList()[1].getSource().empty());
    ASSERT_EQ(oConfig.getTarget("Signal2")->getAssignmentList()[1].getTo() , "ui32ValidityMask");
    ASSERT_TRUE(oConfig.getTarget("Signal2")->getAssignmentList()[1].getTransformation().empty());

    // this tests a regression where setting a ddl description would clear the complete configuration
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.setDescription(poDDL.get()));
    ASSERT_EQ(oConfig.getTargetList().size() , 2);

    // load ddl that doesn't contain the target signal lateralcontrol
    // so the mapping config should be cleared
    std::unique_ptr<ddl::DDLDescription> poBadDDL(LoadDDL
        ("files/test_without_lateralcontrol_signal.description"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.setDescription(poBadDDL.get()));
    ASSERT_EQ(oConfig.getTargetList().size() , 0);
}

/**
* @detail Test Configuration to merge conflicting Map Files
*/
TEST(cTesterMapping,
    TestMergeMapsFailConfig)
{
    TEST_REQ("FEPSDK-283");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    a_util::xml::DOM oDom;
    ASSERT_TRUE(oDom.load("files/base.map"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.loadFromDOM(oDom));

    for (int i = 0; i <= 4; ++i)
    {
        oDom = a_util::xml::DOM();      
        ASSERT_TRUE(oDom.load(a_util::strings::format("files/nok_merge%d.map", i)));
        a_util::result::Result nRes = oConfig.loadFromDOM(oDom, MapConfiguration::mc_merge_mapping);
        //tMapErrorList list = oConfig.getErrorList();
        //LOG_INFO(list.Join(','));
        ASSERT_TRUE(a_util::result::isFailed(nRes)) <<
            a_util::strings::format("%d should have failed but didn't", i).c_str();
    }
    // nok_merge0.map -> Trying to merge conflicting source definition for 'Light'
    // nok_merge1.map -> Trying to merge conflicting target definition for 'Signal'
    // nok_merge2.map -> Trying to merge conflicting transformation definition for 'polynom1'
    // nok_merge3.map -> Trying to merge conflicting target definition for 'Signal'
    // nok_merge4.map -> Trying to merge conflicting transformation definition for 'table1'    
}

/**
* @detail Test load then write configuration
*/
TEST(cTesterMapping,
    TestReadWriteMapFile)
{
    TEST_REQ("CDPKGDDL-28");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    a_util::xml::DOM oDom;
    ASSERT_TRUE(oDom.load("files/base.map"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.loadFromDOM(oDom));

    a_util::xml::DOM oNewDom;
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.writeToDOM(oNewDom));
    ASSERT_EQ(oNewDom.getRoot().getName() , oDom.getRoot().getName());
    ASSERT_EQ(oNewDom.getRoot().getChildren().size() , oDom.getRoot().getChildren().size());

    //Verify sources
    a_util::xml::DOMElement oSrcsEl = oDom.getRoot().getChild("sources");
    a_util::xml::DOMElement oNewSrcsEl = oNewDom.getRoot().getChild("sources");
    ASSERT_TRUE(!oSrcsEl.isNull() && !oNewSrcsEl.isNull());
    a_util::xml::DOMElementList oOldChildren = oSrcsEl.getChildren();
    a_util::xml::DOMElementList oNewChildren = oNewSrcsEl.getChildren();
    ASSERT_TRUE(oOldChildren.size() == oNewChildren.size());
    a_util::xml::DOMElementList::const_iterator itSrcNew = oNewChildren.begin();
    for( a_util::xml::DOMElementList::const_iterator itSrc = oOldChildren.begin();
        itSrc != oOldChildren.end(); itSrc++, itSrcNew++)
    {
        ASSERT_EQ(itSrc->getName() , itSrcNew->getName());
        ASSERT_EQ(itSrc->getAttributes() , itSrcNew->getAttributes());
        ASSERT_EQ(itSrc->getChildren().size() , itSrcNew->getChildren().size());
    }

    //Verify targets
    a_util::xml::DOMElement oTrgsEl = oDom.getRoot().getChild("targets");
    a_util::xml::DOMElement oNewTrgsEl = oNewDom.getRoot().getChild("targets");
    ASSERT_TRUE(!oTrgsEl.isNull() && !oNewTrgsEl.isNull());
    ASSERT_TRUE(oTrgsEl.getChildren().size() == oNewTrgsEl.getChildren().size());
    a_util::xml::DOMElement oTrgEl = oTrgsEl.getChild("target");
    a_util::xml::DOMElement oNewTrgEl = oNewTrgsEl.getChild("target");
    ASSERT_TRUE(!oTrgEl.isNull() && !oNewTrgEl.isNull());
    oOldChildren = oTrgEl.getChildren();
    oNewChildren = oNewTrgEl.getChildren();
    ASSERT_TRUE(oOldChildren.size() == oNewChildren.size());
    a_util::xml::DOMElementList::const_iterator itNewTrg = oNewChildren.begin();
    for( a_util::xml::DOMElementList::const_iterator itTrg = oOldChildren.begin();
        itTrg != oOldChildren.end(); itTrg++, itNewTrg++)
    {
        ASSERT_EQ(itTrg->getName() , itNewTrg->getName());
        ASSERT_EQ(itTrg->getAttributes() , itNewTrg->getAttributes());
        ASSERT_EQ(itTrg->getChildren().size() , itNewTrg->getChildren().size());
    }

    //Verify transformations
     a_util::xml::DOMElement oTrfEl = oDom.getRoot().getChild("transformations");
     a_util::xml::DOMElement oNewTrfEl = oNewDom.getRoot().getChild("transformations");
     ASSERT_TRUE(!oTrfEl.isNull() && !oNewTrfEl.isNull());
     oOldChildren = oTrfEl.getChildren();
     oNewChildren = oNewTrfEl.getChildren();
     ASSERT_TRUE(oOldChildren.size() == oNewChildren.size());
     a_util::xml::DOMElementList::const_iterator itTrfNew = oNewChildren.begin();
     for( a_util::xml::DOMElementList::const_iterator itTrg = oOldChildren.begin();
         itTrg != oOldChildren.end(); itTrg++, itTrfNew++)
     {
         ASSERT_TRUE(itTrg->getName() == itTrfNew->getName());
         ASSERT_TRUE(itTrg->getAttribute("name") == itTrfNew->getAttribute("name"));
         if(itTrg->getName() == "polynomial")
         {
             char c = 'a';
             a_util::result::Result res = a_util::result::SUCCESS;
             for (int i = 0; i < 5; ++i, ++c)
             {
                 a_util::xml::DOMAttributes oldAttrs = itTrg->getAttributes();
                 a_util::xml::DOMAttributes::const_iterator itAttr = oldAttrs.find(std::string(1, c));
                 a_util::xml::DOMAttributes newAttrs = itTrfNew->getAttributes();
                 a_util::xml::DOMAttributes::const_iterator itAttrNew = newAttrs.find(std::string(1, c));
                 if (itAttr != oldAttrs.end() && itAttrNew != newAttrs.end())
                 {
                     ASSERT_TRUE(a_util::strings::toDouble(itAttr->second) == a_util::strings::toDouble(itAttrNew->second));
                 }
             }
         }
         else
         {
             a_util::xml::DOMAttributes oldAttrs = itTrg->getAttributes();
             a_util::xml::DOMAttributes newAttrs = itTrfNew->getAttributes();
             a_util::xml::DOMAttributes::const_iterator itAttrNew = newAttrs.begin();
             for(a_util::xml::DOMAttributes::const_iterator itAttr = oldAttrs.begin();
                 itAttr != oldAttrs.end(); itAttr++, itAttrNew++)
             {
                 ASSERT_TRUE(itAttr->second == itAttrNew->second);
             }
             ASSERT_TRUE(itTrg->getChildren().size() == itTrfNew->getChildren().size());
         }
    }

    // Write to file
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.writeToFile("files/generated_base.map"));
    ASSERT_TRUE(a_util::filesystem::remove("files/generated_base.map"));
}

/**
* @detail Test to write a simple map configuration
*/
TEST(cTesterMapping,
    TestWriteSimpleMapConfig)
{
    TEST_REQ("CDPKGDDL-28");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    // set header description
    std::string strDesc = "New description";
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.setHeaderDescription(strDesc));

    // add Transformation
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addTransformation("poly1", "polynomial"));
    MapPolynomTransformation *pPolyTrf = dynamic_cast<MapPolynomTransformation*>(oConfig.getTransformation("poly1"));
    ASSERT_TRUE(pPolyTrf);
    std::string strCoefs[5] = {"2","1.34","","0","1e-7"};
    ASSERT_EQ(a_util::result::SUCCESS, pPolyTrf->setCoefficients(strCoefs));

    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addTransformation("enum1", "enum_table"));
    MapEnumTableTransformation *pEnumTrf = dynamic_cast<MapEnumTableTransformation*>(oConfig.getTransformation("enum1"));
    ASSERT_TRUE(pEnumTrf);
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->setEnums("tFEP_Driver_LightState", "tFEP_Driver_WiperState"));
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->setDefault("WS_OnStandard"));
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->addConversion("LS_Auto", "WS_OnStandard"));
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->addConversion("LS_Parking", "WS_Off"));
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->addConversion("LS_OnHighBeam", "WS_Auto"));


    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addTarget("target1", "tFEP_Driver_LongitudinalControl"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addTarget("target2", "tFEP_Driver_WiperControl"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addSource("source1", "tFEP_Driver_LateralControl"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addSource("source2", "tFEP_Driver_LightControl"));

    // set constant
    MapAssignment oAssign1("f32ThrottlePedal");
    ASSERT_EQ(a_util::result::SUCCESS, oAssign1.setConstant("12"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addAssignment(oAssign1));
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[0].getConstant() , "12");    

    // set connection with polynomial trasnformation
    MapAssignment oAssign2("f32BrakePedal");
    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.connect("source1.f32SteeringWheel"));
    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.setTransformation("poly1"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addAssignment(oAssign2));
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[1].getFrom() , "f32SteeringWheel");
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[1].getSource() , "source1");

    // set connection with enum trasnformation
    MapAssignment oAssignEnum("eFrontState");
    ASSERT_EQ(a_util::result::SUCCESS, oAssignEnum.connect("source2.eLightState"));
    ASSERT_EQ(a_util::result::SUCCESS, oAssignEnum.setTransformation("enum1"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target2")->addAssignment(oAssignEnum));
    ASSERT_EQ(oConfig.getTarget("target2")->getAssignmentList()[0].getFrom() , "eLightState");
    ASSERT_EQ(oConfig.getTarget("target2")->getAssignmentList()[0].getSource() , "source2");

    // set SimulationTime function
    MapAssignment oAssign3("f32ClutchPedal");
    ASSERT_EQ(a_util::result::SUCCESS, oAssign3.setSimulationTimeFunction());
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addAssignment(oAssign3));
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[2].getFunction() , "simulation_time");
    ASSERT_TRUE(oConfig.getTarget("target1")->getAssignmentList()[2].getSource().empty());

    // set TriggerCounter function
    MapAssignment oAssign4("f32AccelTarget");
    ASSERT_EQ(a_util::result::SUCCESS, oAssign4.setTriggerCounterFunction("5"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addAssignment(oAssign4));
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[3].getFunction() , "trigger_counter");
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[3].getModulo() , "5");

    // set Received function
    MapAssignment oAssign5("ui32ValidityMask");
    ASSERT_EQ(a_util::result::SUCCESS, oAssign5.setReceivedFunction("source1"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addAssignment(oAssign5));
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[4].getFunction() , "received");
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList()[4].getSource() , "source1");

    // set Triggers
    MapPeriodicTrigger* pPTrigger = new MapPeriodicTrigger(&oConfig);
    ASSERT_EQ(a_util::result::SUCCESS, pPTrigger->setPeriod("10","s"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addTrigger(pPTrigger));
    ASSERT_TRUE(oConfig.getTarget("target1")->getTriggerList()[0]->isEqual(*pPTrigger));

    MapSignalTrigger* pSTrigger = new MapSignalTrigger(&oConfig);
    ASSERT_EQ(a_util::result::SUCCESS, pSTrigger->setVariable("source1"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addTrigger(pSTrigger));
    ASSERT_TRUE(oConfig.getTarget("target1")->getTriggerList()[1]->isEqual(*pSTrigger));

    MapDataTrigger* pDTrigger = new MapDataTrigger(&oConfig);
    ASSERT_EQ(a_util::result::SUCCESS, pDTrigger->setComparison("source1.f32SteeringWheel", "less_than", "1"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addTrigger(pDTrigger));
    ASSERT_TRUE(oConfig.getTarget("target1")->getTriggerList()[2]->isEqual(*pDTrigger));

    // Write in file
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.writeToFile("files/generated_simple.map"));
    ASSERT_TRUE(a_util::filesystem::remove("files/generated_simple.map"));
}

/**
* @detail Test fail to add erroneous elements
*/
TEST(cTesterMapping,
    TestWriteMapFailConfig)
{
    TEST_REQ("CDPKGDDL-28");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    /**
    * Signals
    **/
    ASSERT_EQ(oConfig.addTarget("target", "") , ERR_INVALID_TYPE);
    ASSERT_EQ(oConfig.addSource("source", "nothing") , ERR_INVALID_TYPE);

    ASSERT_EQ(oConfig.getSourceList().size() , 0);
    ASSERT_EQ(oConfig.getTargetList().size() , 0);

    /**
    * Create basis mapping
    **/
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addTarget("target1", "tFEP_Driver_LongitudinalControl"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.addSource("source1", "tFEP_Driver_LateralControl"));
    MapAssignment oAssign1("f32ThrottlePedal");
    ASSERT_EQ(a_util::result::SUCCESS, oAssign1.setConstant("12"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("target1")->addAssignment(oAssign1));

    /**
    * Transformations
    **/
    MapPolynomTransformation *pPolyTrf = new MapPolynomTransformation(&oConfig, "poly1");
    // Coefficient not float
    std::string strCoefs[5] = {"not_a_float","1.34","","0","1e-7"};
    ASSERT_EQ(pPolyTrf->setCoefficients(strCoefs) , ERR_INVALID_ARG);

    MapEnumTableTransformation *pEnumTrf = new MapEnumTableTransformation(&oConfig, "enum1");
    // Enum does not exist
    ASSERT_EQ(pEnumTrf->setEnums("not_an_enum", "tFEP_Driver_WiperState") , ERR_INVALID_TYPE);
    ASSERT_EQ(pEnumTrf->setDefault("WS_OnStandard") , ERR_INVALID_TYPE);
    ASSERT_EQ(pEnumTrf->addConversion("LS_Auto", "WS_OnStandard") , ERR_INVALID_TYPE);
    // Default does not exist
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->setEnums("tFEP_Driver_LightState", "tFEP_Driver_WiperState"));
    ASSERT_EQ(pEnumTrf->setDefault("not_in_enum") , ERR_INVALID_TYPE);
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->addConversion("LS_Auto", "WS_OnStandard"));
    // Enum element does not exist
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->setEnums("tFEP_Driver_LightState", "tFEP_Driver_WiperState"));
    ASSERT_EQ(a_util::result::SUCCESS, pEnumTrf->setDefault("WS_OnStandard"));
    ASSERT_EQ(pEnumTrf->addConversion("not_in_enum", "WS_OnStandard") , ERR_INVALID_TYPE);

    delete pEnumTrf;
    delete pPolyTrf;
    /**
    * Assignments
    **/
    //Target does not exist
    ASSERT_FALSE(oConfig.getTarget("not_a_target"));

    // set same elements twice
    ASSERT_EQ(oConfig.addTarget("target1", "tFEP_Driver_LongitudinalControl") , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.addSource("target1", "tFEP_Driver_DriverCtrl") , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign1) , ERR_INVALID_ARG);

    ASSERT_EQ(oConfig.getSourceList().size() , 1);
    ASSERT_EQ(oConfig.getTargetList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    // set bad assignments
    MapAssignment oAssignEmpty;
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssignEmpty) , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getErrorList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    MapAssignment oAssign2("f32BrakePedal");
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign2) , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getErrorList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.connect("source1.nothing"));
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign2) , ERR_INVALID_TYPE);
    ASSERT_EQ(oConfig.getErrorList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.setConstant("text"));
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign2) , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getErrorList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.setReceivedFunction("nothing"));
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign2) , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getErrorList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    // Transformation does not exist
    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.connect("source1.f32SteeringWheel"));
    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.setTransformation("not_a_transformation"));
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign2) , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getErrorList().size() , 1);
    ASSERT_EQ(oConfig.getTarget("target1")->getAssignmentList().size() , 1);

    /**
    * Triggers
    **/
    // Period is not a number
    MapPeriodicTrigger* pPTrigger = new MapPeriodicTrigger(&oConfig);
    ASSERT_EQ(pPTrigger->setPeriod("10s","s") , ERR_INVALID_ARG);

    // Variable is not in source list
    MapSignalTrigger* pSTrigger = new MapSignalTrigger(&oConfig);
    ASSERT_EQ(pSTrigger->setVariable("not_a_source") , ERR_INVALID_ARG);
    ASSERT_EQ(oConfig.getTarget("target1")->addTrigger(pSTrigger) , ERR_INVALID_ARG);

    // Variable not an element
    MapDataTrigger* pDTrigger = new MapDataTrigger(&oConfig);
    ASSERT_EQ(pDTrigger->setComparison("source1.not_an_element", "less_than", "1") , ERR_INVALID_TYPE);
    ASSERT_EQ(oConfig.getTarget("target1")->addTrigger(pDTrigger) , ERR_INVALID_TYPE);

    // No trigger added
    ASSERT_EQ(oConfig.getTarget("target1")->getTriggerList().size() , 0);

    /** 
    * Remove dll
    **/
    oConfig.modifyDescription(NULL);
    ASSERT_EQ(a_util::result::SUCCESS, oAssign2.setConstant("12"));
    ASSERT_EQ(oConfig.getTarget("target1")->addAssignment(oAssign2) , ERR_INVALID_STATE);
    delete pDTrigger;
    delete pSTrigger;
    delete pPTrigger;
}

/**
* @detail Test load files partially when ddl not complete is
*/
TEST(cTesterMapping,
    TestLoadMapPartiallyConfig)
{
    TEST_REQ("CDPKGDDL-28");

    MapConfiguration oConfig;

    a_util::xml::DOM oDom;
    ASSERT_TRUE(oDom.load("files/base.map"));
    ASSERT_NE(a_util::result::SUCCESS, oConfig.loadPartiallyFromDOM(oDom));

    // Verify source list
    ASSERT_EQ(oConfig.getSourceList().size() , 4);
    ASSERT_TRUE(oConfig.getSource("Light"));
    ASSERT_TRUE(oConfig.getSource("Wiper"));
    ASSERT_TRUE(oConfig.getSource("AALA"));
    ASSERT_TRUE(oConfig.getSource("Additional"));
    ASSERT_EQ(oConfig.getSource("Light")->getType() , "tFEP_Driver_LightControl");
    ASSERT_EQ(oConfig.getSource("Wiper")->getType() , "tFEP_Driver_WiperControl");
    ASSERT_EQ(oConfig.getSource("AALA")->getType() , "tFEP_Driver_AalaControl");
    ASSERT_EQ(oConfig.getSource("Additional")->getType() , "tFEP_Driver_AdditionalControl");
    ASSERT_FALSE(oConfig.getSource("Light")->isValid());
    ASSERT_FALSE(oConfig.getSource("Wiper")->isValid());
    ASSERT_FALSE(oConfig.getSource("AALA")->isValid());
    ASSERT_FALSE(oConfig.getSource("Additional")->isValid());

    // Verify target list
    ASSERT_EQ(oConfig.getTargetList().size() , 1);
    ASSERT_TRUE(oConfig.getTarget("Signal"));
    ASSERT_EQ(oConfig.getTarget("Signal")->getType() , "tFEP_Driver_DriverCtrl");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList().size() , 13);
    ASSERT_FALSE(oConfig.getTarget("Signal")->isValid());

    // Cannot write incomplete mapping
    ASSERT_NE(a_util::result::SUCCESS, oConfig.writeToFile("files/generated_never_base.map"));

    // set valid DDL
    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    oConfig.modifyDescription(poDDL.get());

    // Verify source list
    ASSERT_TRUE(oConfig.getSource("Light")->isValid());
    ASSERT_TRUE(oConfig.getSource("Wiper")->isValid());
    ASSERT_TRUE(oConfig.getSource("AALA")->isValid());
    ASSERT_TRUE(oConfig.getSource("Additional")->isValid());

    // Verify target list
    ASSERT_EQ(oConfig.getTargetList().size() , 1);
    ASSERT_TRUE(oConfig.getTarget("Signal"));
    ASSERT_EQ(oConfig.getTarget("Signal")->getType() , "tFEP_Driver_DriverCtrl");
    ASSERT_EQ(oConfig.getTarget("Signal")->getAssignmentList().size() , 13);
    ASSERT_TRUE(oConfig.getTarget("Signal")->isValid());

    ASSERT_EQ(a_util::result::SUCCESS, oConfig.writeToFile("files/generated_partially_base.map"));
    ASSERT_TRUE(a_util::filesystem::remove("files/generated_partially_base.map"));
}

/**
* @detail Test to modify a mapping
*/
TEST(cTesterMapping,
    TestModifyMapConfig)
{
    TEST_REQ("CDPKGDDL-28");

    std::unique_ptr<ddl::DDLDescription> poDDL(LoadDDL("files/test.description"));
    MapConfiguration oConfig(poDDL.get());

    a_util::xml::DOM oDom;
    ASSERT_TRUE(oDom.load("files/base.map"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.loadFromDOM(oDom));

    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getSource("Light")->setName("new_Light"));
    ASSERT_TRUE(oConfig.getSource("new_Light"));
    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getSource("Additional")->setType("tFEP_Driver_LateralControl"));
    ASSERT_EQ(oConfig.getSource("Additional")->getType() , "tFEP_Driver_LateralControl");

    ASSERT_EQ(a_util::result::SUCCESS, oConfig.getTarget("Signal")->setName("new_Signal"));    
    ASSERT_TRUE(oConfig.getTarget("new_Signal"));    

    ASSERT_EQ(a_util::result::SUCCESS, oConfig.writeToFile("files/generated_modified_base.map"));
    ASSERT_TRUE(a_util::filesystem::remove("files/generated_modified_base.map"));

    // Modify target type, mapping is not valid anymore
    ASSERT_EQ(oConfig.getTarget("new_Signal")->setType("tFEP_Driver_LateralControl") , ERR_INVALID_TYPE);
    ASSERT_EQ(oConfig.getTarget("new_Signal")->getType() , "tFEP_Driver_LateralControl");
    const MapAssignmentList lst = oConfig.getTarget("new_Signal")->getAssignmentList();
    for(MapAssignmentList::const_iterator it = lst.begin(); it != lst.end(); it++)
    {
        if(it->getTo() == "ui32ValidityMask")
        {
            ASSERT_TRUE(it->isValid());
        }
        else
        {
            ASSERT_FALSE(it->isValid());
        }
    }
}

/// stripped down version of the internal helper class fep::cDDLManager used below
class TestDDLManager
{
    ddl::DDLDescription* m_poDDL;

public:
    TestDDLManager() : m_poDDL(NULL)
    {
        m_poDDL = DDLDescription::createDefault();
    }

    ~TestDDLManager()
    {
        delete m_poDDL;
        m_poDDL = NULL;
    }

    a_util::result::Result loadDDLFile(const std::string& strFile)
    {
        std::string strDDL;
        if (a_util::filesystem::readTextFile(strFile, strDDL) != a_util::filesystem::OK)
        {
            return ERR_INVALID_FILE;
        }

        ddl::DDLImporter oImp;
        a_util::result::Result nRes = oImp.setXML(strDDL);
        if (a_util::result::isOk(nRes))
        {
            nRes = oImp.createNew();
        }

        if (a_util::result::isFailed(nRes))
        {
            nRes = ERR_INVALID_ARG;
        }

        if (a_util::result::isOk(nRes))
        {
            delete m_poDDL;
            m_poDDL = oImp.getDDL();   
        }

        return nRes;
    }

    a_util::result::Result ResolveType(const std::string& strType, std::string& strDestination) const
    {
        a_util::result::Result nRes = a_util::result::SUCCESS;
        if (strType.empty())
        {
            nRes = ERR_NOT_FOUND;
        }

        if (a_util::result::isOk(nRes))
        {
            nRes = ERR_NOT_FOUND;

            // since the DDL resolver searches units, datatypes, structs and streams
            // we need to make sure first that the type is a struct - since only them are supported
            DDLComplexVec vecStructs = m_poDDL->getStructs();
            DDLComplexIt itStruct =
                std::find_if(vecStructs.begin(), vecStructs.end(), DDLCompareFunctor<>(strType));

            if (itStruct != vecStructs.end())
            {
                DDLResolver oDDLResolver;
                oDDLResolver.setTargetName(strType);
                if (a_util::result::isOk(oDDLResolver.visitDDL(m_poDDL)))
                {
                    strDestination = oDDLResolver.getResolvedXML();
                    nRes = a_util::result::SUCCESS;
                }
            }
        }
        return nRes;
    }

    const ddl::DDLDescription& getDDL() const
    {
        return *m_poDDL;
    }
};

/// Test class that drives the mapping engine in a standalone fashion
/// It also provides easy access to source and target buffers to make the test easy to read
#pragma warning(disable: 4355)
class MappingDriver : private IMappingEnvironment
{
    struct Signal
    {
        std::string strName;
        std::string strType;
        ISignalListener* pListener;
    };

    struct sPeriodicWrapper
    {
        a_util::system::Timer oTimer;
        mapping::rt::IPeriodicListener* pListener;
        sPeriodicWrapper() : pListener(NULL) {}

        void TimerFunc()
        {
            pListener->onTimer(a_util::system::getCurrentMicroseconds());
        }
    };
    typedef std::map<mapping::rt::IPeriodicListener*, sPeriodicWrapper*> tPeriodicWrappers;

    TestDDLManager m_oManager;
    MapConfiguration m_oConfig;
    MappingEngine m_oEngine;
    std::map<std::string, Signal> mapSources;
    std::map<std::string, a_util::memory::shared_ptr<ddl::StaticCodec> > mapSourceCoders;
    std::map<std::string, Target::MemoryBuffer> mapSourceBuffers;
    std::map<std::string, a_util::memory::shared_ptr<ddl::StaticCodec> > mapTargetCoders;
    std::map<std::string, Target::MemoryBuffer> mapTargetBuffers;
    std::map<std::string, handle_t> mapTargetHandle;
    std::map<handle_t, std::string> mapHandleTarget;
    tPeriodicWrappers m_mapPeriodicWrappers;

public:
    MappingDriver(const std::string& strDDL, const std::string& strMapping) : m_oEngine(*this)
    {
        setup(strDDL, strMapping);
    }

    ~MappingDriver()
    {
        m_oEngine.stop();
        m_oEngine.unmapAll();
        mapSourceCoders.clear();
        mapTargetCoders.clear();
    }

    void setup(const std::string& strDDL, const std::string& strMapping)
    {
        // setup engine
        ASSERT_EQ(a_util::result::SUCCESS, m_oManager.loadDDLFile(strDDL));
        ASSERT_EQ(a_util::result::SUCCESS, m_oConfig.setDescription(&m_oManager.getDDL()));
        ASSERT_EQ(a_util::result::SUCCESS, m_oConfig.loadFromFile(strMapping, MapConfiguration::mc_load_mapping));
        ASSERT_EQ(a_util::result::SUCCESS, m_oEngine.setConfiguration(m_oConfig));

    }

    void addTarget(const std::string& strTarget)
    {
        ASSERT_EQ(mapTargetCoders.find(strTarget) , mapTargetCoders.end());

        // map target
        ASSERT_EQ(a_util::result::SUCCESS, m_oEngine.Map(strTarget, mapTargetHandle[strTarget]));
        mapHandleTarget[mapTargetHandle[strTarget]] = strTarget;
        const MapTarget* pTarget = m_oConfig.getTarget(strTarget);

        // create source buffers and coders
        const MapSourceNameList& lstSources = pTarget->getReferencedSources();
        for (MapSourceNameList::const_iterator it = lstSources.begin(); it != lstSources.end(); ++it)
        {
            const MapSource* pMapSource = m_oConfig.getSource(*it);
            if(mapSourceCoders.find(std::string(pMapSource->getName())) == mapSourceCoders.end())
            {
                std::string strSourceDesc;
                ASSERT_EQ(a_util::result::SUCCESS, m_oManager.ResolveType(pMapSource->getType(), strSourceDesc));

                ddl::CodecFactory oFac(pMapSource->getType().c_str(), strSourceDesc.c_str());
                ASSERT_EQ(a_util::result::SUCCESS, oFac.isValid());

                Target::MemoryBuffer& oSourceBuffer = mapSourceBuffers[pMapSource->getName()];
                oSourceBuffer.resize(oFac.getStaticBufferSize(), 0);

                mapSourceCoders.insert(std::make_pair(std::string(pMapSource->getName()),
                    a_util::memory::shared_ptr<ddl::StaticCodec>(new ddl::StaticCodec(
                    oFac.makeStaticCodecFor(&oSourceBuffer[0], oSourceBuffer.size())))));
            }
        }

        // create target buffer and coder
        std::string strTargetDesc;
        ASSERT_EQ(a_util::result::SUCCESS, m_oManager.ResolveType(pTarget->getType(), strTargetDesc));

        ddl::CodecFactory oFac(pTarget->getType().c_str(), strTargetDesc.c_str());
        ASSERT_EQ(a_util::result::SUCCESS, oFac.isValid());

        Target::MemoryBuffer& oTargetBuffer = mapTargetBuffers[strTarget];
        oTargetBuffer.resize(oFac.getStaticBufferSize(), 0);

        mapTargetCoders.insert(std::make_pair(strTarget, a_util::memory::shared_ptr<ddl::StaticCodec>(new ddl::StaticCodec(
            oFac.makeStaticCodecFor(&oTargetBuffer[0], oTargetBuffer.size())))));
        ddl::StaticCodec& oTargetCoder = *mapTargetCoders[strTarget];
    }

    void resetEngine()
    {
        // reset engine
        ASSERT_EQ(a_util::result::SUCCESS, m_oEngine.reset());
    }

    void startEngine()
    {
        // start engine
        ASSERT_EQ(a_util::result::SUCCESS, m_oEngine.start());
    }

    void stopEngine()
    {
        // start engine
        ASSERT_EQ(a_util::result::SUCCESS, m_oEngine.stop());
    }

    bool targetHasTriggers(const std::string& signal_name)
    {
        // look for trigger
        handle_t target_handle = mapTargetHandle[signal_name];
        return m_oEngine.hasTriggers(target_handle);
    }

    ddl::StaticCodec& getTargetCoder(const std::string& strTarget)
    {
        return *mapTargetCoders[strTarget];
    }

    ddl::StaticCodec& getSourceCoder(const std::string& strSource)
    {
        return *mapSourceCoders[strSource];
    }

    a_util::result::Result sendSourceBuffer(const std::string& strSource)
    {
        Target::MemoryBuffer& oSourceBuf = mapSourceBuffers[strSource];
        return mapSources[strSource].pListener->onSampleReceived(&oSourceBuf[0], oSourceBuf.size());
    }

    a_util::result::Result receiveTargetBuffer(const std::string& strTarget)
    {
        Target::MemoryBuffer& oTargetBuffer = mapTargetBuffers[strTarget];
        handle_t hTargetHandle = mapTargetHandle[strTarget];
        return m_oEngine.getCurrentData(hTargetHandle, &oTargetBuffer[0], oTargetBuffer.size());
    }

private:
    a_util::result::Result registerSource(const char* strSourceName, const char* strTypeName,
        ISignalListener* pListener, handle_t& hHandle) 
    {
        Signal sig;
        sig.strName = strSourceName;
        sig.strType = strTypeName;
        sig.pListener = pListener;
        mapSources[strSourceName] = sig;
        hHandle = (handle_t)&mapSources[strSourceName];
        return a_util::result::SUCCESS;
    }

    a_util::result::Result unregisterSource(handle_t hHandle) 
    {   // dont care
        return a_util::result::SUCCESS;
    }

    a_util::result::Result sendTarget(handle_t hTarget, const void* pData,
        size_t szSize, timestamp_t tmTimeStamp) 
    {
        if(szSize == mapTargetBuffers[mapHandleTarget[hTarget]].size())
        {
            a_util::memory::copy(&mapTargetBuffers[mapHandleTarget[hTarget]][0], szSize, pData, szSize);
        }
        return a_util::result::SUCCESS;
    }

    a_util::result::Result targetMapped(const char* strTargetName,
        const char* strTargetType, handle_t hTarget, size_t szTargetType) 
    {
        return a_util::result::SUCCESS;
    }

    a_util::result::Result targetUnmapped(const char* strTargetName, handle_t hTarget) 
    {
        return a_util::result::SUCCESS;
    }

    a_util::result::Result resolveType(const char* strTypeName, const char*& strTypeDescription) 
    {
        resolveTypeTest(strTypeName, strTypeDescription);
        return a_util::result::SUCCESS;
    }

    void resolveTypeTest(const char* strTypeName, const char*& strTypeDescription)
    {
        static std::string strDesc;
        ASSERT_EQ(a_util::result::SUCCESS, m_oManager.ResolveType(strTypeName, strDesc));
        strTypeDescription = strDesc.c_str();
    }

    timestamp_t getTime() const
    {
        return a_util::system::getCurrentMicroseconds();
    }

    a_util::result::Result registerPeriodicTimer(timestamp_t tmPeriod_us, mapping::rt::IPeriodicListener* pListener)
    {
        sPeriodicWrapper* pWrap = new sPeriodicWrapper;
        pWrap->pListener = pListener;

        m_mapPeriodicWrappers[pListener] = pWrap;
        pWrap->oTimer.setPeriod(tmPeriod_us + 1000);
        pWrap->oTimer.setCallback(&sPeriodicWrapper::TimerFunc, *pWrap);
        pWrap->oTimer.start();
        return a_util::result::SUCCESS;
    }

    a_util::result::Result unregisterPeriodicTimer(timestamp_t tmPeriod_us, mapping::rt::IPeriodicListener* pListener)
    {
        tPeriodicWrappers::iterator it = m_mapPeriodicWrappers.find(pListener);
        delete it->second;
        m_mapPeriodicWrappers.erase(it);
        return a_util::result::SUCCESS;
    }
};

/**
* @detail Test Engine with default values
*/
TEST(cTesterMapping,
    TestDefaultEngine)
{
    TEST_REQ("FEPSDK-278");

    MappingDriver base_test("files/engine.description", "files/engine_default.map");
    base_test.addTarget("OutSignal");
    base_test.startEngine();

    ddl::StaticCodec& oTarget = base_test.getTargetCoder("OutSignal");
    // Target in engine_default.map has no triggers
    ASSERT_EQ(false, base_test.targetHasTriggers("OutSignal"));
    // copy target buffer initially
    base_test.receiveTargetBuffer("OutSignal");

    a_util::variant::Variant var;

    // test ddl defaults
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i8Val").asInt8() , 42);

    // test mapping contants defaults
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i16Val").asInt16() , 3);

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "enumValDef").asInt16() , 60);

    // test mapping contants defaults in structure
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "structMinimal.i8Val").asInt8() , 4);
}

/**
* @detail Test Engine for direct assignments
*/
TEST(cTesterMapping,
    TestDirectMappingEngine)
{
    TEST_REQ("FEPSDK-278");

    MappingDriver base_test("files/engine.description", "files/engine_directmapping.map");
    base_test.addTarget("OutSignal");
    base_test.addTarget("OutSignal2");
    base_test.startEngine();

    ddl::StaticCodec& oTarget = base_test.getTargetCoder("OutSignal");
    ddl::StaticCodec& oTarget2 = base_test.getTargetCoder("OutSignal2");

    ddl::StaticCodec& oSource1 = base_test.getSourceCoder("MinimalSignal");
    ddl::StaticCodec& oSource2 = base_test.getSourceCoder("InSignal");
    ddl::StaticCodec& oSource3 = base_test.getSourceCoder("MinimalSignal2");

    // Targets in engine_directmapping.map have no triggers
    ASSERT_EQ(false, base_test.targetHasTriggers("OutSignal"));
    ASSERT_EQ(false, base_test.targetHasTriggers("OutSignal2"));

    bool bVal = false;

    int8_t i8Val = 0;
    int16_t i16Val = 0;
    int32_t i32Val = 0;
    int64_t i64Val = 0;

    uint8_t ui8Val = 0;
    uint16_t ui16Val = 0;
    uint32_t ui32Val = 0;
    uint64_t ui64Val = 0;

    float f32Val = 0;

    // Test mapping input with same type
    i32Val = 42;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal");
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i32Val").asInt32() , 42);

    // Test mapping input with different type
    f32Val = 42;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "f32Val", a_util::variant::Variant(f32Val)));
    base_test.sendSourceBuffer("MinimalSignal");
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i64Val").asInt64() , 42);

    // Test mapping input array element with same type
    f32Val = 1;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "f32Ary[0]", a_util::variant::Variant(f32Val)));
    base_test.sendSourceBuffer("InSignal");                         
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "f32Ary[0]").asFloat() , 1);

    // Test mapping input array element with different type
    i32Val = 1;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal");                    
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "f32Ary[1]").asFloat() , 1);

    // Test mapping enum
    i16Val = 3;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");                       
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "enumVal").asInt16() , 3);

    // Test mapping input structure element with same type
    i16Val = 5;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i16Val", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("MinimalSignal");                    
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "structMinimal.i16Val").asInt16() , 5);

    // Test mapping input structure element with same type
    i32Val = 5;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "structMinimal.i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("InSignal");                        
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "structMinimal.i32Val").asInt32() , 5);

    // Test mapping input structure element with different type
    ui32Val = 5;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "structMinimal.ui32Val", a_util::variant::Variant(ui32Val)));
    base_test.sendSourceBuffer("InSignal");                        
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "structMinimal.i64Val").asInt64() , 5);

    // Test mapping input structure with same type
    i16Val = 15;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i16Val", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("MinimalSignal");                    
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "structMinimalAry[0].i16Val").asInt16() , 15);

    /**
    * Test second Target
    **/
    base_test.receiveTargetBuffer("OutSignal2");

    // test ddl defaults
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "i8Val").asInt8() , 42);

    // test mapping contants defaults
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "i16Val").asInt16() , 3);

    // Test mapping input structure with same type
    i16Val = 15;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource3, "i16Val", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("MinimalSignal2");
    base_test.receiveTargetBuffer("OutSignal2");

    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "structMinimalAry[0].i16Val").asInt16() , 15);        
}

/**
* @detail Test Engine for assignments with transformation
*/
TEST(cTesterMapping,
    TestTransformationsEngine)
{
    TEST_REQ("FEPSDK-278");

    MappingDriver base_test("files/engine.description", "files/engine_transformations.map");
    base_test.addTarget("OutSignal");
    base_test.addTarget("OutSignal2");
    base_test.startEngine();

    ddl::StaticCodec& oTarget = base_test.getTargetCoder("OutSignal");
    ddl::StaticCodec& oTarget2 = base_test.getTargetCoder("OutSignal2");

    ddl::StaticCodec& oSource1 = base_test.getSourceCoder("MinimalSignal");
    ddl::StaticCodec& oSource2 = base_test.getSourceCoder("InSignal");

    // Targets in engine_transformations.map have no triggers
    ASSERT_EQ(false, base_test.targetHasTriggers("OutSignal"));
    ASSERT_EQ(false, base_test.targetHasTriggers("OutSignal2"));
    // Sources also have no triggers
    ASSERT_EQ(false, base_test.targetHasTriggers("InSignal"));
    ASSERT_EQ(false, base_test.targetHasTriggers("MinimalSignal"));

    bool bVal = false;

    int8_t i8Val = 0;
    int16_t i16Val = 0;
    int32_t i32Val = 0;
    int64_t i64Val = 0;

    uint8_t ui8Val = 0;
    uint16_t ui16Val = 0;
    uint32_t ui32Val = 0;
    uint64_t ui64Val = 0;

    float f32Val = 0;

    // Test mapping input with same type and transformation
    ui8Val = 4;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "ui8Val", a_util::variant::Variant(ui8Val)));
    base_test.sendSourceBuffer("MinimalSignal");
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "ui8Val").asUInt8() , 6);

    // Test mapping input with different type and transformation
    f32Val = 4;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "f32Val", a_util::variant::Variant(f32Val)));
    base_test.sendSourceBuffer("MinimalSignal");
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "ui16Val").asUInt16() , 6);

    // Test mapping input array with same type with transformation
    i8Val = 4;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "i8Ary[0]", a_util::variant::Variant(i8Val)));
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "i8Ary[9]", a_util::variant::Variant(i8Val)));
    base_test.sendSourceBuffer("InSignal");  
    base_test.receiveTargetBuffer("OutSignal");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i8Ary[0]").asInt8() , 6);
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i8Ary[9]").asInt8() , 6);

    /**
    * Test second Target
    **/
    base_test.receiveTargetBuffer("OutSignal2");
    // Test mapping input with different type and transformation
    f32Val = 4;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "f32Val", a_util::variant::Variant(f32Val)));
    base_test.sendSourceBuffer("MinimalSignal");
    base_test.receiveTargetBuffer("OutSignal2");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "ui16Val").asUInt16() , 6);

    // Test mapping input array with different type with transformation
    f32Val = 4;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "f32Ary[0]", a_util::variant::Variant(f32Val)));
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "f32Ary[9]", a_util::variant::Variant(f32Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");

    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i8Ary[0]").asInt8() , 6);
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "i8Ary[9]").asInt8() , 6);

    // Test enum to enum transformation with same enum type
    // Test conversion
    i16Val = 20;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 10);

    i16Val = 40;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 40);

    i16Val = 50;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 40);

    i16Val = 10;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 10);

    i16Val = 29;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 34);

    i16Val = 52;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 35);

    // Test Default
    i16Val = 60;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal").asInt16() , 0);

    // Test enum to enum transformation with different enum types
    i16Val = 20;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal2").asInt32() , 1);

    i16Val = 40;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal2").asInt32() , 2);

    i16Val = 60;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource2, "enumVal", a_util::variant::Variant(i16Val)));
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal2");
    ASSERT_EQ(ddl::access_element::get_value(oTarget2, "enumVal2").asInt32() , 3);
}

/**
* @detail Test Engine for macro assignments
*/
TEST(cTesterMapping,
    TestMacrosEngine)
{
    TEST_REQ("FEPSDK-278");

    MappingDriver base_test("files/engine.description", "files/engine_macros.map");
    base_test.addTarget("OutSignal");
    base_test.startEngine();

    ddl::StaticCodec& oTarget = base_test.getTargetCoder("OutSignal");

    ddl::StaticCodec& oSource1 = base_test.getSourceCoder("MinimalSignal");
    ddl::StaticCodec& oSource2 = base_test.getSourceCoder("InSignal");

    // Target in engine_macros.map has triggers
    ASSERT_EQ(true, base_test.targetHasTriggers("OutSignal"));

    bool bVal = false;

    uint32_t ui32Val = 0;
    uint64_t ui64Val = 0;

    // Test mapping with simulation_time
    for (int i = 0; i < 10; ++i)
    {
        timestamp_t tmTime = a_util::system::getCurrentMicroseconds();
        base_test.receiveTargetBuffer("OutSignal");
        ASSERT_TRUE(ddl::access_element::get_value(oTarget, "ui64Val").asUInt64() >= (uint64_t)tmTime);
    }

    // Test mapping with trigger_counter
    // Trigger three times
    base_test.sendSourceBuffer("InSignal");
    base_test.sendSourceBuffer("InSignal");
    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal");
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "ui32Val").asUInt32() , 3);

    /**
    * Test Reset Engine
    **/
    base_test.stopEngine();
    base_test.resetEngine();
    base_test.startEngine();

    // Test mapping with trigger_counter
    base_test.receiveTargetBuffer("OutSignal");
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "ui32Val").asUInt32() , 0);

    base_test.sendSourceBuffer("InSignal");
    base_test.receiveTargetBuffer("OutSignal");
    ASSERT_EQ(ddl::access_element::get_value(oTarget, "ui32Val").asUInt32() , 1);

    // Test received Boolean
    ASSERT_FALSE(ddl::access_element::get_value(oTarget, "bVal").asBool());
    base_test.sendSourceBuffer("MinimalSignal");
    base_test.receiveTargetBuffer("OutSignal");
    ASSERT_TRUE(ddl::access_element::get_value(oTarget, "bVal").asBool());
}

/**
* @detail Test Engine for triggers
*/
TEST(cTesterMapping,
    TestTriggersEngine)
{
    TEST_REQ("FEPSDK-278");

    MappingDriver base_test("files/engine.description", "files/engine_triggers.map");
    base_test.addTarget("OutSignal3");
    base_test.startEngine();

    ddl::StaticCodec& oTarget3 = base_test.getTargetCoder("OutSignal3");

    ddl::StaticCodec& oSource1 = base_test.getSourceCoder("MinimalSignal");
    ddl::StaticCodec& oSource2 = base_test.getSourceCoder("InSignal");

    // You can guess if engine_triggers.map contains triggers 
    ASSERT_EQ(true, base_test.targetHasTriggers("OutSignal3"));

    base_test.receiveTargetBuffer("OutSignal3");
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 0 % 5);

    // Test signal Trigger
    base_test.sendSourceBuffer("InSignal");
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 1 % 5);

    // Test data Trigger
    int32_t i32Val = -42;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal"); // fires not_equal, less_than and less_than_equal
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 4 % 5);

    i32Val = -1;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal"); // fires equal, not_equal, less_than and less_than_equal
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 8 % 5);

    i32Val = 42;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal"); // fires not_equal, greater_than and greater_than_equal
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 11 % 5);

    i32Val = 3;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal"); // fires not_equal and less_than_equal
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 13 % 5);

    i32Val = 4;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal"); // fires not_equal and greater_than_equal
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 15 % 5);

    i32Val = 2;
    ASSERT_EQ(a_util::result::SUCCESS, ddl::access_element::set_value(oSource1, "i32Val", a_util::variant::Variant(i32Val)));
    base_test.sendSourceBuffer("MinimalSignal"); // fires less_than and less_than_equal
    ASSERT_EQ(ddl::access_element::get_value(oTarget3, "ui32Val").asUInt32(), 17 % 5);
}