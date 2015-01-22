// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

// The first program that was used for debugging in the early phase of PEGTL development.
// It evaluates each command line argument as arithmetic expression consisting of
// - integers with optional sign,
// - the 5 basic arithmetic operations,
// - grouping brackets.
// For example input "3 * ( -7 + 9)" yields result 6.

namespace calculator
{
   using namespace pegtl;

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

   struct cast_action
   {
      template< typename Rule, typename Signed >
      static void matched( const std::string & m, Signed & s )
      {
	 s = utils::string_to_signed< Signed >( m );
      }
   };

   template< typename Rule >
   struct write_cast
	 : action< Rule, cast_action > {};

   struct read_number
	 : seq< white_star, write_cast< seq< opt< one_list< '+', '-' > >, plus< digit > > > > {};

   struct read_open
	 : seq< white_star, one< '(' > > {};

   struct read_close
	 : seq< white_star, one< ')' > > {};

   struct read_line_op
	 : one_list< '+', '-' > {};

   struct read_dot_op
	 : one_list< '*', '/', '%' > {};

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

} // calculator

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      int result = 0;
	 
      if ( pegtl::parse< calculator::read_calc >( argv[ arg ], "command line agument", result ) ) {
	 std::cerr << "input " << argv[ arg ] << " result " << result << "\n";
      }
      else {
	 std::cerr << "input " << argv[ arg ] << " invalid\n";
      }
   }
   return 0;
}
