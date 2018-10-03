#include "Utility.h"
#include "txSerializer.h"
#include "HumanImage.h"
#include "WeaponImage.h"
#include "SQLite.h"
#include "SQLiteCloth.h"
#include "SQLiteClothFrame.h"
#include "SQLiteMonster.h"
#include "SQLiteMonsterFrame.h"
#include "SQLiteWeapon.h"
#include "SQLiteWeaponFrame.h"
#include "HumanAction.h"
#include "WeaponAction.h"

void ImageUtility::encodePNG(const std::string& path, char* color, int width, int height, FREE_IMAGE_FORMAT format)
{
	std::string dir = StringUtility::getFilePath(path);
	FileUtility::createFolder(dir);
	FreeImage_Initialise();
	FIBITMAP* bitmap = FreeImage_Allocate(width, height, 32);
	BYTE* bits = FreeImage_GetBits(bitmap);
	memcpy(bits, color, width * height * 4);
	FreeImage_Save(format, bitmap, path.c_str());
	FreeImage_Unload(bitmap);
	FreeImage_DeInitialise();
}

bool ImageUtility::readWixFile(const std::string& filePath, WIXFileImageInfo& info)
{
	int fileSize = 0;
	char* fileBuffer = FileUtility::openBinaryFile(filePath, &fileSize);
	if (fileBuffer == NULL)
	{
		return false;
	}
	txSerializer serializer(fileBuffer, fileSize);
	serializer.readBuffer(info.mStrHeader, 44);
	serializer.read(info.mIndexCount);
	for (int i = 0; i < info.mIndexCount; ++i)
	{
		int curStartPos = 0;
		if (serializer.read(curStartPos))
		{
			info.mPositionList.push_back(curStartPos);
		}
	}
	TRACE_DELETE_ARRAY(fileBuffer);
	return true;
}

bool ImageUtility::readWilHeader(const std::string& filePath, WILFileHeader& header)
{
	int fileSize = 0;
	char* fileBuffer = FileUtility::openBinaryFile(filePath, &fileSize);
	if (fileBuffer == NULL)
	{
		return false;
	}
	txSerializer serializer(fileBuffer, fileSize);
	serializer.readBuffer(header.mInfo, 44);
	serializer.readBuffer(header.mPlayInfo, 12);
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			serializer.read(header.mColor[i][j]);
		}
	}
	TRACE_DELETE_ARRAY(fileBuffer);
	return true;
}

