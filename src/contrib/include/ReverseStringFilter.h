/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef REVERSESTRINGFILTER_H
#define REVERSESTRINGFILTER_H

#include "TokenFilter.h"

namespace Lucene
{
    /// Reverse token string, for example "country" => "yrtnuoc".
    ///
    /// If marker is supplied, then tokens will be also prepended by that character.
    /// For example, with a marker of &#x5C;u0001, "country" => "&#x5C;u0001yrtnuoc".
    /// This is useful when implementing efficient leading wildcards search.
    ///
    /// You must specify the required {@link Version} compatibility when creating
    /// ReverseStringFilter, or when using any of its static methods:
    /// <ul>
    ///    <li> As of 3.1, supplementary characters are handled correctly
    /// </ul>
    class LPPCONTRIBAPI ReverseStringFilter : public TokenFilter
    {
    public:
        /// Create a new ReverseStringFilter that reverses all tokens in the
        /// supplied {@link TokenStream}.
        ///
        /// The reversed tokens will not be marked.
        // @deprecated use {@link #ReverseStringFilter(Version, TokenStream)} instead.
        ReverseStringFilter(TokenStreamPtr input);

        /// Create a new ReverseStringFilter that reverses all tokens in the
        /// supplied {@link TokenStream}.
        ///
        /// The reversed tokens will not be marked.
        ReverseStringFilter(LuceneVersion::Version matchVersion, TokenStreamPtr input);

        /// Create a new ReverseStringFilter that reverses and marks all tokens
        /// in the supplied {@link TokenStream}.
        ///
        /// The reversed tokens will be prepended (marked) by the marker character.
        /// @deprecated use {@link #ReverseStringFilter(Version, TokenStream, char)}
        /// instead.
        ReverseStringFilter(TokenStreamPtr input, wchar_t marker);

        /// Create a new ReverseStringFilter that reverses and marks all tokens
        /// in the supplied {@link TokenStream}.
        ///
        /// The reversed tokens will be prepended (marked) by the marker character.
        ReverseStringFilter(LuceneVersion::Version matchVersion, TokenStreamPtr input, wchar_t marker);

        virtual ~ReverseStringFilter();

        LUCENE_CLASS(ReverseStringFilter);

    protected:
        CharTermAttributePtr termAtt;
        wchar_t marker;
        LuceneVersion::Version matchVersion;

        static const wchar_t NOMARKER;

    public:
        /// Example marker character: U+0001 (START OF HEADING)
        static const wchar_t START_OF_HEADING_MARKER;

        /// Example marker character: U+001F (INFORMATION SEPARATOR ONE)
        static const wchar_t INFORMATION_SEPARATOR_MARKER;

        /// Example marker character: U+EC00 (PRIVATE USE AREA: EC00)
        static const wchar_t PUA_EC00_MARKER;

        /// Example marker character: U+200F (RIGHT-TO-LEFT MARK)
        static const wchar_t RTL_DIRECTION_MARKER;

    public:
        virtual bool incrementToken();
    };
}

#endif
