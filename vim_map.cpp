function void
vim_setup_mapping(Mapping *mapping) {
    MappingScope();
    SelectMapping(mapping);
    
    //
    // @note: Vim commands
    //
    vim_define_command("s", vim_exec_regex);
    vim_define_command("m", vim_move_lines_selection);
    
    vim_define_command("e",  vim_interactive_open_or_new);
    vim_define_command("edit",  vim_interactive_open_or_new);
    
    vim_define_command("b", vim_interactive_switch_buffer);
    vim_define_command("bw", vim_interactive_kill_buffer);
    
    // @todo vim_define_command("new",   vim_new_file);
    // @todo vim_define_command("vnew",  vim_new_file_open_vertical);
    vim_define_command("write", vim_clean_and_save);
    vim_define_command("wa",    vim_clean_and_save_all_dirty_buffers);
    // @todo vim_define_command("x",     vim_write_file_and_close_view);
    // @todo vim_define_command("wq",    vim_write_file_and_close_view);
    // @todo vim_define_command("xa",    vim_write_file_and_close_all);
    // @todo vim_define_command("wqa",   vim_write_file_and_close_all);
    
    // @todo vim_define_command("exit",  vim_write_file_and_close_view);
    // @todo vim_define_command("exitall", vim_write_file_and_close_view);
    
    vim_define_command("close",   close_panel);
    vim_define_command("quit",    close_panel);
    vim_define_command("quitall", exit_4coder);
    vim_define_command("qa",      exit_4coder);
    
    vim_define_command("vs",     vim_open_view_duplicate_split_vertical);
    vim_define_command("vsplit", vim_open_view_duplicate_split_vertical);
    vim_define_command("sp",     vim_open_view_duplicate_split_horizontal);
    vim_define_command("split",  vim_open_view_duplicate_split_horizontal);
    
    // @todo vim_define_command("cd",          vim_change_directory);
    // @todo vim_define_command("colorscheme", vim_change_colorscheme);
    
    
    //
    // @note: Vim keybindings
    //
    SelectMap(mapid_vim_unbound); // :unbound
    {
        BindCore(vim_startup, CoreCode_Startup);
        BindCore(default_try_exit, CoreCode_TryExit);
    }
    
    SelectMap(mapid_vim_escape_to_normal_mode);
    {
        ParentMap(mapid_vim_unbound);
        
        Bind(vim_enter_mode_normal, KeyCode_Escape);
        Bind(vim_enter_mode_normal, KeyCode_Space, KeyCode_Alt);
    }
    
    //
    // @note Mode maps
    //
    
    /*
            // @note Also has bindings like search, and status_command_input for example.
            // These are all bindings which should be awaiable in all modes which include this command_map.
            // Because we probably want to use a search_command in visual mode for example.
    */
    SelectMap(mapid_vim_movements);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        
        // @note execute command count
        Bind(vim_move_to_line_start__or__vim_execute_command_count_add_predecimal_0, KeyCode_0);
        Bind(vim_execute_command_count_add_predecimal_1, KeyCode_1);
        Bind(vim_execute_command_count_add_predecimal_2, KeyCode_2);
        Bind(vim_execute_command_count_add_predecimal_3, KeyCode_3);
        Bind(vim_execute_command_count_add_predecimal_4, KeyCode_4);
        Bind(vim_execute_command_count_add_predecimal_5, KeyCode_5);
        Bind(vim_execute_command_count_add_predecimal_6, KeyCode_6);
        Bind(vim_execute_command_count_add_predecimal_7, KeyCode_7);
        Bind(vim_execute_command_count_add_predecimal_8, KeyCode_8);
        Bind(vim_execute_command_count_add_predecimal_9, KeyCode_9);
        
        // @note move hjkl
        
        Bind(vim_move_up,    KeyCode_K);
        Bind(vim_move_down,  KeyCode_J);
        Bind(vim_move_left,  KeyCode_H);
        Bind(vim_move_right, KeyCode_L);
        
        Bind(vim_move_to_file_start, KeyCode_K, KeyCode_Control);
        Bind(vim_move_to_file_end,   KeyCode_J, KeyCode_Control);
        // @todo Instead of 0 and $ ?
        // Bind(vim_move_to_line_start, KeyCode_H, KeyCode_Control);
        // Bind(vim_move_to_line_end,   KeyCode_L, KeyCode_Control);
        // @todo Should these be just in normal mode
        // Bind(vim_combine_with_previous_line, KeyCode_H, KeyCode_Control);
        // Bind(vim_combine_with_next_line,     KeyCode_L, KeyCode_Control);
        
        Bind(vim_move_up_by_whitespace,   KeyCode_K, KeyCode_Alt);
        Bind(vim_move_down_by_whitespace, KeyCode_J, KeyCode_Alt);
        
        Bind(vim_move_up_by_page,   KeyCode_K, KeyCode_Shift);
        Bind(vim_move_down_by_page, KeyCode_J, KeyCode_Shift);
        
        Bind(vim_move_up_by_page_half,   KeyCode_K, KeyCode_Shift, KeyCode_Alt);
        Bind(vim_move_down_by_page_half, KeyCode_J, KeyCode_Shift, KeyCode_Alt);
        
        
        // @note move web @todo d?
        Bind(vim_move_right_word_start, KeyCode_W);
        Bind(vim_move_right_word_end,   KeyCode_E);
        Bind(vim_move_left_word_start,  KeyCode_B);
        // Bind(vim_move_left_word_end,    KeyCode_B);
        
        Bind(vim_move_right_one_after_whitespace,  KeyCode_W, KeyCode_Shift);
        Bind(vim_move_right_one_before_whitespace, KeyCode_E, KeyCode_Shift);
        Bind(vim_move_left_one_before_whitespace,  KeyCode_B, KeyCode_Shift);
        // Bind(vim_move_left_one_after_whitespace,   KeyCode_B, KeyCode_Shift);
        
        Bind(vim_move_right_token_start, KeyCode_W, KeyCode_Alt);
        Bind(vim_move_right_token_end,   KeyCode_E, KeyCode_Alt);
        Bind(vim_move_left_token_start,  KeyCode_B, KeyCode_Alt);
        // Bind(vim_move_left_token_end,    KeyCode_B, KeyCode_Alt);
        
        
        Bind(vim_enter_chord_move_right_to_found,     KeyCode_F);
        Bind(vim_enter_chord_move_right_before_found, KeyCode_T);
        Bind(vim_enter_chord_move_left_to_found,      KeyCode_F, KeyCode_Shift);
        Bind(vim_enter_chord_move_left_before_found,  KeyCode_T, KeyCode_Shift);
        
        Bind(vim_move_to_line_start__or__vim_execute_command_count_add_predecimal_0, KeyCode_0);
        Bind(vim_move_to_line_end, KeyCode_4, KeyCode_Shift);
        
        Bind(vim_goto_line, KeyCode_G);
        
        
        // @note :cursor_mark
        // @todo Proper vim marks
        Bind(vim_set_mark,         KeyCode_M); // Should we be able to set mark in visual mode
        Bind(vim_cursor_mark_swap, KeyCode_S);
        // @todo
        // Bind(vim_save_cursor_mark_pos,    KeyCode_S, KeyCode_Alt);
        // Bind(vim_restore_cursor_mark_pos, KeyCode_S, KeyCode_Shift);
        
        
        // @note search bindings
        // @todo
        // Bind(vim_search, KeyCode_ForwardSlash);
        // Bind(vim_search_reverse, KeyCode_?); @keycode_missing
        // Bind(vim_search_next,     KeyCode_N);
        // Bind(vim_search_previous, KeyCode_N, KeyCode_Shift);
        // Bind(vim_search_identifier_under_cursor, KeyCode_*); @keycode_missing
        Bind(search,         KeyCode_S, KeyCode_Control);
        Bind(reverse_search, KeyCode_R, KeyCode_Control);
        Bind(search_identifier,         KeyCode_S, KeyCode_Alt, KeyCode_Control);
        Bind(reverse_search_identifier, KeyCode_R, KeyCode_Alt, KeyCode_Control);
        
        Bind(query_replace,     KeyCode_Q);
        Bind(replace_in_range,  KeyCode_Q, KeyCode_Shift);
        Bind(replace_in_buffer, KeyCode_Q, KeyCode_Shift, KeyCode_Control);
        Bind(query_replace_identifier,     KeyCode_Q, KeyCode_Control);
        
        // @todo
        // This commands are for updating the selection range.
        // @note But I think we don't want them in every mode. Just in visual mode,
        //       so that you don't accidentaly change the cursor pos while typing in insert mode.
        // But maybe we want it in normal mode, so we should probably just unbind it in insert mode.
        //
        // @todo Or we could set suppressing_mouse to true when entering insert mode!
        // Use set_mouse_suppression(b32 suppress);
        // @todo Or we could set suppressing_mouse to true when entering insert mode!
        //
        // bind(context, key_mouse_left, MDFR_NONE, vim_move_click);
        // bind(context, key_mouse_wheel, MDFR_NONE, vim_move_scroll);
        
        Bind(vim_status_command, KeyCode_Period, KeyCode_Shift);
    }
    
    SelectMap(mapid_vim_mode_normal);
    {
        ParentMap(mapid_vim_movements);
        
        // @note :paste
        Bind(vim_paste_after_and_indent, KeyCode_P);
        Bind(vim_paste_before_and_indent, KeyCode_P, KeyCode_Shift);
        Bind(vim_paste_next_and_indent, KeyCode_P, KeyCode_Alt);
        
        // @note delete
        Bind(vim_enter_chord_delete, KeyCode_D);
        Bind(vim_delete_line, KeyCode_D, KeyCode_Shift);
        
        // @note copy/yank
        Bind(vim_enter_chord_yank, KeyCode_Y);
        Bind(vim_yank_line, KeyCode_Y, KeyCode_Shift);
        
        // @note cut
        Bind(vim_delete_char, KeyCode_X);
        
        // @note Modes and chords
        Bind(vim_enter_mode_insert,  KeyCode_I);
        Bind(vim_enter_mode_insert_after,      KeyCode_A);
        Bind(vim_enter_mode_insert_line_start, KeyCode_I, KeyCode_Shift);
        Bind(vim_enter_mode_insert_line_end,   KeyCode_A, KeyCode_Shift);
        Bind(vim_newline_and_enter_mode_insert_after,  KeyCode_O);
        Bind(vim_newline_and_enter_mode_insert_before, KeyCode_O, KeyCode_Shift);
        
        Bind(vim_enter_chord_change, KeyCode_C);
        Bind(vim_enter_chord_replace_single,    KeyCode_R);
        Bind(vim_enter_mode_replace, KeyCode_R, KeyCode_Shift);
        Bind(vim_enter_mode_visual,  KeyCode_V);
        Bind(vim_enter_mode_visual_line, KeyCode_V, KeyCode_Shift);
        
        // @todo Maybe move this to mapid_movements. Centering the view can be useful in other modes.
        Bind(vim_enter_chord_z, KeyCode_Z);
        
        Bind(vim_enter_chord_window, KeyCode_W, KeyCode_Control);
        
        // @keycode_missing Bind(vim_enter_chord_switch_register, KeyCode_");
        
        // @keycode_missing Bind(vim_enter_chord_format, KeyCode_=);
        // @keycode_missing Bind(vim_enter_chord_indent_right, KeyCode_>);
        // @keycode_missing Bind(vim_enter_chord_indent_left,  KeyCode_<);
        
        Bind(undo, KeyCode_U);
        Bind(redo, KeyCode_R, KeyCode_Control);
        
        // @note Build and Debug
        Bind(vim_save_all_dirty_buffers_and_build, KeyCode_M, KeyCode_Alt);
        Bind(goto_next_jump,  KeyCode_N, KeyCode_Alt);
        Bind(goto_prev_jump,  KeyCode_N, KeyCode_Alt, KeyCode_Shift);
        Bind(goto_first_jump, KeyCode_N, KeyCode_Alt, KeyCode_Control);
    }
    