void ImageUtility::wixWilToPNG(const std::string& wixFileName, const std::string& wilFileName, const std::string& outputPath)
{
	// ��wix�ļ�
	WIXFileImageInfo wixFileHeader;
	if (!readWixFile(wixFileName, wixFileHeader))
	{
		std::cout << "�Ҳ���wix�ļ�" << std::endl;
		return;
	}

	// ��wil�ļ�
	WILFileHeader wilHeader;
	if(!readWilHeader(wilFileName, wilHeader))
	{
		std::cout << "�Ҳ���wil�ļ�" << std::endl;
		return;
	}

	POINT* posList = TRACE_NEW_ARRAY(POINT, wixFileHeader.mPositionList.size(), posList);
	int fileSize = 0;
	char* fileBuffer = FileUtility::openBinaryFile(wilFileName, &fileSize);
	txSerializer serializer(fileBuffer, fileSize);
	for (int i = 0; i < wixFileHeader.mPositionList.size(); ++i)
	{
		// ���±����õ���ǰͼƬ��ʼλ��,���Ҷ�ȡͼƬ��Ϣ
		int startPos = wixFileHeader.mPositionList[i];
		if (startPos == 0)
		{
			continue;
		}
		serializer.setIndex(startPos);
		WILFileImageInfo curInfo;
		// ����,λ��ƫ��
		serializer.read(curInfo.mWidth);
		serializer.read(curInfo.mHeight);
		serializer.read(curInfo.mPosX);
		serializer.read(curInfo.mPosY);
		// ������ɫ�����ڵ�ɫ���л�ȡ��ɫ����
		int pixelCount = curInfo.mWidth * curInfo.mHeight;
		TRACE_NEW_ARRAY(char, pixelCount * 4, curInfo.mColor);
		for (int j = 0; j < pixelCount; ++j)
		{
			if (startPos + ImageHeaderLength + j >= fileSize)
			{
				std::cout << "error" <<std::endl;
				break;
			}
			unsigned char pixelIndex = fileBuffer[startPos + ImageHeaderLength + j];
			// 0��,1��,2��
			curInfo.mColor[j * 4 + 0] = wilHeader.mColor[pixelIndex][0];// ��
			curInfo.mColor[j * 4 + 1] = wilHeader.mColor[pixelIndex][1];// ��
			curInfo.mColor[j * 4 + 2] = wilHeader.mColor[pixelIndex][2];// ��
			curInfo.mColor[j * 4 + 3] = pixelIndex == 0 ? 0 : (char)255;
		}
		posList[i].x = curInfo.mPosX;
		posList[i].y = curInfo.mPosY;
		// ��ͼƬת��Ϊpng
		encodePNG(outputPath + StringUtility::intToString(i) + ".png", curInfo.mColor, curInfo.mWidth, curInfo.mHeight, FIF_PNG);
		TRACE_DELETE_ARRAY(curInfo.mColor);
	}
	TRACE_DELETE_ARRAY(fileBuffer);

	writePositionFile(outputPath + "position.txt", posList, wixFileHeader.mPositionList.size());
	TRACE_DELETE_ARRAY(posList);
}

void ImageUtility::writePositionFile(const std::string& positionFile, POINT* posList, int posCount)
{
	std::string posStr;
	for (int i = 0; i < posCount; ++i)
	{
		posStr += StringUtility::intToString(posList[i].x);
		posStr += ",";
		posStr += StringUtility::intToString(posList[i].y);
		posStr += "\n";
	}
	FileUtility::writeFile(positionFile, posStr);
}

POINT* ImageUtility::readPositionFile(const std::string& positionFile, int& posCount)
{
	std::string posStr = FileUtility::openTxtFile(positionFile);
	txVector<std::string> posStrList;
	StringUtility::split(posStr, "\n", posStrList);
	posCount = posStrList.size();
	POINT* posList = TRACE_NEW_ARRAY(POINT, posCount, posList);
	for (int i = 0; i < posCount; ++i)
	{
		txVector<std::string> pointList;
		StringUtility::split(posStrList[i], ",", pointList);
		if (pointList.size() != 2)
		{
			continue;
		}
		posList[i].x = StringUtility::stringToInt(pointList[0]);
		posList[i].y = StringUtility::stringToInt(pointList[1]);
	}
	return posList;
}

void ImageUtility::autoGroupHumanImage(const std::string& path)
{
	// �Ȳ��λ���ļ�
	splitPositionFile(path);
	// ����600���ļ�һ��,���뵥�����ļ�����
	autoMoveFile(path, HUMAN_GROUP_SIZE);

	txVector<std::string> folderList;
	FileUtility::findFolders(path, folderList);
	int folderCount = folderList.size();
	for (int i = 0; i < folderCount; ++i)
	{
		// ���ն���������
		txVector<std::string> fileList;
		FileUtility::findFiles(folderList[i], fileList, ".png", false);
		sortByFileNumber(fileList);
		int fileCount = fileList.size();
		for (int j = 0; j < fileCount; ++j)
		{
			std::string actionName;
			int direction;
			int frameIndex;
			bool isValid = getHumanActionInfo(j, actionName, direction, frameIndex);
			// �������ЧͼƬ����Ҫɾ��
			if (!isValid)
			{
				deleteImageWithPosition(fileList[j]);
			}
			std::string actionFolderName = actionName + "_dir" + StringUtility::intToString(direction);
			std::string destPath = StringUtility::getFilePath(fileList[j]) + "/" + actionFolderName + "/";
			moveImageWithPosition(fileList[j], destPath + actionFolderName + "_" + StringUtility::intToString(frameIndex) + StringUtility::getFileSuffix(fileList[j], true));
		}
	}
}

