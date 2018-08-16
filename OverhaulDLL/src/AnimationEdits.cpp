#include "AnimationEdits.h"
#include "DllMain.h"
#include "GameData.h"
#include <unordered_map>

// Injection addresses
const static uint8_t* anim_id_write_injection_point = (uint8_t*)0xE2B2CF;
const static uint8_t* dodge_left_injection_point    = (uint8_t*)0x00E1833D;
const static uint8_t* dodge_right_injection_point   = (uint8_t*)0x00E18363;
const static uint8_t* dodge_back_injection_point    = (uint8_t*)0x00E18389;
const static uint8_t* dodge_forward_injection_point = (uint8_t*)0x00E18371;
const static uint8_t* sprint_end_injection_point    = (uint8_t*)0x00E1802D;

// Return addresses
uint32_t returnhere_write_anim_id = 0;
uint32_t returnhere_dodge_forward_intercept = 0;
uint32_t returnhere_dummy = 0;
uint32_t returnhere_sprint_end = 0;

uint32_t locked_dodge_jmp_address = 0x00E1806F;

// Original bytes (for unpatching)
const static uint8_t anim_id_write_original_bytes[6]      = { 0x89, 0x4E, 0x60, 0x8B, 0x50, 0x04 };
const static uint8_t dodge_left_jmp_original_bytes[5]     = { 0xE9, 0x2D, 0xFD, 0xFF, 0xFF };
const static uint8_t dodge_right_jmp_original_bytes[5]    = { 0xE9, 0x07, 0xFD, 0xFF, 0xFF };
const static uint8_t dodge_back_jmp_original_bytes[5]     = { 0xE9, 0xE1, 0xFC, 0xFF, 0xFF };
const static uint8_t dodge_forward_jmp_original_bytes[10] = { 0x66, 0x0F, 0x2F, 0xC1, 0x0F, 0x86, 0xF4, 0xFC, 0xFF, 0xFF };
const static uint8_t sprint_end_original_bytes[6]         = { 0xF3, 0x0F, 0x5A, 0x44, 0x24, 0x18 };

// Animation IDs for the default set of gesture animations in the game
const uint32_t AnimationEdits::gesture_anim_ids[15] = { 6800, 6801, 6802, 6803, 6804, 6805, 6806, 6807, 6808, 6809, 6810, 6815, 6820, 6825, 6830 };

// Omni-directional dodging currently enabled/disabled
uint32_t AnimationEdits::omni_directional_dodge = 1;

// Gesture cancelling enabled/disabled
bool AnimationEdits::gesture_cancelling = true;

// Indicates whether next whiff animation should be played or skipped
uint32_t AnimationEdits::skip_next_whiff_anim = 0;

uint32_t skip_next_lockon_reset = 0;