#if 0 // :unbound For what was that used again?
    // @todo mapid_vim_unbound is mapid_global
    // @todo unbound name???
    SelectMap(mapid_vim_unbound);
    {
        ParentMap(mapid_vim_movements);
    }
#endif
    
    SelectMap(mapid_vim_mode_visual);
    {
        ParentMap(mapid_vim_movements);
        // @todo
        // search_selection
        // query_replace_selection
        // vim_query_replace_in_visual_selection_range
        
        Bind(vim_visual_mode_delete, KeyCode_D);
        Bind(vim_visual_mode_delete, KeyCode_X);
        Bind(vim_visual_mode_yank,   KeyCode_Y);
        Bind(vim_visual_mode_change, KeyCode_C);
    }
    
    SelectMap(mapid_vim_mode_insert);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(write_text_input);
        
        Bind(vim_newline, KeyCode_Return);
        Bind(backspace_char, KeyCode_Backspace);
    }
    
#if 0
    SelectMap(mapid_vim_insert_chord_single_quote);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
    }
    
    SelectMap(mapid_vim_insert_chord_double_quote);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
    }
#endif
    
    SelectMap(mapid_vim_mode_replace);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(vim_replace_character_and_move_right);
    }
    
    //
    // @note Chord maps
    //
    
    // @note Chord replace single
    SelectMap(mapid_vim_chord_replace_single);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(vim_replace_character_and_enter_mode_normal);
    }
    
    // @note Chord delete
    SelectMap(mapid_vim_chord_delete);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        ParentMap(mapid_vim_movements);
        
        Bind(vim_exec_pending_action_on_line_range, KeyCode_D);
        // @todo Bind(vim_visual_change_line, KeyCode_C);
    }
    
    // @note Chord yank
    SelectMap(mapid_vim_chord_yank);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        ParentMap(mapid_vim_movements);
        
        Bind(vim_exec_pending_action_on_line_range, KeyCode_Y);
    }
    
    // @note Chord change
    SelectMap(mapid_vim_chord_change);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        ParentMap(mapid_vim_movements);
        
        Bind(vim_exec_pending_action_on_line_range, KeyCode_C);
    }
    
    SelectMap(mapid_vim_chord_choose_register);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
    }
    
    SelectMap(mapid_vim_chord_move_right_to_found);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(vim_move_right_to_found);
    }
    
    SelectMap(mapid_vim_chord_move_right_before_found);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(vim_move_right_before_found);
    }
    
    SelectMap(mapid_vim_chord_move_left_to_found);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(vim_move_left_to_found);
    }
    
    SelectMap(mapid_vim_chord_move_left_before_found);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        BindTextInput(vim_move_left_before_found);
    }
    
    SelectMap(mapid_vim_chord_indent_left);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        
        // @todo @keycode_missing Bind(vim_exec_pending_action_on_line_range, KeyCode_<);
    }
    
    SelectMap(mapid_vim_chord_indent_right);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        
        // @todo @keycode_missing Bind(vim_exec_pending_action_on_line_range, KeyCode_>);
    }
    
    SelectMap(mapid_vim_chord_format);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        
        // @todo @keycode_missing Bind(vim_exec_pending_action_on_line_range, KeyCode_=);
    }
    
    SelectMap(mapid_vim_chord_g);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
    }
    
    SelectMap(mapid_vim_chord_z);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
    }
    
    SelectMap(mapid_vim_chord_window);
    {
        ParentMap(mapid_vim_escape_to_normal_mode);
        
        Bind(vim_cycle_view_focus, KeyCode_W);
        Bind(vim_cycle_view_focus, KeyCode_W, KeyCode_Control);
        Bind(vim_rotate_view_buffers, KeyCode_R);
        Bind(vim_rotate_view_buffers, KeyCode_R, KeyCode_Control);
        
        Bind(vim_open_view_duplicate_split_vertical, KeyCode_V);
        Bind(vim_open_view_duplicate_split_vertical, KeyCode_V, KeyCode_Control);
        Bind(vim_open_view_duplicate_split_horizontal, KeyCode_S);
        Bind(vim_open_view_duplicate_split_horizontal, KeyCode_S, KeyCode_Control);
        
        Bind(vim_open_view_split_horizontal, KeyCode_N);
        Bind(vim_open_view_split_horizontal, KeyCode_N, KeyCode_Control);
        // Bind(vim_open_view_split_vertical, KeyCode_);
        // Bind(vim_open_view_split_vertical, KeyCode_, KeyCode_Control);
        
        Bind(vim_close_view, KeyCode_Q);
        Bind(vim_close_view, KeyCode_Q, KeyCode_Control);
        
        Bind(vim_focus_view_left,  KeyCode_H);
        Bind(vim_focus_view_left,  KeyCode_H, KeyCode_Control);
        Bind(vim_focus_view_down,  KeyCode_J);
        Bind(vim_focus_view_down,  KeyCode_J, KeyCode_Control);
        Bind(vim_focus_view_up,    KeyCode_K);
        Bind(vim_focus_view_up,    KeyCode_K, KeyCode_Control);
        Bind(vim_focus_view_right, KeyCode_L);
        Bind(vim_focus_view_right, KeyCode_L, KeyCode_Control);
    }
}

