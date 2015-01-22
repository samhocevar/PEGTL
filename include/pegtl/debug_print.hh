// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_DEBUG_PRINT_HH
#define COHI_PEGTL_DEBUG_PRINT_HH

namespace pegtl
{
   template< typename... Rules > struct insert_help;

   template<>
   struct insert_help<>
   {
      template< typename Print >
      static void insert( Print &, const bool )
      { }
   };

   template< typename Rule, typename... Rules >
   struct insert_help< Rule, Rules... >
   {
      template< typename Print >
      static void insert( Print & st, const bool force )
      {
	 st.template insert_impl< Rule >( force );
	 insert_help< Rules... >::insert( st, force );
      }
   };

   struct printer
   {
      typedef void ( * inserter ) ( printer & );

      template< typename TopRule >
      printer( const tag< TopRule > & )
	    : m_top_inserter( & TopRule::template prepare< printer > ),
	      m_top_rule_key( key< TopRule >() ),
	      m_top_rule_value( value< TopRule >() )
      { }

      struct value_type
      {
	 value_type()
	 { }

	 value_type( const std::string & name, const std::string & expr )
	       : m_name( name ),
		 m_expr( expr )
	 { }

	 std::string m_name;
	 std::string m_expr;
      };

      typedef std::string key_type;
      typedef std::map< key_type, value_type > map_type;

      template< typename Rule >
      static key_type key()
      {
	 return typeid( typename Rule::key_type ).name();
      }

      template< typename Rule >
      static value_type value()
      {
	 const std::string d = demangle< Rule >();
	 return value_type( d, d );
      }

      template< typename Rule >
      std::string rule()
      {
	 const value_type & e = find< Rule >();

	 if ( e.m_name.empty() || ( e.m_name == e.m_expr ) ) {
	    return e.m_name;
	 }
	 else {
	    return e.m_name + " === " + e.m_expr;
	 }
      }

      template< typename Rule >
      const std::string & name()
      {
	 return find< Rule >().m_name;
      }

      template< typename Rule >
      const std::string & expr()
      {
	 return find< Rule >().m_expr;
      }      

      template< typename... Rules >
      void insert( const bool force = false )
      {
	 insert_help< Rules... >::insert( *this, force );
      }

      template< typename Rule >
      void update( const std::string & expr, const bool both = false )
      {
	 if ( both ) {
	    m_rules[ key< Rule >() ] = value_type( expr, expr );
	 }
	 else {
	    m_rules[ key< Rule >() ].m_expr = expr;
	 }
      }

      template< typename Rule >
      void update( const std::string & name, const std::string & expr )
      {
	 m_rules[ key< Rule >() ] = value_type( name, expr );
      }

      void print_rules()
      {
	 ensure_insert();

	 for ( map_type::const_iterator i = m_rules.begin(); i != m_rules.end(); ++i ) {
	    if ( i->second.m_name == i->second.m_expr ) {
	       PEGTL_PRINT( "RULE1 " << i->second.m_name );
	    }
	    else {
	       PEGTL_PRINT( "RULE2 " << i->second.m_name << " === " << i->second.m_expr );
	    }
	 }
      }

   private:
      map_type m_rules;
      const value_type m_default_value;

      const inserter m_top_inserter;
      const key_type m_top_rule_key;
      const value_type m_top_rule_value;

   public:
      void ensure_insert()
      {
	 if ( m_rules.empty() ) {
	    m_rules.insert( std::make_pair( m_top_rule_key, m_top_rule_value ) );
	    ( *m_top_inserter )( *this );
	 }
      }

      template< typename Rule >
      void insert_impl( const bool force = false )
      {
	 if ( m_rules.insert( std::make_pair( key< Rule >(), value< Rule >() ) ).second || force ) {
	    Rule::prepare( *this );
	 }
      }

      template< typename Rule >
      const value_type & find()
      {
	 ensure_insert();

	 const typename map_type::const_iterator i = m_rules.find( key< Rule >() );

	 if ( i == m_rules.end() ) {
	    return m_default_value;
	 }
	 return i->second;
      }
   };

   struct names_impl
   {
      explicit names_impl( const std::string & result )
	    : m_result( result )
      { }

      std::string operator() () const
      {
	 return m_result;
      }

      operator std::string () const
      {
	 return m_result;
      }

   protected:
      template< typename Rule, typename Print >
      static std::string name( Print & st )
      {
	 return st.template name< Rule >();
      }

      const std::string m_result;
   };

   template< typename ... Rules > struct names;

   template< typename Rule >
   struct names< Rule > : public names_impl
   {
      template< typename Print >
      names( Print & st, const std::string & a, const std::string &, const std::string & c )
	    : names_impl( a + name< Rule >( st ) + c )
      { }
   };

   template< typename Rule1, typename Rule2, typename ... Rules >
   struct names< Rule1, Rule2, Rules ... > : public names_impl
   {
      template< typename Print >
      names( Print & st, const std::string & a, const std::string & b, const std::string & c )
	    : names_impl( a + name< Rule1 >( st ) + b + names< Rule2, Rules ... >( st, std::string(), b, c )() )
      { }
   };

   template< typename TopRule >
   void print_rules()
   {
      printer( tag< TopRule >() ).print_rules();
   };

} // pegtl

#endif
