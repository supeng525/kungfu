//
// Created by qlu on 2019/1/14.
//

#ifndef WC_2_ORDER_MAPPER_H
#define WC_2_ORDER_MAPPER_H

#include <string>
#include <spdlog/spdlog.h>
#include <SQLiteCpp/SQLiteCpp.h>

namespace kungfu
{
    namespace wingchun
    {
        namespace ufx
        {
            struct UfxOrder
            {
                uint64_t internal_order_id;
                uint64_t ufx_order_id;
                uint64_t parent_id;
                uint32_t source;
                int64_t insert_time;
                char trading_day[50];
            };

            class OrderMapper
            {
            public:
                OrderMapper(const std::string &file_name) : db_(file_name.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
                {
                    create_table_if_not_exist();
                }

                void create_table_if_not_exist()
                {
                    try
                    {
                        std::string sql = "CREATE TABLE IF NOT EXISTS ufx_order(\"internal_order_id\" INTERGER PRIMARY KEY NOT NULL,\"ufx_order_id\" INTERGER NOT NULL, \"parent_id\" INTERGER, \"source\" INTERGER, \"insert_time\" INTERGER,\"trading_day\" char(50));";
                        db_.exec(sql);
                    }
                    catch (std::exception &e)
                    {
                        SPDLOG_TRACE(e.what());
                    }
                }

                void add_order(const XtpOrder &order)
                {
                    try
                    {
                        SQLite::Statement insert(db_, "INSERT INTO ufx_order VALUES(?, ?, ?, ?, ?, ?)");
                        insert.bind(1, (long long) order.internal_order_id);
                        insert.bind(2, (long long) order.ufx_order_id);
                        insert.bind(3, (long long) order.parent_id);
                        insert.bind(4, (long long) order.source);
                        insert.bind(5, order.insert_time);
                        insert.bind(6, order.trading_day);
                        insert.exec();
                    }
                    catch (std::exception &e)
                    {
                        SPDLOG_ERROR(e.what());
                    }
                }

                const UfxOrder get_order_by_ufx_order_id(const char *trading_day, const uint64_t ufx_order_id)
                {
                    UfxOrder order = {};
                    try
                    {
                        SQLite::Statement query(db_, "SELECT * FROM ufx_order WHERE ufx_order_id = ? AND trading_day = ?");
                        query.bind(1, (long long) ufx_order_id);
                        query.bind(2, trading_day);
                        if (query.executeStep())
                        {
                            order.internal_order_id = (long long) (query.getColumn(0));
                            order.ufx_order_id = ufx_order_id;
                            order.parent_id = (long long) (query.getColumn(2));
                            order.source = (long long) (query.getColumn(3));
                            order.insert_time = (long long) (query.getColumn(4));
                            strcpy(order.trading_day, query.getColumn(5));
                        }
                    }
                    catch (std::exception &e)
                    {
                        SPDLOG_ERROR(e.what());
                    }
                    return order;
                }

                const uint64_t get_ufx_order_id(const uint64_t internal_order_id)
                {
                    uint64_t ufx_order_id = 0;
                    try
                    {
                        SQLite::Statement query(db_, "SELECT ufx_order_id FROM ufx_order WHERE internal_order_id = ?");
                        query.bind(1, (long long) internal_order_id);
                        if (query.executeStep())
                        {
                            ufx_order_id = (long long) (query.getColumn(0));
                        }
                    }
                    catch (std::exception &e)
                    {
                        SPDLOG_TRACE(e.what());
                    }
                    return ufx_order_id;
                }

                const uint64_t get_internal_order_id(const char *trading_day, const uint64_t ufx_order_id)
                {
                    uint64_t internal_order_id = 0;
                    try
                    {
                        SQLite::Statement query(db_, "SELECT internal_order_id FROM ufx_order WHERE ufx_order_id = ? AND trading_day = ?");
                        query.bind(1, (long long) ufx_order_id);
                        query.bind(2, trading_day);
                        if (query.executeStep())
                        {
                            internal_order_id = (long long) (query.getColumn(0));
                        }
                    }
                    catch (std::exception &e)
                    {
                        SPDLOG_ERROR(e.what());
                    }
                    return internal_order_id;
                }
            private:
                SQLite::Database db_;
            };
        }
    }
}
#endif //WC_2_ORDER_MAPPER_H
