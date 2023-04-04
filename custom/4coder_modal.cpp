String_ID mapid_shared;
String_ID mapid_normal;
String_ID mapid_write;

Command_Map_ID get_current_mapid(Application_Links* app)
{
    View_ID view = get_active_view( app, 0 );
    Buffer_ID buffer = view_get_buffer( app, view, 0 );
    Managed_Scope scope = buffer_get_managed_scope( app, buffer );
    Command_Map_ID* map_id_ptr = scope_attachment( app, scope, buffer_map_id, Command_Map_ID );
    return *map_id_ptr;
}

void set_current_mapid( Application_Links* app, Command_Map_ID mapid ) {
    
    View_ID view = get_active_view( app, 0 );
    Buffer_ID buffer = view_get_buffer( app, view, 0 );
    Managed_Scope scope = buffer_get_managed_scope( app, buffer );
    Command_Map_ID* map_id_ptr = scope_attachment( app, scope, buffer_map_id, Command_Map_ID );
    *map_id_ptr = mapid;
}

static ARGB_Color old_cursor_write_color;
static ARGB_Color old_at_cursor_write_color;
static ARGB_Color old_margin_active_write_color;

CUSTOM_COMMAND_SIG(switch_modal)
CUSTOM_DOC("switch modal")
{
    Command_Map_ID map_id = get_current_mapid(app);
    if(map_id == mapid_normal) 
    {
        set_current_mapid(app, mapid_write);
        old_cursor_write_color = active_color_table.arrays[defcolor_cursor].vals[0]; 
        old_at_cursor_write_color = active_color_table.arrays[defcolor_at_cursor].vals[0];
        old_margin_active_write_color = active_color_table.arrays[defcolor_margin_active].vals[0];
        
        active_color_table.arrays[defcolor_cursor].vals[0] = finalize_color(defcolor_cursor_write, 0);
        active_color_table.arrays[defcolor_at_cursor].vals[0] = finalize_color(defcolor_at_cursor_write, 0);
        active_color_table.arrays[defcolor_margin_active].vals[0] = finalize_color(defcolor_margin_active_write, 0);
    }
    else 
    {
        set_current_mapid(app, mapid_normal);
        
        active_color_table.arrays[defcolor_cursor].vals[0] = old_cursor_write_color;
        active_color_table.arrays[defcolor_at_cursor].vals[0] = old_at_cursor_write_color;
        active_color_table.arrays[defcolor_margin_active].vals[0] = old_margin_active_write_color;
    }
}

