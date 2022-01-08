#include "PgSQLSession.h"
#include <Any.h>
#include <libpq/libpq-fe.h>
#include <thread>

namespace LLDB {

    Exception buildException(PGconn* conn) {
        return Exception(PQerrorMessage(conn), -1, LLDB::POSTGRESQL);
    }

    void PgSQLSession::open(ConnectionArgs args) {
        auto host = args.getString("host");
        auto port = (uint16_t)args.getInt("port", true, 5432);
        auto dbname = args.getString("database");
        auto user = args.getString("user");
        auto passwd = args.getString("passwd");
        auto timeout = args.getInt("timeout", true, 20);
        auto sslmode = args.getString("sslmode", true, "disable");
        std::ostringstream oss;
        oss << "host='" << host << "' port=" << port << " user='" << user <<
            "' password='" << passwd << "' connect_timeout=" << timeout << 
            " sslmode=" << sslmode;
        if (!dbname.empty()) {
            oss << " dbname=" << dbname;
        }
        if (sslmode != "disable") {
            auto sslkey = args.getString("sslkey", true, "~/.postgresql/postgresql.key");
            auto sslcert = args.getString("sslcert", true, "~/.postgresql/postgresql.crt");
            auto sslrootcert = args.getString("sslrootcert", true, "~/.postgresql/root.crt");
            auto sslcrl = args.getString("sslcrl", true, "~/.postgresql/root.crl");
            auto sslcompression = args.getBool("sslcompression", true, true);
            oss << " sslkey=" << sslkey << " sslcert='" << sslcert
                << "' sslrootcert='" << sslrootcert << "' sslcrl='" << sslcrl
                << "' sslcompression=" << sslcompression;
        }
        conn = PQconnectdb(oss.str().c_str());
        if (PQstatus(conn) == ConnStatusType::CONNECTION_OK) {
            _isOpen = true;
        }
        else {
            throw buildException(conn);
        }
    }
    void PgSQLSession::close() {
        if (conn) {
            PQfinish(conn);
            conn = 0;
        }
    }
    void PgSQLSession::exec(const std::string& sql) {
        while (PQgetResult(conn)) {
            //std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        if (!PQsendQuery(conn, sql.c_str())) {
            throw buildException(conn);
        }
    }
    void PgSQLSession::query(const std::string& sql, 
        std::function<bool(Row&, int)> cb) {
        static int stmt_num = 0;
        stmt_num = 0;
        auto sqls = split(sql, ';');
        for (auto& s : sqls) {
            if (s.empty() || s.back() == '\\') {
                continue;
            }
            exec(s);
            auto succ = PQsetSingleRowMode(conn);
            PGresult* res = nullptr;
            if (succ) {
                while ((res = PQgetResult(conn)) && 
                       PQresultStatus(res) == ExecStatusType::PGRES_SINGLE_TUPLE) {
                    auto fnum = PQnfields(res);
                    for (int i = 0; i < fnum; i++) {
                        Any val;
                        std::string fname = PQfname(res, i);
                        //if (PQgetisnull(res, 0, i)) {
                            auto tp = PQftype(res, i);
                            auto v = PQgetvalue(res, 0, i);
                            printf("#%d# %s = %s\n", i, fname.c_str(), v);
                            
                        //}
                    }
                    PQclear(res);
                }
            }
            stmt_num++;
        }
        stmt_num = 0;
    }

    DatabaseType PgSQLSession::getType() {
        return DatabaseType::POSTGRESQL;
    }

    bool PgSQLSession::isOpen() {
        auto status = PQstatus(conn);
        if (status == ConnStatusType::CONNECTION_BAD) {
            close();
            _isOpen = false;
        }
        return _isOpen;
    }

}

LLDB::Session& newPgSQLSession() {
    LLDB::Session* sess = new LLDB::PgSQLSession();
    return *sess;
}
