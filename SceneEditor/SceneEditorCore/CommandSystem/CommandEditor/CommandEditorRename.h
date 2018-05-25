#ifndef _COMMAND_EDITOR_RENAME_H_
#define _COMMAND_EDITOR_RENAME_H_

#include "EditorCoreCommand.h"

class CommandEditorRename : public EditorCoreCommand
{
public:
	virtual void reset()
	{
		mNewName = EMPTY_STRING;
	}
	virtual void execute();
	virtual std::string showDebugInfo();
public:
	std::string mNewName;
};

#endif