void ImageUtility::autoGroupWeaponImage(const std::string& path)
{
	// �Ȳ��λ���ļ�
	splitPositionFile(path);
	// ����600���ļ�һ��,���뵥�����ļ�����
	autoMoveFile(path, WEAPON_GROUP_SIZE);
	// ��Ϊ�����ļ��ǽ�ɫ�ļ���2��,�����������ֵ�����,������ʱֻ����ǰһ����ļ�,�����ɾ��
	txVector<std::string> folderList;
	FileUtility::findFolders(path, folderList);
	int folderCount = folderList.size();
	for (int i = 0; i < folderCount; ++i)
	{
		// ���ն���������
		txVector<std::string> fileList;
		FileUtility::findFiles(folderList[i], fileList, ".png", false);
		sortByFileNumber(fileList);
		int fileCount = fileList.size();
		for (int j = 0; j < fileCount; ++j)
		{
			if (WEAPON_GROUP_SIZE == 1200 && j >= WEAPON_GROUP_SIZE / 2)
			{
				deleteImageWithPosition(fileList[j]);
			}
			else
			{
				std::string actionName;
				int direction;
				int frameIndex;
				bool isValid = getHumanActionInfo(j, actionName, direction, frameIndex);
				// �������ЧͼƬ����Ҫɾ��
				if (!isValid)
				{
					deleteImageWithPosition(fileList[j]);
				}
				std::string actionFolderName = actionName + "_dir" + StringUtility::intToString(direction);
				std::string destPath = StringUtility::getFilePath(fileList[j]) + "/" + actionFolderName + "/";
				moveImageWithPosition(fileList[j], destPath + actionFolderName + "_" + StringUtility::intToString(frameIndex) + StringUtility::getFileSuffix(fileList[j], true));
			}
		}
	}
}

void ImageUtility::autoGroupMonsterImage0(const std::string& path)
{
	// ���λ���ļ�
	ImageUtility::splitPositionFile(path);
	// Ȼ��360���ļ�һ��,�ƶ����������ļ���
	ImageUtility::autoMoveFile(path, MONSTER_GROUP_SIZE);
	// ����ͼƬ�����ɫ������ͼƬ�������й�������,����ֻ���ڴ��·�����ֶ���ÿ���ļ��н��ж�������
}

void ImageUtility::autoGroupMonsterImage1(const std::string& path)
{
	// �ֶ��Զ������з����,�Ϳ��Զ�ÿ�鶯���ļ����з������
	// �������ļ�,��ÿ���ļ����е�ͼƬ��������Ϊ���ļ����е�λ�����
	ImageUtility::renameImage(path);
	// �Զ����㷽�򲢷���
	ImageUtility::renameByDirection(path);
}