// Intercepts the function that writes current animation ID
void __declspec(naked) __stdcall intercept_anim_id_write()
{
    __asm
    {
        // Check if ending animation
        cmp ecx, 0xFFFFFFFF
        je reset_lockon_mode

        // Check if whiff animation should be skipped
        cmp [AnimationEdits::skip_next_whiff_anim], 0
        je check_dodge_anim

        // Check if next animation is whiff animation
        cmp ecx, 0x1D56 // 7510 = "Can't use item"/"whiff" animation ID
        jne check_dodge_anim
        jmp do_skip_whiff_anim
        
        check_dodge_anim:
        //// Check if omni-directional dodging is enabled
        //cmp [AnimationEdits::omni_directional_dodge], 0
        //je originalcode_write_anim_id

        //// Check if invaders are present
        //cmp [Game::invaders_present_in_world], 0
        //jne originalcode_write_anim_id

        //// Omni-directional dodging is enabled; check for dodge animation
        //cmp ecx, 0x2BC // 700
        //jl originalcode_write_anim_id
        //cmp ecx, 0x2E2 // 738
        //jg originalcode_write_anim_id

        //// Fast roll
        //cmp ecx, 0x2BF // 703
        //jg check_mid_roll
        //mov ecx, 0x2BC // 700 = Fast roll forward animation ID
        //mov ebx, 0x2BC
        //mov dword ptr [esp+8], 0x2BC
        //mov dword ptr [esp+0x14], 0x2BC
        //mov dword ptr [esp+0x3C], 0x2BC
        //mov dword ptr [esp+0xAC], 0x2BC
        //jmp originalcode_write_anim_id

        //// Mid roll
        //check_mid_roll:
        //cmp ecx, 0x2C9 // 713
        //jg check_slow_roll
        //mov ecx, 0x2C6 // 710 = Mid roll forward animation ID
        //mov ebx, 0x2C6
        //mov dword ptr [esp+8], 0x2C6
        //mov dword ptr [esp+0x14], 0x2C6
        //mov dword ptr [esp+0x3C], 0x2C6
        //mov dword ptr [esp+0xAC], 0x2C6
        //jmp originalcode_write_anim_id

        //// Fat/slow roll
        //check_slow_roll:
        //cmp ecx, 0x2D3 // 723
        //jg check_overburdened_roll
        //mov ecx, 0x2D0 // 720 = Slow roll forward animation ID
        //mov ebx, 0x2D0
        //mov dword ptr [esp+8], 0x2D0
        //mov dword ptr [esp+0x14], 0x2D0
        //mov dword ptr [esp+0x3C], 0x2D0
        //mov dword ptr [esp+0xAC], 0x2D0
        //jmp originalcode_write_anim_id

        //// Overburdened roll
        //check_overburdened_roll:
        //cmp ecx, 0x2DD // 733
        //jg check_dark_wood_grain_roll
        //mov ecx, 0x2DA // 730 = Stumble forward animation ID
        //mov ebx, 0x2DA
        //mov dword ptr [esp+8], 0x2DA
        //mov dword ptr [esp+0x14], 0x2DA
        //mov dword ptr [esp+0x3C], 0x2DA
        //mov dword ptr [esp+0xAC], 0x2DA
        //jmp originalcode_write_anim_id

        //// Dark Wood Grain Ring roll
        //check_dark_wood_grain_roll:
        //mov ecx, 0x2DF // 735 = Front handspring animation ID
        //mov ebx, 0x2DF
        //mov dword ptr [esp+8], 0x2DF
        //mov dword ptr [esp+0x14], 0x2DF
        //mov dword ptr [esp+0x3C], 0x2DF
        //mov dword ptr [esp+0xAC], 0x2DF
        jmp originalcode_write_anim_id

        // Skip this whiff animation
        do_skip_whiff_anim:
        mov [AnimationEdits::skip_next_whiff_anim], 0
        jmp exit_write_anim_id

        reset_lockon_mode:
        //cmp [skip_next_lockon_reset], 1
        //je end_reset_lockon_mode
        
        push eax
        //// Reset lockon mode
        //mov eax, 0x137D6C8
        //mov eax, [eax]
        //mov byte ptr [eax+0x1150], 1
        //// Lock character rotation
        //mov eax, 0x012E29E8
        //mov eax, [eax]
        //mov eax, [eax]
        //mov byte ptr [eax+0x128], 1

        // Restore je instructions
        mov eax, 0xE67B3B
        mov byte ptr [eax], 0x74

        mov eax, 0xDBC862
        mov byte ptr [eax], 0x74

        pop eax

        //end_reset_lockon_mode:
        //mov dword ptr [skip_next_lockon_reset], 0

        originalcode_write_anim_id:
        mov [esi+0x60], ecx
        exit_write_anim_id:
        mov edx, [eax+4]
        jmp returnhere_write_anim_id
    }
}


const static uint8_t* roll_branch_injection_point = (uint8_t*)0xE1830C;
uint32_t returnhere_roll_branch = 0;
uint32_t jmp_roll_branch = 0xE1838E;
const static uint8_t roll_branch_original_bytes[9] = { 0x80, 0xB8, 0x28, 0x01, 0x00, 0x00, 0x00, 0x74, 0x79 };
void __declspec(naked) __stdcall intercept_roll_branch()
{
    __asm
    {
        // Check if omni-directional dodging is enabled
        cmp [AnimationEdits::omni_directional_dodge], 0
        je originalcode_roll_branch

        // Check if invaders are present
        cmp [Game::invaders_present_in_world], 0
        jne originalcode_roll_branch

        // Change je to jmp
        push eax
        mov eax, 0xE67B3B
        mov byte ptr [eax], 0xEB
        mov eax, 0xDBC862
        mov byte ptr [eax], 0xEB
        pop eax

        force_jmp:
        jmp jmp_roll_branch

        originalcode_roll_branch:
        cmp byte ptr [eax+0x128], 0
        je force_jmp

        jmp returnhere_roll_branch
    }
}


