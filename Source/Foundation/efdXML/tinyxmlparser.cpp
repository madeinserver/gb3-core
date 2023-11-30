/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2002 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/

/*
    MODIFIED BY:  Emergent Game Technologies, Inc.
    12 May 2008:
        - Added SAX parsing capability.
    15 Dec 2008:
        - Place all TinyXML symbols in the efd namespace.
    6 Jan 2009:
        - Converted all raw assert calls to EE_ASSERT
    11 Feb 2009:
        - Modified macro's to prepend EE_
    24 Feb 2009:
        - Modified tabs to spaces.
    12 Jan 2010:
        - Added support for the "xml-stylesheet" tag via the TiXmlStylesheet::Parse functions
*/


#include <ctype.h>
#include <stddef.h>

#include "tinyxml.h"

//#define DEBUG_PARSER
#if defined( DEBUG_PARSER )
#    if defined( DEBUG ) && defined( _MSC_VER )
#        include <windows.h>
#        define EE_TIXML_LOG OutputDebugString
#    else
#        define EE_TIXML_LOG printf
#    endif
#endif

using namespace efd;

// Note tha "PutString" hardcodes the same list. This
// is less flexible than it appears. Changing the entries
// or order will break putstring.    
TiXmlBase::Entity TiXmlBase::entity[ NUM_ENTITY ] = 
{
    { "&amp;",  5, '&' },
    { "&lt;",   4, '<' },
    { "&gt;",   4, '>' },
    { "&quot;", 6, '\"' },
    { "&apos;", 6, '\'' }
};

// Bunch of unicode info at:
//        http://www.unicode.org/faq/utf_bom.html
// Including the basic of this table, which determines the #bytes in the
// sequence from the lead byte. 1 placed for invalid sequences --
// although the result will be junk, pass it through as much as possible.
// Beware of the non-characters in UTF-8:    
//                ef bb bf (Microsoft "lead bytes")
//                ef bf be
//                ef bf bf 

const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

const int TiXmlBase::utf8ByteTable[256] = 
{
    //    0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x00
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x10
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x20
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x30
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x40
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x50
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x60
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x70    End of ASCII range
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x80 0x80 to 0xc1 invalid
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0x90 
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0xa0 
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    // 0xb0 
        1,    1,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    // 0xc0 0xc2 to 0xdf 2 byte
        2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    // 0xd0
        3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    // 0xe0 0xe0 to 0xef 3 byte
        4,    4,    4,    4,    4,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1    // 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};


void TiXmlBase::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (input < 0x80) 
        *length = 1;
    else if ( input < 0x800 )
        *length = 2;
    else if ( input < 0x10000 )
        *length = 3;
    else if ( input < 0x200000 )
        *length = 4;
    else
        { *length = 0; return; }    // This code won't covert this correctly anyway.

    output += *length;

    // Scary scary fall throughs.
    switch (*length) 
    {
        case 4:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 3:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 2:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 1:
            --output; 
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
    }
}


/*static*/ int TiXmlBase::IsAlpha( unsigned char anyByte, TiXmlEncoding /*encoding*/ )
{
    // This will only work for low-ascii, everything else is assumed to be a valid
    // letter. I'm not sure this is the best approach, but it is quite tricky trying
    // to figure out alhabetical vs. not across encoding. So take a very 
    // conservative approach.

//    if ( encoding == TIXML_ENCODING_UTF8 )
//    {
        if ( anyByte < 127 )
            return isalpha( anyByte );
        else
            return 1;    // What else to do? The unicode set is huge...get the english ones right.
//    }
//    else
//    {
//        return isalpha( anyByte );
//    }
}


/*static*/ int TiXmlBase::IsAlphaNum( unsigned char anyByte, TiXmlEncoding /*encoding*/ )
{
    // This will only work for low-ascii, everything else is assumed to be a valid
    // letter. I'm not sure this is the best approach, but it is quite tricky trying
    // to figure out alhabetical vs. not across encoding. So take a very 
    // conservative approach.

//    if ( encoding == TIXML_ENCODING_UTF8 )
//    {
        if ( anyByte < 127 )
            return isalnum( anyByte );
        else
            return 1;    // What else to do? The unicode set is huge...get the english ones right.
//    }
//    else
//    {
//        return isalnum( anyByte );
//    }
}

/*
    MODIFIED:  12 May 2008.  Added SAX parsing capability.
    BY:  Emergent Game Technologies, Inc.
        This class has been enhanced to include management for buffered file access during all
        (SAX/DOM mode) parsing.

        The original method was to read an entire XML file into memory, then parse it from the
        buffer.  The new method pre-loads data into a buffer of fixed size and reuses the heap
        space as the parser reads out the data.
*/
TiXmlParsingData::TiXmlParsingData(const char*, int _tabsize, int row, int col)
:   m_buffer(NULL)
,   m_readpos(NULL)
,   m_file(NULL)
{
    tabsize = _tabsize;
    cursor.row = row;
    cursor.col = col;
}

TiXmlParsingData::~TiXmlParsingData()
{
    if (m_buffer)
        delete[] m_buffer;
}

void TiXmlParsingData::NewLine()
{
    cursor.row++;
    cursor.col = 0;

    EE_ASSERT( cursor.row >= -1 );
}

char* TiXmlParsingData::setFile (FILE* file)
{
    m_file = file;

    if (m_buffer)
    {
        delete[] m_buffer;
        m_buffer = NULL;
    }

    m_readpos = NULL;
    fill (m_readpos);
    return m_readpos;
}

