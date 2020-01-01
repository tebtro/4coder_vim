#include "vim.h"


function void vim_improved_newline(Application_Links *app, b32 below = true, b32 newline_at_sol_eol = true);

//
// @note helpers
//

inline b32
character_is_newline(char c) {
    b32 result = (c == '\n');
    return result;
}

#define VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app) \
View_ID view_id = get_this_ctx_view((app), Access_Always); \
Managed_Scope view_scope = view_get_managed_scope((app), view_id); \
Vim_View_State *vim_state = scope_attachment((app), view_scope, view_vim_state_id, Vim_View_State);

#define VIM_GET_VIEW_ID_AND_BUFFER_ID(app) \
View_ID view_id = get_this_ctx_view((app), Access_Always); \
Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadVisible);

// @note View for macro
#define for_views(app, it) \
for (View_ID it = get_view_next(app, 0, Access_Always); \
it != 0;                                           \
it = get_view_next(app, it, Access_Always))


//
// @note vim initialization
//

function void
vim_setup_mode_and_chord_color_tables(Application_Links *app) {
    Arena *arena = &global_theme_arena;
    Vim_Global_State *state = &vim_global_state;
    Color_Table *table;
    
    for (int i = 0; i < ArrayCount(vim_global_state.color_tables_array); ++i) {
        vim_global_state.color_tables_array[i] = make_color_table(app, arena);
        
        for (u32 x = 0; x < active_color_table.count; ++x) {
            vim_global_state.color_tables_array[i].arrays[x] = active_color_table.arrays[x];
        }
    }
    
    
    // @note mode normal
    table = &state->color_tables.mode_normal;
    
    // @note mode insert
    table = &state->color_tables.mode_insert;
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0x1100FF00);
    
    // @note mode visual
    table = &state->color_tables.mode_visual;
    table->arrays[defcolor_cursor] = make_colors(arena, 0xFF0DDFFF);
    table->arrays[defcolor_mark]   = make_colors(arena, 0xAA0DDFFF);
    
    // @note mode replace
    table = &state->color_tables.mode_replace;
    table->arrays[defcolor_cursor]    = make_colors(arena, 0xFF9500C2);
    table->arrays[defcolor_at_cursor] = make_colors(arena, 0xFFFFFFFF);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0x22D90BCC);
    
    // @note chord replace single
    table = &state->color_tables.chord_replace_single;
    table->arrays[defcolor_cursor]    = make_colors(arena, 0xFFD90BCC);
    table->arrays[defcolor_at_cursor] = make_colors(arena, 0xFFFFFFFF);
    
    // @note chord delete
    table = &state->color_tables.chord_delete;
    table->arrays[defcolor_cursor] = make_colors(arena, 0xFFFF0000);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0x22FF0000);
    
    // @note chord change
    table = &state->color_tables.chord_change;
    table->arrays[defcolor_cursor] = make_colors(arena, 0xFFFF9800);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0x22E9710C);
    
    // @note chord yank
    table = &state->color_tables.chord_yank;
    table->arrays[defcolor_cursor] = make_colors(arena, 0xFFFFFF00);
    table->arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0x22CCCC00);
}


//
// @note Vim mode and chord
//

function void
vim_set_mode_and_command_map(Application_Links *app,
                             Vim_Mode new_mode, Vim_Action new_action, Command_Map_ID new_map_id, Color_Table *color_table_ptr) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    
    Command_Map_ID *map_id_ptr = scope_attachment(app, view_scope, view_map_id, Command_Map_ID);
    *map_id_ptr = new_map_id;
    
    if (new_mode != 0) {
        vim_state->mode = new_mode;
        vim_state->color_table_ptr = color_table_ptr;
    }
    if (new_action != 0) {
        vim_state->pending_action = new_action;
        vim_state->color_table_ptr = color_table_ptr;
    }
    if (new_mode == 0 && new_action == 0 && color_table_ptr != 0) {
        vim_state->color_table_ptr = color_table_ptr;
    }
    
#if 1
    if (vim_state->color_table_ptr == 0) {
        vim_state->color_table_ptr = &vim_global_state.color_tables.default;
    }
#endif
}

//
// @note vim mode handling
//

function void
vim_enter_mode(Application_Links *app,
               Vim_Mode new_mode, Command_Map_ID new_map_id, Color_Table *color_table_ptr) {
    vim_set_mode_and_command_map(app, new_mode, vim_action_none, new_map_id, color_table_ptr);
}


// @todo @incomplete
// chord_bar

CUSTOM_COMMAND_SIG(vim_enter_mode_normal) {
    // @todo @incomplete
    vim_enter_mode(app, vim_mode_normal, mapid_vim_mode_normal, &vim_global_state.color_tables.mode_normal);
}

// @note enter insert mode commands
CUSTOM_COMMAND_SIG(vim_enter_mode_insert) {
    // @todo @incomplete
    // end_visual_selection
    // end_chord_bar
    vim_enter_mode(app, vim_mode_insert, mapid_vim_mode_insert, &vim_global_state.color_tables.mode_insert);
}
CUSTOM_COMMAND_SIG(vim_enter_mode_insert_after) {
    VIM_GET_VIEW_ID_AND_BUFFER_ID(app);
    i64 cursor_pos = view_get_cursor_pos(app, view_id);
    u8 next_char;
    if (buffer_read_range(app, buffer_id, Ii64(cursor_pos, cursor_pos + 1), &next_char)) {
        if (!character_is_newline(next_char)) {
            move_right(app);
        }
    }
    vim_enter_mode_insert(app);
}
CUSTOM_COMMAND_SIG(vim_enter_mode_insert_line_start) {
    seek_pos_of_visual_line(app, Side_Min); // :non_virtual_whitespace textual or visual line
    vim_enter_mode_insert(app);
}
CUSTOM_COMMAND_SIG(vim_enter_mode_insert_line_end) {
    seek_pos_of_visual_line(app, Side_Max); // :non_virtual_whitespace textual or visual line
    vim_enter_mode_insert(app);
}
CUSTOM_COMMAND_SIG(vim_newline_and_enter_mode_insert_after) {
    vim_improved_newline(app, true, false);
    vim_enter_mode_insert(app);
}
CUSTOM_COMMAND_SIG(vim_newline_and_enter_mode_insert_before) {
    vim_improved_newline(app, false, false);
    vim_enter_mode_insert(app);
}

CUSTOM_COMMAND_SIG(vim_enter_mode_replace) {
    // @todo @incomplete
    // clear_register_selection
    vim_enter_mode(app, vim_mode_replace, mapid_vim_mode_replace, &vim_global_state.color_tables.mode_replace);
}

