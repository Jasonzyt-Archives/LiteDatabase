#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <LLDB.h>

//#define SQLITE_TEST
#define MYSQL_TEST

using namespace std;
using namespace LLDB;

namespace fs = std::filesystem;

struct test {
	std::string a;
	uint64_t b = 0;
};
struct dbname {
	std::string db;
};

void from_row(Row& row, test& t) {
	t.a = row["A"].get<string>();
	t.b = row["B"].get<uint64_t>();
}
void from_row(Row& row, dbname& d) {
	d.db = row["Database"].get<string>();
}

int main(int argc, char** argv) {
	SetCurrentDirectory(L"..\\x64\\Release");
	cout << "================ LLDB Test ================" << endl;
#if defined(SQLITE_TEST)
	cout << "- SQLite: " << endl;
	auto& sess = newSession(SQLITE);
	try {
		sess.open({ {"target", "./test.db"}, {"create", true} });
		sess.exec("CREATE TABLE IF NOT EXISTS \"TEST\" (A TEXT,B INTEGER);");
		Results res;
		sess << "SELECT * FROM TEST", into(res);
		for (auto& row : res) {
			auto c1 = row.get("A");
			cout << "- A: " << c1.get<std::string>() << endl;
			auto c2 = row.get("B");
			cout << "- B: " << c2.get<uint64_t>() << endl;
		}
		cout << endl;
		std::vector<test> ress;
		Once onc(sess);
		onc << "select * from TEST where A='awa';select * from TEST where A='?';", use("wdnmd"), into(ress);
		for (auto& a : ress) {
			cout << a.a << a.b << endl;
		}
		sess.release();
	}
	catch (std::exception e) {
		cout << "[ERROR] " << e.what() << endl;
	}
#endif
#if defined(MYSQL_TEST)
	cout << "- MySQL: " << endl;
	auto& mysql = newSession(MYSQL);
	try {
		mysql.open({ {"host","127.0.0.1"},{"user","root"},{"passwd","root"} });
		mysql.setCurrentDB("test");
		mysql << "CREATE TABLE IF NOT EXISTS TEST (A TEXT,B INTEGER);", into();
		std::vector<test> res;
		mysql << "SELECT * FROM `TEST`", into(res);
		for (auto& t : res) {
			cout << "- " << t.a << ' ' << t.b << endl;
		}
		std::vector<dbname> dbs;
		mysql << "SHOW DATABASES", into(dbs);
		for (auto& t : dbs) {
			cout << "* " << t.db << endl;
		}
		mysql.release();
	}
	catch (std::exception e) {
		cout << "[ERROR] " << e.what() << endl;
	}
#endif
	return 0;
}