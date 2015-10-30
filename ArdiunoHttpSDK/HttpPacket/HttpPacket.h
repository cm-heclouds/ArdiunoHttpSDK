/**********************************************************************
 *  Copyright(c) 2014-2015 China Mobile M2M Company Limited.
 *  All rights reserved.
 *
 *  文件名称: HttpPacket.h
 *  简要描述: Arduino http packets encapsulation library
 *
 *  创建日期: 2015.4.24
 *  作者: Zhou Jiaxu
 *  说明:  Arduino http packets encapsulation library
 *  版本:  V2.1
 *
 *  修改日期:2015.5.29
 *  作者:Zhou Jiaxu
 *  说明:相对于V1.0修改关于批量查询数据流的函数，添加
 *               SDK使用示例
 *
 *  修改日期:2015.10.30
 *  作者:Zhou Jiaxu
 *  说明:相对于V2.0修改
 *              int createCmdPacket(OPS_TYPE operType, RESRC_TYPE type, JsonObject&  jsonData);
 *                                        为 
 *              int createCmdPacket(OPS_TYPE operType, RESRC_TYPE type, char* jsonData);
 *              V2.0使用JSON库, 导致内存占用过大，可能导致程序
 *              崩溃，V2.1直接使用封装好的JSON串作为输入，节省
 *              内存开销，降低耦合。
 **********************************************************************/

#ifndef HTTP_PACKET_H_
#define HTTP_PACKET_H_

using namespace std;

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define URLPAPACNT 7    
#define RET_ERR 404
#define RET_OK 1
#define CMDLEN 300

typedef enum operation_type
{
    POST,
    PUT,
    GET,
    DELETE
}OPS_TYPE;

typedef enum resouce_type
{
    TYPE_USER = 1,
    TYPE_DEV,
    TYPE_DATASTREAM,
    TYPE_DATAPOINT,
    TYPE_TRIGGER, 
    TYPE_APIKEY, 
    TYPE_BINDATA,  
    TYPE_APILOG,  
    TYPE_HISTORYDATA
}RESRC_TYPE;

class HttpPacketHead
{
    public:

        char content[CMDLEN];

        HttpPacketHead();
        ~HttpPacketHead();
        
        void addAPIKey(char* key);
        void setAccessKey(char* key);
        void setHostAddress(char* hostAddr);
        void setDevId(char* Dev_Id);
        void setDataStreamId(char* StreamId);
        void setTriggerId(char* TrigId);
        void setBinIdx(char* Idx);
        
        void addUrlParameter(char* key, char* value); 
        void updateUrlParameterWith(char* key, char* value);
        void clearUrlParameter(void);
        void removeUrlParameter(char* key);

        
        int createCmdPacket(OPS_TYPE operType, RESRC_TYPE type, char* jsonData);
        int createCmdPacket(OPS_TYPE operType, RESRC_TYPE type);
        
    private:
        char* apiKey;
        char* accessKey;
        char* hostAddress;
        char* devId;
        char* dataStreamId;
        char* triggerId;
        char* index;
        char* urlParaArr[URLPAPACNT];
        int urlParaCnt;
        
        void productPostUrl(RESRC_TYPE Type);
        void productPutUrl(RESRC_TYPE Type);
        void productGetUrl(RESRC_TYPE Type);
        void productDeleteUrl(RESRC_TYPE Type);
        void productJsonLen(int jsonLen);
        void findUrlParameter(char* key);
        int urlParaIsExist(char* key);
        void urlParaToCmd(void);
        
};

#endif  //HTTP_PACKET_H_
