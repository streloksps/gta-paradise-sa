#include "config.hpp"
#include "pawn_events.hpp"
#include "samp-plugin-sdk/amx/amx.h"
#include "core/application.hpp"

namespace pawn {
    REGISTER_IN_APPLICATION(events);

    events::ptr events::instance() {
        // Кешируем указатель
        if (ptr single_ptr = events::single.lock()) {
            return single_ptr;
        }
        events::ptr rezult = application::instance()->get_item<events>();
        if (rezult) {
            events::single = wptr(rezult);
        }
        return rezult;
    }
    events::wptr events::single;

    events::events() {
    }

    events::~events() {
    }

    void events::configure_pre() {
    }

    void events::configure(buffer::ptr const& buff, def_t const& def) {
    }

    void events::configure_post() {
    }

    void events::add_callback_impl(call_base_ptr call_ptr) {
        calls.push_back(call_ptr);
    }

    int events::callback(int index, AMX* amx, cell* params) {
        if (0 > index || static_cast<int>(calls.size()) <= index) {
            return 0;
        }
        return calls[index]->process_call(amx, params);
    }

    namespace {
        cell AMX_NATIVE_CALL callback00(AMX* amx, cell* params) { return events::instance()->callback(0, amx, params); }
        cell AMX_NATIVE_CALL callback01(AMX* amx, cell* params) { return events::instance()->callback(1, amx, params); }
        cell AMX_NATIVE_CALL callback02(AMX* amx, cell* params) { return events::instance()->callback(2, amx, params); }
        cell AMX_NATIVE_CALL callback03(AMX* amx, cell* params) { return events::instance()->callback(3, amx, params); }
        cell AMX_NATIVE_CALL callback04(AMX* amx, cell* params) { return events::instance()->callback(4, amx, params); }
        cell AMX_NATIVE_CALL callback05(AMX* amx, cell* params) { return events::instance()->callback(5, amx, params); }
        cell AMX_NATIVE_CALL callback06(AMX* amx, cell* params) { return events::instance()->callback(6, amx, params); }
        cell AMX_NATIVE_CALL callback07(AMX* amx, cell* params) { return events::instance()->callback(7, amx, params); }
        cell AMX_NATIVE_CALL callback08(AMX* amx, cell* params) { return events::instance()->callback(8, amx, params); }
        cell AMX_NATIVE_CALL callback09(AMX* amx, cell* params) { return events::instance()->callback(9, amx, params); }
        cell AMX_NATIVE_CALL callback10(AMX* amx, cell* params) { return events::instance()->callback(10, amx, params); }
        cell AMX_NATIVE_CALL callback11(AMX* amx, cell* params) { return events::instance()->callback(11, amx, params); }
        cell AMX_NATIVE_CALL callback12(AMX* amx, cell* params) { return events::instance()->callback(12, amx, params); }
        cell AMX_NATIVE_CALL callback13(AMX* amx, cell* params) { return events::instance()->callback(13, amx, params); }
        cell AMX_NATIVE_CALL callback14(AMX* amx, cell* params) { return events::instance()->callback(14, amx, params); }
        cell AMX_NATIVE_CALL callback15(AMX* amx, cell* params) { return events::instance()->callback(15, amx, params); }
        cell AMX_NATIVE_CALL callback16(AMX* amx, cell* params) { return events::instance()->callback(16, amx, params); }
        cell AMX_NATIVE_CALL callback17(AMX* amx, cell* params) { return events::instance()->callback(17, amx, params); }
        cell AMX_NATIVE_CALL callback18(AMX* amx, cell* params) { return events::instance()->callback(18, amx, params); }
        cell AMX_NATIVE_CALL callback19(AMX* amx, cell* params) { return events::instance()->callback(19, amx, params); }
        cell AMX_NATIVE_CALL callback20(AMX* amx, cell* params) { return events::instance()->callback(20, amx, params); }
        cell AMX_NATIVE_CALL callback21(AMX* amx, cell* params) { return events::instance()->callback(21, amx, params); }
        cell AMX_NATIVE_CALL callback22(AMX* amx, cell* params) { return events::instance()->callback(22, amx, params); }
        cell AMX_NATIVE_CALL callback23(AMX* amx, cell* params) { return events::instance()->callback(23, amx, params); }
        cell AMX_NATIVE_CALL callback24(AMX* amx, cell* params) { return events::instance()->callback(24, amx, params); }
        cell AMX_NATIVE_CALL callback25(AMX* amx, cell* params) { return events::instance()->callback(25, amx, params); }
        cell AMX_NATIVE_CALL callback26(AMX* amx, cell* params) { return events::instance()->callback(26, amx, params); }
        cell AMX_NATIVE_CALL callback27(AMX* amx, cell* params) { return events::instance()->callback(27, amx, params); }
        cell AMX_NATIVE_CALL callback28(AMX* amx, cell* params) { return events::instance()->callback(28, amx, params); }
        cell AMX_NATIVE_CALL callback29(AMX* amx, cell* params) { return events::instance()->callback(29, amx, params); }
        cell AMX_NATIVE_CALL callback30(AMX* amx, cell* params) { return events::instance()->callback(30, amx, params); }
        cell AMX_NATIVE_CALL callback31(AMX* amx, cell* params) { return events::instance()->callback(31, amx, params); }
        cell AMX_NATIVE_CALL callback32(AMX* amx, cell* params) { return events::instance()->callback(32, amx, params); }
        cell AMX_NATIVE_CALL callback33(AMX* amx, cell* params) { return events::instance()->callback(33, amx, params); }
        cell AMX_NATIVE_CALL callback34(AMX* amx, cell* params) { return events::instance()->callback(34, amx, params); }
        cell AMX_NATIVE_CALL callback35(AMX* amx, cell* params) { return events::instance()->callback(35, amx, params); }
        cell AMX_NATIVE_CALL callback36(AMX* amx, cell* params) { return events::instance()->callback(36, amx, params); }
        cell AMX_NATIVE_CALL callback37(AMX* amx, cell* params) { return events::instance()->callback(37, amx, params); }
        cell AMX_NATIVE_CALL callback38(AMX* amx, cell* params) { return events::instance()->callback(38, amx, params); }
        cell AMX_NATIVE_CALL callback39(AMX* amx, cell* params) { return events::instance()->callback(39, amx, params); }
        cell AMX_NATIVE_CALL callback40(AMX* amx, cell* params) { return events::instance()->callback(40, amx, params); }
        cell AMX_NATIVE_CALL callback41(AMX* amx, cell* params) { return events::instance()->callback(41, amx, params); }
        cell AMX_NATIVE_CALL callback42(AMX* amx, cell* params) { return events::instance()->callback(42, amx, params); }
        cell AMX_NATIVE_CALL callback43(AMX* amx, cell* params) { return events::instance()->callback(43, amx, params); }
        cell AMX_NATIVE_CALL callback44(AMX* amx, cell* params) { return events::instance()->callback(44, amx, params); }
        cell AMX_NATIVE_CALL callback45(AMX* amx, cell* params) { return events::instance()->callback(45, amx, params); }
        cell AMX_NATIVE_CALL callback46(AMX* amx, cell* params) { return events::instance()->callback(46, amx, params); }
        cell AMX_NATIVE_CALL callback47(AMX* amx, cell* params) { return events::instance()->callback(47, amx, params); }
        cell AMX_NATIVE_CALL callback48(AMX* amx, cell* params) { return events::instance()->callback(48, amx, params); }
        cell AMX_NATIVE_CALL callback49(AMX* amx, cell* params) { return events::instance()->callback(49, amx, params); }
        cell AMX_NATIVE_CALL callback50(AMX* amx, cell* params) { return events::instance()->callback(50, amx, params); }
        cell AMX_NATIVE_CALL callback51(AMX* amx, cell* params) { return events::instance()->callback(51, amx, params); }
        cell AMX_NATIVE_CALL callback52(AMX* amx, cell* params) { return events::instance()->callback(52, amx, params); }
        cell AMX_NATIVE_CALL callback53(AMX* amx, cell* params) { return events::instance()->callback(53, amx, params); }
        cell AMX_NATIVE_CALL callback54(AMX* amx, cell* params) { return events::instance()->callback(54, amx, params); }
        cell AMX_NATIVE_CALL callback55(AMX* amx, cell* params) { return events::instance()->callback(55, amx, params); }
        cell AMX_NATIVE_CALL callback56(AMX* amx, cell* params) { return events::instance()->callback(56, amx, params); }
        cell AMX_NATIVE_CALL callback57(AMX* amx, cell* params) { return events::instance()->callback(57, amx, params); }
        cell AMX_NATIVE_CALL callback58(AMX* amx, cell* params) { return events::instance()->callback(58, amx, params); }
        cell AMX_NATIVE_CALL callback59(AMX* amx, cell* params) { return events::instance()->callback(59, amx, params); }
        cell AMX_NATIVE_CALL callback60(AMX* amx, cell* params) { return events::instance()->callback(60, amx, params); }
        cell AMX_NATIVE_CALL callback61(AMX* amx, cell* params) { return events::instance()->callback(61, amx, params); }
        cell AMX_NATIVE_CALL callback62(AMX* amx, cell* params) { return events::instance()->callback(62, amx, params); }
        cell AMX_NATIVE_CALL callback63(AMX* amx, cell* params) { return events::instance()->callback(63, amx, params); }
        cell AMX_NATIVE_CALL callback64(AMX* amx, cell* params) { return events::instance()->callback(64, amx, params); }
        cell AMX_NATIVE_CALL callback65(AMX* amx, cell* params) { return events::instance()->callback(65, amx, params); }
        cell AMX_NATIVE_CALL callback66(AMX* amx, cell* params) { return events::instance()->callback(66, amx, params); }
        cell AMX_NATIVE_CALL callback67(AMX* amx, cell* params) { return events::instance()->callback(67, amx, params); }
        cell AMX_NATIVE_CALL callback68(AMX* amx, cell* params) { return events::instance()->callback(68, amx, params); }
        cell AMX_NATIVE_CALL callback69(AMX* amx, cell* params) { return events::instance()->callback(69, amx, params); }
        cell AMX_NATIVE_CALL callback70(AMX* amx, cell* params) { return events::instance()->callback(70, amx, params); }
        cell AMX_NATIVE_CALL callback71(AMX* amx, cell* params) { return events::instance()->callback(71, amx, params); }
        cell AMX_NATIVE_CALL callback72(AMX* amx, cell* params) { return events::instance()->callback(72, amx, params); }
        cell AMX_NATIVE_CALL callback73(AMX* amx, cell* params) { return events::instance()->callback(73, amx, params); }
        cell AMX_NATIVE_CALL callback74(AMX* amx, cell* params) { return events::instance()->callback(74, amx, params); }
        cell AMX_NATIVE_CALL callback75(AMX* amx, cell* params) { return events::instance()->callback(75, amx, params); }
        cell AMX_NATIVE_CALL callback76(AMX* amx, cell* params) { return events::instance()->callback(76, amx, params); }
        cell AMX_NATIVE_CALL callback77(AMX* amx, cell* params) { return events::instance()->callback(77, amx, params); }
        cell AMX_NATIVE_CALL callback78(AMX* amx, cell* params) { return events::instance()->callback(78, amx, params); }
        cell AMX_NATIVE_CALL callback79(AMX* amx, cell* params) { return events::instance()->callback(79, amx, params); }
        cell AMX_NATIVE_CALL callback80(AMX* amx, cell* params) { return events::instance()->callback(80, amx, params); }
        cell AMX_NATIVE_CALL callback81(AMX* amx, cell* params) { return events::instance()->callback(81, amx, params); }
        cell AMX_NATIVE_CALL callback82(AMX* amx, cell* params) { return events::instance()->callback(82, amx, params); }
        cell AMX_NATIVE_CALL callback83(AMX* amx, cell* params) { return events::instance()->callback(83, amx, params); }
        cell AMX_NATIVE_CALL callback84(AMX* amx, cell* params) { return events::instance()->callback(84, amx, params); }
        cell AMX_NATIVE_CALL callback85(AMX* amx, cell* params) { return events::instance()->callback(85, amx, params); }
        cell AMX_NATIVE_CALL callback86(AMX* amx, cell* params) { return events::instance()->callback(86, amx, params); }
        cell AMX_NATIVE_CALL callback87(AMX* amx, cell* params) { return events::instance()->callback(87, amx, params); }
        cell AMX_NATIVE_CALL callback88(AMX* amx, cell* params) { return events::instance()->callback(88, amx, params); }
        cell AMX_NATIVE_CALL callback89(AMX* amx, cell* params) { return events::instance()->callback(89, amx, params); }
        cell AMX_NATIVE_CALL callback90(AMX* amx, cell* params) { return events::instance()->callback(90, amx, params); }
        cell AMX_NATIVE_CALL callback91(AMX* amx, cell* params) { return events::instance()->callback(91, amx, params); }
        cell AMX_NATIVE_CALL callback92(AMX* amx, cell* params) { return events::instance()->callback(92, amx, params); }
        cell AMX_NATIVE_CALL callback93(AMX* amx, cell* params) { return events::instance()->callback(93, amx, params); }
        cell AMX_NATIVE_CALL callback94(AMX* amx, cell* params) { return events::instance()->callback(94, amx, params); }
        cell AMX_NATIVE_CALL callback95(AMX* amx, cell* params) { return events::instance()->callback(95, amx, params); }
        cell AMX_NATIVE_CALL callback96(AMX* amx, cell* params) { return events::instance()->callback(96, amx, params); }
        cell AMX_NATIVE_CALL callback97(AMX* amx, cell* params) { return events::instance()->callback(97, amx, params); }
        cell AMX_NATIVE_CALL callback98(AMX* amx, cell* params) { return events::instance()->callback(98, amx, params); }
        cell AMX_NATIVE_CALL callback99(AMX* amx, cell* params) { return events::instance()->callback(99, amx, params); }

