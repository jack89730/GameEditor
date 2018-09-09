#include "EditorCoreRoot.h"
#include "SceneEditorCore.h"

#include "EditorCommandHeader.h"
#include "EditorFrame.h"
#include "SceneHierarchy.h"

void CommandEditorFrameNotifySceneOpen::execute()
{
	EditorFrame* editorFrame = CMD_CAST<EditorFrame*>(mReceiver);
	// ��ʾ������ͼ
	mSceneHierarchy->showScene(mSceneEditorCore->getCurScene());
}

std::string CommandEditorFrameNotifySceneOpen::showDebugInfo()
{
	COMMAND_DEBUG("scene name : %s", mSceneName.c_str());
}