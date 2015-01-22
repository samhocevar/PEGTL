// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_VALUE_HH
#define COHI_PEGTL_VALUE_HH


namespace pegtl
{
   struct any
   {
      static std::string key()
      {
	 return typeid( any ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
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
      static void s_print( Print & st )
      {
	 const std::string n = "\"" + utils::escape( C ) + "\"";
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
      static void s_print( Print & st )
      {
	 const std::string n = std::string( "\"[^" ) + utils::escape( C ) + "]\"";
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


   template< int... Chars > struct one_list;


   template<>
   struct one_list<>
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
   struct one_list< Char, Chars... >
   {
      static std::string key()
      {
	 return typeid( one_list ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 std::ostringstream o;
	 d_i( o );
	 const std::string n = std::string( "\"[" ) + o.str() + "]\"";
	 st.template update< one_list >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {      
	 character< Input > h( in );
	 return h( m_i( h ) );
      }

      static void d_i( std::ostream & o )
      {
	 o << utils::escape( Char );
	 one_list< Chars... >::d_i( o );
      }

      static bool m_i( const int i )
      {
	 return ( i == Char ) || one_list< Chars... >::m_i( i );
      }
   };


   template< int... Chars >
   struct at_one_list
	 : at< one_list< Chars... > > {};


   template< int C, int D >
   struct one_range
   {
      static std::string key()
      {
	 return typeid( one_range ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 const std::string n = std::string( "\"[" ) + utils::escape( C ) + "-" + utils::escape( D ) + "]\"";
	 st.template update< one_range >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 character< Input > h( in );
	 return h( ( h >= C ) && ( h <= D ) );
      }
   };


   template< int C, int D >
   struct at_one_range
	 : at< one_range< C, D > > {};


   template< int... Chars > struct string;


   template<>
   struct string<> : success
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
	 o << utils::escape( Char );
	 string< Chars... >::d_i( o, st );
      }
      
      template< typename Print >
      static void s_print( Print & st )
      {
	 std::ostringstream o;
	 d_i( o, st );
	 const std::string n = std::string( "\"" ) + o.str() + "\"";
	 st.template update< string >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return de.template match< one< Char > >( in, std::forward< Class >( cl ) ... ) && string< Chars... >::template s_match( in, std::forward< Class >( cl ) ... );
      }
   };


   template< int Char, int... Chars >
   struct at_string
	 : at< string< Char, Chars... > > {};

} // pegtl

#endif
