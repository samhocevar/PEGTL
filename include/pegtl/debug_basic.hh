// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_BASIC_HH
#define COHI_PEGTL_DEBUG_BASIC_HH


namespace pegtl
{
   template< typename Rule, typename Input, typename Debug >
   struct basic_guard
   {
      typedef typename Input::location_type Location;

      basic_guard( Location && w, counter & t, printer & p )
	    : m_result( 2 ),
	      m_location( w ),
	      m_counter( t ),
	      m_printer( p )
      {
	 m_counter.enter();
      }

      ~basic_guard()
      {
	 if ( m_result > 1 ) {
	    trace_error();
	 }
	 m_counter.leave();
      }

      bool operator() ( const bool result, const bool must )
      {
	 if ( !( m_result = result ) && must ) {
	    m_result = 2;
	    throw_error();
	 }
	 return result;
      }

   protected:
      unsigned m_result;
      const Location m_location;
      counter & m_counter;
      printer & m_printer;

      void trace_error()
      {
	 const std::string rule = m_printer.template rule< Rule >();

	 if ( ! rule.empty() ) {
	    PEGTL_LOGGER( Debug, "pegtl: nesting #" << std::setw( 2 ) << m_counter.nest() << " at " << m_location << " rule " << rule );
	 }
      }
   
      void throw_error()
      {
	 PEGTL_LOGGER( Debug, "pegtl: syntax error at " << m_location );
	 PEGTL_THROW( "pegtl: parsing aborted at " << m_location );
      }
   };


   struct basic_debug
   {
      template< typename TopRule >
      explicit
      basic_debug( const tag< TopRule > & help )
	    : m_printer( help )
      { }

      static void log( const std::string & message )
      {
	 std::cerr << message << std::endl;
      }

      template< bool Must, typename Rule, typename Input, typename ... States >
      bool match( Input & in, States && ... st )
      {
	 basic_guard< Rule, Input, basic_debug > d( in.location(), m_counter, m_printer );
	 return d( Rule::template s_match< Must >( in, *this, std::forward< States >( st ) ... ), Must );
      }
	 
   protected:
      counter m_counter;
      printer m_printer;
   };

} // pegtl

#endif
