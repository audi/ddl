#ifndef DDL_LIBRARY_TEST_FUNCTION_API_TEST_FIXTURE_DDL_TYPES_HEADER_INCLUDED_
#define DDL_LIBRARY_TEST_FUNCTION_API_TEST_FIXTURE_DDL_TYPES_HEADER_INCLUDED_

#include <type_traits>
#include <gtest/gtest.h>

class DDLTypesTestFixture : public ::testing::Test
{
public:
    template<typename DDLType, typename ... Args>
    bool isCopyable(Args&&... args) const noexcept
    {   
        // copyable compile time check
        static_assert(std::is_copy_constructible<DDLType>::value, "Not copy constructible");
        static_assert(std::is_copy_assignable<DDLType>::value, "Not copy assignable");

        // copyable runtime functionality
        const DDLType copy_source(std::forward<Args>(args)...);
        DDLType copy_sink(copy_source);
        copy_sink = copy_sink.operator=(copy_source);
        return true;
    }

    template<typename DDLType, typename ... Args>
    bool isMovable(Args&&...) const noexcept
    {
        static_assert(std::is_move_constructible<DDLType>::value, "Not move constructible");
        static_assert(std::is_move_assignable<DDLType>::value, "Not move assignable");

        // TODO: Add check for valid move during runtime
        return true;
    }

    template<typename DDLType, typename ... Args>
    bool isNotCopyable(Args&&... args) const noexcept
    {
        // copyable compile time check
        static_assert(!std::is_copy_constructible<DDLType>::value, "Copy constructible");
        static_assert(!std::is_copy_assignable<DDLType>::value, "Copy assignable");
        return true;
    }

    template<typename DDLType, typename ... Args>
    bool isNotMovable(Args&&...) const noexcept
    {
        static_assert(!std::is_move_constructible<DDLType>::value, "Move constructible");
        static_assert(!std::is_move_assignable<DDLType>::value, "Move assignable");

        // TODO: Add check for valid move during runtime
        return true;
    }
};

#endif // DDL_LIBRARY_TEST_FUNCTION_API_TEST_FIXTURE_DDL_TYPES_HEADER_INCLUDED_