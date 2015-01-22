// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_GENERIC_HH
#define COHI_PEGTL_PARSE_GENERIC_HH

namespace pegtl
{
   template< typename TopRule, typename Input, typename Debug, typename ... Class >
   bool parse( Input & in, Debug & de, Class && ... cl )
   {
      try
      {
	 return de.template match< TopRule >( true, in, std::forward< Class >( cl ) ... );
      }
      catch ( std::exception & e )
      {
	 PEGTL_PRINT( "pegtl: parsing input from " << in.debug_source() << " aborted" );
	 PEGTL_PRINT( e.what() );
      }
      return false;
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool dummy_parse( Input & in, Class && ... cl )
   {
      dummy_debug de( tag< TopRule >( 0 ) );
      return parse< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool basic_parse( Input & in, Class && ... cl )
   {
      basic_debug de( tag< TopRule >( 0 ) );
      return parse< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool trace_parse( const bool trace, Input & in, Class && ... cl )
   {
      trace_debug de( tag< TopRule >( 0 ), trace );
      return parse< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

} // pegtl

#endif
