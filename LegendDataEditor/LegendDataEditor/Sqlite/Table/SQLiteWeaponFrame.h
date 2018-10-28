﻿#ifndef _SQLITE_WEAPON_FRAME
#define _SQLITE_WEAPON_FRAME

#include "SQLiteTable.h"

class WeaponFrameData : public SQLiteTableData
{
public:
	static std::string COL_ID;
	static std::string COL_LABEL;
	static std::string COL_DIRECTION;
	static std::string COL_ACTION;
	static std::string COL_FRAME_COUNT;
	static std::string COL_POSX;
	static std::string COL_POSY;
	int mID;
	std::string mLabel;
	int mDirection;
	std::string mAction;
	int mFrameCount;
	txVector<int> mPosX;
	txVector<int> mPosY;
public:
	virtual void parse(SQLiteDataReader* reader, SQLiteTable* table)
	{
		mID = reader->getInt(table->getCol(COL_ID));
		mLabel = reader->getString(table->getCol(COL_LABEL));
		mDirection = reader->getInt(table->getCol(COL_DIRECTION));
		mAction = reader->getString(table->getCol(COL_ACTION));
		mFrameCount = reader->getInt(table->getCol(COL_FRAME_COUNT));
		StringUtility::stringToIntArray(reader->getString(table->getCol(COL_POSX)), mPosX);
		StringUtility::stringToIntArray(reader->getString(table->getCol(COL_POSY)), mPosY);
	}
	static void registeColumn(SQLiteTable* table)
	{
		table->registeColumn(COL_ID);
		table->registeColumn(COL_LABEL);
		table->registeColumn(COL_DIRECTION);
		table->registeColumn(COL_ACTION);
		table->registeColumn(COL_FRAME_COUNT);
		table->registeColumn(COL_POSX);
		table->registeColumn(COL_POSY);
	}
};

class SQLiteWeaponFrame : SQLiteTable
{
public:
	SQLiteWeaponFrame(SQLite* sqlite)
		:SQLiteTable("WeaponFrame", sqlite)
	{
		WeaponFrameData::registeColumn(this);
	}
	void query(int weaponID, txVector<WeaponFrameData>& dataList);
	void query(int weaponID, int direction, txVector<WeaponFrameData>& dataList);
	void query(int weaponID, int direction, const std::string& action, txVector<WeaponFrameData>& dataList);
	bool updateData(const WeaponFrameData& data);
	bool insert(const WeaponFrameData& data);
};

#endif