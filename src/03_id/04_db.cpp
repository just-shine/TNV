// ID服务器
// 实现数据库访问类
//
#include "01_globals.h"
#include "03_db.h"

// 构造函数
db_c::db_c(void) : m_mysql(mysql_init(nullptr))
{ // 创建MySQL对象
    if (!m_mysql)
        logger_error("create m_mysql fail: %s", mysql_error(m_mysql));
}

// 析构函数
db_c::~db_c(void)
{
    // 销毁MySQL对象
    if (m_mysql)
    {
        mysql_close(m_mysql);
        m_mysql = nullptr;
    }
}

// 连接数据库
int db_c::connect(void)
{
    MYSQL *mysql = m_mysql;

    // 遍历MySQL地址表，尝试连接数据库
    for (std::vector<std::string>::const_iterator maddr =
             g_maddrs.begin();
         maddr != g_maddrs.end(); ++maddr)
        if ((m_mysql = mysql_real_connect(mysql, maddr->c_str(),
                                          "root", "123456", "tnv_idsdb", 0, nullptr, 0)))
            return OK;

    logger_error("connect database fail: %s",
                 mysql_error(m_mysql = mysql));
    return ERROR;
}

int db_c::get(char const *key, int inc, long *value) const
{
    // 关闭自动提交
    mysql_autocommit(m_mysql, 0); // 关闭自动提交
    // query database
    acl::string sql;
    sql.format("SELECT id_value FROM t_id_gen WHERE id='%s';", key);
    if (mysql_query(m_mysql, sql.c_str()))
    {
        logger_error("query database fail: %s,sql: %s",
                     mysql_error(m_mysql), sql.c_str());
        mysql_autocommit(m_mysql, 1);
        return ERROR;
    }

    // get result set 获取查询结果
    MYSQL_RES *res = mysql_store_result(m_mysql);
    if (!res)
    {
        logger_error("result is null : %s,sql: %s",
                     mysql_error(m_mysql), sql.c_str());
        mysql_autocommit(m_mysql, 1);
        return ERROR;
    }
    // get result row 获取结果记录
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row)
    { // exist 有记录
        // update
        sql.format("UPDATE t_id_gen SET id_value=id_value+%d WHERE id='%s' ;", inc, key);
        if (mysql_query(m_mysql, sql.c_str()))
        {
            logger_error("update database fail : %s,sql: %s", mysql_error(m_mysql), sql.c_str());
            mysql_autocommit(m_mysql, 1);
            return ERROR;
        }
        // 手动提交数据库
        mysql_commit(m_mysql); // 手动提交
        // 库中当前值
        *value = atol(row[0]);
    }
    else
    { // none 无记录
        // insert
        sql.format("INSERT INTO t_id_gen SET id='%s',id_value= '%d';", key, inc);
        if (mysql_query(m_mysql, sql.c_str()))
        {
            logger_error("insert database fail: %s, sql: %s",
                         mysql_error(m_mysql), sql.c_str());
            mysql_autocommit(m_mysql, 1);
            return ERROR;
        }
        mysql_commit(m_mysql); // 手动提交
    }
    // 打开自动提交
    mysql_autocommit(m_mysql, 1);
    return OK;
}
