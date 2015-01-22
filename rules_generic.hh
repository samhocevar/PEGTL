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
      static void s_insert( Print & st )
      {
	 const std::string n = B ? "T" : "_|_";
	 st.template update< bool_rule >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input &, Debug &, Class && ... )
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
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template expr< Rule >() + "?";
	 st.template update< opt >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return in.eof() || de.template match< Rule >( false, in, std::forward< Class >( cl ) ... ) || true;
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
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template expr< Rule >() + "{" + s_print() + "}";
	 st.template update< rep >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 static_assert( N <= M, "pegtl: illegal expression rep< R, N, M > where N is greater than M" );

	 marker< Input > p( in );

	 for ( long long i = 0; i < N; ++i ) {
	    if ( ! de.template match< Rule >( in, std::forward< Class >( cl ) ... ) ) {
	       return p( false );
	    }
	 }
	 for ( long long i = N; i < M; ++i ) {
	    if ( ! de.template match< Rule >( in, std::forward< Class >( cl ) ... ) ) {
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
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template name< Rule >() + "*";
	 st.template update< star >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 while ( ( ! in.eof() ) && de.template match< Rule >( false, in, std::forward< Class >( cl ) ... ) ) {}
	 return true;
      }
   };

   template< typename Rule >
   struct plus
   {
      typedef plus key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template name< Rule >() + "+";
	 st.template update< plus >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return de.template match< Rule >( in, std::forward< Class >( cl ) ... ) && star< Rule >::template s_match( in, de, std::forward< Class >( cl ) ... );
      }
   };

   template< typename... > struct sor;

   template<>
   struct sor<>
	 : failure {};

   template< typename Rule, typename... Rules >
   struct sor< Rule, Rules... >
   {
      typedef sor key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule, Rules... >();
	 const std::string n = names< Rule, Rules... >( st, "( ", " / ", " )" );
	 st.template update< sor >( n, ! st.template name< sor >().compare( 0, 11, "pegtl::sor<" ) );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return de.template match< Rule >( de.must() & ! sizeof... ( Rules ), in, std::forward< Class >( cl ) ... ) || pegtl::sor< Rules... >::template s_match( in, de, std::forward< Class >( cl ) ... );
      }
   };

   template< typename ... > struct seq;

   template<>
   struct seq<>
	 : success {};

   template< typename Rule, typename... Rules >
   struct seq< Rule, Rules... >
   {
      typedef seq key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule, Rules... >();
	 const std::string n = names< Rule, Rules... >( st, "( ", " ", " )" );
	 st.template update< seq >( n, ! st.template name< seq >().compare( 0, 11, "pegtl::seq<" ) );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > h( in );
	 return h( de.template match< Rule >( in, std::forward< Class >( cl ) ... ) && pegtl::seq< Rules... >::template s_match( in, de, std::forward< Class >( cl ) ... ) );
      }
   };

   template< typename Rule >
   struct must
   {
      typedef Rule key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "!" + st.template name< Rule >();
	 st.template update< must >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return de.template match< Rule >( true, in, std::forward< Class >( cl ) ... );
      }
   };

   template< typename Rule >
   struct at
   {
      typedef at key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "&" + st.template name< Rule >();
	 st.template update< at >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 const marker< Input > p( in );
	 return de.template match< Rule >( in, std::forward< Class >( cl ) ... );
      }
   };

   template< typename Rule >
   struct not_at
   {
      typedef not_at key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "!" + st.template name< Rule >();
	 st.template update< not_at >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 const marker< Input > p( in );
	 return ! de.template match< Rule >( in, std::forward< Class >( cl ) ... );
      }
   };

   struct eof
   {
      typedef eof key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template update< eof >( "&eof", true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 return in.eof();
      }
   };

   template< typename Cond >
   struct until1
   {
      typedef until1 key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Cond >();
	 const std::string n = st.template name< Cond >() + "@";
	 st.template update< until1 >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > p( in );

	 while ( ! de.template match< Cond >( false, in, std::forward< Class >( cl ) ... ) ) {
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
      static void s_insert( Print & st )
      {
	 st.template insert< What, Cond >();
	 const std::string n = "( " + st.template name< What >() + " % " + st.template name< Cond >() + " )";
	 st.template update< until >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > p( in );

	 while ( ! de.template match< Cond >( false, in, std::forward< Class >( cl ) ... ) ) {
	    if ( in.eof() || ( ! de.template match< What >( in, std::forward< Class >( cl ) ... ) ) ) {
	       return p( false );
	    }
	 }
	 return p( true );
      }
   };

   template< bool Must, typename Cond, typename Then >
   struct cond2impl
   {
      typedef cond2impl key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Cond, Then >();
	 const std::string n = "( " + st.template name< Cond >() + ( Must ? " ->> " : " --> " ) + st.template name< Then >() + " )";
	 st.template update< cond2impl >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > p( in );

	 if ( de.template match< Cond >( false, in, std::forward< Class >( cl ) ... ) ) {
	    return p( de.template match< Then >( Must, in, std::forward< Class >( cl ) ... ) );
	 } else {
	    return p( ! Must );
	 }
      }
   };

   template< typename Cond, typename Then >
   struct ifmust
	 : cond2impl< true, Cond, Then >
   {
      //      typedef seq< Cond, must< Then > > key_type;
   };

   template< typename Cond, typename Then >
   struct ifthen
	 : cond2impl< false, Cond, Then >
   {
      //      typedef sor< seq< Cond, Then >, not_at< Cond > > key_type;
   };

   template< bool Must, typename Cond, typename Then, typename Else >
   struct cond3impl
   {
      typedef cond3impl key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Cond, Then, Else >();
	 const std::string n = "( " + st.template name< Cond >() + ( Must ? " ->> " : " --> " ) + st.template name< Then >() + " / " + st.template name< Else >() + " )";
	 st.template update< cond3impl >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > p( in );

	 if ( de.template match< Cond >( false, in, std::forward< Class >( cl ) ... ) ) {
	    return p( de.template match< Then >( Must, in, std::forward< Class >( cl ) ... ) );
	 }
	 else {
	    return p( de.template match< Else >( Must, in, std::forward< Class >( cl ) ... ) );
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
      template< typename ... Dones >
      struct min
      {
	 static const size_t value = What::template min< pad, Dones ... >::value;
      };

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< seq< star< RulePadL >, What, star< RulePadR > > >( true );
	 const std::string n = st.template name< What >();
	 const std::string e = st.template expr< What >();
	 st.template update< pad >( n, e );
      }
   };

   template< typename Rule, typename Func >
   struct action
   {
      template< typename ... Dones >
      struct min
      {
	 static const size_t value = Rule::template min< action, Dones ... >::value;
      };

      typedef typename Rule::key_type key_type;

      template< typename Print >
      static void s_insert( Print & st )
      {
	 st.template insert< Rule >( true );
	 const std::string e = st.template expr< Rule >();
	 st.template update< action >( e, false );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 marker< Input > p( in );

	 if ( Rule::template s_match( in, de, std::forward< Class >( cl ) ... ) ) {
	    Func::template matched< Rule >( std::string( p.here(), in.here() ), std::forward< Class >( cl ) ... );
	    return p( true );
	 }
	 return p( false );
      }
   };
   
} // pegtl

#endif
