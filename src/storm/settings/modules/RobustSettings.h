#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

#include "storm/builder/ExplorationOrder.h"

namespace storm {
    namespace settings {
        namespace modules {
            class RobustSettings : public ModuleSettings {
            public:

                RobustSettings();
                
                int rounds() const;
                int runs() const;
                bool generateObservations() const;
                bool generateUncertainMdp() const;
                bool isShedulerSet() const;
                std::string getSchedulerInputFilename() const;
                bool isObservationsExportSet() const;
                std::string getObservationsExportFilename() const;
                std::string getPolicyType() const;
                std::string getPriorType() const;
                double getPriorInit() const;
                bool isInitialStateSet() const;
                int getInitialState() const;
                bool isPriorFileSet() const;
                std::string getPriorFile() const;
                bool isIntervalExportSet() const;
                std::string getIntervalExportFilename() const;

                // The name of the module.
                static const std::string moduleName;

            private:
                // Define the string names of the options as constants.
                static const std::string roundsName;
                static const std::string runsName;
                static const std::string generateObservationsName;
                static const std::string generateUncertainMdpName;
                static const std::string schedulerName;
                static const std::string observationsExportName;
                static const std::string policyTypeName;
                static const std::string priorTypeName;
                static const std::string initialStateName;
                static const std::string priorFileName;
                static const std::string intervalExportName;
                static const std::string priorInitName;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

