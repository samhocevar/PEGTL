// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>


namespace sexpr
{
   using namespace pegtl;


   struct data_base
	 : private utils::nocopy< data_base >
   {
      virtual ~data_base() {}
   };


   struct token_data
	 : public data_base
   {
      token_data( const std::string & value,
		  const std::string & debug )
	    : m_value( value ),
	      m_debug( debug )
      { }

      const std::string m_value;
      const std::string m_debug;
   };


   struct cons_data
	 : public data_base
   {
      cons_data( const std::shared_ptr< data_base > & a,
		 const std::shared_ptr< data_base > & d )
	    : m_car( a ),
	      m_cdr( d )
      { }

      const std::shared_ptr< data_base > m_car;
      const std::shared_ptr< data_base > m_cdr;
   };


   template< typename Rule >
   struct token
   {
      static std::string key()
      {
	 return typeid( token ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
      }

      template< typename Input, typename Debug >
      static bool s_match( Input & in, Debug & de, std::shared_ptr< data_base > & result )
      {
	 position< Input > p( in );

	 if ( ! de.template match< Rule >( in ) ) {
	    return p( false );
	 }
	 result.reset( new token_data( std::string( p.here().get(), in.here().get() ), "foo" ) );
	 return p( true );
      }
   };


   struct comment
	 : ifmust< one< ';' >, until_eol_or_eof > {};

   struct separator
	 : sor< comment, white_plus > {};

   struct read_expr;

   struct read_atom
	 : pad< token< plus< digit > >, separator > {};

   struct read_cons
	 : ifmust< pad_one< '(', separator >, seq< star< read_expr >, pad_one< ')', separator > > > {};

   struct read_expr
	 : sor< read_cons, read_atom > {};

   struct read_file
	 : until< read_expr, until< separator, eof > > {};

} // sexpr

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      std::shared_ptr< sexpr::data_base > result;
      pegtl::input input( argv[ arg ], "command line argument" );
      pegtl::parser< sexpr::read_file, pegtl::trace_debug > parser;

      if ( parser( input, result ) ) {
	 UTILS_PRINT( "input " << argv[ arg ] << " valid" );
      }
      else {
	 UTILS_PRINT( "input " << argv[ arg ] << " invalid" );
      }
   }
   return 0;
}