char* TiXmlParsingData::fill (const char* current_read_pos)
{
    m_readpos = const_cast<char*> (current_read_pos);

    // We may not be reading from a file. If the reader is a string,
    // then setFile() will not be called.
    if (!m_file)
        return m_readpos;

    // On the first call to fill, the buffer and readpos are empty (0)
    if (!m_readpos)
    {
        // Allocate and fill a buffer.
        m_buffer = new char[PARSER_DATA_BUFFER_SZ+1];
        memset (m_buffer, 0, PARSER_DATA_BUFFER_SZ+1);

        size_t size = fread (m_buffer, 1, PARSER_DATA_BUFFER_SZ, m_file);

        if (size > 0)
            m_buffer[size] = 0;
        else
            m_buffer[0] = 0;

        // Set read position to the start of the buffer.
        m_readpos = m_buffer;
    }
    else if (m_readpos > (m_buffer + (3*PARSER_DATA_BUFFER_SZ/4)))
    {
        size_t size = m_buffer+PARSER_DATA_BUFFER_SZ-m_readpos;
        char* end;

        // Create a new buffer, since current buffer is 3/4 exhausted.
        char* buf = new char[PARSER_DATA_BUFFER_SZ+1];
        memset (buf, 0, PARSER_DATA_BUFFER_SZ+1);

        // Copy remaining data from old buffer.
        memcpy (buf, m_readpos, size);

        // Fill rest of buffer with new data from file.
        end = buf + size;
        size = fread (end, 1, PARSER_DATA_BUFFER_SZ-size, m_file);

        if (size > 0)
            end += size;
        *end = 0;

        // Free the old buffer.
        delete[] m_buffer;

        // Set read position to the start of the new buffer.
        m_buffer = buf;
        m_readpos = buf;
    }

    return m_readpos;
}

const char* TiXmlBase::SkipWhiteSpace( const char* p, TiXmlEncoding encoding )
{
    TiXmlParsingData* data = (TiXmlParsingData*) GetUserData();

    p = data->fill (p);
    
    if ( !p || !*p )
    {
        return 0;
    }
    if ( encoding == TIXML_ENCODING_UTF8 )
    {
        while ( *p )
        {
            const unsigned char* pU = (const unsigned char*)p;
            
            // Skip the stupid Microsoft UTF-8 Byte order marks
            if (    *(pU+0)==TIXML_UTF_LEAD_0
                 && *(pU+1)==TIXML_UTF_LEAD_1 
                 && *(pU+2)==TIXML_UTF_LEAD_2 )
            {
                p += 3;
                continue;
            }
            else if(*(pU+0)==TIXML_UTF_LEAD_0
                 && *(pU+1)==0xbfU
                 && *(pU+2)==0xbeU )
            {
                p += 3;
                continue;
            }
            else if(*(pU+0)==TIXML_UTF_LEAD_0
                 && *(pU+1)==0xbfU
                 && *(pU+2)==0xbfU )
            {
                p += 3;
                continue;
            }

            if ( IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' )        // Still using old rules for white space.
            {
                if (*p == '\n' || *p =='\r')
                    data->NewLine();
                ++p;
            }
            else
                break;
        }
    }
    else
    {
        /*
            MODIFIED: 4 Jan 2009.  Added () around && to get rid of warning in gcc 4.3.2.
            BY:  Emergent Game Technologies, Inc.
        */
        while ( (*p && IsWhiteSpace( *p )) || *p == '\n' || *p =='\r' )
        {
            if (*p == '\n' || *p =='\r')
                data->NewLine();

            ++p;
        }
    }

    p = data->fill (p);
    return p;
}

/*
    MODIFIED:  8 Oct 2008.  Added function that preserves "with preceding white space" pointer.
    BY:  Emergent Game Technologies, Inc.
        Function preserves a "with preceding white space" pointer (i.e., "pWithWhiteSpace"),
        even if "p" and "pWithWhiteSpace" both change as part of reading in and appending
        another buffer of data from a file, as a side effect.
*/
const char* TiXmlBase::SkipWhiteSpace_PreserveNonSkipped(const char* p, TiXmlEncoding encoding,
    const char*& pWithWhiteSpace)
{
    TiXmlParsingData* data = (TiXmlParsingData*) GetUserData();

    p = data->fill (p);

    // Reset pWithWhiteSpace, in case fill() changes p as a part of reallocation side effect.
    pWithWhiteSpace = p;
    
    if (!p || !*p)
        return 0;

    if ( encoding == TIXML_ENCODING_UTF8 )
    {
        while ( *p )
        {
            const unsigned char* pU = (const unsigned char*)p;
            
            // Skip the Microsoft UTF-8 Byte order marks
            if (   *(pU+0)==TIXML_UTF_LEAD_0
                && *(pU+1)==TIXML_UTF_LEAD_1 
                && *(pU+2)==TIXML_UTF_LEAD_2)
            {
                p += 3;
                continue;
            }
            else if (*(pU+0)==TIXML_UTF_LEAD_0
                  && *(pU+1)==0xbfU
                  && *(pU+2)==0xbeU)
            {
                p += 3;
                continue;
            }
            else if (*(pU+0)==TIXML_UTF_LEAD_0
                  && *(pU+1)==0xbfU
                  && *(pU+2)==0xbfU)
            {
                p += 3;
                continue;
            }

            // Still using old rules for white space.
            if (IsWhiteSpace( *p ) || *p == '\n' || *p =='\r')
            {
                if (*p == '\n' || *p =='\r')
                    data->NewLine();

                ++p;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        /*
            MODIFIED: 4 Jan 2009.  Added () around && to get rid of warning in gcc 4.3.2.
            BY:  Emergent Game Technologies, Inc.
        */
        while ((*p && IsWhiteSpace( *p )) || *p == '\n' || *p =='\r')
        {
            if (*p == '\n' || *p =='\r')
                data->NewLine();

            ++p;
        }
    }

    return p;
}

#ifdef EE_TIXML_USE_STL
/*static*/ bool TiXmlBase::StreamWhiteSpace( std::istream * in, EE_TIXML_STRING * tag )
{
    for( ;; )
    {
        if ( !in->good() ) return false;

        int c = in->peek();
        // At this scope, we can't get to a document. So fail silently.
        if ( !IsWhiteSpace( c ) || c <= 0 )
            return true;

        *tag += (char) in->get();
    }
}

/*static*/ bool TiXmlBase::StreamTo( std::istream * in, int character, EE_TIXML_STRING * tag )
{
    //EE_ASSERT( character > 0 && character < 128 );    // else it won't work in utf-8
    while ( in->good() )
    {
        int c = in->peek();
        if ( c == character )
            return true;
        if ( c <= 0 )        // Silent failure: can't get document at this scope
            return false;

        in->get();
        *tag += (char) c;
    }
    return false;
}
#endif

// One of TinyXML's more performance demanding functions. Try to keep the memory overhead down. The
// "assign" optimization removes over 10% of the execution time.
//
const char* TiXmlBase::ReadName( const char* p, EE_TIXML_STRING * name, TiXmlEncoding encoding )
{
    // Oddly, not supported on some comilers,
    //name->clear();
    // So use this:
    *name = "";
    EE_ASSERT( p );

    // Names start with letters or underscores.
    // Of course, in unicode, tinyxml has no idea what a letter *is*. The
    // algorithm is generous.
    //
    // After that, they can be letters, underscores, numbers,
    // hyphens, or colons. (Colons are valid ony for namespaces,
    // but tinyxml can't tell namespaces from names.)
    if (    p && *p 
         && ( IsAlpha( (unsigned char) *p, encoding ) || *p == '_' ) )
    {
        const char* start = p;
        while(        p && *p
                &&    (        IsAlphaNum( (unsigned char ) *p, encoding ) 
                         || *p == '_'
                         || *p == '-'
                         || *p == '.'
                         || *p == ':' ) )
        {
            //(*name) += *p; // expensive
            ++p;
        }
        if ( p-start > 0 ) {
            name->assign( start, p-start );
        }
        return p;
    }
    return 0;
}

const char* TiXmlBase::GetEntity( const char* p, char* value, int* length, TiXmlEncoding encoding )
{
    // Presume an entity, and pull it out.
    EE_TIXML_STRING ent;
    int i;
    *length = 0;

    if ( *(p+1) && *(p+1) == '#' && *(p+2) )
    {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if ( *(p+2) == 'x' )
        {
            // Hexadecimal.
            if ( !*(p+3) ) return 0;

            const char* q = p+3;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != 'x' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else if ( *q >= 'a' && *q <= 'f' )
                    ucs += mult * (*q - 'a' + 10);
                else if ( *q >= 'A' && *q <= 'F' )
                    ucs += mult * (*q - 'A' + 10 );
                else 
                    return 0;
                mult *= 16;
                --q;
            }
        }
        else
        {
            // Decimal.
            if ( !*(p+2) ) return 0;

            const char* q = p+2;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != '#' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else 
                    return 0;
                mult *= 10;
                --q;
            }
        }
        if ( encoding == TIXML_ENCODING_UTF8 )
        {
            // convert the UCS to UTF-8
            ConvertUTF32ToUTF8( ucs, value, length );
        }
        else
        {
            *value = (char)ucs;
            *length = 1;
        }
        return p + delta + 1;
    }

    // Now try to match it.
    for( i=0; i<NUM_ENTITY; ++i )
    {
        if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
        {
            EE_ASSERT( strlen( entity[i].str ) == entity[i].strLength );
            *value = entity[i].chr;
            *length = 1;
            return ( p + entity[i].strLength );
        }
    }

    // So it wasn't an entity, its unrecognized, or something like that.
    *value = *p;    // Don't put back the last one, since we return it!
    //*length = 1;    // Leave unrecognized entities - this doesn't really work.
                    // Just writes strange XML.
    return p+1;
}


bool TiXmlBase::StringEqual( const char* p,
                             const char* tag,
                             bool ignoreCase,
                             TiXmlEncoding encoding )
{
    EE_ASSERT( p );
    EE_ASSERT( tag );
    if ( !p || !*p )
    {
        EE_ASSERT( 0 );
        return false;
    }

    const char* q = p;

    if ( ignoreCase )
    {
        while ( *q && *tag && ToLower( *q, encoding ) == ToLower( *tag, encoding ) )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )
            return true;
    }
    else
    {
        while ( *q && *tag && *q == *tag )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )        // Have we found the end of the tag, and everything equal?
            return true;
    }
    return false;
}

