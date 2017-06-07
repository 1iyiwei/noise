/*
 * MDHash.cpp
 *
 * Li-Yi Wei
 *
 * 12/24/2003
 *
 */

#include "MDHash.hpp"
#include "Exception.hpp"

MDHash::MessageBlock::MessageBlock(void)
{
    for(int i = 0; i < 16; i++)
    {
        _data[i] = 0;
    }
}

MDHash::MDHash(void)
{
    // nothing to do
}

MDHash::~MDHash(void)
{
    // nothing to do
}

int MDHash::Hash(const MessageBlock & input, MessageBlock & digest)
{
    MDInit(&_context);
    MDUpdate(&_context, const_cast<unsigned char *>(input._data), 16);
    MDFinal(digest._data, &_context);

    return 1;
}
    