// :vim_visual_range
inline void
vim_update_visual_range(Application_Links *app, View_ID view_id, Vim_View_State *vim_state, b32 reset_mark = false) {
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadVisible);
    i64 buffer_size = buffer_get_size(app, buffer_id);
    
    // :cursor_mark
    i64 cursor_pos = view_get_cursor_pos(app, view_id);
    i64 mark_pos;
    if (reset_mark) {
        mark_pos = cursor_pos;
        view_set_mark(app, view_id, seek_pos(mark_pos));
    } else {
        mark_pos = view_get_mark_pos(app, view_id);
    }
    if (cursor_pos >= buffer_size) {
        move_left(app);
        cursor_pos = view_get_cursor_pos(app, view_id);
    }
    if (mark_pos >= buffer_size) {
        mark_pos = buffer_size;
        view_set_mark(app, view_id, seek_pos(mark_pos - 1));
    }
    
    Range_i64 range = Ii64(cursor_pos, mark_pos);
    vim_state->selection_range = Ii64(range.start, range.end + 1);
    
    if (vim_state->selection_range.one_past_last >= buffer_size) {
        vim_state->selection_range.one_past_last = buffer_size;
    }
}
CUSTOM_COMMAND_SIG(vim_enter_mode_visual) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    
    vim_update_visual_range(app, view_id, vim_state, true);
    
    // @todo @incomplete
    // clear_register_selection
    vim_enter_mode(app, vim_mode_visual, mapid_vim_mode_visual, &vim_global_state.color_tables.mode_visual);
}

// :vim_visual_range
inline void
vim_update_visual_line_range(Application_Links *app, View_ID view_id, Vim_View_State *vim_state, b32 reset_mark = false) {
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_Always);
    i64 buffer_size = buffer_get_size(app, buffer_id);
    
    i64 cursor_pos = view_get_cursor_pos(app, view_id);
    i64 mark_pos;
    if (reset_mark) {
        mark_pos = cursor_pos;
    } else {
        mark_pos = view_get_mark_pos(app, view_id);
    }
    if (cursor_pos >= buffer_size) {
        move_left(app);
        cursor_pos = view_get_cursor_pos(app, view_id);
    }
    
    Range_i64 pos_range = Ii64(cursor_pos, mark_pos);
    Range_i64 line_range = get_line_range_from_pos_range(app, buffer_id, pos_range);
    Range_i64 range = get_pos_range_from_line_range(app, buffer_id, line_range);
    
    vim_state->selection_range = Ii64(range.start, range.end + 1);
    
    if (vim_state->selection_range.one_past_last >= buffer_size) {
        vim_state->selection_range.one_past_last = buffer_size;
    }
    
    // :cursor_mark
    if (cursor_pos <= mark_pos) {
        view_set_mark(app, view_id, seek_pos(vim_state->selection_range.one_past_last - 1));
    }
    else {
        view_set_mark(app, view_id, seek_pos(vim_state->selection_range.start));
    }
}
CUSTOM_COMMAND_SIG(vim_enter_mode_visual_line) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    
    vim_update_visual_line_range(app, view_id, vim_state, true);
    
    // @todo @incomplete
    // clear_register_selection
    vim_enter_mode(app, vim_mode_visual_line, mapid_vim_mode_visual, &vim_global_state.color_tables.mode_visual);
}

//
// @note vim chord handling
//

function void
vim_enter_chord(Application_Links *app, Command_Map_ID new_map_id, Color_Table *color_table_ptr) {
    vim_set_mode_and_command_map(app, (Vim_Mode)0, (Vim_Action)0, new_map_id, color_table_ptr);
}
function void
vim_enter_chord(Application_Links *app,
                Vim_Action new_action, Command_Map_ID new_map_id, Color_Table *color_table_ptr) {
    vim_set_mode_and_command_map(app, (Vim_Mode)0, new_action, new_map_id, color_table_ptr);
}


CUSTOM_COMMAND_SIG(vim_enter_chord_replace_single) {
    // @todo @incomplete
    // clear_register_selection
    vim_enter_chord(app, mapid_vim_chord_replace_single, &vim_global_state.color_tables.chord_replace_single);
}

CUSTOM_COMMAND_SIG(vim_enter_chord_delete) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, vim_action_delete_range, mapid_vim_chord_delete, &vim_global_state.color_tables.chord_delete);
}

CUSTOM_COMMAND_SIG(vim_enter_chord_change) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, vim_action_change_range, mapid_vim_chord_change, &vim_global_state.color_tables.chord_change);
}

CUSTOM_COMMAND_SIG(vim_enter_chord_yank) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, vim_action_yank_range, mapid_vim_chord_yank, &vim_global_state.color_tables.chord_yank);
}


CUSTOM_COMMAND_SIG(vim_enter_chord_format) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, vim_action_format_range, mapid_vim_chord_format, 0);
}

CUSTOM_COMMAND_SIG(vim_enter_chord_indent_right) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, vim_action_indent_right_range, mapid_vim_chord_indent_right, 0);
}

CUSTOM_COMMAND_SIG(vim_enter_chord_indent_left) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, vim_action_indent_left_range, mapid_vim_chord_indent_left, 0);
}


CUSTOM_COMMAND_SIG(vim_enter_chord_move_right_to_found) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_move_right_to_found, 0);
}
CUSTOM_COMMAND_SIG(vim_enter_chord_move_right_before_found) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_move_right_before_found, 0);
}
CUSTOM_COMMAND_SIG(vim_enter_chord_move_left_to_found) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_move_left_to_found, 0);
}
CUSTOM_COMMAND_SIG(vim_enter_chord_move_left_before_found) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_move_left_before_found, 0);
}


CUSTOM_COMMAND_SIG(vim_enter_chord_g) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_g, 0);
}
CUSTOM_COMMAND_SIG(vim_enter_chord_z) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_z, 0);
}


CUSTOM_COMMAND_SIG(vim_enter_chord_mark) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_mark, 0);
}
CUSTOM_COMMAND_SIG(vim_enter_chord_window) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_window, 0);
}
CUSTOM_COMMAND_SIG(vim_enter_chord_choose_register) {
    // @todo @incomplete
    // chord_bar
    vim_enter_chord(app, mapid_vim_chord_choose_register, 0);
}

//
// @note Chord helper
//
template <CUSTOM_COMMAND_SIG(command)>
CUSTOM_COMMAND_SIG(vim_chord_command) {
    command(app);
    vim_enter_mode_normal(app);
}

//
// @note vim handle actions
//
function void
vim_exec_pending_action(Application_Links *app, Range_i64 range, b32 is_line = false) {
    ProfileScope(app, "vim_exec_pending_action");
    
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadWriteVisible);
    Vim_Action action = vim_state->pending_action;
    // @todo When we need it. Vim_Global_State *vim_global_state = &vim_global_state;
    
    // @note execute pending action
    switch (action) {
        case vim_action_delete_range:
        case vim_action_change_range: {
            // @todo save to vim register
            
            // @note post to clipboard and remove range in buffer
            if (clipboard_post_buffer_range(app, 0, buffer_id, range)) {
                // @note If changing a line, don't remove the newline char!
                if (is_line && action == vim_action_change_range) {
                    --range.one_past_last;
                }
                
                buffer_replace_range(app, buffer_id, range, string_u8_empty);
            }
            
            // :change_mode
            if (action == vim_action_change_range) {
                vim_enter_mode_insert(app);
            }
        } break;
        
        case vim_action_yank_range: {
            // @todo save to vim register
            
            clipboard_post_buffer_range(app, 0, buffer_id, range);
        } break;
        
        case vim_action_indent_left_range:  // @todo If not in virtual_whitespace should decrease indentation
        case vim_action_indent_right_range: // @todo If not in virtual_whitespace should increase indentation
        case vim_action_format_range: {
            auto_indent_buffer(app, buffer_id, range);
            move_past_lead_whitespace(app, view_id, buffer_id);
        } break;
    }
    
    // :change_mode
    switch (vim_state->mode) {
        case vim_mode_normal: {
            vim_enter_mode_normal(app);
        } break;
        
        // @note Set mapid, but don't change visual selection_range
        case vim_mode_visual: {
            vim_enter_mode(app, vim_mode_visual, mapid_vim_mode_visual, &vim_global_state.color_tables.mode_visual);
        } break;
        case vim_mode_visual_line: {
            vim_enter_mode(app, vim_mode_visual_line, mapid_vim_mode_visual, &vim_global_state.color_tables.mode_visual);
        } break;
    }
}

