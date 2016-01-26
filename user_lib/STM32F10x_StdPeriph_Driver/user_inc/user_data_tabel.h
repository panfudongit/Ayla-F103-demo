

/*************************
* Flash verct 
*************************/
#define FLAG   0  //0xB1B1A2A2
#define HEAD   4  //0xfaxxxx00
#define MODE1  8  //0x11223344
#define MODE2  12 //0x55667788
#define MODE3  16 //0x99aabbcc
#define PRON   20 //0xaabb
#define VER    24 //0Xvvvvvvvv

#define FLAG_DATA  0xB1B1A2A2

/****************************
* Info Type
*****************************/

#define REQUEST  0x00   //request devices info
#define LIVE     0x01
#define NETCONF  0x06   //network config
#define SYNCPRO  0x04   //sync property
#define SET      0x03   // set host
#define RESPONSE 0x02   //host response
#define APPNOTE  0x05   //app note   
#define ERRUP    0x0e   //error up
#define FWUP     0x09   //firmware upgrade
#define TIMESYC  0x01   //sync time
#define FSYNCPRO 0x0d   //force sync property
#define SYNCPROS 0x0C   //sync propertys
#define MODULID  0x0f   //read modul id
#define ENTMODE  0x0b   //enter test mode

#define HEADLEN  4
#define DATALEN  32


