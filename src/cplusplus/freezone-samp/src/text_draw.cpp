#include "config.hpp"
#include "text_draw.hpp"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/bind.hpp>
#include "core/module_c.hpp"
#include "core/messages/messages_storage.hpp"
#include "core/samp/samp_api.hpp"
#include "core/player/player.hpp"
#include "core/messages/player_messages/tags.hpp"
#include "core/logger/logger.hpp"
#include "core/utility/locale_ru.hpp"

namespace std { namespace tr1 {
    template<class _Ty1,
    class _Ty2>
        bool operator<(const weak_ptr<_Ty1>& _W1,
        const weak_ptr<_Ty2>& _W2)
    {	// return true if _S1 precedes _S2 (order defined by control block)
        return (_W1.owner_before(_W2));
    }
} }

namespace text_draw {
    std::string const alignment_strs[] = {"left", "justified", "center", "right"};
    std::string const style_strs[] = {"style0", "style1", "style2", "style3"};

    template <int size, typename enum_t>
    std::string get_str_by_enum(enum_t e, std::string const* style_strs) {
        if (e < 0 || e >= size) {
            assert(false);
            return style_strs[0];
        }
        return style_strs[e];
    }
    
    template <int size, typename enum_t>
    void get_enum_by_str(enum_t& e, std::string const* style_strs, std::string const& src) {
        for (int i = 0; i < size; ++i) {
            if (boost::iequals(src, style_strs[i])) {
                e = static_cast<enum_t>(i);
            }
        }
    }

    void get_items_by_text_params(tags const& tag, size_t pos, params_t& params) {
        params.x = tag.get_closed_tag_val<float>("x", pos, params.x);
        params.y = tag.get_closed_tag_val<float>("y", pos, params.y);
        params.color = tag.get_closed_tag_val<unsigned int>("color", pos, params.color);
        get_enum_by_str<sizeof(alignment_strs)/sizeof(alignment_strs[0])>(params.alignment, alignment_strs, tag.get_closed_tag_val<std::string>("alignment", pos, get_str_by_enum<sizeof(alignment_strs)/sizeof(alignment_strs[0])>(params.alignment, alignment_strs)));
        params.background_color = tag.get_closed_tag_val<unsigned int>("background_color", pos, params.background_color);
        get_enum_by_str<sizeof(style_strs)/sizeof(style_strs[0])>(params.font_style, style_strs, tag.get_closed_tag_val<std::string>("font_style", pos, get_str_by_enum<sizeof(style_strs)/sizeof(style_strs[0])>(params.font_style, style_strs)));
        params.letter_size_x = tag.get_closed_tag_val<float>("letter_size_x", pos, params.letter_size_x);
        params.letter_size_y = tag.get_closed_tag_val<float>("letter_size_y", pos, params.letter_size_y);
        params.is_proportional = tag.get_closed_tag_val<bool>("is_proportional", pos, params.is_proportional);
        params.shadow = tag.get_closed_tag_val<int>("shadow", pos, params.shadow);
        params.outline = tag.get_closed_tag_val<int>("outline", pos, params.outline);
        params.box_is_use = tag.get_closed_tag_val<bool>("box_is_use", pos, params.box_is_use);
        params.box_color = tag.get_closed_tag_val<unsigned int>("box_color", pos, params.box_color);
        params.box_x = tag.get_closed_tag_val<float>("box_x", pos, params.box_x);
        params.box_y = tag.get_closed_tag_val<float>("box_y", pos, params.box_y);
    }

    td_samp_item_t::td_samp_item_t()
        :id(samp::api::INVALID_TEXT_DRAW)
    {
    }

    td_samp_item_t::td_samp_item_t(td_samp_item_base_t const& td_samp_item_base)
        :id(samp::api::INVALID_TEXT_DRAW)
        ,td_samp_item_base_t(td_samp_item_base)
    {
    }

    td_item::td_item(std::string const& text)
        :text(text)
        ,buffer_ptr(messages_storage::instance()->get_msg_buff()->children_create_connected())
        ,buffer_params(buffer_ptr)
    {
    }

    td_item::~td_item() {
        for (;!show_for_list.empty();) {
            hide_for_player(*show_for_list.begin());
        }
    }

    void td_item::update(std::string const& text) {
        // Потом может быть переделать на разные отображения в зависимости от версии руссификации
        this->text = text;
        update();
    }

