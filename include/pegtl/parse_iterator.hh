// Copyright (c) 2008 by Dr. Colin Hirsch 
// Please see license.txt for license.

#ifndef COHI_PEGTL_HH
#error "Please #include only pegtl.hh (rather than individual pegtl_*.hh files)."
#endif

#ifndef COHI_PEGTL_PARSE_ITERATOR_HH
#define COHI_PEGTL_PARSE_ITERATOR_HH

namespace pegtl
{
   // Functions to parse input given as iterator range.

   // Wrapper functions that add another convenience layer: instantiation
   // and initialisation of the input class. See file parse_generic.hh for
   // the wrapped functions.

   // The *_parse_input_* functions set up the parser to parse input from
   // a range of INPUT iterators; only the minimum required for back-tracking
   // is buffered, therefore these functions work fine for very large inputs,
   // however only parsers that otherwise only perform a single pass on the
   // input can be used.

   template< typename TopRule, typename InputIter, typename Location = dummy_location, typename ... States >
   bool dummy_parse_input_throws( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return dummy_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = dummy_location, typename ... States >
   bool dummy_parse_input_nothrow( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return dummy_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = offset_location, typename ... States >
   bool basic_parse_input_throws( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return basic_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = offset_location, typename ... States >
   bool basic_parse_input_nothrow( const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return basic_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = offset_location, typename ... States >
   bool trace_parse_input_throws( const bool trace, const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return trace_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename InputIter, typename Location = offset_location, typename ... States >
   bool trace_parse_input_nothrow( const bool trace, const InputIter & begin, const InputIter & end, States && ... st )
   {
      buffer_input< InputIter, Location > in( begin, end );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   // The *_parse_forward_* functions set up the parser to parse input from
   // a range of FORWARD iterators; this is more efficient than the functions
   // for input iterators and should be used whenever possible; there are no
   // specialisations for random-access iterators.

   template< typename TopRule, typename ForwardIter, typename Location = dummy_location, typename ... States >
   bool dummy_parse_forward_throws( const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      forward_input< ForwardIter, Location > in( begin, end );
      return dummy_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename ForwardIter, typename Location = dummy_location, typename ... States >
   bool dummy_parse_forward_nothrow( const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      forward_input< ForwardIter, Location > in( begin, end );
      return dummy_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename ForwardIter, typename Location = ascii_location, typename ... States >
   bool basic_parse_forward_throws( const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      forward_input< ForwardIter, Location > in( begin, end );
      return basic_parse_throws< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename ForwardIter, typename Location = ascii_location, typename ... States >
   bool basic_parse_forward_nothrow( const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      forward_input< ForwardIter, Location > in( begin, end );
      return basic_parse_nothrow< TopRule >( in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename ForwardIter, typename Location = ascii_location, typename ... States >
   bool trace_parse_forward_throws( const bool trace, const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      forward_input< ForwardIter, Location > in( begin, end );
      return trace_parse_throws< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename ForwardIter, typename Location = ascii_location, typename ... States >
   bool trace_parse_forward_nothrow( const bool trace, const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      forward_input< ForwardIter, Location > in( begin, end );
      return trace_parse_nothrow< TopRule >( trace, in, std::forward< States >( st ) ... );
   }

   // Please read the comment on the smart_parse_* functions in parse_generic.hh!

   template< typename TopRule, typename ForwardIter, typename Location = ascii_location, typename ... States >
   bool smart_parse_forward_throws( const bool trace, const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      return dummy_parse_forward_nothrow( begin, end, std::forward< States >( st ) ... ) || trace_parse_forward_throws( trace, begin, end, std::forward< States >( st ) ... );
   }

   template< typename TopRule, typename ForwardIter, typename Location = ascii_location, typename ... States >
   bool smart_parse_forward_nothrow( const bool trace, const ForwardIter & begin, const ForwardIter & end, States && ... st )
   {
      return dummy_parse_forward_nothrow( begin, end, std::forward< States >( st ) ... ) || trace_parse_forward_nothrow( trace, begin, end, std::forward< States >( st ) ... );
   }

} // pegtl

#endif
