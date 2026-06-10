#pragma once

#include <Windows.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "framework/nlohmann/json.hpp"
#include "debug_ostream.h"

struct ScoreSummary
{
    std::string jsonname;
    std::string musicname;
    std::string musicauthor;
    std::string scoreauthor;
    float difficulty = 0.0f;
    float bpm = 0.0f;
    std::string thumbnail;
    std::string music;
};

class ScoreSummaryManager
{
public:
    static ScoreSummaryManager& GetInstance()
    {
        static ScoreSummaryManager instance;
        return instance;
    }

    bool ReloadSummaries(const std::string& directoryPath = "asset\\score")
    {
        summaries.clear();

        const std::string pattern = directoryPath + "\\*.json";
        WIN32_FIND_DATAA findData = {};
        HANDLE findHandle = FindFirstFileA(pattern.c_str(), &findData);
        if (findHandle == INVALID_HANDLE_VALUE)
        {
            selectedJsonName.clear();
            return false;
        }

        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                continue;
            }

            const std::string jsonName = findData.cFileName;
            const std::string jsonPath = directoryPath + "\\" + jsonName;

            std::ifstream file(jsonPath);
            if (!file.is_open())
            {
                continue;
            }

            try
            {
                nlohmann::json jsonData;
                file >> jsonData;

                ScoreSummary summary;
                summary.jsonname = jsonName;
                hal::dout << summary.jsonname << std::endl;
                summary.musicname = jsonData.value("musicname", "");
                hal::dout << summary.musicname << std::endl;
                summary.musicauthor = jsonData.value("musicauthor", "");
                summary.scoreauthor = jsonData.value("scoreauthor", "");
                summary.difficulty = jsonData.value("difficulty", 0.0f);
                summary.bpm = jsonData.value("bpm", 0.0f);
                summary.thumbnail = jsonData.value("thumbnail", "");
                summary.music = jsonData.value("music", "");

                summaries.push_back(summary);
            }
            catch (...)
            {
                // 読み込み失敗ファイルのみスキップ
                continue;
            }
        } while (FindNextFileA(findHandle, &findData) != 0);

        FindClose(findHandle);

        std::sort(summaries.begin(), summaries.end(), [](const ScoreSummary& a, const ScoreSummary& b)
        {
            if (a.difficulty == b.difficulty)
            {
                return a.jsonname < b.jsonname;
            }
            return a.difficulty > b.difficulty;
        });

        if (summaries.empty())
        {
            selectedJsonName.clear();
        }
        else if (selectedJsonName.empty() || !HasJsonName(selectedJsonName))
        {
            selectedJsonName = summaries.front().jsonname;
        }

        return !summaries.empty();
    }

    const std::vector<ScoreSummary>& GetSummaries() const
    {
        return summaries;
    }

    void SetSelectedJsonName(const std::string& jsonName)
    {
        selectedJsonName = jsonName;
    }

    const std::string& GetSelectedJsonName() const
    {
        return selectedJsonName;
    }

    std::size_t GetSummaryCount() const
    {
        return summaries.size();
    }

private:
    bool HasJsonName(const std::string& jsonName) const
    {
        for (const auto& summary : summaries)
        {
            if (summary.jsonname == jsonName)
            {
                return true;
            }
        }
        return false;
    }

    std::vector<ScoreSummary> summaries;
    std::string selectedJsonName;
};