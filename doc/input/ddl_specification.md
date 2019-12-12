# DDL Definition File Format {#page_ddl_specification}

With ADTF a default DDL Definition File 'adtf.description' is provided. The
format of this file is XML and it contains all common datatypes and structs
which are used within ADTF.

Which DDL Definition File is used in ADTF can be specified either in global
settings or in configuration settings. *It is possible to specify more than
one DDL Definition File within the property 'media_descripton_files' by
separating them with a semicolon (;)*. If more than one DDL Definition
File is provided, the contained information will be merged.

The DDL were released with several versions. This table will show the
the supported DDL versions of different ADTF releases.

**Warning:** 
Keep in mind, that the DDL language version 1.0 is not supported
within ADTF.

*The currently valid version is DDL 3.0.*

|ADTF Version|DDL Version supported|
|--- |--- |
|2.4.x|1.0+|
|2.5.x|1.0+|
|2.6.x|1.0+|
|2.7.x|1.0+ and 1.02|
|2.8.x|1.0+, 1.02, and 2.0|
|2.9.x|1.0+, 1.02, and 2.0|
|2.10.x and newer|1.0+, 1.02, 2.0, and 3.0|
|3.0.x and newer|1.0+, 1.02, 2.0, 3.0 and 4.0|

Changes to definitions between the language versions are explicitly explained in the tables below.

&nbsp;

# Specification

The newer versions of the DDL Definition File Format are also specified by a XSD specification file.
See the XSD specification for further details:
- [DDL 3 XSD specification](../ddl3.xsd)
- [DDL 4 XSD specification](../ddl4.xsd)

# Description of General Datatypes in this specification

There are several types that are used in attributes and as tag data in the
DDL. The following table lists the types with a description of the values
and/or their formatting.

|Type|Allowed values|
|--- |--- |
|Char|Single character of [a-z][A-Z][0-9] _.-+/|
|String|Characters of [a-z][A-Z][0-9] _.-+/|
|Text|All visible ASCII characters|
|UInt|Unsigned integer values|
|Int|signed integer values|
|Float|signed float values|
|Date|Allowed formats: yyyymmdd, dd-mm-yyyy, yyyy-mm-dd or dd.mm.yyyy|
|Class|any name value of another DDL element|
|Enum|A predefined value from the description of the attribute|


ADTF provides several predefined baseunits, prefixes, datatypes and enums.

&nbsp;

# header
The header section contains meta information about the document and version
information. Example:
````xml
<header>
     <language_version>2.0</language_version>
     <author>AUDI Electronics Venture GmbH</author>
     <date_creation>20100407</date_creation>
     <date_change />
     <description>ADTF Common Description File</description>
</header>
````

&nbsp;

|Tag|Type|Description|
|--- |--- |--- |
|language_version|Float|Version number of the file|
|author|String|Author|
|date_creation|Date|Creation date|
|date_change|Date|Last modification date|
|description|Text|Short description|


All previously mentioned header tags are *mandatory*.

Additional information can be added by using the `ext_declaration` tag.
Example:

````xml
<header>
     ...
     <ext_declaration key="AnyKey" value="Any value for this key"/>
</header> 
````

&nbsp;

|Attribute|Type|Description|
|--- |--- |--- |
|key|String|Name of the additional information|
|Value|Text|Value of the additional information|

&nbsp;

# units

The definition of units will be divided in SI-base units and own units. The
SI-based units are the following ones:

|SI (see ISO 1000)|no SI, but needed for daily usage|
|--- |--- |
|Metre|Degree|
|Gram|Radiant|
|Second|Unitless|
|Ampere| |
|Candela| |
|Kelvin| |
|Mole| |

