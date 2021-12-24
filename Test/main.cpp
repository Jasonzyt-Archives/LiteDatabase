#include <iostream>
#include <filesystem>
#include <Session.h>
#include <Results.h>
#include <Utils.h>
#include <Once.h>
#include <Windows.h>

using namespace std;
using namespace LLDB;

namespace fs = std::filesystem;

struct test {
	std::string a;
	uint64_t b;
};

void from_row(Row& row, test& t) {
	t.a = row["A"].getString();
	t.b = row["B"].getNumber<uint64_t>();
}

int main(int argc, char** argv) {
	SetCurrentDirectory(L"..\\x64\\Release");
	cout << "================ LLDB Test ================" << endl;
	cout << "[DEBUG] argv[0]: " << argv[0] << endl;
	cout << "- SQLite: " << endl;
	auto& sess = newSession(SQLITE);
	try {
		sess.open({ {"target", "./test.db"}, {"create", true} });
		sess.exec("CREATE TABLE IF NOT EXISTS \"TEST\" (A TEXT,B INTEGER);");
		Results res;
		sess << "SELECT * FROM TEST";//, into(res);
		for (auto& row : res) {
			auto c1 = row.get("A");
			cout << "- A: " << c1.getString() << endl;
			auto c2 = row.get("B");
			cout << "- B: " << c2.getNumber<uint64_t>() << endl;
		}
		cout << endl;
		std::vector<test> ress;
		Once onc(sess);
		onc << "select * from TEST where A='awa';select * from TEST where A='?'", use("wdnmd"), into(ress);
		for (auto& a : ress) {
			cout << a.a << a.b << endl;
		}
		sess.release();
	}
	catch (std::exception e) {
		cout << "[ERROR] " << e.what() << endl;
	}
	return 0;
}