// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

namespace calculator
{
   // The first program used during development and debugging
   // of the library that uses actions. It evaluates each command
   // line argument as arithmetic expression consisting of
   // - integers with optional sign,
   // - the four basic arithmetic operations,
   // - grouping brackets.
   // For example input "3 * ( -7 + 9)" yields result 6.
   
   using namespace pegtl;

   // The state.

   // Canonical use of an evaluation stack,
   // here implemented with a std::vector.

   typedef int value_type;
   typedef std::vector< value_type > stack_type;

   // Helper function that's [exception] safe with ints as values.

   value_type pull( stack_type & s )
   {
      assert( ! s.empty() );
      value_type nrv( s.back() );
      s.pop_back();
      return nrv;
   }
   
   // The actions.

   // This action converts the matched sub-string
   // to an integer and pushes it on the stack,
   // which must be its only additional state argument.

   struct push_action
   {
      static void apply( const std::string & m, stack_type & s )
      {
	 s.push_back( string_to_signed< value_type >( m ) );
      }
   };

   // Class op_action performs an operation on the two
   // top-most elements of the evaluation stack. This
   // should always be possible in the sense that the
   // grammar must make sure to only apply this action
   // when sufficiently many operands are on the stack.

   template< typename Operation >
   struct op_action
   {
      static void apply( const std::string &, stack_type & s )
      {
	 const value_type a = pull( s );
	 const value_type b = pull( s );
	 s.push_back( Operation()( b, a ) );
      }
   };
   
   template<>
   struct op_action< std::divides< value_type > >
   {
      template< typename State >
      static void apply( const std::string &, State & s )
      {
	 const value_type rhs = pull( s );
	 if ( ! rhs ) {
	    PEGTL_THROW( "pegtl: division by zero" );
	 }
	 const value_type lhs = pull( s );
	 s.push_back( std::divides< value_type >()( lhs, rhs ) );
      }
   };

   // The grammar rules.

   struct read_number
	 : seq< opt< list< '+', '-' > >, plus< digit > > {};

   // This rule uses the rule read_number to match a
   // number in the input and, on success, applies the
   // push_action to the matched sub-string and the
   // state, in calculator.cc: an instance of stack_type,
   // in order to push the number on the evaluation stack.

   struct push_rule
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
	 : sor< push_rule, seq< read_open, read_expr, read_close > > {};

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
	 : seq< read_expr, space_until_eof > {};

} // calculator

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      calculator::stack_type stack;
      if ( pegtl::basic_parse_string_nothrow< calculator::read_calc >( argv[ arg ], stack ) ) {
	 assert( stack.size() == 1 );
	 std::cerr << "input " << argv[ arg ] << " result " << stack.front() << "\n";
      }
      else {
	 std::cerr << "input " << argv[ arg ] << " invalid\n";
      }
   }
   return 0;
}
