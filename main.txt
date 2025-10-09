#include <iostream>
#include <string>
#include <filesystem>
#include "stegano_image.hpp"

using namespace std;
namespace fs=std::filesystem;

int main(){
    while(true){
        cout<<"\n=== MENU ===\n1. Cacher\n2. Extraire\n3. Quitter\nChoix : ";
        int ch; cin>>ch; cin.ignore();

        if(ch==1){
            string src,dst,msg; int bpc;
            cout<<"Image source : "; getline(cin,src);
            if(!fs::exists(src)){cerr<<"Fichier inexistant !\n"; continue;}
            cout<<"Image sortie (.png) : "; getline(cin,dst);
            cout<<"Message : "; getline(cin,msg);
            cout<<"Bits par canal (1-4) : "; cin>>bpc; cin.ignore();

            int w,h,c; auto img=readImage(src,w,h,c);
            if(!img) continue;
            embedMessage(img,w,h,c,msg,bpc);
            if(writeImage(dst,img,w,h,c)) cout<<"✅ Message caché !\n";
            stbi_image_free(img);
        }
        else if(ch==2){
            string src; int bpc;
            cout<<"Image à lire : "; getline(cin,src);
            cout<<"Bits par canal utilisés : "; cin>>bpc; cin.ignore();
            int w,h,c; auto img=readImage(src,w,h,c);
            if(!img) continue;
            string msg=extractMessage(img,w,h,c,bpc);
            cout<<"💬 Message extrait : "<<msg<<"\n";
            stbi_image_free(img);
        }
        else break;
    }
}
