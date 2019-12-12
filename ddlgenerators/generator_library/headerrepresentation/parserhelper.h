/**
 * @file
 * 
 * Parser helper functions.
 * 
 * @copyright
 * @verbatim
   Copyright @ 2017 Audi Electronics Venture GmbH. All rights reserved.

       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.

   You may add additional accurate notices of copyright ownership.
   @endverbatim
*/
#include <a_util/strings.h>


#ifndef DDL_GEN_PARSER_HELPER_CLASS_HEADER
#define DDL_GEN_PARSER_HELPER_CLASS_HEADER

namespace ddl_generator
{


/**
    * Advances the pointer to point at a position after the next whitespace sequence.
    *
    * @param p [inout] The current position.
    * @param additional_whitechars [in] Additional characters which are to be skipped.
    * @return void
    * @rtsafe
    */
void skipWhitespace(const char*& p, const char* additional_whitechars=nullptr);

/**
    * Copies the next word in a string into a string object.
    *
    * @param src                 [in] The source string.
    * @param dest              [out] The destination string.
    * @param additional_separator [in] Additional separator characters.
    * @param use_escape           [in] Uses the \\ character to detect a word too
    *
    * @return tTrue if successful, otherwise tFalse.
    */
bool getNextWord(const char*& src, 
                            std::string &dest, 
                            const char* additional_separator=nullptr, 
                            bool use_escape = true);

/**
    * Advances a pointer in a string to the next occurance of specified characters.
    * @param p [inout] The current position.
    * @param chars [in] The characters to look for.
    * @param ignore_quotes [in] Whether or not to ignore occurences which occur between quotes.
    * @return void
    * @rtsafe
    */
void seekChars(const char*& p, const char* chars, bool ignore_quotes=false);

/**
    * Advances a pointer in a string to the next occurcance of a string.
    * @param p [inout] The current position.
    * @param seek_string [in] The string to look for.
    * @param comment [out] Optional buffer that will be filled with the skipped characters.
    * @param ignore_quotes [in] Whether or not to ignore occurences which occur between quotes.
    * @return void
    * @rtsafe
    */
void seekString(const char*& p, const char* seek_string, char* comment = nullptr, bool ignore_quotes=false);

} // namespace A_UTILS_NS

#endif // DDL_GEN_PARSER_HELPER_CLASS_HEADER
