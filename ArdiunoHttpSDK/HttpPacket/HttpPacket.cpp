/**********************************************************************
 *  Copyright(c) 2014-2015 China Mobile M2M Company Limited.
 *  All rights reserved.
 *
 *  文件名称: HttpPacket.cpp
 *  简要描述: Arduino http packets encapsulation library
 *
 *  创建日期: 2015.4.24
 *  作者: Zhou Jiaxu
 *  说明:  Arduino http packets encapsulation library
 *  版本:  V2.0
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

#include <HttpPacket.h>

/**********************************************************************
 *  HttpPacketHead::HttpPacketHead(void)
 *  功能描述: 构造函数
 *
 *  @return
 **********************************************************************/
HttpPacketHead::HttpPacketHead(void)
{
    clearUrlParameter();
    memset(content, 0, CMDLEN);

    apiKey = NULL;
    accessKey = NULL;
    hostAddress = NULL;
    devId = NULL;
    dataStreamId = NULL;
    triggerId = NULL;
    index = NULL;
    urlParaCnt = 0;

    for(int i = 0; i < URLPAPACNT; i++)
    {
        urlParaArr[i] = NULL;
    }

}

/**********************************************************************
 *  void HttpPacketHead::setHostAddress(char* hostAddr)
 *  功能描述: 设置http包头主机地址
 *  @param param1   char* hostAddr  服务器地址字符串指针
 *
 *  @return  void
 **********************************************************************/
void HttpPacketHead::setHostAddress(char* hostAddr)
{
    hostAddress = hostAddr;
    return;
}

/**********************************************************************
 *  void HttpPacketHead::updateUrlParameterWith(char* key, char* value)
 *  功能描述: 更新单个URL参数
 *  @param param1   char* key      参数名字字符串指针
 *  @param param2   char* value    参数的值字符串指针
 *  @return  void
 **********************************************************************/
void HttpPacketHead::updateUrlParameterWith(char* key, char* value)
{
    int tmp = urlParaIsExist(key);
    char* pKey = NULL;

    if(RET_ERR != tmp)
    {
        if((pKey = (char*)malloc(strlen(key) + strlen(value) + 5))  != NULL)
        {
            sprintf(pKey, "%s=%s", key, value);
            urlParaArr[tmp] = pKey;
        }
    }
}

/**********************************************************************
 * void HttpPacketHead::clearUrlParameter(void)
 *  功能描述: 清除所有之前设置的URL参数
 *  @param void
 *  @return  void
 **********************************************************************/
void HttpPacketHead::clearUrlParameter(void)
{
    for(int i = 0; i < URLPAPACNT; i++)
    {
        free(urlParaArr[i]);
        urlParaArr[i] = NULL;
    }
    urlParaCnt = 0;
}

/**********************************************************************
 * void HttpPacketHead::removeUrlParameter(char* key)
 *  功能描述:删除一个之前已经存在的URL参数，如果没有这
 *                        个参数 不做任何操作
 *  @param char* key   参数名称
 *  @return  void
 **********************************************************************/
void HttpPacketHead::removeUrlParameter(char* key)
{
    int i;
    int tmp = urlParaIsExist(key);

    if(RET_ERR != tmp)
    {
        free(urlParaArr[tmp]);
        urlParaArr[tmp] = NULL;
        for(i = tmp; i < urlParaCnt - 1; i++)
        {
            urlParaArr[i] = urlParaArr[i + 1];
        }
        urlParaArr[i] = NULL;
        urlParaCnt--;
    }
}

/**********************************************************************
 * int HttpPacketHead::urlParaIsExist(char* key)
 *  功能描述:判断http包中是否存在某个url参数
 *  @param char* key   参数名称
 *  @return   RET_ERR   参数不存在
 *                  i            参数存在，并返回参数的index
 **********************************************************************/
int HttpPacketHead::urlParaIsExist(char* key)
{
    int i;
    char buf[20];

    sprintf(buf, "%s=", key);

    for(i = 0; i < urlParaCnt; i++)
    {
        if(strlen(urlParaArr[i]) && strstr(urlParaArr[i], buf) != NULL)
        {
            break;
        }
    }

    if(urlParaCnt <= i) //新参数
    {
        return RET_ERR;
    }
    else
    {
        return i;
    }
}

/**********************************************************************
 * void HttpPacketHead::addUrlParameter(char* key, char* value)
 *  功能描述:添加一个url 参数http包中，如果存在该参数，
 *                       不做任何操作.
 *  @param1 char* key       参数名称
 *  @param2 char* value     参数值
 *  @return   void
 **********************************************************************/
