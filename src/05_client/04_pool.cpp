// 客户机
// 声明连接池类

#include <acl-lib/acl_cpp/lib_acl.hpp>
#include "01_conn.h"
#include "03_pool.h"

// 连接池类

    //构造函数
pool_c::pool_c(char const* destaddr,int count,ssize_t index/* = 0*/):
    connect_pool(destaddr,count,index),
    m_ctimeout(30),m_rtimeout(60),m_itimeout(90){
}
    //设置超时
    void pool_c::timeouts(int ctimeout/* = 30*/,int rtimeout/* = 60*/,int itimeout/* = 90*/){
        m_ctimeout = ctimeout;
        m_rtimeout = rtimeout;
        m_itimeout = itimeout;
    }
    //获取连接
    acl::connect_client* pool_c::peek(void){
        connect_pool::check_idle(m_itimeout,true);
        return connect_pool::peek();
    }
    // 创建连接
    acl::connect_client* pool_c::create_connect(){
         return new conn_c(addr_,m_ctimeout,m_rtimeout);
    }
