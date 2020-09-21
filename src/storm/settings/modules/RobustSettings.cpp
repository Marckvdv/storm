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

            RobustSettings::RobustSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, roundsName, 100, "amount of rounds to perform").setIsAdvanced().build());
            }
            
            int RobustSettings::rounds() const {
                return this->getOption(roundsName).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
