# Signal Mapping Format Specification {#page_signal_mapping_spec}

# Configuration format

The actual mapping of signals is configured using a xml-based configuration format.

## Overview
A mapping configuration file contains a header with meta information about the file, source signal declarations, target signal definitions and transformation definitions. A source signal consists of a name and a signal type. Target signals also consist of a name and a signal type. They define any number of signal element assignments as well as any trigger conditions.

Conceptionally, signal element assignments define how the target signal is assembled while triggers define when the target signal buffer is submitted to the user.

A target signal element can be assigned a constant numeric value or a source signal element. In the later case, a transformation can be used to alter the source signal element value during assignment. For details see \ref subsec_features_types. Trigger conditions are optional, since it is always possible to actively get the current target signal buffer. For now, only periodic triggers can be defined, using a period and a unit of time.

## Specification

This specification contains the following parts:
- Header
- Sources
- Targets
- Transformations

These sections will be described in the following paragraphs, see the [XSD specification](../mapping_configuration.xsd) for more details. 


### Header

The header section contains meta information about the document and version information. 
It is similar to the header informations in the DDL Definition File Format.

|Tag|Type|Required|Description|
|--- |--- |--- |--- |
|language_version|Float|mandatory|Version number of the mapping format|
|author|String|mandatory|Author|
|date_creation|Date|mandatory|Creation date|
|date_change|Date|mandatory|Last modification date|
|description|Text|mandatory|Short description|


### Sources

This section defines the source signals that can be used in assignments to any target signal elements(s).

The tag `&lt;source&gt;` supports the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|Name of the signal|
|type|String|mandatory|DDL structured type of the signal|


### Targets

This section contains definitions of target signals, assignement to their target elements as well as any trigger definitions.

The tag `&lt;target&gt;` supports the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|Name of the signal|
|type|String|mandatory|DDL structured type of the signal|

The tag `&lt;assignment&gt;` supports the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|to|String|mandatory|Name of the target signal element to be assign|
|constant|String|optional*|Float value which will assigned during initialisation|
|function|String|optional*|Macro to assign|
|from|String|optional*|Source signal or source signal element to assign|
|transformation|String|optional|Transformation to apply to the source element|


__*__: The attributes _constant_, _function_ and _from_ are mutually exclusive but one of them is required.
The attribute _transformation_ can only be applied when the attribute _from_ is defined.

The attribute _function_ can have the values:

|Function|Parameter|Description|
|--- |--- |--- |
|simulation_time()|None|Evaluates to the current simulation time|
|trigger_counter()|None|Trigger counter, incremented whenever a trigger for the target fires|
|trigger_counter()|Positive numeric value, modulo parameter|Trigger counter with wrap-around|
|received()|Name of the source signal|Received indicator, evaluates to true if the source signal has been received|


The tag `&lt;trigger&gt;` supports the following attributes:
 
|Name|Type|Required|Description|
|--- |--- |--- |--- |
|type|String|mandatory|Trigger type|
|period|String|optional|Float value defining the length of the period|
|unit|String|optional|unit to interpret the given _period_|
|variable|String|optional|For _signal_ triggers: name of the source signal, for _data triggers: name of the source signal element|
|operator|String|optional|Operator used for data triggers: less_than, greater_than, less_than_equal, greater_than_equal, equal or not_equal|
|value|String|optional|Float value to compare with the source element|


The attribute _type_ has three acceptable values:
- _periodic_
- _data_
- _signal_
 
For the _periodic_ Trigger type, the attributes _period_ and _unit_ are mandatory.
For the _data_ Trigger type, the attributes _variable_, _operator_ and _value_ are mandatory.
For the _signal_ Trigger type, the attribute _variable_ is mandatory.

### Transformations

This section contains definitions of transformations that can be used in assignements. 
For the moment the only transformations available are polynomial tranformations and enum to enum transformations.

The tag `&lt;polynomial&gt;` supports the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|Name of the transformation|
|a|String|optional|parameter of the polynom for X^0|
|b|String|optional|parameter of the polynom for X^1|
|c|String|optional|parameter of the polynom for X^2|
|d|String|optional|parameter of the polynom for X^3|
|e|String|optional|parameter of the polynom for X^4|


The tag `&lt;enum_table&gt;` supports the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|Name of the transformation|
|from|String|mandatory|Enumeration type used as transformation source|
|to|String|mandatory|Enumeration type used as transformation target|
|default|String|mandatory|Default value used when no conversion is defined. It must be an element of the target enumeration|


The tag `&lt;conversion&gt;` supports the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|from|String|mandatory|Element from the enumeration type used as transformation source|
|to|String|mandatory|Element from the enumeration type used as transformation target|


## Example
The following example shows the mapping of the target signals `LightSource` and `Object`. 
`LightSource` elements are mapped from two source signals, `LightOrientation` and `LightPos`, as well as some constants.
 Two of its assignments are also transformed during mapping. The entire target is triggered by a periodic trigger with a period of 5s, and when LightPos is received and LightPos.f64X is lesser than 2. 
 `Object` element is an enumeration, mapped with transformation from `SourceObject`. 
 The Enumeration Type _tObjectType_ is mapped in an obsolete Enumeration created for the example.
 This Target is trigger when `LightOrientation` is received.
 