const char* TiXmlBase::ReadText(    const char* p, 
                                    EE_TIXML_STRING * text, 
                                    bool trimWhiteSpace, 
                                    const char* endTag, 
                                    bool caseInsensitive,
                                    TiXmlEncoding encoding )
{
    *text = "";
    if (    !trimWhiteSpace            // certain tags always keep whitespace
         || !condenseWhiteSpace )    // if true, whitespace is always kept
    {
        // Keep all the white space.
        while (       p && *p
                && !StringEqual( p, endTag, caseInsensitive, encoding )
              )
        {
            int len;
            char cArr[4] = { 0, 0, 0, 0 };
            p = GetChar( p, cArr, &len, encoding );
            /*
                MODIFIED:  8 Feb 2009.  Fill with more data if end of buffer reached.
                BY:  Emergent Game Technologies, Inc.
                    Because of Emergent's system of paging in the XML data, it is possible to 
                    hit the end of the data buffer before running out of actual text.
                    Check for that situation and page in new data as needed.
            */
            if (p != NULL && *p == 0)
            {
                // If we've over
                TiXmlParsingData* data = (TiXmlParsingData*) GetUserData();
                p = data->fill (p);
            }
            text->append( cArr, len );
        }
    }
    else
    {
        bool whitespace = false;

        // Remove leading white space:
        p = SkipWhiteSpace( p, encoding );
        while (       p && *p
                && !StringEqual( p, endTag, caseInsensitive, encoding ) )
        {
            if ( *p == '\r' || *p == '\n' )
            {
                whitespace = true;
                ++p;
            }
            else if ( IsWhiteSpace( *p ) )
            {
                whitespace = true;
                ++p;
            }
            else
            {
                // If we've found whitespace, add it before the
                // new character. Any whitespace just becomes a space.
                if ( whitespace )
                {
                    (*text) += ' ';
                    whitespace = false;
                }
                int len;
                char cArr[4] = { 0, 0, 0, 0 };
                p = GetChar( p, cArr, &len, encoding );
                if ( len == 1 )
                    (*text) += cArr[0];    // more efficient
                else
                    text->append( cArr, len );
            }
        }
    }
    if ( p ) 
        p += strlen( endTag );
    return p;
}

#ifdef EE_TIXML_USE_STL

