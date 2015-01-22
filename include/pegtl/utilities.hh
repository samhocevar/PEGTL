// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_UTILITIES_HH
#define COHI_PEGTL_UTILITIES_HH

namespace pegtl
{
   template< typename T >
   struct tag
   {
      typedef T type;

      tag()
      { }

      explicit
      tag( const int )
      { }

      explicit
      tag( const volatile T * )
      { }
   };

#define PEGTL_PRINT( MeSSaGe )				\
   do { std::cerr << MeSSaGe << '\n'; } while( 0 )

#define PEGTL_TRACE							\
   PEGTL_PRINT( __FILE__ << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ )
   
#define PEGTL_DEBUG( MeSSaGe )						\
   PEGTL_PRINT( __FILE__ << " : " << __LINE__ << " : " << MeSSaGe )

#define PEGTL_THROW( MeSSaGe )						\
   do { std::ostringstream oss; oss << MeSSaGe; throw std::runtime_error( oss.str() ); } while( 1 )

#define PEGTL_LOGGER( DeBuG, MeSSaGe )			\
   do { std::ostringstream oss; oss << MeSSaGe; DeBuG::log( oss.str() ); } while( 0 )

   inline void escape_impl( std::string & result, const int i )
   {
      switch ( i )
      {
	 case '"':
	    result += "\\\"";
	    break;
	 case '\\':
	    result += "\\\\";
	    break;
	 case '\a':
	    result += "\\a";
	    break;
	 case '\b':
	    result += "\\b";
	    break;
	 case '\t':
	    result += "\\t";
	    break;
	 case '\n':
	    result += "\\n";
	    break;
	 case '\r':
	    result += "\\r";
	    break;
	 case '\v':
	    result += "\\v";
	    break;
	 case 32 ... 33:
	 case 35 ... 91:
	 case 93 ... 126:
	    result += char( i );
	    break;
	 default: {
	    char tmp[ 12 ];
	    ::snprintf( tmp, sizeof( tmp ), "\\u%04x", i );
	    result += tmp;
	 }  break;
      }
   }

   inline std::string escape( const int i )
   {
      std::string nrv;
      escape_impl( nrv, i );
      return nrv;
   }

   inline std::string escape( const std::string & s )
   {
      std::string nrv;

      for ( std::string::size_type i = 0; i < s.size(); ++i ) {
	 escape_impl( nrv, s[ i ] );
      }
      return nrv;
   }

   namespace nocopy_impl
   {
      template< typename >
      class nocopy
      {
      protected:
	 nocopy() {}
	 ~nocopy() {}
	 
      private:
	 nocopy( const nocopy & );
	 void operator= ( const nocopy & );
      };

   } // nocopy_impl

   using namespace nocopy_impl;

   template< typename T > class freer : private nocopy< freer< T > >
   {
   public:
      freer( T * const p )
	    : m_p( p )
      { }
      
      ~freer()
      {
	 ::free( const_cast< void * >( reinterpret_cast< const volatile void * >( m_p ) ) );
      }

      T * get() const
      {
	 return m_p;
      }

   private:
      T * const m_p;
   };

   inline std::string demangle_impl( const char * const mangled )
   {
      if ( ! mangled ) {
	 return "(null)";
      }
      else if ( ! mangled[ 0 ] ) {
	 return "(empty)";
      }
      int status = -1;
      const freer< const char > demangled( abi::__cxa_demangle( mangled, 0, 0, & status ) );
      
      if ( ! demangled.get() ) {
	 return mangled;
      }
      else if ( 0 == status ) {
	 return std::string( demangled.get() );
      }
      return mangled;
   }

   template< typename T > std::string demangle()
   {
      return demangle_impl( typeid( T ).name() );
   }

   std::string nomespace( const std::string & d )
   {
      const std::string::size_type n = d.rfind( "::" );

      if ( n == std::string::npos ) {
	 return d;
      }
      return d.substr( n + 2 );
   }

   class file_reader : private nocopy< file_reader >
   {
   public:
      explicit
      file_reader( const std::string & filename )
	    : m_fd( ::open( filename.c_str(), O_RDONLY ) ),
	      m_fn( filename )
      {
	 if ( m_fd < 0 ) {
	    PEGTL_THROW( "pegtl: unable to open() file " << m_fn << " for reading errno " << errno );
	 }
      }

