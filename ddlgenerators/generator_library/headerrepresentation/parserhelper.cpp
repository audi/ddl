/**
 * @file
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

#include <cstring>
#include "parserhelper.h"


namespace ddl_generator
{

void skipWhitespace(const char*& p, const char* additional_whitechars)
{
    if (nullptr == p)
    {
        return;
    }
    if (additional_whitechars != nullptr)
    {
        while (a_util::strings::white_space.find(*p) != std::string::npos || (*p != '\0' && strchr(additional_whitechars, *p) != nullptr))
        {
            p++;
        }
    }
    else
    {
        while (a_util::strings::white_space.find(*p) != std::string::npos)
        {
            p++;
        }
    }
}


bool getNextWord(const char*& src, std::string& dest, const char* additional_separator, bool use_escape)
{
    if (nullptr == src)
    {
        return false;
    }
    dest.clear();

    skipWhitespace(src);

    if (*src == '\0')
    {
        return false;
    }

    char escape_active = false;
    char last_char = '\0';
    char quote = '\0';

    if (*src == '\"' || *src == '\'')
    {
        quote = *(src++);
        const char* strc_start = src;

        while (*src != '\0' && (escape_active || *src != quote))
        {
            escape_active = use_escape && (*src == '\\' && last_char != '\\');  // escape next char?
            last_char = *src;
            src++;
        }
        dest.assign(strc_start, src - strc_start);
        //dest.set(strc_start, src-strc_start);

        if (*src == quote)
        {
            src++;
        }
    }
    else
    {
        const char* src_start = src;

        if (additional_separator == nullptr)
        {
            while (*src != '\0' && a_util::strings::white_space.find(*src) == std::string::npos)
            {               
                src++;
                size_t n = a_util::strings::white_space.find(*src);
                size_t n_m = std::string::npos;
                if (*src == '\"' || *src == '\'')
                {
                    quote = *(src);

                    do
                    {
                        escape_active = use_escape && (*src == '\\' && last_char != '\\');
                        last_char = *src;
                        src++;
                    } while (*src != '\0' && (escape_active || *src != quote));
                }
            }

            dest.assign(src_start, src-src_start);
        }
        else
        {
            while (*src != '\0' && (a_util::strings::white_space.find(*src) == std::string::npos 
                && strchr(additional_separator, *src) == nullptr))
            {
                src++;
            }

            dest.assign(src_start, src-src_start);
        }
    }

    return true;
}

void seekChars(const char*& p, const char* chars, bool ignore_quotes)
{

    if ((nullptr == p) ||
        (nullptr == chars))
    {
        return;
    }

    char escape_active = false;
    char last_char = '\0';
    bool inside_quotes = false;
    char quote = '\0';

    while (*p != '\0' && (escape_active || inside_quotes || (strchr(chars, *p) == nullptr)))
    {
        if (!ignore_quotes)
        {
            if (!escape_active && (*p == '\"' || *p == '\''))
            {
                if (inside_quotes && quote == *p)
                {
                    quote = '\0';
                    inside_quotes = false;
                }
                else
                {
                    quote = *p;
                    inside_quotes = true;
                }
            }
        }
        else
        {
            if (!escape_active && (*p == '\"' || *p == '\''))
            {
                    quote = '\0';
            }
        }

        escape_active = (*p == '\\' && last_char != '\\');  // escape next char?
        last_char = *p;
        p++;
    }
}

void seekString(const char*& p, const char* seek_string, char* comment, bool ignore_quotes)
{
    if ((nullptr == p) ||
        (nullptr == seek_string))
    {
        return;
    }
    bool escape_active = false;
    char last_char = '\0';
    bool inside_quotes = false;
    char quote = '\0';
    size_t  str_len = a_util::strings::getLength(seek_string);
    int  n_char = 0;

    if (comment)
    {
        comment[n_char] = '\0';
    }
    while (*p != '\0')
    {
        if (!ignore_quotes)
        {
            if (!escape_active && !inside_quotes && *p == *seek_string)
            {
                if (a_util::strings::compare(p, seek_string, str_len) == 0)
                {
                    if (comment)
                    {
                        comment[n_char] = '\0';
                    }
                    return;
                }
            }

            if (!escape_active && (*p == '\"' || *p == '\''))
            {
                if (inside_quotes && quote == *p)
                {
                    quote = '\0';
                    inside_quotes = false;
                }
                else
                {
                    quote = *p;
                    inside_quotes = true;
                }
            }
        }
        else
        {
            if (!escape_active && *p == *seek_string)
            {
                if (a_util::strings::compare(p, seek_string, str_len) == 0)
                {
                    if (comment)
                    {
                        comment[n_char] = '\0';
                    }
                    return;
                }
            }
        
        }
        escape_active = (*p == '\\' && last_char != '\\');  // escape next char?
        last_char = *p;
        if (comment)
        {
            comment[n_char] = *p;
        }
        ++p;
        n_char++;
    }
    if (comment)
    {
        comment[n_char] = '\0';
    }
}

} // namespace A_UTILS_NS
