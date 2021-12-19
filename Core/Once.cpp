#include <Once.h>
#include <Session.h>
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
	void Once::replace(const std::vector<std::string>& vals) {
		auto str = sql.str();
		int j = -1;
		for (int i = 0; i < str.size() - 1; i++) {
			if (str[i] == '$' && str[i + 1] == '$') {
				str.erase(i, 1);
			}
			else if (str[i] == '$') {
				str = str.substr(0, i) + vals[++j] + str.substr(i + 1);
				i += vals[j].size() - 1;
			}
		}
		if (str.back() == '$') {
			str.pop_back();
			str += vals[++j];
		}
		sql = std::ostringstream();
		sql << str;
	}

	Once& Once::operator,(const use_type& u) {
		if (u.list) {
			replace(u.vals);
		}
		else {
			replace(u.key, u.val);
		}
		return *this;
	}

	template <typename T>
	void Once::operator,(into_type<T>& i) {
		auto res = sess.query(sql.str());
		if (res.size()) {
			from_row(res[0], i.val);
		}
	}
	template <typename T>
	void Once::operator,(into_type<std::vector<T>>& i) {
		auto res = sess.query(sql.str());
		for (auto& row : res.getAll()) {
			T v;
			from_row(row, v);
			i.val.push_back(v);
		}
	}
	template <typename T>
	void Once::operator,(into_type<Row>& i) {
		auto res = sess.query(sql.str());
		return res[0];
	}
	template <typename T>
	void Once::operator,(into_type<Results>& i) {
		auto res = sess.query(sql.str());
		return res;
	}
	template <typename T>
	void Once::operator,(into_type<bool>& i) {
		try {
			sess.exec(sql.str()); 
			i.val = true;
		}
		catch (...) {
			i.val = false;
		}
	}
	void Once::operator,(into_null_type) {
		sess.exec(sql.str());
	}

	std::string Once::getSQL() {
		return sql.str();
	}

	use_type::use_type(const std::string& k, const std::string& v) : key(k), val(v) {
		list = false;
	}
	use_type::use_type(std::vector<std::string> vals) : vals(vals) {
		list = true;
	}

}