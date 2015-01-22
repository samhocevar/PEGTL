// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

namespace example
{
   // Simple example that shows how the capture
   // class is used, both as rule and as action.

   using namespace pegtl;

   // For every given key, a grammar must always
   // first contain the action with class capture,
   // and later use class capture as rule.
   // The action creates (or updates) an entry in
   // the map that is passed as state with the
   // matched sub-string from the rule to which
   // class capture is attaced as action, the rule
   // later retrieves the corresponding entry from
   // the state, here: the map, and tries to match
   // the input against the retrieved string.

   // This example matches all strings that consist
   // of the same two sequences of digits, separated
   // by a non-empty sequence of tabs and spaces.

   struct grammar
	 : seq< action< plus< digit >, capture< 42 > >, plus< blank >, capture< 42 >, eof > {};

   // Note that class capture as rule simply fails
   // when no entry to match can be found in the map.

} // example

int main( int argc, char ** argv )
{
   pegtl::capture_map map;

   for ( int i = 1; i < argc; ++i ) {
      if ( pegtl::basic_parse_string_nothrow< example::grammar >( argv[ i ], map ) ) {
	 PEGTL_PRINT( "input " << argv[ i ] << " map entry " << map[ 42 ] );
      }
      else {
	 PEGTL_PRINT( "input " << argv[ i ] << " invalid" );
      }
   }
   return 0;
}
