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

#include <ddl.h>
#include "header_importer.h"
#include "header_basic_type.h"
#include "header_typedef.h"
#include "header_header.h"
#include "parserhelper.h"

namespace ddl_generator
{
    namespace oo
    {
        //define all needed error types and values locally
        _MAKE_RESULT(0, ERR_NOERROR)
        _MAKE_RESULT(-4, ERR_POINTER)
        _MAKE_RESULT(-5, ERR_INVALID_ARG)
        _MAKE_RESULT(-11, ERR_INVALID_FILE)
        _MAKE_RESULT(-20, ERR_NOT_FOUND)
        _MAKE_RESULT(-24, ERR_PATH_NOT_FOUND)
        _MAKE_RESULT(-36, ERR_UNKNOWN_FORMAT)
        _MAKE_RESULT(-37, ERR_NOT_INITIALISED)
        _MAKE_RESULT(-38, ERR_FAILED)
    }
}

using namespace ddl_generator::oo;

using namespace ddl;


namespace ddl
{
#define AMOUNT_OF_TOKENS 3
    //#define AMOUNT_OF_SINGLE_SEPARATORS 27
    const std::string g_tokens[AMOUNT_OF_TOKENS] = { "struct", "enum", "union" };
    const std::string g_single_separators = "{}[]#()<>%:;.?*+-/�&|�!=,\\\"�;";
    const std::string g_name_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";


    class HeaderParserHelper
    {
    public:
        /**
         * CTOR
         */
        HeaderParserHelper() {};

        /**
         * DTOR
         */
        virtual ~HeaderParserHelper() {};

        /**
         * The method skipToNextContent skips comments and whitespace
         * until the next code fragment or the end of the file is reached.
         *
         * @param [in] src  Pass a pointer to a string,
         *                   returns a pointer to the next character in the string,
         *                   that is part of the code.
         */
        static void skipToNextContent(const char* &src)
        {
            const char* current = NULL;
            const char* forward_search = NULL;
            while (current != src)
            {
                current = src;
                forward_search = src;
                skipComment(src, getNextElement(forward_search));
                ddl_generator::skipWhitespace(src);
            }
        }

        /**
         * The method isValidName checks whether a string is a valid name for either a variable or a type.
         * Used definition:
         * Must start with a letter oder an underscore.
         * Second to last character can be a number, letter or underscore.
         *
         * @param [in] name  The name to be analyzed.
         * @returns  True if the name is valid, false otherwise.
         */
        static bool isValidName(const std::string &name)
        {
            a_util::regex::RegularExpression reg_ex("^[_a-zA-Z][_a-zA-Z0-9]*$");
            int consumed;
            return reg_ex.match(name, a_util::regex::RegularExpression::AT_Both, consumed);
        }

        /**
         * The method skipBlock skips the next block.
         * Example:
         * bla = 5;
         * typedef struct {
         *   int a;
         * };
         * The function skips to the semicolon after the last parenthesis.
         *
         * @param [in, out] src  The source to be skipped, points to the next character after the parenthesis.
         */
        static void skipBlock(const char* &src)
        {
            uint64_t depth = 0;
            while (*src != '\0' && *src != '{')
            {
                src++;
            }
            depth++;
            while (*src != '\0')
            {
                if (*src == '{')
                {
                    depth++;
                }
                else if (*src == '}')
                {
                    depth--;
                    if (depth == 0)
                    {
                        src++;
                        break;
                    }
                }
                src++;
            }
        }

        /**
         * The method exitBlock exists from the current block;
         * Example
         * blubb = 5;
         * bla blubber;
         * }
         * The method jumps to the character after the parenthesis.
         *
         * @param [in] src  The string to be analyzed, returns a pointer to the character after the parenthesis
         * of the current block or to the end of the string.
         */
        static void exitBlock(const char* &src)
        {
            while (*src != '\0')
            {
                if (*src == '}')
                {
                    src++;
                    break;
                }
                if (*src == '{')
                {
                    skipBlock(src);
                }
                else
                {
                    src++;
                }
            }
        }

