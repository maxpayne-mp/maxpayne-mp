/*

          :::::::::  :::::::::  ::::::::::: :::     :::  ::::::::  :::::::::                 :::   :::   :::::::::
         :+:    :+: :+:    :+:     :+:     :+:     :+: :+:    :+: :+:    :+:               :+:+: :+:+:  :+:    :+:
        +:+    +:+ +:+    +:+     +:+     +:+     +:+        +:+ +:+    +:+              +:+ +:+:+ +:+ +:+    +:+
       +#+    +:+ +#++:++#:      +#+     +#+     +:+     +#++:  +#++:++#: +#++:++#++:++ +#+  +:+  +#+ +#++:++#+
      +#+    +#+ +#+    +#+     +#+      +#+   +#+         +#+ +#+    +#+              +#+       +#+ +#+
     #+#    #+# #+#    #+#     #+#       #+#+#+#   #+#    #+# #+#    #+#              #+#       #+# #+#
    #########  ###    ### ###########     ###      ########  ###    ###              ###       ### ###

    ______________________________________________________________________________________________________________

    - Created by: maph0rip (mateus-bsod)
	- Module: patterns
    - Description:
		this is a simple pattern scanning utility for finding byte patterns in the memory 

*/

#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <psapi.h>

class Pattern {
public:
    static uintptr_t Find(const std::string& pattern) {
        HMODULE hModule = GetModuleHandleA(NULL);
        if (!hModule) return 0;

        MODULEINFO moduleInfo;
        GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo));

        std::vector<uint8_t> patternBytes = ParsePattern(pattern);
        uint8_t* base = (uint8_t*)hModule;

        for (uintptr_t i = 0; i < moduleInfo.SizeOfImage - patternBytes.size(); i++) {
            bool found = true;
            for (size_t j = 0; j < patternBytes.size(); j++) {
                if (patternBytes[j] != 0xFF && base[i + j] != patternBytes[j]) {
                    found = false;
                    break;
                }
            }
            if (found) return (uintptr_t)(base + i);
        }
        return 0;
    }

private:
    static std::vector<uint8_t> ParsePattern(const std::string& pattern) {
        std::vector<uint8_t> result;
        std::string hex = pattern;
        hex.erase(std::remove(hex.begin(), hex.end(), ' '), hex.end());

        for (size_t i = 0; i < hex.length(); i += 2) {
            if (hex[i] == '?' && hex[i + 1] == '?') {
                result.push_back(0xFF);
                i++;
            }
            else {
                std::string byteStr = hex.substr(i, 2);
                result.push_back((uint8_t)strtol(byteStr.c_str(), NULL, 16));
            }
        }
        return result;
    }
};