    void td_item::update() {
        if (get_processed_text() != last_showed_text) {
            do_update();
        }
    }

    void td_item::show_for_player(player_ptr_t const& player_ptr, int timeout /*= 0 */) {
        if (show_for_list.end() != show_for_list.find(player_ptr)) {
            return;
        }
        show_for_list.insert(player_ptr);
        time_base::millisecond_t hide_time = 0;
        if (timeout) {
            hide_time = time_base::tick_count_milliseconds() + timeout;
        }
        player_ptr->get_item<text_draw_manager_item>()->text_draws.insert(std::make_pair(wptr(shared_from_this()), hide_time));
        if (1 == show_for_list.size()) {
            do_init();
        }
        do_update();
    }

    void td_item::hide_for_player(player_ptr_t const& player_ptr) {
        if (show_for_list.end() == show_for_list.find(player_ptr)) {
            return;
        }
        player_ptr->get_item<text_draw_manager_item>()->text_draws.erase(wptr(shared_from_this()));
        show_for_list.erase(player_ptr);
        if (show_for_list.empty()) {
            do_fini();
        }
        do_update();
    }

    void td_item::do_init() {

    }

    void td_item::do_fini() {

    }

    std::string td_item::get_processed_text() const {
        return cp1251_to_transliteration(buffer_ptr->process_all_vars(text));
    }

    void td_item::do_update() {
        td_samp_item_bases_t new_items = get_items_by_text();
        int curr_items_size = curr_items.size();
        int new_items_size = new_items.size();
        int update_count = curr_items_size;

        // Итемов добавилось
        for (int i = curr_items_size, end = new_items_size; i < end; ++i) {
            td_samp_item_t item;
            do_update_item(item, new_items[i]);
            curr_items.push_back(item);
        }
        if (new_items_size < curr_items_size) {
            // Итем убавилось
            for (int i = new_items_size, end = curr_items_size; i < end; ++i) {
                do_destroy_item(curr_items[i]);
            }
            curr_items.resize(new_items_size);
            update_count = new_items_size;
        }
        
        for (int i = 0; i < update_count; ++i) {
            do_update_item(curr_items[i], new_items[i]);
        }

        last_showed_text = get_processed_text();
    }

    td_samp_item_bases_t td_item::get_items_by_text() const {
        td_samp_item_bases_t rezult;
        int max_len = text_draw_manager::instance()->max_textdraw_len;
        tags rezult_tags(get_processed_text());
        
        std::string work_str = rezult_tags.get_untaget_str();
        int         work_str_shift = 0;
        tags::poses_t const poses = rezult_tags.get_tags_poses();
        
        params_t last_params;
        get_items_by_text_params(rezult_tags, 0, last_params);
        for (tags::poses_t::const_iterator poses_it = poses.begin(), poses_end = poses.end(); poses_end != poses_it; ++poses_it) {
            params_t curr_params;
            get_items_by_text_params(rezult_tags, *poses_it, curr_params);
            if (last_params != curr_params) {
                get_items_by_text_fix_len(rezult, td_samp_item_base_t(work_str.substr(work_str_shift, *poses_it - work_str_shift), last_params), max_len);
                last_params = curr_params;
                work_str_shift = *poses_it;
            }
        }

        get_items_by_text_fix_len(rezult, td_samp_item_base_t(work_str.substr(work_str_shift), last_params), max_len);
        return rezult;
    }

    int get_items_by_text_find_delim_count(std::string const& where, std::string const& delim) {
        int rezult = 0;
        std::string::size_type pos = 0;
        for (; ; ) {
            pos = where.find(delim, pos);
            if (std::string::npos == pos) {
                return rezult;
            }
            pos += delim.length();
            ++rezult;
        }
        return rezult;
    }

    std::string get_items_by_text_make_delims(std::string const& delim, int count) {
        std::string rezult;
        for (int i = 0; i < count; ++i) {
            rezult += delim;
        }
        return rezult;
    }

