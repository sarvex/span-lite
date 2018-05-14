//
// span-lite is based on GSL: Guideline Support Library.
// For more information see https://github.com/martinmoene/span-lite
//
// Copyright (c) 2015 Martin Moene
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "span-main.t.hpp"

#define DIMENSION_OF( a ) ( sizeof(a) / sizeof(0[a]) )

typedef span<int>::index_type index_type;

CASE( "span<>: Terminates construction from a nullptr and a non-zero size (C++11)" )
{
#if span_HAVE( NULLPTR )
    struct F { static void blow() { span<int> v( nullptr, 42 ); } };

    EXPECT_THROWS( F::blow() );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Terminates construction from two pointers in the wrong order" )
{
    struct F { static void blow() { int a[2]; span<int> v( &a[1], &a[0] ); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "span<>: Terminates construction from a null pointer and a non-zero size" )
{
    struct F { static void blow() { int * p = span_nullptr; span<int> v( p, 42 ); } };

    EXPECT_THROWS( F::blow() );
}

CASE( "span<>: Terminates creation of a sub span of the first n elements for n exceeding the span" )
{
    struct F {
    static void blow()
    {
        int arr[] = { 1, 2, 3, };
        span<int> v( arr );

        (void) v.first( 4 );
    }};

    EXPECT_THROWS( F::blow() );
}

CASE( "span<>: Terminates creation of a sub span of the last n elements for n exceeding the span" )
{
    struct F {
    static void blow()
    {
        int arr[] = { 1, 2, 3, };
        span<int> v( arr );

        (void) v.last( 4 );
    }};

    EXPECT_THROWS( F::blow() );
}

CASE( "span<>: Terminates creation of a sub span outside the span" )
{
    struct F {
        static void blow_offset() { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v.subspan( 4 ); }
        static void blow_count()  { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v.subspan( 1, 3 ); }
    };

    EXPECT_THROWS( F::blow_offset() );
    EXPECT_THROWS( F::blow_count()  );
}

CASE( "span<>: Terminates access outside the span" )
{
    struct F {
        static void blow_ix(index_type i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v[i]; }
        static void blow_iv(index_type i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v(i); }
//      static void blow_at(index_type i) { int arr[] = { 1, 2, 3, }; span<int> v( arr ); (void) v.at(i); }
    };

    EXPECT_NO_THROW( F::blow_ix(2) );
    EXPECT_NO_THROW( F::blow_iv(2) );
//  EXPECT_NO_THROW( F::blow_at(2) );
    EXPECT_THROWS(   F::blow_ix(3) );
    EXPECT_THROWS(   F::blow_iv(3) );
//  EXPECT_THROWS(   F::blow_at(3) );
}

CASE( "span<>: Termination throws nonstd::span_lite::detail::contract_violation exception [span_CONFIG_CONTRACT_VIOLATION_THROWS=1]" )
{
#if span_CONFIG( CONTRACT_VIOLATION_THROWS )
    struct F {
        static void blow() { int arr[] = { 1, }; span<int> v( arr ); (void) v[1]; }
    };

    EXPECT_THROWS_AS( F::blow(), nonstd::span_lite::detail::contract_violation );
#else
    EXPECT( !!"exception contract_violation is not available (non-throwing contract violation)" );
#endif
}

CASE( "span<>: Allows to default-construct" )
{
    span<int> v;

    EXPECT( v.size() == index_type( 0 ) );
}

CASE( "span<>: Allows to construct from a nullptr and a zero size (C++11)" )
{
#if span_HAVE( NULLPTR )
    span<      int> v( nullptr, index_type(0) );
    span<const int> w( nullptr, index_type(0) );

    EXPECT( v.size() == index_type( 0 ) );
    EXPECT( w.size() == index_type( 0 ) );
#else
    EXPECT( !!"nullptr is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v( arr, arr + DIMENSION_OF( arr ) );
    span<const int> w( arr, arr + DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to construct from two pointers to const" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v( arr, arr + DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows to construct from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v( arr, DIMENSION_OF( arr ) );
    span<const int> w( arr, DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to construct from a non-null pointer to const and a size" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v( arr, DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows to construct from a temporary pointer and a size" )
{
    int x = 42;

    span<      int> v( &x, 1 );
    span<const int> w( &x, 1 );

    EXPECT( std::equal( v.begin(), v.end(), &x ) );
    EXPECT( std::equal( w.begin(), w.end(), &x ) );
}

CASE( "span<>: Allows to construct from a temporary pointer to const and a size" )
{
    const int x = 42;

    span<const int> v( &x, 1 );

    EXPECT( std::equal( v.begin(), v.end(), &x ) );
}

CASE( "span<>: Allows to construct from any pointer and a zero size" )
{
    struct F {
        static void null() {
            int * p = span_nullptr; span<int> v( p, index_type( 0 ) );
        }
        static void nonnull() {
            int i = 7; int * p = &i; span<int> v( p, index_type( 0 ) );
        }
    };

    EXPECT_NO_THROW( F::null() );
    EXPECT_NO_THROW( F::nonnull() );
}

CASE( "span<>: Allows to construct from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v( arr );
    span<const int> w( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to construct from a const C-array" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "span<>: Allows to construct from a C-array with size via decay to pointer (potentially dangerous)" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    {
    span<      int> v( arr, DIMENSION_OF(arr) );
    span<const int> w( arr, DIMENSION_OF(arr) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
    }

#if span_CPP14_OR_GREATER
    {
    span<      int> v( arr, 3 );
    span<const int> w( arr, 3 );

    EXPECT( std::equal( v.begin(), v.end(), arr, arr + 3 ) );
    EXPECT( std::equal( w.begin(), w.end(), arr, arr + 3 ) );
    }
#endif
}

CASE( "span<>: Allows to construct from a const C-array with size via decay to pointer (potentially dangerous)" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    {
    span<const int> v( arr, DIMENSION_OF(arr) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    }

#if span_CPP14_OR_GREATER
    {
    span<const int> w( arr, 3 );

    EXPECT( std::equal( w.begin(), w.end(), arr, arr + 3 ) );
    }
#endif
}

CASE( "span<>: Allows to construct from a std::array<> (C++11)" )
{
#if span_HAVE( ARRAY )
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<      int> v( arr );
    span<const int> w( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "span<>: Allows to construct from a std::array<> with const data (C++11, span_CONFIG_PROVIDE_CONSTR..._ELEMENT_TYPE=1)" )
{
#if span_PROVIDE( CONSTRUCTION_FROM_STDARRAY_ELEMENT_TYPE )
# if span_HAVE( ARRAY )
    std::array<const int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<const int> v( arr );
    span<const int> const w( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), arr.begin() ) );
# else
    EXPECT( !!"std::array<> is not available (no C++11)" );
# endif
#else
    EXPECT( !!"construction is not available (span_CONFIG_PROVIDE_CONSTRUCTION_FROM_STDARRAY_ELEMENT_TYPE=0)" );
#endif
}

CASE( "span<>: Allows to construct from a container (std::vector<>)" )
{
#if span_HAVE( INITIALIZER_LIST )
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    std::vector<int> vec( arr, arr + DIMENSION_OF(arr) );
#endif

#if span_HAVE( CONSTRAINED_SPAN_CONTAINER_CTOR ) || span_HAVE( UNCONSTRAINED_SPAN_CONTAINER_CTOR )
    span<      int> v( vec );
    span<const int> w( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), vec.begin() ) );
#else
    EXPECT( !!"(un)constrained construction from container is not available" );
#endif
}

CASE( "span<>: Allows to tag-construct from a container (std::vector<>)" )
{
#if span_PROVIDE_TO_STD( WITH_CONTAINER )
# if span_HAVE( INITIALIZER_LIST )
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
# else
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    std::vector<int> vec( arr, arr + DIMENSION_OF(arr) );
# endif
    span<      int> v( with_container, vec );
    span<const int> w( with_container, vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (span_CONFIG_PROVIDE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "span<>: Allows to tag-construct from a const container (std::vector<>)" )
{
#if span_PROVIDE_TO_STD( WITH_CONTAINER )
# if span_HAVE( INITIALIZER_LIST )
    const std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
# else
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    const std::vector<int> vec( arr, arr + DIMENSION_OF(arr) );
# endif
    span<      int> v( with_container, vec );
    span<const int> w( with_container, vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
    EXPECT( std::equal( w.begin(), w.end(), vec.begin() ) );
#else
    EXPECT( !!"with_container is not available (span_CONFIG_PROVIDE_WITH_CONTAINER_TO_STD)" );
#endif
}

CASE( "span<>: Allows to copy-construct from another span of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v( arr );
    span<const int> w( arr );

    span<      int> x( v );
    span<const int> y( w );

    EXPECT( std::equal( x.begin(), x.end(), arr ) );
    EXPECT( std::equal( y.begin(), y.end(), arr ) );
}

CASE( "span<>: Allows to copy-construct from another span of a compatible type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v( arr );
    span<const int> w( arr );

    span<const volatile int> x( v );
    span<const volatile int> y( v );

    EXPECT( std::equal( x.begin(), x.end(), arr ) );
    EXPECT( std::equal( y.begin(), y.end(), arr ) );
}

CASE( "span<>: Allows to copy-construct from a temporary span of the same type (C++11)" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<      int> v(( span<      int>( arr ) ));
//  span<      int> w(( span<const int>( arr ) ));
    span<const int> x(( span<      int>( arr ) ));
    span<const int> y(( span<const int>( arr ) ));

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( x.begin(), x.end(), arr ) );
    EXPECT( std::equal( y.begin(), y.end(), arr ) );
}

CASE( "span<>: Allows to copy-assign from another span of the same type" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v( arr );
    span<      int> s;
    span<const int> t;

    s = v;
    t = v;

    EXPECT( std::equal( s.begin(), s.end(), arr ) );
    EXPECT( std::equal( t.begin(), t.end(), arr ) );
}

CASE( "span<>: Allows to copy-assign from a temporary span of the same type (C++11)" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
    span<      int> v;
    span<const int> w;

    v = span<int>( arr );
    w = span<int>( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
    EXPECT( std::equal( w.begin(), w.end(), arr ) );
}

CASE( "span<>: Allows to create a sub span of the first n elements" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int> v( arr );
    index_type count = 3;

    span<      int> s = v.first( count );
    span<const int> t = v.first( count );

    EXPECT( s.size() == count );
    EXPECT( t.size() == count );
    EXPECT( std::equal( s.begin(), s.end(), arr ) );
    EXPECT( std::equal( t.begin(), t.end(), arr ) );
}

CASE( "span<>: Allows to create a sub span of the last n elements" )
{
    int arr[] = { 1, 2, 3, 4, 5, };
    span<int> v( arr );
    index_type count = 3;

    span<      int> s = v.last( count );
    span<const int> t = v.last( count );

    EXPECT( s.size() == count );
    EXPECT( t.size() == count );
    EXPECT( std::equal( s.begin(), s.end(), arr + v.size() - count ) );
    EXPECT( std::equal( t.begin(), t.end(), arr + v.size() - count ) );
}

CASE( "span<>: Allows to create a sub span starting at a given offset" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    index_type offset = 1;

    span<      int> s = v.subspan( offset );
    span<const int> t = v.subspan( offset );

    EXPECT( s.size() == v.size() - offset );
    EXPECT( t.size() == v.size() - offset );
    EXPECT( std::equal( s.begin(), s.end(), arr + offset ) );
    EXPECT( std::equal( t.begin(), t.end(), arr + offset ) );
}

CASE( "span<>: Allows to create a sub span starting at a given offset with a given length" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );
    index_type offset = 1;
    index_type length = 1;

    span<      int> s = v.subspan( offset, length );
    span<const int> t = v.subspan( offset, length );

    EXPECT( s.size() == length );
    EXPECT( t.size() == length );
    EXPECT( std::equal( s.begin(), s.end(), arr + offset ) );
    EXPECT( std::equal( t.begin(), t.end(), arr + offset ) );
}

//CASE( "span<>: Allows to create an empty sub span at full offset" )
//{
//    int arr[] = { 1, 2, 3, };
//    span<int> v( arr );
//    index_type offset = v.size() - 1;
//
//    span<      int> s = v.subspan( offset );
//    span<const int> t = v.subspan( offset );
//
//    EXPECT( s.empty() );
//    EXPECT( t.empty() );
//}

//CASE( "span<>: Allows to create an empty sub span at full offset with zero length" )
//{
//    int arr[] = { 1, 2, 3, };
//    span<int> v( arr );
//    index_type offset = v.size();
//    index_type length = 0;
//
//    span<      int> s = v.subspan( offset, length );
//    span<const int> t = v.subspan( offset, length );
//
//    EXPECT( s.empty() );
//    EXPECT( t.empty() );
//}

CASE( "span<>: Allows forward iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    for ( span<int>::iterator pos = v.begin(); pos != v.end(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( v.begin(), pos )] );
    }
}

CASE( "span<>: Allows const forward iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    for ( span<int>::const_iterator pos = v.cbegin(); pos != v.cend(); ++pos )
    {
        EXPECT( *pos == arr[ std::distance( v.cbegin(), pos )] );
    }
}

CASE( "span<>: Allows reverse iteration" )
{
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    for ( span<int>::reverse_iterator pos = v.rbegin(); pos != v.rend(); ++pos )
    {
//        size_t dist = narrow<size_t>( std::distance(v.rbegin(), pos) );
        index_type dist = std::distance(v.rbegin(), pos);
        EXPECT( *pos == arr[ v.size() - 1 - dist ] );
    }
}

CASE( "span<>: Allows const reverse iteration" )
{
    int arr[] = { 1, 2, 3, };
    const span<int> v( arr );

    for ( span<int>::const_reverse_iterator pos = v.crbegin(); pos != v.crend(); ++pos )
    {
//        size_t dist = narrow<size_t>( std::distance(v.crbegin(), pos) );
        index_type dist = std::distance(v.crbegin(), pos);
        EXPECT( *pos == arr[ v.size() - 1 - dist ] );
    }
}

CASE( "span<>: Allows to observe an element via array indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v[i] == arr[i] );
        EXPECT( w[i] == arr[i] );
    }
}

CASE( "span<>: Allows to observe an element via call indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v(i) == arr[i] );
        EXPECT( w(i) == arr[i] );
    }
}

CASE( "span<>: Allows to observe an element via data()" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    EXPECT( *v.data() == *v.begin() );
    EXPECT( *w.data() == *v.begin() );

    for ( index_type i = 0; i < v.size(); ++i )
    {
        EXPECT( v.data()[i] == arr[i] );
        EXPECT( w.data()[i] == arr[i] );
    }
}

CASE( "span<>: Allows to observe the first element via front() [span_CONFIG_PROVIDE_BACK_FRONT=1]" )
{
#if span_PROVIDE( BACK_FRONT )
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    EXPECT( v.front() == 1 );
#else
    EXPECT( !!"front() is not available (span_CONFIG_PROVIDE_BACK_FRONT undefined or 0)" );
#endif
}

CASE( "span<>: Allows to observe the last element via back() [span_CONFIG_PROVIDE_BACK_FRONT=1]" )
{
#if span_PROVIDE( BACK_FRONT )
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    EXPECT( v.back() == 3 );
#else
    EXPECT( !!"back()is not available (span_CONFIG_PROVIDE_BACK_FRONT undefined or 0)" );
#endif
}

CASE( "span<>: Allows to change an element via array indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    v[1] = 22;
    w[2] = 33;

    EXPECT( 22 == arr[1] );
    EXPECT( 33 == arr[2] );
}

CASE( "span<>: Allows to change an element via call indexing" )
{
    int arr[] = { 1, 2, 3, };
    span<int>       v( arr );
    span<int> const w( arr );

    v(1) = 22;
    w(2) = 33;

    EXPECT( 22 == arr[1] );
    EXPECT( 33 == arr[2] );
}

CASE( "span<>: Allows to change an element via data()" )
{
    int arr[] = { 1, 2, 3, };

    span<int> v( arr );
    span<int> const w( arr );

    *v.data() = 22;
    EXPECT( 22 == *v.data() );

    *w.data() = 33;
    EXPECT( 33 == *w.data() );
}

CASE( "span<>: Allows to change the first element via front() [span_CONFIG_PROVIDE_BACK_FRONT=1]" )
{
#if span_PROVIDE( BACK_FRONT )
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    v.front() = 42;

    EXPECT( v.front() == 42 );
#else
    EXPECT( !!"front() is not available (span_CONFIG_PROVIDE_BACK_FRONT undefined or 0)" );
#endif
}

CASE( "span<>: Allows to change the last element via back() [span_CONFIG_PROVIDE_BACK_FRONT=1]" )
{
#if span_PROVIDE( BACK_FRONT )
    int arr[] = { 1, 2, 3, };
    span<int> v( arr );

    v.back() = 42;

    EXPECT( v.back() == 42 );
#else
    EXPECT( !!"back()is not available (span_CONFIG_PROVIDE_BACK_FRONT undefined or 0)" );
#endif
}

CASE( "span<>: Allows to swap with another span [span_CONFIG_PROVIDE_SWAP=1]" )
{
#if span_PROVIDE( SWAP )
    int arr[] = { 1, 2, 3, };
    span<int> a( arr );
    span<int> b = a.subspan( 1 );

    a.swap( b );

    EXPECT( a.size() == 2 );
    EXPECT( b.size() == 3 );
    EXPECT( a[0]     == 2 );
    EXPECT( b[0]     == 1 );
#else
    EXPECT( !!"swap()is not available (span_CONFIG_PROVIDE_SWAP undefined or 0)" );
#endif
}

CASE( "span<>: Allows to identfy if a span is the same as another span [span_CONFIG_PROVIDE_SAME=1]" )
{
#if span_PROVIDE( SAME )
    int  a[] = { 1 }, b[] = { 1 }, c[] = { 1, 2 };
    char x[] = { '\x1' };

    span<int > va( a );
    span<int > vb( b );
    span<int > vc( c );
    span<char> vx( x );
    span<unsigned char> vu( reinterpret_cast<unsigned char*>( &x[0] ), 1 );

    EXPECT(     same( va, va ) );
    EXPECT_NOT( same( vb, va ) );
    EXPECT_NOT( same( vc, va ) );
    EXPECT_NOT( same( vx, va ) );
    EXPECT_NOT( same( vx, vu ) );

    EXPECT(         va == va );
    EXPECT(         vb == va );
    EXPECT_NOT(     vc == va );
    EXPECT(         vx == va );
    EXPECT(         vx == vu );
#else
    EXPECT( !!"same() is not available (span_CONFIG_PROVIDE_SAME=0)" );
#endif
}

CASE( "span<>: Allows to compare equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 1 }, c[] = { 2 }, d[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    span<int> vd( d );

    EXPECT(     va == va );
    EXPECT(     vb == va );
    EXPECT_NOT( vc == va );
    EXPECT_NOT( vd == va );
}

CASE( "span<>: Allows to compare unequal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 1 }, c[] = { 2 }, d[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );
    span<int> vd( d );

    EXPECT_NOT( va != va );
    EXPECT_NOT( vb != va );
    EXPECT(     vc != va );
    EXPECT(     vd != va );
}

CASE( "span<>: Allows to compare less than another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va < va );
    EXPECT(     va < vb );
    EXPECT(     va < vc );
}

CASE( "span<>: Allows to compare less than or equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( vb <= va );
    EXPECT(     va <= vb );
    EXPECT(     va <= vc );
}

CASE( "span<>: Allows to compare greater than another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va > va );
    EXPECT(     vb > va );
    EXPECT(     vc > va );
}

CASE( "span<>: Allows to compare greater than or equal to another span of the same type" )
{
    int a[] = { 1 }, b[] = { 2 }, c[] = { 1, 2 };
    span<int> va( a );
    span<int> vb( b );
    span<int> vc( c );

    EXPECT_NOT( va >= vb );
    EXPECT(     vb >= va );
    EXPECT(     vc >= va );
}

CASE( "span<>: Allows to compare to another span of the same type and different cv-ness [span_CONFIG_PROVIDE_SAME=0]" )
{
#if span_PROVIDE( SAME )
    EXPECT( !!"skipped as same() is provided via span_CONFIG_PROVIDE_SAME=1" );
#else
    int aa[] = { 1 }, bb[] = { 2 };
    span<         int>  a( aa );
    span<   const int> ca( aa );
    span<volatile int> va( aa );
    span<         int>  b( bb );
    span<   const int> cb( bb );

    EXPECT( va == ca );
    EXPECT(  a == va );

    EXPECT(  a == ca );
    EXPECT(  a != cb );
    EXPECT(  a <= cb );
    EXPECT(  a <  cb );
    EXPECT(  b >= ca );
    EXPECT(  b >  ca );
#endif
}

CASE( "span<>: Allows to compare empty spans as equal" )
{
    int a;

    span<int> p;
    span<int> q;
    span<int> r( &a, index_type( 0 ) );

    EXPECT( p == q );
    EXPECT( p == r );

#if span_HAVE( NULLPTR )
    span<int> s( nullptr, index_type( 0 ) );
    span<int> t( nullptr, index_type( 0 ) );

    EXPECT( s == p );
    EXPECT( s == r );
    EXPECT( s == t );
#endif
}

CASE( "span<>: Allows to test for empty span via empty(), empty case" )
{
    span<int> v;

    EXPECT( v.empty() );
}

CASE( "span<>: Allows to test for empty span via empty(), non-empty case" )
{
    int a[] = { 1 };
    span<int> v( a );

    EXPECT_NOT( v.empty() );
}

CASE( "span<>: Allows to obtain the number of elements via size()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.size() == index_type( DIMENSION_OF( a ) ) );
    EXPECT( vb.size() == index_type( DIMENSION_OF( b ) ) );
    EXPECT(  z.size() == index_type( 0 ) );
}

CASE( "span<>: Allows to obtain the number of bytes via size_bytes()" )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 1, 2, 3, 4, 5, };

    span<int> z;
    span<int> va( a );
    span<int> vb( b );

    EXPECT( va.size_bytes() == index_type( DIMENSION_OF( a ) * sizeof(int) ) );
    EXPECT( vb.size_bytes() == index_type( DIMENSION_OF( b ) * sizeof(int) ) );
    EXPECT(  z.size_bytes() == index_type( 0 * sizeof(int) ) );
}

//CASE( "span<>: Allows to swap with another span of the same type" )
//{
//    int a[] = { 1, 2, 3, };
//    int b[] = { 1, 2, 3, 4, 5, };
//
//    span<int> va0( a );
//    span<int> vb0( b );
//    span<int> va( a );
//    span<int> vb( b );
//
//    va.swap( vb );
//
//    EXPECT( va == vb0 );
//    EXPECT( vb == va0 );
//}

#if span_HAVE( BYTE )

static bool is_little_endian()
{
    union U
    {
        U() : i(1) {}
        int i;
        char c[ sizeof(int) ];
    };
    return 1 != U().c[ sizeof(int) - 1 ];
}
#endif

CASE( "span<>: Allows to view the elements as read-only bytes" )
{
#if span_HAVE( BYTE )
    using std::byte;
    using type = std::int32_t;

    EXPECT( sizeof( type ) == size_t( 4 ) );

    type  a[] = { 0x12345678, };
    byte be[] = { byte{0x12}, byte{0x34}, byte{0x56}, byte{0x78}, };
    byte le[] = { byte{0x78}, byte{0x56}, byte{0x34}, byte{0x12}, };

    byte * b = is_little_endian() ? le : be;

    span<type> va( a );
    span<const byte> vb( as_bytes( va ) );

    EXPECT( vb[0] == b[0] );
    EXPECT( vb[1] == b[1] );
    EXPECT( vb[2] == b[2] );
    EXPECT( vb[3] == b[3] );
#else
    EXPECT( !!"std::byte is not available (no C++17)" );
#endif
}

CASE( "span<>: Allows to view and change the elements as writable bytes" )
{
#if span_HAVE( BYTE )
    using std::byte;
    using type = std::int32_t;

    EXPECT( sizeof(type) == size_t( 4 ) );

    type  a[] = { 0x0, };
    span<type> va( a );
    span<byte> vb( as_writeable_bytes(va) );

    for ( std::ptrdiff_t i = 0; i < std::ptrdiff_t( sizeof(type) ); ++i )
    {
        EXPECT( vb[i] == byte{0} );
    }

    vb[0] = byte{0x42};

    EXPECT( vb[0] == byte{0x42} );
    for ( std::ptrdiff_t i = 1; i < std::ptrdiff_t( sizeof(type) ); ++i )
    {
        EXPECT( vb[i] == byte{0} );
    }
#else
    EXPECT( !!"std::byte is not available (no C++17)" );
#endif
}

//CASE( "span<>: Allows to view the elements as a span of another type" )
//{
//#if span_HAVE( SIZED_TYPES )
//    typedef int32_t type1;
//    typedef int16_t type2;
//#else
//    typedef int   type1;
//    typedef short type2;
//#endif
//    EXPECT( sizeof( type1 ) == size_t( 4 ) );
//    EXPECT( sizeof( type2 ) == size_t( 2 ) );
//
//    type1  a[] = { 0x12345678, };
//    type2 be[] = { type2(0x1234), type2(0x5678), };
//    type2 le[] = { type2(0x5678), type2(0x1234), };
//
//    type2 * b = is_little_endian() ? le : be;
//
//    span<type1> va( a );
//    span<type2> vb( va );
//
//    EXPECT( vb[0] == b[0] );
//    EXPECT( vb[1] == b[1] );
//}

//CASE( "span<>: Allows to change the elements from a span of another type" )
//{
//#if span_HAVE( SIZED_TYPES )
//    typedef int32_t type1;
//    typedef int16_t type2;
//#else
//    typedef int   type1;
//    typedef short type2;
//#endif
//    EXPECT( sizeof( type1 ) == size_t( 4 ) );
//    EXPECT( sizeof( type2 ) == size_t( 2 ) );
//
//    type1  a[] = { 0x0, };
//
//    span<type1> va( a );
//#if span_COMPILER_MSVC_VERSION == 6
//    span<type2> vb( va.as_span( type2() ) );
//#else
//    span<type2> vb( va.as_span<type2>() );
//#endif
//
//    {for ( size_t i = 0; i < sizeof(type2); ++i ) EXPECT( vb[i] == type2(0) ); }
//
//    vb[0] = 0x42;
//
//    EXPECT( vb[0] == type2(0x42) );
//    {for ( size_t i = 1; i < sizeof(type2); ++i ) EXPECT( vb[i] == type2(0) ); }
//}

#if span_PROVIDE_TO_STD( MAKE_SPAN )

CASE( "make_span() [span_CONFIG_PROVIDE_MAKE_SPAN_TO_STD=99]" )
{
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]
}

CASE( "make_span(): Allows building from two pointers" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr, arr + DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from two const pointers" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr, arr + DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a non-null pointer and a size" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr, DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a non-null const pointer and a size" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr, DIMENSION_OF( arr ) );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a C-array" )
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a const C-array" )
{
    const int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span<const int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr ) );
}

