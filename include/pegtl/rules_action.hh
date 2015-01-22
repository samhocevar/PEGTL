// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_RULES_ACTION_HH
#define COHI_PEGTL_RULES_ACTION_HH


namespace pegtl
{
   template< typename ... Funcs > struct apply_impl;

   template<>
   struct apply_impl<>
   {
      template< typename ... States >
      static void apply( const std::string &, States && ... )
      { }
   };

   template< typename Func, typename ... Funcs >
   struct apply_impl< Func, Funcs ... >
   {
      template< typename ... States >
      static void apply( const std::string & s, States && ... st )
      {
	 Func::apply( s, std::forward< States >( st ) ... );
	 apply_impl< Funcs ... >::apply( s, std::forward< States >( st ) ... );
      }
   };
   
   template< typename Rule, typename ... Funcs >
   struct action
   {
      typedef typename Rule::key_type key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >( true );
	 const std::string e = st.template expr< Rule >();
	 st.template update< action >( e, false );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< false > p( in );

	 if ( Rule::template match< Must >( in, de, std::forward< States >( st ) ... ) ) {
	    apply_impl< Funcs ... >::apply( std::string( p.here(), in.here() ), std::forward< States >( st ) ... );
	    return p( true );
	 }
	 return p( false );
      }
   };
   
   template< unsigned N, typename ... Funcs >
   struct apply_nth
   {
      template< typename State, typename ... States >
      static void apply( const std::string & s, State &&, States && ... st )
      {
	 apply_nth< N - 1, Funcs ... >::template apply< States ... >( s, std::forward< States >( st ) ... );
      }
   };

   template< typename ... Funcs >
   struct apply_nth< 0, Funcs ... >
   {
      template< typename State, typename ... States >
      static void apply( const std::string & s, State && st, States && ... )
      {
	 apply_impl< Funcs ... >::apply( s, std::forward< State >( st ) );
      }
   };

   template< unsigned N, typename Rule, typename ... Funcs >
   struct action_nth
   	 : action< Rule, apply_nth< N, Funcs ... > > {};

   // Some simple action classes.

   struct apply_insert
   {
      template< typename Container >
      static void apply( const std::string & s, Container & c )
      {
	 c.insert( c.end(), s );
      }      
   };

   struct apply_push_back
   {
      template< typename Container >
      static void apply( const std::string & s, Container & c )
      {
	 c.push_back( s );
      }
   };
   
   // Class capture is both an action and a rule.
   
   // As action, it stores the matched string as value in the
   // 'capture_map &' state, using the template argument as key.

   // As rule, it retrieves a string from the 'capture_map' state,
   // again using the template argument as key, and attempts to
   // match the input against that string.

   template< unsigned Key >
   struct capture
   {
      typedef capture key_type;
      
      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template update< capture >( '\\' + to_string( Key ), true );
      }

      template< typename Map >
      static void apply( const std::string & s, Map & m )
      {
	 m[ Key ] = s;
      }

      template< bool Must, typename Input, typename Debug, typename Map >
      static bool match( Input & in, Debug &, const Map & m )
      {
	 typename Input::template marker< Must > p( in );
	 
	 const typename Map::const_iterator i = m.find( Key );

	 if ( i == m.end() ) {
	    return p( false );  // Or assume empty string?
	 }
	 const std::string & s = i->second;

	 for ( size_t j = 0; j < s.size(); ++j ) {
	    character< Input > c( in );
	    if ( ! c( c == s[ j ] ) ) {
	       return p( false );
	    }
	 }
	 return p( true );
      }
   };

   // Typedef for a map that can be used together with class capture.

   typedef std::map< unsigned, std::string > capture_map;

} // pegtl

#endif
