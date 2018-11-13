﻿#ifndef _SQLITE_H_
#define _SQLITE_H_

#include "ServerDefine.h"

class SQLiteEquip;
class SQLiteEquipFrame;
class SQLiteMonster;
class SQLiteMonsterFrame;
class SQLiteEffect;
class SQLiteEffectFrame;
class SQLiteDataReader;
class SQLiteSceneMap;
class SQLite
{
public:
	sqlite3* mSQlite3;
	SQLiteEquip* mSQLiteEquip;
	SQLiteEquipFrame* mSQLiteEquipFrame;
	SQLiteMonster* mSQLiteMonster;
	SQLiteMonsterFrame* mSQLiteMonsterFrame;
	SQLiteEffect* mSQLiteEffect;
	SQLiteEffectFrame* mSQLiteEffectFrame;
	SQLiteSceneMap* mSQLiteSceneMap;
public:
	SQLite(const std::string& dbFileName);
	~SQLite() { destroy(); }
	void destroy();
	bool executeNonQuery(const std::string& queryString);
	SQLiteDataReader* executeQuery(const std::string& queryString);
	void releaseReader(SQLiteDataReader* reader);
};

#endif