CASE( "make_span(): Allows building from a std::array<> (C++11)" )
{
#if span_HAVE( ARRAY )
    std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a const std::array<> (C++11)" )
{
#if span_HAVE( ARRAY )
    const std::array<int,9> arr = {{ 1, 2, 3, 4, 5, 6, 7, 8, 9, }};

    span<const int> v = make_span( arr );

    EXPECT( std::equal( v.begin(), v.end(), arr.begin() ) );
#else
    EXPECT( !!"std::array<> is not available (no C++11)" );
#endif
}

CASE( "make_span(): Allows building from a container (std::vector<>)" )
{
#if span_HAVE( INITIALIZER_LIST )
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    span<int> v = make_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "make_span(): Allows building from a const container (std::vector<>)" )
{
#if span_HAVE( INITIALIZER_LIST )
    const std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    const std::vector<int> vec( 10, 42 );
#endif
    span<const int> v = make_span( vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
}

CASE( "make_span(): Allows building from a container (with_container_t, std::vector<>)" )
{
#if span_PROVIDE_TO_STD( WITH_CONTAINER )
#if span_HAVE( INITIALIZER_LIST )
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    std::vector<int> vec; {for ( int i = 1; i < 10; ++i ) vec.push_back(i); }
#endif
    span<int> v = make_span( with_container, vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
#else
    EXPECT( !!"make_span(with_container,...) is not available (span_PROVIDE_WITH_CONTAINER_TO_STD=0)" );
#endif
}

CASE( "make_span(): Allows building from a const container (with_container_t, std::vector<>)" )
{
#if span_PROVIDE_TO_STD( WITH_CONTAINER )
#if span_HAVE( INITIALIZER_LIST )
    const std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
#else
    const std::vector<int> vec( 10, 42 );
#endif
    span<const int> v = make_span( with_container, vec );

    EXPECT( std::equal( v.begin(), v.end(), vec.begin() ) );
#else
    EXPECT( !!"make_span(with_container,...) is not available (span_PROVIDE_WITH_CONTAINER_TO_STD=0)" );
#endif
}

#endif // span_CONFIG_PROVIDE_MAKE_SPAN

#if span_PROVIDE( BYTE_SPAN )

CASE( "byte_span() [span_CONFIG_PROVIDE_BYTE_SPAN=1]" )
{
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]
}

CASE( "byte_span(): Allows building a span of std::byte from a single object (C++17)" )
{
#if span_HAVE( BYTE )
    int x = std::numeric_limits<int>::max();

    span<std::byte> spn = byte_span( x );

    EXPECT( spn.size() == std::ptrdiff_t( sizeof x ) );
    EXPECT( spn[0]     == std::byte( 0xff ) );
#else
    EXPECT( !!"std::byte is not available (no C++17)" );
#endif
}

CASE( "byte_span(): Allows building a span of const std::byte from a single const object (C++17)" )
{
#if span_HAVE( BYTE )
    const int x = std::numeric_limits<int>::max();

    span<const std::byte> spn = byte_span( x );

    EXPECT( spn.size() == std::ptrdiff_t( sizeof x ) );
    EXPECT( spn[0]     == std::byte( 0xff ) );
#else
    EXPECT( !!"std::byte is not available (no C++17)" );
#endif
}

#endif // span_PROVIDE( BYTE_SPAN )

// Issues

#include <cassert>

CASE( "[hide][issue 3: heterogeneous comparison]" )
{
#if span_PROVIDE_TO_STD( MAKE_SPAN )
    static const int data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };

    span< const int > spn( data );

    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]

    assert( make_span( data ) == make_span(data) ); // Ok, non-heterogeneous comparison
    assert( make_span( data ) == spn             ); // Compile error: comparing fixed with dynamic extension
#else
    EXPECT( !!"test is unavailable as make_span() is not provided via span_CONFIG_PROVIDE_MAKE_SPAN_TO_STD=99" );
#endif // span_PROVIDE_TO_STD( MAKE_SPAN )
}