CUSTOM_COMMAND_SIG(vim_exec_pending_action_on_line_range) {
    VIM_GET_VIEW_ID_AND_BUFFER_ID(app);
    
    i64 cursor_pos = view_get_cursor_pos(app, view_id);
    i64 line_number = get_line_number_from_pos(app, buffer_id, cursor_pos);
    Range_i64 range = get_line_pos_range(app, buffer_id, line_number);
    range.one_past_last = range.end + 1;
    
    vim_exec_pending_action(app, range, true);
}


//
// @note vim movement commands
//

#define VIM_NTIMES_CUSTOM_COMMAND_SIG(name) void name(struct Application_Links *app, View_ID view_id, Buffer_ID buffer_id, Managed_Scope view_scope, Managed_Scope buffer_scope, Vim_View_State *vim_state)

// @note execute ntimes

inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_up) {
    i64 line_count = buffer_get_line_count(app, buffer_id);
    i64 n = min(line_count, vim_state->execute_command_count);
    
    // @todo @robustness This function is not moving by the exact line amount. If the window is not maximized???
    move_vertical_lines(app, view_id, -n);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_down) {
    i64 line_count = buffer_get_line_count(app, buffer_id);
    i64 n = min(line_count, vim_state->execute_command_count);
    
    // @todo @robustness This function is not moving by the exact line amount. If the window is not maximized???
    move_vertical_lines(app, view_id, n);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left) {
    view_set_cursor_by_character_delta(app, view_id, -vim_state->execute_command_count);
    no_mark_snap_to_cursor_if_shift(app, view_id);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right) {
    view_set_cursor_by_character_delta(app, view_id, vim_state->execute_command_count);
    no_mark_snap_to_cursor_if_shift(app, view_id);
}

inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_up_by_page) {
    f32 page_jump = get_page_jump(app, view_id);
    move_vertical_pixels(app, -page_jump);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_down_by_page) {
    f32 page_jump = get_page_jump(app, view_id);
    move_vertical_pixels(app, page_jump);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_up_by_page_half) {
    f32 page_jump = get_page_jump(app, view_id) * 0.5f;
    move_vertical_pixels(app, -page_jump);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_down_by_page_half) {
    f32 page_jump = get_page_jump(app, view_id) * 0.5f;
    move_vertical_pixels(app, page_jump);
}

/* @todo :non_virtual_whitespace This option only really changes the behaviour in non_virtual_whitspace_mode.
PositionWithinLine_Start
PositionWithinLine_SkipLeadingWhitespace
PositionWithinLine_End
*/
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_up_by_whitespace) {
    for (int i = 0; i < vim_state->execute_command_count; ++i) {
        seek_blank_line(app, Scan_Backward, PositionWithinLine_SkipLeadingWhitespace);
    }
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_down_by_whitespace) {
    for (int i = 0; i < vim_state->execute_command_count; ++i) {
        seek_blank_line(app, Scan_Forward, PositionWithinLine_SkipLeadingWhitespace);
    }
}

inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_once_move_to_file_start) {
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(0));
    no_mark_snap_to_cursor_if_shift(app, view_id);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_once_move_to_file_end) {
    i32 size = (i32)buffer_get_size(app, buffer_id);
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(size));
    no_mark_snap_to_cursor_if_shift(app, view_id);
}

inline void
vim_get_found_input_character_pos(Application_Links *app, View_ID view_id, Buffer_ID buffer_id, Vim_View_State *vim_state, Scan_Direction direction, b32 one_before_found = false) {
    i64 pos = view_get_cursor_pos(app, view_id);
    if (one_before_found) {
        pos += (direction == Scan_Forward) ? 1 : -1;
    }
    
    User_Input user_input = get_current_input(app);
    String_Const_u8 character = to_writable(&user_input);
    Assert(character.size == 1);
    Character_Predicate character_predicate = character_predicate_from_character(character.str[0]);
    
    i64 new_pos;
    String_Match match;
    for (int i = 0; i < vim_state->execute_command_count; ++i) {
        match = buffer_seek_character_class(app, buffer_id, &character_predicate, direction, pos);
        if (match.buffer == 0)  break; // If not found break
        new_pos = match.range.min;
        if (pos == new_pos)  break;
        pos = new_pos;
    }
    
    if (one_before_found) {
        pos += (direction == Scan_Forward) ? -1 : 1;
    }
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(pos));
    no_mark_snap_to_cursor_if_shift(app, view_id);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_to_found) {
    vim_get_found_input_character_pos(app, view_id, buffer_id, vim_state, Scan_Forward);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_before_found) {
    vim_get_found_input_character_pos(app, view_id, buffer_id, vim_state, Scan_Forward, true);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_to_found) {
    vim_get_found_input_character_pos(app, view_id, buffer_id, vim_state, Scan_Backward);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_before_found) {
    vim_get_found_input_character_pos(app, view_id, buffer_id, vim_state, Scan_Backward, true);
}


inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_word_start) {
    // @todo Wrong :behaviour
    move_right_alpha_numeric_boundary(app);
    move_right(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_token_start) {
    // @todo Wrong :behaviour
    move_right_token_boundary(app);
    move_right(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_one_after_whitespace) {
    // @todo Wrong :behaviour
    move_right_whitespace_boundary(app);
    move_right(app);
}

inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_word_end) {
    // @cleanup
    move_right(app);
    move_right_alpha_numeric_boundary(app);
    move_left(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_token_end) {
    // @cleanup
    move_right(app);
    move_right_token_boundary(app);
    move_left(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_right_one_before_whitespace) {
    // @cleanup
    move_right(app);
    move_right_whitespace_boundary(app);
    move_left(app);
}


inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_word_start) {
    move_left_alpha_numeric_boundary(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_token_start) {
    move_left_token_boundary(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_one_before_whitespace) {
    move_left_whitespace_boundary(app);
}

inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_word_end) {
    // @todo Wrong :behaviour
    move_left_alpha_numeric_boundary(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_token_end) {
    // @todo Wrong :behaviour
    move_left_token_boundary(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_ntimes_move_left_one_after_whitespace) {
    // @todo Wrong :behaviour
    move_left_whitespace_boundary(app);
}

// @note execute once

/* @todo :non_virtual_whitespace
seek_pos_of_visual_line
seek_pos_of_textual_line
*/
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_once_move_to_line_start) {
    seek_pos_of_visual_line(app, Side_Min);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_once_move_to_line_end) {
    seek_pos_of_visual_line(app, Side_Max);
}

inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_once_goto_line) {
    goto_line(app);
}


inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_set_mark) {
    set_mark(app);
}
inline VIM_NTIMES_CUSTOM_COMMAND_SIG(_vim_cursor_mark_swap) {
    cursor_mark_swap(app);
}


//
// @note compound commands
//

//
// @note execute command n times
//
// Uh, templates ...

template <VIM_NTIMES_CUSTOM_COMMAND_SIG(command), b32 one_past_last = false>
CUSTOM_COMMAND_SIG(vim_command_execute_ntimes) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_Always);
    Managed_Scope buffer_scope = buffer_get_managed_scope(app, buffer_id);
    
    i64 pos_before = view_get_cursor_pos(app, view_id);
    
    command(app, view_id, buffer_id, view_scope, buffer_scope, vim_state);
    
    vim_state->execute_command_count = 1;
    vim_state->predecimal_count = 0;
    
    i64 pos_after = view_get_cursor_pos(app, view_id);
    
    
    Range_i64 diff_range = Ii64(pos_before, pos_after);
    if (one_past_last) {
        i64 buffer_size = buffer_get_size(app, buffer_id);
        if (diff_range.end < buffer_size) {
            diff_range.one_past_last = diff_range.end + 1;
        }
    }
    
    // @note Vim exec pending action
    vim_exec_pending_action(app, diff_range);
    
    
    // @note Update :vim_visual_range
    if (vim_state->mode == vim_mode_visual) {
        vim_update_visual_range(app, view_id, vim_state);
    }
    else if (vim_state->mode == vim_mode_visual_line) {
        vim_update_visual_line_range(app, view_id, vim_state);
    }
}