Units are defined within the tags `<units>` and `</units>`.
Example: 
````xml
<units>
       <baseunit description="Fundamental unit for electric current" name="Ampere" symbol="A" />
       <baseunit description="Fundamental unit for luminous intensity" name="Candela" symbol="cd" />
       <baseunit description="Non-SI standard unit for angle" name="Degree" symbol="deg" /> 
       <baseunit description="Fundamental unit for thermodynamic temperature" name="Kelvin" symbol="K" />
       <baseunit description="Fundamental unit for mass" name="Kilogram" symbol="kg" />
       <baseunit description="Fundamental unit for length" name="Metre" symbol="m" />
       <baseunit description="Fundamental unit for amount of substance" name="Mole" symbol="mol" />
       <baseunit description="Non-SI standard unit for angle" name="Radiant" symbol="rad" />
       <baseunit description="Fundamental unit for time" name="Second" symbol="s" />
       <baseunit description="No SI, but needed for own unit definitions" name="Unitless" symbol="" />

       <prefixes name="atto" power="-18" symbol="a" />
       <prefixes name="centi" power="-2" symbol="c" />
       <prefixes name="deca" power="1" symbol="da" />
       <prefixes name="deci" power="-1" symbol="d" />
       <prefixes name="exa" power="18" symbol="E" />
       <prefixes name="femto" power="-15" symbol="f" />
       <prefixes name="giga" power="9" symbol="G" />
       <prefixes name="hecto" power="2" symbol="h" />
       <prefixes name="kilo" power="3" symbol="k" />
       <prefixes name="mega" power="6" symbol="M" />
       <prefixes name="micro" power="-6" symbol="u" />
       <prefixes name="milli" power="-3" symbol="m" />
       <prefixes name="nano" power="-9" symbol="n" />
       <prefixes name="peta" power="15" symbol="P" />
       <prefixes name="pico" power="-12" symbol="p" />
       <prefixes name="tera" power="12" symbol="T" />
       <prefixes name="yocto" power="-24" symbol="y" />
       <prefixes name="yotta" power="24" symbol="Y" />
       <prefixes name="zepto" power="-21" symbol="z" />
       <prefixes name="zetta" power="21" symbol="Z" />
   </units>
````


## baseunits
A concrete base unit definition will be specified by the tag
`<baseunit>` and `</baseunit>`

The baseunit needs the following mandatory attributes:

|Name|Value|Description|
|--- |--- |--- |
|name|STRING|Name of the base unit e.g. metre|
|symbol|STRING|Symbol of the base unit e.g. m|
|description|TEXT|Description of the represented base unit|


## prefixes
Prefixes between 10 power(-24) and 10 power(24) are predefined.
A prefix can be defined by the `<prefixes>` tag.

Every `<prefixes>` tag needs the following mandatory attributes:

|Name|Value|Description|changes between 1.02 and 1.0+|
|--- |--- |--- |--- |
|name|STRING|Name of the prefix| |
|symbol|STRING|Represents a short symbol e.g. k|changed to STRING from CHAR in DDL1.02|
|power|INT|Defines the power of the prefix| |


## units
A self defined unit is specified within the `<unit>` and `</unit>`
tag and needs the following mandatory attributes:

|Name|Value|Description|
|--- |--- |--- |
|name|STRING|Name of the new unit|


The `<unit>` tags needs the following mandatory sub-tags:

|Name|Value|Description|
|--- |--- |--- |
|numerator|STRING containing pi/PI or afloating-point value|Numerator of the new unit related to the baseunits|
|denominator|STRING containing pi/PI or afloating-point value. The value '0' is not defined.|Denominator of the new unit related to the baseunits|
|offset|FLOAT|Offset to the baseunits|


The new unit is able to use several base units. To represent this, it is
possible to specify the related base units by the `<refUnit>` tag. This
tag uses the following mandatory attributes:

|Name|Value|Description|changes between 1.02 and 1.0+|
|--- |--- |--- |--- |
|name|CLASS|The referenced unit|changed to CLASS from STRING in DDL1.02|
|power|INT|Power of the new unit related to the base one| |
|prefix|CLASS|Reference to the prefix to use|changed to CLASS from STRING in DDL1.02|


## Calculation of new unit

The newly defined unit relates to the SI base units like this:

newUnit = offset + (numerator / denominator) * Product (prefix(n) * baseUnit(n) ^ power(n))

&nbsp;

# datatypes
This section describes the primitive data types which can be used within the
struct elements. Example:

