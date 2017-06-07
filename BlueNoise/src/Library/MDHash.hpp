/*
 * MDHash.hpp
 *
 * hash function based on MD standard
 *
 * Li-Yi Wei
 * 12/24/2003
 *
 */

#ifndef _MD_HASH_HPP
#define _MD_HASH_HPP

#include "MD/md_global.h"
#include "MD/md5.h"

#ifndef MD
#define MD 5
#endif

#if MD == 2
#include "MD/md2.h"
#endif
#if MD == 4
#include "MD/md4.h"
#endif
#if MD == 5
#include "MD/md5.h"
#endif

#if MD == 2
#define MD_CTX MD2_CTX
#define MDInit MD2Init
#define MDUpdate MD2Update
#define MDFinal MD2Final
#endif
#if MD == 4
#define MD_CTX MD4_CTX
#define MDInit MD4Init
#define MDUpdate MD4Update
#define MDFinal MD4Final
#endif
#if MD == 5
#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final
#endif

class MDHash
{
public:
    class MessageBlock
    {
    public:
        friend class MDHash;
        
        MessageBlock(void);
        
        template<class T>
        MessageBlock(const T & input);

        template<class T>
        operator T(void) const;
        
    protected:
        unsigned char _data[16];
    };
    
public:
    MDHash(void);
    ~MDHash(void);

    int Hash(const MessageBlock & input, MessageBlock & digest);
    
protected:
    MD_CTX _context;
};

template<class T>
MDHash::MessageBlock::MessageBlock(const T & input)
{
    for(int i = 0; i < 16; i++)
    {
        _data[i] = 0;
    }

    const int num_input_bytes = sizeof(input);

    if(num_input_bytes > 16)
    {
        throw Exception("MDHash::MessageBlock::MessageBlock() : input too big");
    }
    else
    {
        const char * input_data = reinterpret_cast<const char *>(&input);
        
        for(int i = 0; i < num_input_bytes; i++)
        {
            _data[i] = input_data[i];
        }
    }
}

template<class T>
MDHash::MessageBlock::operator T(void) const
{
    T output;
    
    const int num_output_bytes = sizeof(output);

    if(num_output_bytes > 16)
    {
        throw Exception("MDHash::MessageBlock::MessageBlock() : output too big");
        return output; // this is junk but just make compiler happy
    }
    else
    {
        char * output_data = reinterpret_cast<char *>(&output);
 
        for(int i = 0; i < num_output_bytes; i++)
        {
            output_data[i] = _data[i];
        }
    }
    
    return output;
}

#endif