        /**
         * The function getNextElement extracts the next c++ element.
         * This can be a whole word like a type name or a single character like
         * a plus sign or the beginning/end of a comment line //, / * or * /
         * (cannot write the "comment close" without space or it would break the code of this file)
         * This function will not check any validity. So Names like "123onetwothree" are allowed.
         *
         * @param [in, out] src  The string to be analyzed, returns a pointer to the character after found element.
         * @returns  A string containing the element.
         */
        static std::string getNextElement(const char* &src)
        {
            std::string result = "";
            while (*src != '\0')
            {
                //Check for a number or a name
                if (g_name_chars.find(*src) != std::string::npos)
                {
                    result.push_back(*src);
                    src++;
                    while (g_name_chars.find(*src) != std::string::npos)
                    {
                        result.push_back(*src);
                        src++;
                    }
                    break;
                }
                // Check for a single character like +, - and so on
                else if (g_single_separators.find(*src) != std::string::npos)
                {
                    result = *src;
                    src++;
                    // Check for comment
                    if (result == "/")
                    {
                        if (*src == '/' || *src == '*')
                        {
                            result.push_back(*src);
                            src++;
                        }
                    }
                    else if (result == "*")
                    {
                        if (*src == '/')
                        {
                            result.push_back(*src);
                            src++;
                        }
                    }
                    //Check for escape character
                    else if (result == "\\")
                    {
                        result.push_back(*src);
                        src++;
                    }
                    break;
                }
                else
                {
                    // Probably just space or newlines. If anything else has not been captured by this algorithm,
                    // please add it here or where it fits.
                    src++;
                }
            }
            return result;
        }

        /**
         * The function getNextCodeElement extracts the next c++ element outside a comment.
         * This can be a whole word like a type name or a single character like
         * a plus sign.
         * This function will not check any validity. So Names like "123onetwothree" are allowed.
         * This function assumes that it is outside a comment.
         *
         * @param [in, out] src  The string to be analyzed, returns a pointer to the character after found element.
         * @returns  A string containing the element.
         */
        static std::string getNextCodeElement(const char* &src)
        {
            std::string result;
            while (*src != '\0')
            {
                result = getNextElement(src);
                if (result == "/*" || result == "//")
                {
                    skipComment(src, result);
                }
                else
                {
                    break;
                }
            }
            return result;
        }

        /**
         * The method skipComment skips a comment. Use in conjunction with getNextElement().
         *
         * @param [in, out] p  A pointer to the string containing the comment.
                               The pointer will point to the first character after the comment.
         * @param [in] comment_begin  The string containing the begin of the comment.
         *             if the string does not contain a comment beginning (line // or / *)
         *             nothing will be skipped.
         */
        static void skipComment(const char* &src, const std::string &comment_begin)
        {
            if (comment_begin == "//")
            {
                skipEOL(src);
            }
            else if (comment_begin == "/*")
            {
                while ((getNextElement(src) != "*/") && (*src != '\0'))
                {
                }
            }
        }

        /**
         * The method SkipToEOL [...]
         *
         * @param [in, out] src  A pointer to the string containing the comment.
                                  The pointer will point to the first character after the EOL character (0x0A).
         */
        static void skipEOL(const char* &src)
        {
            while (*src != '\n' && *src != '\0')
            {
                src++;
            }
            src++;
        }
    };


    HeaderImporter::HeaderImporter()
    {
        _types = NULL;
        _header = NULL;
        _default_type = NULL;
    }

    HeaderImporter::~HeaderImporter()
    {
        _types = NULL;
        _header = NULL;
        _default_type = NULL;
    }

    a_util::result::Result HeaderImporter::setHeaderString(const std::string &source)
    {
        _input_file = a_util::filesystem::Path();
        _header_source = source;
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::setFileName(const a_util::filesystem::Path &filename)
    {
        _header_source = "";
        _input_file = filename;
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::setKnownTypes(const HeaderTypesVec* types)
    {
        _types = types;
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::setDefaultIntegerType(const HeaderType* type)
    {
        _default_type = type;
        return ERR_NOERROR;
    }

    HeaderTypesVec* HeaderImporter::getDefaultTypes()
    {
        HeaderBasicType* basic_type = NULL;
        HeaderTypesVec* types = new HeaderTypesVec();
        
        // Remark: t-Types(for example tBool, tUInt32) are keeped for compliance with old versions

        basic_type = new HeaderBasicType("bool", 1, 8);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("tBool", basic_type));
        
        basic_type = new HeaderBasicType("char", 1, 8);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("tChar", basic_type));

        basic_type = new HeaderBasicType("uint8_t", 1, 8); 
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("unsigned char", basic_type));
        types->push_back(new HeaderTypedef("tUInt8", basic_type));
        types->push_back(new HeaderTypedef("uint_least8_t", basic_type));
        types->push_back(new HeaderTypedef("uint_least8_t", basic_type));
        types->push_back(new HeaderTypedef("uint_fast8_t", basic_type));

        basic_type = new HeaderBasicType("int8_t", 1, 8);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("signed char", basic_type));
        types->push_back(new HeaderTypedef("char", basic_type)); 
        types->push_back(new HeaderTypedef("tInt8", basic_type));
        types->push_back(new HeaderTypedef("int_least8_t", basic_type));
        types->push_back(new HeaderTypedef("int_least8_t", basic_type));
        types->push_back(new HeaderTypedef("int_fast8_t", basic_type));

