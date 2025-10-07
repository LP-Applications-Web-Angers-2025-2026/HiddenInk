#pragma once
#include "stb_image.h"
#include "stb_image_write.h"
#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <cstdint>
#include <algorithm>

using namespace std;

// --- Base64 simple pour sécuriser les caractères spéciaux ---
static const string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

inline string base64_encode(const string &in) {
    string out; int val=0, valb=-6;
    for (unsigned char c : in) { val=(val<<8)+c; valb+=8;
        while (valb>=0) { out.push_back(base64_chars[(val>>valb)&0x3F]); valb-=6; } }
    if (valb>-6) out.push_back(base64_chars[((val<<8)>>(valb+8))&0x3F]);
    while (out.size()%4) out.push_back('=');
    return out;
}

inline string base64_decode(const string &in) {
    vector<int> T(256,-1);
    for (int i=0;i<64;i++) T[base64_chars[i]]=i;
    string out; int val=0, valb=-8;
    for (unsigned char c : in) { if (T[c]==-1) break;
        val=(val<<6)+T[c]; valb+=6;
        if(valb>=0){ out.push_back(char((val>>valb)&0xFF)); valb-=8; } }
    return out;
}

// --- Lire et écrire image ---
inline unsigned char* readImage(const string& path,int &w,int &h,int &c) { return stbi_load(path.c_str(),&w,&h,&c,0);}
inline bool writeImage(const string& path,unsigned char* img,int w,int h,int c){
    return stbi_write_png(path.c_str(),w,h,c,img,w*c); }

// --- Conversion message ↔ bits ---
inline vector<int> messageToBits(const string &msg){
    vector<int> bits;
    for(char ch: msg) for(int i=7;i>=0;--i) bits.push_back((ch>>i)&1);
    return bits;
}
inline string bitsToMessage(const vector<int> &bits){
    string msg;
    for(size_t i=0;i+7<bits.size();i+=8){ char c=0;
        for(int j=0;j<8;j++) c=(c<<1)|bits[i+j];
        msg+=c;
    }
    return msg;
}

// --- Cacher un message ---
inline void embedMessage(unsigned char* img,int w,int h,int c,const string& msg,int bitsPerChannel=2){
    bitsPerChannel=max(1,min(bitsPerChannel,4));
    string encoded=base64_encode(msg);
    vector<int> bits=messageToBits(encoded);

    // taille message en 32 bits
    vector<int> sizeBits(32);
    int sz=(int)encoded.size();
    for(int i=31;i>=0;i--) sizeBits[31-i]=(sz>>i)&1;
    bits.insert(bits.begin(),sizeBits.begin(),sizeBits.end());

    size_t bitIndex=0;
    for(int i=0;i<w*h*c && bitIndex<bits.size();++i)
        for(int b=0;b<bitsPerChannel && bitIndex<bits.size();++b){
            img[i]&=~(1<<b); img[i]|=bits[bitIndex++];
        }
}

// --- Extraire un message ---
inline string extractMessage(unsigned char* img,int w,int h,int c,int bitsPerChannel=2){
    bitsPerChannel=max(1,min(bitsPerChannel,4));
    vector<int> bits;
    for(int i=0;i<w*h*c;++i) for(int b=0;b<bitsPerChannel;++b) bits.push_back((img[i]>>b)&1);

    uint32_t sz=0;
    for(int i=0;i<32;i++) sz=(sz<<1)|bits[i];
    vector<int> msgBits(bits.begin()+32,bits.begin()+32+sz*8);
    string encoded=bitsToMessage(msgBits);
    return base64_decode(encoded);
}
