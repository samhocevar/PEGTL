// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_COUNT_HH
#define COHI_PEGTL_DEBUG_COUNT_HH

namespace pegtl
{
   struct counter
   {
      counter()
	    : m_rule_counter( 0 ),
	      m_nest_counter( 0 )
      { }

      size_t rule() const
      {
	 return m_rule_counter;
      }

      size_t nest() const
      {
	 return m_nest_counter;
      }

      void reset()
      {
	 m_rule_counter = 0;
	 m_nest_counter = 0;
      }

      void enter()
      {
	 ++m_rule_counter;
	 ++m_nest_counter;
      }

      void leave()
      {
	 --m_nest_counter;
      }

   private:
      size_t m_rule_counter;
      size_t m_nest_counter;      
   };

} // pegtl

#endif
