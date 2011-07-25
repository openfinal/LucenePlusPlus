/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StandardTokenizer.h"
#include "StandardTokenizerImpl.h"
#include "ClassicTokenizerImpl.h"
#include "StandardAnalyzer.h"
#include "CharTermAttribute.h"
#include "OffsetAttribute.h"
#include "PositionIncrementAttribute.h"
#include "TypeAttribute.h"

namespace Lucene
{
    const int32_t StandardTokenizer::ALPHANUM = 0;
    const int32_t StandardTokenizer::APOSTROPHE = 1;
    const int32_t StandardTokenizer::ACRONYM = 2;
    const int32_t StandardTokenizer::COMPANY = 3;
    const int32_t StandardTokenizer::EMAIL = 4;
    const int32_t StandardTokenizer::HOST = 5;
    const int32_t StandardTokenizer::NUM = 6;
    const int32_t StandardTokenizer::CJ = 7;
    
    const int32_t StandardTokenizer::ACRONYM_DEP = 8;
    
    const int32_t StandardTokenizer::SOUTHEAST_ASIAN = 9;
    const int32_t StandardTokenizer::IDEOGRAPHIC = 10;
    const int32_t StandardTokenizer::HIRAGANA = 11;
    const int32_t StandardTokenizer::KATAKANA = 12;
    const int32_t StandardTokenizer::HANGUL = 13;
    
    StandardTokenizer::StandardTokenizer(LuceneVersion::Version matchVersion, ReaderPtr input)
    {
        init(input, matchVersion);
    }
    
    StandardTokenizer::StandardTokenizer(LuceneVersion::Version matchVersion, AttributeSourcePtr source, ReaderPtr input) : Tokenizer(source)
    {
        init(input, matchVersion);
    }
    
    StandardTokenizer::StandardTokenizer(LuceneVersion::Version matchVersion, AttributeFactoryPtr factory, ReaderPtr input) : Tokenizer(factory)
    {
        init(input, matchVersion);
    }
    
    StandardTokenizer::~StandardTokenizer()
    {
    }
    
    const Collection<String> StandardTokenizer::TOKEN_TYPES()
    {
        static Collection<String> _TOKEN_TYPES;
        if (!_TOKEN_TYPES)
        {
            _TOKEN_TYPES = Collection<String>::newInstance();
            _TOKEN_TYPES.add(L"<ALPHANUM>");
            _TOKEN_TYPES.add(L"<APOSTROPHE>");
            _TOKEN_TYPES.add(L"<ACRONYM>");
            _TOKEN_TYPES.add(L"<COMPANY>");
            _TOKEN_TYPES.add(L"<EMAIL>");
            _TOKEN_TYPES.add(L"<HOST>");
            _TOKEN_TYPES.add(L"<NUM>");
            _TOKEN_TYPES.add(L"<CJ>");
            _TOKEN_TYPES.add(L"<ACRONYM_DEP>");
            _TOKEN_TYPES.add(L"<SOUTHEAST_ASIAN>");
            _TOKEN_TYPES.add(L"<IDEOGRAPHIC>");
            _TOKEN_TYPES.add(L"<HIRAGANA>");
            _TOKEN_TYPES.add(L"<KATAKANA>");
            _TOKEN_TYPES.add(L"<HANGUL>");
        }
        return _TOKEN_TYPES;
    }
    
    void StandardTokenizer::init(ReaderPtr input, LuceneVersion::Version matchVersion)
    {
        if (LuceneVersion::onOrAfter(matchVersion, LuceneVersion::LUCENE_31))
            this->scanner = newLucene<StandardTokenizerImpl>(input);
        else
            this->scanner = newLucene<ClassicTokenizerImpl>(input);
        replaceInvalidAcronym = LuceneVersion::onOrAfter(matchVersion, LuceneVersion::LUCENE_24);
        maxTokenLength = StandardAnalyzer::DEFAULT_MAX_TOKEN_LENGTH;
        this->input = input;
        termAtt = addAttribute<CharTermAttribute>();
        offsetAtt = addAttribute<OffsetAttribute>();
        posIncrAtt = addAttribute<PositionIncrementAttribute>();
        typeAtt = addAttribute<TypeAttribute>();
    }
    
    void StandardTokenizer::setMaxTokenLength(int32_t length)
    {
        this->maxTokenLength = length;
    }
    
    int32_t StandardTokenizer::getMaxTokenLength()
    {
        return maxTokenLength;
    }
    
    bool StandardTokenizer::incrementToken()
    {
        clearAttributes();
        int32_t posIncr = 1;
        
        while (true)
        {
            int32_t tokenType = scanner->getNextToken();
            
            if (tokenType == StandardTokenizerImpl::YYEOF)
                return false;
            
            if (scanner->yylength() <= maxTokenLength)
            {
                posIncrAtt->setPositionIncrement(posIncr);
                scanner->getText(termAtt);
                int32_t start = scanner->yychar();
                offsetAtt->setOffset(correctOffset(start), correctOffset(start + termAtt->length()));
                
                // This 'if' should be removed in the next release. For now, it converts invalid acronyms to HOST. 
                /// When removed, only the 'else' part should remain.
                if (tokenType == ACRONYM_DEP)
                {
                    if (replaceInvalidAcronym)
                    {
                        typeAtt->setType(TOKEN_TYPES()[HOST]);
                        termAtt->setTermLength(termAtt->length() - 1); // remove extra '.'
                    }
                    else
                        typeAtt->setType(TOKEN_TYPES()[ACRONYM]);
                }
                else
                    typeAtt->setType(TOKEN_TYPES()[tokenType]);
                return true;
            }
            else
            {
                // When we skip a too-long term, we still increment the position increment
                ++posIncr;
            }
        }
    }
    
    void StandardTokenizer::end()
    {
        // set final offset
        int32_t finalOffset = correctOffset(scanner->yychar() + scanner->yylength());
        offsetAtt->setOffset(finalOffset, finalOffset);
    }
    
    void StandardTokenizer::reset(ReaderPtr input)
    {
        Tokenizer::reset(input);
        scanner->yyreset(input);
    }
    
    bool StandardTokenizer::isReplaceInvalidAcronym()
    {
        return replaceInvalidAcronym;
    }
    
    void StandardTokenizer::setReplaceInvalidAcronym(bool replaceInvalidAcronym)
    {
        this->replaceInvalidAcronym = replaceInvalidAcronym;
    }
}