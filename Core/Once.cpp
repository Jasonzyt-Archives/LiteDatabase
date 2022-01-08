#include <Once.h>
#include <Utils.h>
#include <Results.h>
#include <Row.h>

namespace LLDB {

    Once::Once(Session& sess) : sess(sess) {}

    void Once::replace(const std::string& k, const std::string& v, bool escape) {
        auto sym = "${" + k + "}";
        auto str = sql.str();
        auto pos = str.find(sym);
        auto val = v;
        if (pos == std::string::npos) {
            throw Exception::build("Once::replace: ${", k, "} not found");
        }
        str = str.substr(0, pos) + val + str.substr(pos + sym.size());
        sql = std::ostringstream();
        sql << str;
    }
    void Once::replace(const std::string& v, bool escape) {
        auto str = sql.str();
        auto val = v;
        for (size_t i = start; i < str.size(); i++) {
            if (str[i] == '?') {
                if (i + 1 == str.size()) {
                    str.pop_back();
                    str += v;
                }
                else if (str[i + 1] != '?') {
                    str = str.substr(0, i) + v + str.substr(i + 1);
                }
                else {
                    i++;
                    continue;
                }
                start += v.size() - 1;
                break;
            }
        }
        sql = std::ostringstream();
        sql << str;
    }
    void Once::escapeSingleQuote(std::string& str) {
        for (size_t i = 0; i < str.size(); i++) {
            if (str[i] == '\'' && str[i - 1] != '\'') {
                str.insert(i, 1, '\'');
                i++;
            }
        }
    }
    void Once::escapeDoubleQuote(std::string& str) {
        for (size_t i = 0; i < str.size(); i++) {
            if (str[i] == '\"' && str[i - 1] != '\\') {
                str.insert(i, 1, '\\');
                i++;
            }
        }
    }

    Once& Once::operator,(const use_type& u) {
        if (u.noKey) {
            replace(u.val, u.escape);
        }
        else {
            replace(u.key, u.val, u.escape);
        }
        return *this;
    }

    void Once::operator,(into_type<Row>&& i) {
        auto res = sess.query(getSQL());
        i.val = res[0];
        release();
    }
    void Once::operator,(into_type<Results>&& i) {
        auto res = sess.query(getSQL());
        i.val = res;
        release();
    }
    void Once::operator,(into_type<bool>&& i) {
        try {
            sess.exec(getSQL()); 
            i.val = true;
        }
        catch (...) {
            i.val = false;
        }
        release();
    }
    void Once::operator,(into_null_type) {
        sess.exec(getSQL());
        release();
    }

    std::string Once::getSQL() {
        auto str = sql.str();
        for (size_t i = 0; i < str.size() - 1; i++) {
            if (str[i] == '?' && str[i + 1] == '?') {
                str.erase(i, 1);
            }
        }
        return str;
    }

    void Once::release() {
        if (heap) {
            LLDB::release(*this);
        }
    }

    void release(Once& once) {
        Once* ptr = &once;
        delete ptr;
        ptr = 0;
    }

    use_type::use_type(const std::string& k, const std::string& v, bool escape) : key(k), val(v) {
        this->noKey = false;
        this->escape = escape;
    }
    use_type::use_type(const std::string& v, bool escape) : val(v) {
        this->noKey = true;
        this->escape = escape;
    }

}