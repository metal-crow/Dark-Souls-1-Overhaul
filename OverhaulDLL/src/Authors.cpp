/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++
*/

#include "DllMain.h"


// List of people who directly contributed to the Dark Souls Overhaul Mod
std::vector<std::string> Mod::authors = {
    // @TODO: Get everyone's preferred name to be credited with
    "Ainsley Harriott",
    "Ashley",
    "B3LYP",
    "Birdulon",
    "Cestial",
    "CoelyPuffs",
    "Elissa (loosepolygon)",
    "frublox",
    "GenericDM",
    "hzpe",
    "illusorywall",
    "jellybaby34",
    "Kali",
    "KushiSushi",
    "LIBERTY PRIME",
    "Local",
    "LordOfSandvich",
    "Meowmaritus",
    "metal-crow",
    "Not a Cleric",
    "nyxojaele",
    "Pav",
    "RavagerChris37",
    "Sean Pesce",
    "SiriusTextra",
    "StarBoss",
    "Sub",
    "SuperIronBob",
    "TheLagWizard",
    "Undakovrist",
    "Wulf2k",
    "Xerv.io/us",
    // Apologies in advance to anyone I forgot, just let me know so I can add you to the list
    //     -Sean
};


// Detailed list of authors (with roles)
std::vector<std::string> Mod::author_roles =
{
    "+----------------------------+---------------------------------------------------------------------------------------------+",
    "|          CONTRIBUTOR       |                                     ROLE(S)                                                 |",
    "+----------------------------+---------------------------------------------------------------------------------------------+",
    "| Ainsley Harriott           | Adviser, Reverse Engineer, Anti-Cheat Developer                                             |",
    "| Ashley                     | Project Lead, Lead Reverse Engineer, Anti-Cheat Developer, Adviser, Tester                  |",
    "| B3LYP                      | Reverse Engineer, Utility Developer                                                         |",
    "| Birdulon                   | Adviser, Reverse Engineer                                                                   |",
    "| Cestial                    | Adviser                                                                                     |",
    "| CoelyPuffs                 | Adviser, Researcher                                                                         |",
    "| Elissa (loosepolygon)      | Adviser, Reverse Engineer, Utility Developer                                                |",
    "| frublox                    | Adviser, Researcher                                                                         |",
    "| GenericDM                  | Researcher                                                                                  |",
    "| hzpe                       | Project Lead, Adviser, Tester                                                               |",
    "| illusorywall               | Researcher, Adviser, Tester                                                                 |",
    "| jellybaby34                | Reverse Engineer                                                                            |",
    "| Kali                       | Project Lead, Community Ambassador, Adviser, Researcher, Tester                             |",
    "| KushiSushi                 | Adviser, Researcher, Reverse Engineer                                                       |",
    "| LIBERTY PRIME              | Adviser, Researcher                                                                         |",
    "| Local                      | Adviser, Researcher                                                                         |",
    "| LordOfSandvich             | Adviser, Researcher, Tester                                                                 |",
    "| Meowmaritus                | Lead Adviser, Reverse Engineer, Utility Developer                                           |",
    "| metal-crow                 | Project Lead, Lead Reverse Engineer, Lead Programmer, Anti-Cheat Developer, Adviser, Tester |",
    "| Not a Cleric               | Researcher                                                                                  |",
    "| nyxojaele                  | Adviser, Reverse Engineer, Utility Developer                                                |",
    "| Pav                        | Adviser, Researcher                                                                         |",
    "| RavagerChris37             | Reverse Engineer, Utility Developer                                                         |",
    "| Sean Pesce                 | Lead Programmer, Reverse Engineer, Utility Developer, Software Design & Deployment          |",
    "| SiriusTextra               | Artist, Asset Creation                                                                      |",
    "| StarBoss                   | Researcher                                                                                  |",
    "| Sub                        | Researcher, Tester                                                                          |",
    "| SuperIronBob               | Adviser, Utility Developer                                                                  |",
    "| TheLagWizard               | Researcher, Tester                                                                          |",
    "| Undakovrist                | Adviser, Researcher, Tester                                                                 |",
    "| Wulf2k                     | Adviser, Reverse Engineer, Utility Developer                                                |",
    "| Xerv.io/us                 | Adviser, Researcher, Tester                                                                 |",
    "+----------------------------+---------------------------------------------------------------------------------------------+"
};


// List of people who indirectly contributed to the mod, but had no direct involvement
std::vector<std::string> Mod::contributors = {
    "Atvaark",
    "Durante",
    "eur0pa",
    "IAmV3nom"
    "Tarvitz"
    "Xiang"
    "Youri \"NullBy7e\" de Mooij",
    "Zullie The Witch"
};

// Others who made the project possible
std::vector<std::string> Mod::special_thanks = {
    "FROM Software"
};


// Plays the credits for the Dark Souls Overhaul Mod (displays the list of people who contributed to development)
DWORD WINAPI Mod::play_credits(LPVOID lpParam)
{
    // @TODO: Fade screen to black and scroll names (first make sure user doesn't have a character loaded)
    constexpr static const unsigned int delay = 500;
    print_console("");
    print_console("");
    print_console("    ///////////////////////////////////////////////////////////////////");
    Sleep(delay);
    print_console("    ///////////////// Dark Souls Overhaul Mod Authors /////////////////");
    Sleep(delay);
    print_console("    ///////////////////////////////////////////////////////////////////");
    Sleep(delay);
    print_console("");
        
    for (std::string s : Mod::authors)
    {
        Sleep(delay);
        print_console("                " + s);
    }

    Sleep(delay);
    print_console("");
    return 0;
}

// Plays the detailed credits for the Dark Souls Overhaul Mod (displays the list of people who contributed to development, & their roles)
DWORD WINAPI Mod::play_credits_detailed(LPVOID lpParam)
{
    // @TODO: Fade screen to black and scroll names (first make sure user doesn't have a character loaded)
    constexpr static const unsigned int delay = 500;
    print_console("");
    print_console("");
    print_console("                                    ///////////////////////////////////////////////////////////////////");
    Sleep(delay);
    print_console("                                    ///////////////// Dark Souls Overhaul Mod Authors /////////////////");
    Sleep(delay);
    print_console("                                    ///////////////////////////////////////////////////////////////////");
    Sleep(delay);
    print_console("");

    for (std::string s : Mod::author_roles)
    {
        Sleep(delay);
        print_console("        " + s);
    }

    Sleep(delay);
    print_console("");
    return 0;
}
