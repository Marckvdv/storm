#include "storm/settings/modules/RobustSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string RobustSettings::moduleName = "robust";

            const std::string RobustSettings::roundsName = "rounds";
            const std::string RobustSettings::runsName = "runs";
            const std::string RobustSettings::generateObservationsName = "generateObservations";

            RobustSettings::RobustSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, roundsName, false, "amount of rounds to perform")
                    .setIsAdvanced()
                    .addArgument(
                        storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("rounds", "rounds")
                        .setDefaultValueUnsignedInteger(100).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, runsName, false, "amount of runs to perform")
                    .setIsAdvanced()
                    .addArgument(
                        storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("runs", "runs")
                        .setDefaultValueUnsignedInteger(10).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, generateObservationsName, false, "generate observations")
                    .setIsAdvanced().build());
            }
            
            int RobustSettings::rounds() const {
                return this->getOption(roundsName).getArgumentByName("rounds").getValueAsUnsignedInteger();
            }

            int RobustSettings::runs() const {
                return this->getOption(runsName).getArgumentByName("runs").getValueAsUnsignedInteger();
            }

            bool RobustSettings::generateObservations() const {
                return this->getOption(generateObservationsName).getHasOptionBeenSet();
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