void ImageUtility::saveFrameInfo(const std::string& path, IMAGE_TYPE imageType, SQLite* sqlite)
{
	if (imageType == IT_MONSTER)
	{
		// �����ļ����е�����ͼƬ
		txVector<std::string> folderList;
		FileUtility::findFolders(path, folderList);
		int folderCount = folderList.size();
		for (int i = 0; i < folderCount; ++i)
		{
			MonsterImageGroup imageGroup;
			txVector<std::string> fileList;
			FileUtility::findFiles(folderList[i], fileList, ".png");
			int fileCount = fileList.size();
			for (int j = 0; j < fileCount; ++j)
			{
				std::string posFileName = StringUtility::getFilePath(fileList[j]) + "/" + StringUtility::getFileNameNoSuffix(fileList[j]) + ".txt";
				std::string posFile = FileUtility::openTxtFile(posFileName);
				txVector<int> posValue;
				StringUtility::stringToIntArray(posFile, posValue);
				if (posValue.size() != 2)
				{
					std::cout << "λ���ļ����ݴ��� : " << fileList[j] << std::endl;
					break;
				}
				MonsterImage monsterImage;
				monsterImage.mLabel = StringUtility::getFileName(folderList[i]);
				monsterImage.mPosX = posValue[0];
				monsterImage.mPosY = posValue[1];
				monsterImage.mMonsterID = i + 1;
				monsterImage.setFileName(StringUtility::getFileNameNoSuffix(fileList[j]));
				imageGroup.addImage(monsterImage);
			}
			// ������֡������,�������ݺ�д�����ݿ�
			writeSQLite(imageGroup.mAllAction, sqlite);
		}
	}
	else if (imageType == IT_HUMAN)
	{
		// �����ļ����е�����ͼƬ
		txVector<std::string> folderList;
		FileUtility::findFolders(path, folderList);
		int folderCount = folderList.size();
		for (int i = 0; i < folderCount; ++i)
		{
			HumanImageGroup imageGroup;
			txVector<std::string> fileList;
			FileUtility::findFiles(folderList[i], fileList, ".png");
			int fileCount = fileList.size();
			for (int j = 0; j < fileCount; ++j)
			{
				std::string posFileName = StringUtility::getFilePath(fileList[j]) + "/" + StringUtility::getFileNameNoSuffix(fileList[j]) + ".txt";
				std::string posFile = FileUtility::openTxtFile(posFileName);
				txVector<int> posValue;
				StringUtility::stringToIntArray(posFile, posValue);
				if (posValue.size() != 2)
				{
					std::cout << "λ���ļ����ݴ��� : " << fileList[j] << std::endl;
					break;
				}
				HumanImage monsterImage;
				monsterImage.mLabel = StringUtility::getFileName(folderList[i]);
				monsterImage.mPosX = posValue[0];
				monsterImage.mPosY = posValue[1];
				monsterImage.mClothID = i + 1;
				monsterImage.setFileName(StringUtility::getFileNameNoSuffix(fileList[j]));
				imageGroup.addImage(monsterImage);
			}
			// ������֡������,�������ݺ�д�����ݿ�
			writeSQLite(imageGroup.mAllAction, sqlite);
		}
	}
	else if (imageType == IT_WEAPON)
	{
		// �����ļ����е�����ͼƬ
		txVector<std::string> folderList;
		FileUtility::findFolders(path, folderList);
		int folderCount = folderList.size();
		for (int i = 0; i < folderCount; ++i)
		{
			WeaponImageGroup imageGroup;
			txVector<std::string> fileList;
			FileUtility::findFiles(folderList[i], fileList, ".png");
			int fileCount = fileList.size();
			for (int j = 0; j < fileCount; ++j)
			{
				std::string posFileName = StringUtility::getFilePath(fileList[j]) + "/" + StringUtility::getFileNameNoSuffix(fileList[j]) + ".txt";
				std::string posFile = FileUtility::openTxtFile(posFileName);
				txVector<int> posValue;
				StringUtility::stringToIntArray(posFile, posValue);
				if (posValue.size() != 2)
				{
					std::cout << "λ���ļ����ݴ��� : " << fileList[j] << std::endl;
					break;
				}
				WeaponImage monsterImage;
				monsterImage.mLabel = StringUtility::getFileName(folderList[i]);
				monsterImage.mPosX = posValue[0];
				monsterImage.mPosY = posValue[1];
				monsterImage.mWeaponID = i + 1;
				monsterImage.setFileName(StringUtility::getFileNameNoSuffix(fileList[j]));
				imageGroup.addImage(monsterImage);
			}
			// ������֡������,�������ݺ�д�����ݿ�
			writeSQLite(imageGroup.mAllAction, sqlite);
		}
	}
}