````xml
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<mapping>
    <header>
        <language_version>1.00</language_version>
        <author>AUDI Electronic Ventures GmbH</author>
        <date_creation>2016-Jun-08</date_creation>
        <date_change>2016-Jun-08</date_change>
        <description>Example mapping configuration</description>
    </header>
    <sources>
        <source name="LightOrientation" type="tFEP_VU_Coord" />
        <source name="LightPos" type="tFEP_VU_PointCartesian" />
        <source name="SourceObject" type="tObject" />
    </sources>
    <targets>
        <target name="LightSource" type="tFEP_VU_LightSource">
            <assignment to="f64SimTime" function="simulation_time()" />
            <assignment to="ui32Id" constant="1" />
            <assignment to="ui8State" function="trigger_counter(3)" />
            <assignment to="sPosIntertial.f64X" from="LightPos.f64X" transformation="cm_to_m" />
            <assignment to="sPosIntertial.f64Y" from="LightPos.f64Y" transformation="cm_to_m" />
            <assignment to="sPosIntertial.f64Z" constant="0" />
            <assignment to="sPosIntertial.f64H" from="LightOrientation.f64H" />
            <assignment to="sPosIntertial.f64P" from="LightOrientation.f64P" />
            <assignment to="sPosIntertial.f64R" from="LightOrientation.f64R" />
            <trigger type="periodic" period="5" unit="s"/>            
            <trigger type="data" variable="LightPos.f64X" operator="greater_than" value="2"/>
        </target>
        <target name="Object" type="tObjectObsolete">
            <assignment to="objectType" from="SourceObject.objectType" transformation="table1" />
            <trigger type="signal" variable="LightOrientation"/>
        </target>
    </targets>
    <transformations>
        <polynomial name="cm_to_m" a="0" b="0.01" />
        <enum_table name="table1" from="tObjectType" to="tObjectTypeObsolete" default="OT_Undefined" >
            <conversion from="OT_Car" to="OT_Vehicle"/>
            <conversion from="OT_Truck" to="OT_Vehicle"/>
            <conversion from="OT_Van" to="OT_Vehicle"/>
            <conversion from="OT_Motorbike" to="OT_Vehicle"/>
            <conversion from="OT_Bicycle" to="OT_Vehicle"/>
            <conversion from="OT_Pedestrian" to="OT_Human"/>
            <conversion from="OT_Animal" to="OT_Animal"/>
        </enum_table>
    </transformations>
</mapping>
````

# Features

## Constants and Macros

A target signal element can be assigned a constant numeric value. This value will be parsed as a floating point number and casted to the target element type.

A target signal element can also be assigned the simulation time, a trigger counter or a received flag.

**Note:**

Constants and Macros can only be used to initialize scalar as well as arrays of scalar elements
Any unassigned target elements will be assigned the default value configured in the DDL definition of the target signal type

## Transformations

Transformations can be used to alter the value of an assignment during runtime. 
Polynomial and enum to enum transformations are supported.

Polynomial transformations use the following polynomial to transform the assignments:

````
value = a*value^0 + b*value^1 + c*value^2 + d*value^3 + e*value^4
````

Any undefined coefficients are set to 0.


**Note:**

Transformations are only supported for scalar as well as arrays of scalar elements

## Supported types and cross-type assignments

Assignments from one type to another are supported in the following way

Source               | Target                | Specification
-------------         | -------------          | -------------
Constant            | Scalar                 | Datatype conversion using standard C++ casting methods
Simulation time  | Scalar                 | Datatype conversion using standard C++ casting methods
Trigger counter   | Scalar                 | Datatype conversion using standard C++ casting methods
Received Flag     | Scalar                 | Only for boolean
Scalar                | Scalar                 | Datatype conversion using standard C++ casting methods. Transformations allowed
Array<Scalar>     | Array<Scalar>     | Only for arrays of same size. Datatype conversion using standard C++ casting methods. Transformations allowed
Structure            | Structure             | Only for structures of same type
Array<Structure> | Array<Structure> | Only for arrays of same size and structures of same type
Enum                 | Enum                 | Datatype conversion to the underlying numerical datatype using standard C++ casting methods
Array<Enum>     | Array<Enum>       | Only for arrays of same size. Datatype conversion using standard C++ casting methods.

Scalar datatypes are enumerations, `tBool`, `tChar`, `tUInt8`, `tInt8`, `tUInt16`, `tInt16`, `tUInt32`, `tInt32`, `tUInt64`, `tInt64`, `tFloat32` and `tFloat64`.

Array and structure elements can be used separately in assignments, for example:

````xml
<assignment to="structMinimal.i64Val" from="InSignal.structMinimal.ui32Val" />
<assignment to="structMinimalAry[0]" from="MinimalSignal" />
````

**Note:**

A target element can only be assigned once. For instance, if one element in a substructure is already assigned, the whole substructure can not be assigned and reciprocally.

Target and source signals may only use structured types

