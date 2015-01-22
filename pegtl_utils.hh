// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_UTILS_HH
#define COHI_PEGTL_UTILS_HH


namespace pegtl
{
   template< typename RuleWhat, typename RulePadL, typename RulePadR = RulePadL >
   struct pad
	 : seq< star< RulePadL >, RuleWhat, star< RulePadR > >
   {
      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< seq< star< RulePadL >, RuleWhat, star< RulePadR > > >( true );
	 const std::string n = st.template name< RuleWhat >();
	 const std::string e = st.template expr< RuleWhat >();
	 st.template update< pad >( n, e );
      }
   };


   template< int Char, typename RulePadL, typename RulePadR = RulePadL >
   struct pad_one
	 : pad< one< Char >, RulePadL, RulePadR > {};


   template< typename Rule, typename Func >
   struct action
   {
      static std::string key()
      {
	 return typeid( Rule ).name();
      }

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >( true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );
	 if ( Rule::template s_match( in, de, std::forward< Class >( cl ) ... ) ) {
	    Func::template matched< Rule >( std::string( p.here(), in.here() ), std::forward< Class >( cl ) ... );
	    return p( true );
	 }
	 return p( false );
      }
   };

} // pegtl

#endif