        AMX_NATIVE_INFO array_to_export[] = {
            {0, callback00},
            {0, callback01},
            {0, callback02},
            {0, callback03},
            {0, callback04},
            {0, callback05},
            {0, callback06},
            {0, callback07},
            {0, callback08},
            {0, callback09},
            {0, callback10},
            {0, callback11},
            {0, callback12},
            {0, callback13},
            {0, callback14},
            {0, callback15},
            {0, callback16},
            {0, callback17},
            {0, callback18},
            {0, callback19},
            {0, callback20},
            {0, callback21},
            {0, callback22},
            {0, callback23},
            {0, callback24},
            {0, callback25},
            {0, callback26},
            {0, callback27},
            {0, callback28},
            {0, callback29},
            {0, callback30},
            {0, callback31},
            {0, callback32},
            {0, callback33},
            {0, callback34},
            {0, callback35},
            {0, callback36},
            {0, callback37},
            {0, callback38},
            {0, callback39},
            {0, callback40},
            {0, callback41},
            {0, callback42},
            {0, callback43},
            {0, callback44},
            {0, callback45},
            {0, callback46},
            {0, callback47},
            {0, callback48},
            {0, callback49},
            {0, callback50},
            {0, callback51},
            {0, callback52},
            {0, callback53},
            {0, callback54},
            {0, callback55},
            {0, callback56},
            {0, callback57},
            {0, callback58},
            {0, callback59},
            {0, callback60},
            {0, callback61},
            {0, callback62},
            {0, callback63},
            {0, callback64},
            {0, callback65},
            {0, callback66},
            {0, callback67},
            {0, callback68},
            {0, callback69},
            {0, callback70},
            {0, callback71},
            {0, callback72},
            {0, callback73},
            {0, callback74},
            {0, callback75},
            {0, callback76},
            {0, callback77},
            {0, callback78},
            {0, callback79},
            {0, callback80},
            {0, callback81},
            {0, callback82},
            {0, callback83},
            {0, callback84},
            {0, callback85},
            {0, callback86},
            {0, callback87},
            {0, callback88},
            {0, callback89},
            {0, callback90},
            {0, callback91},
            {0, callback92},
            {0, callback93},
            {0, callback94},
            {0, callback95},
            {0, callback96},
            {0, callback97},
            {0, callback98},
            {0, callback99}
        };
    }