// @note execute ntimes

#define vim_move_up    vim_command_execute_ntimes<_vim_ntimes_move_up>
#define vim_move_down  vim_command_execute_ntimes<_vim_ntimes_move_down>
#define vim_move_left  vim_command_execute_ntimes<_vim_ntimes_move_left>
#define vim_move_right vim_command_execute_ntimes<_vim_ntimes_move_right>

#define vim_move_up_by_page   vim_command_execute_ntimes<_vim_ntimes_move_up_by_page>
#define vim_move_down_by_page vim_command_execute_ntimes<_vim_ntimes_move_down_by_page>
#define vim_move_up_by_page_half   vim_command_execute_ntimes<_vim_ntimes_move_up_by_page_half>
#define vim_move_down_by_page_half vim_command_execute_ntimes<_vim_ntimes_move_down_by_page_half>

#define vim_move_up_by_whitespace   vim_command_execute_ntimes<_vim_ntimes_move_up_by_whitespace>
#define vim_move_down_by_whitespace vim_command_execute_ntimes<_vim_ntimes_move_down_by_whitespace>


#define vim_move_right_word_start  vim_command_execute_ntimes<_vim_ntimes_move_right_word_start>
#define vim_move_right_token_start vim_command_execute_ntimes<_vim_ntimes_move_right_token_start>
#define vim_move_right_one_after_whitespace  vim_command_execute_ntimes<_vim_ntimes_move_right_one_after_whitespace>

#define vim_move_right_word_end  vim_command_execute_ntimes<_vim_ntimes_move_right_word_end, true>
#define vim_move_right_token_end vim_command_execute_ntimes<_vim_ntimes_move_right_token_end, true>
#define vim_move_right_one_before_whitespace  vim_command_execute_ntimes<_vim_ntimes_move_right_one_before_whitespace, true>


#define vim_move_left_word_start  vim_command_execute_ntimes<_vim_ntimes_move_left_word_start>
#define vim_move_left_token_start  vim_command_execute_ntimes<_vim_ntimes_move_left_token_start>
#define vim_move_left_one_before_whitespace  vim_command_execute_ntimes<_vim_ntimes_move_left_one_before_whitespace>

#define vim_move_left_word_end  vim_command_execute_ntimes<_vim_ntimes_move_left_word_end>
#define vim_move_left_token_end vim_command_execute_ntimes<_vim_ntimes_move_left_token_end>
#define vim_move_left_one_after_whitespace  vim_command_execute_ntimes<_vim_ntimes_move_left_one_after_whitespace>

// @note execute once

#define vim_move_to_file_start vim_command_execute_ntimes<_vim_once_move_to_file_start>
#define vim_move_to_file_end   vim_command_execute_ntimes<_vim_once_move_to_file_end>

#define vim_move_to_line_start vim_command_execute_ntimes<_vim_once_move_to_line_start>
#define vim_move_to_line_end   vim_command_execute_ntimes<_vim_once_move_to_line_end>

#define vim_goto_line vim_command_execute_ntimes<_vim_once_goto_line>

#define vim_move_right_to_found     vim_command_execute_ntimes<_vim_ntimes_move_right_to_found, true>
#define vim_move_right_before_found vim_command_execute_ntimes<_vim_ntimes_move_right_before_found, true>
#define vim_move_left_to_found      vim_command_execute_ntimes<_vim_ntimes_move_left_to_found, true>
#define vim_move_left_before_found  vim_command_execute_ntimes<_vim_ntimes_move_left_before_found, true>

#define vim_set_mark  vim_command_execute_ntimes<_vim_set_mark>
#define vim_cursor_mark_swap  vim_command_execute_ntimes<_vim_cursor_mark_swap>


//
// @note commands to increase the ntimes count
//
template <int x>
CUSTOM_COMMAND_SIG(vim_execute_command_count_add_predecimal) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    
    if (x == 0 &&
        vim_state->predecimal_count == 0) {
        vim_state->execute_command_count = 1;
        return;
    }
    
    // if (vim_state->predecimal_count >= 3)  return;
    ++vim_state->predecimal_count;
    
    if (vim_state->predecimal_count > 1) {
        vim_state->execute_command_count *= 10;
        vim_state->execute_command_count += x;
    }
    else if (vim_state->predecimal_count == 1) {
        vim_state->execute_command_count = x;
    }
}

#define vim_execute_command_count_add_predecimal_0  vim_execute_command_count_add_predecimal<0>
#define vim_execute_command_count_add_predecimal_1  vim_execute_command_count_add_predecimal<1>
#define vim_execute_command_count_add_predecimal_2  vim_execute_command_count_add_predecimal<2>
#define vim_execute_command_count_add_predecimal_3  vim_execute_command_count_add_predecimal<3>
#define vim_execute_command_count_add_predecimal_4  vim_execute_command_count_add_predecimal<4>
#define vim_execute_command_count_add_predecimal_5  vim_execute_command_count_add_predecimal<5>
#define vim_execute_command_count_add_predecimal_6  vim_execute_command_count_add_predecimal<6>
#define vim_execute_command_count_add_predecimal_7  vim_execute_command_count_add_predecimal<7>
#define vim_execute_command_count_add_predecimal_8  vim_execute_command_count_add_predecimal<8>
#define vim_execute_command_count_add_predecimal_9  vim_execute_command_count_add_predecimal<9>

CUSTOM_COMMAND_SIG(vim_move_to_line_start__or__vim_execute_command_count_add_predecimal_0) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    
    if (vim_state->predecimal_count == 0) {
        vim_move_to_line_start(app);
    }
    else {
        vim_execute_command_count_add_predecimal_0(app);
    }
}

//
// @note action commands
//

#if 0
// @todo vim_search could maybe also be a compound_move. So it sets the selection_range.
CUSTOM_COMMAND_SIG(vim_search) {
}
CUSTOM_COMMAND_SIG(vim_search_reverse) {
}
CUSTOM_COMMAND_SIG(vim_search_next) {
}
CUSTOM_COMMAND_SIG(vim_search_previous) {
}
#endif


