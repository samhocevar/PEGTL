// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

// The first non-trivial grammar used during development and debugging of the library.
// This grammar recognises a subset of parsing expression grammar rules.

namespace grammar
{
   using namespace pegtl;

   struct read_expr;

   struct read_comment
	 : seq< one< '#' >, until_eol > {};

   struct read_terminal_char
	 : ifmust< one< '\'' >, seq< not_one< '\'' >, one< '\'' > > > {};

   struct read_terminal_string
	 : ifmust< one< '"' >, seq< star< not_one< '"' > >, one< '"' > > > {};

   struct read_infix
	 : pad< list< '/', '.' >, blank > {};

   struct read_prefix
	 : pad< list< '!', '&' >, blank > {};

   struct read_postfix
	 : pad< list< '+', '*', '?' >, blank > {};

   struct read_paren
	 : ifmust< pad_one< '(', blank >, seq< read_expr, pad_one< ')', blank > > > {};

   struct read_atomic
	 : sor< read_terminal_char, read_terminal_string > {};

   struct read_body
	 : sor< read_paren, read_atomic > {};

   struct read_expr
	 : seq< opt< read_prefix >, read_body, opt< read_postfix >, ifthen< read_infix, read_expr > > {};

   struct read_rule
	 : seq< identifier, pad_one< '=', blank >, read_expr > {};

   struct read_line
	 : sor< read_comment, read_rule > {};

   struct read_file
	 : until< read_line, white_until_eof > {};

} // grammar

int main( int argc, char ** argv )
{
   for ( int arg = 1; arg < argc; ++arg ) {
      if ( pegtl::parse< grammar::read_file >( true, argv[ arg ], "command line argument" ) ) {
	 std::cerr << "input " << argv[ arg ] << " valid\n";
      }
      else {
	 std::cerr << "input " << argv[ arg ] << " invalid\n";
      }
   }
   return 0;
}