CASE( "[hide][issue 3: same()]" )
{
#if span_PROVIDE_TO_STD( MAKE_SPAN )
#if span_PROVIDE( SAME )
    EXPECT( !!"(avoid warning)" );  // suppress: unused parameter 'lest_env' [-Wunused-parameter]

    typedef unsigned char uint8_type;

    static uint8_type const data[]    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    static float      const farray[4] = { 0, 1, 2, 3 };

    span<float const> fspan1 = make_span( farray );

    assert( fspan1.data() == farray );
    assert( fspan1.size() == DIMENSION_OF( farray ) );

#if span_HAVE( BYTE )
    span<std::byte const> fspan2 = byte_span( farray[0] );

    assert( static_cast<void const *>( fspan1.data() ) == fspan2.data() );
    assert(                            fspan1.size()   == fspan2.size() );
    assert(                    ! same( fspan1          ,  fspan2 )      );
# endif

    span<uint8_type const> bspan4 = make_span( data, 4 );

    assert(        bspan4 == fspan1   );
    assert(        fspan1 == bspan4   );
    assert( !same( fspan1 ,  bspan4 ) );

#if span_HAVE( BYTE )
    assert(        as_bytes( fspan1 ) != as_bytes( bspan4 )   );
    assert( !same( as_bytes( fspan1 ) ,  as_bytes( bspan4 ) ) );
#endif

    union
    {
        int i;
        float f;
        char c;
    } u = { 0x12345678 };

    span<int  > uspan1 = make_span( &u.i, 1 );
    span<float> uspan2 = make_span( &u.f, 1 );
    span<char > uspan3 = make_span( &u.c, 1 );

    assert( static_cast<void const *>( uspan1.data() ) == uspan2.data() );
    assert(                            uspan1.size()   == uspan2.size() );
    assert( static_cast<void const *>( uspan1.data() ) == uspan3.data() );
    assert(                            uspan1.size()   == uspan3.size() );

    assert( !same( uspan1, uspan2 ) );
    assert( !same( uspan1, uspan3 ) );
    assert( !same( uspan2, uspan3 ) );

    assert( uspan1 != uspan2 );
    assert( uspan1 != uspan3 );
    assert( uspan2 != uspan3 );

#if span_HAVE( BYTE )
    assert(  same( as_bytes( uspan1 ), as_bytes( uspan2 ) ) );
    assert( !same( as_bytes( uspan1 ), as_bytes( uspan3 ) ) );
    assert( !same( as_bytes( uspan2 ), as_bytes( uspan3 ) ) );
#endif
#else
    EXPECT( !!"same() is not provided via span_CONFIG_PROVIDE_SAME=1" );
#endif // span_PROVIDE( SAME )
#else
    EXPECT( !!"test is unavailable as make_span is not provided via span_CONFIG_PROVIDE_MAKE_SPAN_TO_STD=99" );
#endif // span_PROVIDE_TO_STD( MAKE_SPAN )
}

// end of file