````xml
<datatypes>
     <datatype description="predefined ADTF tBool datatype"  size="8" name="tBool" />
     <datatype description="predefined ADTF tChar datatype"  size="8" name="tChar" />
     <datatype description="predefined ADTF tUInt8 datatype" size="8" name="tUInt8" />
     <datatype description="predefined ADTF tInt8 datatype"  size="8" name="tInt8" />
     ...     
</datatypes>
````

&nbsp;

|Name|Type|Required|Description|changes between 2.0 and 3.0|changes between 1.0+ and 1.0|
|--- |--- |--- |--- |--- |--- |
|name|String|mandatory|Name of the primitive data type|attribute name changed to "name" from "type"|
|size|UInt|mandatory|Number of bits  (relevant for serialization)| | |
|description|String|optional|Description of the primitive data type| | |
|arraysize|UInt|optional|= 1 -> primitive presentation> 1 -> array with number of elements. This feature is not supported within DDL.| | |
|unit|Class|optional|Unit of the data type| | |
|min|String|optional|Minimum value of the data type|introduced in DDL 3.0| |
|max|String|optional|Maximum value of the data type|introduced in DDL 3.0| |


The following predefined data types are provided with @c adtf.description:

|Type|Description|Number of bits|
|--- |--- |--- |
|tBool|Boolean|8 (C++ data type)|
|tBit|Bit|1|
|tChar|Character|8|
|tInt8|Signed integer|8|
|tUInt8|Unsigned integer|8|
|tInt16|Signed integer|16|
|tUInt16|Unsigned integer|16|
|tInt32|Signed integer|32|
|tUInt32|Unsigned integer|32|
|tInt64|Signed integer|64|
|tUInt64|Unsigned integer|64|
|tFloat32|IEEE Float|32|
|tFloat64|IEEE Float|64|


# enums
This section describes the enum type which can be used within the struct
elements. Example:

````xml
<enums>
     <enum name="tValueDefinitions" type="tUInt32">
		<element name="ELEMENT_ONE" value="10"/>
		<element name="ELEMENT_TWO" value="20"/>
		<element name="ELEMENT_THREE" value="5"/>
	</enum>
     ...     
</enums>
<struct alignment="1" name="tEnumData" version="1">
    <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="enumData" type="tValueDefinitions" />
</struct>
````

&nbsp;

Enum Attributes

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|Name of the enum|
|type|String|mandatory|Data type of the enum|


Enum Element Attributes

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|Name of the element|
|value|Type specific|mandatory|Value of the element|


**Remarks:**
- An enum is also valid without enum elements.

&nbsp;

# constants
This section describes constants which are implemented using the enum type. Example:

````xml
<enums>
     <enum name="tConstants" type="tUInt32">
		<element name="CONSTANT_ONE" value="42"/>
		<element name="CONSTANT_TWO" value="10"/>
	</enum>
     ...     
</enums>
<struct alignment="1" name="tEnumData" version="1">
    <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="enumData" type="tConstants" value="CONSTANT_ONE" />
</struct>
````

&nbsp;

# structs

The definition of structs allows to build complex data types. Example:

````xml
<struct alignment="4" name="tTest" version="1" ddlversion="4.0">
    <element name="bBool" type="tBool" arraysize="1">
        <serialized byteorder="LE" bytepos="0" bitpos="0" numbits="8"/>
        <deserialized alignment="1"/>
    </element>
    <element name="nInt8" type="tInt8" arraysize="1">
        <serialized byteorder="LE" bytepos="1" bitpos="0" numbits="8"/>
        <deserialized alignment="1"/>
    </element>
    <element name="nUInt32" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="2" bitpos="0" numbits="32"/>
        <deserialized alignment="4"/>
    </element>
    <element name="fFloat32" type="tFloat32" arraysize="1">
        <serialized byteorder="LE" bytepos="6" bitpos="0" numbits="32"/>
        <deserialized alignment="4"/>
    </element>
</struct>
````

The tag `<struct>` uses the following attributes:

