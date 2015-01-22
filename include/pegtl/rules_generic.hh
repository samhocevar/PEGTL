// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_RULES_GENERIC_HH
#define COHI_PEGTL_RULES_GENERIC_HH


namespace pegtl
{
   template< bool B >
   struct bool_rule
   {
      typedef bool_rule key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 const std::string n = B ? "T" : "_|_";
	 st.template update< bool_rule >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input &, Debug &, States && ... )
      {
	 return B;
      }
   };

   struct success
	 : bool_rule< true > {};

   struct failure
	 : bool_rule< false > {};

   template< typename Rule >
   struct opt
   {
      typedef opt key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template expr< Rule >() + "?";
	 st.template update< opt >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return in.eof() || de.template match< false, Rule >( in, std::forward< States >( st ) ... ) || true;
      }
   };

   template< typename Rule, unsigned N, unsigned M = N >
   struct rep
   {
      typedef rep key_type;

      static std::string s_print()
      {
	 if ( M == N ) {
	    return to_string( N );
	 }
	 else {
	    return to_string( N ) + ',' + to_string( M );
	 }	 
      }

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template expr< Rule >() + "{" + s_print() + "}";
	 st.template update< rep >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 static_assert( N <= M, "pegtl: illegal expression rep< R, N, M > where N is greater than M" );

	 typename Input::template marker< Must > p( in );

	 for ( long long i = 0; i < N; ++i ) {
	    if ( ! de.template match< Must, Rule >( in, std::forward< States >( st ) ... ) ) {
	       return p( false );
	    }
	 }
	 for ( long long i = N; i < M; ++i ) {
	    if ( ! de.template match< Must, Rule >( in, std::forward< States >( st ) ... ) ) {
	       return p( true );
	    }
	 }
	 return p( true );
      }
   };

   template< typename Rule >
   struct star
   {
      typedef star key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template name< Rule >() + "*";
	 st.template update< star >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 while ( ( ! in.eof() ) && de.template match< false, Rule >( in, std::forward< States >( st ) ... ) ) {}
	 return true;
      }
   };

   template< typename Rule >
   struct plus
   {
      typedef plus key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template name< Rule >() + "+";
	 st.template update< plus >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return de.template match< Must, Rule >( in, std::forward< States >( st ) ... ) && star< Rule >::template match< Must >( in, de, std::forward< States >( st ) ... );
      }
   };

   template< typename... > struct sor;

   template<>
   struct sor<>
	 : failure {};

   template< typename Rule, typename... Rules >
   struct sor< Rule, Rules ... >
   {
      typedef sor key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule, Rules ... >();
	 const std::string n = names< Rule, Rules ... >( st, "( ", " / ", " )" );
	 st.template update< sor >( n, ! st.template name< sor >().compare( 0, 11, "pegtl::sor<" ) );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return de.template match< Must && ! sizeof ... ( Rules ), Rule >( in, std::forward< States >( st ) ... ) || pegtl::sor< Rules ... >::template match< Must >( in, de, std::forward< States >( st ) ... );
      }
   };

   template< typename ... > struct seq;

   template<>
   struct seq<>
	 : success
   {
      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool i_match( Input &, Debug &, States && ... )
      {
	 return true;
      }
   };

   template< typename Rule, typename... Rules >
   struct seq< Rule, Rules ... >
   {
      typedef seq key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule, Rules ... >();
	 const std::string n = names< Rule, Rules ... >( st, "( ", " ", " )" );
	 st.template update< seq >( n, ! st.template name< seq >().compare( 0, 11, "pegtl::seq<" ) );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > h( in );
	 return h( i_match< Must >( in, de, std::forward< States >( st ) ... ) );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool i_match( Input & in, Debug & de, States && ... st )
      {
	 return de.template match< Must, Rule >( in, std::forward< States >( st ) ... ) && pegtl::seq< Rules ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... );
      }
   };

   template< typename Rule >
   struct must
   {
      typedef Rule key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "!" + st.template name< Rule >();
	 st.template update< must >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return de.template match< true, Rule >( in, std::forward< States >( st ) ... );
      }
   };

   template< typename Rule >
   struct at
   {
      typedef at key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "&" + st.template name< Rule >();
	 st.template update< at >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< false > p( in );
	 return de.template match< Must, Rule >( in, std::forward< States >( st ) ... );
      }
   };

   template< typename Rule >
   struct not_at
   {
      typedef not_at key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "!" + st.template name< Rule >();
	 st.template update< not_at >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< false > p( in );
	 return ! de.template match< Must, Rule >( in, std::forward< States >( st ) ... );
      }
   };

   struct eof
   {
      typedef eof key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template update< eof >( "&eof", true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug &, States && ... )
      {
	 return in.eof();
      }
   };

   template< typename Cond >
   struct until1
   {
      typedef until1 key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Cond >();
	 const std::string n = st.template name< Cond >() + "@";
	 st.template update< until1 >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > p( in );

	 while ( ! de.template match< false, Cond >( in, std::forward< States >( st ) ... ) ) {
	    if ( in.eof() ) {
	       return p( false );
	    }
	    in.bump();
	 }
	 return p( true );
      }
   };

   template< typename What, typename Cond >
   struct until
   {
      typedef until key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< What, Cond >();
	 const std::string n = "( " + st.template name< What >() + " % " + st.template name< Cond >() + " )";
	 st.template update< until >( n, true );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > p( in );

	 while ( ! de.template match< false, Cond >( in, std::forward< States >( st ) ... ) ) {
	    if ( in.eof() || ( ! de.template match< Must, What >( in, std::forward< States >( st ) ... ) ) ) {
	       return p( false );
	    }
	 }
	 return p( true );
      }
   };

   template< bool Must, typename Cond, typename ... Thens >
   struct cond2impl
   {
      typedef cond2impl key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Cond, Thens ... >();
	 const std::string n = "( " + st.template name< Cond >() + ( Must ? " ->> " : " --> " ) + names< Thens ... >( st, " ", " ", " " )() + " )";
	 st.template update< cond2impl >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > p( in );

	 if ( de.template match< false, Cond >( in, std::forward< States >( st ) ... ) ) {
	    return p( seq< Thens ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... ) );
	 } else {
	    return p( ! Must );
	 }
      }
   };

   template< typename Cond, typename ... Thens >
   struct ifmust
	 : cond2impl< true, Cond, Thens ... > {};

   template< typename Cond, typename ... Thens >
   struct ifthen
	 : cond2impl< false, Cond, Thens ... > {};

   template< bool Must, typename Cond, typename Then, typename Else >
   struct cond3impl
   {
      typedef cond3impl key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< Cond, Then, Else >();
	 const std::string n = "( " + st.template name< Cond >() + ( Must ? " ->> " : " --> " ) + st.template name< Then >() + " / " + st.template name< Else >() + " )";
	 st.template update< cond3impl >( n, true );
      }

      template< bool, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > p( in );

	 if ( de.template match< false, Cond >( in, std::forward< States >( st ) ... ) ) {
	    return p( de.template match< Must, Then >( in, std::forward< States >( st ) ... ) );
	 }
	 else {
	    return p( de.template match< Must, Else >( in, std::forward< States >( st ) ... ) );
	 }
      }
   };

   template< typename Cond, typename Then, typename Else >
   struct ifmustelse
	 : cond3impl< true, Cond, Then, Else >
   {
      //      typedef sor< seq< Cond, must< Then > >, seq< not_at< Cond >, must< Else > > > key_type;
   };

   template< typename Cond, typename Then, typename Else >
   struct ifthenelse
	 : cond3impl< false, Cond, Then, Else >
   {
      //      typedef sor< seq< Cond, Then >, seq< not_at< Cond >, Else > > key_type;
   };

   template< typename What, typename RulePadL, typename RulePadR = RulePadL >
   struct pad
	 : seq< star< RulePadL >, What, star< RulePadR > >
   {
      template< typename Print >
      static void prepare( Print & st )
      {
	 st.template insert< seq< star< RulePadL >, What, star< RulePadR > > >( true );
	 const std::string n = st.template name< What >();
	 const std::string e = st.template expr< What >();
	 st.template update< pad >( n, e );
      }
   };
   
} // pegtl

#endif
