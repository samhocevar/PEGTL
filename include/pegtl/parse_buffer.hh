// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_BUFFER_HH
#define COHI_PEGTL_PARSE_BUFFER_HH

namespace pegtl
{
   // Wrapper functions that add another convenience layer: instantiation
   // and initialisation of the input class. See file parse_generic.hh for
   // the wrapped functions.

   // The *_parse_buffer_* functions set up the parser to parse input from
   // a range of input iterators; only the minimum required for back-tracking
   // is buffered, therefore these functions work fine for very large inputs,
   // however only parsers that otherwise only perform a single pass on the
   // input can be used.

   template< typename TopRule, typename InputIter, typename Location = ascii_location, typename ... States >
   bool dummy_parse_buffer_throws( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return dummy_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = ascii_location, typename ... States >
   bool dummy_parse_buffer_nothrow( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return dummy_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = ascii_location, typename ... States >
   bool basic_parse_buffer_throws( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return basic_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = ascii_location, typename ... States >
   bool basic_parse_buffer_nothrow( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return basic_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = ascii_location, typename ... States >
   bool trace_parse_buffer_throws( const bool trace, const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return trace_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = ascii_location, typename ... States >
   bool trace_parse_buffer_nothrow( const bool trace, const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

} // pegtl

#endif
