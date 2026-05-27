#pragma once
#include "../UIElement.h"
#include "../Components/Textfield.h"
#include "../../Graphics/Text.h"
#include "../../Character/QuestLog.h"
#include "nlnx/nx.hpp"
#include <vector>

namespace jrc
{
    class UIQuestLog : public UIElement
    {
    public:
        static constexpr Type TYPE = QUESTLOG;
        static constexpr bool FOCUSED = false;
        static constexpr bool TOGGLED = true;

        UIQuestLog(const Questlog& ql);

        void draw(float inter) const override;
        void update() override;
        void update_screen(int16_t new_width, int16_t new_height) override;

        CursorResult send_cursor(bool clicked, Point<int16_t> cursorpos) override;
        void send_scroll(double yoffset) override;
        void send_key(int32_t keycode, bool pressed, bool escape) override;

        UIElement::Type get_type() const override;

    private:
        enum Tab : int8_t
        {
            IN_PROGRESS,
            COMPLETED,
            NUM_TABS
        };

        void change_tab(Tab tab);
        int16_t get_quest_name_y(int16_t index) const;

        static constexpr int16_t WIDTH = 420;
        static constexpr int16_t HEIGHT = 340;
        static constexpr int16_t TAB_HEIGHT = 22;
        static constexpr int16_t LIST_ENTRY_HEIGHT = 18;
        static constexpr int16_t LIST_X = 8;
        static constexpr int16_t LIST_Y = 36;
        static constexpr int16_t LIST_WIDTH = 180;
        static constexpr int16_t DETAIL_X = 200;
        static constexpr int16_t DETAIL_Y = 50;
        static constexpr int16_t DETAIL_WIDTH = 210;
        static constexpr int16_t VISIBLE_ENTRIES = 14;

        const Questlog& questlog;
        Tab tab;

        // Quest list entries (cached on tab switch)
        struct QuestEntry
        {
            int16_t id;
            std::string name;
            std::string data;
        };
        std::vector<QuestEntry> entries;
        int16_t selected_index;
        int16_t scroll_offset;

        // UI sprites
        Texture background;
        Texture tab_bg;
        bool dirty;
    };
}
