#include "pch.h"


void MyPublishReactor::OnAboutToBeginBackgroundPublishing(AcPublishBeforeJobInfo *pInfo)
{
    if (pInfo == nullptr)
        return;

    const AcPlDSDData* dsdData = pInfo->GetDSDData();

    WCHAR dir[MAX_PATH] = { 0 };

    const auto* destination = dsdData->destinationName();
    wcscpy(dir, destination);

}

void MyPublishReactor::OnEndPublish(AcPublishReactorInfo *pInfo)
{
    if (pInfo == nullptr)
        return;
}

void MyPublishReactor::RealOnEndPublish()
{
    MessageBox(NULL, L"Publish finished", L"Message Box Title", MB_OK | MB_ICONINFORMATION);

    // test if files exist at this point
    // if we publish to DWF -> everything is ok
    // if we publish to PDF -> files are not published yet

}
