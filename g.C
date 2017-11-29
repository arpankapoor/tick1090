#include<arpa/inet.h>
#include<cstring>
#include<iostream>
#include<sstream>
#include<vector>
#include"k.h"
using namespace std;

#define D(m) {perror(m);exit(1);}
#define SBS_IP "localhost"
#define TP_IP "localhost"
#define SBS_PORT 30003
#define TP_PORT 5010
#define BUFLEN 1000

struct data{
 string sym;   //icao hex code
 string mtype; //message type
 I ttype;      //transmission type
 string cs;    //call sign
 I alt;        //altitude
 I gs;         //ground speed
 I track;      //track?
 F lat;        //latitude
 F lon;        //longitude
 I vrate;      //vertical rate
 string sqwk;  //mode a squawk code
 I alert;      //squawk has changed?
 I emerg;
 I spi;        //special position indicator
 I og;         //is on ground?
};

Z I connectToSBS(){
 I s;
 struct sockaddr_in a={.sin_family=AF_INET,.sin_port=htons(SBS_PORT)};
 inet_pton(AF_INET,SBS_IP,&(a.sin_addr));
 if((s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)D("socket");
 if(connect(s,(struct sockaddr*)&a,sizeof a)==-1)D("connect");
 R s;
}

Z I mstoi(const string& s){R (s.size()==0)?ni:stoi(s);}
Z F mstod(const string& s){R (s.size()==0)?nf:stod(s);}
Z data parseSBSLine(const string& line){
 I i=0;
 data d;
 string t;
 istringstream ss{line};
 while(getline(ss,t,',')){
  ++i;
  SW(i){
   CS(1,d.mtype=t)
   CS(2,d.ttype=mstoi(t))
   CS(5,d.sym=t)
   CS(11,d.cs=t)
   CS(12,d.alt=mstoi(t))
   CS(13,d.gs=mstoi(t))
   CS(14,d.track=mstoi(t))
   CS(15,d.lat=mstod(t))
   CS(16,d.lon=mstod(t))
   CS(17,d.vrate=mstoi(t))
   CS(18,d.sqwk=t)
   CS(19,d.alert=mstoi(t))
   CS(20,d.emerg=mstoi(t))
   CS(21,d.spi=mstoi(t))
   CS(22,d.og=mstoi(t))
  }
 }
 R d;
}

Z vector<data> parseData(const S d){
 string line;
 vector<data> a;
 istringstream ss{d};
 while(getline(ss,line)){
  line.pop_back();
  if(line.size()>0)a.push_back(parseSBSLine(line));
 }
 R a;
}

#define scs(x) ks(const_cast<char*>((x).c_str()))
Z V sendToTp(I c,const data& d){
 K row=knk(15,scs(d.sym),scs(d.mtype),kh(d.ttype),scs(d.cs),ki(d.alt),ki(d.gs),ki(d.track),kf(d.lat),kf(d.lon),ki(d.vrate),scs(d.sqwk),kb(d.alert),kb(d.emerg),kb(d.spi),kb(d.og));
 K r=k(-c,".u.upd",ks((const S)"fd"),row,(K)0);
 if(!r)D("k");
}

I main(I argc,S argv[]){
 I sfd,c,ds;
 C buf[BUFLEN];
 sfd=connectToSBS(); 
 if((c=khpu(TP_IP,TP_PORT,(const S)"a"))<=0)D("khpu");
 while(1){
  memset(buf,0,BUFLEN);
  if((ds=recv(sfd,buf,BUFLEN-1,0))==-1)D("recv");
  for(auto d:parseData(buf)){
   cout<<"sym="<<d.sym<<";mtype="<<d.mtype<<";ttype="<<d.ttype<<";cs="<<d.cs<<";alt="<<d.alt<<";gs="<<d.gs<<";track="<<d.track<<";lat="<<d.lat<<";lon="<<d.lon<<"\n";
   sendToTp(c,d);
  }
 }
 R 0;
}