// Intercept for dodging left/right/backwards when locked on
//void __declspec(naked) __stdcall intercept_locked_dodge()
//{
//    __asm
//    {
//        // Check if omni-directional dodging is enabled
//        cmp [AnimationEdits::omni_directional_dodge], 0
//        je exit_locked_dodge_intercept
//
//        // Check if invaders are present
//        cmp [Game::invaders_present_in_world], 0
//        jne exit_locked_dodge_intercept
//
//        
//        push eax
//        //// Change lockon mode
//        //mov eax, 0x137D6C8
//        //mov eax, [eax]
//        //mov byte ptr [eax+0x1150], 0
//        mov dword ptr [skip_next_lockon_reset], 1
//        //// Unlock character rotation
//        //mov eax, 0x012E29E8
//        //mov eax, [eax]
//        //mov eax, [eax]
//        //mov byte ptr [eax+0x128], 0
//
//        mov eax, 0xE67B3B
//        mov byte ptr [eax], 0x75
//
//        pop eax
//
//        exit_locked_dodge_intercept:
//        jmp locked_dodge_jmp_address
//    }
//}


// Intercept for dodging forward when locked on
//void __declspec(naked) __stdcall intercept_locked_dodge_forward()
//{
//    __asm
//    {
//        comisd xmm0, xmm1
//        jbe check_omni_dodge_forward
//        jmp exit_locked_dodge_foward_intercept
//        check_omni_dodge_forward:
//
//        // Check if omni-directional dodging is enabled
//        cmp [AnimationEdits::omni_directional_dodge], 0
//        je do_original_jbe
//
//        // Check if invaders are present
//        cmp [Game::invaders_present_in_world], 0
//        jne do_original_jbe
//
//        
//        push eax
//        //// Change lockon mode
//        //mov eax, 0x137D6C8
//        //mov eax, [eax]
//        //mov byte ptr [eax+0x1150], 0
//        ////mov dword ptr [skip_next_lockon_reset], 1
//        //// Unlock character rotation
//        //mov eax, 0x012E29E8
//        //mov eax, [eax]
//        //mov eax, [eax]
//        //mov byte ptr [eax+0x128], 0
//
//        mov eax, 0xE67B3B
//        mov byte ptr [eax], 0x75
//
//        pop eax
//
//        do_original_jbe:
//        jmp locked_dodge_jmp_address
//
//        exit_locked_dodge_foward_intercept:
//        jmp returnhere_dodge_forward_intercept
//    }
//}


// Intercept for ending "sprint forward" animation
void __declspec(naked) __stdcall intercept_sprint_end()
{
    __asm
    {
        cvtss2sd xmm0, [esp+0x18]
        
        push eax
        //// Reset lockon mode
        //mov eax, 0x137D6C8
        //mov eax, [eax]
        //mov byte ptr [eax+0x1150], 1
        //// Lock character rotation
        //mov eax, 0x012E29E8
        //mov eax, [eax]
        //mov eax, [eax]
        //mov byte ptr [eax+0x128], 1

        mov eax, 0xE67B3B
        mov byte ptr [eax], 0x74

        mov eax, 0xDBC862
        mov byte ptr [eax], 0x74

        pop eax

        jmp returnhere_sprint_end
    }
}


void AnimationEdits::apply_anim_id_write_intercept()
{
    set_mem_protection(const_cast<uint8_t*>(anim_id_write_injection_point), 6, MEM_PROTECT_RWX);
    inject_jmp_5b(const_cast<uint8_t*>(anim_id_write_injection_point), &returnhere_write_anim_id, 1, &intercept_anim_id_write);

    //set_mem_protection(const_cast<uint8_t*>(dodge_left_injection_point), 5, MEM_PROTECT_RWX);
    //inject_jmp_5b(const_cast<uint8_t*>(dodge_left_injection_point), &returnhere_dummy, 0, &intercept_locked_dodge);

    //set_mem_protection(const_cast<uint8_t*>(dodge_right_injection_point), 5, MEM_PROTECT_RWX);
    //inject_jmp_5b(const_cast<uint8_t*>(dodge_right_injection_point), &returnhere_dummy, 0, &intercept_locked_dodge);

    //set_mem_protection(const_cast<uint8_t*>(dodge_back_injection_point), 5, MEM_PROTECT_RWX);
    //inject_jmp_5b(const_cast<uint8_t*>(dodge_back_injection_point), &returnhere_dummy, 0, &intercept_locked_dodge);

    //set_mem_protection(const_cast<uint8_t*>(dodge_forward_injection_point), 10, MEM_PROTECT_RWX);
    //inject_jmp_5b(const_cast<uint8_t*>(dodge_forward_injection_point), &returnhere_dodge_forward_intercept, 5, &intercept_locked_dodge_forward);

    set_mem_protection(const_cast<uint8_t*>(sprint_end_injection_point), 6, MEM_PROTECT_RWX);
    inject_jmp_5b(const_cast<uint8_t*>(sprint_end_injection_point), &returnhere_sprint_end, 1, &intercept_sprint_end);

    set_mem_protection(reinterpret_cast<uint8_t*>(0xE67B3B), 2, MEM_PROTECT_RWX);

    set_mem_protection(reinterpret_cast<uint8_t*>(0xDBC862), 2, MEM_PROTECT_RWX);

    set_mem_protection(const_cast<uint8_t*>(roll_branch_injection_point), 9, MEM_PROTECT_RWX);
    inject_jmp_5b(const_cast<uint8_t*>(roll_branch_injection_point), &returnhere_roll_branch, 4, &intercept_roll_branch);
}


