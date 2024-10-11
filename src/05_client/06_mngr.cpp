//客户机
//实现连接池管理器类

#include"03_pool.h"
#include"05_mngr.h"

    //创建连接池
    acl::connect_pool* mngr_c::create_pool(char const* destaddr,ssize_t count,ssize_t index){
        return new pool_c(destaddr,count,index);
    }
