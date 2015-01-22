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

   template< typename Rule, typename ... Rules >
   struct seq< Rule, Rules ... >
   {
      typedef seq key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< seq, Rule, Rules ... >( st, "", "( ", " ", " )", "" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must || ! sizeof ... ( Rules ) > h( in );
	 return h( i_match< Must >( in, de, std::forward< States >( st ) ... ) );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool i_match( Input & in, Debug & de, States && ... st )
      {
	 return de.template match< Must, Rule >( in, std::forward< States >( st ) ... ) && pegtl::seq< Rules ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... );
      }
   };

   template< typename ... Rules >
   struct opt
   {
      typedef opt key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< opt, Rules ... >( st, "", "( ", " ", " )", "?" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return in.eof() || seq< Rules ... >::template match< false >( in, de, std::forward< States >( st ) ... ) || true;
      }
   };

   template< unsigned N, typename ... Rules >
   struct rep
   {
      typedef rep key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< rep, Rules ... >( st, "", "( ", " ", " )", "{" + to_string( N ) + "}" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > p( in );

	 for ( unsigned i = 0; i < N; ++i ) {
	    if ( ! seq< Rules ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... ) ) {
	       return p( false );
	    }
	 }
	 return p( true );
      }
   };

   template< typename ... Rules >
   struct star
   {
      typedef star key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< star, Rules ... >( st, "", "( ", " ", " )", "*" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 while ( ( ! in.eof() ) && seq< Rules ... >::template match< false >( in, de, std::forward< States >( st ) ... ) ) {}
	 return true;
      }
   };

   template< typename ... Rules >
   struct plus
   {
      typedef plus key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< plus, Rules ... >( st, "", "( ", " ", " )", "+" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return seq< Rules ... >::template match< Must >( in, de, std::forward< States >( st ) ... ) && star< Rules ... >::template match< Must >( in, de, std::forward< States >( st ) ... );
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
	 prepare1< sor, Rules ... >( st, "", "( ", " / ", " )", "" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return de.template match< Must && ! sizeof ... ( Rules ), Rule >( in, std::forward< States >( st ) ... ) || pegtl::sor< Rules ... >::template match< Must >( in, de, std::forward< States >( st ) ... );
      }
   };

   template< typename ... Rules >
   struct must
   {
      typedef must key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< must, Rules ... >( st, "!", "( ", " ", " )", "" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 return seq< Rules ... >::template match< true >( in, de, std::forward< States >( st ) ... );
      }
   };

   template< typename Rule >
   struct at
   {
      typedef at key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< at, Rule >( st, "&", "( ", " ", " )", "" );
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
	 prepare1< not_at, Rule >( st, "!", "( ", " ", " )", "" );
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

   template< typename Cond, typename ... Rules > struct until;
   
   template< typename Cond, typename Rule, typename ... Rules >
   struct until< Cond, Rule, Rules ... >
   {
      typedef until key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare2< until, Cond, Rule, Rules ... >( st, "( ", " % ", " ", " )" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 typename Input::template marker< Must > p( in );

	 while ( ! de.template match< false, Cond >( in, std::forward< States >( st ) ... ) ) {
	    if ( in.eof() || ( ! seq< Rule, Rules ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... ) ) ) {
	       return p( false );
	    }
	 }
	 return p( true );
      }
   };

    template< typename Cond >
    struct until< Cond >
    {
       typedef until key_type;

       template< typename Print >
       static void prepare( Print & st )
       {
	  prepare1< until, Cond >( st, "$", "", "", "", "" );
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

   template< bool Must, typename Cond, typename ... Thens >
   struct cond2impl
   {
      typedef cond2impl key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare2< cond2impl, Cond, Thens ... >( st, "( ", ( Must ? " ->> " : " --> " ), " ", " )" );
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
	 prepare2< cond3impl, Cond, Then, Else >( st, "( ", ( Must ? " ->> " : " --> " ), " / ", " )" );
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
	 : cond3impl< true, Cond, Then, Else > {};

   template< typename Cond, typename Then, typename Else >
   struct ifthenelse
	 : cond3impl< false, Cond, Then, Else > {};

   template< unsigned N, unsigned M, typename ... Rules >
   struct rep2
   {
      typedef rep2 key_type;

      template< typename Print >
      static void prepare( Print & st )
      {
	 prepare1< rep2, Rules ... >( st, "", "( ", " ", " )", "{" + to_string( N ) + ',' + to_string( M ) + "}" );
      }

      template< bool Must, typename Input, typename Debug, typename ... States >
      static bool match( Input & in, Debug & de, States && ... st )
      {
	 static_assert( N <= M, "pegtl: illegal expression rep< R, N, M > where N is greater than M" );

	 typename Input::template marker< Must > p( in );

	 for ( unsigned i = 0; i < N; ++i ) {
	    if ( ! seq< Rules ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... ) ) {
	       return p( false );
	    }
	 }
	 for ( unsigned i = N; i < M; ++i ) {
	    if ( ! seq< Rules ... >::template i_match< Must >( in, de, std::forward< States >( st ) ... ) ) {
	       return p( true );
	    }
	 }
	 return p( not_at< seq< Rules ... > >::template match< Must >( in, de, std::forward< States >( st ) ... ) );
      }
   };

   template< typename Rule, typename PadL, typename PadR = PadL >
   struct pad
	 : seq< star< PadL >, Rule, star< PadR > > {};

   template< typename Rule, typename PadL >
   struct padl
	 : seq< star< PadL >, Rule > {};

   template< typename Rule, typename PadR >
   struct padr
	 : seq< Rule, star< PadR > > {};

   template< typename Rule, typename Glue >
   struct list
	 : seq< Rule, star< ifmust< Glue, Rule > > > {};

} // pegtl

#endif
