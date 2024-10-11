// 存储服务器

// 实现ID客户机类

#include <acl-lib/acl_cpp/lib_acl.hpp>
#include "../01_common/02_proto.h"
#include "../01_common/03_util.h"
#include "01_globals.h"
#include "09_id.h"

// 从ID服务器获取与ID键相对应的值
long id_c::get(char const *key) const
{
    //检查ID键
    if(!key){//不为空
        logger_error("key is null");
        return -1;
    }
    sssize_t keylen = strlen(key);
    if(!keylen){//长度不为零
        logger_error("key is null");
        return -1;
    }
    if(keylen> ID_KEY_MAX){//长度不能过长
        logger_error("key too big : %lu > %d",keylen,ID_KEY_MAX);
        return -1;
    }
    //|包体长度|命令|状态| ID键|
    //|8      |1   |1  |包体长度|
    //构造请求
    long long bodylen = keylen+1;
    long long requlen = HEADLEN + bodylen;
    char requ[requlen] = {};
    llton(bodylen,requ);
    requ[BODYLEN_SIZE] = CMD_ID_GET;
    requ[BODYLEN_SIZE+COMMAND_SIZE] = 0;
    strcpy(requ+HEADLEN,key);
    // 向ID服务器发送请求,接受并解析响应,从中获得ID值
    return client(requ,requlen);    
}

// 向ID服务器发送请求,接受并解析响应,从中获得ID值
long id_c::client(char const *requ, long long requlen) const
{
    acl::socket_stream conn;
    //从ID服务器地址表中随机抽取一台服务器,尝试连接
    //g_iaddrs
    //conn.open("127.0.0.1:22000",0,0)->true:成功,false:失败
    srand(time(nullptr));
    int nids = g_iaddrs.size();
    int nrand = rand() % nids;//随机下标
    for(int i = 0 ; i< nids; ++i){
        if(conn.open(g_iaddrs[nrand].c_str(),0,0)){
            logger("connect id success , addr: %s",g_iaddrs[nrand].c_str());
            break;
        }else{
            logger("connect id fail,addr: %s",g_iaddrs[nrand].c_str());
            nrand = (nrand +1) % nids;
        }
    }
    if(!conn.alive()){
        logger_error("connect if fail , addrs: %s",cfg_iaddrs);
        return -1;
    }
    //向ID服务器发送请求
    if(conn.write(requ,requlen)< 0){
        logger_error("write fail :%s,requlen : %lld, to : %s",acl::last_serror(),requlen,conn.get_peer());
        conn.close();
        return -1;
    }
    //从ID服务器接受响应
    long long resplen = HEADLEN + BODYLEN_SIZE;
    char resp[resplen] = {};
    if(conn.read(requ,requlen)< 0){
        logger_error("read fail :%s,requlen : %lld, to : %s",acl::last_serror(),requlen,conn.get_peer());
        conn.close();
        return -1;
    }
    //|包体长度|命令|状态|ID值|
    //|8      |1    |1   |8   |
    //从ID服务器的响应中解析出ID值
    long value = ntol(resp + HEADLEN);

    conn.close();
    return value;


}