CUSTOM_COMMAND_SIG(vim_delete_char) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    vim_state->pending_action = vim_action_delete_range;
    vim_move_right(app);
}

CUSTOM_COMMAND_SIG(vim_delete_line) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    vim_state->pending_action = vim_action_delete_range;
    vim_exec_pending_action_on_line_range(app);
}

CUSTOM_COMMAND_SIG(vim_yank_line) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    vim_state->pending_action = vim_action_yank_range;
    vim_exec_pending_action_on_line_range(app);
}


CUSTOM_COMMAND_SIG(vim_replace_character) {
    User_Input user_input = get_current_input(app);
    String_Const_u8 replace = to_writable(&user_input);
    if (replace.str == 0 || replace.size <= 0)  return;
    
    View_ID view_id = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadWriteVisible);
    
    Range_i64 range = {};
    range.start = view_get_cursor_pos(app, view_id);
    range.end = range.start + 1;
    
    buffer_replace_range(app, buffer_id, range, replace);
}
CUSTOM_COMMAND_SIG(vim_replace_character_and_move_right) {
    vim_replace_character(app);
    move_right(app);
}
CUSTOM_COMMAND_SIG(vim_replace_character_and_enter_mode_normal) {
    vim_replace_character(app);
    vim_enter_mode_normal(app);
}


//
// @note visual mode specific commands
//

CUSTOM_COMMAND_SIG(vim_visual_mode_delete) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    vim_state->pending_action = vim_action_delete_range;
    vim_exec_pending_action(app, vim_state->selection_range, (vim_state->mode == vim_mode_visual_line));
    vim_enter_mode_normal(app);
}

CUSTOM_COMMAND_SIG(vim_visual_mode_yank) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    vim_state->pending_action = vim_action_yank_range;
    vim_exec_pending_action(app, vim_state->selection_range, (vim_state->mode == vim_mode_visual_line));
    vim_enter_mode_normal(app);
}

CUSTOM_COMMAND_SIG(vim_visual_mode_change) {
    VIM_GET_VIEW_ID_SCOPE_AND_VIM_STATE(app);
    vim_state->pending_action = vim_action_change_range;
    vim_exec_pending_action(app, vim_state->selection_range, (vim_state->mode == vim_mode_visual_line));
}


//
// @note paste commands
//

function void
vim_paste(Application_Links *app, b32 paste_after = true) {
    i32 count = clipboard_count(app, 0);
    if (count <= 0)  return;
    
    View_ID view_id = get_active_view(app, Access_ReadWriteVisible);
    if_view_has_highlighted_range_delete_range(app, view_id);
    
    Managed_Scope view_scope = view_get_managed_scope(app, view_id);
    Rewrite_Type *next_rewrite = scope_attachment(app, view_scope, view_next_rewrite_loc, Rewrite_Type);
    *next_rewrite = Rewrite_Paste;
    i32 *paste_index = scope_attachment(app, view_scope, view_paste_index_loc, i32);
    *paste_index = 0;
    
    Scratch_Block scratch(app);
    
    String_Const_u8 string = push_clipboard_index(app, scratch, 0, *paste_index);
    if (string.size <= 0)  return;
    
    // @note Check if there are newline characters in the string
    b32 is_line = false;
    for (u64 i = 0; i < string.size; ++i) {
        if (character_is_newline(string.str[i])) {
            is_line = true;
            break;
        }
    }
    
    i64 pos = view_get_cursor_pos(app, view_id);
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadWriteVisible);
    i64 buffer_size = buffer_get_size(app, buffer_id);
    
    if (is_line) {
        i64 line_number = get_line_number_from_pos(app, buffer_id, pos);
        if (paste_after) {
            i64 buffer_line_count = buffer_get_line_count(app, buffer_id);
            if (line_number == buffer_line_count) {
                buffer_replace_range(app, buffer_id, Ii64(buffer_size), string_u8_litexpr("\n"));
            }
            ++line_number;
        }
        pos = get_line_start_pos(app, buffer_id, line_number);
    }
    else if (paste_after) {
        if (pos < buffer_size)  ++pos;
    }
    
    buffer_replace_range(app, buffer_id, Ii64(pos), string);
    
    // :cursor_mark
    // @note paste_after positions
    i64 new_cursor_pos = pos + (i64)string.size - 1;
    i64 new_mark_pos = pos;
    if (!paste_after) {
        // @note paste_before positions
        new_mark_pos = new_cursor_pos;
        new_cursor_pos = pos;
    }
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(new_cursor_pos));
    view_set_mark(app, view_id, seek_pos(new_mark_pos));
    
    // @note Fade pasted text
    ARGB_Color argb_fade = fcolor_resolve(fcolor_id(defcolor_paste));
    view_post_fade(app, view_id, 0.667f, Ii64_size(pos, string.size), argb_fade);
#if 0
    // @todo 4coder 4.0 Do we need to do that?
    if (!is_line) {
        view_post_fade(app, &view, 0.667f, pos, pos + len, paste.color);
    } else {
        view_post_fade(app, &view, 0.667f, seek_line_beginning(app, &buffer, pos), seek_line_beginning(app, &buffer, pos) + len, paste.color);
    }
#endif
}

CUSTOM_COMMAND_SIG(vim_paste_after) {
    vim_paste(app, true);
}
CUSTOM_COMMAND_SIG(vim_paste_after_and_indent) {
    vim_paste_after(app);
    auto_indent_range(app);
}

CUSTOM_COMMAND_SIG(vim_paste_before) {
    vim_paste(app, false);
}
CUSTOM_COMMAND_SIG(vim_paste_before_and_indent) {
    vim_paste_before(app);
    auto_indent_range(app);
}

CUSTOM_COMMAND_SIG(vim_paste_next) {
    Scratch_Block scratch(app);
    
    i32 count = clipboard_count(app, 0);
    if (count <= 0)  return;
    View_ID view_id = get_active_view(app, Access_ReadWriteVisible);
    Managed_Scope view_scope = view_get_managed_scope(app, view_id);
    no_mark_snap_to_cursor(app, view_scope);
    
    Rewrite_Type *rewrite = scope_attachment(app, view_scope, view_rewrite_loc, Rewrite_Type);
    if (*rewrite != Rewrite_Paste)  return;
    
    Rewrite_Type *next_rewrite = scope_attachment(app, view_scope, view_next_rewrite_loc, Rewrite_Type);
    *next_rewrite = Rewrite_Paste;
    
    i32 *paste_index_ptr = scope_attachment(app, view_scope, view_paste_index_loc, i32);
    i32 paste_index = (*paste_index_ptr) + 1;
    *paste_index_ptr = paste_index;
    
    String_Const_u8 string = push_clipboard_index(app, scratch, 0, paste_index);
    
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadWriteVisible);
    
    Range_i64 range = get_view_range(app, view_id);
    range.one_past_last = range.end + 1;
    i64 pos = range.min;
    
    buffer_replace_range(app, buffer_id, range, string);
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(pos + string.size));
    
    // :cursor_mark
    // @todo The cursor mark pos should keep the previous order,
    //       depending on if it was paste_after or paste_before.
    i64 new_cursor_pos = pos + (i64)string.size - 1;
    i64 new_mark_pos = pos;
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(new_cursor_pos));
    view_set_mark(app, view_id, seek_pos(new_mark_pos));
    
    // @note Fade pasted text
    ARGB_Color argb = fcolor_resolve(fcolor_id(defcolor_paste));
    view_post_fade(app, view_id, 0.667f, Ii64_size(pos, string.size), argb);
    
}
CUSTOM_COMMAND_SIG(vim_paste_next_and_indent) {
    vim_paste_next(app);
    auto_indent_range(app);
}

