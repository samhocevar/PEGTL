// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_COUNTER_HH
#define COHI_PEGTL_PARSE_COUNTER_HH

namespace pegtl
{
   struct counter
   {
      explicit
      counter( const size_t rule_maximum = 1234567890, const size_t nest_maximum = 123456 )
	    : m_must( true ),
	      m_rule_counter( 0 ),
	      m_nest_counter( 0 ),
	      m_rule_maximum( rule_maximum ),
	      m_nest_maximum( nest_maximum )
      { }

      bool must() const
      {
	 return m_must;
      }

      size_t rule() const
      {
	 return m_rule_counter;
      }

      size_t nest() const
      {
	 return m_nest_counter;
      }

      void set_must( const bool m )
      {
	 m_must = m;
      }

      void reset()
      {
	 m_must = true;
	 m_rule_counter = 0;
	 m_nest_counter = 0;
      }

      void enter()
      {
	 if ( ++m_rule_counter > m_rule_maximum ) {
	    PEGTL_THROW( "pegtl: total number of parsing expression grammar rule applications exceeds limit " << m_rule_maximum );
	 }
	 if ( ++m_nest_counter > m_nest_maximum ) {
	    PEGTL_THROW( "pegtl: number of nested parsing expression grammar rule applications exceeds limit " << m_nest_maximum );
	 }
      }

      void leave()
      {
	 --m_nest_counter;
      }

      void leave( const bool m )
      {
	 leave();
	 set_must( m );
      }

   private:
      bool m_must;

      size_t m_rule_counter;
      size_t m_nest_counter;      

      const size_t m_rule_maximum;
      const size_t m_nest_maximum;
   };

} // pegtl

#endif
