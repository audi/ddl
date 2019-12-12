# Object-oriented representation of the DDL (OO-DDL) {#page_oo_ddl}

# Introduction
To facilitate the handling of DDL hierarchies there is an object-oriented
approach of representation, called OO-DDL.

Within the class structure of OO-DDL every node type inside the DDL definition
is representated by an own class.

**Note:** 

These classes are associated by using the GoF design patterns "Visitor",
"Decorator", and "Factory Method". 
The main classes are described in the following section.

# Structure of classes
The basic interface of nearly all classes of the OO-DDL is `ddl::IDDL`.
The root element of every DDL structure is an instance of
`ddl::cDDLDescription`. For example a DDL Definition File may be
represented by `cDDLDescription`. This object holds references to the main
parts like the header (`ddl::cDDLHeader`) or the datatypes
(`ddl::IDDLDataType`). For further information on all available OO-DDL
classes see the SDK documentation of `ddl::IDDL`.

Every link between DDL-representing objects is realized as pointer to the
according object. So lots of string comparisons for matching e.g. child
elements could be removed.

# Building up - using ddl::IDDLFactoryMethod
For all creation tasks concerning OO-DDL implementations of the interface
`IDDLFactoryMethod` are used. A basic implementation of `IDDLFactoryMethod`
is `ddl::cDDLImporter`.

# Processing DDL - using ddl::IDDLVisitor
For every task of processing DDL implementations of `ddl::IDDLVisitor` are
used. Utilizing this interface the DDL-representing hierarchy can be walked
through and appropriate actions may be taken. The basic implementation of
`IDDLVisitor` is `ddl::cDDLPrinter`.

# The Correlation Between OO-DDL and DDL.
Every tag and some attributes in the DDL have a correspondent in the OO-DDL. 
The following table shows the OO-DDL classes with their matching DDL tag or attribute:

|DDL Tag/Attribute|OO-DDL Class|
|--- |--- |
|`<adtf:ddl>`|cDDLDescription|
|`<header>`|cDDLHeader|
|`<ext_declaration>`|cDDLExtDeclaration|
|`<baseunit>`|cDDLBaseunit|
|`<prefix>`|cDDLPrefix|
|`<unit>`|cDDLUnit|
|`<refUnit>`|cDDLRefUnit|
|`<datatype>`|cDDLDataType|
|`<enum>`|cDDLEnum|
|`<struct>`|cDDLComplex|
|`<element>`|cDDLElement|
|alignment=xxx|cDDLAlignment|
|byteorder=xxx|cDDLByteorder|
|`<stream>`|cDDLStream|
|`<struct>` (within a `<stream>` tag)|cDDLStreamStruct|