//
// @note Improved newline
//
// @todo Arguments as enum?
// @todo Maybe use List_String_Const_u8 and string_list_flatten() if needed.
// @cleanup Some ranges can just be i64
// @todo Multiline comment, maybe insert * at the start, because of 4coder indentation.
function void
vim_improved_newline(Application_Links *app, b32 below/* = true*/, b32 newline_at_cursor/* = true*/) {
    VIM_GET_VIEW_ID_AND_BUFFER_ID(app);
    
    // @note Get current line range
    i64 cursor_pos = view_get_cursor_pos(app, view_id);
    i64 line_number = get_line_number_from_pos(app, buffer_id, cursor_pos);
    Range_i64 line_range = get_line_pos_range(app, buffer_id, line_number);
    line_range.one_past_last = line_range.end + 1;
    
    Scratch_Block scratch(app);
    String_Const_u8 line_continuation_string = {0};
    
    // @note Check for comment and surrounding braces
    b32 has_surrounding_braces = false;
    b32 cursor_is_inside_comment = false;
    Managed_Scope buffer_scope = buffer_get_managed_scope(app, buffer_id);
    b32 file_is_c_like = *scope_attachment(app, buffer_scope, buffer_file_is_c_like, b32);
    if (file_is_c_like) {
        // @note Preserve on u8 at the start of the string
        line_continuation_string.str = push_array(scratch, u8, range_size(line_range + 1));
        line_continuation_string.str[0] = '\n';
        ++line_continuation_string.str;
        
        String_Const_u8 line_string = line_continuation_string;
        line_string.size = range_size(line_range);
        buffer_read_range(app, buffer_id, line_range, line_string.str);
        if (line_string.size <= 1) {
            if (line_string.str[0] == '\n') {
                line_continuation_string.size = 1;
            }
            goto end_c_comment;
        }
        
        // @note Get comment range
        b32 line_is_comment = false;
        Range_i64 comment_range = {};
        u8 *c = line_string.str;
        int i = 0;
        for (; i < line_string.size; ++i) {
            if (!character_is_whitespace(*c)) {
                if (*c == '/' && *(c + 1) == '/') {
                    line_is_comment = true;
                    comment_range = Ii64(line_range.start + i, line_range.one_past_last);
                    c += 2;
                }
                break;
            }
            ++c;
        }
        if (!line_is_comment) {
            // @note Check for surrounding braces, if not inside a comment
            i64 braces_pos = cursor_pos - line_range.start - 1;
            if (line_string.str[braces_pos] == '{'  &&
                line_string.str[braces_pos+1] == '}') {
                has_surrounding_braces = true;
            }
            
            goto end_c_comment;
        }
        if (cursor_pos < comment_range.start + 2)  goto end_c_comment;
        cursor_is_inside_comment = true;
        
        // @note Get text range inside comment
        Range_i64 comment_text_range = {};
        for (i += 2; i < line_range.max; ++i) {
            if (!character_is_whitespace(*c)) {
                comment_text_range = Ii64(line_range.start + i, line_range.one_past_last);
                break;
            }
            ++c;
        }
        b32 cursor_is_inside_comment_text = (cursor_pos >= comment_text_range.start);
        
        // @note Search for colon as new comment_text_range start
        b32 cursor_is_after_comment_text_colon = false;
        for (; i < line_range.max; ++i) {
            if (*c == ':') {
                if (!character_is_whitespace(*(c - 1)) && character_is_whitespace(*(c + 1)) ) {
                    if (cursor_pos >= (line_range.start + i + 1)) {
                        cursor_is_after_comment_text_colon = true;
                        comment_text_range = Ii64(line_range.start + i + 2, line_range.one_past_last);
                    }
                    break;
                }
            }
            ++c;
        }
        
        // @note Get line continuation range and string
        Range_i64 line_continuation_range = {0};
        if (cursor_is_after_comment_text_colon) {
            for (i = (int)comment_range.start + 2; i < comment_text_range.start; ++i) {
                line_string.str[i - line_range.start] = ' ';
            }
            line_continuation_range = Ii64(line_range.start, comment_text_range.start);
        }
        else if (cursor_is_inside_comment_text) {
            line_continuation_range = Ii64(line_range.start, comment_text_range.start);
        }
        else if (cursor_is_inside_comment) {
            // @note Plus 1 adds one space after the double-slash
            line_continuation_range = Ii64(line_range.start, (comment_range.start + 2) + 1);
        }
        if (cursor_is_inside_comment_text || cursor_is_after_comment_text_colon) {
            if (comment_text_range.start == (comment_range.start + 2)) {
                line_string.str[comment_text_range.start - line_range.start] = ' ';
                ++line_continuation_range.end;
            }
        }
        line_continuation_string = { line_string.str, (u64)range_size(line_continuation_range) };
    }
    end_c_comment:;
    
    if (line_continuation_string.size == 0) {
        --line_continuation_string.str;
        line_continuation_string.size = 1;
    }
    else if (line_continuation_string.str[line_continuation_string.size - 1] != '\n') {
        line_continuation_string.str[line_continuation_string.size++] = '\n';
    }
    
    // @note Get the insert position
    Range_i64 range = {};
    if (newline_at_cursor) {
        range = Ii64(cursor_pos);
        
        if (cursor_is_inside_comment) {
            --line_continuation_string.str;
            // @note Newline at the end is already removed because of decrementing the pointer.
            // --line_continuation_string.size;
        }
        else {
            if (has_surrounding_braces) {
                line_continuation_string.str[1] = '\n';
                line_continuation_string.size = 2;
            }
        }
    }
    else {
        if (below)  range = Ii64(line_range.one_past_last);
        else  range = Ii64(line_range.start);
    }
    
    buffer_replace_range(app, buffer_id, range, line_continuation_string);
    
    // :cursor_mark
    i64 new_cursor_pos = range.start + line_continuation_string.size;
    if (!newline_at_cursor ||
        (newline_at_cursor && has_surrounding_braces)) {
        --new_cursor_pos;
    }
    view_set_cursor_and_preferred_x(app, view_id, seek_pos(new_cursor_pos));
}

CUSTOM_COMMAND_SIG(vim_newline) {
    vim_improved_newline(app);
}

//
// @note Window/view commands
//
template <CUSTOM_COMMAND_SIG(command)>
CUSTOM_COMMAND_SIG(vim_window_command) {
    vim_enter_mode_normal(app);
    command(app);
}

CUSTOM_COMMAND_SIG(_vim_swap_buffers_between_two_views) {
    swap_panels(app);
}

CUSTOM_COMMAND_SIG(_vim_cycle_view_focus) {
    View_ID view_id = get_active_view(app, Access_Always);
    View_ID next_view_id = get_next_view_looped_all_panels(app, view_id, Access_Always);
    view_set_active(app, next_view_id);
}
CUSTOM_COMMAND_SIG(_vim_rotate_view_buffers) {
    // @todo Rotate all buffers
    _vim_swap_buffers_between_two_views(app);
}