void TiXmlDocument::StreamIn( std::istream * in, EE_TIXML_STRING * tag )
{
    // The basic issue with a document is that we don't know what we're
    // streaming. Read something presumed to be a tag (and hope), then
    // identify it, and call the appropriate stream method on the tag.
    //
    // This "pre-streaming" will never read the closing ">" so the
    // sub-tag can orient itself.

    if ( !StreamTo( in, '<', tag ) ) 
    {
        SetError( TIXML_ERROR_PARSING_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return;
    }

    while ( in->good() )
    {
        int tagIndex = (int) tag->length();
        while ( in->good() && in->peek() != '>' )
        {
            int c = in->get();
            if ( c <= 0 )
            {
                SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
                break;
            }
            (*tag) += (char) c;
        }

        if ( in->good() )
        {
            // We now have something we presume to be a node of 
            // some sort. Identify it, and call the node to
            // continue streaming.
            TiXmlNode* node = Identify( tag->c_str() + tagIndex, TIXML_DEFAULT_ENCODING );

            if ( node )
            {
                node->StreamIn( in, tag );
                bool isElement = node->ToElement() != 0;
                delete node;
                node = 0;

                // If this is the root element, we're done. Parsing will be
                // done by the >> operator.
                if ( isElement )
                {
                    return;
                }
            }
            else
            {
                SetError( TIXML_ERROR, 0, 0, TIXML_ENCODING_UNKNOWN );
                return;
            }
        }
    }
    // We should have returned sooner.
    SetError( TIXML_ERROR, 0, 0, TIXML_ENCODING_UNKNOWN );
}

#endif

const char* TiXmlDocument::Parse( const char* p, TiXmlParsingData* prevData, TiXmlEncoding encoding )
{
    SetUserData(prevData);
    ClearError();

    // Parse away, at the document level. Since a document
    // contains nothing but other tags, most of what happens
    // here is skipping white space.
    if ( !p || !*p )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return 0;
    }

    location = prevData->Cursor();

    if ( encoding == TIXML_ENCODING_UNKNOWN )
    {
        // Check for the Microsoft UTF-8 lead bytes.
        const unsigned char* pU = (const unsigned char*)p;
        if (    *(pU+0) && *(pU+0) == TIXML_UTF_LEAD_0
             && *(pU+1) && *(pU+1) == TIXML_UTF_LEAD_1
             && *(pU+2) && *(pU+2) == TIXML_UTF_LEAD_2 )
        {
            encoding = TIXML_ENCODING_UTF8;
            useMicrosoftBOM = true;
        }
    }

    p = SkipWhiteSpace( p, encoding );
    if ( !p )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return 0;
    }

    // Start of document parsing for SAX.
    if (saxHandler)
        saxHandler->startDocument();
    
    while ( p && *p )
    {
        TiXmlNode* node = Identify( p, encoding );
        if ( node )
        {
            p = node->Parse(p, prevData, encoding);
            LinkEndChild( node );
        }
        else
        {
            break;
        }

        // Did we get encoding info?
        if (    encoding == TIXML_ENCODING_UNKNOWN
             && node->ToDeclaration() )
        {
            TiXmlDeclaration* dec = node->ToDeclaration();
            const char* enc = dec->Encoding();
            EE_ASSERT( enc );

            if ( *enc == 0 )
                encoding = TIXML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF-8", true, TIXML_ENCODING_UNKNOWN ) )
                encoding = TIXML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF8", true, TIXML_ENCODING_UNKNOWN ) )
                encoding = TIXML_ENCODING_UTF8;    // incorrect, but be nice
            else 
                encoding = TIXML_ENCODING_LEGACY;
        }

        p = SkipWhiteSpace( p, encoding );
    }

    // Was this empty?
    if ( !firstChild ) {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, encoding );
        return 0;
    }

    // All is well.
    // End of document parsing for SAX.
    if (saxHandler)
        saxHandler->endDocument();

    return p;
}

/*
    MODIFIED:  4 Sept 2009.
    Added to support beginning a SAX parsing process that could be interrupted.  The beginning 
    portion can not be interrupted, but it doesn't do anything except initialize the state and 
    read enough of the file to understand header information.
    BY:  Emergent Game Technologies, Inc.
*/
TiXmlBase::SAXResult TiXmlDocument::SAXBeginParse( const char* p, TiXmlEncoding encoding )
{
    // Delete the existing data:
    Clear();
    location.Clear();

    char* dummy = 0;
    m_pData = new TiXmlParsingData(dummy, TabSize(), location.row, location.col);
    m_pData->fill(p);

    SetUserData(m_pData);
    ClearError();

    m_saxp = p;

    // Parse away, at the document level. Since a document
    // contains nothing but other tags, most of what happens
    // here is skipping white space.
    if ( !m_saxp || !*m_saxp )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return TIXML_SAX_ERROR;
    }

    location = m_pData->Cursor();

    if ( encoding == TIXML_ENCODING_UNKNOWN )
    {
        // Check for the Microsoft UTF-8 lead bytes.
        const unsigned char* pU = (const unsigned char*)m_saxp;
        if (   *(pU+0) && *(pU+0) == TIXML_UTF_LEAD_0
            && *(pU+1) && *(pU+1) == TIXML_UTF_LEAD_1
            && *(pU+2) && *(pU+2) == TIXML_UTF_LEAD_2 )
        {
            encoding = TIXML_ENCODING_UTF8;
            useMicrosoftBOM = true;
        }
    }

    m_saxp = SkipWhiteSpace( m_saxp, encoding );
    if ( !m_saxp )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return TIXML_SAX_ERROR;
    }

    // Start of document parsing for SAX.
    if (saxHandler)
        saxHandler->startDocument();

    return TIXML_SAX_PARSING;

}

