unsigned int leftRotate(in unsigned int x, in unsigned int n)
{
    unsigned int t =  ( ((x) << (n)) | ((x) >> (32u-n)) );
    return t;
}

unsigned int F(unsigned int x, unsigned int y, unsigned int z)
{
    unsigned int t;
    t = ( (x & y) | ((~x) & z) );
    return t;
}

unsigned int G(unsigned int x, unsigned int y, unsigned int z)
{
    unsigned int t;
    t = ( (x & z) | ((~z) & y) );
    return t;
}

unsigned int H(unsigned int x, unsigned int y, unsigned int z)
{
    unsigned int t;
    t = ( (x ^ y ^ z) );
    return t;
}

unsigned int I(unsigned int x, unsigned int y, unsigned int z)
{
    unsigned int t;
    t = ( y ^ ((x) | (~z)));
    return t;
}

void FF(inout uvec4 td, int i, inout uvec4 Fr, float p, in unsigned int data[16])
{
    unsigned int Ft = F(td.y,td.z,td.w);
    unsigned int r = Fr.x;
    Fr = Fr.yzwx;
	
    float t = sin(float(i)) * p;
    unsigned int trigFunc = unsigned int(floor(t));
    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );
    td = td.yzwx;
}

void GG(inout uvec4 td, int i, inout uvec4 Gr, float p, in unsigned int data[16])
{
    unsigned int Ft = G(td.y,td.z,td.w);
    i = (5*i+1) %16;
    unsigned int r = Gr.x;
    Gr = Gr.yzwx;
    
    float t = sin(float(i)) * p;
    unsigned int trigFunc = unsigned int(floor(t));
    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );
    td = td.yzwx;
}

void HH(inout uvec4 td, int i, inout uvec4 Hr, float p, in unsigned int data[16])
{
    unsigned int Ft = H(td.y,td.z,td.w);
    i = (3*i+5) %16;
    unsigned int r = Hr.x;
    Hr = Hr.yzwx;
    
    float t = sin(float(i)) * p;
    unsigned int  trigFunc = unsigned int(floor(t));
    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );
    td = td.yzwx;
}

void II(inout uvec4 td, int i, inout uvec4 Ir, float p, in unsigned int data[16])
{
    unsigned int Ft = I(td.y,td.z,td.w);
    i = (7*i) %16;
    unsigned int r = Ir.x;
    Ir = Ir.yzwx;
    
    float t = sin(float(i)) * p;
    unsigned int trigFunc = unsigned int(floor(t));
    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );
    td = td.yzwx;
}

//init ABCD with the magical numbers
void initABCD(inout unsigned int A, inout unsigned int B, inout unsigned int C, inout unsigned int D)
{
    A = 0x67452301u;
    B = 0xefcdab89u;
    C = 0x98badcfeu;
    D = 0x10325476u;
}

void setupInput(float2 absCoord, int key, inout unsigned int input[16])
{
    unsigned int inputLen = 128u;  //the input length is four 32 bit integers
    //assume that the absCoord are in un-normalized range of window size

    input[0] = unsigned int(floor(absCoord.x));
    input[1] = unsigned int(floor(absCoord.y));

    //add key to this input
    input[0] = input[0] ^ unsigned int(key);
    input[1] = input[1] ^ unsigned int(key);
    //now add the key to the third input
    input[2] = unsigned int(key);
    input[3] = unsigned int(key);
	
    input[4] = 0x80000000u;

    //testing use only	
    //now we make sure the rest is zero until input 14 and 15
    for(int i=5; i<16; i++)
        input[i] = 0u;
        
    //now we can fix 14 and 15 ourselves, 14 is zero and 15 is inputLen  
    input[14] = 0u;
    input[15] = inputLen;
}

void MD5opt(in unsigned int data[16], inout uvec4 digest)
{
    unsigned int A,B,C,D, Ft, Gt;

    //initABCD(A,B,C,D);
    digest = uvec4(0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u);
    uvec4 td = digest;
    float p = pow(2.0,32.0);
    float t;
	
    //save the initial values
    /*unsigned int AA = A;
      unsigned int BB = B;
      unsigned int CC = C;
      unsigned int DD = D;*/
    
    unsigned int i; 
    unsigned int index;
    unsigned int trigFunc;
    
    uvec4 Fr = uvec4(7u,12u, 17u,22u);  
    uvec4 Gr = uvec4(5u, 9u, 14u,20u);
    uvec4 Hr = uvec4(4u,11u, 16u,23u);
    uvec4 Ir = uvec4(6u,10u, 15u,21u);
    unsigned int r;
	
    //figure out how to rotate it
    for(i=0u; i<64u; i++)
    {
        if( i >= 0u && i < 16u)
        {
            //Ft = F(B,C,D);
            Ft = F(td.y,td.z,td.w);
            Gt = i;
            r = Fr.x;
            Fr = Fr.yzwx;
        }//end if
        else if (i >= 16u && i < 32u)
        {
            //Ft = G(B,C,D);
            Ft = G(td.y,td.z,td.w);
            Gt = 5u * i + 1u;
            Gt = Gt % 16u;
            r = Gr.x;
            Gr = Gr.yzwx;
        }//end else if
        else if (i >= 32u && i < 48u)
        {
            //Ft = H(B,C,D);
            Ft = H(td.y,td.z,td.w);
            Gt = 3u*i + 5u;
            Gt = Gt %16u;
            r = Hr.x;
            Hr = Hr.yzwx;
        }//end else if
        else
        {
            //Ft = I(B,C,D);
            Ft = I(td.y,td.z,td.w);
            Gt = 7u*i;
            Gt = Gt % 16u;
            r = Ir.x;
            Ir = Ir.yzwx;
        }//end else
        
        t = sin(float(i)) * p;
        trigFunc = unsigned int(floor(t));
        td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[int(Gt)]), r );
        td = td.yzwx;
    }//end for
	
    digest += td;

}//end MD5opt

