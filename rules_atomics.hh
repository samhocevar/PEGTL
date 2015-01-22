// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_ATOMICS_HH
#define COHI_PEGTL_ATOMICS_HH

namespace pegtl
{
   struct any
   {
      static std::string key()
      {
	 return typeid( any ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< any >( ".", true );
      }

      template< typename Input, typename Debug, typename ... Class >
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
      static std::string key()
      {
	 return typeid( one ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = "\"" + escape( C ) + "\"";
	 st.template update< one >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
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
      static std::string key()
      {
	 return typeid( not_one ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = std::string( "\"[^" ) + escape( C ) + "]\"";
	 st.template update< not_one >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
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
      static std::string key()
      {
	 return typeid( list ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 std::ostringstream o;
	 d_i( o );
	 const std::string n = std::string( "\"[" ) + o.str() + "]\"";
	 st.template update< list >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {      
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

   template< int... Chars >
   struct not_list
   {
      static std::string key()
      {
	 return typeid( not_list ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 std::ostringstream o;
	 list< Chars ... >::d_i( o );
	 const std::string n = std::string( "\"[^" ) + o.str() + "]\"";
	 st.template update< not_list >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {      
	 character< Input > h( in );
	 return h( ! list< Chars ... >::m_i( h ) );
      }
   };

   template< int... Chars >
   struct at_list
	 : at< list< Chars... > > {};

   template< int... Chars >
   struct at_not_list
	 : at< not_list< Chars... > > {};

   template< int C, int D >
   struct range
   {
      static std::string key()
      {
	 return typeid( range ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = std::string( "\"[" ) + escape( C ) + "-" + escape( D ) + "]\"";
	 st.template update< range >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 static_assert( C <= D, "pegtl: illegal expression range< C, D > where C is greater than D" );
	 character< Input > h( in );
	 return h( ( h >= C ) && ( h <= D ) );
      }
   };

   template< int C, int D >
   struct not_range
   {
      static std::string key()
      {
	 return typeid( not_range ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 const std::string n = std::string( "\"[^" ) + escape( C ) + "-" + escape( D ) + "]\"";
	 st.template update< not_range >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 static_assert( C <= D, "pegtl: illegal expression not_range< C, D > where C is greater than D" );
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
      static void d_i( std::ostream &, Print & )
      {
	 ;
      }
   };

   template< int Char, int... Chars >
   struct string< Char, Chars... >
   {
      static std::string key()
      {
	 return typeid( string ).name();
      }

      template< typename Print >
      static void d_i( std::ostream & o, Print & st )
      {
	 st.template insert< one< Char > >();
	 o << escape( Char );
	 string< Chars... >::d_i( o, st );
      }
      
      template< typename Print >
      static void s_insert( Print & st )
      {
	 std::ostringstream o;
	 d_i( o, st );
	 const std::string n = std::string( "\"" ) + o.str() + "\"";
	 st.template update< string >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return de.template match< one< Char > >( in, std::forward< Class >( cl ) ... ) && string< Chars... >::template s_match( in, de, std::forward< Class >( cl ) ... );
      }
   };

   template< int Char, int... Chars >
   struct at_string
	 : at< string< Char, Chars... > > {};

   template< int Char, typename RulePadL, typename RulePadR = RulePadL >
   struct pad_one
	 : pad< one< Char >, RulePadL, RulePadR > {};

} // pegtl

#endif