/*
    MODIFIED:  4 Sept 2009.
    Added to support a SAX parsing that could be interrupted.  This parse call may need to be 
    called multiple times to complete loading a document.
    BY:  Emergent Game Technologies, Inc.
*/
TiXmlBase::SAXResult TiXmlDocument::SAXParse(TiXmlEncoding encoding )
{
    while ( m_saxp && *m_saxp )
    {
        TiXmlNode* node = Identify( m_saxp, encoding );
        if ( node )
        {
            m_saxp = node->Parse(m_saxp, m_pData, encoding);

            if (SAXIsInterruptingParse())
            {
                m_interruptParse = false;
                return TIXML_SAX_INTERRUPTED;
            }

            LinkEndChild( node );

            if (SAXIsInterruptingParse())
            {
                m_interruptParse = false;
                return TIXML_SAX_INTERRUPTED;
            }
        }
        else
        {
            break;
        }

        // Did we get encoding info?
        if (    encoding == TIXML_ENCODING_UNKNOWN
            && node->ToDeclaration() )
        {
            TiXmlDeclaration* dec = node->ToDeclaration();
            const char* enc = dec->Encoding();
            EE_ASSERT( enc );

            if ( *enc == 0 )
                encoding = TIXML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF-8", true, TIXML_ENCODING_UNKNOWN ) )
                encoding = TIXML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF8", true, TIXML_ENCODING_UNKNOWN ) )
                encoding = TIXML_ENCODING_UTF8;    // incorrect, but be nice
            else 
                encoding = TIXML_ENCODING_LEGACY;
        }

        m_saxp = SkipWhiteSpace( m_saxp, encoding );
    }

    // Was this empty?
    if ( !firstChild ) {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, encoding );
        return TIXML_SAX_ERROR;
    }

    // All is well.
    // End of document parsing for SAX.
    if (saxHandler)
        saxHandler->endDocument();

    return TIXML_SAX_DONE;
}

  /*
    MODIFIED:  4 Sept 2009.
    Added to support interrupting a SAX parsing operation.  This should only be used if SAXParse
    and SAXBeginParse are being used to read the file.
    BY:  Emergent Game Technologies, Inc.
*/
void TiXmlDocument::SAXInterruptParse()
{
    m_interruptParse = true;
}

void TiXmlDocument::SetError( int err, const char* pError, TiXmlParsingData* data, TiXmlEncoding )
{    
    // The first error in a chain is more accurate - don't set again!
    if ( error )
        return;

    EE_ASSERT( err > 0 && err < TIXML_ERROR_STRING_COUNT );
    error   = true;
    errorId = err;
    errorDesc = errorString[ errorId ];

    errorLocation.Clear();
    if ( pError && data )
    {
        errorLocation = data->Cursor();
    }
}

/*
    MODIFIED:  11 Oct 2008.  Now pass p as a reference, due to possible side effects in
                             SkipWhiteSpace() that may change p.
    BY:  Emergent Game Technologies, Inc.
*/
TiXmlNode* TiXmlNode::Identify( const char*& p, TiXmlEncoding encoding )
{
    TiXmlNode* returnNode = 0;

    p = SkipWhiteSpace( p, encoding );
    if( !p || !*p || *p != '<' )
    {
        return 0;
    }

    TiXmlDocument* doc = GetDocument();
    p = SkipWhiteSpace( p, encoding );

    if ( !p || !*p )
    {
        return 0;
    }

    // What is this thing? 
    // - Elements start with a letter or underscore, but xml is reserved.
    // - Comments: <!--
    // - Decleration: <?xml
    // - Everthing else is unknown to tinyxml.
    //

    const char* xmlStylesheet = { "<?xml-stylesheet" };
    const char* xmlHeader = { "<?xml" };
    const char* commentHeader = { "<!--" };
    const char* dtdHeader = { "<!" };
    const char* cdataHeader = { "<![CDATA[" };

    if ( StringEqual( p, xmlStylesheet, true, encoding ) )
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing Stylesheet\n" );
        #endif
        returnNode = new TiXmlStylesheet();
    }
    else if ( StringEqual( p, xmlHeader, true, encoding ) )
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing Declaration\n" );
        #endif
        returnNode = new TiXmlDeclaration();
    }
    else if ( StringEqual( p, commentHeader, false, encoding ) )
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing Comment\n" );
        #endif
        returnNode = new TiXmlComment();
    }
    else if ( StringEqual( p, cdataHeader, false, encoding ) )
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing CDATA\n" );
        #endif
        TiXmlText* text = new TiXmlText( "" );
        text->SetCDATA( true );
        returnNode = text;
    }
    else if ( StringEqual( p, dtdHeader, false, encoding ) )
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing Unknown(1)\n" );
        #endif
        returnNode = new TiXmlUnknown();
    }
    else if (    IsAlpha( *(p+1), encoding )
              || *(p+1) == '_' )
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing Element\n" );
        #endif
        returnNode = new TiXmlElement( "" );
    }
    else
    {
        #ifdef DEBUG_PARSER
            EE_TIXML_LOG( "XML parsing Unknown(2)\n" );
        #endif
        returnNode = new TiXmlUnknown();
    }

    if ( returnNode )
    {
        // Set the parent, so it can report errors
        returnNode->parent = this;
    }
    else
    {
        if ( doc )
            doc->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
    }
    
    // Do SAX callbacks.
    TiXmlDefaultHandler* saxHandler = doc->GetSAXHandler();
    if (saxHandler)
    {
        switch ( returnNode->Type() )
        {
            case TiXmlNode::DOCUMENT:
                saxHandler->startDocument();
                break;
        }
    }
        
    return returnNode;
}

#ifdef EE_TIXML_USE_STL

