# DDL Decoder/Codec {#page_codec}

# Accessing Data with a Decoder/Codec

Let's take a look at a basic structure:

````xml
<struct alignment="4" name="tTest" version="1" ddlversion="2.0">
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

# Decoding

Read access is handled with the ddl::Decoder class:

````cpp
tFloat64 readData(const void* const_data, size_t data_size)
{
    // this should of course be cached (as a member etc.)
    ddl::CodecFactory factory("tTest", "<adtf:ddl>....");

    auto decoder = factory.makeDecoderFor(const_data, data_size);

    adtf_util::cVariant value;

    // for name based lookup use the access_element namespace
    {
        value = ddl::access_element::get_value(decoder, "fFloat32");
    }

    // alternativley you can of course use indexed based lookup
    {
        decoder.getElementValue(3, value);
    }

    return value;
}
````


# Encoding

Write access is handled with the ddl::Codec class:

````cpp
void writeData(void* data, size_t data_size, tFloat64 value)
{
    // this should of course be cached (as a member etc.)
    ddl::CodecFactory factory("tTest", "<adtf:ddl>....");

    auto codec = factory.makeCodecFor(data, data_size);

    // name based lookup
    ddl::access_element::set_value(codec, "fFloat32", value);
    // or index based
    codec.setElementValue(3, value);
}
````

# Selecting the Data Representation

By default decoders/codecs are created for the deserialized representation.
To create them for the serialized representation pass the correct parameters to the make... methods.

````cpp
auto decoder = factory.makeDecoderFor(const_data, data_size,
                                        DataRepresentation::serialized);
````

# Inspection

You can inspect the struct handled by a decoder/codec with the help of ddl::StaticDecoder::getElement:

````cpp
void dumstruct_elements(const ddl::StaticDecoder& decoder)
{
    for (size_t element = 0; element < decoder.getElementCount(); ++element)
    {
        const ddl::StructElement* struct_element;
        decoder.GetElement(element, struct_element);
        std::cout << struct_element->strName << std::endl;
    }
}
````

# Transformation

Converting between the representations can be done with ddl::serialization::transform:

````cpp
tSize serialized_size = decoder.getBufferSize(ddl::DataRepresentation::serialized);
uint8_t* buffer = new uint8_t[serialized_size];
auto Codec = decoder.makeCodecFor(buffer, serialized_size, ddl::DataRepresentation::serialized);
ddl::serialization::transform(decoder, codec);
````

There is also a convienence method ddl::serialization::transform_to_buffer that handles the allocation of memory for you with the help of an adtf_util::cMemoryBlock.

