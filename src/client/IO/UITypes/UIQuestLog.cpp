#include "UIQuestLog.h"
#include "../UI.h"
#include "../../Util/Misc.h"
#include <algorithm>

namespace jrc
{
    UIQuestLog::UIQuestLog(const Questlog& ql) :
        questlog(ql),
        tab(IN_PROGRESS),
        selected_index(-1),
        scroll_offset(0),
        dirty(true)
    {
        position = Point<int16_t>(
            static_cast<int16_t>(Constants::viewwidth() / 2 - WIDTH / 2),
            static_cast<int16_t>(Constants::viewheight() / 2 - HEIGHT / 2)
        );
        dimension = { WIDTH, HEIGHT };
        change_tab(IN_PROGRESS);
    }

    void UIQuestLog::draw(float inter) const
    {
        if (!is_active())
            return;

        // Tab headers
        for (int i = 0; i < NUM_TABS; i++)
        {
            const char* name = (i == 0) ? "In Progress" : "Completed";
            int16_t tx = position.x() + 6 + i * 100;
            int16_t ty = position.y() + 4;
            
            Text label(Text::A12B, Text::LEFT, 
                (i == tab) ? Text::WHITE : Text::LIGHTGREY, name);
            label.draw({ tx, ty });
        }

        // Quest list
        int16_t list_y = position.y() + 30;
        int16_t max_y = position.y() + HEIGHT - 12;
        for (size_t i = scroll_offset; i < entries.size() && list_y < max_y; i++)
        {
            const auto& entry = entries[i];
            Text::Color color = (static_cast<int16_t>(i) == selected_index)
                ? Text::YELLOW : Text::WHITE;
            
            Text line(Text::A11M, Text::LEFT, color, entry.name);
            line.draw({ position.x() + 8, list_y });
            list_y += 16;
        }

        // Detail pane
        if (selected_index >= 0 && static_cast<size_t>(selected_index) < entries.size())
        {
            const auto& entry = entries[selected_index];
            
            Text detail(Text::A12B, Text::LEFT, Text::YELLOW, entry.name);
            detail.draw({ position.x() + 200, position.y() + 50 });
            
            std::string info = "ID: " + std::to_string(entry.id);
            Text info_txt(Text::A11M, Text::LEFT, Text::WHITE, info);
            info_txt.draw({ position.x() + 200, position.y() + 72 });
        }
    }

    void UIQuestLog::update()
    {
    }

    void UIQuestLog::update_screen(int16_t new_width, int16_t new_height)
    {
        position = Point<int16_t>(
            static_cast<int16_t>(new_width / 2 - WIDTH / 2),
            static_cast<int16_t>(new_height / 2 - HEIGHT / 2)
        );
    }

    void UIQuestLog::change_tab(Tab newtab)
    {
        tab = newtab;
        entries.clear();
        selected_index = -1;
        scroll_offset = 0;

        if (tab == IN_PROGRESS)
        {
            const auto& started = questlog.get_started();
            for (const auto& pair : started)
            {
                QuestEntry e;
                e.id = pair.first;
                e.name = "Quest " + std::to_string(pair.first);
                e.data = pair.second;
                entries.push_back(e);
            }
        }
        else
        {
            const auto& completed = questlog.get_completed();
            for (const auto& pair : completed)
            {
                QuestEntry e;
                e.id = pair.first;
                e.name = "Quest " + std::to_string(pair.first);
                e.data = "Completed";
                entries.push_back(e);
            }
        }
        
        dirty = true;
    }

    UIElement::CursorResult UIQuestLog::send_cursor(bool clicked, Point<int16_t> cursorpos)
    {
        if (!is_in_range(cursorpos))
            return { Cursor::IDLE, false };

        // Tab clicks
        for (int i = 0; i < NUM_TABS; i++)
        {
            Rectangle<int16_t> tab_area(
                position.x() + 6 + i * 100,
                position.x() + 6 + i * 100 + 95,
                position.y() + 2,
                position.y() + 2 + 20
            );
            if (tab_area.contains(cursorpos) && clicked)
            {
                change_tab(static_cast<Tab>(i));
                return { Cursor::IDLE, true };
            }
        }

        // List entries
        int16_t list_y = position.y() + 30;
        for (size_t i = scroll_offset; i < entries.size(); i++)
        {
            Rectangle<int16_t> entry_area(
                position.x() + 8,
                position.x() + 8 + 190,
                list_y,
                list_y + 16
            );
            if (entry_area.contains(cursorpos))
            {
                if (clicked)
                    selected_index = static_cast<int16_t>(i);
                return { Cursor::IDLE, true };
            }
            list_y += 16;
        }

        return { Cursor::IDLE, true };
    }

    void UIQuestLog::send_scroll(double yoffset)
    {
        int16_t max_scroll = std::max<int16_t>(0,
            static_cast<int16_t>(entries.size()) - VISIBLE_ENTRIES);
        scroll_offset = std::clamp(
            static_cast<int16_t>(scroll_offset - static_cast<int16_t>(yoffset)),
            (int16_t)0, max_scroll);
    }

    void UIQuestLog::send_key(int32_t keycode, bool pressed, bool escape)
    {
        if (pressed && escape)
            deactivate();
    }

    UIElement::Type UIQuestLog::get_type() const
    {
        return TYPE;
    }
}
