// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_STRING_HH
#define COHI_PEGTL_PARSE_STRING_HH

namespace pegtl
{
   // Functions to parse input given as std::string.

   // Wrapper functions that add another convenience layer: instantiation
   // and initialisation of the input class. See file parse_generic.hh for
   // the wrapped functions.

   // The *_parse_string_* functions set up the parser to parse a string.

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool dummy_parse_string_throws( const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return dummy_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool dummy_parse_string_nothrow( const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return dummy_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool basic_parse_string_throws( const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return basic_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool basic_parse_string_nothrow( const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return basic_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool trace_parse_string_throws( const bool trace, const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return trace_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool trace_parse_string_nothrow( const bool trace, const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   // Please read the comment on the smart_parse_* functions in parse_generic.hh!

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool smart_parse_string_throws( const bool trace, const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return smart_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename Location = ascii_location, typename ... States >
   bool smart_parse_string_nothrow( const bool trace, const std::string & string, States && ... st )
   {
      string_input< Location > in( string );
      return smart_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

} // pegtl

#endif
