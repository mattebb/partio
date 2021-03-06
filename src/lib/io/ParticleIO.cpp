/*
PARTIO SOFTWARE
Copyright 2010 Disney Enterprises, Inc. All rights reserved

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

* The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
Studios" or the names of its contributors may NOT be used to
endorse or promote products derived from this software without
specific prior written permission from Walt Disney Pictures.

Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#include <iostream>
#include "../Partio.h"
#include "readers.h"

namespace Partio
{

using namespace std;

// reader and writer code
typedef ParticlesDataMutable* (*READER_FUNCTION)(const char*,const bool,char**,int);
typedef bool (*WRITER_FUNCTION)(const char*,const ParticlesData&,const bool);

map<string,READER_FUNCTION>&
readers()
{
    static map<string,READER_FUNCTION> data;
    static bool initialized=false;
    if(!initialized){
        data["bgeo"]=readBGEO;
        data["geo"]=readGEO;
        data["pdb"]=readPDB;
        data["pdb32"]=readPDB32;
        data["pdb64"]=readPDB64;
        data["pda"]=readPDA;
        data["mc"]=readMC;
        data["ptc"]=readPTC;
	data["pdc"]=readPDC;
	data["prt"]=readPRT;
	data["bin"]=readBIN;
    }
    return data;
}

map<string,WRITER_FUNCTION>&
writers()
{
    static map<string,WRITER_FUNCTION> data;
    static bool initialized=false;
    if(!initialized){
        data["bgeo"]=writeBGEO;
        data["geo"]=writeGEO;
        data["pdb"]=writePDB;
        data["pdb32"]=writePDB32;
        data["pdb64"]=writePDB64;
        data["pda"]=writePDA;
        data["ptc"]=writePTC;
        data["rib"]=writeRIB;
	data["pdc"]=writePDC;
	data["prt"]=writePRT;
	data["bin"]=writeBIN;
    }
    return data;
}

//! Gives extension of a file ignoring any trailing .gz
//! i.e. for 'foo.pdb.gz' it gives 'pdb', for 'foo.pdb' it gives 'pdb'
bool extensionIgnoringGz(const string& filename,string& ret,bool &endsWithGz)
{
    size_t period=filename.rfind('.');
    endsWithGz=false;
    if(period==string::npos){
        cerr<<"Partio: No extension detected in filename"<<endl;
        return false;
    }
    string extension=filename.substr(period+1);
    if(extension=="gz"){
        endsWithGz=true;
        size_t period2=filename.rfind('.',period-1);
        if(period2==string::npos){
            cerr<<"Partio: No extension detected in filename"<<endl;
            return false;
        }
        string extension2=filename.substr(period2+1,period-period2-1);
        ret=extension2;
    }else{
        ret=extension;
    }
    return true;
}

ParticlesDataMutable*
read(const char* c_filename)
{
    string filename(c_filename);
    string extension;
    bool endsWithGz;
    if(!extensionIgnoringGz(filename,extension,endsWithGz)) return 0;
    map<string,READER_FUNCTION>::iterator i=readers().find(extension);
    if(i==readers().end()){
        cerr<<"Partio: No reader defined for extension "<<extension<<endl;
        return 0;
    }
    return (*i->second)(c_filename,false,NULL,100);
}

ParticlesDataMutable*
readPart(const char* c_filename, char** attributes, int percentage)
{
    string filename(c_filename);
    string extension;
    bool endsWithGz;
    if(!extensionIgnoringGz(filename,extension,endsWithGz)) return 0;
    map<string,READER_FUNCTION>::iterator i=readers().find(extension);
    if(i==readers().end()){
        cerr<<"Partio: No reader defined for extension "<<extension<<endl;
        return 0;
    }
    return (*i->second)(c_filename,false,attributes,percentage);
}

ParticlesInfo*
readHeaders(const char* c_filename)
{
    string filename(c_filename);
    string extension;
    bool endsWithGz;
    if(!extensionIgnoringGz(filename,extension,endsWithGz)) return 0;
    map<string,READER_FUNCTION>::iterator i=readers().find(extension);
    if(i==readers().end()){
        cerr<<"Partio: No reader defined for extension "<<extension<<endl;
        return 0;
    }
    return (*i->second)(c_filename,true,false,0);
}

void 
write(const char* c_filename,const ParticlesData& particles,const bool forceCompressed)
{
    string filename(c_filename);
    string extension;
    bool endsWithGz;
    if(!extensionIgnoringGz(filename,extension,endsWithGz)) return;
    map<string,WRITER_FUNCTION>::iterator i=writers().find(extension);
    if(i==writers().end()){
        cerr<<"Partio: No writer defined for extension "<<extension<<endl;
        return;
    }
    (*i->second)(c_filename,particles,forceCompressed || endsWithGz);
}

} // namespace Partio