CUSTOM_COMMAND_SIG(_vim_open_view_duplicate_split_vertical) {
    open_panel_vsplit(app);
}
CUSTOM_COMMAND_SIG(_vim_open_view_duplicate_split_horizontal) {
    open_panel_hsplit(app);
}

CUSTOM_COMMAND_SIG(_vim_open_view_split_vertical) {
    View_ID view = get_active_view(app, Access_Always);
    View_ID new_view = open_view(app, view, ViewSplit_Right);
    new_view_settings(app, new_view);
}
CUSTOM_COMMAND_SIG(_vim_open_view_split_horizontal) {
    View_ID view = get_active_view(app, Access_Always);
    View_ID new_view = open_view(app, view, ViewSplit_Bottom);
    new_view_settings(app, new_view);
}

CUSTOM_COMMAND_SIG(_vim_close_view) {
    close_panel(app);
}

// @todo Fix focus commands
CUSTOM_COMMAND_SIG(_vim_focus_view_left) {
    View_ID view_id = get_active_view(app, Access_Always);
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    f32 x0 = view_rect.x0;
    f32 y0 = view_rect.y0;
    
    View_ID best_view_id = view_id;
    Rect_f32 best_view_rect = view_rect;
    
    for_views(app, it) {
        if (it == view_id)  continue;
        
        Rect_f32 it_rect = view_get_screen_rect(app, it);
        if (y0 < it_rect.y0 || y0 > it_rect.y1)  continue;
        if (x0 < it_rect.x0)  continue;
        
        if (best_view_id == view_id ||
            it_rect.x0 > best_view_rect.x0) {
            best_view_id   = it;
            best_view_rect = it_rect;
        }
    }
    
    view_set_active(app, best_view_id);
}
CUSTOM_COMMAND_SIG(_vim_focus_view_right) {
    View_ID view_id = get_active_view(app, Access_Always);
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    f32 x0 = view_rect.x0;
    f32 y0 = view_rect.y0;
    
    View_ID best_view_id = view_id;
    Rect_f32 best_view_rect = view_rect;
    
    for_views(app, it) {
        if (it == view_id)  continue;
        
        Rect_f32 it_rect = view_get_screen_rect(app, it);
        if (y0 < it_rect.y0 || y0 > it_rect.y1)  continue;
        if (x0 > it_rect.x0)  continue;
        
        if (best_view_id == view_id ||
            it_rect.x0 < best_view_rect.x0) {
            best_view_id   = it;
            best_view_rect = it_rect;
        }
    }
    
    view_set_active(app, best_view_id);
}
CUSTOM_COMMAND_SIG(_vim_focus_view_down) {
    View_ID view_id = get_active_view(app, Access_Always);
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    f32 x0 = view_rect.x0;
    f32 y0 = view_rect.y0;
    
    View_ID best_view_id = view_id;
    Rect_f32 best_view_rect = view_rect;
    
    for_views(app, it) {
        if (it == view_id)  continue;
        
        Rect_f32 it_rect = view_get_screen_rect(app, it);
        if (x0 < it_rect.x0 || x0 > it_rect.x1)  continue;
        if (y0 < it_rect.y0)  continue;
        
        if (best_view_id == view_id ||
            it_rect.y0 > best_view_rect.y0) {
            best_view_id   = it;
            best_view_rect = it_rect;
        }
    }
    
    view_set_active(app, best_view_id);
}
CUSTOM_COMMAND_SIG(_vim_focus_view_up) {
    View_ID view_id = get_active_view(app, Access_Always);
    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    f32 x0 = view_rect.x0;
    f32 y0 = view_rect.y0;
    
    View_ID best_view_id = view_id;
    Rect_f32 best_view_rect = view_rect;
    
    for_views(app, it) {
        if (it == view_id)  continue;
        
        Rect_f32 it_rect = view_get_screen_rect(app, it);
        if (x0 < it_rect.x0 || x0 > it_rect.x1)  continue;
        if (y0 > it_rect.y0)  continue;
        
        if (best_view_id == view_id ||
            it_rect.y0 < best_view_rect.y0) {
            best_view_id   = it;
            best_view_rect = it_rect;
        }
    }
    
    view_set_active(app, best_view_id);
}


#define vim_swap_buffers_between_two_views  vim_window_command<_vim_swap_buffers_between_two_views>

#define vim_cycle_view_focus    vim_window_command<_vim_cycle_view_focus>
#define vim_rotate_view_buffers vim_window_command<_vim_rotate_view_buffers>

#define vim_open_view_duplicate_split_vertical   vim_window_command<_vim_open_view_duplicate_split_vertical>
#define vim_open_view_duplicate_split_horizontal vim_window_command<_vim_open_view_duplicate_split_horizontal>

#define vim_open_view_split_horizontal vim_window_command<_vim_open_view_split_horizontal>
#define vim_open_view_split_vertical   vim_window_command<_vim_open_view_split_vertical>

#define vim_close_view  vim_window_command<_vim_close_view>

#define vim_focus_view_left  vim_window_command<_vim_focus_view_left>
#define vim_focus_view_right vim_window_command<_vim_focus_view_right>
#define vim_focus_view_down  vim_window_command<_vim_focus_view_down>
#define vim_focus_view_up    vim_window_command<_vim_focus_view_up>

//
// @note File commands
//

#define vim_exit_4coder  vim_window_command<exit_4coder>
#define vim_interactive_open_or_new  vim_window_command<interactive_open_or_new>

// @copynpaste auto_indent_whole_file
function void
auto_indent_whole_file(Application_Links *app, Buffer_ID buffer_id) {
    i64 buffer_size = buffer_get_size(app, buffer_id);
    auto_indent_buffer(app, buffer_id, Ii64(0, buffer_size));
}

// @copynpaste clean_all_lines
function void
clean_all_lines(Application_Links *app, Buffer_ID buffer_id) {
    ProfileScope(app, "clean all lines");
    
    Scratch_Block scratch(app);
    Batch_Edit *batch_first = 0;
    Batch_Edit *batch_last = 0;
    
    String_Const_u8 text = push_whole_buffer(app, scratch, buffer_id);
    
    u64 whitespace_start = 0;
    for (u64 i = 0; i < text.size; i += 1) {
        u8 v = string_get_character(text, i);
        if (v == '\n' || i + 1 == text.size) {
            if (whitespace_start < i) {
                Batch_Edit *batch = push_array(scratch, Batch_Edit, 1);
                sll_queue_push(batch_first, batch_last, batch);
                batch->edit.text = SCu8();
                batch->edit.range = Ii64(whitespace_start, i);
            }
            whitespace_start = i + 1;
        }
        else if (!character_is_whitespace(v)) {
            whitespace_start = i + 1;
        }
    }
    
    buffer_batch_edit(app, buffer_id, batch_first);
}

// @copynpaste set_eol_mode_to_lf
// @note eol_nixify is lf line ending mode. Unix line ending.
function void
buffer_eol_nixify(Application_Links *app, Buffer_ID buffer_id) {
    Managed_Scope buffer_scope = buffer_get_managed_scope(app, buffer_id);
    Line_Ending_Kind *eol_setting = scope_attachment(app, buffer_scope, buffer_eol_setting, Line_Ending_Kind);
    if (eol_setting != 0) {
        *eol_setting = LineEndingKind_LF;
    }
}

