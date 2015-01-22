// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_RULES_HH
#define COHI_PEGTL_RULES_HH


namespace pegtl
{
   template< bool B >
   struct boolean
   {
      static std::string key()
      {
	 return typeid( boolean ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 const std::string n = B ? "T" : "_";
	 st.template update< boolean >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input &, Debug &, Class && ... )
      {
	 return B;
      }
   };


   struct success
	 : boolean< true > {};


   struct failure
	 : boolean< false > {};


   template< typename Rule >
   struct opt
   {
      static std::string key()
      {
	 return typeid( opt ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
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


   template< typename Rule, typename Predicate >
   struct rep 
   {
      static std::string key()
      {
	 return typeid( rep ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template name< Rule >() + p_i();
	 st.template update< rep >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 size_t i = 0;
	 position< Input > p( in );

	 while ( de.template match< Rule >( in, std::forward< Class >( cl ) ... ) ) {
	    ++i;
	 }
	 return p( Predicate::k( i ) );
      }

      std::string p_i()
      {
	 std::ostringstream o;
	 Predicate::p( o );
	 return o.str();
      }
   };


   template< typename Rule >
   struct star
   {
      static std::string key()
      {
	 return typeid( star ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
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
      static std::string key()
      {
	 return typeid( plus ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
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


   template< size_t Minimum, size_t Maximum >
   struct range_predicate
   {
      static void p( std::ostream & o )
      {
	 o << "{" << Minimum << "-" << Maximum << "}";
      }

      static bool k( const size_t i )
      {
	 return ( Minimum <= i ) && ( i <= Maximum );
      }
   };


   template< size_t Modulus, size_t Result >
   struct modulus_predicate
   {
      static void p( std::ostream & o )
      {
	 o << "{n%" << Modulus << "=" << Result << "}";
      }

      static bool k( const size_t i )
      {
	 return ( i % Modulus ) == Result;
      }
   };


   template< typename Rule >
   struct odd
	 : rep< Rule, modulus_predicate< 2, 1 > > {};

   template< typename Rule > 
   struct even
	 : rep< Rule, modulus_predicate< 2, 0 > > {};

   template< typename Rule, size_t Minimum, size_t Maximum >
   struct range
	 : rep< Rule, range_predicate< Minimum, Maximum > > {};


   template< typename... > struct sor;


   template<>
   struct sor<>
	 : failure {};


   template< typename Rule, typename... Rules >
   struct sor< Rule, Rules... >
   {
      static std::string key()
      {
	 return typeid( sor ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
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


   template< typename... > struct seq;


   template<>
   struct seq<>
	 : success {};


   template< typename Rule, typename... Rules >
   struct seq< Rule, Rules... >
   {
      static std::string key()
      {
	 return typeid( seq ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule, Rules... >();
	 const std::string n = names< Rule, Rules... >( st, "( ", " ", " )" );
	 st.template update< seq >( n, ! st.template name< seq >().compare( 0, 11, "pegtl::seq<" ) );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > h( in );
	 return h( de.template match< Rule >( in, std::forward< Class >( cl ) ... ) && pegtl::seq< Rules... >::template s_match( in, de, std::forward< Class >( cl ) ... ) );
      }
   };


   template< typename Rule >
   struct at
   {
      static std::string key()
      {
	 return typeid( at ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "&" + st.template name< Rule >();
	 st.template update< at >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );
	 return de.template match< Rule >( in, std::forward< Class >( cl ) ... );
      }
   };


   template< typename Rule >
   struct not_at
   {
      static std::string key()
      {
	 return typeid( not_at ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = "!" + st.template name< Rule >();
	 st.template update< not_at >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );
	 return ! de.template match< Rule >( in, std::forward< Class >( cl ) ... );
      }
   };


   struct eof
   {
      static std::string key()
      {
	 return typeid( eof ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template update< eof >( "&eof", true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug &, Class && ... )
      {
	 return in.eof();
      }
   };


   template< typename Rule >
   struct must
   {
      static std::string key()
      {
	 return typeid( must ).name();
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 return de.template match< Rule >( true, in, std::forward< Class >( cl ) ... );
      }
   };

   
   template< typename... Rules >
   struct seq_must
	 : seq< must< Rules >... > {};


   template< typename Rule >
   struct until1
   {
      static std::string key()
      {
	 return typeid( until1 ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< Rule >();
	 const std::string n = st.template name< Rule >() + "@";
	 st.template update< until1 >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );

	 while ( ! de.template match< Rule >( false, in, std::forward< Class >( cl ) ... ) ) {
	    if ( in.eof() ) {
	       return p( false );
	    }
	    in.bump();
	 }
	 return p( true );
      }
   };


   template< typename RuleWhat, typename RuleCond >
   struct until
   {
      static std::string key()
      {
	 return typeid( until ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< RuleWhat, RuleCond >();
	 const std::string n = "( " + st.template name< RuleWhat >() + " % " + st.template name< RuleCond >() + " )";
	 st.template update< until >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );

	 while ( ! de.template match< RuleCond >( false, in, std::forward< Class >( cl ) ... ) ) {
	    if ( in.eof() || ( ! de.template match< RuleWhat >( in, std::forward< Class >( cl ) ... ) ) ) {
	       return p( false );
	    }
	 }
	 return p( true );
      }
   };


   template< bool Must, typename RuleIf, typename RuleThen >
   struct cond2impl
   {
      static std::string key()
      {
	 return typeid( cond2impl ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< RuleIf, RuleThen >();
	 const std::string n = "( " + st.template name< RuleIf >() + ( Must ? " ->> " : " --> " ) + st.template name< RuleThen >() + " )";
	 st.template update< cond2impl >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );

	 if ( de.template match< RuleIf >( false, in, std::forward< Class >( cl ) ... ) ) {
	    return p( de.template match< RuleThen >( Must, in, std::forward< Class >( cl ) ... ) );
	 } else {
	    return ! Must;
	 }
      }
   };


   template< typename RuleIf, typename RuleThen >
   struct ifmust
	 : cond2impl< true, RuleIf, RuleThen > {};


   template< typename RuleIf, typename RuleThen >
   struct ifthen
	 : cond2impl< false, RuleIf, RuleThen > {};


   template< bool Must, typename RuleIf, typename RuleThen, typename RuleElse >
   struct cond3impl
   {
      static std::string key()
      {
	 return typeid( cond3impl ).name();
      }

      template< typename Print >
      static void s_print( Print & st )
      {
	 st.template insert< RuleIf, RuleThen, RuleElse >();
	 const std::string n = "( " + st.template name< RuleIf >() + ( Must ? " ->> " : " --> " ) + st.template name< RuleThen >() + " / " + st.template name< RuleElse >() + " )";
	 st.template update< cond3impl >( n, true );
      }

      template< typename Input, typename Debug, typename ... Class >
      static bool s_match( Input & in, Debug & de, Class && ... cl )
      {
	 position< Input > p( in );

	 if ( de.template match< RuleIf >( false, in, std::forward< Class >( cl ) ... ) ) {
	    return p( de.template match< RuleThen >( Must, in, std::forward< Class >( cl ) ... ) );
	 }
	 else {
	    return p( de.template match< RuleElse >( Must, in, std::forward< Class >( cl ) ... ) );
	 }
      }
   };


   template< typename RuleIf, typename RuleThen, typename RuleElse >
   struct ifmustelse
	 : cond3impl< true, RuleIf, RuleThen, RuleElse > {};


   template< typename RuleIf, typename RuleThen, typename RuleElse >
   struct ifthenelse
	 : cond3impl< false, RuleIf, RuleThen, RuleElse > {};
   
} // pegtl

#endif
