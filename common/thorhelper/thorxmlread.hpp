/*##############################################################################

    Copyright (C) 2011 HPCC Systems.

    All rights reserved. This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
############################################################################## */

#ifndef THORXMLREAD_HPP
#define THORXMLREAD_HPP

#ifdef _WIN32
 #ifdef THORHELPER_EXPORTS
  #define thorhelper_decl __declspec(dllexport)
 #else
  #define thorhelper_decl __declspec(dllimport)
 #endif
#else
 #define thorhelper_decl
#endif

#include "eclhelper.hpp"
#include "jptree.hpp"
#include "thorhelper.hpp"
#include "csvsplitter.hpp"

class thorhelper_decl XmlColumnProvider : public CInterface, implements IColumnProvider
{
public:
    IMPLEMENT_IINTERFACE;

    void setRow(IPropertyTree * _row)       { row.set(_row); }

protected:
    Owned<IPropertyTree> row;
};

class thorhelper_decl XmlDatasetColumnProvider : public XmlColumnProvider
{
public:
    IMPLEMENT_IINTERFACE

//IColumnProvider
    virtual bool        getBool(const char * name);
    virtual void        getData(size32_t len, void * text, const char * name);
    virtual void        getDataX(size32_t & len, void * & text, const char * name);
    virtual __int64     getInt(const char * name);
    virtual void        getQString(size32_t len, char * text, const char * name);
    virtual void        getString(size32_t len, char * text, const char * name);
    virtual void        getStringX(size32_t & len, char * & text, const char * name);
    virtual void        getUnicodeX(size32_t & len, UChar * & text, const char * name);
    virtual void        getUtf8X(size32_t & len, char * & text, const char * path);
    virtual bool        getIsSetAll(const char * path);
    virtual IColumnProviderIterator * getChildIterator(const char * path);

    virtual bool        readBool(const char * path, bool _default);
    virtual void        readData(size32_t len, void * text, const char * path, size32_t _lenDefault, const void * _default);
    virtual void        readDataX(size32_t & len, void * & text, const char * path, size32_t _lenDefault, const void * _default);
    virtual __int64     readInt(const char * path, __int64 _default);
    virtual void        readQString(size32_t len, char * text, const char * path, size32_t _lenDefault, const char * _default);
    virtual void        readString(size32_t len, char * text, const char * path, size32_t _lenDefault, const char * _default);
    virtual void        readStringX(size32_t & len, char * & text, const char * path, size32_t _lenDefault, const char * _default);
    virtual void        readUnicodeX(size32_t & len, UChar * & text, const char * path, size32_t _lenDefault, const UChar * _default);
    virtual bool        readIsSetAll(const char * path, bool _default);
    virtual void        readUtf8X(size32_t & len, char * & text, const char * path, size32_t _lenDefault, const char * _default);

    virtual void        getDataRaw(size32_t len, void * text, const char * name);
    virtual void        getDataRawX(size32_t & len, void * & text, const char * name);
    virtual void        readDataRaw(size32_t len, void * text, const char * path, size32_t _lenDefault, const void * _default);
    virtual void        readDataRawX(size32_t & len, void * & text, const char * path, size32_t _lenDefault, const void * _default);

};

class thorhelper_decl XmlColumnIterator : public CInterface, implements IColumnProviderIterator
{
public:
    XmlColumnIterator(IPropertyTreeIterator * _iter);
    IMPLEMENT_IINTERFACE;

    virtual IColumnProvider * first();
    virtual IColumnProvider * next();

protected:
    void setCurrent();

protected:
    Owned<IPropertyTreeIterator> iter;
    Linked<XmlDatasetColumnProvider> cur;
};

class thorhelper_decl XmlSetColumnProvider : public XmlColumnProvider
{
public:
    IMPLEMENT_IINTERFACE

//IColumnProvider
    virtual bool        getBool(const char * name);
    virtual void        getData(size32_t len, void * text, const char * name);
    virtual void        getDataX(size32_t & len, void * & text, const char * name);
    virtual __int64     getInt(const char * name);
    virtual void        getQString(size32_t len, char * text, const char * name);
    virtual void        getString(size32_t len, char * text, const char * name);
    virtual void        getStringX(size32_t & len, char * & text, const char * name);
    virtual void        getUnicodeX(size32_t & len, UChar * & text, const char * name);
    virtual void        getUtf8X(size32_t & len, char * & text, const char * path);
    virtual bool        getIsSetAll(const char * path);
    virtual IColumnProviderIterator * getChildIterator(const char * path);

    virtual bool        readBool(const char * path, bool _default);
    virtual void        readData(size32_t len, void * text, const char * path, size32_t _lenDefault, const void * _default);
    virtual void        readDataX(size32_t & len, void * & text, const char * path, size32_t _lenDefault, const void * _default);
    virtual __int64     readInt(const char * path, __int64 _default);
    virtual void        readQString(size32_t len, char * text, const char * path, size32_t _lenDefault, const char * _default);
    virtual void        readString(size32_t len, char * text, const char * path, size32_t _lenDefault, const char * _default);
    virtual void        readStringX(size32_t & len, char * & text, const char * path, size32_t _lenDefault, const char * _default);
    virtual void        readUnicodeX(size32_t & len, UChar * & text, const char * path, size32_t _lenDefault, const UChar * _default);
    virtual bool        readIsSetAll(const char * path, bool _default);
    virtual void        readUtf8X(size32_t & len, char * & text, const char * path, size32_t _lenDefault, const char * _default);

