// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_INPUT_HH
#define COHI_PEGTL_INPUT_HH


namespace pegtl
{
   class where
   {
   public:
      where()
	    : m_line( 1 ),
	      m_column( 1 )
      { }

      int operator() ( const int c )
      {
	 if ( c == '\n' ) {
	    ++m_line;
	    m_column = 0;
	 }
	 ++m_column;
	 return c;
      }

      void write_to( std::ostream & o ) const
      {
	 o << "line=" << m_line << " column=" << m_column;
      }

   private:
      size_t m_line;
      size_t m_column;
   };


   inline std::ostream & operator<< ( std::ostream & o, const where & w )
   {
      w.write_to( o );
      return o;
   }


   class input
   {
   public:
      explicit
      input( const std::string & filename )
	    : m_string( utils::read_file( filename ) ),
	      m_debug( "file '" + filename + "'" ),
	      m_run( m_string ),
	      m_end( m_string.end() )
      { }

      input( const std::string & string, const int )
	    : m_string( string ),
	      m_debug( "command line argument" ),
	      m_run( m_string ),
	      m_end( m_string.end() )
      { }
      
      input( const std::string & string, const std::string & source )
	    : m_string( string ),
	      m_debug( source ),
	      m_run( m_string ),
	      m_end( m_string.end() )
      { }
      
      template< typename Iterator >
      input( const Iterator & begin, const Iterator & end, const std::string & source )
	    : m_string( begin, end ),
	      m_debug( source ),
	      m_run( m_string ),
	      m_end( m_string.end() )
      { }

      class iterator : public std::iterator< std::forward_iterator_tag, char >
      {
      public:
	 explicit
	 iterator( const std::string & s )
	       : m_iterator( s.begin() )
	 { }

	 char operator* () const
	 {
	    return *m_iterator;
	 }

	 iterator operator++ ( int )
	 {
	    iterator nrv( *this );
	    this->operator++ ();
	    return nrv;
	 }

	 const iterator & operator++ ()
	 {
	    m_where( *m_iterator );
	    ++m_iterator;
	    return *this;
	 }

	 const pegtl::where & where() const
	 {
	    return m_where;
	 }

	 const std::string::const_iterator & get() const
	 {
	    return m_iterator;
	 }

	 bool operator== ( const iterator & i ) const
	 {
	    return i.get() == m_iterator;
	 }

	 bool operator!= ( const iterator & i ) const
	 {
	    return i.get() != m_iterator;
	 }

      private:
	 pegtl::where m_where;
	 std::string::const_iterator m_iterator;
      };

      bool eof() const
      {
	 return m_run.get() == m_end;
      }

      iterator here() const
      {
	 return m_run;
      }

      pegtl::where where() const
      {
	 return m_run.where();
      }

      void test() const
      {
	 if ( eof() ) {
	    UTILS_THROW( "input exhausted error" );
	 }
      }

      void bump()
      {
	 test();
	 ++m_run;
      }

      void jump( const iterator iter )
      {
	 m_run = iter;
      }
      
      int peek() const
      {
	 test();
	 return * m_run;
      }

      const std::string & debug() const
      {
	 return m_debug;
      }

   private:
      const std::string m_string;
      const std::string m_debug;
      iterator m_run;
      const std::string::const_iterator m_end;
   };


   template< typename Input >
   class position
   {
   public:
      explicit position( Input & in )
	    : m_input( & in ),
	      m_iterator( in.here() )
      { }

      ~position()
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
	 if ( m_input && ! success ) {
	    m_input->jump( m_iterator );
	 }
	 m_input = 0;
	 return success;
      }

   private:
      Input * m_input;
      const iterator m_iterator;
   };


   template< typename Input >
   struct character
   {
   public:
      explicit character( Input & in )
	    : m_input( in ),
	      m_iterator( in.here() )
      { }

      operator int () const
      {
	 return * m_iterator;
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
      const typename Input::iterator m_iterator;
   };

} // pegtl

#endif
