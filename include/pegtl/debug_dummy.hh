// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_DUMMY_HH
#define COHI_PEGTL_DEBUG_DUMMY_HH


namespace pegtl
{
   struct dummy_debug
   {
      dummy_debug()
      { }

      template< typename TopRule >
      explicit
      dummy_debug( const tag< TopRule > & )
      { }

      static void log( const std::string & message )
      {
	 std::cerr << message << std::endl;
      }

      template< bool Must, typename Rule, typename Input, typename ... States >
      bool match( Input & in, States && ... st )
      {
	 if ( Rule::template match< Must >( in, * this, std::forward< States >( st ) ... ) ) {
	    return true;
	 }
	 else if ( Must ) {
	    PEGTL_THROW( "pegtl: required rule " << demangle< Rule >() << " failed" );
	 }
	 return false;
      }
   };

} // pegtl

#endif
