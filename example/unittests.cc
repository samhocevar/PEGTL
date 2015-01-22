// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl/pegtl.hh>

namespace
{
   int passed = 0;
   int failed = 0;

   using namespace pegtl;

   template< typename Rule >
   bool test_parse( const std::string & string )
   {
      string_input< ascii_location > in( string );
      return dummy_parse_nothrow< Rule >( in );
   }

   template< typename Rule >
   void s( const std::string & i )
   {
      if ( ! test_parse< Rule >( i ) ) {
	 PEGTL_PRINT( __PRETTY_FUNCTION__ << " failed" );
	 ++failed;
	 return;
      }
      ++passed;
   }

   template< typename Rule >
   void f( const std::string & i )
   {
      if ( test_parse< Rule >( i ) ) {
	 PEGTL_PRINT( __PRETTY_FUNCTION__ << " failed" );
	 ++failed;
	 return;
      }
      ++passed;
   }

   template< typename Rule >
   void b( const std::string & i, const bool t )
   {
      return t ? s< Rule >( i ) : f< Rule >( i );
   }

   void simple_tests()
   {
      PEGTL_PRINT( __FUNCTION__ );

      const std::string e;
      assert( e.empty() );
      s< eol >( e );
      s< success >( e );
      f< failure >( e );
   }

   template< char A >
   void atomic_tests()
   {
      PEGTL_PRINT( __FUNCTION__ << " " << int( A ) );

      for ( char c = 0; ! ( c & 0x80 ); ++c ) {
	 const std::string t( 1, c );
	 assert( t.size() == 1 );
	 b< lf >( t, c == '\n' );
	 b< cr >( t, c == '\r' );
	 s< any >( t );
	 s< success >( t );
	 f< failure >( t );
	 s< opt< one< A > > >( t );
	 s< star< one< A > > >( t );
	 b< plus< one< A > > >( t, c == A );
	 s< rep< one< A >, 0 > >( t );
	 b< rep< one< A >, 1 > >( t, c == A );
	 f< rep< one< A >, 2 > >( t );
	 f< rep< one< 'a' >, 2 > >( t );
	 f< rep< one< 'Z' >, 2 > >( t );
	 f< rep< one< '5' >, 2 > >( t );
	 f< rep< one< '\0' >, 2 > >( t );
	 b< one< A > >( t, c == A );
	 b< at_one< A > >( t, c == A );
	 b< seq< at_one< A >, one< A > > >( t, c == A );
	 b< not_one< A > >( t, c != A );
	 b< at_not_one< A > >( t, c != A );
	 b< seq< at_not_one< A >, not_one< A > > >( t, c != A );
	 b< lower >( t, ::islower( c ) );
	 b< upper >( t, ::isupper( c ) );
	 b< alpha >( t, ::isalpha( c ) );
	 b< digit >( t, ::isdigit( c ) );
	 b< alnum >( t, ::isalnum( c ) );
	 b< blank >( t, ::isblank( c ) );
	 b< space >( t, ::isspace( c ) );
	 b< xdigit >( t, ::isxdigit( c ) );
	 b< string< A > >( t, c == A );
	 b< at_string< A > >( t, c == A );
	 b< seq< at_string< A >, string< A > > >( t, c == A );
	 b< list< A > >( t, c == A );
	 b< at_list< A > >( t, c == A );
	 b< seq< at_list< A >, list< A > > >( t, c == A );
	 b< range< A, A > >( t, c == A );
	 b< at_range< A, A > >( t, c == A );
	 b< seq< at_range< A, A >, range< A, A > > >( t, c == A );
	 b< not_list< A > >( t, c != A );
	 b< at_not_list< A > >( t, c != A );
	 b< seq< at_not_list< A >, not_list< A > > >( t, c != A );
	 b< not_range< A, A > >( t, c != A );
	 b< at_not_range< A, A > >( t, c != A );
	 b< seq< at_not_range< A, A >, not_range< A, A > > >( t, c != A );
      }
   }

