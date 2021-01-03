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
            const std::string RobustSettings::generateUncertainMdpName = "generateUncertainMdp";
            const std::string RobustSettings::schedulerName = "scheduler";
            const std::string RobustSettings::observationsExportName = "observationsExport";
            const std::string RobustSettings::policyTypeName = "policyType";
            const std::string RobustSettings::priorTypeName = "priorType";
            const std::string RobustSettings::initialStateName = "initialState";
            const std::string RobustSettings::priorFileName = "priorFile";
            const std::string RobustSettings::intervalExportName = "intervalExport";

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

                this->addOption(storm::settings::OptionBuilder(moduleName, generateUncertainMdpName, false, "generate uncertain mdp")
                    .setIsAdvanced().build());

                this->addOption(storm::settings::OptionBuilder(moduleName, schedulerName, false, "Parses the scheduler given in the scheduler format.")
                    .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the scheduler input.").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, observationsExportName, false, "Output file for writing the generated observations.")
                    .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to which to write the observations.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, policyTypeName, false, "Policy type to use.")
                    .addArgument(
                        storm::settings::ArgumentBuilder::createStringArgument("policy", "[uniform, static, explore]")
                            .setDefaultValueString("uniform").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, priorTypeName, false, "Prior type to use.")
                    .addArgument(
                        storm::settings::ArgumentBuilder::createStringArgument("prior", "[no, file]")
                            .setDefaultValueString("no").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, initialStateName, false, "Initial state to use.")
                    .addArgument(
                        storm::settings::ArgumentBuilder::createIntegerArgument("state", "Initial state number").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, intervalExportName, false, "Output file for writing the generated intervals.")
                    .addArgument(storm::settings::ArgumentBuilder::createStringArgument("output", "The name of the file to which to write the intervals.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, priorFileName, false, "Parses the prior given in the file.")
                    .addArgument(storm::settings::ArgumentBuilder::createStringArgument("priorfile", "The name of the file from which to read the prior input.").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
            }
            
            int RobustSettings::rounds() const {
                return
                    this->getOption(roundsName)
                        .getArgumentByName("rounds")
                        .getValueAsUnsignedInteger();
            }

            int RobustSettings::runs() const {
                return
                    this->getOption(runsName)
                        .getArgumentByName("runs")
                        .getValueAsUnsignedInteger();
            }

            bool RobustSettings::generateObservations() const {
                return
                    this->getOption(generateObservationsName)
                        .getHasOptionBeenSet();
            }

            bool RobustSettings::generateUncertainMdp() const {
                return
                    this->getOption(generateUncertainMdpName)
                        .getHasOptionBeenSet();
            }

            bool RobustSettings::isShedulerSet() const {
                return
                    this->getOption(schedulerName)
                        .getHasOptionBeenSet();
            }

            std::string RobustSettings::getSchedulerInputFilename() const {
                return
                    this->getOption(schedulerName)
                    .getArgumentByName("filename")
                    .getValueAsString();
            }

            bool RobustSettings::isObservationsExportSet() const {
                return
                    this->getOption(observationsExportName)
                        .getHasOptionBeenSet();
            }

            std::string RobustSettings::getObservationsExportFilename() const {
                return
                    this->getOption(observationsExportName)
                        .getArgumentByName("filename")
                        .getValueAsString();
            }

            std::string RobustSettings::getPolicyType() const {
                return
                    this->getOption(policyTypeName)
                        .getArgumentByName("policy")
                        .getValueAsString();
            }

            std::string RobustSettings::getPriorType() const {
                return
                    this->getOption(priorTypeName)
                        .getArgumentByName("prior")
                        .getValueAsString();
            }

            bool RobustSettings::isInitialStateSet() const {
                return
                    this->getOption(initialStateName)
                        .getHasOptionBeenSet();
            }

            int RobustSettings::getInitialState() const {
                return
                    this->getOption(initialStateName)
                        .getArgumentByName("state")
                        .getValueAsInteger();
            }

            bool RobustSettings::isPriorFileSet() const {
                return
                    this->getOption(priorFileName)
                        .getHasOptionBeenSet();
            }

            std::string RobustSettings::getPriorFile() const {
                return
                    this->getOption(priorFileName)
                        .getArgumentByName("priorfile")
                        .getValueAsString();
            }

            bool RobustSettings::isIntervalExportSet() const {
                return
                    this->getOption(intervalExportName)
                        .getHasOptionBeenSet();
            }

            std::string RobustSettings::getIntervalExportFilename() const {
                return
                    this->getOption(intervalExportName)
                        .getArgumentByName("output")
                        .getValueAsString();
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
