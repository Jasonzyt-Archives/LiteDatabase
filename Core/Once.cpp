#include <Once.h>
#include <Utils.h>
#include <Results.h>
#include <Row.h>

namespace LLDB {

	Once::Once(Session& sess) : sess(sess) {}

	void Once::replace(const std::string& k, const std::string& v) {
		auto sym = "${" + k + "}";
		auto str = sql.str();
		auto pos = str.find(sym);
		str = str.substr(0, pos) + v + str.substr(pos + sym.size());
		sql = std::ostringstream();
		sql << str;
	}
	void Once::replace(const std::string& v) {
		auto str = sql.str();
		for (int i = start; i < str.size(); i++) {
			if (str[i] == '$') {
				if (i + 1 == str.size()) {
					str.pop_back();
					str += v;
				}
				else if (str[i + 1] != '$') {
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

	Once& Once::operator,(const use_type& u) {
		if (u.noKey) {
			replace(u.val);
		}
		else {
			replace(u.key, u.val);
		}
		return *this;
	}

	void Once::operator,(into_type<Row>&& i) {
		auto res = sess.query(getSQL());
		i.val = res[0];
	}
	void Once::operator,(into_type<Results>&& i) {
		auto res = sess.query(getSQL());
		i.val = res;
	}
	void Once::operator,(into_type<bool>&& i) {
		try {
			sess.exec(getSQL()); 
			i.val = true;
		}
		catch (...) {
			i.val = false;
		}
	}
	void Once::operator,(into_null_type) {
		sess.exec(getSQL());
	}

	std::string Once::getSQL() {
		auto str = sql.str();
		for (int i = 0; i < str.size() - 1; i++) {
			if (str[i] == '$' && str[i + 1] == '$') {
				str.erase(i, 1);
			}
		}
		return str;
	}

	use_type::use_type(const std::string& k, const std::string& v) : key(k), val(v) {
		noKey = false;
	}
	use_type::use_type(const std::string& v) : val(v) {
		noKey = true;
	}

}