void ImageUtility::writeSQLite(txMap<std::string, WeaponActionSet>& actionSetList, SQLite* sqlite)
{
	// ������֡������,�������ݺ�д�����ݿ�
	// �������ж���
	auto iter = actionSetList.begin();
	auto iterEnd = actionSetList.end();
	for (; iter != iterEnd; ++iter)
	{
		// �����ö��������з���
		for (int j = 0; j < DIRECTION_COUNT; ++j)
		{
			WeaponActionAnim& actionAnim = iter->second.mDirectionAction[j];
			WeaponFrameData data;
			data.mID = actionAnim.mImageFrame[0].mWeaponID;
			data.mLabel = StringUtility::ANSIToUTF8(actionAnim.mImageFrame[0].mLabel);
			data.mDirection = j;
			data.mAction = iter->first;
			data.mFrameCount = actionAnim.mImageFrame.size();
			// �����ö���������֡��
			for (int kk = 0; kk < data.mFrameCount; ++kk)
			{
				data.mPosX.push_back(actionAnim.mImageFrame[kk].mPosX);
				data.mPosY.push_back(actionAnim.mImageFrame[kk].mPosY);
			}
			bool ret = sqlite->mSQLiteWeaponFrame->insertOrUpdate(data);
			if (!ret)
			{
				break;
			}
		}
	}
}

void ImageUtility::writeSQLite(txMap<std::string, HumanActionSet>& actionSetList, SQLite* sqlite)
{
	auto iter = actionSetList.begin();
	auto iterEnd = actionSetList.end();
	for (; iter != iterEnd; ++iter)
	{
		// �����ö��������з���
		for (int j = 0; j < DIRECTION_COUNT; ++j)
		{
			HumanActionAnim& actionAnim = iter->second.mDirectionAction[j];
			ClothFrameData data;
			data.mID = actionAnim.mImageFrame[0].mClothID;
			data.mLabel = StringUtility::ANSIToUTF8(actionAnim.mImageFrame[0].mLabel);
			data.mDirection = j;
			data.mAction = iter->first;
			data.mFrameCount = actionAnim.mImageFrame.size();
			// �����ö���������֡��
			for (int kk = 0; kk < data.mFrameCount; ++kk)
			{
				data.mPosX.push_back(actionAnim.mImageFrame[kk].mPosX);
				data.mPosY.push_back(actionAnim.mImageFrame[kk].mPosY);
			}
			bool ret = sqlite->mSQLiteClothFrame->insertOrUpdate(data);
			if (!ret)
			{
				break;
			}
		}
	}
}

void ImageUtility::writeSQLite(txMap<std::string, MonsterActionSet>& actionSetList, SQLite* sqlite)
{
	auto iter = actionSetList.begin();
	auto iterEnd = actionSetList.end();
	for (; iter != iterEnd; ++iter)
	{
		// �����ö��������з���
		for (int j = 0; j < DIRECTION_COUNT; ++j)
		{
			MonsterActionAnim& actionAnim = iter->second.mDirectionAction[j];
			MonsterFrameData data;
			data.mID = actionAnim.mImageFrame[0].mMonsterID;
			data.mLabel = StringUtility::ANSIToUTF8(actionAnim.mImageFrame[0].mLabel);
			data.mDirection = j;
			data.mAction = iter->first;
			data.mFrameCount = actionAnim.mImageFrame.size();
			// �����ö���������֡��
			for (int kk = 0; kk < data.mFrameCount; ++kk)
			{
				data.mPosX.push_back(actionAnim.mImageFrame[kk].mPosX);
				data.mPosY.push_back(actionAnim.mImageFrame[kk].mPosY);
			}
			bool ret = sqlite->mSQLiteMonsterFrame->insertOrUpdate(data);
			if (!ret)
			{
				break;
			}
		}
	}
}

