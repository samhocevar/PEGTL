// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_DEBUGGER_HH
#define COHI_PEGTL_PARSE_DEBUGGER_HH


namespace pegtl
{
   struct dummy_debug
   {
      dummy_debug()
      { }

      template< typename TopRule >
      dummy_debug( const tag< TopRule > & )
      { }

      bool must() const
      {
	 return true;  // Don't care, could also be false.
      }

      template< typename Rule, typename Input, typename... Class >
      bool match( Input & in, Class && ... cl )
      {
	 return Rule::template s_match( in, *this, std::forward< Class >( cl ) ... );
      }
	 
      template< typename Rule, typename Input, typename... Class >
      bool match( const bool, Input & in, Class && ... cl )
      {
	 return Rule::template s_match( in, *this, std::forward< Class >( cl ) ... );
      }
   };

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

   template< typename Rule, typename Input >
   struct trace_guard : private basic_guard< Rule, Input >
   {
   public:
      trace_guard( Input & in, counter & t, printer & p )
	    : basic_guard< Rule, Input >( in.location(), t, p ),
	      m_input( in ),
	      m_begin( in.here() ),
	      m_rule_counter( t.rule() )
      {
	 print_debug( "start  " );
      }

      trace_guard( Input & in, counter & t, printer & p, const bool must )
	    : basic_guard< Rule, Input >( in.location(), t, p, must ),
	      m_input( in ),
	      m_begin( in.here() ),
	      m_rule_counter( t.rule() )
      {
	 print_debug( "start  " );
      }

      ~trace_guard()
      {
	 const char * const msgs[] = { "failure", "success", "unwind " };
	 print_debug( msgs[ this->m_result ] );
      }

      bool operator() ( const bool result )
      {
	 this->m_result = result;
	 print_debug( "result " );
	 return basic_guard< Rule, Input >::operator() ( result );
      }

   private:
      Input & m_input;
      const typename Input::iterator m_begin;

      const size_t m_rule_counter;

      void print_debug( const char * msg )
      {
	 PEGTL_PRINT( "pegtl: " << msg << " flags " << this->m_must_old << this->m_counter.must() << this->m_result << " rule " << std::setw( 4 ) << m_rule_counter << " nest " << std::setw( 3 ) << this->m_counter.nest() << " at " << m_input.location() << " expression " << this->m_printer.template rule< Rule >() << " input \"" << m_input.debug_escape( m_begin, m_input.here() ) << "\"" );
      }
   };

   struct trace_debug
   {
      template< typename TopRule >
      trace_debug( const tag< TopRule > & help, const bool trace = true )
	    : m_trace( trace ),
	      m_printer( help )
      { }

      bool must() const
      {
	 return m_counter.must();
      }

      bool trace() const
      {
	 return m_trace;
      }

      void set_trace( const bool trace )
      {
	 m_trace = trace;
      }

      template< typename Rule, typename Input, typename ... Class >
      bool match( Input & in, Class && ... cl )
      {
	 if ( m_trace ) {
	    trace_guard< Rule, Input > d( in, m_counter, m_printer );
	    return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
	 }
	 else {
	    basic_guard< Rule, Input > d( in.location(), m_counter, m_printer );
	    return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
	 }
      }
	 
      template< typename Rule, typename Input, typename ... Class >
      bool match( const bool must, Input & in, Class && ... cl )
      {
	 if ( m_trace ) {
	    trace_guard< Rule, Input > d( in, m_counter, m_printer, must );
	    return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
	 }
	 else {
	    basic_guard< Rule, Input > d( in.location(), m_counter, m_printer, must );
	    return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
	 }
      }

   protected:
      bool m_trace;
      counter m_counter;
      printer m_printer;
   };

} // pegtl

#endif
