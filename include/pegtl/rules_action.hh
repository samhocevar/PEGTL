// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_RULES_ACTION_HH
#define COHI_PEGTL_RULES_ACTION_HH


namespace pegtl
{
   template< typename Rule, typename Func >
   struct action_all
   {
      typedef typename Rule::key_type key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >( true );
	 const std::string e = st.template expr< Rule >();
	 st.template update< action_all >( e, false );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool s_match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< false > p( in );  // Don't need full power of marker< false, ... > here.

	 if ( Rule::template s_match< Must >( in, de, std::forward< States >( st ) ... ) ) {
	    Func::matched( std::string( p.here(), in.here() ), std::forward< States >( st ) ... );
	    return p( true );
	 }
	 return p( false );
      }
   };
   
   template< unsigned N, typename Func, typename ... States > struct action_nth_impl;

   template< typename Func, typename State, typename ... States >
   struct action_nth_impl< 0, Func, State, States ... >
   {
      static void apply( const std::string & s, State && st, States && ... )
      {
	 Func::matched( s, std::forward< State >( st ) );
      }
   };

   template< unsigned N, typename Func, typename State, typename ... States >
   struct action_nth_impl< N, Func, State, States ... >
   {
      static void apply( const std::string & s, State &&, States && ... st )
      {
	 action_nth_impl< N - 1, Func, States ... >::apply( s, std::forward< States >( st ) ... );
      }
   };

   template< unsigned N, typename Rule, typename Func >
   struct action_nth
   {
      typedef typename Rule::key_type key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >( true );
	 const std::string e = st.template expr< Rule >();
	 st.template update< action_nth >( e, false );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool s_match( Input & in, Debug & de, States && ... st )
      {
	 static_assert( sizeof... st >= N, "pegtl: too few state arguments in action_nth rule" );

	 typename Input::template marker< false > p( in );  // Don't need full power of marker< false, ... > here.

	 if ( Rule::template s_match< Must >( in, de, std::forward< States >( st ) ... ) ) {
	    action_nth_impl< N, Func, States ... >::apply( std::string( p.here(), in.here() ), std::forward< States >( st ) ... );
	    return p( true );
	 }
	 return p( false );
      }
   };
   
} // pegtl

#endif
