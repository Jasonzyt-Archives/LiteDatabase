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

int main(int argc, char** argv) {
	SetCurrentDirectory(L"..\\x64\\Release");
	cout << "================ LLDB Test ================" << endl;
	cout << "[DEBUG] argv[0]: " << argv[0] << endl;
	cout << "- SQLite: " << endl;
	auto& sess = newSession(DatabaseType::SQLITE);
	try {
		sess.open({ {"target", "./test.db"}, {"create", true} });
		sess.exec("CREATE TABLE IF NOT EXISTS \"TEST\" (A TEXT,B INTEGER);");
		auto res = sess.selectAll("TEST");
		for (auto& row : res.getAll()) {
			auto c1 = row.get("A");
			cout << "- A: " << get<std::string>(c1) << endl;
			auto c2 = row.get("B");
			cout << "- B: " << getInteger<uint64_t>(c2) << endl;
		}
		cout << endl;
		Once onc(sess);
		onc << "INSERT INTO `$` VALUES('$$k',$)", 
			use("TEST", "123456");
		cout << onc.getSQL() << endl;
		sess.close();
		getchar();
	}
	catch (std::exception e) {
		cout << "[ERROR] " << e.what() << endl;
	}
	return 0;
}