void ImageUtility::renameImage(const std::string& path)
{
	// ��Ŀ¼�е��ļ����ļ��������,������Ϊ��0��ʼ������
	txVector<std::string> folderList;
	FileUtility::findFolders(path, folderList, true);
	int folderCount = folderList.size();
	for (int i = 0; i < folderCount; ++i)
	{
		txVector<std::string> fileList;
		FileUtility::findFiles(folderList[i], fileList, ".png");
		// �ȸ����ļ�����������
		sortByFileNumber(fileList);
		int count = fileList.size();
		for (int j = 0; j < count; ++j)
		{
			std::string curFilePath = StringUtility::getFilePath(fileList[j]) + "/";
			std::string suffix = StringUtility::getFileSuffix(fileList[j]);
			renameImageWithPosition(fileList[j], curFilePath + StringUtility::intToString(j) + "." + suffix);
		}
	}
}

void ImageUtility::splitPositionFile(const std::string& path)
{
	// ��position.txt�ļ����Ϊ������txt�ļ�,ÿ��txt�ļ���ֻ����һ������
	int posCount = 0;
	POINT* posList = readPositionFile(path + "/position.txt", posCount);
	for (int i = 0; i < posCount; ++i)
	{
		std::string posStr = StringUtility::intToString(posList[i].x) + "," + StringUtility::intToString(posList[i].y);
		FileUtility::writeFile(path + "/" + StringUtility::intToString(i) + ".txt", posStr);
	}
	TRACE_DELETE_ARRAY(posList);
}

void ImageUtility::renameByDirection(const std::string& path)
{
	// ��Ŀ¼�е������ļ��Ȱ����ļ�������,Ȼ����˳�����Ϊ8������,�ٶ�ÿ��������ļ�������
	txVector<std::string> folderList;
	FileUtility::findFolders(path, folderList, true);
	int folderCount = folderList.size();
	for (int i = 0; i < folderCount; ++i)
	{
		txVector<std::string> fileList;
		FileUtility::findFiles(folderList[i], fileList, ".png", false);
		sortByFileNumber(fileList);
		int fileCount = fileList.size();
		int actionFrameCount = fileCount / DIRECTION_COUNT;
		for (int j = 0; j < fileCount; ++j)
		{
			if (fileCount % DIRECTION_COUNT != 0)
			{
				std::cout << "ͼƬ��������,����Ϊ�����������" << std::endl;
				break;
			}
			int imageDir = j / actionFrameCount;
			int index = j % actionFrameCount;
			// ���ļ��ƶ���һ���½��ļ�����
			std::string curPath = StringUtility::getFilePath(fileList[j]) + "/";
			std::string destFolderName = StringUtility::getFolderName(fileList[j]) + "_dir" + StringUtility::intToString(imageDir);
			std::string destPath = StringUtility::getFilePath(curPath) + "/" + destFolderName + "/";
			moveImageWithPosition(fileList[j], destPath + destFolderName + "_" + StringUtility::intToString(index) + "." + StringUtility::getFileSuffix(fileList[j]));
		}
	}
	// ɾ���յ�Ŀ¼
	FileUtility::deleteEmptyFolder(path);
}

void ImageUtility::sortByFileNumber(txVector<std::string>& fileList)
{
	// �����ļ��������ֽ�������
	txMap<int, std::string> sortedList;
	int count = fileList.size();
	for (int i = 0; i < count; ++i)
	{
		sortedList.insert(StringUtility::stringToInt(StringUtility::getFileNameNoSuffix(fileList[i])), fileList[i]);
	}
	if (sortedList.size() != fileList.size())
	{
		return;
	}
	fileList.clear();
	auto iter = sortedList.begin();
	auto iterEnd = sortedList.end();
	for (; iter != iterEnd; ++iter)
	{
		fileList.push_back(iter->second);
	}
}

