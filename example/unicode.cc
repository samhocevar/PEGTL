// Copyright (c) 2008 by Dr. Colin Hirsch
// Please see license.txt for license.

// Include the only public header file for the PEGTL.

#include <pegtl.hh>

namespace example
{
   using namespace pegtl;

   // This example defines a rule for quoted strings with C-style backslash-escapes
   // that is compatible with UTF-8 input, i.e. the quoted string can contain UTF-8
   // without ever truncating a multi-byte character.

   // The rule 'escaped' matches a backslash-escaped character; the use of ifmust<>
   // ensures that every backlash is followed by one of the appropriate characters.

   struct escaped
	 : ifmust< one< '\\' >, one< '\\', '"', '\'', 'a', 'f', 'n', 'r', 't', 'v' > > {};

   // A regular character is anything that is not an ASCII control character. This
   // also matches any byte of a unicode character with multi-byte encoding in UTF-8.

   struct regular
	 : not_range< 0, 31 > {};

   // This is simple, a character in the quoted string is either an escaped character
   // or a regular character. Note that the order of the two rules is important; with
   // sor< regular, escaped >, a backslash would always match rule 'regular', and rule
   // 'escaped' would never fire, so backslash would loose its special meaning.

   struct character
	 : sor< escaped, regular > {};

   // A quoted string starts with a quote, and then contains characters until another
   // quote is encountered. Escaped quotes can be embedded, they will be matched by
   // rule 'escaped'. A seq<> could be used instead of the ifmust<>, however usually
   // a grammar will have only one rule that will match a string starting with a quote,
   // and in this case the ifmust<> will produce a better error message (because an
   // input that starts with a quote, but does not contain the corresponding closing
   // quote, will produce an error in rule 'quoted', rather than wherever any sor<>
   // backtracking in the grammar might lead to).

   struct quoted
	 : ifmust< one< '"' >, until< one< '"' >, character > > {};

   // Note that rule 'quoted' will work fine with UTF-8 in the sense that the quoted
   // string can contain any UTF-8 character, however no validation of multi-byte
   // encoded characters is performed. Support for UTF-8 will likely be extended
   // in the future....

} // example

int main( int argc, char ** argv )
{
   for ( int i = 1; i < argc; ++i ) {
      pegtl::basic_parse_string< example::quoted >( argv[ i ] );
   }
   return 0;
}
