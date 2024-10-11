//客户机
//声明连接池管理器类

#pragma once

#include<acl-lib/acl_cpp/lib_acl.hpp>

//连接池管理器
class mngr_c: public acl::connect_manager{

protected:
    //创建连接池
    acl::connect_pool* create_pool(char const* destaddr,ssize_t count,ssize_t index);
};