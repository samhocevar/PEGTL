// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_INPUT_GENERIC_HH
#define COHI_PEGTL_INPUT_GENERIC_HH


namespace pegtl
{
   template< typename Iterator, typename Location >
   class forward_iterator : public std::iterator< std::forward_iterator_tag, typename Iterator::value_type >
   {
   public:
      forward_iterator()
      { }

      explicit
      forward_iterator( const Iterator & it )
	    : m_iterator( it )
      { }

      typedef typename std::iterator_traits< Iterator >::value_type value_type;

      value_type operator* () const
      {
	 return * m_iterator;
      }
      
      const value_type * operator-> () const
      {
	 return m_iterator.operator->();
      }

      forward_iterator operator++ ( int )
      {
	 forward_iterator nrv( * this );
	 this->operator++ ();
	 return nrv;
      }

      const forward_iterator & operator++ ()
      {
	 m_location( * m_iterator );
	 ++m_iterator;
	 return *this;
      }

      const Location & location() const
      {
	 return m_location;
      }
      
      bool operator== ( const forward_iterator & i ) const
      {
	 return m_iterator == i.m_iterator;
      }

      bool operator!= ( const forward_iterator & i ) const
      {
	 return m_iterator != i.m_iterator;
      }

      bool operator>= ( const forward_iterator & i ) const
      {
	 return m_iterator >= i.m_iterator;
      }

      void assign( const Iterator & it )
      {
	 m_iterator = it;
	 m_location = Location();
      }

   private:
      Location m_location;
      Iterator m_iterator;
   };

   template< typename Iterator, typename Location >
   class iterator_input : private nocopy< iterator_input< Iterator, Location > >
   {
   public:
      iterator_input( const Iterator begin, const Iterator end, const std::string & source )
	    : m_run( begin ),
	      m_begin( begin ),
	      m_end( end ),
	      m_source( source )
      { }

      typedef Location location_type;
      typedef forward_iterator< Iterator, Location > iterator;
      typedef typename std::iterator_traits< Iterator >::value_type value_type;

      bool eof() const
      {
	 return m_run >= m_end;
      }

      const iterator & here() const
      {
	 return m_run;
      }

      const iterator & end() const
      {
	 return m_end;
      }

      iterator_input & rewind()
      {
	 m_run = m_begin;
	 return * this;
      }

      Location location() const
      {
	 return m_run.location();
      }

      void bump()
      {
	 throw_at_eof();
	 ++m_run;
      }

      value_type peek() const
      {
	 throw_at_eof();
	 return *m_run;
      }

      void jump( const iterator iter )
      {
	 m_run = iter;
      }
      
      const std::string & debug_source() const
      {
	 return m_source;
      }

      std::string debug_escape( const iterator & begin, const iterator & end ) const
      {
	 std::string nrv;

	 for ( iterator run = begin; run != end; ++run ) {
	    escape_impl( nrv, * run );
	 }
	 return nrv;
      }

   protected:
      iterator m_run;

      const iterator m_begin;
      const iterator m_end;

      const std::string m_source;

      void throw_at_eof() const
      {
	 if ( eof() ) {
	    PEGTL_THROW( "pegtl: attempt to read beyond end of input" );
	 }
      }
   };

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
      explicit marker( Input & in )
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
      explicit character( Input & in )
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