void HttpPacketHead::addUrlParameter(char* key, char* value)
{
    int i = 0, len = 0;
    char buf[20];
    char tmp[20];
    char* p;

    sprintf(buf, "%s=", key);

    if(urlParaCnt >= URLPAPACNT)
    {
        return;
    }

    for(i = 0; i < urlParaCnt; i++)
    {
        if(strlen(urlParaArr[i]) && strstr(urlParaArr[i], buf) != NULL)
        {
            break;
        }
    }

    if(strstr(urlParaArr[i], "datastream_ids=") != NULL) //已有参数
    {
        len = strlen(urlParaArr[i]) - strlen(value);
        p = urlParaArr[i];
        p = p + len - 2;
        
        strcpy(tmp, p);
        sprintf(buf, ",%s", value);
        if(0 != strcmp(tmp, buf))
        {
            sprintf(buf, "=%s,", value);
            if(NULL == strstr(urlParaArr[i], buf))
            {
                sprintf(buf, ",%s,", value);
                if(NULL == strstr(urlParaArr[i], buf))
                {
                    strcat(urlParaArr[i] , ",");
                    strcat(urlParaArr[i] , value);
                }
            }
        }
    }

    if(urlParaCnt <= i && urlParaCnt < URLPAPACNT) //新参数
    {
        if(strcmp(key, "datastream_ids") == 0)
        {
            urlParaArr[i] = (char*)malloc(50);
        }
        else
        {
            urlParaArr[i] = (char*)malloc(strlen(key) + strlen(value) + 5);
        }

        if(NULL != urlParaArr[i])
        {
            sprintf(urlParaArr[i], "%s=%s", key, value);
        }
        urlParaCnt++;
    }
}

/**********************************************************************
 * void HttpPacketHead::setDevId(char* Dev_Id)
 *  功能描述:设置http包中要要访问的设备id
 *  @param1 char* Dev_Id       设备ID 字符串
 *  @return   void
 **********************************************************************/
void HttpPacketHead::setDevId(char* Dev_Id)
{
    devId = Dev_Id;
    return;
}

/**********************************************************************
 * void HttpPacketHead::setDataStreamId(char* StreamId)
 *  功能描述:设置http包中要要访问的数据流ID
 *  @param1 char* StreamId      数据流ID 字符串
 *  @return   void
 **********************************************************************/
void HttpPacketHead::setDataStreamId(char* StreamId)
{
    dataStreamId = StreamId;
    return;
}

/**********************************************************************
 * void HttpPacketHead::setTriggerId(char* TrigId)
 *  功能描述:设置http包中要要访问的触发器ID
 *  @param1 char* TrigId      触发器ID 字符串
 *  @return   void
 **********************************************************************/
void HttpPacketHead::setTriggerId(char* TrigId)
{
    triggerId = TrigId;
    return;
}

/**********************************************************************
 * void HttpPacketHead::setBinIdx(char* Idx)
 *  功能描述:设置http包中要要访问的bin文件索引
 *  @param1 char* Idx    文件索引
 *  @return   void
 **********************************************************************/
void HttpPacketHead::setBinIdx(char* Idx)
{
    index = Idx;
    return;
}

/**********************************************************************
 * void HttpPacketHead::addAPIKey(char* key)
 *  功能描述:添加一个API key，只有在API 更新操作中才会设置.
 *  @param1 char* key    key字符串
 *  @return   void
 **********************************************************************/
void HttpPacketHead::addAPIKey(char* key)
{
    apiKey = key;
    return;
}

/**********************************************************************
 * void HttpPacketHead::setAccessKey(char* key)
 *  功能描述:设置访问设备所需的密钥.
 *  @param1 char* key    key字符串
 *  @return   void
 **********************************************************************/
void HttpPacketHead::setAccessKey(char* key)
{
    accessKey = key ;
    return;
}

/**********************************************************************
 * void HttpPacketHead::productGetUrl(RESRC_TYPE Type)
 *  功能描述:产生查询操作所需url参数
 *  @param1 RESRC_TYPE Type     资源类型
 *  @return   void
 **********************************************************************/
