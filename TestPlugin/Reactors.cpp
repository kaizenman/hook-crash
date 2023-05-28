#include "pch.h"

MyDatabaseReactor* Reactors::s_DbDatabaseReactor = nullptr;
MyEditorReactor* Reactors::s_EditorReactor = nullptr;
MyDocManagerReactor* Reactors::s_DocManagerReactor = nullptr;
MyPlotReactor* Reactors::s_PlotReactor = nullptr;
MyPublishReactor Reactors::s_PublishReactor;

void Reactors::Init()
{
    s_EditorReactor = new MyEditorReactor();
    acedEditor->addReactor(s_EditorReactor);
    
    s_DocManagerReactor = new MyDocManagerReactor();
    acDocManager->addReactor(s_DocManagerReactor);
    
    s_PlotReactor = new MyPlotReactor();
    acplPlotReactorMgrPtr()->addReactor(s_PlotReactor);

    s_DbDatabaseReactor = new MyDatabaseReactor();
    AcDbDatabase* pDwg = acDocManager->curDocument()->database(); 
    pDwg->addReactor(Reactors::s_DbDatabaseReactor);

    if (!acrxServiceIsRegistered(_T("AdskPublish")))
    {
        acrxLoadModule(_T("AcPublish.crx"), false, false);
    }

    HINSTANCE hInst = ::GetModuleHandle(_T("AcPublish.crx"));
    if (!hInst)
    {
        return;
    }
    ///    ACGLOBADDPUBLISHREACTOR pAcGlobAddPublishReactor = (ACGLOBADDPUBLISHREACTOR)GetProcAddress(hInst, AcGlobAddPublishReactor");
    ACGLOBADDPUBLISHREACTOR s_pAcGlobAddPublishReactor = (ACGLOBADDPUBLISHREACTOR)GetProcAddress(hInst, "AcGlobAddPublishReactor");
    if (s_pAcGlobAddPublishReactor != NULL)
    {
        s_pAcGlobAddPublishReactor(&s_PublishReactor);
    }
}


void Reactors::Shutdown()
{
    acedEditor->removeReactor(s_EditorReactor);
    delete s_EditorReactor;

    acDocManager->removeReactor(s_DocManagerReactor);
    delete s_DocManagerReactor;

    acplPlotReactorMgrPtr()->removeReactor(s_PlotReactor);
    delete s_PlotReactor;

    delete s_DbDatabaseReactor;
}