    void td_item::get_items_by_text_fix_len(td_samp_item_bases_t& dest, td_samp_item_base_t const& item, int max_len) const {
        if (static_cast<int>(item.text.length()) > max_len) {
            // Строку необходимо разбить на несколько
            std::string const delimer = "~n~";
            std::string work_string = item.text;
            int delim_count = 0;
            for (;;) {
                std::string::size_type delim_pos =  work_string.rfind(delimer, max_len - delim_count * delimer.length());
                if (std::string::npos == delim_pos || static_cast<int>(work_string.length()) <= max_len - delim_count * delimer.length()) {
                    dest.push_back(td_samp_item_base_t(get_items_by_text_make_delims(delimer, delim_count) + work_string, item.params));
                    if (std::string::npos == delim_pos && static_cast<int>(work_string.length()) > max_len - delim_count * delimer.length()) {
                        // Не удалось разбить на кусочки
                        logger::instance()->log("text_draw/error_fix_len", (boost::format("'%1%'") % item.text).str());
                        assert(false);
                    }
                    return;
                }
                std::string work_part = work_string.substr(0, delim_pos);
                dest.push_back(td_samp_item_base_t(get_items_by_text_make_delims(delimer, delim_count) + work_part, item.params));
                delim_count += (1 + get_items_by_text_find_delim_count(work_part, delimer));
                work_string = work_string.substr(delim_pos + delimer.length());
            }
        }
        else {
            dest.push_back(item);
        }
    }

    void td_item::do_update_item(td_samp_item_t& curr_item, td_samp_item_base_t const& new_item) {
        if (show_for_list.empty()) {
            // Удаляем все итемы и синхронизируем их
            do_destroy_item(curr_item);
            curr_item = new_item;
            return;
        }
        samp::api::ptr api_ptr = samp::api::instance();
        
        if (samp::api::INVALID_TEXT_DRAW == curr_item.id) {
            // Создаем новый текст драв
            curr_item = new_item;
            do_update_item_create(curr_item);
            do_update_item_set_params(curr_item.id, curr_item.params);
            do_update_item_show_for_players(curr_item);
            return;
        }

        // Обновляем текстдрав
        if (curr_item.params != new_item.params) {
            // Необходимо обновить параметры + возможно скрыть/отобразить
            do_destroy_item(curr_item);
            curr_item = new_item;
            do_update_item_create(curr_item);
            do_update_item_set_params(curr_item.id, curr_item.params);
            do_update_item_show_for_players(curr_item);
            //do_update_item_hide_for_players(curr_item);
            //do_update_item_set_params(curr_item.id, curr_item.params);
            //api_ptr->textdraw_set_string(curr_item.id, curr_item.text);
            //do_update_item_show_for_players(curr_item);
            return;
        }

        // Необходимо обновить текст
        if (curr_item.text != new_item.text) {
            curr_item.text = new_item.text;
            do_update_item_update_text(curr_item);
        }

        //Синхронизируем список игроков
        BOOST_FOREACH(player_ptr_t const& player_ptr, curr_item.show_for) {
            if (show_for_list.end() == show_for_list.find(player_ptr)) {
                // Скрываем
                api_ptr->textdraw_hide_for_player(player_ptr->get_id(), curr_item.id);
            }
        }
        BOOST_FOREACH(player_ptr_t const& player_ptr, show_for_list) {
            if (curr_item.show_for.end() == curr_item.show_for.find(player_ptr)) {
                // Показываем
                api_ptr->textdraw_show_for_player(player_ptr->get_id(), curr_item.id);
            }
        }

        curr_item.show_for = show_for_list;
    }

    void td_item::do_destroy_item(td_samp_item_t& td_samp_item) {
        if (samp::api::INVALID_TEXT_DRAW == td_samp_item.id) {
            td_samp_item.show_for.clear();
            return;
        }
        samp::api::ptr api_ptr = samp::api::instance();
        do_update_item_hide_for_players(td_samp_item);
        api_ptr->textdraw_destroy(td_samp_item.id);
        td_samp_item.id = samp::api::INVALID_TEXT_DRAW;
    }

    void td_item::do_update_item_set_params(int textdraw_id, params_t const& params) {
        if (samp::api::INVALID_TEXT_DRAW == textdraw_id) {
            return;
        }
        samp::api::ptr api_ptr = samp::api::instance();
        api_ptr->textdraw_color(textdraw_id, params.color); // Чтобы вступило в силу нужно спрятать/показать текст драв игроку
        api_ptr->textdraw_alignment(textdraw_id, params.alignment);
        api_ptr->textdraw_background_color(textdraw_id, params.background_color); // Чтобы вступило в силу нужно спрятать/показать текст драв игроку
        api_ptr->textdraw_font(textdraw_id, params.font_style);
        api_ptr->textdraw_letter_size(textdraw_id, params.letter_size_x, params.letter_size_y);
        api_ptr->textdraw_set_proportional(textdraw_id, params.is_proportional);
        api_ptr->textdraw_set_shadow(textdraw_id, params.shadow);
        api_ptr->textdraw_set_outline(textdraw_id, params.outline);
        api_ptr->textdraw_box_color(textdraw_id, params.box_color);
        if (params.box_is_use) {
            api_ptr->textdraw_use_box(textdraw_id, params.box_is_use);
            api_ptr->textdraw_text_size(textdraw_id, params.box_x, params.box_y);
        }
    }

