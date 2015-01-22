// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_HH
#define COHI_PEGTL_PARSE_HH


namespace pegtl
{
   struct dummy_debug
   {
      bool must() const
      {
	 return true;  // Don't care, could also be false.
      }

      template< typename Rule >
      void init()
      { }

      template< typename Rule, typename Input, typename... Class >
      bool match( Input & in, Class && ... cl )
      {
	 return Rule::template s_match( in, std::forward< Class >( cl ) ... );
      }
	 
      template< typename Rule, typename Input, typename... Class >
      bool match( const bool, Input & in, Class && ... cl )
      {
	 return Rule::template s_match( in, std::forward< Class >( cl ) ... );
      }
   };


   template< typename Rule, typename Debug >
   class parser
   {
   public:
      parser()
      {
	 m_debug.template init< Rule >();
      }

      explicit
      parser( const Debug & de )
	    : m_debug( de )
      { }

      template< typename ... Class > explicit
      parser( Class && ... cl )
	    : m_debug( std::forward< Class >( cl ) ... )
      {
	 m_debug.template init< Rule >();
      }

      template< typename Input, typename ... Class >
      bool operator() ( Input & in, Class && ... cl )
      {
	 try
	 {
	    return m_debug.template match< Rule >( true, in, std::forward< Class >( cl ) ... );
	 }
	 catch ( std::exception & e )
	 {
	    UTILS_PRINT( "pegtl: parsing input from " << in.debug() << " aborted" );
	    UTILS_PRINT( e.what() );
	 }
	 return false;
      }

   private:
      Debug m_debug;
   };


   template< typename Rule, typename Debug = basic_debug, typename ... Class >
   bool parse( const std::string & i, const int d, Class && ... cl )
   {
      input in( i, d );
      return parser< Rule, Debug >()( in, std::forward< Class >( cl ) ... );
   }


   template< typename Rule, typename Debug = basic_debug, typename ... Class >
   bool parse( const std::string & i, const std::string & d, Class && ... cl )
   {
      input in( i, d );
      return parser< Rule, Debug >()( in, std::forward< Class >( cl ) ... );
   }


   template< typename Rule, typename Debug = trace_debug, typename ... Class >
   bool parse( const bool trace, const std::string & i, const std::string & d, Class && ... cl )
   {
      input in( i, d );
      return parser< Rule, Debug >( trace )( in, std::forward< Class >( cl ) ... );
   }

} // pegtl

#endif
