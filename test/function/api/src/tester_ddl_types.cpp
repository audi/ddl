#include "ddl.h"
#include "test_fixture_ddl_types.h"

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDL)
{
    TEST_REQ("CDDDL-89");
    struct DDLMOC : public ddl::DDL
    {
        using Result = a_util::result::Result;
        Result accept(ddl::IDDLChangeVisitor*) override { return a_util::result::SUCCESS; }
        Result accept(ddl::IDDLVisitor*) const override { return a_util::result::SUCCESS; }
        const std::string& getName() const override { return _name; }

    private:
        std::string _name;
    };

    EXPECT_TRUE(isCopyable<DDLMOC>());
    EXPECT_TRUE(isMovable<DDLMOC>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLAlignment)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLAlignment>());
    EXPECT_TRUE(isMovable<ddl::DDLAlignment>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLBaseunit)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLBaseunit>());
    EXPECT_TRUE(isMovable<ddl::DDLBaseunit>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLByteorder)
{
    TEST_REQ("CDDDL-89");
    struct DDLByteOrderMOC : public ddl::DDLByteorder
    {
        using Result = a_util::result::Result;
        Result accept(ddl::IDDLChangeVisitor*) override { return a_util::result::SUCCESS; }
        Result accept(ddl::IDDLVisitor*) const override { return a_util::result::SUCCESS; }
        const std::string& getName() const override { return _name; }

    private:
        std::string _name;
    };

    EXPECT_TRUE(isCopyable<DDLByteOrderMOC>());
    EXPECT_TRUE(isMovable<DDLByteOrderMOC>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLComplex)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLComplex>());
    EXPECT_TRUE(isMovable<ddl::DDLComplex>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLDataType)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLDataType>());
    EXPECT_TRUE(isMovable<ddl::DDLDataType>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLAutoVec)
{
    TEST_REQ("CDDDL-89");
    struct DeleteTester : public ddl::DDLElement
    {
        DeleteTester() { ++DeleteTester::not_deleted(); }
        ~DeleteTester() { --DeleteTester::not_deleted(); }
        static int& not_deleted() { static int count{}; return count; }
    };

    {
        std::vector<DeleteTester*> elements(10, nullptr);
        std::for_each(elements.begin(), elements.end(), [](DeleteTester*& element) {
            element = new DeleteTester();
        });
        EXPECT_TRUE(isCopyable<ddl::DDLAutoVec<DeleteTester>>(&elements));
        EXPECT_TRUE(isMovable<ddl::DDLAutoVec<DeleteTester>>(&elements));
    }
    EXPECT_EQ(DeleteTester::not_deleted(), 0);
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLCloner)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLCloner>());
    EXPECT_TRUE(isMovable<ddl::DDLCloner>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLCompare)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLCompare>());
    EXPECT_TRUE(isMovable<ddl::DDLCompare>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLContainerNoClone)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLContainerNoClone<ddl::DDLDataType>>());
    EXPECT_TRUE(isMovable<ddl::DDLContainerNoClone<ddl::DDLDataType>>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLContainer)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLContainer<ddl::DDLDataType>>());
    EXPECT_TRUE(isMovable<ddl::DDLContainer<ddl::DDLDataType>>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLDescription)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isNotCopyable<ddl::DDLDescription>(nullptr));
    EXPECT_TRUE(isMovable<ddl::DDLDescription>(nullptr));
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLElement)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLElement>());
    EXPECT_TRUE(isMovable<ddl::DDLElement>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLEnum)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLEnum>());
    EXPECT_TRUE(isMovable<ddl::DDLEnum>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLError)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLError>());
    EXPECT_TRUE(isMovable<ddl::DDLError>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLExtDeclaration)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLExtDeclaration>());
    EXPECT_TRUE(isMovable<ddl::DDLExtDeclaration>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLHeader)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLHeader>());
    EXPECT_TRUE(isMovable<ddl::DDLHeader>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLImporter)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLImporter>());
    EXPECT_TRUE(isMovable<ddl::DDLImporter>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLInspector)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLInspector>());
    EXPECT_TRUE(isMovable<ddl::DDLInspector>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLPrefix)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLPrefix>());
    EXPECT_TRUE(isMovable<ddl::DDLPrefix>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLPrinter)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLPrinter>());
    EXPECT_TRUE(isMovable<ddl::DDLPrinter>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLProperty)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLProperty>());
    EXPECT_TRUE(isMovable<ddl::DDLProperty>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLRefUnit)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLRefUnit>());
    EXPECT_TRUE(isMovable<ddl::DDLRefUnit>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLRepair)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLRepair>());
    EXPECT_TRUE(isMovable<ddl::DDLRepair>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLResolver)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLResolver>());
    EXPECT_TRUE(isMovable<ddl::DDLResolver>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLStream)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLStream>());
    EXPECT_TRUE(isMovable<ddl::DDLStream>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLStreamMetaType)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLStreamMetaType>());
    EXPECT_TRUE(isMovable<ddl::DDLStreamMetaType>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLStreamStruct)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLStreamStruct>());
    EXPECT_TRUE(isMovable<ddl::DDLStreamStruct>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLUnit)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLUnit>());
    EXPECT_TRUE(isMovable<ddl::DDLUnit>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDDLVersion)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::DDLVersion>());
    EXPECT_TRUE(isMovable<ddl::DDLVersion>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsStaticDecoder)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isNotCopyable<ddl::StaticDecoder>());
    EXPECT_TRUE(isMovable<ddl::StaticDecoder>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsDecoder)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isNotCopyable<ddl::Decoder>());
    EXPECT_TRUE(isMovable<ddl::Decoder>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsCodec)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isNotCopyable<ddl::Codec>());
    EXPECT_TRUE(isMovable<ddl::Codec>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsStaticCodec)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isNotCopyable<ddl::StaticCodec>());
    EXPECT_TRUE(isMovable<ddl::StaticCodec>());
}

TEST_F(DDLTypesTestFixture, testCopyAndMoveSemanticsCodecFactory)
{
    TEST_REQ("CDDDL-89");
    EXPECT_TRUE(isCopyable<ddl::CodecFactory>());
    EXPECT_TRUE(isMovable<ddl::CodecFactory>());
}