    void td_item::do_update_item_show_for_players(td_samp_item_t& item) {
        samp::api::ptr api_ptr = samp::api::instance();
        BOOST_FOREACH(player_ptr_t const& player_ptr, show_for_list) {
            api_ptr->textdraw_show_for_player(player_ptr->get_id(), item.id);
        }
        item.show_for = show_for_list;
    }

    void td_item::do_update_item_hide_for_players(td_samp_item_t& item) {
        samp::api::ptr api_ptr = samp::api::instance();
        BOOST_FOREACH(player_ptr_t const& player_ptr, item.show_for) {
            api_ptr->textdraw_hide_for_player(player_ptr->get_id(), item.id);
        }
        item.show_for.clear();
    }

    void td_item::do_update_item_create(td_samp_item_t& item) {
        samp::api::ptr api_ptr = samp::api::instance();
        item.id = api_ptr->textdraw_create(item.params.x, item.params.y, item.text);
        if (text_draw_manager::instance()->is_debug) {
            logger::instance()->debug("text_draw/create", (boost::format("%1% '%2%'") % item.id % item.text).str());
        }
    }

    void td_item::do_update_item_update_text(td_samp_item_t& item) {
        samp::api::ptr api_ptr = samp::api::instance();
        api_ptr->textdraw_set_string(item.id, item.text);
        if (text_draw_manager::instance()->is_debug) {
            logger::instance()->debug("text_draw/updatetext", (boost::format("%1% '%2%'") % item.id % item.text).str());
        }
    }

    std::string td_item::save_chars = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890.,:;@$()-_ []";

    std::string td_item::get_save_string(std::string const& str) {
        return boost::find_format_all_copy(boost::replace_all_copy(str, "~", " "), boost::token_finder(boost::lambda::bind(std::logical_not<bool>(),boost::lambda::bind(boost::is_any_of(save_chars), boost::lambda::_1))), boost::empty_formatter("")); // Оставляем только разрешенные символы
    }

    REGISTER_IN_APPLICATION(text_draw_manager);

    text_draw_manager::ptr text_draw_manager::instance() {
        return application::instance()->get_item<text_draw_manager>();
    }

    text_draw_manager::text_draw_manager() {
    }

    text_draw_manager::~text_draw_manager() {
    }

    void text_draw_manager::configure_pre() {
        is_debug = false;
        max_textdraw_len = 256;
    }

    void text_draw_manager::configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_ITEM(is_debug);
        SERIALIZE_ITEM(max_textdraw_len);
    }

    void text_draw_manager::configure_post() {
    }

    void text_draw_manager::on_pre_connect(player_ptr_t const& player_ptr) {
        text_draw_manager_item::ptr item(new text_draw_manager_item(*this), &player_item::do_delete);
        player_ptr->add_item(item);
    }

    text_draw_manager_item::text_draw_manager_item(text_draw_manager& manager)
        :manager(manager)
    {
    }

    text_draw_manager_item::~text_draw_manager_item() {

    }

    void text_draw_manager_item::on_timer250() {
        time_base::millisecond_t const curr_time = time_base::tick_count_milliseconds();
        for (text_draws_t::iterator it = text_draws.begin(); text_draws.end() != it; ) {
            if (it->second && curr_time > it->second) {
                // Пора скрывать
                it++->first.lock()->hide_for_player(get_root());
            }
            else {
                ++it;
            }
        }
    }

    void text_draw_manager_item::on_disconnect(samp::player_disconnect_reason reason) {
        for (;!text_draws.empty();) {
            if (!text_draws.begin()->first.expired()) {
                text_draws.begin()->first.lock()->hide_for_player(get_root());
            }
            else {
                assert(false);
                text_draws.erase(text_draws.begin());
            }
        }
    }
} // namespace text_draw {
