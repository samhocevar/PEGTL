// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_UTILS_HH
#define COHI_UTILS_HH

#include <cassert>
#include <sstream>
#include <fstream>
#include <cxxabi.h>
#include <iterator>
#include <stdexcept>
#include <algorithm>


namespace utils
{
#define UTILS_PRINT( MeSSaGe )				\
   do { std::cerr << MeSSaGe << '\n'; } while( 0 )

#define UTILS_TRACE							\
   UTILS_PRINT( __FILE__ << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ )
   
#define UTILS_DEBUG( MeSSaGe )						\
   UTILS_PRINT( __FILE__ << " : " << __LINE__ << " : " << MeSSaGe )

#define UTILS_THROW( MeSSaGe )						\
   do { std::ostringstream oss; oss << MeSSaGe; throw std::runtime_error( oss.str() ); } while( 1 )


   inline std::string read_file( const std::string & filename )
   {
      std::ifstream stream( filename.c_str(), std::ifstream::in );
      return std::string( std::istream_iterator< std::string::value_type >( stream ),
			  std::istream_iterator< std::string::value_type >() );
   }


   inline void escape_impl( std::string & result, const int i )
   {
      switch ( i )
      {
	 case '"':
	    result += "\\\"";
	    break;
	 case '\\':
	    result += "\\";
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
	    ::snprintf( tmp, sizeof( tmp ), "\\%03d", i );
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


   template< class S > S string_to_signed( const std::string & value )
   {
      S nrv = 0;
      S tmp;
         
      if ( value.empty() ) {
	 UTILS_THROW( "string-to-integer conversion failed for empty string" );
      }
      unsigned run = 0;
         
      bool neg = value[ run ] == '-';
         
      if ( value[ run ] == '+' || value[ run ] == '-' ) {
	 if ( value.size() < 2 ) {
	    UTILS_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- sign without number" );
	 }
	 else {
	    ++run;
	 }
      }
      if ( ! isdigit( value[ run ] ) ) {
	    UTILS_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- no digits" );
      }
      for ( ; ( run < value.size() ) && ::isdigit( value[ run ] ); ++run )
      {
	 if ( ( tmp = nrv * 10 ) / 10 != nrv ) {
	    UTILS_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- integer overflow" );
	 }
	 if ( neg ) {
	    if ( ( nrv = tmp - ( value[ run ] - '0' ) ) + ( value[ run ] - '0' ) != tmp ) {
	       UTILS_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- integer overflow" );
	    }
	 }
	 if ( ! neg ) {
	    if ( ( nrv = tmp + ( value[ run ] - '0' ) ) - ( value[ run ] - '0' ) != tmp ) {
	       UTILS_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- integer overflow" );
	    }
	 }
      }
      for ( ; run < value.size(); ++run ) {
	 if ( ! ::isspace( value[ run ] ) ) {
	    UTILS_THROW( "string-to-integer conversion failed for string \"" << value << "\" -- trailing garbage" );
	 }
      }         
      return nrv;             
   }

} // utils

#endif
