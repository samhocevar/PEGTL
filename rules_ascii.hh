// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_ASCII_HH
#define COHI_PEGTL_ASCII_HH

namespace pegtl
{
   struct digit
	 : range< '0', '9' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< digit >( "digit", true );
      }
   };

   struct lower
	 : range< 'a', 'z' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< lower >( "lower", true );
      }      
   };

   struct upper
	 : range< 'A', 'Z' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< upper >( "upper", true );
      }      
   };

   struct alpha
	 : sor< lower, upper >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< alpha >( "alpha", true );
      }
   };

   struct alnum
	 : sor< alpha, digit >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< alpha >( "alnum", true );
      }
   };

   struct xdigit
	 : sor< digit, range< 'a', 'f' >, range< 'A', 'F' > >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< alpha >( "xdigit", true );
      }
   };

   struct ident1
	 : sor< one< '_' >, alpha > {};

   struct ident2
	 : sor< digit, ident1 > {};

   struct identifier
	 : seq< ident1, star< ident2 > > {};

   struct lf
	 : one< '\n' > {};

   struct cr
	 : one< '\r' > {};

   struct crlf
	 : seq< cr, lf > {};

   struct eol
	 : sor< eof, crlf, lf, cr > {};

   struct blank
	 : list< ' ', '\t' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< blank >( "blank", true );
      }
   };

   struct space_star
	 : star< blank > {};

   struct space_plus
	 : plus< blank > {};

   struct space
	 : list< ' ', '\n', '\r', '\t', '\v', '\f' >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< space >( "space", true );
      }
   };

   struct white_star
	 : star< space > {};

   struct white_plus
	 : plus< space > {};

   struct until_eol
	 : until1< eol > {};

   struct white_until_eof
	 : until< space, eof > {};

   struct space_until_eol
	 : until< blank, eol > {};

   struct shebang
	 : ifmust< string< '#', '!' >, until_eol > {};

} // pegtl

#endif