void set_modal_mappings(Application_Links* app)
{
    Thread_Context* tctx = get_thread_context(app);
    String_ID global_map_id = vars_save_string_lit("keys_global");
    String_ID file_map_id = vars_save_string_lit("keys_file");
    String_ID code_map_id = vars_save_string_lit("keys_code");
    
    // NOTE(allen): setup for default framework
    default_framework_init(app);
    
    // NOTE(allen): default hooks and command maps
    set_all_default_hooks(app);
    mapping_init(tctx, &framework_mapping);
    
    mapid_shared = vars_save_string_lit("modal_shared");
    mapid_normal = vars_save_string_lit("modal_normal");  
    mapid_write  = vars_save_string_lit("modal_write");
    
    MappingScope();
    SelectMapping(&framework_mapping);
    
    SelectMap(global_map_id);
    Bind(command_lister, KeyCode_X, KeyCode_Alt);
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    
    SelectMap(mapid_shared);
    ParentMap(global_map_id);
    BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    BindMouseMove(click_set_cursor_if_lbutton);
    Bind(change_active_panel, KeyCode_P, KeyCode_Alt);
    Bind(interactive_open_or_new, KeyCode_F, KeyCode_Alt);
    Bind(switch_modal, KeyCode_I, KeyCode_Alt);
    Bind(project_fkey_command, KeyCode_F1);
    Bind(project_fkey_command, KeyCode_F2);
    Bind(project_fkey_command, KeyCode_F3);
    Bind(project_fkey_command, KeyCode_F4);
    Bind(project_fkey_command, KeyCode_F5);
    Bind(project_fkey_command, KeyCode_F6);
    Bind(project_fkey_command, KeyCode_F7);
    Bind(project_fkey_command, KeyCode_F8);
    Bind(project_fkey_command, KeyCode_F9);
    Bind(project_fkey_command, KeyCode_F10);
    Bind(project_fkey_command, KeyCode_F11);
    Bind(project_fkey_command, KeyCode_F12);
    Bind(project_fkey_command, KeyCode_F13);
    Bind(project_fkey_command, KeyCode_F14);
    Bind(project_fkey_command, KeyCode_F15);
    Bind(project_fkey_command, KeyCode_F16);
    Bind(query_replace, KeyCode_R, KeyCode_Alt);
    Bind(save, KeyCode_S, KeyCode_Alt);
    Bind(save_all_dirty_buffers, KeyCode_S, KeyCode_Alt, KeyCode_Shift);
    Bind(interactive_switch_buffer, KeyCode_O, KeyCode_Alt);
    Bind(interactive_kill_buffer, KeyCode_K, KeyCode_Alt);
    
    SelectMap(mapid_normal);
    ParentMap(mapid_shared);
    Bind(move_left, KeyCode_J);
    Bind(move_right, KeyCode_L);
    Bind(move_up, KeyCode_I);
    Bind(move_down, KeyCode_K);
    Bind(move_left_alpha_numeric_boundary, KeyCode_J, KeyCode_A);
    Bind(move_right_alpha_numeric_boundary, KeyCode_L, KeyCode_A);
    Bind(move_up_to_blank_line_end, KeyCode_I, KeyCode_A);
    Bind(move_down_to_blank_line_end, KeyCode_K, KeyCode_A);
    Bind(delete_char, KeyCode_D);
    Bind(backspace_char, KeyCode_S);
    Bind(backspace_char, KeyCode_Backspace);
    Bind(undo, KeyCode_U);
    Bind(redo, KeyCode_R);
    Bind(set_mark, KeyCode_Space);
    Bind(copy, KeyCode_C);
    Bind(paste, KeyCode_V);
    Bind(cut, KeyCode_W);
    Bind(reverse_search, KeyCode_N);
    Bind(search, KeyCode_M);
    Bind(seek_beginning_of_line, KeyCode_A, KeyCode_Alt);
    Bind(seek_end_of_line, KeyCode_D, KeyCode_Alt);
    Bind(goto_next_jump, KeyCode_M);
    Bind(goto_prev_jump, KeyCode_N);
    Bind(goto_end_of_file, KeyCode_H);
    Bind(goto_beginning_of_file, KeyCode_Y);
    
    SelectMap(mapid_write);
    ParentMap(mapid_shared);
    BindTextInput(write_text_and_auto_indent); 
    Bind(word_complete, KeyCode_Tab);
    
    SelectMap(file_map_id);
    ParentMap(mapid_normal);
    
    SelectMap(code_map_id);
    ParentMap(mapid_normal);
}

function b32
modal_handle_backspace(Application_Links* app, Lister* lister, Lister_Activation_Code* code)
{
    if(lister->handlers.backspace != 0) {
        lister->handlers.backspace(app);
    }
    else if(lister->handlers.key_stroke != 0) {
        *code = lister->handlers.key_stroke(app);
    }
    else {
        return false;
    }
    
    return true;
}

function b32
modal_handle_navigation(Application_Links* app, Lister* lister, View_ID view, Lister_Activation_Code* code, i32 offset)
{
    if(lister->handlers.navigate != 0) {
        lister->handlers.navigate(app, view, lister, offset);
    }
    else if(lister->handlers.key_stroke != 0) {
        *code = lister->handlers.key_stroke(app);
    }
    else {
        return false;
    }
    
    return true;
}

function b32 modal_has_code_and_modifier(Input_Event* event, Key_Code code, Key_Code modifier)
{
    return (event->key.code == code) && has_modifier(&event->key.modifiers, modifier);
}

