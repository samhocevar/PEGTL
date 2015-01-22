// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_GENERIC_HH
#define COHI_PEGTL_PARSE_GENERIC_HH

namespace pegtl
{
   // All parse function return true when parsing succeeds; when parsing
   // fails there are two possibilities:
   // 1) The *_parse_*_throws() functions throw an exception; this can be
   //    either an exception thrown by the library, or an exception thrown
   //    by a user-created matching rule or action.
   // 2) The *_parse_*_nothrow() functions eat all exceptions and return
   //    false.

   // The following two functions are the central wrappers used for parsing.

   template< typename TopRule, typename Input, typename Debug, typename ... Class >
   bool parse_throws( Input & in, Debug & de, Class && ... cl )
   {
      if ( ! de.template match< TopRule >( true, in, std::forward< Class >( cl ) ... ) )
      {
	 // This is not particularly informative, however the only way to trigger this is when
	 // the dummy_debug is used, in which case the user was interested in speed, not good
	 // error messages; when using basic_debug and trace_debug, these classes throw the
	 // exception when the top-level rule fails.
	 PEGTL_THROW( "pegtl: top-level parsing rule " << demangle< TopRule >() << " failed" );
      }
      return true;
   }

   template< typename TopRule, typename Input, typename Debug, typename ... Class >
   bool parse_nothrow( Input & in, Debug & de, Class && ... cl )
   {
      try
      {
	 return de.template match< TopRule >( true, in, std::forward< Class >( cl ) ... );
      }
      catch ( std::exception & e )
      {
	 PEGTL_PRINT( e.what() );
      }
      catch ( ... )
      {
	 PEGTL_PRINT( "pegtl: top-level parsing rule " << demangle< TopRule >() << " failed" );
      }
      return false;
   }

   // The next functions add one convenience layer: they take care of instantiating
   // one (or more) of the debug classes supplied with the library, corresponding to
   // their respective names.

   // Parse with a dummy_debug (full speed, no diagnostics).
   
   template< typename TopRule, typename Input, typename ... Class >
   bool dummy_parse_throws( Input & in, Class && ... cl )
   {
      dummy_debug de( tag< TopRule >( 0 ) );
      return parse_throws< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool dummy_parse_nothrow( Input & in, Class && ... cl )
   {
      dummy_debug de( tag< TopRule >( 0 ) );
      return parse_nothrow< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   // Parse with a dummy_debug (slower, diagnostics on error).

   template< typename TopRule, typename Input, typename ... Class >
   bool basic_parse_throws( Input & in, Class && ... cl )
   {
      basic_debug de( tag< TopRule >( 0 ) );
      return parse_throws< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool basic_parse_nothrow( Input & in, Class && ... cl )
   {
      basic_debug de( tag< TopRule >( 0 ) );
      return parse_nothrow< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   // Parse with a trace_debug (fixed overhead depending on the grammar,
   // diagnostics on error, plus optionally trace of all rule invocations).
   // The first bool argument called trace enables or disables rule tracing.

   template< typename TopRule, typename Input, typename ... Class >
   bool trace_parse_throws( const bool trace, Input & in, Class && ... cl )
   {
      trace_debug de( tag< TopRule >( 0 ), trace );
      return parse_throws< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool trace_parse_nothrow( const bool trace, Input & in, Class && ... cl )
   {
      trace_debug de( tag< TopRule >( 0 ), trace );
      return parse_nothrow< TopRule >( in, de, std::forward< Class >( cl ) ... );
   }

   // Parse with a dummy_debug and, when that fails, use a trace_debug, i.e.
   // try the fastest way, with neither diagnostics nor safety-nets first,
   // and fall-back to a more verbose version only when necessary. In case
   // of errors, the whole input is parsed two times. Note that the state
   // is not cleared in between the two parsing runs; it might be necessary
   // for the application to copy'n'paste this function with an appropriate
   // preparation of the state before starting the second run...!

   template< typename TopRule, typename Input, typename ... Class >
   bool smart_parse_throws( const bool trace, Input & in, Class && ... cl )
   {
      return dummy_parse_nothrow( in, std::forward< Class >( cl ) ... ) || trace_parse_throws( trace, in.rewind(), std::forward< Class >( cl ) ... );
   }

   template< typename TopRule, typename Input, typename ... Class >
   bool smart_parse_nothrow( const bool trace, Input & in, Class && ... cl )
   {
      return dummy_parse_nothrow( in, std::forward< Class >( cl ) ... ) || trace_parse_nothrow( trace, in.rewind(), std::forward< Class >( cl ) ... );
   }

} // pegtl

#endif