|Name|Type|Required|Description|changes between 2.0 and 3.0|changes between 1.0+ and 1.0|
|--- |--- |--- |--- |--- |--- |
|name|String|mandatory|Description of the data type| | |
|version|UInt|mandatory|Version number of the specified data type| | |
|comment|Text|optional|Additional comments| | |
|alignment|Enum of 0/1/2/4/8/16/32/64 (defaut 1)|optional|Alignment value to get the whole packing of the complex data type which is important to get the calculated size of the structure  (relevant for serialization)|From version 3.0 on, the alignment influences the size of the struct. The size will always be a multiple of the alignment.|introduced in DDL 1.0+|
|ddlversion|String|optional|The version of the size calculation scheme, see alignment. If not specified the version from the containing definition will be used.| | |


**Remarks:**
- If the alignment is set to "0", the alignment will be set to the sum of the sizes of all types the struct contains! 

The tag `<element>` uses the following attributes:

|Name|Type|Required|Description|changes between 3.0 and 4.0|changes between 2.0 and 3.0|changes between 1.02 and 2.0|changes between 1.02 and 1.0+|changes between 1.0+ and 1.0|
|--- |--- |--- |--- |--- |--- |--- |--- |--- |
|type|Class|mandatory|Reference to an existing data type| | | | | |
|name|String|mandatory|Name of the created element| | | | | |
|bytepos|UInt|deprecated|Byte position of the data in the serialized representation of the containing struct. This is NOT relevant for the struct layout in memory (deserialized)!Started with '0'Elements following a dynamic arraymust have a byte pos of '-1'|From version 4.0 on this information is specified within the `<serialized>` tag.| | | | |
|bitpos|UInt|deprecated|Bit position of the data in the serialized representation of the containing struct. This is NOT relevant for the struct layout in memory (deserialized)!default = 0(in the range of 0 to 7) (relevant for serialization)|From version 4.0 on this information is specified within the `<serialized>` tag.| | | |default value changes from 1 to 0 in DDL1.0+|
|numbits|UInt|deprecated|Specifies the amount of bits used in the serialized representation, if not set the size of the type will be used. e.g. tInt8 with numbits of 7 (numbits can only be used to non-arrays)This is NOT relevant for the struct layout in memory (deserialized)!|From version 4.0 on this information is specified within the `<serialized>` tag.| | | | |
|byteorder|Enum of LE/BE/Motorola/Intel|deprecated|Defines the byte order in the serialized representation.|From version 4.0 on this information is specified within the `<serialized>` tag.| | | | |
|alignment|Enum of 0/1/2/4/8/16/32/64|deprecated|Defines the alignment of the element in the deserialized representation.|From version 4.0 on this information is specified within the `<deserialized>` tag.|From version 3.0 on, the alignment influences the size of the element. The size will always be a the lowest common multiple of the alignment and the size of the type of the element.| | | |
|description|String|optional|Description of the created data type| | | | | |
|unit|Class|optional|Unit of the element|Changed to optional| | | | |
|comment|Text|optional|Additional comments| | | | | |
|arraysize|tUInt or String|mandatory|Defines the array size of the element. The default is arraysize="1". Starting with DDL 2.0 the name of a preceding struct element can be used to specify a dynamic array.arraysize="elementName" For a detailed explanation of dynamic arrays refer to *dynamic arrays*| | |Dynamic array support| | |
|value|Enum element|optional|Constant value for element| | |introduced in DDL 2.0| | |
|min|String|optional|Minimum value of the element| |introduced in DDL 3.0| | | |
|max|String|optional|Maximum value of the element| |introduced in DDL 3.0| | | |
|default|String|optional|Default value of the element| |introduced in DDL 3.0| | | |
|scale|String|optional|Scaling value of the element| |introduced in DDL 3.0| | | |
|offset|String|optional|Offset value of the element| |introduced in DDL 3.0| | | |


The tag `<serialized>` uses the following attributes:
 
