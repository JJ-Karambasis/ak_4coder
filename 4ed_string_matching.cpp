/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 16.06.2019
 *
 * Routines for string matching within chunked streams.
 *
 */

// TOP

internal u64_Array
string_compute_prefix_table(Arena *arena, String_Const_u8 string, Scan_Direction direction){
    u64_Array array = {};
    array.count = (i32)(string.size);
    array.vals = push_array(arena, u64, array.count);
    
    u8 *str = string.str;
    if (direction == Scan_Backward){
        str = string.str + string.size - 1;
    }
    
    array.vals[0] = 0;
    for (u64 i = 1; i < string.size; i += 1){
        u64 previous_longest_prefix = array.vals[i - 1];
        for (;;){
            u8 *a = str + previous_longest_prefix;
            u8 *b = str + i;
            if (direction == Scan_Backward){
                a = str - previous_longest_prefix;
                b = str - i;
            }
            if (character_to_upper(*a) == character_to_upper(*b)){
                array.vals[i] = previous_longest_prefix + 1;
                break;
            }
            if (previous_longest_prefix == 0){
                array.vals[i] = 0;
                break;
            }
            previous_longest_prefix = array.vals[previous_longest_prefix - 1];
        }
    }
    
    return(array);
}

internal u64_Array
string_compute_needle_jump_table(Arena *arena, u64_Array longest_prefixes){
    u64_Array array = {};
    array.count = longest_prefixes.count + 1;
    array.vals = push_array(arena, u64, array.count);
    array.vals[0] = 0;
    for (u64 i = 1; i < array.count; i += 1){
        array.vals[i] = i - longest_prefixes.vals[i - 1];
    }
    return(array);
}

internal u64_Array
string_compute_needle_jump_table(Arena *arena, String_Const_u8 needle, Scan_Direction direction){
    u64_Array prefix_table = string_compute_prefix_table(arena, needle, direction);
    return(string_compute_needle_jump_table(arena, prefix_table));
}

internal void
string_match_list_push(Arena *arena, String_Match_List *list,
                       u64 index, String_Match_Flag flags, Buffer_ID buffer){
    String_Match *match = push_array(arena, String_Match, 1);
    sll_queue_push(list->first, list->last, match);
    list->count += 1;
    match->buffer = buffer;
    match->flags = flags;
    match->index = index;
}

#define character_predicate_check_character(p, c) (((p).b[(c)/8] & (1 << ((c)%8))) != 0)

internal String_Match_List
find_all_matches_forward(Arena *arena, i32 maximum_output_count,
                         String_Const_u8_Array chunks, String_Const_u8 needle,
                         u64_Array jump_table, Character_Predicate *predicate,
                         u64 base_index, Buffer_ID buffer){
    String_Match_List list = {};
    
    if (chunks.count > 0){
        u64 i = 0;
        u64 j = 0;
        b8 current_l = false;
        i64 last_insensitive = -1;
        i64 last_boundary = -1;
        
        i32 chunk_index = 0;
        i64 chunk_pos = 0;
        String_Const_u8 chunk = chunks.strings[chunk_index];
        
        i32 jump_back_code = 0;
        
        if (false){
            iterate_forward:
            i += 1;
            chunk_pos += 1;
            if (chunk_pos >= (i64)chunk.size){
                last_boundary = i;
                chunk_pos = 0;
                chunk_index += 1;
                if (chunk_index < chunks.count){
                    chunk = chunks.strings[chunk_index];
                }
            }
            
            switch (jump_back_code){
                case 0:
                {
                    goto jump_back_0;
                }break;
                case 1:
                {
                    goto jump_back_1;
                }break;
            }
        }
        
        for (;chunk_index < chunks.count;){
            u8 c = chunk.str[chunk_pos];
            u64 n = i - j;
            u8 needle_c = needle.str[n];
            if (character_to_upper(c) == character_to_upper(needle_c)){
                if (c != needle_c){
                    last_insensitive = i;
                }
                
                jump_back_code = 0;
                goto iterate_forward;
                jump_back_0:
                
                if (n + 1 == needle.size){
                    String_Match_Flag flags = {};
                    if (!(last_insensitive >= 0 &&
                          j <= (u64)last_insensitive &&
                          (u64)last_insensitive < j + needle.size)){
                        AddFlag(flags, StringMatch_CaseSensitive);
                    }
                    if (!(last_boundary >= 0 &&
                          j <= (u64)last_boundary &&
                          (u64)last_boundary < j + needle.size)){
                        AddFlag(flags, StringMatch_Straddled);
                    }
                    if (chunk_index < chunks.count){
                        u8 next_c = chunk.str[chunk_pos];
                        if (character_predicate_check_character(*predicate, next_c)){
                            AddFlag(flags, StringMatch_RightSideSloppy);
                        }
                    }
                    if (current_l){
                        AddFlag(flags, StringMatch_LeftSideSloppy);
                    }
                    string_match_list_push(arena, &list, base_index + j, flags, buffer);
                    if (list.count >= maximum_output_count){
                        break;
                    }
                    u64 jump = jump_table.vals[n + 1];
                    current_l = character_predicate_check_character(*predicate, needle.str[jump - 1]);
                    j += jump;
                }
                
            }
            else{
                u64 jump = jump_table.vals[n];
                if (jump == 0){
                    current_l = character_predicate_check_character(*predicate, c);
                    
                    jump_back_code = 1;
                    goto iterate_forward;
                    jump_back_1:
                    
                    j += 1;
                }
                else{
                    u8 prev_c = needle.str[jump - 1];
                    current_l = character_predicate_check_character(*predicate, prev_c);
                    j += jump;
                }
            }
        }
    }
    
    return(list);
}

