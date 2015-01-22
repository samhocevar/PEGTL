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
	 : one_range< '0', '9' >
   {
      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< digit >( "digit", true );
      }
   };


   struct lower
	 : one_range< 'a', 'z' >
   {
      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< lower >( "lower", true );
      }      
   };


   struct upper
	 : one_range< 'A', 'Z' >
   {
      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< upper >( "upper", true );
      }      
   };


   struct alpha
	 : sor< lower, upper >
   {
      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< alpha >( "alpha", true );
      }
   };


   struct ident1
	 : sor< one< '_' >, alpha > {};


   struct ident2
	 : sor< digit, ident1 > {};


   struct ident
	 : seq< ident1, star< ident2 > > {};


   struct eol
	 : one< '\n' > {};


   struct eol_or_eof
	 : sor< eof, eol > {};


   struct space
	 : one_list< ' ', '\t' >
   {
      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< space >( "space", true );
      }
   };


   struct space_star
	 : star< space > {};


   struct space_plus
	 : plus< space > {};


   struct white
	 : one_list< ' ', '\n', '\r', '\t', '\v' >
   {
      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< white >( "white", true );
      }
   };


   struct white_star
	 : star< white > {};

   struct white_plus
	 : plus< white > {};

   struct until_eol
	 : until1< eol > {};

   struct until_eol_or_eof
	 : until1< eol_or_eof > {};

   struct white_until_eof
	 : until< white, eof > {};

   struct space_until_eol_or_eof
	 : until< space, eol_or_eof > {};

   struct shebang
	 : ifmust< string< '#', '!' >, until_eol_or_eof > {};

} // pegtl

#endif
