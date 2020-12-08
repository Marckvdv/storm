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
            const std::string RobustSettings::schedulerName = "scheduler";
            const std::string RobustSettings::observationsExportName = "observationsExport";

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

                this->addOption(storm::settings::OptionBuilder(moduleName, schedulerName, false, "Parses the scheduler given in the scheduler format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the scheduler input.").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, observationsExportName, false, "Output file for writing the generated observations.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to which to write the observations.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());
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

            bool RobustSettings::isShedulerSet() const {
                return this->getOption(schedulerName).getHasOptionBeenSet();
            }

            std::string RobustSettings::getSchedulerInputFilename() const {
                return this->getOption(schedulerName).getArgumentByName("filename").getValueAsString();
            }

            bool RobustSettings::isObservationsExportSet() const {
                return this->getOption(observationsExportName).getHasOptionBeenSet();
            }

            std::string RobustSettings::getObservationsExportFilename() const {
                return this->getOption(observationsExportName).getArgumentByName("filename").getValueAsString();
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