void MD5unroll(in unsigned int data[16], inout uvec4 digest)
{
    unsigned int Ft, Gt;
    
    //initABCD(A,B,C,D);
    digest = uvec4(0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u);
    uvec4 td = digest;
    float p = pow(2.0,32.0);
    
    uvec4 Fr = uvec4(7u,12u, 17u,22u);  
    uvec4 Gr = uvec4(5u, 9u, 14u,20u);
    uvec4 Hr = uvec4(4u,11u, 16u,23u);
    uvec4 Ir = uvec4(6u,10u, 15u,21u);
	
    FF(td, 0,Fr,p,data);
    FF(td, 1,Fr,p,data);
    FF(td, 2,Fr,p,data);
    FF(td, 3,Fr,p,data);
    FF(td, 4,Fr,p,data);
    FF(td, 5,Fr,p,data);
    FF(td, 6,Fr,p,data);
    FF(td, 7,Fr,p,data);
    FF(td, 8,Fr,p,data);
    FF(td, 9,Fr,p,data);
    FF(td,10,Fr,p,data);
    FF(td,11,Fr,p,data);
    FF(td,12,Fr,p,data);
    FF(td,13,Fr,p,data);
    FF(td,14,Fr,p,data);
    FF(td,15,Fr,p,data);
    
    GG(td,16,Gr,p,data);
    GG(td,17,Gr,p,data);
    GG(td,18,Gr,p,data);
    GG(td,19,Gr,p,data);
    GG(td,20,Gr,p,data);
    GG(td,21,Gr,p,data);
    GG(td,22,Gr,p,data);
    GG(td,23,Gr,p,data);
    GG(td,24,Gr,p,data);
    GG(td,25,Gr,p,data);
    GG(td,26,Gr,p,data);
    GG(td,27,Gr,p,data);
    GG(td,28,Gr,p,data);
    GG(td,29,Gr,p,data);
    GG(td,30,Gr,p,data);
    GG(td,31,Gr,p,data);
    
    HH(td,32,Hr,p,data);
    HH(td,33,Hr,p,data);
    HH(td,34,Hr,p,data);
    HH(td,35,Hr,p,data);
    HH(td,36,Hr,p,data);
    HH(td,37,Hr,p,data);
    HH(td,38,Hr,p,data);
    HH(td,39,Hr,p,data);
    HH(td,40,Hr,p,data);
    HH(td,41,Hr,p,data);
    HH(td,42,Hr,p,data);
    HH(td,43,Hr,p,data);
    HH(td,44,Hr,p,data);
    HH(td,45,Hr,p,data);
    HH(td,46,Hr,p,data);
    HH(td,47,Hr,p,data);
    
    II(td,48,Ir,p,data);
    II(td,49,Ir,p,data);
    II(td,50,Ir,p,data);
    II(td,51,Ir,p,data);
    II(td,52,Ir,p,data);
    II(td,53,Ir,p,data);
    II(td,54,Ir,p,data);
    II(td,55,Ir,p,data);
    II(td,56,Ir,p,data);
    II(td,57,Ir,p,data);
    II(td,58,Ir,p,data);
    II(td,59,Ir,p,data);
    II(td,60,Ir,p,data);
    II(td,61,Ir,p,data);
    II(td,62,Ir,p,data);
    II(td,63,Ir,p,data);
	
    digest += td;
}//end MD5unroll

// Li-Yi: this is incorrect; see ConvertToFloat01 below
float convertToFloatStanley(unsigned int value)
{
    unsigned int signbit = (value & 0x80000000u) >> 31u;
    unsigned int mantissa = (value & 0x7fffff00u) >> 8u;
    unsigned int exponent = (value & 0x000000ffu);
    
    float fVal;
    fVal = float(mantissa) / pow(2.0,23.0) + 1;
    
    fVal *= 1;
	
    //fVal *= (signbit == 1u) ? 1 : -1;
    return fVal;
}

float ConvertToFloat01(unsigned int value)
{
    // float result = float(value)/float(0xffffffffu)+0.5;

    float result = intBitsToFloat(0x3f800000u | (value & 0x007fffffu)) - 1.0;
    
    return result;
}

float4 ConvertToFloat01(uvec4 value)
{
    float4 result;

    result.x = ConvertToFloat01(value.x);
    result.y = ConvertToFloat01(value.y);
    result.z = ConvertToFloat01(value.z);
    result.w = ConvertToFloat01(value.w);

    return result;
}

float4 UniformRandom01(float2 input, uniform int key)
{
    unsigned int input_padded[16];
    setupInput(input, key, input_padded);

    uvec4 digest;
    MD5unroll(input_padded, digest);

    return ConvertToFloat01(digest);
}

struct FragmentInput
{
    float4 tex : TEX0;
    float4 col : COL0;
};

struct FragmentOutput
{
    float4 col : COL;
};

FragmentOutput fragment(FragmentInput input, uniform int key, uniform float2 res)
{
    FragmentOutput output;
    output.col = UniformRandom01(input.tex.xy * res, key);

    return output;
}