void AnimationEdits::remove_anim_id_write_intercept()
{
    forget_injection((uint32_t)anim_id_write_injection_point);
    apply_byte_patch(const_cast<uint8_t*>(anim_id_write_injection_point), anim_id_write_original_bytes, 6);

    //forget_injection((uint32_t)dodge_left_injection_point);
    //apply_byte_patch(const_cast<uint8_t*>(dodge_left_injection_point), dodge_left_jmp_original_bytes, 5);

    //forget_injection((uint32_t)dodge_right_injection_point);
    //apply_byte_patch(const_cast<uint8_t*>(dodge_right_injection_point), dodge_right_jmp_original_bytes, 5);

    //forget_injection((uint32_t)dodge_back_injection_point);
    //apply_byte_patch(const_cast<uint8_t*>(dodge_back_injection_point), dodge_back_jmp_original_bytes, 5);

    //forget_injection((uint32_t)dodge_forward_injection_point);
    //apply_byte_patch(const_cast<uint8_t*>(dodge_forward_injection_point), dodge_forward_jmp_original_bytes, 10);

    forget_injection((uint32_t)sprint_end_injection_point);
    apply_byte_patch(const_cast<uint8_t*>(sprint_end_injection_point), sprint_end_original_bytes, 6);

    *(uint8_t*)0xE67B3B = 0x74;

    *(uint8_t*)0xDBC862 = 0x74;

    forget_injection((uint32_t)roll_branch_injection_point);
    apply_byte_patch(const_cast<uint8_t*>(roll_branch_injection_point), roll_branch_original_bytes, 9);
}


// Enables gesture cancelling via rolling
bool AnimationEdits::enable_gesture_cancelling()
{
    int gestures_changed = 0;
    if (Game::characters_loaded && Game::player_tae.is_initialized())
    {
        for (uint32_t id : gesture_anim_ids) {
            int n_events = Game::player_tae.get_event_count_by_id(id);
            for (int i = 0; i < n_events; i++) {
                bool anim_updated = false;
                if (Game::player_tae.get_event_type_by_id(id, i) == 0 &&
                    (
                        Game::player_tae.get_event_param_by_id(id, i, 0) == TAE_type0_param_values::allow_animation_cancel_events ||
                        Game::player_tae.get_event_param_by_id(id, i, 0) == TAE_type0_param_values::cancel_by_rolling_or_backstepping
                        )
                    )
                {
                    Game::player_tae.set_event_start_by_id(id, i, 0.0f);
                    //if (!print_console("Updated gesture " + std::to_string(id) + ", event " + std::to_string(i) + " to allow cancelling"))
                    //    Mod::startup_messages.push_back("Updated gesture " + std::to_string(id) + ", event " + std::to_string(i) + " to allow cancelling");
                    if (!anim_updated) {
                        anim_updated = true;
                        gestures_changed++;
                    }
                }
            }
        }
        return (gestures_changed >= 15);
    }
    else {
        return false;
    }
}


// key - animation id
// value - new speed ratio (Any large number ~>10 does a frame 1 skip), and point to start speed ajustment
static const std::unordered_map<int32_t, std::tuple<float, float>> ANIMATIONS_TO_AJUST_SPEED_RATIO = {
    { 6209, {10.0f, 0.0f}}, { 6309, {10.0f, 0.0f}},  //Firestorm startup
    { 6409, {1.6f,  0.0f}}, { 6509, {1.6f,  0.0f}},  //Firestorm main animation
    { 6215, {10.0f, 0.0f}}, { 6315, {10.0f, 0.0f}},  //Gravelord Sword Dance startup
    { 6415, {1.6f,  0.0f}}, { 6515, {1.6f,  0.0f}},  //Gravelord Sword Dance animation
    { 6203, {6.0f,  0.0f}}, { 6303, {6.0f,  0.0f}},  //Heal knealing startup
    { 6403, {1.0f,  0.0f}}, { 6503, {1.0f,  0.0f}},  //Heal knealing animation
    { 6220, {6.0f,  0.0f}}, { 6320, {6.0f,  0.0f}},  //Sunlight Heal knealing startup
    { 6420, {1.0f,  0.0f}}, { 6520, {1.0f,  0.0f}},  //Sunlight Heal knealing animation
    { 6218, {10.0f, 0.0f}}, { 6318, {10.0f, 0.0f}},  //lightning spear starting animation
    { 6418, {1.2f,  0.0f}}, { 6518, {1.2f,  0.0f}},  //lightning spear throwing animation
    { 9000, {1.25f, 3.0f}}, { 9420, {1.25f, 3.0f}},   //getting backstabbed (total times 5.9 and 5.766667)
};


