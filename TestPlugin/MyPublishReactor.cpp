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
