// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_TRACE_HH
#define COHI_PEGTL_DEBUG_TRACE_HH


namespace pegtl
{
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

      ~trace_guard()
      {
	 const char * const msgs[] = { "failure", "success", "unwind " };
	 print_debug( msgs[ this->m_result ] );
      }

      bool operator() ( const bool result, const bool must )
      {
	 this->m_result = result;
	 print_debug( "result " );
	 return basic_guard< Rule, Input >::operator() ( result, must );
      }

   private:
      Input & m_input;
      const typename Input::iterator m_begin;

      const size_t m_rule_counter;

      void print_debug( const char * msg )
      {
	 PEGTL_PRINT( "pegtl: " << msg << " flags " << this->m_result << " rule " << std::setw( 4 ) << m_rule_counter << " nest " << std::setw( 3 ) << this->m_counter.nest() << " at " << m_input.location() << " expression " << this->m_printer.template rule< Rule >() << " input \"" << m_input.debug_escape( m_begin, m_input.here() ) << "\"" );
      }
   };


   struct trace_debug
   {
      template< typename TopRule >
      trace_debug( const tag< TopRule > & help, const bool trace = true )
	    : m_trace( trace ),
	      m_printer( help )
      { }

      bool trace() const
      {
	 return m_trace;
      }

      void set_trace( const bool trace )
      {
	 m_trace = trace;
      }

      template< bool Must, typename Rule, typename Input, typename ... States >
      bool match( Input & in, States && ... st )
      {
	 if ( m_trace ) {
	    trace_guard< Rule, Input > d( in, m_counter, m_printer );
	    return d( Rule::template s_match< Must >( in, *this, std::forward< States >( st ) ... ), Must );
	 }
	 else {
	    basic_guard< Rule, Input > d( in.location(), m_counter, m_printer );
	    return d( Rule::template s_match< Must >( in, *this, std::forward< States >( st ) ... ), Must );
	 }
      }

   protected:
      bool m_trace;
      counter m_counter;
      printer m_printer;
   };

} // pegtl

#endif
