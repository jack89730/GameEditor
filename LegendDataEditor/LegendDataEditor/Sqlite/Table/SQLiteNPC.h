﻿#ifndef _SQLITE_NPC_H_
#define _SQLITE_NPC_H_

#include "SQLiteTable.h"

class NPCData : public SQLiteTableData
{
public:
	static string COL_ID;
	static string COL_SCRIPT;
	static string COL_MAP;
	static string COL_POSX;
	static string COL_POSY;
	static string COL_LABEL;
	static string COL_FLAG;
	static string COL_APPERANCE;
public:
	int mID;
	string mScript;
	string mMap;
	int mPosX;
	int mPosY;
	string mLabel;
	int mFlag;
	int mApperance;
public:
	virtual void parse(SQLiteDataReader* reader, SQLiteTable* table)
	{
		mID = reader->getInt(table->getCol(COL_ID));
		mScript = reader->getString(table->getCol(COL_SCRIPT));
		mMap = reader->getString(table->getCol(COL_MAP));
		mPosX = reader->getInt(table->getCol(COL_POSX));
		mPosY = reader->getInt(table->getCol(COL_POSY));
		mLabel = reader->getString(table->getCol(COL_LABEL));
		mFlag = reader->getInt(table->getCol(COL_FLAG));
		mApperance = reader->getInt(table->getCol(COL_APPERANCE));
	}
	virtual void insert(string& valueString) const
	{
		StringUtility::appendValueInt(valueString, mID);
		StringUtility::appendValueString(valueString, mScript);
		StringUtility::appendValueString(valueString, mMap);
		StringUtility::appendValueInt(valueString, mPosX);
		StringUtility::appendValueInt(valueString, mPosY);
		StringUtility::appendValueString(valueString, mLabel);
		StringUtility::appendValueInt(valueString, mFlag);
		StringUtility::appendValueInt(valueString, mApperance);
	}
	virtual void update(string& updateString) const
	{
		StringUtility::appendUpdateInt(updateString, COL_ID, mID);
		StringUtility::appendUpdateString(updateString, COL_SCRIPT, mScript);
		StringUtility::appendUpdateString(updateString, COL_MAP, mMap);
		StringUtility::appendUpdateInt(updateString, COL_POSX, mPosX);
		StringUtility::appendUpdateInt(updateString, COL_POSY, mPosY);
		StringUtility::appendUpdateString(updateString, COL_LABEL, mLabel);
		StringUtility::appendUpdateInt(updateString, COL_FLAG, mFlag);
		StringUtility::appendUpdateInt(updateString, COL_APPERANCE, mApperance);
	}
	static void registeColumn(SQLiteTable* table)
	{
		table->registeColumn(COL_ID);
		table->registeColumn(COL_SCRIPT);
		table->registeColumn(COL_MAP);
		table->registeColumn(COL_POSX);
		table->registeColumn(COL_POSY);
		table->registeColumn(COL_LABEL);
		table->registeColumn(COL_FLAG);
		table->registeColumn(COL_APPERANCE);
	}
};

class SQLiteNPC : public SQLiteTable
{
public:
	SQLiteNPC(SQLite* sqlite)
		:SQLiteTable("NPC", sqlite)
	{
		NPCData::registeColumn(this);
	}
	void query(txVector<NPCData>& dataList);
	bool insert(const NPCData& data);
};

#endif