inline void
vim_clean_and_save_buffer(Application_Links *app, Scratch_Block *scratch, Buffer_ID buffer_id, String_Const_u8 postfix) {
    auto_indent_whole_file(app, buffer_id);
    clean_all_lines(app, buffer_id);
    buffer_eol_nixify(app, buffer_id);
    
    Temp_Memory temp = begin_temp(*scratch);
    String_Const_u8 file_name = push_buffer_file_name(app, *scratch, buffer_id);
    if (string_match(string_postfix(file_name, postfix.size), postfix)) {
        buffer_save(app, buffer_id, file_name, 0);
    }
    end_temp(temp);
}

CUSTOM_COMMAND_SIG(vim_clean_and_save) {
    VIM_GET_VIEW_ID_AND_BUFFER_ID(app);
    String_Const_u8 postfix = {0};
    Scratch_Block scratch(app);
    
    vim_clean_and_save_buffer(app, &scratch, buffer_id, postfix);
}

CUSTOM_COMMAND_SIG(vim_clean_and_save_all_dirty_buffers) {
    ProfileScope(app, "clean and save all dirty buffers");
    String_Const_u8 postfix = {0};
    
    Scratch_Block scratch(app);
    for (Buffer_ID buffer_id = get_buffer_next(app, 0, Access_ReadWriteVisible);
         buffer_id != 0;
         buffer_id = get_buffer_next(app, buffer_id, Access_ReadWriteVisible)) {
        Dirty_State dirty = buffer_get_dirty_state(app, buffer_id);
        if (dirty == DirtyState_UnsavedChanges) {
            vim_clean_and_save_buffer(app, &scratch, buffer_id, postfix);
        }
    }
}

//
// @note Buffer commands
//

#define vim_interactive_switch_buffer  vim_window_command<interactive_switch_buffer>
#define vim_interactive_kill_buffer    vim_window_command<interactive_kill_buffer>


//
// @note Project commands (build, debug, ...)
//

CUSTOM_COMMAND_SIG(vim_clean_save_all_dirty_buffers_and_build) {
    vim_clean_and_save_all_dirty_buffers(app);
    build_in_build_panel(app);
}

CUSTOM_COMMAND_SIG(vim_save_all_dirty_buffers_and_build) {
    save_all_dirty_buffers(app);
    build_in_build_panel(app);
}


//
// @note Vim statusbar
//

#define VIM_COMMAND_SIG(name)  void name(struct Application_Links *app, \
const String_Const_u8 command, \
const String_Const_u8 arg_str, \
b32 force)
typedef VIM_COMMAND_SIG(Vim_Command_Function);
typedef CUSTOM_COMMAND_SIG(Vim_Custom_Command_Function);

struct Vim_Command_Node {
    Vim_Command_Node *next;
    
    String_Const_u8 command;
    
    b32 is_vim_command;
    union {
        Vim_Command_Function        *vim_command_function;
        Vim_Custom_Command_Function *custom_command_function;
    };
};

struct Vim_Command_List {
    Vim_Command_Node *first;
    Vim_Command_Node *last;
    i32 count;
};

global Arena global_vim_command_arena = {};
global Vim_Command_List global_vim_command_list = {};

inline Vim_Command_Node *
vim_make_command() {
    if (global_vim_command_arena.base_allocator == 0) {
        global_vim_command_arena = make_arena_system();
    }
    
    Vim_Command_Node *node = push_array(&global_vim_command_arena, Vim_Command_Node, 1);
    sll_queue_push(global_vim_command_list.first, global_vim_command_list.last, node);
    ++global_vim_command_list.count;
    
    return node;
}
function void
_vim_define_command(String_Const_u8 command_string, Vim_Command_Function func) {
    Vim_Command_Node *command = vim_make_command();
    command->command = command_string;
    command->is_vim_command = true;
    command->vim_command_function = func;
}
function void
_vim_define_command(String_Const_u8 command_string, Vim_Custom_Command_Function func) {
    Vim_Command_Node *command = vim_make_command();
    command->command = command_string;
    command->is_vim_command = false;
    command->custom_command_function = func;
}

#define vim_define_command(command, func)  _vim_define_command(SCu8(command), func)


CUSTOM_COMMAND_SIG(vim_status_command) {
    vim_enter_mode_normal(app);
    
    
    Query_Bar_Group group(app);
    Query_Bar bar = {};
    if (start_query_bar(app, &bar, 0) == 0) {
        return;
    }
    
    u8 bar_string_space[256];
    bar.string = SCu8(bar_string_space, (u64)0);
    bar.prompt = string_u8_litexpr(":");
    
    User_Input input = {};
    for (;;) {
        input = get_next_input(app, EventPropertyGroup_AnyKeyboardEvent, EventProperty_Escape | EventProperty_ViewActivation);
        if (input.abort)  return;
        
        String_Const_u8 input_string = to_writable(&input);
        
        if (match_key_code(&input, KeyCode_Return)) {
            break;
        }
        else if (match_key_code(&input, KeyCode_Tab)) {
            // @todo auto complete
        }
        else if (input_string.str != 0 && input_string.size > 0) {
            String_u8 bar_string = Su8(bar.string, sizeof(bar_string_space));
            string_append(&bar_string, input_string);
            bar.string = bar_string.string;
        }
        else if (match_key_code(&input, KeyCode_Backspace)) {
            if (is_unmodified_key(&input.event)) {
                bar.string = backspace_utf8(bar.string);
            }
            else if (has_modifier(&input.event.key.modifiers, KeyCode_Control)) {
                if (bar.string.size > 0) {
                    bar.string.size = 0;
                }
            }
        }
        else {
            leave_current_input_unhandled(app);
        }
    }
    
    
    u64 command_offset = 0;
    while (command_offset < bar.string.size && character_is_whitespace(bar.string.str[command_offset])) {
        ++command_offset;
    }
    u64 command_end = command_offset;
    while (command_end < bar.string.size && !character_is_whitespace(bar.string.str[command_end])) {
        ++command_end;
    }
    if (command_offset == command_end)  return;
    
    String_Const_u8 command = string_substring(bar.string, Ii64(command_offset, command_end));
    
    b32 force_command = false;
    if (command.str[command.size - 1] == '!') {
        --command.size;
        force_command = true;
    }
    
    // @note goto line
    if (string_is_integer(bar.string, 10)) {
        u64 line_number = string_to_integer(bar.string, 10);
        View_ID view_id = get_active_view(app, Access_ReadVisible);
        view_set_cursor_and_preferred_x(app, view_id, seek_line_col(line_number, 0));
        return;
    }
    
    // @note find and call command
    for (Vim_Command_Node *node = global_vim_command_list.first; node != 0; node = node->next) {
        if (string_match(node->command, command)) {
            if (node->is_vim_command) {
                // @note command arguments
                u64 args_start = command_end;
                while (args_start < bar.string.size && character_is_whitespace(bar.string.str[args_start])) {
                    ++args_start;
                }
                String_Const_u8 args_string = string_substring(bar.string, Ii64(args_start, (bar.string.size - args_start)));
                
                node->vim_command_function(app, command, args_string, force_command);
            }
            else {
                node->custom_command_function(app);
            }
        }
    }
}


//
// @note Statusbar commands
//

VIM_COMMAND_SIG(vim_exec_regex) {
    // @todo
}
VIM_COMMAND_SIG(vim_move_lines_selection) {
    
}