internal String_Match_List
find_all_matches_backward(Arena *arena, i32 maximum_output_count,
                          String_Const_u8_Array chunks, String_Const_u8 needle,
                          u64_Array jump_table, Character_Predicate *predicate,
                          u64 base_index, Buffer_ID buffer){
    String_Match_List list = {};
    
    if (chunks.count > 0){
        i64 size = 0;
        for (i32 i = 0; i < chunks.count; i += 1){
            size += chunks.strings[i].size;
        }
        
        i64 i = size - 1;
        i64 j = size - 1;
        b8 current_r = false;
        i64 last_insensitive = size;
        i64 last_boundary = size;
        
        i32 chunk_index = chunks.count - 1;
        String_Const_u8 chunk = chunks.strings[chunk_index];
        i64 chunk_pos = chunk.size - 1;
        
        i32 jump_back_code = 0;
        
        if (false){
            iterate_backward:
            i -= 1;
            chunk_pos -= 1;
            if (chunk_pos < 0){
                last_boundary = i;
                chunk_index -= 1;
                if (chunk_index >= 0){
                    chunk = chunks.strings[chunk_index];
                    chunk_pos = chunk.size - 1;
                }
            }
            
            switch (jump_back_code){
                case 0:
                {
                    goto jump_back_0;
                }break;
                case 1:
                {
                    goto jump_back_1;
                }break;
            }
        }
        
        for (;chunk_index >= 0;){
            u8 c = chunk.str[chunk_pos];
            u64 n = j - i;
            u8 needle_c = needle.str[needle.size - 1 - n];
            if (character_to_upper(c) == character_to_upper(needle_c)){
                if (c != needle_c){
                    last_insensitive = i;
                }
                
                jump_back_code = 0;
                goto iterate_backward;
                jump_back_0:
                
                if (n + 1 == needle.size){
                    String_Match_Flag flags = {};
                    if (!(last_insensitive < size &&
                          j >= last_insensitive &&
                          last_insensitive > j - (i64)needle.size)){
                        AddFlag(flags, StringMatch_CaseSensitive);
                    }
                    if (!(last_boundary < size &&
                          j >= last_boundary &&
                          last_boundary > j - (i64)needle.size)){
                        AddFlag(flags, StringMatch_Straddled);
                    }
                    if (chunk_index >= 0){
                        u8 next_c = chunk.str[chunk_pos];
                        if (character_predicate_check_character(*predicate, next_c)){
                            AddFlag(flags, StringMatch_LeftSideSloppy);
                        }
                    }
                    if (current_r){
                        AddFlag(flags, StringMatch_RightSideSloppy);
                    }
                    string_match_list_push(arena, &list,
                                           base_index + (j - (needle.size - 1)), flags, buffer);
                    if (list.count >= maximum_output_count){
                        break;
                    }
                    u64 jump = jump_table.vals[n + 1];
                    u64 m = needle.size - jump;
                    u8 needle_m = needle.str[m];
                    current_r = character_predicate_check_character(*predicate, needle_m);
                    j -= jump;
                }
                
            }
            else{
                u64 jump = jump_table.vals[n];
                if (jump == 0){
                    current_r = character_predicate_check_character(*predicate, c);
                    
                    jump_back_code = 1;
                    goto iterate_backward;
                    jump_back_1:
                    
                    j -= 1;
                }
                else{
                    u64 m = needle.size - jump;
                    u8 needle_m = needle.str[m];
                    current_r = character_predicate_check_character(*predicate, needle_m);
                    j -= jump;
                }
            }
        }
    }
    
    return(list);
}

internal String_Match_List
find_all_matches(Arena *arena, i32 maximum_output_count,
                 String_Const_u8_Array chunks, String_Const_u8 needle,
                 u64_Array jump_table, Character_Predicate *predicate,
                 Scan_Direction direction,
                 u64 base_index, Buffer_ID buffer){
    String_Match_List list = {};
    switch (direction){
        case Scan_Forward:
        {
            list = find_all_matches_forward(arena, maximum_output_count,
                                            chunks, needle, jump_table, predicate,
                                            base_index, buffer);
        }break;
        
        case Scan_Backward:
        {
            list = find_all_matches_backward(arena, maximum_output_count,
                                             chunks, needle, jump_table, predicate,
                                             base_index, buffer);
        }break;
    }
    return(list);
}

// BOTTOM

