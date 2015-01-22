// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_RULES_STRING_HH
#define COHI_PEGTL_RULES_STRING_HH

namespace pegtl
{
   struct any
   {
      typedef any key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< any >( ".", true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 if ( in.eof() ) {
	    return false;
	 }
	 in.bump();
	 return true;
      }
   };

   template< int C >
   struct one
   {
      typedef one key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = "\"" + escape( C ) + "\"";
	 st.template update< one >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 if ( in.eof() ) {
	    return false;
	 }
	 character< Input > h( in );
	 return h( h == C );
      }
   };

   template< int C >
   struct at_one
	 : at< one< C > > {};

   template< int C >
   struct not_one
   {
      typedef not_one key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = std::string( "\"[^" ) + escape( C ) + "]\"";
	 st.template update< not_one >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 if ( in.eof() ) {
	    return false;
	 }
	 character< Input > h( in );
	 return h( h != C );
      }
   };

   template< int C >
   struct at_not_one
	 : at< not_one< C > > {};

   template< int... Chars > struct list;

   template<>
   struct list<>
   {
      static void d_i( std::ostream & )
      {
	 ;
      }

      static bool m_i( const int )
      {
	 return false;
      }
   };

   template< int Char, int... Chars >
   struct list< Char, Chars... >
   {
      typedef list key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 std::ostringstream o;
	 d_i( o );
	 const std::string n = std::string( "\"[" ) + o.str() + "]\"";
	 st.template update< list >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {      
	 if ( in.eof() ) {
	    return false;
	 }
	 character< Input > h( in );
	 return h( m_i( h ) );
      }

      static void d_i( std::ostream & o )
      {
	 o << escape( Char );
	 list< Chars... >::d_i( o );
      }

      static bool m_i( const int i )
      {
	 return ( i == Char ) || list< Chars... >::m_i( i );
      }
   };

   template< int ... Chars >
   struct not_list
   {
      typedef not_list key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 std::ostringstream o;
	 list< Chars ... >::d_i( o );
	 const std::string n = std::string( "\"[^" ) + o.str() + "]\"";
	 st.template update< not_list >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {      
	 if ( in.eof() ) {
	    return false;
	 }
	 character< Input > h( in );
	 return h( ! list< Chars ... >::m_i( h ) );
      }
   };

   template< int ... Chars >
   struct at_list
	 : at< list< Chars ... > > {};

   template< int... Chars >
   struct at_not_list
	 : at< not_list< Chars ... > > {};

   template< int C, int D >
   struct range
   {
      typedef range key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = std::string( "\"[" ) + escape( C ) + "-" + escape( D ) + "]\"";
	 st.template update< range >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 static_assert( C <= D, "pegtl: illegal expression range< C, D > where C is greater than D" );
	 if ( in.eof() ) {
	    return false;
	 }
	 character< Input > h( in );
	 return h( ( h >= C ) && ( h <= D ) );
      }
   };

   template< int C, int D >
   struct not_range
   {
      typedef not_range key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = std::string( "\"[^" ) + escape( C ) + "-" + escape( D ) + "]\"";
	 st.template update< not_range >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 static_assert( C <= D, "pegtl: illegal expression not_range< C, D > where C is greater than D" );

	 if ( in.eof() ) {
	    return false;
	 }
	 character< Input > h( in );
	 return h( ( h < C ) || ( h > D ) );
      }
   };

   template< int C, int D >
   struct at_range
	 : at< range< C, D > > {};

   template< int C, int D >
   struct at_not_range
	 : at< not_range< C, D > > {};

   template< int... Chars > struct string;

   template<>
   struct string<>
	 : success
   {
      template< typename Print >
      static void i_insert( std::ostream &, Print & )
      { }

      template< bool, typename Input, typename Debug, typename ... Class >
      static bool i_match( Input &, Debug &, Class && ... )
      {
	 return true;
      }
   };

   template< int Char, int ... Chars >
   struct string< Char, Chars ... >
   {
      typedef string key_type;

      template< typename Print >
      static void i_insert( std::ostream & o, Print & st )
      {
	 st.template insert< one< Char > >();
	 o << escape( Char );
	 string< Chars ... >::i_insert( o, st );
      }
      
      template< typename Print >
      static void s_insert( Print & st )
      {
	 std::ostringstream o;
	 i_insert( o, st );
	 const std::string n = std::string( "\"" ) + o.str() + "\"";
	 st.template update< string >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > h( in );
	 return h( i_match< Must >( in, de, std::forward< Class >( cl ) ... ) );
      }

      template< bool Must, typename Input, typename Debug, typename ... Class >
      static bool i_match( Input & in, Debug & de, Class && ... cl )
      {
	 return one< Char >::template s_match< Must >( in, de, std::forward< Class >( cl ) ... ) && pegtl::string< Chars ... >::template i_match< Must >( in, de, std::forward< Class >( cl ) ... );
      }
   };

   template< int Char, int ... Chars >
   struct at_string
	 : at< string< Char, Chars ... > > {};

   template< int Char, typename RulePadL, typename RulePadR = RulePadL >
   struct pad_one
	 : pad< one< Char >, RulePadL, RulePadR > {};

   struct digit
	 : range< '0', '9' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< digit >( "digit", true );
      }
   };

   struct lower
	 : range< 'a', 'z' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< lower >( "lower", true );
      }      
   };

   struct upper
	 : range< 'A', 'Z' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< upper >( "upper", true );
      }      
   };

   struct alpha
	 : sor< lower, upper >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< alpha >( "alpha", true );
      }
   };

   struct alnum
	 : sor< alpha, digit >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< alpha >( "alnum", true );
      }
   };

   struct xdigit
	 : sor< digit, range< 'a', 'f' >, range< 'A', 'F' > >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< alpha >( "xdigit", true );
      }
   };

   struct ident1
	 : sor< one< '_' >, alpha > {};

   struct ident2
	 : sor< digit, ident1 > {};

   struct identifier
	 : seq< ident1, star< ident2 > > {};

   struct lf
	 : one< '\n' > {};

   struct cr
	 : one< '\r' > {};

   struct crlf
	 : seq< cr, lf > {};

   struct eol
	 : sor< eof, crlf, lf, cr > {};

   struct blank
	 : list< ' ', '\t' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< blank >( "blank", true );
      }
   };

   struct space_star
	 : star< blank > {};

   struct space_plus
	 : plus< blank > {};

   struct space
	 : list< ' ', '\n', '\r', '\t', '\v', '\f' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< space >( "space", true );
      }
   };

   struct white_star
	 : star< space > {};

   struct white_plus
	 : plus< space > {};

   struct until_eol
	 : until1< eol > {};

   struct white_until_eof
	 : until< space, eof > {};

   struct space_until_eol
	 : until< blank, eol > {};

   struct shebang
	 : ifmust< string< '#', '!' >, until_eol > {};

} // pegtl

#endif
