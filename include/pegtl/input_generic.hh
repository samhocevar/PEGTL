// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_INPUT_GENERIC_HH
#define COHI_PEGTL_INPUT_GENERIC_HH


namespace pegtl
{
   // The following classes are helper classes to simplify the implementation
   // of rules: used correctly, they make adherence to 'do not consume input
   // on failure' easy.
   // They are modelled after a transactional style of programming.
   // The constructor is the 'begin', and 'operator()' when called with
   // a value of 'true' is the 'commit'. A 'rollback' is performed either
   // when 'operator()' is called with 'false', or when the destructor is
   // called without 'operator()' having been called earlier; this provides
   // for correct behaviour in the presence of exceptions (side note: in C++,
   // the destructor is the most important 'thing' for exception safety, not
   // the try-catch block, as in some other languages I do not want to name).

   // Class marker remembers (marks) the current position in the input; a
   // 'commit' is a nop, a 'rollback' rewinds to the initial position.

   template< typename Input >
   class marker
   {
   public:
      explicit
      marker( Input & in )
	    : m_input( & in ),
	      m_iterator( in.here() )
      { }

      ~marker()
      {
	 if ( m_input ) {
	    m_input->jump( m_iterator );
	 }
      }

      typedef typename Input::iterator iterator;

      iterator here() const
      {
	 return m_iterator;
      }

      bool operator() ( const bool success )
      {
	 if ( success ) {
	    m_input = 0;
	 }
	 return success;
      }

   private:
      Input * m_input;
      const typename Input::iterator m_iterator;
   };

   // Class character acts as proxy to the current character in the input;
   // a 'commit' consumes the character (thereby moving the input to the next
   // position), a 'rollback' is a nop.

   template< typename Input >
   struct character
   {
   public:
      explicit
      character( Input & in )
	    : m_input( in ),
	      m_value( in.peek() )
      { }

      typedef typename Input::value_type value_type;

      operator const value_type & () const
      {
	 return m_value;
      }
      
      bool operator() ( const bool success ) const
      {
	 if ( success ) {
	    m_input.bump();
	 }
	 return success;
      }

   private:
      Input & m_input;
      const value_type m_value;
   };

   // Please see the supplied rule classes for usage examples...

} // pegtl

#endif