|Name|Type|Required|Description|changes between 2.0 and 3.0|changes between 1.02 and 2.0|changes between 1.02 and 1.0+|changes between 1.0+ and 1.0|
|--- |--- |--- |--- |--- |--- |--- |--- |
|bytepos|UInt|mandatory|Byte position of the data in the serialized representation of the containing struct. This is NOT relevant for the struct layout in memory (deserialized)!Started with '0'Elements following a dynamic arraymust have a byte pos of '-1'| | | | |
|bitpos|UInt|optional|Bit position of the data in the serialized representation of the containing struct. This is NOT relevant for the struct layout in memory (deserialized)!default = 0(in the range of 0 to 7) (relevant for serialization)| | | |default value changes from 1 to 0 in DDL1.0+|
|numbits|UInt|optional|Specifies the amount of bits used in the serialized representation, if not set the size of the type will be used. e.g. tInt8 with numbits of 7 (numbits can only be used to non-arrays)This is NOT relevant for the struct layout in memory (deserialized)!| | | | |
|byteorder|Enum of LE/BE/Motorola/Intel|mandatory|Defines the byte order in the serialized representation.| | | | |


The tag `<deserialized>` uses the following attributes:
 
|Name|Type|Required|Description|changes between 2.0 and 3.0|
|--- |--- |--- |--- |--- |
|alignment|Enum of 0/1/2/4/8/16/32/64|deprecated|Defines the alignment of the element in the deserialized representation.|From version 3.0 on, the alignment influences the size of the element. The size will always be a the lowest common multiple of the alignment and the size of the type of the element.|

**Remarks:**
- The number of structs defined in a DDL description file is not limited.
- If the name of another struct is used as type for an element, a hierarchical structure is created.
- The maximum depth of such hierarchical structures is limited by the value of the define `ADTF_DDL_MAX_DESC_HIERARCHY`.
- If the alignment is set to "0", the alignment will be set to the size of the elements type! 

## Alignment of structs and in structs

This section will explain the different meanings of alignment used in the DDL. Since alignment
is only needed for deserialized representation, this section will only consider this kind of
representation.

Inside a struct, every element has a explicit alignment value. This value influences, at what
position the element will be placed inside the struct. The memory placement is initially
determined by the order of the element tags inside the struct. After that the alignment takes
effect. An element inside a struct will only be placed at memory positions that are multiples
of the alignment value. The considered memory address is always relative to the beginning of the
struct (memory position 0).
As an example we assume having the following struct definition:

````xml
<struct alignment="4" name="tStruct" version="1">
    <element name="ui8Array" type="tUInt8" arraysize="5">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
    <element name="ui32Value" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="5"/>
        <deserialized alignment="4"/>
    </element>
</struct>
````

In this case, ui8Array will be placed at address 0. Because of the order of the element tags,
ui32Value has to be placed after ui8Array. The next memory position after ui8Array is 5. But
since the element ui32Value has an alignment of 4 and therfore the memory address has to be a
multiple of 4, ui32Value will be placed at address 8 since this is the first address to matches
all requirements. The attibute bytepos is only valid for serialized representation.

The alignment of a struct does not affect the alignment of its elements. A struct's alignment only
determines the positioning of this struct inside of arrays.
Assuming we have the following struct:

````xml
<struct alignment="4" name="tInnerStruct" version="1">
    <element name="ui8Value1" type="tUInt8" arraysize="1">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
    <element name="ui8Value2" type="tUInt8" arraysize="1">
        <serialized byteorder="LE" bytepos="1"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````

Lets now assume we have another struct `tOuterStruct`, that contains an array element and the
element's type is the struct `tInnerStruct`:

````xml
<struct alignment="1" name="tOuterStruct" version="1">
    <element name="aValue" type="tInnerStruct" arraysize="5">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````

Inside the array, the array element's positions are influenced by the alignment of the type
(`tInnerStruct`). This means that the first array element is at position 0, in relation to the
beginning of the array. The next free memory address would be 2, but since the alignment of the
struct is 4, the memory address has to be a multiple of 4 and the second element of the array now
is situated at memory address 4. The third will be at 8, the fourth at 12 and so on.

## Why should I use anything else but an alignment of 1?

If for example you are planning to describe data with DDL that was originally defined as a struct
in a header file, you should also define the alignment values inside your DDL file accordingly.
If on the other hand you are shure, that the data you are using is always interpreted using DDL,
there is no need to use an alignment other than 1.