function Lister_Result
modal_run_lister(Application_Links *app, Lister *lister)
{
    lister->filter_restore_point = begin_temp(lister->arena);
    lister_update_filtered_list(app, lister);
    
    View_ID view = get_this_ctx_view(app, Access_Always);
    View_Context ctx = view_current_context(app, view);
    ctx.render_caller = lister_render;
    ctx.hides_buffer = true;
    View_Context_Block ctx_block(app, view, &ctx);
    
    for (;;){
        User_Input in = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
        if (in.abort){
            block_zero_struct(&lister->out);
            lister->out.canceled = true;
            break;
        }
        
        Lister_Activation_Code result = ListerActivation_Continue;
        b32 handled = true;
        switch (in.event.kind){
            case InputEventKind_TextInsert:
            {
                if (lister->handlers.write_character != 0){
                    result = lister->handlers.write_character(app);
                }
            }break;
            
            case InputEventKind_KeyStroke:
            {
                switch (in.event.key.code){
                    case KeyCode_Return:
                    case KeyCode_Tab:
                    {
                        void *user_data = 0;
                        if (0 <= lister->raw_item_index &&
                            lister->raw_item_index < lister->options.count){
                            user_data = lister_get_user_data(lister, lister->raw_item_index);
                        }
                        lister_activate(app, lister, user_data, false);
                        result = ListerActivation_Finished;
                    }break;
                    
                    
                    case KeyCode_Backspace:
                    {
                        handled = modal_handle_backspace(app, lister, &result);
                    }break;
                    
                    case KeyCode_Up:
                    {
                        handled = modal_handle_navigation(app, lister, view, &result, -1);
                    }break;
                    
                    case KeyCode_Down:
                    {
                        handled = modal_handle_navigation(app, lister, view, &result, 1);
                    }break;
                    
                    case KeyCode_PageUp:
                    {
                        handled = modal_handle_navigation(app, lister, view, &result, -lister->visible_count);
                    }break;
                    
                    case KeyCode_PageDown:
                    {
                        handled = modal_handle_navigation(app, lister, view, &result, lister->visible_count);
                    }break;
                    
                    default:
                    {
                        if(modal_has_code_and_modifier(&in.event, KeyCode_S, KeyCode_Alt)) {
                            handled = modal_handle_backspace(app, lister, &result);
                        }
                        if(modal_has_code_and_modifier(&in.event, KeyCode_I, KeyCode_Alt)) {
                            handled = modal_handle_navigation(app, lister, view, &result, -1);
                        }
                        else if(modal_has_code_and_modifier(&in.event, KeyCode_K, KeyCode_Alt)) {
                            handled = modal_handle_navigation(app, lister, view, &result, 1);
                        }
                        else
                        {
                            if (lister->handlers.key_stroke != 0){
                                result = lister->handlers.key_stroke(app);
                            }
                            else{
                                handled = false;
                            }
                        }
                    }break;
                }
            }break;
            
            case InputEventKind_MouseButton:
            {
                switch (in.event.mouse.code){
                    case MouseCode_Left:
                    {
                        Vec2_f32 p = V2f32(in.event.mouse.p);
                        void *clicked = lister_user_data_at_p(app, view, lister, p);
                        lister->hot_user_data = clicked;
                    }break;
                    
                    default:
                    {
                        handled = false;
                    }break;
                }
            }break;
            
            case InputEventKind_MouseButtonRelease:
            {
                switch (in.event.mouse.code){
                    case MouseCode_Left:
                    {
                        if (lister->hot_user_data != 0){
                            Vec2_f32 p = V2f32(in.event.mouse.p);
                            void *clicked = lister_user_data_at_p(app, view, lister, p);
                            if (lister->hot_user_data == clicked){
                                lister_activate(app, lister, clicked, true);
                                result = ListerActivation_Finished;
                            }
                        }
                        lister->hot_user_data = 0;
                    }break;
                    
                    default:
                    {
                        handled = false;
                    }break;
                }
            }break;
            
            case InputEventKind_MouseWheel:
            {
                Mouse_State mouse = get_mouse_state(app);
                lister->scroll.target.y += mouse.wheel;
                lister_update_filtered_list(app, lister);
            }break;
            
            case InputEventKind_MouseMove:
            {
                lister_update_filtered_list(app, lister);
            }break;
            
            case InputEventKind_Core:
            {
                switch (in.event.core.code){
                    case CoreCode_Animate:
                    {
                        lister_update_filtered_list(app, lister);
                    }break;
                    
                    default:
                    {
                        handled = false;
                    }break;
                }
            }break;
            
            default:
            {
                handled = false;
            }break;
        }
        
        if (result == ListerActivation_Finished){
            break;
        }
        
        if (!handled){
            Mapping *mapping = lister->mapping;
            Command_Map *map = lister->map;
            
            Fallback_Dispatch_Result disp_result =
                fallback_command_dispatch(app, mapping, map, &in);
            if (disp_result.code == FallbackDispatch_DelayedUICall){
                call_after_ctx_shutdown(app, view, disp_result.func);
                break;
            }
            if (disp_result.code == FallbackDispatch_Unhandled){
                leave_current_input_unhandled(app);
            }
            else{
                lister_call_refresh_handler(app, lister);
            }
        }
    }
    
    return(lister->out);
}