// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_BASIC_HH
#define COHI_PEGTL_DEBUG_BASIC_HH


namespace pegtl
{
   template< typename Rule, typename Input >
   struct basic_guard
   {
      typedef typename Input::location_type Location;

      basic_guard( Location && w, counter & t, printer & p )
	    : m_result( 2 ),
	      m_location( w ),
	      m_counter( t ),
	      m_printer( p ),
	      m_must_old( t.must() )
      {
	 m_counter.enter();
      }

      basic_guard( Location && w, counter & t, printer & p, const bool must )
	    : m_result( 2 ),
	      m_location( w ),
	      m_counter( t ),
	      m_printer( p ),
	      m_must_old( t.must() )
      {
	 m_counter.enter();
	 m_counter.set_must( must );
      }

      ~basic_guard()
      {
	 if ( m_result > 1 ) {
	    trace_error();
	 }
	 m_counter.leave( m_must_old );
      }

      bool operator() ( const bool result )
      {
	 if ( !( m_result = result ) && m_counter.must() ) {
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

      const bool m_must_old;

      void trace_error()
      {
	 const std::string rule = m_printer.template rule< Rule >();

	 if ( ! rule.empty() ) {
	    PEGTL_PRINT( "pegtl: nesting #" << std::setw( 2 ) << m_counter.nest() << " at " << m_location << " rule " << rule );
	 }
      }
   
      void throw_error()
      {
	 PEGTL_PRINT( "pegtl: syntax error at " << m_location );
	 PEGTL_THROW( "pegtl: parsing aborted at " << m_location );
      }
   };


   struct basic_debug
   {
      template< typename TopRule >
      basic_debug( const tag< TopRule > & help )
	    : m_printer( help )
      { }

      bool must() const
      {
	 return m_counter.must();
      }

      template< typename Rule, typename Input, typename... Class >
      bool match( Input & in, Class && ... cl )
      {
	 basic_guard< Rule, Input > d( in.location(), m_counter, m_printer );
	 return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
      }
	 
      template< typename Rule, typename Input, typename... Class >
      bool match( const bool must, Input & in, Class && ... cl )
      {
	 basic_guard< Rule, Input > d( in.location(), m_counter, m_printer, must );
	 return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
      }

   protected:
      counter m_counter;
      printer m_printer;
   };

} // pegtl

#endif
