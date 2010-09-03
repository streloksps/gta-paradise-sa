#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H
#include <string>
#include <map>
#include <boost/noncopyable.hpp>
#include <memory>
#include "core/player/player_fwd.hpp"
#include "core/messages/messages_params.hpp"
#include "core/module_h.hpp"
#include "core/time_outs.hpp"


/*
    Полезные ссылки для работы с текстдравами:
    http://www.gtamodding.com/index.php?title=GXT
    http://www.gtavision.com/index.php?section=downloads&site=download&id=1245


*/
namespace text_draw {
    enum alignment_e {
        alignment_left = 0
        ,alignment_justified = 1
        ,alignment_center = 2
        ,alignment_right = 3
    };

    enum font_style_e {
        font_style0 = 0
        ,font_style1 = 1
        ,font_style2 = 2
        ,font_style3 = 3
    };

    //<x /><y /><color /><alignment /><background_color /><font_style /><letter_size_x /><letter_size_y /><is_proportional /><shadow /><outline /><box_is_use /><box_color /><box_x /><box_y />
    struct params_t {
        float           x;
        float           y;
        unsigned int    color;
        alignment_e     alignment;
        unsigned int    background_color;
        font_style_e    font_style;
        float           letter_size_x;
        float           letter_size_y;
        bool            is_proportional;
        int             shadow;
        int             outline;
        bool            box_is_use;
        unsigned int    box_color;
        float           box_x;
        float           box_y;
        params_t(float x = 0.0f, float y = 0.0f, unsigned int color = 0
            ,alignment_e alignment = alignment_left, unsigned int background_color = 0, font_style_e font_style = font_style0
            ,float letter_size_x = 0.0f, float letter_size_y = 0.0f, bool is_proportional = true
            ,int shadow = 0, int outline = 0, bool box_is_use = false, unsigned int box_color = 0
            ,float box_x = 0.0f, float box_y = 0.0f)
            :x(x), y(y), color(color), alignment(alignment), background_color(background_color), font_style(font_style)
            ,letter_size_x(letter_size_x), letter_size_y(letter_size_y), is_proportional(is_proportional), shadow(shadow), outline(outline)
            ,box_is_use(box_is_use), box_color(box_color), box_x(box_x), box_y(box_y) {}

        bool operator==(params_t const& params) const {
            return x == params.x
                && y == params.y
                && color == params.color
                && alignment == params.alignment
                && background_color == params.background_color
                && font_style == params.font_style
                && letter_size_x == params.letter_size_x
                && letter_size_y == params.letter_size_y
                && is_proportional == params.is_proportional
                && shadow == params.shadow
                && outline == params.outline
                && box_is_use == params.box_is_use
                && box_color == params.box_color
                && box_x == params.box_x
                && box_y == params.box_y
                ;
        }
        bool operator !=(params_t const& params) const {
            return !operator==(params);
        }
    };

    struct td_samp_item_base_t {
        std::string text;
        params_t    params;
        td_samp_item_base_t(std::string const& text, params_t const& params): text(text), params(params) {}
        td_samp_item_base_t() {}
    };

    struct td_samp_item_t: td_samp_item_base_t {
        int         id;
        players_t   show_for;
        td_samp_item_t();
        td_samp_item_t(td_samp_item_base_t const& td_samp_item_base);
    };
    typedef std::vector<td_samp_item_base_t> td_samp_item_bases_t;
    typedef std::vector<td_samp_item_t> td_samp_items_t;

    class text_draw_manager;
    class text_draw_manager_item;

    // Данный класс можно создавать только динамически и помещать в умный указатель
    class td_item
        :boost::noncopyable
        ,public std::tr1::enable_shared_from_this<td_item>
    {
    public: // Публичный интерфейс
        typedef std::tr1::shared_ptr<td_item> ptr;
        typedef std::tr1::weak_ptr<td_item> wptr;

        td_item(std::string const& text = "empty");
        ~td_item();

        void update(std::string const& text); // Обновляет текст
        void update(); // Обновляет текст, учитывая переменные

        inline messages_params& get_params() {return buffer_params;}

        void show_for_player(player_ptr_t const& player_ptr, int timeout = 0 ); // Показывает текст драв на заданное, в миллисекундах, время или на всегда (timeout = 0)
        void hide_for_player(player_ptr_t const& player_ptr);

        static std::string get_save_string(std::string const& str);
    
    private: // Текущее состояние
        friend class text_draw_manager;
        friend class text_draw_manager_item;

        std::string text;
        std::string last_showed_text;
        players_t   show_for_list; // Список игроков, для которых отображается данный текстдрав
        std::string get_processed_text() const;

    private: // Рабочие переменные
        static std::string save_chars;

        buffer_ptr_t buffer_ptr;
        messages_params buffer_params;
        td_samp_items_t curr_items;

        void do_init();
        void do_fini();
        void do_update();

        td_samp_item_bases_t get_items_by_text() const;
        void get_items_by_text_fix_len(td_samp_item_bases_t& dest, td_samp_item_base_t const& item, int max_len) const;
        void do_update_item_create(td_samp_item_t& item);
        void do_update_item_update_text(td_samp_item_t& item);
        void do_update_item(td_samp_item_t& td_samp_item);
        void do_update_item(td_samp_item_t& curr_item, td_samp_item_base_t const& new_item);
        void do_destroy_item(td_samp_item_t& td_samp_item);
        void do_update_item_set_params(int textdraw_id, params_t const& params);
        void do_update_item_show_for_players(td_samp_item_t& item);
        void do_update_item_hide_for_players(td_samp_item_t& item);
    };


    class text_draw_manager
        :public application_item
        ,public configuradable
        ,public players_events::on_pre_connect_i
    {
    public:
        typedef std::tr1::shared_ptr<text_draw_manager> ptr;
        static ptr instance();

    public:
        friend class td_item;
        friend class text_draw_manager_item;
        text_draw_manager();
        virtual ~text_draw_manager();

    public: // configuradable
        virtual void configure_pre();
        virtual void configure(buffer::ptr const& buff, def_t const& def);
        virtual void configure_post();
    public: // auto_name
        SERIALIZE_MAKE_CLASS_NAME(text_draw_manager);

    public: // players_events::*
        virtual void on_pre_connect(player_ptr_t const& player_ptr);

    private:
        bool is_debug;
        int max_textdraw_len;
    };

    class text_draw_manager_item
        :public player_item
        ,public on_timer250_i
        ,public player_events::on_disconnect_i
    {
    public:
        typedef std::tr1::shared_ptr<text_draw_manager_item> ptr;
    private:
        friend class td_item;
        friend class text_draw_manager;

        text_draw_manager_item(text_draw_manager& manager);
        virtual ~text_draw_manager_item();

    public: // player_events::*
        virtual void on_timer250();
        virtual void on_disconnect(samp::player_disconnect_reason reason);

    private:
        typedef std::map<td_item::wptr, time_base::millisecond_t> text_draws_t;

        text_draw_manager& manager;
        text_draws_t text_draws;
    };

} // namespace text_draw {

#endif // TEXT_DRAW_H