void TiXmlElement::StreamIn (std::istream * in, EE_TIXML_STRING * tag)
{
    // We're called with some amount of pre-parsing. That is, some of "this"
    // element is in "tag". Go ahead and stream to the closing ">"
    while( in->good() )
    {
        int c = in->get();
        if ( c <= 0 )
        {
            TiXmlDocument* document = GetDocument();
            if ( document )
                document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
            return;
        }
        (*tag) += (char) c ;
        
        if ( c == '>' )
            break;
    }

    if ( tag->length() < 3 ) return;

    // Okay...if we are a "/>" tag, then we're done. We've read a complete tag.
    // If not, identify and stream.

    if (    tag->at( tag->length() - 1 ) == '>' 
         && tag->at( tag->length() - 2 ) == '/' )
    {
        // All good!
        return;
    }
    else if ( tag->at( tag->length() - 1 ) == '>' )
    {
        // There is more. Could be:
        //        text
        //        cdata text (which looks like another node)
        //        closing tag
        //        another node.
        for ( ;; )
        {
            StreamWhiteSpace( in, tag );

            // Do we have text?
            if ( in->good() && in->peek() != '<' ) 
            {
                // Yep, text.
                TiXmlText text( "" );
                text.StreamIn( in, tag );

                // What follows text is a closing tag or another node.
                // Go around again and figure it out.
                continue;
            }

            // We now have either a closing tag...or another node.
            // We should be at a "<", regardless.
            if ( !in->good() ) return;
            EE_ASSERT( in->peek() == '<' );
            int tagIndex = (int) tag->length();

            bool closingTag = false;
            bool firstCharFound = false;

            for( ;; )
            {
                if ( !in->good() )
                    return;

                int c = in->peek();
                if ( c <= 0 )
                {
                    TiXmlDocument* document = GetDocument();
                    if ( document )
                        document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
                    return;
                }
                
                if ( c == '>' )
                    break;

                *tag += (char) c;
                in->get();

                // Early out if we find the CDATA id.
                if ( c == '[' && tag->size() >= 9 )
                {
                    size_t len = tag->size();
                    const char* start = tag->c_str() + len - 9;
                    if ( strcmp( start, "<![CDATA[" ) == 0 ) {
                        EE_ASSERT( !closingTag );
                        break;
                    }
                }

                if ( !firstCharFound && c != '<' && !IsWhiteSpace( c ) )
                {
                    firstCharFound = true;
                    if ( c == '/' )
                        closingTag = true;
                }
            }
            // If it was a closing tag, then read in the closing '>' to clean up the input stream.
            // If it was not, the streaming will be done by the tag.
            if ( closingTag )
            {
                if ( !in->good() )
                    return;

                int c = in->get();
                if ( c <= 0 )
                {
                    TiXmlDocument* document = GetDocument();
                    if ( document )
                        document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
                    return;
                }
                EE_ASSERT( c == '>' );
                *tag += (char) c;

                // We are done, once we've found our closing tag.
                return;
            }
            else
            {
                // If not a closing tag, id it, and stream.
                const char* tagloc = tag->c_str() + tagIndex;
                TiXmlNode* node = Identify( tagloc, TIXML_DEFAULT_ENCODING );
                if ( !node )
                    return;
                node->StreamIn( in, tag );
                delete node;
                node = 0;

                // No return: go around from the beginning: text, closing tag, or node.
            }
        }
    }
}
#endif

void TiXmlElement::DoSAXElement()
{
    TiXmlDocument* document = GetDocument();
    if (document)
    {
        TiXmlDefaultHandler* sax = document->GetSAXHandler();
        if (sax)
            sax->startElement (ValueTStr(), attributeSet);
    }
}

const char* TiXmlElement::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    SetUserData(data);
    p = SkipWhiteSpace( p, encoding );
    TiXmlDocument* document = GetDocument();

    if ( !p || !*p )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, 0, 0, encoding );
        return 0;
    }

    if ( *p != '<' )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, p, data, encoding );
        return 0;
    }

    p = SkipWhiteSpace( p+1, encoding );

    // Read the name.
    const char* pErr = p;

    p = ReadName( p, &value, encoding );
    if ( !p || !*p )
    {
        if ( document )    document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME, pErr, data, encoding );
        return 0;
    }

    EE_TIXML_STRING endTag ("</");
    endTag += value;
    endTag += ">";

    // Check for and read attributes. Also look for an empty
    // tag or an end tag.
    while ( p && *p )
    {
        pErr = p;
        p = SkipWhiteSpace( p, encoding );
        if ( !p || !*p )
        {
            if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
            return 0;
        }
        if ( *p == '/' )
        {
            ++p;
            // Empty tag.
            if ( *p  != '>' )
            {
                if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY, p, data, encoding );        
                return 0;
            }
            
            // Invoke SAX callbacks.
            DoSAXElement();
            
            return (p+1);
        }
        else if ( *p == '>' )
        {
            // Invoke SAX callbacks.
            DoSAXElement();

            // Read the value -- which can include other
            // elements -- read the end tag, and return.
            ++p;
            p = ReadValue( p, data, encoding );        // Note this is an Element method, and will set the error if one happens.

            /*
                   MODIFIED:  4 Sept 2009
                   Adding code to support interrupting a parse and jump back up the stack 
                   without altering the state of the p string pointer.
                   BY:  Emergent Game Technologies, Inc.
              */
            if (document->SAXIsInterruptingParse())
                return p;

            if ( !p || !*p ) {
                // We were looking for the end tag, but found nothing.
                // Fix for [ 1663758 ] Failure to report error on bad XML
                if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG, p, data, encoding );
                return 0;
            }

            // We should find the end tag now
            if ( StringEqual( p, endTag.c_str(), false, encoding ) )
            {
                p += endTag.length();

                return p;
            }
            else
            {
                if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG, p, data, encoding );
                return 0;
            }
        }
        else
        {
            // Try to read an attribute:
            TiXmlAttribute* attrib = new TiXmlAttribute();
            if ( !attrib )
            {
                if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, pErr, data, encoding );
                return 0;
            }

            attrib->SetDocument( document );
            pErr = p;
            p = attrib->Parse( p, data, encoding );

            if ( !p || !*p )
            {
                if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, pErr, data, encoding );
                delete attrib;
                return 0;
            }

            // Handle the strange case of double attributes:
            #ifdef EE_TIXML_USE_STL
            TiXmlAttribute* node = attributeSet.Find( attrib->NameTStr() );
            #else
            TiXmlAttribute* node = attributeSet.Find( attrib->Name() );
            #endif
            if ( node )
            {
                node->SetValue( attrib->Value() );
                delete attrib;
                return 0;
            }

            attributeSet.Add( attrib );
        }
    }
    return p;
}