## What consequences do the changes in DDL 3.0 have regarding alignment?

The changes in DDL 3.0 regarding alignment are meant to make it more easy to describe structs
defined in C(++). Since C(++) does not use alignment but packing for its structs, the alignment
in DDL now also influences the size of a struct defined in DDL (see new definition of alignment
in DDL 3.0). This means that arrays and structs may now become bigger in DDL 3.0 as they were in
DDL 2.x. For example the struct

````xml
<struct alignment="2" name="tFirstStruct" version="1" ddlversion="2.0/3.0">
    <element name="ui8Value" type="tUInt8" arraysize="1">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````

has the deserialized size of 1 byte in DDL 2.x and a deserialized size of 2 byte in DDL 3.0.
The struct

````xml
<struct alignment="1" name="tSecondStruct" version="1" ddlversion="2.0/3.0">
    <element name="aValue" type="tFirstStruct" arraysize="3">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````

has a size of 5 bytes in DDL 2.x and a size of 6 Bytes in DDL 3.0.
This is due to the fact, that in 2.x the array looks like this:

|Bytepos|Element|
|--- |--- |
|0|aValue[0]|
|1|padding Element|
|2|aValue[1]|
|3|padding Element|
|4|aValue[2]|


resulting in a size of 5 bytes.
The padding elements are inserted, so that the *following* Element is correctly aligned.
In DDL 3.0 the `tFirstStruct` has already a padding alement attached to its end so that its
size matches the requirements of the alignment attribute (multiple of 2). Therefore, the array
in 3.0 will look like this:

|Bytepos|Element|
|--- |--- |
|0 to 1|aValue[0]|
|2 to 3|aValue[1]|
|4 to 5|aValue[2]|

resulting in a size of 6 bytes.

## Changing from DDL 2.x to 3.0

Assuming you have a couple of structs defined in DDL 2.x, if you now plan to change the language
version from 2.x to 3.0, that you are creating new structs, that may not be compatible to its
namesakes defined in DDL 2.x. Its better to use new names for the structs to differentiate
between the different versions.

## dynamic arrays

Detailed explanation of the dynamic array functionality.


Dynamic arrays were introduced in DDL 2.0.

**Attention:** 

The use of dynamic arrays will cause a severe performance drop
compared to static arrays.
It is therefore recommended to use static arrays whenever possible.

To minimize the performance impact of dynamic arrays the user should adhere
to the following guidelines:<br>
- Always place dynamic data the end of a structure or structure hirarchy so
all static data precede the dynamic data.
- Use the struct element directly preceding the dynamic array for array size.
- Use dynamic arrays of primitive data types instead of complex data types.
- Prefer flat data over deeply structured data.
- Do not use nested dynamic arrays!
- Use alignment=1 for all elements.
- Use default values bitpos.
- Use a bytepos that matches the position in memory when alignment=1.

Dynamic Arrays cannot be used with the ADTF Signal Registry.

### Fastest way to use dynamic arrays:

````xml
<struct alignment="1" name="tDynStruct" version="1">
    <element name="ui32SomeData" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
    <element name="ui32DynArraySize" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="4"/>
        <deserialized alignment="1"/>
    </element>
    <element name="f64DynamicArray" type="tFloat64" arraysize="ui32DynArraySize">
        <serialized byteorder="LE" bytepos="8"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````

### When the dynamic data is not the last element the bytepos of the following elements must be -1

````xml
<struct alignment="1" name="tDynStruct" version="1">
    <element name="ui32DynArraySize" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
    <element name="f64DynamicArray" type="tFloat64" arraysize="ui32DynArraySize">
        <serialized byteorder="LE" bytepos="4"/>
        <deserialized alignment="1"/>
    </element>
    <element name="ui32SomeData" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="-1"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````

### Dynamic array of complex data:

````xml
<struct alignment="1" name="tVector" version="1">
    <element name="f64X" type="tFloat64" arraysize="1">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
    <element name="f64Y" type="tFloat64" arraysize="1">
        <serialized byteorder="LE" bytepos="8"/>
        <deserialized alignment="1"/>
    </element>
    <element name="f64Z" type="tFloat64" arraysize="1">
        <serialized byteorder="LE" bytepos="16"/>
        <deserialized alignment="1"/>
    </element>
