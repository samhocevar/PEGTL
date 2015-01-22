// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_INPUT_STRING_HH
#define COHI_PEGTL_INPUT_STRING_HH


namespace pegtl
{
   class ascii_location
   {
   public:
      ascii_location()
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

   inline std::ostream & operator<< ( std::ostream & o, const ascii_location & w )
   {
      w.write_to( o );
      return o;
   }

} // pegtl

#endif
