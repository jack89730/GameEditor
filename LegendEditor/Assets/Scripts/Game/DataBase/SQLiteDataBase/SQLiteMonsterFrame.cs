﻿using Mono.Data.Sqlite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public class MonsterFrameData
{
	public string mLabel;
	public int mID;
	public int mDirection;
	public string mAction;
	public int mFrameCount;
	public float[] mPosX;
	public float[] mPosY;
}

public class SQLiteMonsterFrame : SQLiteTable
{
	string COL_LABEL = "MonsterLabel";
	string COL_ID = "MonsterID";
	string COL_DIRECTION = "Direction";
	string COL_ACTION = "Action";
	string COL_FRAME_COUNT = "FrameCount";
	string COL_POSX = "PosX";
	string COL_POSY = "PosY";
	public SQLiteMonsterFrame(SQLite sqlite)
		:base("MonsterFrame", sqlite)
	{}
	public void query(int monsterID, out List<MonsterFrameData> dataList)
	{
		string queryStr = "SELECT * FROM " + mTableName + " WHERE ";
		queryStr += COL_ID + " = " + StringUtility.intToString(monsterID);
		parseReader(mSQLite.query(queryStr), out dataList);
	}
	public void query(int monsterID, int direction, out List<MonsterFrameData> dataList)
	{
		string queryStr = "SELECT * FROM " + mTableName + " WHERE ";
		queryStr += COL_ID + " = " + StringUtility.intToString(monsterID) + " and " + 
					COL_DIRECTION + " = " + StringUtility.intToString(direction);
		parseReader(mSQLite.query(queryStr), out dataList);
	}
	public void query(int monsterID, int direction, string action, out List<MonsterFrameData> dataList)
	{
		string queryStr = "SELECT * FROM " + mTableName + " WHERE ";
		queryStr += COL_ID + " = " + StringUtility.intToString(monsterID) + " and " +
					COL_DIRECTION + " = " + StringUtility.intToString(direction) + " and " + 
					COL_ACTION + " = " + "\"" + action + "\"";
		parseReader(mSQLite.query(queryStr), out dataList);
	}
	//--------------------------------------------------------------------------------------------------------------------
	protected void parseReader(SqliteDataReader reader, out List<MonsterFrameData> dataList)
	{
		dataList = new List<MonsterFrameData>();
		while (reader.Read())
		{
			MonsterFrameData data = new MonsterFrameData();
			data.mLabel = reader[COL_LABEL].ToString();
			data.mID = StringUtility.stringToInt(reader[COL_ID].ToString());
			data.mDirection = StringUtility.stringToInt(reader[COL_DIRECTION].ToString());
			data.mAction = reader[COL_ACTION].ToString();
			data.mFrameCount = StringUtility.stringToInt(reader[COL_FRAME_COUNT].ToString());
			StringUtility.stringToFloatArray(reader[COL_POSX].ToString(), ref data.mPosX);
			StringUtility.stringToFloatArray(reader[COL_POSY].ToString(), ref data.mPosY);
			dataList.Add(data);
		}
		reader.Close();
	}
};