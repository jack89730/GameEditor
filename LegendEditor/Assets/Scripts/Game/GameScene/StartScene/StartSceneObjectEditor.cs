﻿using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class StartSceneObjectEditor : SceneProcedure
{
	public StartSceneObjectEditor(PROCEDURE_TYPE type, GameScene gameScene)
		:
	base(type, gameScene)
	{
		;
	}
	protected override void onInit(SceneProcedure lastProcedure, string intent)
	{
		LayoutTools.LOAD_NGUI_SHOW(LAYOUT_TYPE.LT_OBJECT_EDITOR, 0);
	}
	protected override void onUpdate(float elapsedTime)
	{
		;
	}
	protected override void onExit(SceneProcedure nextProcedure)
	{
		LayoutTools.UNLOAD_LAYOUT(LAYOUT_TYPE.LT_OBJECT_EDITOR);
	}
	protected override void onKeyProcess(float elapsedTime)
	{
		;
	}
}