void ImageUtility::autoMoveFile(const std::string& path, int groupSize)
{
	txVector<std::string> fileList;
	FileUtility::findFiles(path, fileList, ".png");
	sortByFileNumber(fileList);
	int fileCount = fileList.size();
	for (int i = 0; i < fileCount; ++i)
	{
		int groupIndex = i / groupSize;
		std::string destFolderName = StringUtility::intToString(groupIndex);
		std::string destPath = StringUtility::getFilePath(fileList[i]) + "/" + destFolderName + "/";
		moveImageWithPosition(fileList[i], destPath + StringUtility::getFileName(fileList[i]));
	}
}

bool ImageUtility::getHumanActionInfo(int index, std::string& actionName, int& dir, int& frameIndex)
{
	int i = 0;
	while (true)
	{
		if (index - HUMAN_ACTION[i].mMaxFrame * DIRECTION_COUNT < 0)
		{
			break;
		}
		index -= HUMAN_ACTION[i].mMaxFrame * DIRECTION_COUNT;
		++i;
	}
	// ��Ϊһ�鶯����Դ������8�������ϵ����ж���,���Կ��Ը����±��������������֡�±�,ǰ���Ǳ����˿�ͼƬ��Ϊ���λ��
	dir = index / HUMAN_ACTION[i].mMaxFrame;
	frameIndex = index % HUMAN_ACTION[i].mMaxFrame;
	actionName = HUMAN_ACTION[i].mName;
	return frameIndex < HUMAN_ACTION[i].mFrameCount;
}

void ImageUtility::moveImageWithPosition(const std::string& fullFileName, const std::string& destFullFileName)
{
	std::string sourceFileNameNoSuffix = StringUtility::getFileNameNoSuffix(fullFileName);
	std::string destFileNameNoSuffix = StringUtility::getFileNameNoSuffix(destFullFileName);
	std::string sourcePath = StringUtility::getFilePath(fullFileName) + "/";
	std::string destPath = StringUtility::getFilePath(destFullFileName) + "/";
	FileUtility::moveFile(fullFileName, destFullFileName);
	std::string positionFileName = sourcePath + sourceFileNameNoSuffix + ".txt";
	if (FileUtility::isFileExist(positionFileName))
	{
		FileUtility::moveFile(positionFileName, destPath + destFileNameNoSuffix + ".txt");
	}
}

void ImageUtility::renameImageWithPosition(const std::string& fullFileName, const std::string& destFullFileName)
{
	std::string sourceFileNameNoSuffix = StringUtility::getFileNameNoSuffix(fullFileName);
	std::string destFileNameNoSuffix = StringUtility::getFileNameNoSuffix(destFullFileName);
	std::string sourcePath = StringUtility::getFilePath(fullFileName) + "/";
	std::string destPath = StringUtility::getFilePath(destFullFileName) + "/";
	FileUtility::renameFile(fullFileName, destFullFileName);
	// �����ͬ��λ���ļ�,��Ҳ��Ҫһ��������
	std::string positionFileName = sourcePath + sourceFileNameNoSuffix + ".txt";
	if (FileUtility::isFileExist(positionFileName))
	{
		FileUtility::renameFile(positionFileName, destPath + destFileNameNoSuffix  + ".txt");
	}
}

void ImageUtility::deleteImageWithPosition(const std::string& fullFileName)
{
	std::string sourceFileNameNoSuffix = StringUtility::getFileNameNoSuffix(fullFileName);
	std::string sourcePath = StringUtility::getFilePath(fullFileName) + "/";
	FileUtility::deleteFile(fullFileName);
	// �����ͬ��λ���ļ�,��Ҳ��Ҫһ��������
	std::string positionFileName = sourcePath + sourceFileNameNoSuffix + ".txt";
	if (FileUtility::isFileExist(positionFileName))
	{
		FileUtility::deleteFile(positionFileName);
	}
}