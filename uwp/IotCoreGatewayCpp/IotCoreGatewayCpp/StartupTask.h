#pragma once

#include "pch.h"

namespace IotCoreGatewayCpp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class StartupTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask
    {
    public:
        virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance^ taskInstance);

	private:
		Windows::ApplicationModel::Background::BackgroundTaskDeferral ^deferral;
		IotCoreGatewayUtilitiesCpp::MessageBus^ messageBus;
    };
}