static uint32_t animation_entry_set_return;

void AnimationEdits::alter_animation_speeds()
{
    if (!print_console("    Enabling animation speed alteration injection...")) {
        Mod::startup_messages.push_back("    Enabling animation speed alteration injection...");
    }

    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_entry_set_offset + ((uint32_t)Game::ds1_base));
    set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
    inject_jmp_5b(write_address, &animation_entry_set_return, 0, &animation_entry_set_injection);
}

//Thread data = speed ratio, start time, speed ajustment ptr
static DWORD WINAPI DelayAnimationSpeedAjustment(void* thread_data) {
    uint32_t start = Game::get_game_time_ms();
    uint32_t cur = start;

    //Wait till animation reaches desired point
    float adjust_time = ((float*)thread_data)[1];

    while (cur < start+adjust_time*1000) {
        cur = Game::get_game_time_ms();
        Sleep(1);
    }

    //set speed
    float* speed_ptr = ((float**)thread_data)[2];
    *speed_ptr = ((float*)thread_data)[0];

    free(thread_data);
    return 0;
}

static void __stdcall read_body_aid_injection_helper_function(int32_t* animation_id, float* speed) {
    //Since we set animation speed at the table entry level, when it gets unset the speed is automatically reset. No cleanup needed

    //If this is an animation to be changed, ajust speed while we're in it
    auto ajust_aid = ANIMATIONS_TO_AJUST_SPEED_RATIO.find(*animation_id);
    if (ajust_aid != ANIMATIONS_TO_AJUST_SPEED_RATIO.end()) {
        //if this is a instant speed ajustment
        if (std::get<1>(ajust_aid->second) == 0) {
            *speed = std::get<0>(ajust_aid->second);
            return;
        }
        //if delayed speed ajustment
        else {
            //need the args to be on the heap
            void* thread_data = malloc(sizeof(float)*2+sizeof(float*));
            ((float*)thread_data)[0] = std::get<0>(ajust_aid->second);
            ((float*)thread_data)[1] = std::get<1>(ajust_aid->second);
            ((float**)thread_data)[2] = speed;

            CreateThread(NULL, 0, DelayAnimationSpeedAjustment, thread_data, 0, NULL);
            return;
        }
    }

    //handle backstabing detection (b/c it's a ton of diff animations)
    if (*animation_id > 200000) {
        if (*animation_id % 1000 == 400 || *animation_id % 1000 == 401) {
            void* thread_data = malloc(sizeof(float)*2 + sizeof(float*));
            ((float*)thread_data)[0] = 1.25f;
            ((float*)thread_data)[1] = 3.0f;
            ((float**)thread_data)[2] = speed;

            CreateThread(NULL, 0, DelayAnimationSpeedAjustment, thread_data, 0, NULL);
            return;
        }
    }
}

void __declspec(naked) __stdcall AnimationEdits::animation_entry_set_injection() {
    //"DARKSOULS.exe" + 9929B6:
    //movss[ecx + 40], xmm1
    //Alt: db F3 0F 11 49 40
    _asm {
        //original code
        movss [ecx + 0x40], xmm1

        push eax
        push ecx
        push edx

        add ecx, 0x40
        push ecx //Animation entry speed ptr
        push edi //Animation aid ptr
        call read_body_aid_injection_helper_function

        pop edx
        pop ecx
        pop eax
        //restore SSM registers
        movss xmm1, DWORD PTR ds : 0x115F59C
        xorps xmm0, xmm0
        jmp animation_entry_set_return
    }
}


void AnimationEdits::disable_whiff_animations() {
    if (!print_console("    Enabling remove animation whiffs...")) {
        Mod::startup_messages.push_back("    Enabling remove animation whiffs...");
    }

    //make jump over setting whiff unconditional
    uint8_t *write_address = (uint8_t*)(AnimationEdits::animation_whiff_set_offset + ((uint32_t)Game::ds1_base));
    uint8_t jmp_patch[2] = { 0xEB, 0x35 };
    apply_byte_patch(write_address, jmp_patch, 2);
}