void HttpPacketHead::productGetUrl(RESRC_TYPE Type)
{
    int len = 0;

    strcpy(content, "GET ");
    len = strlen(content);

    switch(Type)
    {
        case TYPE_USER:
            strcat(content, "/user");
            break;

        case TYPE_DEV:
            strcat(content, "/devices/");
            strcat(content, devId);
            break;

        case TYPE_DATASTREAM:
            if (RET_ERR != urlParaIsExist("datastream_ids"))
            {
                strcat(content, "/devices/");
                strcat(content, devId);
                strcat(content, "/datastreams");
             }
            else
            {
                strcat(content, "/devices/");
                strcat(content, devId);
                strcat(content, "/datastreams/");
                strcat(content, dataStreamId);
            }
            break;

        case TYPE_DATAPOINT:
            strcat(content, "/devices/");
            strcat(content, devId);
            strcat(content, "/datapoints");
            break;

        case TYPE_TRIGGER:
            strcat(content, "/triggers/");
            strcat(content, triggerId);
            break;

        case TYPE_APIKEY:
            strcat(content, "/keys");
            break;

        case TYPE_BINDATA:
            strcat(content, "/bindata/");
            strcat(content, index);
            break;

        case TYPE_APILOG:
            strcat(content, "/logs/");
            strcat(content, devId);
            break;

        case TYPE_HISTORYDATA:
            strcat(content, "/datapoints");
            break;

        default:
            break;
    }

    if(strlen(content) > len)
    {
        urlParaToCmd();
    }
    else
    {
        memset(content, 0, sizeof(content));
    }

}


/**********************************************************************
 * void HttpPacketHead::productPostUrl(RESRC_TYPE Type)
 *  功能描述:产生POST操作所需url参数
 *  @param1 RESRC_TYPE Type     资源类型
 *  @return  void
 **********************************************************************/
void HttpPacketHead::productPostUrl(RESRC_TYPE Type)
{
    int len = 0;

    strcpy(content, "POST ");
    len = strlen(content);

    switch(Type)
    {
        case TYPE_DEV:
            strcat(content, "/devices");
            break;

        case TYPE_DATASTREAM:
            strcat(content, "/devices/");
            strcat(content, devId);
            strcat(content, "/datastreams");
            break;

        case TYPE_DATAPOINT:
            strcat(content, "/devices/");
            strcat(content, devId);
            strcat(content, "/datapoints");
            break;

        case TYPE_TRIGGER:
            strcat(content, "/triggers");
            break;

        case TYPE_APIKEY:
            strcat(content, "/keys");
            break;

        case TYPE_BINDATA:
            strcat(content, "/bindata");
            break;

        default:
            break;
    }

    if(strlen(content) > len)
    {
        urlParaToCmd();
    }
    else
    {
        memset(content, 0, sizeof(content));
    }
}


/**********************************************************************
 * void HttpPacketHead::productPutUrl(RESRC_TYPE Type)
 *  功能描述:产生PUT操作所需url参数
 *  @param1 RESRC_TYPE Type     资源类型
 *  @return   void
 **********************************************************************/
void HttpPacketHead::productPutUrl(RESRC_TYPE Type)
{
    int len = 0;

    strcpy(content, "PUT ");
    len = strlen(content);

    switch(Type)
    {
        case TYPE_DEV:
            strcat(content, "/devices/");
            strcat(content, devId);
            break;

        case TYPE_DATASTREAM:
            strcat(content, "/devices/");
            strcat(content, devId);
            strcat(content, "/datastreams/");
            strcat(content, dataStreamId);
            break;

        case TYPE_TRIGGER:
            strcat(content, "/triggers/");
            strcat(content, triggerId);
            break;

        case TYPE_APIKEY:
            strcat(content, "/keys/");
            strcat(content, apiKey);
            break;

        default:
            break;
    }

    if(strlen(content) > len)
    {
        urlParaToCmd();
    }
    else
    {
        memset(content, 0, sizeof(content));
    }
}

/**********************************************************************
 * void HttpPacketHead::productDeleteUrl(RESRC_TYPE Type)
 *  功能描述:产生DELETE操作所需url参数
 *  @param1 RESRC_TYPE Type     资源类型
 *  @return   void
 **********************************************************************/
void HttpPacketHead::productDeleteUrl(RESRC_TYPE Type)
{
    int len = 0;

    strcpy(content, "DELETE ");
    len = strlen(content);

    switch(Type)
    {
        case TYPE_DEV:
            strcat(content, "/devices/");
            strcat(content, devId);
            break;

        case TYPE_DATASTREAM:
            strcat(content, "/devices/");
            strcat(content, devId);
            strcat(content, "/datastreams/");
            strcat(content, dataStreamId);
            break;

        case TYPE_DATAPOINT:
            strcat(content, "/devices/");
            strcat(content, devId);
            strcat(content, "/datapoints");
            break;

        case TYPE_TRIGGER:
            strcat(content, "/triggers/");
            strcat(content, triggerId);
            break;

        case TYPE_APIKEY:
            strcat(content, "/keys/");
            strcat(content, apiKey);
            break;

        case TYPE_BINDATA:
            strcat(content,  "/bindata/");
            strcat(content, index);
            break;
        default:
            break;
    }

    if(strlen(content) > len)
    {
        urlParaToCmd();
    }
    else
    {
        memset(content, 0, sizeof(content));
    }
}