        basic_type = new HeaderBasicType("uint16_t", 2, 16);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("unsigned short", basic_type));
        types->push_back(new HeaderTypedef("tUInt16", basic_type)); 
        types->push_back(new HeaderTypedef("uint_least16_t", basic_type));

        basic_type = new HeaderBasicType("int16_t", 2, 16);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("signed short", basic_type));
        types->push_back(new HeaderTypedef("short", basic_type));
        types->push_back(new HeaderTypedef("tInt16", basic_type)); 
        types->push_back(new HeaderTypedef("int_least16_t", basic_type));
        
        basic_type = new HeaderBasicType("uint32_t", 4, 32);
        types->push_back(basic_type);
#ifdef __ADTF32
        types->push_back(new HeaderTypedef("unsigned long", basic_type));
#endif
        types->push_back(new HeaderTypedef("unsigned int", basic_type));
        types->push_back(new HeaderTypedef("tUInt32", basic_type)); 
        types->push_back(new HeaderTypedef("uint_least32_t", basic_type));
        types->push_back(new HeaderTypedef("uint_fast16_t", basic_type));
        types->push_back(new HeaderTypedef("uint_fast32_t", basic_type));

        basic_type = new HeaderBasicType("int32_t", 4, 32);
        types->push_back(basic_type);
#ifdef __ADTF32
        types->push_back(new HeaderTypedef("signed long", basic_type));
        types->push_back(new HeaderTypedef("long", basic_type));