    virtual void        getDataRaw(size32_t len, void * text, const char * name);
    virtual void        getDataRawX(size32_t & len, void * & text, const char * name);
    virtual void        readDataRaw(size32_t len, void * text, const char * path, size32_t _lenDefault, const void * _default);
    virtual void        readDataRawX(size32_t & len, void * & text, const char * path, size32_t _lenDefault, const void * _default);

};

class thorhelper_decl CXmlToRawTransformer : public CInterface, implements IXmlToRawTransformer
{
public:
    CXmlToRawTransformer(IXmlToRowTransformer & _rowTransformer, bool _stripWhitespace)
    : rowTransformer(&_rowTransformer), stripWhitespace(_stripWhitespace)
    {
    }
    IMPLEMENT_IINTERFACE

    virtual IDataVal & transform(IDataVal & result, size32_t len, const void * text, bool isDataset);
    virtual IDataVal & transformTree(IDataVal & result, IPropertyTree &tree, bool isDataset);

protected:
    Linked<IXmlToRowTransformer> rowTransformer;
    bool stripWhitespace;
};

class thorhelper_decl CCsvToRawTransformer : public CInterface, implements ICsvToRawTransformer
{
public:
    CCsvToRawTransformer(ICsvToRowTransformer & _rowTransformer) { rowTransformer.set(&_rowTransformer); }
    IMPLEMENT_IINTERFACE

    virtual IDataVal & transform(IDataVal & result, size32_t len, const void * text, bool isDataset);

protected:
    Owned<ICsvToRowTransformer> rowTransformer;
};

#if 0
class thorhelper_decl CSVColumnProvider : public CInterface, implements IColumnProvider
{
    CSVSplitter csvSplitter;

public:
    IMPLEMENT_IINTERFACE;
    inline size32_t splitLine(size32_t maxLen, const byte * start) { return csvSplitter.splitLine(maxLen, start); }
    
    virtual void        getData(size32_t & len, void * & text, const char * name) { UNIMPLEMENTED; }
    virtual __int64     getInt(const char * name) { UNIMPLEMENTED; }
    virtual void        getString(size32_t & len, char * & text, const char * name) { UNIMPLEMENTED; }
    virtual void        getUnicode(size32_t & len, UChar * & text, const char * name) { UNIMPLEMENTED; }
};


class thorhelper_decl CCsvToRawTransformer : public CInterface, implements IXmlToRawTransformer
{
public:
    CCsvToRawTransformer(IXmlToRowTransformer & _rowTransformer) { rowTransformer.set(&_rowTransformer); }
    IMPLEMENT_IINTERFACE

    virtual IDataVal & transform(IDataVal & result, size32_t len, const void * text, bool isDataset);

protected:
    Owned<IXmlToRowTransformer> rowTransformer;
    CSVColumnProvider csvSplitter;
};
#endif
extern thorhelper_decl IXmlToRawTransformer * createXmlRawTransformer(IXmlToRowTransformer * xmlTransformer, bool stripWhitespace);
extern thorhelper_decl ICsvToRawTransformer * createCsvRawTransformer(ICsvToRowTransformer * csvTransformer);


#ifndef CHEAP_UCHAR_DEF
#include "unicode/utf.h"
#endif

interface IXMLSelect : extends IInterface
{
    virtual void match(IColumnProvider &entry, offset_t startOffset, offset_t endOffset) = 0;
};

interface IXMLParse : extends IInterface
{
    virtual bool next() = 0;
    virtual void reset() = 0;
};
thorhelper_decl IXMLParse *createXMLParse(const char *filename, const char *xpath, IXMLSelect &iselect, XmlReaderOptions xmlOptions=xr_none, bool contentRequired=true);
thorhelper_decl IXMLParse *createXMLParse(ISimpleReadStream &stream, const char *xpath, IXMLSelect &iselect, XmlReaderOptions xmlOptions=xr_none, bool contentRequired=true);
thorhelper_decl IXMLParse *createXMLParse(const void *buffer, unsigned bufLen, const char *xpath, IXMLSelect &iselect, XmlReaderOptions xmlOptions=xr_none, bool contentRequired=true);
thorhelper_decl IXMLParse *createXMLParseString(const char *str, const char *xpath, IXMLSelect &iselect, XmlReaderOptions xmlOptions=xr_none, bool contentRequired=true);

thorhelper_decl size32_t createRowFromXml(ARowBuilder & rowBuilder, size32_t size, const char * utf8, IXmlToRowTransformer * xmlTransformer, bool stripWhitespace);
thorhelper_decl const void * createRowFromXml(IEngineRowAllocator * rowAllocator, size32_t len, const char * utf8, IXmlToRowTransformer * xmlTransformer, bool stripWhitespace);

#endif // THORXMLREAD_HPP