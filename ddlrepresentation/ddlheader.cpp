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

#include "ddlheader.h"

#include "ddlextdeclaration.h"
#include "ddldescription.h"

namespace ddl
{
    //define all needed error types and values locally
    _MAKE_RESULT(-20, ERR_NOT_FOUND);

    static const std::string strRegExDate [] = {
        std::string("^(\\d{4})(\\d{2})(\\d{2})$"),
        std::string("^(\\d{2})-(\\d{2})-(\\d{4})$"),
        std::string("^(\\d{4})-(\\d{2})-(\\d{2})$"),
        std::string("^(\\d{2})\\.(\\d{2})\\.(\\d{4})$")
    };

    DDLHeader& DDLHeader::operator=(DDLHeader other)
    {
        swap(*this, other);
        return *this;
    }

    DDLHeader::DDLHeader(DDLHeader&& other)
    {
        swap(*this, other);
    }

    DDLHeader::~DDLHeader()
    {
        std::transform(_ddl_ext_declarations.begin(),
            _ddl_ext_declarations.end(), _ddl_ext_declarations.begin(),
            deleteChild<DDLExtDeclaration>);
    }

    DDLHeader::DDLHeader(const DDLVersion& language_version,
        const std::string& strAuthor, a_util::datetime::Date const sDateCreation,
        a_util::datetime::Date const sDateChange, const std::string& strDescription,
        DDLExtDeclarationVec vecExtDecls) :
    _language_version(language_version), _author(strAuthor),
        _date_creation(sDateCreation), _date_change(sDateChange),
        _description(strDescription), _ddl_ext_declarations(vecExtDecls),
        _init_flag(true)
    {
    }

    a_util::result::Result DDLHeader::create(DDLVersion& language_version,
        const std::string& strAuthor, a_util::datetime::Date const sDateCreation,
        a_util::datetime::Date const sDateChange, const std::string& strDescription,
        DDLExtDeclarationVec vecExtDecls)
    {
        _language_version = language_version;
        _author = strAuthor;
        _date_creation = sDateCreation;
        _date_change = sDateChange;
        _description = strDescription;
        _ddl_ext_declarations = vecExtDecls;
        _init_flag = true;
        return a_util::result::SUCCESS;
    }

    a_util::datetime::Date DDLHeader::dateFromString(const std::string& strDate)
    {
        int32_t nDay = 0;
        int32_t nMonth = 0;
        int32_t nYear = 0;
        a_util::datetime::Date sDate(1900, 1, 1);

        std::string arg1, arg2, arg3;
        a_util::regex::RegularExpression oRegExDate(strRegExDate[0]);
        if (oRegExDate.fullMatch(strDate, arg1, arg2, arg3))
        {
            nYear = a_util::strings::toInt32(arg1);
            nMonth = a_util::strings::toInt32(arg2);
            nDay = a_util::strings::toInt32(arg3);
            sDate = a_util::datetime::Date(nYear, nMonth, nDay);
        }
        oRegExDate.setPattern(strRegExDate[1]);
        if (oRegExDate.fullMatch(strDate, arg1, arg2, arg3))
        {
            nDay = a_util::strings::toInt32(arg1);
            nMonth = a_util::strings::toInt32(arg2);
            nYear = a_util::strings::toInt32(arg3);
            sDate = a_util::datetime::Date(nYear, nMonth, nDay);
        }
        oRegExDate.setPattern(strRegExDate[2]);
        if (oRegExDate.fullMatch(strDate, arg1, arg2, arg3))
        {
            nYear = a_util::strings::toInt32(arg1);
            nMonth = a_util::strings::toInt32(arg2);
            nDay = a_util::strings::toInt32(arg3);
            sDate = a_util::datetime::Date(nYear, nMonth, nDay);
        }
        oRegExDate.setPattern(strRegExDate[3]);
        if (oRegExDate.fullMatch(strDate, arg1, arg2, arg3))
        {
            nDay = a_util::strings::toInt32(arg1);
            nMonth = a_util::strings::toInt32(arg2);
            nYear = a_util::strings::toInt32(arg3);
            sDate = a_util::datetime::Date(nYear, nMonth, nDay);
        }

        return sDate;
    }