/**********************************************************************
 * void HttpPacketHead::productJsonLen(int jsonLen)
 *  功能描述:产生http 的Content-Length:字段内容
 *  @param1 int jsonLen     http报所带json 内容长度
 *  @return   void
 **********************************************************************/
void HttpPacketHead::productJsonLen(int jsonLen)
{
    char buff[30];
    
    sprintf(buff, "Content-Length:%d\r\n\r\n", jsonLen);
    strcat(content, buff);
}

/**********************************************************************
void HttpPacketHead::urlParaToCmd(void)
 *  功能描述:添加url参数到命令包
 *  @param1 void
 *  @return   void
 **********************************************************************/
void HttpPacketHead::urlParaToCmd(void)
{
    int i =0;

    if(urlParaCnt > 0)
    {
        strcat(content, "?");
        strcat(content, urlParaArr[0]);
        for(i = 1; i < urlParaCnt; i++)
        {
            strcat(content, "&");
            strcat(content, urlParaArr[i]);
        }
    }

    strcat(content, " HTTP/1.1\r\n");
}

/**********************************************************************
int HttpPacketHead::createCmdPacket(OPS_TYPE operType, RESRC_TYPE type, char* jsonData)
 *  功能描述:产生http 数据包
 *  @param1 OPS_TYPE     operType          操作类型
 *  @param2 RESRC_TYPE  type                资源类型
 *  @param3 char* jsonData                    http内容json结构
 *  @return   RET_ERR      产生数据包失败，可能因为参数传入不当
                  RET_OK      产生数据包成功
 **********************************************************************/
int HttpPacketHead::createCmdPacket(OPS_TYPE operType, RESRC_TYPE type, char* jsonData)
{
    int jsonLen = strlen(jsonData);
    char buf[50];

    memset(content, 0, CMDLEN);
    memset(buf, 0, sizeof(buf));
    switch(operType)
    {
        case POST:
            productPostUrl(type);
            if(strlen(content))
            {
                sprintf(buf, "api-key:%s\r\n", accessKey);
                strcat(content, buf);
                sprintf(buf, "Host:%s\r\n", hostAddress);
                strcat(content, buf);
                productJsonLen(jsonLen);
            }
            break;

        case PUT:
            productPutUrl(type);
            if(strlen(content))
            {
                sprintf(buf, "api-key:%s\r\n", accessKey);
                strcat(content, buf);
                sprintf(buf, "Host:%s\r\n", hostAddress);
                strcat(content, buf);
                productJsonLen(jsonLen);
            }
            break;

        default:
            break;
    }

    if(strlen(content))
    {
        return RET_OK;
    }
    else
    {
        return RET_ERR;
    }
}

/**********************************************************************
int HttpPacketHead::createCmdPacket(OPS_TYPE operType, RESRC_TYPE type)
 *  功能描述:产生http 数据包
 *  @param1 OPS_TYPE     operType          操作类型
 *  @param2 RESRC_TYPE  type                资源类型
 *  @return   RET_ERR      产生数据包失败，可能因为参数传入不当
                  RET_OK      产生数据包成功
 **********************************************************************/
int HttpPacketHead::createCmdPacket(OPS_TYPE operType, RESRC_TYPE type)
{
    char buf[50];

    memset(content, 0, CMDLEN);
    memset(buf, 0, sizeof(buf));
    switch(operType)
    {
        case GET:
            productGetUrl(type);
            if(strlen(content))
            {
                sprintf(buf, "api-key:%s\r\n", accessKey);
                strcat(content, buf);
                sprintf(buf, "Host:%s\r\n\r\n", hostAddress);
                strcat(content, buf);
            }
            break;

        case DELETE:
            productDeleteUrl(type);
            if(strlen(content))
            {
                sprintf(buf, "api-key:%s\r\n", accessKey);
                strcat(content, buf);
                sprintf(buf, "Host:%s\r\n\r\n", hostAddress);
                strcat(content, buf);
            }
            break;

        default:
            break;
    }

    if(strlen(content))
    {
        return RET_OK;
    }
    else
    {
        return RET_ERR;
    }
}

/**********************************************************************
HttpPacketHead::~HttpPacketHead(void)
 *  功能描述:析构函数
 *  @param1
 *  @return
 **********************************************************************/
HttpPacketHead::~HttpPacketHead(void)
{
    for(int i = 0; i < URLPAPACNT; i++)
    {
        free(urlParaArr[i]);
        urlParaArr[i]= NULL;
    }

}
