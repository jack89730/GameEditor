#ifndef _COMMAND_EDITOR_FRAME_NOTIFY_SELECTION_CHANGED_H_
#define _COMMAND_EDITOR_FRAME_NOTIFY_SELECTION_CHANGED_H_

#include "EditorCommand.h"

class CommandEditorFrameNotifySelectionChanged : public EditorCommand
{
public:
	virtual void reset(){}
	virtual void execute();
	virtual std::string showDebugInfo();
public:
};

#endif