    a_util::result::Result DDLHeader::accept(IDDLVisitor *poVisitor) const
    {
        return poVisitor->visit(this);
    }
    a_util::result::Result DDLHeader::accept(IDDLChangeVisitor *poVisitor)
    {
        return poVisitor->visit(this);
    }

    const std::string& DDLHeader::getName() const
    {
        static std::string s_Name("Header");
        return s_Name;
    }

    bool DDLHeader::isInitialized() const
    {
        return _init_flag;
    }

    DDLVersion DDLHeader::getLanguageVersion() const
    {
        return _language_version;
    }

    void DDLHeader::setLanguageVersion(const DDLVersion& language_version)
    {
        _language_version = language_version;
    }

    std::string DDLHeader::getAuthor() const
    {
        return _author;
    }

    void DDLHeader::setAuthor(const std::string& strAuthor)
    {
        _author = strAuthor;
    }

    a_util::datetime::Date DDLHeader::getDateCreation() const
    {
        return _date_creation;
    }

    a_util::datetime::Date DDLHeader::getDateChange() const
    {
        return _date_change;
    }

    void DDLHeader::setDateChange(a_util::datetime::Date const sDateChange)
    {
        _date_change = sDateChange;
    }

    void DDLHeader::setDateCreation(a_util::datetime::Date const sDateCreation)
    {
        _date_creation = sDateCreation;
    }

    std::string DDLHeader::getDescription() const
    {
        return _description;
    }

    void DDLHeader::setDescription(const std::string& strDescription)
    {
        _description = strDescription;
    }

    void DDLHeader::setExtDeclarations(DDLExtDeclarationVec vecDDLExtDeclarations)
    {
        _ddl_ext_declarations = vecDDLExtDeclarations;
    }

    void DDLHeader::addExtDeclaration(DDLExtDeclaration * poExtDeclaration, int nPos)
    {
        if (NULL != poExtDeclaration)
        {
            const DDLExtDeclarationVec::size_type szPos =
                static_cast<DDLExtDeclarationVec::size_type>(nPos);
            if (0 <= szPos && szPos < _ddl_ext_declarations.size())
            {
                _ddl_ext_declarations.insert(_ddl_ext_declarations.begin() + szPos,
                                               poExtDeclaration);
            }
            else
            {
                _ddl_ext_declarations.push_back(poExtDeclaration);
            }
        }
    }

    a_util::result::Result DDLHeader::removeExtDeclaration(const std::string& strKey)
    {
        DDLExtDeclarationIt oIt = _ddl_ext_declarations.begin();
        while (oIt != _ddl_ext_declarations.end())
        {
            if ((*oIt)->getKey() == strKey)
            {
                DDLDescription::deleteChild(*oIt);
                _ddl_ext_declarations.erase(oIt);
                return a_util::result::SUCCESS;
            }
            oIt++;
        }
        return ERR_NOT_FOUND;
    }

    const DDLExtDeclarationVec& DDLHeader::getExtDeclarations() const
    {
        return _ddl_ext_declarations;
    }

    DDLExtDeclarationVec& DDLHeader::getExtDeclarations()
    {
        return _ddl_ext_declarations;
    }

    void swap(DDLHeader& lhs, DDLHeader& rhs) noexcept
    {
        using std::swap;
        swap(lhs._language_version, rhs._language_version);
        swap(lhs._author, rhs._author);
        swap(lhs._date_creation, rhs._date_creation);
        swap(lhs._date_change, rhs._date_change);
        swap(lhs._description, rhs._description);
        swap(lhs._ddl_ext_declarations, rhs._ddl_ext_declarations);
        swap(lhs._init_flag, rhs._init_flag);
    }

}   // namespace ddl