   void combinator_tests()
   {
      const std::string t = "abcabc";
      
      s< range< 'a', 'b' > >( t );
      s< range< 'a', 'c' > >( t );
      f< range< 'A', 'B' > >( t );
      f< range< 'A', 'C' > >( t );

      typedef string< 'a', 'b', 'c' > abc;
      typedef string< 'a', 'b', 'd' > abd;
      typedef string< 'b', 'b', 'd' > bbd;

      s< abc >( t );
      s< seq< abc, abc, eof > >( t );
      s< seq< sor< bbd, abc >, abc, eof > >( t );
      s< seq< sor< abd, abc >, abc, eof > >( t );

      s< rep< abc, 1 > >( t );
      s< rep< abc, 2 > >( t );
      f< rep< abc, 3 > >( t );
      s< star< abc > >( t );
      s< plus< abc > >( t );
      s< seq< plus< abc >, star< abc >, eol > >( t );
      s< seq< plus< abc >, plus< abc >, star< abc >, eol > >( t );
      s< seq< plus< abc >, eol > >( t );
      s< seq< plus< abc >, eof > >( t );
      s< seq< star< abc >, eol > >( t );
      s< seq< star< abc >, eof > >( t );

      const std::string d = "abcabcd";

      s< seq< plus< abc >, one< 'd' > > >( d );
      s< seq< star< abc >, one< 'd' > > >( d );
      s< seq< rep< abc, 2 >, one< 'd' > > >( d );
      f< seq< rep< abc, 2 >, abc > >( d );
      s< seq< abc, abc, one< 'd' > > >( d );

      s< must< abc > >( t );
      f< must< one< 'd' > > >( t );
      s< until1< one< 'd' > > >( d );
      s< until1< one< 'a' > > >( d );
      s< until1< one< 'c' > > >( d );
      f< until1< one< 'd' > > >( t );

      s< until< one< 'b' >, one< 'a' > > >( t );
      f< until< one< 'b' >, one< 'c' > > >( t );
      s< until< one< 'a' >, one< 'b' > > >( t );
      f< until< one< 'a' >, one< 'c' > > >( t );

      s< ifthen< one< 'a' >, one< 'b' > > >( t );
      s< ifthen< one< 'a' >, one< 'c' > > >( t );
      s< ifthen< one< 'b' >, one< 'z' > > >( t );

      s< seq< ifthen< one< 'a' >, one< 'b' > >, one< 'c' > > >( t );
      s< seq< ifthen< one< 'a' >, one< 'c' > >, one< 'a' > > >( t );
      s< seq< ifthen< one< 'b' >, one< 'z' > >, one< 'a' > > >( t );

      s< ifmust< one< 'a' >, one< 'b' > > >( t );
      f< ifmust< one< 'a' >, one< 'c' > > >( t );
      f< ifmust< one< 'b' >, one< 'z' > > >( t );

      s< seq< ifmust< one< 'a' >, one< 'b' > >, one< 'c' > > >( t );
      s< sor< ifmust< one< 'a' >, one< 'c' > >, one< 'a' > > >( t );
      s< sor< ifmust< one< 'b' >, one< 'z' > >, one< 'a' > > >( t );

      s< ifthenelse< one< 'a' >, one< 'b' >, one< 'z' > > >( t );
      f< ifthenelse< one< 'a' >, one< 'c' >, one< 'z' > > >( t );
      f< ifthenelse< one< 'b' >, one< 'z' >, one< 'z' > > >( t );
      s< ifthenelse< one< 'b' >, one< 'z' >, one< 'a' > > >( t );

      s< ifmustelse< one< 'a' >, one< 'b' >, one< 'z' > > >( t );
      f< ifmustelse< one< 'a' >, one< 'c' >, one< 'z' > > >( t );
      f< ifmustelse< one< 'b' >, one< 'z' >, one< 'z' > > >( t );
      s< ifmustelse< one< 'b' >, one< 'z' >, one< 'a' > > >( t );
   }

} //

int main()
{
   simple_tests();

   atomic_tests< 'a' >();
   atomic_tests< 'Z' >();
   atomic_tests< '5' >();
   atomic_tests< ' ' >();
   atomic_tests< '"' >();
   atomic_tests< '\n' >();
   atomic_tests< '\'' >();
   atomic_tests< '\0' >();

   if ( failed ) {
      std::cout << "ERROR, tests passed=" << passed << " tests failed=" << failed << "!" << std::endl;
      return 1;
   }
   else {
      std::cout << "OK, all " << passed << " tests passed." << std::endl;
      return 0;
   }
}
