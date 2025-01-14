// 客户机
// 声明客户机类
//
#pragma once

#include <vector>
#include <string>
#include <lib_acl.hpp>
//
// 客户机类
//
class client_c {
public:
    // 初始化
    static int init(char const* taddrs,
        int tcount = 16, int scount = 8);
    // 终结化
    static void deinit(void);

    // 从跟踪服务器获取存储服务器地址列表
    int saddrs(char const* appid, char const* userid,
        char const* fileid, std::string& saddrs);
    // 从跟踪服务器获取组列表
    int groups(std::string& groups);

    // 向存储服务器上传文件
    int upload(char const* appid, char const* userid,
        char const* fileid, char const* filepath);
    // 向存储服务器上传文件
    int upload(char const* appid, char const* userid,
        char const* fileid, char const* filedata, long long filesize);
    // 向存储服务器询问文件大小
    int filesize(char const* appid, char const* userid,
        char const* fileid, long long* filesize);
    // 从存储服务器下载文件
    int download(char const* appid, char const* userid,
        char const* fileid, long long offset, long long size,
        char** filedata, long long* filesize);
    // 删除存储服务器上的文件
    int del(char const* appid, char const* userid, char const* fileid);

private:
    static acl::connect_manager* s_mngr; // 连接池管理器
    static std::vector<std::string> s_taddrs; // 跟踪服务器地址表
    static int s_scount; // 存储服务器连接数上限
};
