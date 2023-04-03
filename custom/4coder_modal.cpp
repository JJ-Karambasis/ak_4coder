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
    
    SelectMap(mapid_shared);
    ParentMap(global_map_id);
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
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
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
    Bind(query_replace, KeyCode_R, KeyCode_Alt);
    
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
    Bind(backspace_char, KeyCode_Backspace);
    Bind(save, KeyCode_S, KeyCode_Alt);
    Bind(save_all_dirty_buffers, KeyCode_S, KeyCode_Alt, KeyCode_Shift);
    Bind(undo, KeyCode_U);
    Bind(redo, KeyCode_R);
    Bind(set_mark, KeyCode_Space);
    Bind(copy, KeyCode_C);
    Bind(paste, KeyCode_V);
    Bind(cut, KeyCode_W);
    Bind(reverse_search, KeyCode_N);
    Bind(search, KeyCode_M);
    
    SelectMap(mapid_write);
    ParentMap(mapid_shared);
    BindTextInput(write_text_and_auto_indent); 
    
    SelectMap(file_map_id);
    ParentMap(mapid_normal);
    
    SelectMap(code_map_id);
    ParentMap(mapid_normal);
}

