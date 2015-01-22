// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_INPUT_HH
#define COHI_PEGTL_INPUT_HH


namespace pegtl
{
   // Helper class to keep track of line and column numbers in a file.

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

   // Helper class to prevent leaking file descriptors.

   struct file_impl
   {
      explicit
      file_impl( const std::string & filename )
	    : m_fd( ::open( filename.c_str(), O_RDONLY ) ),
	      m_fn( filename )
      {
	 if ( m_fd < 0 ) {
	    PEGTL_THROW( "pegtl: unable to open() file " << m_fn << " for reading errno " << errno );
	 }
      }

      ~file_impl()
      {
	 if ( m_fd > -1 ) {
	    ::close( m_fd );
	 }
      }

      std::string read()
      {
	 errno = 0;
	 struct stat st;

	 if ( ::fstat( m_fd, & st ) < 0 ) {
	    PEGTL_THROW( "pegtl: unable to fstat() file " << m_fn << " descriptor " << m_fd << " errno " << errno );
	 }
	 std::string nrv( size_t( st.st_size ), '\0' );

	 if ( ::read( m_fd, const_cast< char * >( nrv.data() ), nrv.size() ) != int( nrv.size() ) ) {
	    PEGTL_THROW( "pegtl: unable to read() file " << m_fn << " descriptor " << m_fd << " errno " << errno );
	 }
	 return nrv;
      }

   private:
      const int m_fd;
      const std::string m_fn;
   };

   // Convenience function to read a file in one go.

   inline std::string read_file( const std::string & filename )
   {
      return file_impl( filename ).read();
   }

   // Input abstraction for the PEGTL with several constructors...

   class input
   {
   public:
      explicit
      input( const std::string & filename )
	    : m_string( read_file( filename ) ),
	      m_debug( "file " + filename ),
	      m_run( m_string ),
	      m_end( m_string.end() )
      { }

      input( const std::string & string, const int arg )
	    : m_string( string ),
	      m_debug( "command line argument " + to_string( arg ) ),
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
	 return m_run.get() >= m_end;
      }

      iterator here() const
      {
	 return m_run;
      }

      pegtl::where where() const
      {
	 return m_run.where();
      }

      bool test() const
      {
	 if ( eof() ) {
	    PEGTL_THROW( "pegtl: attempt to read beyond end of input" );
	 }
	 return true;
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

   // Helper class for the implementation of rules:
   // destructor rewinds to initial position in input when operator()
   // was not called with 'true' before.

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

   // Helper class for the implementation of rules that read a character:
   // only actually consumes the input when operator() is called with 'true'.

   template< typename Input >
   struct character
   {
   public:
      explicit character( Input & in )
	    : m_input( in ),
	      m_value( in.test() ? ( * in.here() ) : 0xf1234567 )
      { }

      operator int () const
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
      const int m_value;
   };

} // pegtl

#endif