</struct>
<struct alignment="1" name="tDynStruct" version="1">
    <element name="ui32SomeData" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="0"/>
        <deserialized alignment="1"/>
    </element>
    <element name="ui32DynArraySize" type="tUInt32" arraysize="1">
        <serialized byteorder="LE" bytepos="4"/>
        <deserialized alignment="1"/>
    </element>
    <element name="tVecDynamicArray" type="tVector" arraysize="ui32DynArraySize">
        <serialized byteorder="LE" bytepos="8"/>
        <deserialized alignment="1"/>
    </element>
</struct>
````


# streammetatypes

Stream Meta Types are defined within the tags `<streammetatypes>` and `</streammetatypes>`.
Example:

````xml
<streammetatypes>
  <streammetatype name="adtf/default" version="1">
      <property name="md_struct" type="string"/>
      <property name="md_definitions" type="string"/>
      <property name="md_data_serialized" type="bool"/>
  </streammetatype>
  <streammetatype name="test" version="1" parent="adtf/default">
      <property name="test_prop" type="tInt32"/>
  </streammetatype>
</streammetatypes>
````

The tag `<streammetatype>` uses the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|The identifier of the stream meta type|
|version|UInt|mandatory|Version number of the specified stream meta type|
|parent|Text|optional|Identifier of a parent stream meta type|

The tag `<property>` uses the following attributes:

|Name|Type|Required|Description|
|--- |--- |--- |--- |
|name|String|mandatory|The name of the property|
|type|UInt|mandatory|The type of the property|


# predefined elements

The following base units are provided as default:

|Base Unit Name|Description|Symbol|
|--- |--- |--- |
|Metre|Fundamental unit for length|m|
|Kilogram|Fundamental unit for mass|kg|
|Second|Fundamental unit for time|s|
|Ampere|Fundamental unit for electric current|A|
|Kelvin|Fundamental unit for thermodynamic temperature|K|
|Mole|Fundamental unit for amount of substance|mol|
|Candela|Fundamental unit for luminous intensity|cd|
|Degree|Non-SI standard unit for angle|deg|
|Radiant|Non-SI standard unit for angle|rad|
|Unitless|No SI, but needed for own unit definitions| |
|nou|No SI, but needed for no unit definitions| |

The following prefixes are provided as default:

|Prefix Name|Power|Symbol|
|--- |--- |--- |
|yotta|24|Y|
|zetta|21|Z|
|exa|18|E|
|peta|15|P|
|tera|12|T|
|giga|9|G|
|mega|6|M|
|kilo|3|k|
|hecto|2|h|
|deca|1|da|
|deci|-1|d|
|centi|-2|c|
|milli|-3|m|
|micro|-6|u|
|nano|-9|n|
|pico|-12|p|
|femto|-15|f|
|atto|-18|a|
|zepto|-21|z|
|yocto|-24|y|

The following data types are provided as default:

|Data Type Name|Description|Size|
|--- |--- |--- |
|tBool|predefined ADTF tBool datatype|8|
|tChar|predefined ADTF tChar datatype|8|
|tUInt8|Upredefined ADTF tUInt8 datatype|8|
|tInt8|predefined ADTF tInt8 datatype|8|
|tUInt16|predefined ADTF tUInt16 datatype|16|
|tInt16|predefined ADTF tInt16 datatype|16|
|tUInt32|predefined ADTF tUInt32 datatype|32|
|tInt32|predefined ADTF tInt32 datatype|32|
|tUInt64|predefined ADTF tUInt64 datatype|64|
|tInt64|predefined ADTF tInt64 datatype|64|
|tFloat32|predefined ADTF tFloat32 datatype|32|
|tFloat64|predefined ADTF tFloat64 datatype|64|


The following enums are provided as default:

|Enum Name|Type|
|--- |--- |
|tMediaTypeMajor|tUInt32|
|tPixelFormat|tInt16|




