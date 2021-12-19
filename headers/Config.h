#pragma once
#include <string>
#include <unordered_map>

const static std::string DLL_PREFIX = "LLDB-";
const static std::string DLL_SUFFIX = ".dll";

const static std::string DT_SQLITE  = "SQLite";
const static std::string DT_MYSQL   = "MySQL";
const static std::string DT_PGSQL   = "PostgreSQL";
const static std::string DT_ORACLE  = "Oracle";
const static std::string DT_ODBC    = "ODBC";
const static std::string DT_UNKNOWN = "Unknown";

const static std::unordered_map<std::string, std::string> CREATION_FUNCTIONS {
	{DT_SQLITE, "newSQLiteSession"},
	{DT_MYSQL, "newMySQLSession"},
	{DT_PGSQL, "newPgSQLSession"},
	{DT_ORACLE, "newOracleSession"},
	{DT_ODBC, "newODBCSession"}
};