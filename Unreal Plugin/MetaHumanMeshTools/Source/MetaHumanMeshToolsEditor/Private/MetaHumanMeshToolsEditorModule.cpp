#include "Modules/ModuleManager.h"

class FMetaHumanMeshToolsEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        // You can add initialization code for your editor module here if needed.
    }

    virtual void ShutdownModule() override
    {
        // Cleanup code, if necessary.
    }
};

IMPLEMENT_MODULE(FMetaHumanMeshToolsEditorModule, MetaHumanMeshToolsEditor)
