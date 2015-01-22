// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#include <pegtl.hh>

// The first example from the documentation.

namespace
{
   using namespace pegtl;

   struct first
	 : seq< alpha, star< sor< alpha, digit > > > {};
}

int main( int argc, char ** argv )
{
   for ( int i = 1; i < argc; ++i ) {
      pegtl::parse< first >( argv[ i ], "command line argument" );
   }
   return 0;
}
