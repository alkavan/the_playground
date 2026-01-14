#include <iostream>
#include <string>
#include <vector>

#include "context.h"
#include "roles.h"

class BearReaction {
public:
    static void react(const Bear& bear, const EnvironmentContext& context) {
        std::cout << bear.getName() << " the " << bear.getSpecies() << " ";

        Location location;
        std::vector<Activity> activities;

        switch (context.season)
        {
        case Season::Winter:
            location = Location::Den;
            activities = {Activity::Hibernating};
            break;
        case Season::Spring:
            location = Location::Roaming;
            activities = {Activity::Marking};
        case Season::Summer:
            location = Location::Roaming;
            activities = {Activity::Foraging, Activity::Fishing};
            if(bear.haveCubs()) {
                activities.push_back(Activity::Nurturing);
            }
            break;
        case Season::Autumn:
            location = Location::Roaming;
            activities = {Activity::Foraging};
            break;
        default:
            throw std::runtime_error("Invalid season");
        }

        std::cout << std::endl;
    }
};

class FoxReaction {
public:
    static void react(const Fox& fox, const EnvironmentContext& context) {
        std::cout << fox.getName() << " the " << fox.getSpecies() << " ";

        Location location;
        std::vector<Activity> activities;

        switch (context.season)
        {
        case Season::Winter:
            location = Location::Roaming;
            activities = {Activity::Scavenging, Activity::Hunting};
            break;
        case Season::Spring:
            location = Location::Roaming;
            activities = {Activity::Hunting};
            if(fox.havePups()) {
                location = Location::Den;
                activities.push_back(Activity::Nurturing);
            }
        case Season::Summer:
            location = Location::Roaming;
            activities = {Activity::Socializing, Activity::Marking, Activity::Hunting};
            if(fox.havePups()) {
                activities.push_back(Activity::Nurturing);
            }
            break;
        case Season::Autumn:
            location = Location::Roaming;
            activities = {Activity::Hunting};
            break;
        default:
            throw std::runtime_error("Invalid season");
        }

        std::cout << std::endl;
    }
};

int main() {
    // In winter a bear would be inside the Den and Hibernating.
    // In winter a fox would be Roaming and Scavenging.

    // In spring a bear would be Roaming and Marking territory.
    // In spring a fox would be inside the Den and Breeding.

    // In summer a bear would be Roaming and Fishing or Foraging.
    // In summer a fox would be Roaming, Socializing or Marking territory.

    // In autumn in the morning a bear would be Roaming and Foraging.
    // In autumn in the morning a fox would be Roaming and Hunting.

    const EnvironmentContext autumn_morning { Season::Autumn, TimeOfDay::Morning };
    const EnvironmentContext winter_noon    { Season::Winter, TimeOfDay::Noon    };
    const EnvironmentContext spring_evening { Season::Spring, TimeOfDay::Evening };
    const EnvironmentContext summer_night   { Season::Summer, TimeOfDay::Night   };

    const Bear bear("Pooh", "American Black Bear");
    const Fox fox("Smirre", "Red Fox");

    const AnimalReactionRole bearReaction(bear, BearReaction());
    const AnimalReactionRole foxReaction(fox, FoxReaction());

    std::cout << "During Autumn Morning:" << std::endl;
    bearReaction.react(autumn_morning);
    foxReaction.react(autumn_morning);

    std::cout << "During Winter Noon:" << std::endl;
    bearReaction.react(winter_noon);
    foxReaction.react(winter_noon);

    std::cout << "During Spring Evening:" << std::endl;
    bearReaction.react(spring_evening);
    foxReaction.react(spring_evening);

    std::cout << "During Summer:" << std::endl;
    bearReaction.react(summer_night);
    foxReaction.react(summer_night);

    return 0;
}