const char* TiXmlElement::ReadValue( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    TiXmlDocument* document = GetDocument();

    // Read in text and elements in any order.
    const char* pWithWhiteSpace = p;
    /*
        MODIFIED:  8 Oct 2008.  Now use function that preserves "with preceding white space" pointer.
        BY:  Emergent Game Technologies, Inc.
            Function preserves a "with preceding white space" pointer (i.e., "pWithWhiteSpace"),
            even if "p" and "pWithWhiteSpace" both change as part of reading in and appending
            another buffer of data from a file, as a side effect.
    */
    p = SkipWhiteSpace_PreserveNonSkipped(p, encoding, pWithWhiteSpace);

    while ( p && *p )
    {
        if ( *p != '<' )
        {
            // Take what we have, make a text element.
            TiXmlText* textNode = new TiXmlText( "" );

            if ( !textNode )
            {
                if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, encoding );
                    return 0;
            }

            if ( TiXmlBase::IsWhiteSpaceCondensed() )
            {
                p = textNode->Parse( p, data, encoding );
            }
            else
            {
                // Special case: we want to keep the white space
                // so that leading spaces aren't removed.
                p = textNode->Parse( pWithWhiteSpace, data, encoding );
            }

            if ( !textNode->Blank() )
                LinkEndChild( textNode );
            else
                delete textNode;
        } 
        else 
        {
            // We hit a '<'
            // Have we hit a new element or an end tag? This could also be
            // a TiXmlText in the "CDATA" style.
            if ( StringEqual( p, "</", false, encoding ) )
            {
                return p;
            }
            else
            {
                TiXmlNode* node = Identify( p, encoding );
                if ( node )
                {
                    p = node->Parse( p, data, encoding );

                    /*
                        MODIFIED:  4 Sept 2009
                        Adding code to support interrupting a parse and jump back up the stack 
                        without altering the state of the p string pointer.
                        BY:  Emergent Game Technologies, Inc.
                    */
                    if (document->SAXIsInterruptingParse())
                        return p;

                    LinkEndChild( node );

                    /*
                        MODIFIED:  4 Sept 2009
                        Adding code to support interrupting a parse and jump back up the stack 
                        without altering the state of the p string pointer.
                        BY:  Emergent Game Technologies, Inc.
                    */
                    if (document->SAXIsInterruptingParse())
                        return p;
                }                
                else
                {
                    return 0;
                }
            }
        }
        pWithWhiteSpace = p;
        p = SkipWhiteSpace_PreserveNonSkipped(p, encoding, pWithWhiteSpace);
    }

    if ( !p )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE, 0, 0, encoding );
    }    
    return p;
}


#ifdef EE_TIXML_USE_STL
void TiXmlUnknown::StreamIn( std::istream * in, EE_TIXML_STRING * tag )
{
    while ( in->good() )
    {
        int c = in->get();    
        if ( c <= 0 )
        {
            TiXmlDocument* document = GetDocument();
            if ( document )
                document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
            return;
        }
        (*tag) += (char) c;

        if ( c == '>' )
        {
            // All is well.
            return;        
        }
    }
}
#endif


const char* TiXmlUnknown::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    SetUserData(data);
    TiXmlDocument* document = GetDocument();
    p = SkipWhiteSpace( p, encoding );

    if ( !p || !*p || *p != '<' )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_UNKNOWN, p, data, encoding );
        return 0;
    }
    ++p;
    value = "";

    while ( p && *p && *p != '>' )
    {
        value += *p;
        ++p;
    }

    if ( !p )
    {
        if ( document )    document->SetError( TIXML_ERROR_PARSING_UNKNOWN, 0, 0, encoding );
    }
    if ( *p == '>' )
        return p+1;
    return p;
}

#ifdef EE_TIXML_USE_STL
void TiXmlComment::StreamIn( std::istream * in, EE_TIXML_STRING * tag )
{
    while ( in->good() )
    {
        int c = in->get();    
        if ( c <= 0 )
        {
            TiXmlDocument* document = GetDocument();
            if ( document )
                document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
            return;
        }

        (*tag) += (char) c;

        if ( c == '>' 
             && tag->at( tag->length() - 2 ) == '-'
             && tag->at( tag->length() - 3 ) == '-' )
        {
            // All is well.
            return;        
        }
    }
}
#endif


const char* TiXmlComment::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    SetUserData(data);
    TiXmlDocument* document = GetDocument();
    value = "";

    p = SkipWhiteSpace( p, encoding );

    const char* startTag = "<!--";
    const char* endTag   = "-->";

    if ( !StringEqual( p, startTag, false, encoding ) )
    {
        document->SetError( TIXML_ERROR_PARSING_COMMENT, p, data, encoding );
        return 0;
    }
    p += strlen( startTag );

    // [ 1475201 ] TinyXML parses entities in comments
    // Oops - ReadText doesn't work, because we don't want to parse the entities.
    // p = ReadText( p, &value, false, endTag, false, encoding );
    //
    // from the XML spec:
    /*
     [Definition: Comments may appear anywhere in a document outside other markup; in addition, 
                  they may appear within the document type declaration at places allowed by the grammar. 
                  They are not part of the document's character data; an XML processor MAY, but need not, 
                  make it possible for an application to retrieve the text of comments. For compatibility, 
                  the string "--" (double-hyphen) MUST NOT occur within comments.] Parameter entity 
                  references MUST NOT be recognized within comments.

                  An example of a comment:

                  <!-- declarations for <head> & <body> -->
    */

    value = "";
    // Keep all the white space.
    while (    p && *p && !StringEqual( p, endTag, false, encoding ) )
    {
        value.append( p, 1 );
        ++p;
    }
    if ( p ) 
        p += strlen( endTag );

    return p;
}


const char* TiXmlAttribute::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    SetUserData(data);
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p ) return 0;