#endif
        types->push_back(new HeaderTypedef("signed int", basic_type));
        types->push_back(new HeaderTypedef("int", basic_type)); 
        types->push_back(new HeaderTypedef("tInt32", basic_type));
        types->push_back(new HeaderTypedef("int_least32_t", basic_type));
        types->push_back(new HeaderTypedef("int_fast16_t", basic_type));
        types->push_back(new HeaderTypedef("int_fast32_t", basic_type));

        basic_type = new HeaderBasicType("uint64_t", 8, 64);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("unsigned long", basic_type));
        types->push_back(new HeaderTypedef("unsigned long long", basic_type));
        types->push_back(new HeaderTypedef("unsigned __int64", basic_type));
        types->push_back(new HeaderTypedef("tUInt64", basic_type)); 
        types->push_back(new HeaderTypedef("uint_least64_t", basic_type));
        types->push_back(new HeaderTypedef("uint_least64_t", basic_type));

        basic_type = new HeaderBasicType("int64_t", 8, 64);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("signed long", basic_type));
        types->push_back(new HeaderTypedef("long", basic_type));
        types->push_back(new HeaderTypedef("signed long long", basic_type));
        types->push_back(new HeaderTypedef("long long", basic_type));
        types->push_back(new HeaderTypedef("signed __int64", basic_type));
        types->push_back(new HeaderTypedef("__int64", basic_type));
        types->push_back(new HeaderTypedef("tInt64", basic_type)); 
        types->push_back(new HeaderTypedef("int_least64_t", basic_type));
        types->push_back(new HeaderTypedef("int_least64_t", basic_type));

        basic_type = new HeaderBasicType("float", 4, 32); 
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("tFloat32", basic_type));

        basic_type = new HeaderBasicType("double", 8, 64);

        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("tFloat64", basic_type)); 

        // tFloat128 is not supported, because type is deprecated
        /*basic_type = new HeaderBasicType("tFloat128", 4, 128);
        types->push_back(basic_type);
        types->push_back(new HeaderTypedef("long double", basic_type));*/

        return types;
    }

    Header * HeaderImporter::getHeader()
    {
        return _header;
    }

    a_util::result::Result HeaderImporter::createNew()
    {
        if (_header_source == "" && _input_file.getLastElement().isEmpty())
        {
            return ERR_NOT_INITIALISED;
        }
        if (!_input_file.getLastElement().isEmpty())
        {
            if (a_util::filesystem::readTextFile(_input_file, _header_source) != a_util::filesystem::OK)
            {
                return (ERR_FAILED);
            }
        }

        _last_error = "";
        // Old header will not be destroyed since caller is responsible for the pointer.
        _header = new Header();
        _header->setName(_input_file.getLastElement());
        if (isFailed((BuildTypedefs())))
        {
            return ERR_FAILED;
        }
        if (isFailed((BuildConstants())))
        {
            return ERR_FAILED;
        }
        if (isFailed(buildEnums()))
        {
            return ERR_FAILED;
        }
        if (isFailed((buildStructs())))
        {
            return ERR_FAILED;
        }
        if (_last_error.length() > 0)
        {
            return ERR_FAILED;
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::DestroyHeader()
    {
        delete _header;
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::BuildTypedefs()
    {
        if (_header == NULL)
        {
            return ERR_POINTER;
        }
        const char* pos = _header_source.c_str();
        while (*pos != '\0')
        {
            std::string next_word = HeaderParserHelper::getNextCodeElement(pos);
            if (next_word == "typedef")
            {
                std::string original_name;
                std::string new_name;
                std::string token;
                if (a_util::result::isOk(parseTypedef(pos, token, original_name, new_name)))
                {
                    const HeaderType* original_type = findKnownType(original_name);
                    if (original_type != NULL)
                    {
                        HeaderTypedef* new_type = new HeaderTypedef(new_name, original_type);
                        _header->addTypedef(new_type);
                    }
                    else
                    {
                        addErrorDescription(std::string("Unknown type name: ").append(original_name), pos);
                    }
                }
            }
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::BuildConstants()
    {
        if (_header == NULL)
        {
            return ERR_POINTER;
        }

        // Only constants with integers as values will be accepted, since they are needed for the size of arrays.
        // Buildup of a constant:
        // const Type Name = number;
        // #define NAME number
        const char* pos = _header_source.c_str();
        while (*pos != '\0')
        {
            std::string next_word = HeaderParserHelper::getNextCodeElement(pos);
            if (next_word == "const")
            {
                next_word = HeaderParserHelper::getNextCodeElement(pos);
                if (HeaderParserHelper::isValidName(next_word))
                {
                    std::string type_name = next_word;
                    if (type_name == "signed" || type_name == "unsigned")
                    {
                        next_word = HeaderParserHelper::getNextCodeElement(pos);
                        type_name.append(" ");
                        type_name.append(next_word);
                    }
                    // Look for the type in the list of known types.
                    const HeaderType* type = findKnownType(next_word);
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                    // Pointers will be ignored, so next element has to be the variable name
                    if (HeaderParserHelper::isValidName(next_word))
                    {
                        std::string name = next_word;
                        next_word = HeaderParserHelper::getNextCodeElement(pos);
                        if (next_word == "=")
                        {
                            next_word = HeaderParserHelper::getNextCodeElement(pos);
                            // Anything but integers will be ignored since constants are only only useful for the parser,
                            // if an array size refers to them.
                            if (a_util::strings::isInt64(next_word))
                            {
                                int64_t value = a_util::strings::toInt64(next_word);
                                next_word = HeaderParserHelper::getNextCodeElement(pos);
                                // No fancy values like 34+32 or 866+other_const will be accepted at the moment, so we expect a semicolon now.
                                if (next_word == ";")
                                {
                                    //Yeah! We found a constant with an int.
                                    HeaderConstant* constant = new HeaderConstant();
                                    constant->setName(name);
                                    constant->setType(type);
                                    constant->reset(value);
                                    _header->addConstant(constant);
                                }
                            }
                        }
                    }
                }
            }
            else if (next_word == "#")
            {
                next_word = HeaderParserHelper::getNextCodeElement(pos);
                if (next_word == "define")
                {
                    // Look for end of line (ignoring multiline defines)
                    const char* eol_pos = pos;
                    HeaderParserHelper::skipEOL(eol_pos);
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                    std::string const_name = next_word;
                    // Still same line?
                    if (pos <= eol_pos)
                    {
                        const char* tmp_pos = pos;

                        // a #define doesn't necessarily have a value. so maybe
                        // we're already in the next line and mistakenly analyze
                        // the next statement already. this most likely breaks the
                        // parsing process, because this "next_word" would be lost.
                        // so don't consume the next word until we're sure to have a 
                        // value
                        next_word = HeaderParserHelper::getNextCodeElement(tmp_pos);
                        if (tmp_pos <= eol_pos)
                        {
                            // everything ok, go on exploring this header
                            next_word = HeaderParserHelper::getNextCodeElement(pos);
                        }

                        // Still same line? Did we find an int?
                        if (pos <= eol_pos && a_util::strings::isInt64(next_word))
                        {
                            int64_t value = a_util::strings::toInt64(next_word);
                            // Check if the next word is not in the same line anymore or eof is reached.
                            // Allowed is only /*...*/ comments.
                            // We must not increase pos, since otherwise a "#define" or "const"
                            // from the next line could be skipped.
                            next_word = HeaderParserHelper::getNextElement(tmp_pos);
                            while (tmp_pos < eol_pos && next_word == "/*")
                            {
                                HeaderParserHelper::skipComment(tmp_pos, next_word);
                                next_word = HeaderParserHelper::getNextElement(tmp_pos);
                            }
                            if (tmp_pos > eol_pos || *tmp_pos == '\0')
                            {
                                const HeaderType* type = findKnownType("tUInt64");
                                if (_default_type != NULL)
                                {
                                    type = _default_type;
                                }
                                if (type != NULL)
                                {
                                    HeaderConstant* constant = new HeaderConstant();
                                    constant->setName(const_name);
                                    constant->setType(type);
                                    constant->reset(value);
                                    _header->addConstant(constant);
                                }
                            }
                        }
                    }
                }
            }
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::buildStructs()
    {
        // The following types of definitions will raise our parsing attention:
        // 1. struct original_name {..};
        // 2. typedef struct {..} typedef_name;
        // 3. typedef struct original_name {..} typedef_name;
        // #3 will result in a type entry and a typedef entry in the header, since both names refer to the same type
        // Not supported are unnamed structs, like the following:
        // 4. struct {..} name;
        // 5. struct {..};
        // 6. typedef struct {..};
        const char* pos = _header_source.c_str();
        // This pack information could be extracted as a configurable setting later
        std::vector<size_t> pack_stack;
        // Set default packing depending n compiler used
#if defined(_MSC_VER) || (defined(__GNUC__) && defined(__ADTF64))
        pack_stack.push_back(8);
#else
#if defined(__GNUC__) & defined(__ADTF32)
        pack_stack.push_back(4);
#else
    //unknown compiler / platform
        pack_stack.push_back(4);
#endif
#endif
        while (*pos != '\0')
        {
            char tmp = *pos;
            bool is_typedef = false;
            std::string next_word = HeaderParserHelper::getNextCodeElement(pos);
            // Packing identification
            if (next_word == "#")
            {
                next_word = HeaderParserHelper::getNextCodeElement(pos);
                if (next_word == "pragma")
                {
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                    if (next_word == "pack")
                    {
                        next_word = HeaderParserHelper::getNextCodeElement(pos);
                        if (next_word == "(")
                        {
                            std::string command;
                            next_word = HeaderParserHelper::getNextCodeElement(pos);
                            if (!a_util::strings::isInt64(next_word))
                            {
                                if (next_word == "push" || next_word == "pop")
                                {
                                    command = next_word;
                                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                                    if (next_word == ",")
                                    {
                                        next_word = HeaderParserHelper::getNextCodeElement(pos);
                                    }
                                }
                                else
                                {
                                    addErrorDescription("Cannot parse pragma, only push and pop are supported.", pos);
                                }
                            }
                            if (command == "pop")
                            {
                                if (next_word == ")")
                                {
                                    if (pack_stack.size() > 1)
                                    {
                                        pack_stack.pop_back();
                                    }
                                    else
                                    {
                                        addErrorDescription("Too many pragma pop.", pos);
                                    }
                                }
                                else
                                {
                                    addErrorDescription("Pragma pop is missing a closing parenthesis.", pos);
                                }
                            }
                            else
                            {
                                if (a_util::strings::isInt64(next_word))
                                {
                                    if (command == "")
                                    {
                                        pack_stack[pack_stack.size() - 1] = a_util::strings::toInt64(next_word);
                                    }
                                    else if (command == "push")
                                    {
                                        pack_stack.push_back(a_util::strings::toInt64(next_word));
                                    }
                                    else {
                                        addErrorDescription(a_util::strings::format("Unknown pragma command %s.", command.c_str()), pos);
                                    }
                                }
                                else
                                {
                                    addErrorDescription("Unknown value found within pragma, expected a number.", pos);
                                }
                            }
                        }
                    }
                }
            }
            // Check for typedef of #2 or #3
            if (next_word == "typedef")
            {
                is_typedef = true;
                next_word = HeaderParserHelper::getNextCodeElement(pos);
            }
            // Check for #1 - #3
            if (next_word == "struct")
            {
                next_word = HeaderParserHelper::getNextCodeElement(pos);
                // Check for #1 or #3
                std::string original_name;
                std::string typedef_name;
                if (next_word != "{")
                {
                    original_name = next_word;
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                }
                if (next_word == "{")
                {
                    HeaderStructElementVec elements = extractStructElements(pos);
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                    if (next_word != ";")
                    {
                        if (!is_typedef)
                        {
                            addErrorDescription("Cannot parse struct.", pos);
                        }
                        else
                        {
                            //check for #2 or #3
                            if (HeaderParserHelper::isValidName(next_word))
                            {
                                typedef_name = next_word;
                                next_word = HeaderParserHelper::getNextCodeElement(pos);
                            }
                            else
                            {
                                addErrorDescription("The typedef struct is unreadable.", pos);
                            }
                        }
                    }
                    HeaderStruct* header_struct = new HeaderStruct();
                    if (original_name.size() != 0)
                    {
                        header_struct->setName(original_name);
                        if (is_typedef && typedef_name.size() != 0)
                        {
                            _header->addTypedef(new HeaderTypedef(typedef_name, header_struct));
                        }
                    }
                    else
                    {
                        header_struct->setName(typedef_name);
                    }
                    // Check for not #4 - #6
                    if (header_struct->getName().size() != 0 && !(is_typedef && typedef_name.size() == 0))
                    {
                        for (HeaderStructElementVec::const_iterator iter = elements.begin(); iter != elements.end(); iter++)
                        {
                            header_struct->addElement(*iter);
                        }
                        // Set current packing within struct if current packing is smaller than the native packing of the struct
                        if (header_struct->getPacking() > pack_stack.back())
                        {
                            header_struct->setPacking(pack_stack.back());
                        }
                        _header->addStruct(header_struct);
                    }
                    else
                    {
                        addErrorDescription("Cannot interpret this struct, no name given.", pos);
                        // Clean up allocated memory
                        delete header_struct;
                        for (HeaderStructElementVec::const_iterator iter = elements.begin(); iter != elements.end(); iter++)
                        {
                            delete *iter;
                        }
                    }
                }
                else
                {
                    addErrorDescription("Cannot interpret this struct.", pos);
                    return ERR_UNKNOWN_FORMAT;
                }
            }
            else
            {
                // No typedef for a struct it seems.
            }
        }
        return ERR_NOERROR;
    }

    a_util::result::Result HeaderImporter::buildEnums()
    {
        // The following types of definitions will raise our parsing attention:
        // 1. enum original_name {..};
        // 2. typedef enum {..} typedef_name;
        // 3. typedef enum original_name {..} typedef_name;
        // #3 will result in a type entry and a typedef entry in the header, since both names refer to the same type
        // Not supported are unnamed enums, like the following:
        // 4. enum {..} name;
        // 5. enum {..};
        // 6. typedef enum {..};
        const char* pos = _header_source.c_str();

        while (*pos != '\0')
        {
            char tmp = *pos;
            bool is_typedef = false;
            std::string next_word = HeaderParserHelper::getNextCodeElement(pos);

            // Check for typedef of #2 or #3
            if (next_word == "typedef")
            {
                is_typedef = true;
                next_word = HeaderParserHelper::getNextCodeElement(pos);
            }
            // Check for #1 - #3
            if (next_word == "enum")
            {
                next_word = HeaderParserHelper::getNextCodeElement(pos);
                // Check for #1 or #3
                std::string original_name;
                std::string typedef_name;
                if (next_word != "{")
                {
                    original_name = next_word;
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                }
                if (next_word == "{")
                {
                    HeaderEnumValueMap enum_values = extractEnumValues(pos);
                    next_word = HeaderParserHelper::getNextCodeElement(pos);
                    if (next_word != ";")
                    {
                        if (!is_typedef)
                        {
                            addErrorDescription("Cannot parse enum.", pos);
                        }
                        else
                        {
                            //check for #2 or #3
                            if (HeaderParserHelper::isValidName(next_word))
                            {
                                typedef_name = next_word;
                                next_word = HeaderParserHelper::getNextCodeElement(pos);
                            }
                            else
                            {
                                addErrorDescription("The typedef enum is unreadable.", pos);
                            }
                        }
                    }
                    HeaderEnum* header_enum = new HeaderEnum();
                    if (original_name.size() != 0)
                    {
                        header_enum->setName(original_name);
                        if (is_typedef && typedef_name.size() != 0)
                        {
                            _header->addTypedef(new HeaderTypedef(typedef_name, header_enum));
                        }
                    }
                    else
                    {
                        header_enum->setName(typedef_name);
                    }
                    // Check for not #4 - #6
                    if (header_enum->getName().size() != 0)
                    {
                        for (auto iter = enum_values.begin(); iter != enum_values.end(); iter++)
                        {
                            header_enum->addValue(iter->first, iter->second);
                        }
                        _header->addEnum(header_enum);
                    }
                    else
                    {
                        addErrorDescription("Cannot interpret this enum, no name given.", pos);
                        // Clean up allocated memory
                        delete header_enum;
                    }
                }
                else
                {
                    addErrorDescription("Cannot interpret this enum.", pos);
                    return ERR_UNKNOWN_FORMAT;
                }
            }
            else
            {
                // No typedef for a enum it seems.
            }
        }
        return ERR_NOERROR;
    }


    const std::string& HeaderImporter::getLastError() const
    {
        return _last_error;
    }

    const HeaderType* HeaderImporter::findKnownType(const std::string &name) const
    {
        const HeaderType* type = NULL;
        // Search for name in know types
        for (HeaderTypesVec::const_iterator iter = _types->begin(); iter != _types->end(); iter++)
        {
            if ((*iter)->getName() == name)
            {
                type = *iter;
                break;
            }
        }
        if (type == NULL)
        {
            // If not found in the known types, look in the already parsed typedefs for the type.
            const HeaderTypedefs typedefs = _header->getTypedefs();
            for (HeaderTypedefs::const_iterator iter = typedefs.begin(); iter != typedefs.end(); iter++)
            {
                if ((*iter)->getName() == name)
                {
                    type = *iter;
                    break;
                }
            }
        }
        if (type == NULL)
        {
            // If not found in the already parsed typedefs, look in the already parsed structs for the type.
            const HeaderStructs structs = _header->getStructs();
            for (HeaderStructs::const_iterator iter = structs.begin(); iter != structs.end(); iter++)
            {
                if ((*iter)->getName() == name)
                {
                    type = *iter;
                    break;
                }
            }
        }
        if (type == NULL)
        {
            // If not found in the already parsed typedefs or structs look in the already parsed enums for the type.
            const HeaderEnums enums = _header->getEnums();
            for (auto iter = enums.begin(); iter != enums.end(); iter++)
            {
                if ((*iter)->getName() == name)
                {
                    type = *iter;
                    break;
                }
            }
        }
        return type;
    }

    HeaderStructElementVec HeaderImporter::extractStructElements(const char* &pos)
    {
        // Copied over from the adtfcodec/cc_header.cpp implementation
        // modified to fit current requirements.
        std::string next_element;
        std::set<std::string> structs;
        HeaderStructElementVec elements;

        while (*pos != '\0')
        {
            next_element = HeaderParserHelper::getNextCodeElement(pos);
            if (next_element.compare("}") == 0)
            {
                break;
            }
            // Extract type
            std::string type_name = next_element;
            if (!HeaderParserHelper::isValidName(type_name))
            {
                addErrorDescription(std::string("Unexpected string ").append(type_name), pos);
            }
            else
            {
                if (type_name == "signed" || type_name == "unsigned")
                {
                    next_element = HeaderParserHelper::getNextCodeElement(pos);
                    if (HeaderParserHelper::isValidName(next_element))
                    {
                        type_name.append(" ");
                        type_name.append(next_element);
                    }
                    else
                    {
                        addErrorDescription(std::string("Unexpected string ").append(type_name), pos);
                        HeaderParserHelper::exitBlock(pos);
                        break;
                    }
                }
                const HeaderType* type = findKnownType(type_name);
                if (type != NULL)
                {
                    // Type successfully identified, try to extract member name
                    next_element = HeaderParserHelper::getNextCodeElement(pos);
                    // No pointer support yet
                    if (next_element == "*")
                    {
                        addErrorDescription("No pointers supported yet", pos);
                        HeaderParserHelper::exitBlock(pos);
                        break;
                    }
                    if (!HeaderParserHelper::isValidName(next_element))
                    {
                        addErrorDescription(std::string("Invalid member name ").append(next_element), pos);
                        HeaderParserHelper::exitBlock(pos);
                        break;
                    }
                    std::string element_name = next_element;
                    // try to extract array info
                    size_t len = 1;
                    next_element = HeaderParserHelper::getNextCodeElement(pos);
                    if (next_element == "[")
                    {
                        next_element = HeaderParserHelper::getNextCodeElement(pos);
                        if (!a_util::strings::isInt64(next_element))
                        {
                            // Is it a constant`?
                            if (HeaderParserHelper::isValidName(next_element))
                            {
                                bool found = false;
                                HeaderConstants constants = _header->getConstants();
                                for (HeaderConstants::const_iterator iter = constants.begin(); iter != constants.end(); iter++)
                                {
                                    if ((*iter)->getName() == next_element)
                                    {
                                        found = true;
                                        if ((*iter)->asInt64() > 0)
                                        {
                                            len = (*iter)->asInt64();
                                        }
                                        else
                                        {
                                            addErrorDescription("Array size type is not int", pos);
                                            HeaderParserHelper::exitBlock(pos);
                                            break;
                                        }
                                        break;
                                    }
                                }
                                if (!found)
                                {
                                    addErrorDescription("Array size value could not be found", pos);
                                    HeaderParserHelper::exitBlock(pos);
                                    break;
                                }
                                next_element = HeaderParserHelper::getNextCodeElement(pos);
                                if (next_element != "]")
                                {
                                    addErrorDescription(std::string("Unexpected string ").append(next_element), pos);
                                }
                                next_element = HeaderParserHelper::getNextCodeElement(pos);
                                if (next_element != ";")
                                {
                                    addErrorDescription(std::string("Unexpected string ").append(next_element), pos);
                                }
                            }
                            else
                            {
                                addErrorDescription(std::string("Invalid array size ").append(next_element), pos);
                                HeaderParserHelper::exitBlock(pos);
                                break;
                            }
                        }
                        else
                        {
                            len = a_util::strings::toInt32(next_element);
                            next_element = HeaderParserHelper::getNextCodeElement(pos);
                            if (next_element != "]")
                            {
                                addErrorDescription(std::string("Unexpected string ").append(next_element), pos);
                            }
                            next_element = HeaderParserHelper::getNextCodeElement(pos);
                            if (next_element != ";")
                            {
                                addErrorDescription(std::string("Unexpected string ").append(next_element), pos);
                            }
                        }
                    }
                    else if (next_element != ";")
                    {
                        addErrorDescription(std::string("Missing ; at end of struct member"), pos);
                        HeaderParserHelper::exitBlock(pos);
                        break;
                    }
                    HeaderStructElement* element = new HeaderStructElement();
                    element->setType(type);
                    element->setName(element_name);
                    element->setArraySize(len);
                    // No pointer element recognition implemented yet
                    element->setIsPointer(false);
                    // No static element recognition implemented yet
                    element->setIsStatic(false);
                    // No const element recognition implemented yet
                    element->setIsConst(false);
                    elements.push_back(element);
                }
                else
                {
                    addErrorDescription(std::string("Unknown type name: ").append(type_name), pos);
                    do 
                    {
                        next_element = HeaderParserHelper::getNextCodeElement(pos);
                    } while (next_element != ";" && next_element != "}");
                }
            }
        }
        return elements;
    }

    a_util::result::Result HeaderImporter::addErrorDescription(const std::string &description, const char* pos)
    {
        const char* search_pos = _header_source.c_str();
        uint64_t line_number = 1;
        while (*search_pos != '\0')
        {
            if (search_pos >= pos)
            {
                break;
            }
            else if (*search_pos == '\n')
            {
                line_number++;
            }
            search_pos++;
        }
        if (_last_error.size() != 0)
        {
            _last_error.append("\n");
        }
        _last_error.append(a_util::strings::format("Error: \"%s\" at Line %d", description.c_str(), line_number));
        return ERR_NOERROR;
    }


    a_util::result::Result HeaderImporter::parseTypedef(const char* &pos, std::string &token, std::string &original_type_name, std::string& new_type_name)
    {
        // We found a possible candidate for a new typedef
        // We can have the following cases:
        // 1 typedef token{...
        // 2 typedef token {...
        // 3 typedef token Bla{...
        // 4 typedef token Bla {...
        // 5 typedef token Bla Blubb;
        // 6 typedef Bla Blubb;
        // We are looking only for the last two.
        const char* working_pos = pos;
        std::string local_token;
        // Check if first word if its a token
        std::string next_word = HeaderParserHelper::getNextCodeElement(working_pos);
        for (uint64_t idx = 0; idx < AMOUNT_OF_TOKENS; idx++)
        {
            if (g_tokens[idx] == next_word)
            {
                local_token = next_word;
                next_word = HeaderParserHelper::getNextCodeElement(working_pos);
                break;
            }
        }
        // Now there have to be two valid names and one semicolon. Lets check it out!
        if (HeaderParserHelper::isValidName(next_word))
        {
            std::string local_original_type_name = next_word;
            next_word = HeaderParserHelper::getNextCodeElement(working_pos);
            if (HeaderParserHelper::isValidName(next_word))
            {
                std::string local_new_type_name = next_word;
                next_word = HeaderParserHelper::getNextCodeElement(working_pos);
                if (next_word.compare(";") == 0)
                {
                    // Yes!!! we found a valid typedef, case #5 or #6
                    new_type_name = local_new_type_name;
                    original_type_name = local_original_type_name;
                    token = local_token;
                    pos = working_pos;
                    return ERR_NOERROR;
                }
            }
        }
        return ERR_UNKNOWN_FORMAT;
    }

    HeaderEnumValueMap HeaderImporter::extractEnumValues(const char* &pos)
    {
        std::string next_element;
        HeaderEnumValueMap values;
        int32_t next_idx = 0;

        while (*pos != '\0')
        {
            next_element = HeaderParserHelper::getNextCodeElement(pos);
            if (next_element.compare("}") == 0)
            {
                break;
            }
            // Extract type
            std::string value_name = next_element;
            if (!HeaderParserHelper::isValidName(value_name))
            {
                addErrorDescription(std::string("Unexpected string ").append(value_name), pos);
            }
            else
            {
                next_element = HeaderParserHelper::getNextCodeElement(pos);
                if (next_element == "," || next_element == "}")
                {
                    values[next_idx++] = value_name;
                }
                else if (next_element == "=")
                {
                    std::string index = HeaderParserHelper::getNextCodeElement(pos);
                    if (index == "-")
                    {
                        index += HeaderParserHelper::getNextCodeElement(pos);
                    }
                    int32_t idx = a_util::strings::toInt32(index);
                    if (values.count(idx) > 0)
                    {
                        addErrorDescription(std::string("Duplicate value in enum ").append(index), pos);
                    }
                    else
                    {
                        values[idx] = value_name;
                        next_idx = (std::max)(++next_idx, ++idx); //extra parantheses since msvc defines a max macro somewhere...
                    }

                    next_element = HeaderParserHelper::getNextCodeElement(pos);
                    if (next_element != "," && next_element != "}")
                    {
                        addErrorDescription(std::string("Unexpected token ").append(next_element), pos);
                    }
                }
                else
                {
                    addErrorDescription(std::string("Unexpected token ").append(next_element), pos);
                }

                if (next_element == "}")
                {
                    break;
                }
            }
        }
        return values;
    }

} //namespace ddl