      ~file_reader()
      {
	 ::close( m_fd );
      }

      size_t size() const
      {
	 struct stat st;

	 errno = 0;
	 if ( ::fstat( m_fd, & st ) < 0 ) {
	    PEGTL_THROW( "pegtl: unable to fstat() file " << m_fn << " descriptor " << m_fd << " errno " << errno );
	 }
	 return size_t( st.st_size );
      }

      template< typename Container >
      std::string read()
      {
	 Container nrv;
	 nrv.resize( size() );

	 errno = 0;
	 if ( nrv.size() && ( ::read( m_fd, & nrv[ 0 ], nrv.size() ) != int( nrv.size() ) ) ) {
	    PEGTL_THROW( "pegtl: unable to read() file " << m_fn << " descriptor " << m_fd << " errno " << errno );
	 }
	 //	 PEGTL_PRINT( "pegtl: read " << st.st_size << " bytes from file " << m_fn );
	 return nrv;
      }

   public:
      int internal_fd() const
      {
	 return m_fd;
      }

   private:
      const int m_fd;
      const std::string m_fn;
   };

   class file_mapper : private nocopy< file_mapper >
   {
   public:
      explicit
      file_mapper( const std::string & filename )
	    : m_data( 0 ),
	      m_size( 0 )
      {
	 const file_reader tmp( filename );

	 if ( ! ( m_size = tmp.size() ) ) {
	    return;
	 }
	 errno = 0;

	 if ( intptr_t( m_data = static_cast< const char * >( ::mmap( 0, m_size, PROT_READ, MAP_FILE | MAP_PRIVATE, tmp.internal_fd(), 0 ) ) ) == -1 ) {
	    PEGTL_THROW( "pegtl: unable to mmap() file " << filename << " errno " << errno );
	 }
      }

      ~file_mapper()
      {
	 ::munmap( const_cast< char * >( m_data ), m_size );
      }
      
      size_t size() const
      {
	 return m_size;
      }

      const char * data() const
      {
	 return m_data;
      }

      typedef const char * iterator;

      iterator begin() const
      {
	 return m_data;
      }

      iterator end() const
      {
	 return m_data + m_size;
      }

   private:
      const char * m_data;
      size_t m_size;
   };

   inline std::string read_string( const std::string & filename )
   {
      return file_reader( filename ).read< std::string >();
   }

   template< typename Type >
   inline std::vector< Type > read_vector( const std::string & filename )
   {
      return file_reader( filename ).read< std::vector< Type > >();
   }

   template< typename T >
   std::string to_string( const T & t )
   {
      std::ostringstream oss;
      oss << t;
      return oss.str();
   }

   template< class S > S string_to_signed( const std::string & value )
   {
      S nrv = 0;
      S tmp;
         
      if ( value.empty() ) {
	 PEGTL_THROW( "string-to-integer conversion failed for empty string" );
      }
      unsigned run = 0;
         
      bool neg = value[ run ] == '-';
         
      if ( value[ run ] == '+' || value[ run ] == '-' ) {
	 if ( value.size() < 2 ) {
	    PEGTL_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- sign without number" );
	 }
	 else {
	    ++run;
	 }
      }
      if ( ! isdigit( value[ run ] ) ) {
	    PEGTL_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- no digits" );
      }
      for ( ; ( run < value.size() ) && ::isdigit( value[ run ] ); ++run )
      {
	 if ( ( tmp = nrv * 10 ) / 10 != nrv ) {
	    PEGTL_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- integer overflow" );
	 }
	 if ( neg ) {
	    if ( ( nrv = tmp - ( value[ run ] - '0' ) ) + ( value[ run ] - '0' ) != tmp ) {
	       PEGTL_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- integer overflow" );
	    }
	 }
	 if ( ! neg ) {
	    if ( ( nrv = tmp + ( value[ run ] - '0' ) ) - ( value[ run ] - '0' ) != tmp ) {
	       PEGTL_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- integer overflow" );
	    }
	 }
      }
      for ( ; run < value.size(); ++run ) {
	 if ( ! ::isspace( value[ run ] ) ) {
	    PEGTL_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- trailing garbage" );
	 }
      }         
      return nrv;             
   }

} // pegtl

#endif