    void events::plugin_on_amx_load(AMX* amx) {
        if (calls.empty()) {
            return;
        }
        if (calls.size() > sizeof(array_to_export)/sizeof(array_to_export[0])) {
            assert(false && "Добавлено слишком много элементов - необходимо увеличить внутренний буффер");
            return;
        }

        // Заполняем array_to_export
        // Максимальная длина имени функции - 32 байта с завершающим нулем
        const int max_name_len = 32;
        std::vector<char> names(max_name_len * calls.size(), 0);
        for (calls_t::size_type i = 0, size = calls.size(); i < size; ++i) {
            char* curr_name = &names[i * max_name_len];
            calls[i]->name.copy(curr_name, max_name_len - 1);
            array_to_export[i].name = curr_name;
        }

        amx_Register(amx, array_to_export, calls.size());
    }

        /*
        //OnTimer();
        cell AMX_NATIVE_CALL OnTimer(AMX* amx, cell* params) {
            if (curr_events) {
                curr_events->chronometer_on_timer.event_begin();
                if (curr_events->chronometer_on_timer.get_is_dumb()) {
                    curr_events->chronometer_on_timer.event_dumb(boost::format(""));
                }
            }
            container_execute_handlers(application::instance(), &on_timer_i::on_timer);
            if (curr_events) curr_events->chronometer_on_timer.event_end();
            return 1;
        }
        */

} // namespace pawn {