//    int tabsize = 4;
//    if ( document )
//        tabsize = document->TabSize();

    // Read the name, the '=' and the value.
    const char* pErr = p;
    p = ReadName( p, &name, encoding );
    if ( !p || !*p )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
        return 0;
    }
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p || *p != '=' )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
        return 0;
    }

    ++p;    // skip '='
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
        return 0;
    }
    
    const char* end;
    const char SINGLE_QUOTE = '\'';
    const char DOUBLE_QUOTE = '\"';

    if ( *p == SINGLE_QUOTE )
    {
        ++p;
        end = "\'";        // single quote in string
        p = ReadText( p, &value, false, end, false, encoding );
    }
    else if ( *p == DOUBLE_QUOTE )
    {
        ++p;
        end = "\"";        // double quote in string
        p = ReadText( p, &value, false, end, false, encoding );
    }
    else
    {
        // All attribute values should be in single or double quotes.
        // But this is such a common error that the parser will try
        // its best, even without them.
        value = "";
        while (    p && *p                                            // existence
                && !IsWhiteSpace( *p ) && *p != '\n' && *p != '\r'    // whitespace
                && *p != '/' && *p != '>' )                            // tag end
        {
            if ( *p == SINGLE_QUOTE || *p == DOUBLE_QUOTE ) {
                // [ 1451649 ] Attribute values with trailing quotes not handled correctly
                // We did not have an opening quote but seem to have a 
                // closing one. Give up and throw an error.
                if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
                return 0;
            }
            value += *p;
            ++p;
        }
    }
    return p;
}

#ifdef EE_TIXML_USE_STL
void TiXmlText::StreamIn( std::istream * in, EE_TIXML_STRING * tag )
{
    while ( in->good() )
    {
        int c = in->peek();    
        if ( !cdata && (c == '<' ) ) 
        {
            return;
        }
        if ( c <= 0 )
        {
            TiXmlDocument* document = GetDocument();
            if ( document )
                document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
            return;
        }

        (*tag) += (char) c;
        in->get();    // "commits" the peek made above

        if ( cdata && c == '>' && tag->size() >= 3 ) {
            size_t len = tag->size();
            if ( (*tag)[len-2] == ']' && (*tag)[len-3] == ']' ) {
                // terminator of cdata.
                return;
            }
        }    
    }
}
#endif

const char* TiXmlText::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    SetUserData(data);
    value = "";
    TiXmlDocument* document = GetDocument();

    const char* const startTag = "<![CDATA[";
    const char* const endTag   = "]]>";

    if ( cdata || StringEqual( p, startTag, false, encoding ) )
    {
        cdata = true;

        if ( !StringEqual( p, startTag, false, encoding ) )
        {
            document->SetError( TIXML_ERROR_PARSING_CDATA, p, data, encoding );
            return 0;
        }
        p += strlen( startTag );

        // Keep all the white space, ignore the encoding, etc.
        while (       p && *p
                && !StringEqual( p, endTag, false, encoding )
              )
        {
            value += *p;
            ++p;
        }

        EE_TIXML_STRING dummy; 
        p = ReadText( p, &dummy, false, endTag, false, encoding );
        return p;
    }
    else
    {
        bool ignoreWhite = true;

        const char* end = "<";
        p = ReadText( p, &value, ignoreWhite, end, false, encoding );
        if ( p )
            return p-1;    // don't truncate the '<'
        return 0;
    }
}

#ifdef EE_TIXML_USE_STL
void TiXmlDeclaration::StreamIn( std::istream * in, EE_TIXML_STRING * tag )
{
    while ( in->good() )
    {
        int c = in->get();
        if ( c <= 0 )
        {
            TiXmlDocument* document = GetDocument();
            if ( document )
                document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
            return;
        }
        (*tag) += (char) c;

        if ( c == '>' )
        {
            // All is well.
            return;
        }
    }
}
#endif

const char* TiXmlDeclaration::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding _encoding )
{
    SetUserData(data);
    p = SkipWhiteSpace( p, _encoding );
    // Find the beginning, find the end, and look for
    // the stuff in-between.
    TiXmlDocument* document = GetDocument();
    if ( !p || !*p || !StringEqual( p, "<?xml", true, _encoding ) )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION, 0, 0, _encoding );
        return 0;
    }
    p += 5;

    version = "";
    encoding = "";
    standalone = "";

    while ( p && *p )
    {
        if ( *p == '>' )
        {
            ++p;
            return p;
        }

        p = SkipWhiteSpace( p, _encoding );
        if ( StringEqual( p, "version", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );        
            version = attrib.Value();
        }
        else if ( StringEqual( p, "encoding", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );        
            encoding = attrib.Value();
        }
        else if ( StringEqual( p, "standalone", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );        
            standalone = attrib.Value();
        }
        else
        {
            // Read over whatever it is.
            while( p && *p && *p != '>' && !IsWhiteSpace( *p ) )
                ++p;
        }
    }
    return 0;
}

bool TiXmlText::Blank() const
{
    for ( unsigned i=0; i<value.length(); i++ )
        if ( !IsWhiteSpace( value[i] ) )
            return false;
    return true;
}


#ifdef EE_TIXML_USE_STL
void TiXmlStylesheet::StreamIn( std::istream * in, EE_TIXML_STRING * tag )
{
    while ( in->good() )
    {
        int c = in->get();
        if ( c <= 0 )
        {
            TiXmlDocument* document = GetDocument();
            if ( document )
                document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
            return;
        }
        (*tag) += (char) c;

        if ( c == '>' )
        {
            // All is well.
            return;
        }
    }
}
#endif

const char* TiXmlStylesheet::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding _encoding )
{
    SetUserData(data);
    p = SkipWhiteSpace( p, _encoding );
    // Find the beginning, find the end, and look for the stuff in-between.
    TiXmlDocument* document = GetDocument();
    if ( !p || !*p || !StringEqual( p, "<?xml-stylesheet", true, _encoding ) )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION, 0, 0, _encoding );
        return 0;
    }
    p += 16;

    type = "";
    href = "";

    while ( p && *p )
    {
        if ( *p == '>' )
        {
            ++p;
            return p;
        }

        p = SkipWhiteSpace( p, _encoding );
        if ( StringEqual( p, "type", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );
            type = attrib.Value();
        }
        else if ( StringEqual( p, "href", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );
            href = attrib.Value();
        }
        else
        {
            // Read over whatever it is.
            while( p && *p && *p != '>' && !IsWhiteSpace( *p ) )
                ++p;
        }
    }
    return 0;
}