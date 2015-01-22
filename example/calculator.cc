// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl/pegtl.hh>

// The first program that was used for debugging in the early phase of PEGTL development.
// It evaluates each command line argument as arithmetic expression consisting of
// - integers with optional sign,
// - the four basic arithmetic operations,
// - grouping brackets.
// For example input "3 * ( -7 + 9)" yields result 6.

namespace calculator
{
   using namespace pegtl;

   typedef int value_type;

   struct stack
	 : std::vector< value_type >
   {
      value_type pull()
      {
	 assert( !empty() );
	 value_type nrv = back();
	 pop_back();
	 return nrv;
      }
   };

   template< typename Operation >
   struct op_action
   {
      template< typename Rule, typename Stack >
      static void matched( const std::string &, Stack & s )
      {
	 const decltype( s.pull() ) a = s.pull();
	 const decltype( s.pull() ) b = s.pull();
	 s.push_back( Operation()( b, a ) );
      }
   };

   struct push_action
   {
      template< typename Rule, typename Stack >
      static void matched( const std::string & m, Stack & s )
      {
	 s.push_back( string_to_signed< typename Stack::value_type >( m ) );
      }
   };

   struct read_number
	 : seq< opt< list< '+', '-' > >, plus< digit > > {};

   struct push_number
	 : pad< action< read_number, push_action >, space > {};

   template< int C >
   struct calc_pad
	 : pad_one< C, space > {};

   struct read_open
	 : calc_pad< '(' > {};

   struct read_close
	 : calc_pad< ')' > {};

   struct read_expr;

   struct read_atom
	 : sor< push_number, seq< read_open, read_expr, read_close > > {};

   struct read_mul
	 : action< ifmust< calc_pad< '*' >, read_atom >, op_action< std::multiplies< value_type > > > {};

   struct read_div
	 : action< ifmust< calc_pad< '/' >, read_atom >, op_action< std::divides< value_type > > > {};

   struct read_prod
	 : seq< read_atom, star< sor< read_mul, read_div > > > {};

   struct read_add
	 : action< ifmust< calc_pad< '+' >, read_prod >, op_action< std::plus< value_type > > > {};

   struct read_sub
	 : action< ifmust< calc_pad< '-' >, read_prod >, op_action< std::minus< value_type > > > {};

   struct read_expr
	 : seq< read_prod, star< sor< read_add, read_sub > > > {};

   struct read_calc
	 : seq< read_expr, white_star, eof > {};

} // calculator

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      calculator::stack stack;
      if ( pegtl::basic_parse_arg< calculator::read_calc >( argv[ arg ], arg, stack ) ) {
	 assert( stack.size() == 1 );
	 std::cerr << "input " << argv[ arg ] << " result " << stack.front() << "\n";
      }
      else {
	 std::cerr << "input " << argv[ arg ] << " invalid\n";
      }
   }
   return 0;
}
