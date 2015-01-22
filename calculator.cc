// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

#include <boost/lexical_cast.hpp>


namespace pegtl
{
   struct first_action
   {
      template< typename Rule >
      static void matched( const std::string & m, char & c )
      {
	 assert( m.size() == 1 );
	 c = m[ 0 ];
      }
   };


   template< typename Rule >
   struct write_first
	 : action< Rule, first_action > {};


   template< typename Rule >
   struct write_cast
   {
      static std::string key()
      {
	 return typeid( write_cast ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
      }

      template< typename Input, typename Debug, typename Output >
      static bool s_match( Input & in, Debug & de, Output & cl )
      {      
	 position< Input > p( in );
	 
	 if ( ! de.template match< Rule >( in ) ) {
	    return p( false );
	 }
	 cl = boost::lexical_cast< Output >( std::string( p.here(), in.here() ) );
	 return p( true );
      }
   };


   typedef seq< white_star, write_cast< plus< digit > > > read_number;

   typedef seq< white_star, one< '(' > > read_open;
   typedef seq< white_star, one< ')' > > read_close;

   typedef one_list< '+', '-' > read_line_op;
   typedef one_list< '*', '/', '%' > read_dot_op;

   struct read_expr;

   struct read_value
	 : sor< read_number, seq< read_open, read_expr, read_close > > {};

   template< typename RuleP, typename Rule1 >
   struct read_op
   {
      static std::string key()
      {
	 return typeid( read_op ).name();
      }

      typedef seq< white_star, write_first< RuleP > > RuleW;

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< RuleP, Rule1, RuleW >();
      }

      template< typename Input, typename Debug, typename Output >
      static bool s_match( Input & in, Debug & de, Output & out )
      {
	 position< Input > p( in );

	 if ( ! de.template match< Rule1 >( in, out ) ) {
	    return p( false );
	 }
	 char op = 0;
	 while ( de.template match< RuleW >( false, in, op ) ) {
	    Output ou2;
	    if ( ! de.template match< Rule1 >( true, in, ou2 ) ) {
	       return p( false );
	    }
	    switch ( op ) {
	       case '+':
		  out += ou2;
		  break;
	       case '-':
		  out -= ou2;
		  break;
	       case '*':
		  out *= ou2;
		  break;
	       case '/':
		  out /= ou2;
		  break;
	       case '%':
		  out %= ou2;
		  break;
	       default:
		  assert( false );
		  break;
	    }
	 }
	 return p( true );
      }
   };

   struct read_product
	 : read_op< read_dot_op, read_value > {};

   struct read_expr
	 : read_op< read_line_op, read_product > {};

   struct read_calc
	 : seq< read_expr, white_star, eof > {};

} // pegtl

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg )
   {
      try
      {
	 int result = 0;
	 pegtl::input input( argv[ arg ], "command line" );
	 pegtl::parser< pegtl::read_calc, pegtl::basic_debug > parser;
	 
	 if ( parser( input, result ) ) {
	    std::cerr << "input " << argv[ arg ] << " result " << result << "\n";
	 }
	 else {
	    std::cerr << "input " << argv[ arg ] << " invalid\n";
	 }
      }
      catch ( std::exception & e )
      {
	 UTILS_PRINT( "error: " << e.what() );
      }
   }
   return 0;
}
