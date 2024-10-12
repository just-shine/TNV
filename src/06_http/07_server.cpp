// 跟踪服务器
// 实现服务器类
//
#include "../01_common/01_types.h"
#include "../05_client/07_client.h"
#include "02_globals.h"
#include "04_service.h"
#include "06_server.h"

// 进程启动时被调用
void server_c::proc_on_init(void) {

    // 初始化客户机
    client_c::init(cfg_taddrs);
    //创建并初始化Redis集群
    m_redis = new acl::redis_client_cluster;
    m_redis->init(NULL,cfg_raddrs,cfg_maxthrds,cfg_ctimeout,cfg_rtimeout);
    // 打印配置信息
    logger("cfg_taddrs: %s, cfg_raddrs: %s, cfg_maxthrds: %s, "
           "cfg_ctimeout: %d, cfg_rtimeout: %d, cfg_rsession: %d",
           cfg_taddrs, cfg_raddrs, cfg_maxthrds,
           cfg_ctimeout, cfg_rtimeout, cfg_rsession);
}

// 进程意图退出时被调用
// 返回true，进程立即退出，否则
// 若配置项ioctl_quick_abort非0，进程立即退出，否则
// 待所有客户机连接都关闭后，再退出
bool server_c::proc_exit_timer(ssize_t nclients, ssize_t nthreads) {

    if (!nclients || !nthreads) {
        logger("nclients: %lu, nthreads: %lu", nclients, nthreads);
        return true;
    }

    return false;
}

// 进程退出前被调用
void server_c::proc_on_exit(void) {
    delete m_redis;

    //终结化客户机
    client_c::deinit();
}

// 线程获得连接时被调用
bool server_c::thread_on_accept(acl::socket_stream* conn) {
    logger("connect, from: %s", conn->get_peer());
    //设置读写超时
    conn->set_rw_timeout(cfg_rtimeout);
    //创建会话
    acl::session* session = cfg_rsession ? 
    (acl::session*)new acl::redis_session(*m_redis,cfg_maxthrds) : 
    (acl::session*)new acl::memcache_session("127.0.0.1:11211");
    //创建并设置业务服务器对象
    conn->set_ctx(new service_c(conn,session));
    return true;
}

// 与线程绑定的连接可读时被调用
bool server_c::thread_on_read(acl::socket_stream* conn) {
    service_c* service = (service_c*)conn->get_ctx();
    if(!service){
        logger_fatal("service is null");
    }
    return service->doRun();

}

// 线程读写连接超时时被调用
bool server_c::thread_on_timeout(acl::socket_stream* conn) {
    logger("read timeout, from: %s", conn->get_peer());
    return true;
}

// 以上三个函数返回true，连接将被保持，否则连接即被关闭

// 连接关闭前被调用
void server_c::thread_on_close(acl::socket_stream* conn) {
    logger("client disconnect, from: %s", conn->get_peer());
    service_c* service = (service_c*)conn->get_ctx();
    acl::session* session = &service->getSession();
    delete session;
    delete service;
}
