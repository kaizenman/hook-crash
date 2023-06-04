#pragma once

#include "pch.h"

class Reactors
{
public:
    static void Init();
    static void Shutdown();

    static MyPublishReactor& GetPublishReactor() { return s_PublishReactor; }
    
    
    static MyDatabaseReactor* s_DbDatabaseReactor;
    static MyEditorReactor* s_EditorReactor;
    static MyDocManagerReactor* s_DocManagerReactor;
    static MyPlotReactor* s_PlotReactor;
    static MyPublishReactor s_PublishReactor;
};
