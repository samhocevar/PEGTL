// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_HH
#define COHI_PEGTL_DEBUG_HH


namespace pegtl
{
   template< typename Rule >
   struct basic_guard
   {
      basic_guard( where && w, counter & t, printer & p )
	    : m_result( 2 ),
	      m_where( w ),
	      m_counter( t ),
	      m_printer( p ),
	      m_must_old( t.must() )
      {
	 m_counter.enter();
      }

      basic_guard( where && w, counter & t, printer & p, const bool must )
	    : m_result( 2 ),
	      m_where( w ),
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
      const where m_where;
      counter & m_counter;
      printer & m_printer;

      const bool m_must_old;

      void trace_error()
      {
	 const std::string rule = m_printer.template rule< Rule >();

	 if ( ! rule.empty() ) {
	    UTILS_PRINT( "pegtl: nesting #" << m_counter.nest() << " at " << m_where << " rule " << rule );
	 }
      }
   
      void throw_error()
      {
	 UTILS_PRINT( "pegtl: syntax error at " << m_where );
	 UTILS_THROW( "pegtl: parsing aborted at " << m_where );
      }
   };


   struct basic_debug
   {
      bool must() const
      {
	 return m_counter.must();
      }

      template< typename Rule >
      void init()
      {
	 m_printer.insert< Rule >();
      }

      template< typename Rule, typename Input, typename... Class >
      bool match( Input & in, Class && ... cl )
      {
	 basic_guard< Rule > d( in.where(), m_counter, m_printer );
	 return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
      }
	 
      template< typename Rule, typename Input, typename... Class >
      bool match( const bool must, Input & in, Class && ... cl )
      {
	 basic_guard< Rule > d( in.where(), m_counter, m_printer, must );
	 return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
      }

   protected:
      counter m_counter;
      printer m_printer;
   };


   template< typename Rule, typename Input >
   struct trace_guard : private basic_guard< Rule >
   {
   public:
      trace_guard( Input & in, counter & t, printer & p )
	    : basic_guard< Rule >( in.where(), t, p ),
	      m_input( in ),
	      m_begin( in.here() ),
	      m_rule_counter( t.rule() )
      {
	 print_debug( "start  " );
      }

      trace_guard( Input & in, counter & t, printer & p, const bool must )
	    : basic_guard< Rule >( in.where(), t, p, must ),
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
	 return basic_guard< Rule >::operator() ( result );
      }

   private:
      Input & m_input;
      const typename Input::iterator m_begin;

      const size_t m_rule_counter;

      std::string read_debug() const
      {
	 std::string nrv;
	 const typename Input::iterator end = m_input.here();

	 for ( typename Input::iterator run = m_begin; run != end; ++run ) {
	    utils::escape_impl( nrv, * run );
	 }
	 return nrv;
      }

      void print_debug( const char * msg )
      {
	 UTILS_PRINT( "pegtl: " << msg << " flags " << this->m_must_old << this->m_counter.must() << this->m_result << " rule " << std::setw( 4 ) << m_rule_counter << " nest " << std::setw( 3 ) << this->m_counter.nest() << " at " << m_input.where() << " expression " << this->m_printer.template rule< Rule >() << " input \"" << read_debug() << "\"" );
      }
   };


   struct trace_debug : public basic_debug
   {
      explicit
      trace_debug( const bool trace = true )
	    : m_trace( trace )
      { }

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
	    basic_guard< Rule > d( in.where(), m_counter, m_printer );
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
	    basic_guard< Rule > d( in.where(), m_counter, m_printer, must );
	    return d( Rule::template s_match( in, *this, std::forward< Class >( cl ) ... ) );
	 }
      }

   protected:
      bool m_trace;
   };

} // pegtl

#endif
