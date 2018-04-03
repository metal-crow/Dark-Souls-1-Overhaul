/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce     -   C++
        nyxojaele      -   010 Editor template for TAE files (GitHub: https://github.com/nyxojaele )
        RavagerChris   -   Reverse-engineering Dark Souls Time-Action-Event (TAE) files (GitHub: https://github.com/christopherfavo )


    Asset/Animation/Tae.h

    References:
        https://github.com/christopherfavo/time-action-event-editor/blob/master/DS-TAE%20Editor/DS-TAE%20Editor/TAE.cs
        https://www.youtube.com/watch?v=ZjWAG71t1R8

    nyxojaele's 010 Editor templates:
        https://dl.dropboxusercontent.com/content_link/wd8jBOsCqMIaO2ibj1ICwHjyIZY72AQjkSK7YFLSTXFIlxckp1I1or3w9oYj5zDz/file?dl=1

*/

#pragma once

#ifndef DS1_FILE_LIB_TAE_ANIMATION_FILE_H_
    #define DS1_FILE_LIB_TAE_ANIMATION_FILE_H_


#include "Windows.h"
#include "FileUtil.h"
#include <map>
#include <vector>
#include <fstream>
#include "Archive/Header.h"
#include "SP_AoB_Scan.hpp"



// ??
enum TaeAnimFileStructType {
    TAEAFST_NAMED = 0,
    TAEAFST_UNNAMED = 1
};



// For reading and manipulating TAE animation data files (*.tae)
typedef class TimeActionEventFile {
private:
    // Maps animation IDs to animation data indices
    std::map<uint32_t, int> id_to_index = std::map<uint32_t, int>();
    bool initialized = false;
    // @TODO: Loading from file
    const bool from_file;

    /*
        When the game loads the file, addresses are resolved from the offsets before we access it.
        This means most member data that ends in "offset" is actually an absolute address when loading
        from game memory.

        When we manually load from a file, however, offset_base should be set to the address of header,
        because we haven't calculated the final addresses yet.
    */
    uint32_t offset_base = 0;

public:

    // Main header structure of TAE animation metadata file (.tae)
    struct Header {
        const uint32_t magic32_0 = 0x20454154; // "TAE "
        const uint32_t magic32_1 = 0x00000000;
        const uint32_t magic32_2 = 0x0001000B;
        uint32_t file_size = 0; // Total size of the TAE file (in bytes)
        const uint32_t const32b_0[16] = { // Constant 64 bytes
            0x00000040, 0x00000001, 0x00000050, 0x00000070,
            0x00010002, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x02010001, 0x01010002, 0x00000001, 0x00000000 };
        uint32_t file_id = 0; // Unique ID (NOTE: Not the character ID or animation ID)
        uint32_t anim_id_count = 0; // Length of animation ID array
        uint32_t anim_ids_offset = 0; // Offset (from start of file) of animation ID array
        uint32_t groups_section_offset = 0; // Offset (from start of file) of animation group section (file_start + groups_offset = address of group_count)
        const uint32_t const32b_1 = 0x00000090;
        uint32_t anim_data_count = 0; // Length of animation data array
        uint32_t anim_data_offset = 0; // Offset (from start of file) of animation data array
        const uint32_t const32b_2[10] = { // Constant 40 bytes
            0x00000000, 0x00000001, 0x00000080, 0x00000000, 0x00000000,
            0x000007D0, 0x000007D0, 0x00000050, 0x00000000, 0x00000000 };
        uint32_t subheader_offset = 0; // Offset (from start of file) of subheader (always(?) +4 from here)
    };


    /*
        Sub-header (aka data header) structure; begins immediately after the main header structure.
    */
    struct SubHeader {
        uint32_t skeleton_name_offset = 0; // Offset (from start of file) of HKX skeleton name string
        uint32_t sib_name_offset = 0;      // Offset (from start of file) of SIB name string (Not sure what SIB is)
    };


    // Animation ID structure
    typedef struct AnimationId {
        uint32_t id = 0; // Unique animation ID
        uint32_t offset = 0; // Offset (from start of file) of the AnimData struct corresponding to this animation ID
    } AnimId;


    // Animation group structure
    typedef struct AnimationGroup {
        uint32_t first_id = 0; // ID of first animation in this group
        uint32_t last_id = 0; // ID of last animation in this group (can be equivalent to first_id for groups with only 1 animation)
        uint32_t first_id_offset = 0; // Offset (from start of file) of the AnimId corresponding to first_id
        // (Note: Resolved address of first_id is lower in memory than this AnimGroup structure)
    } AnimGroup;

    // Animation event structure
    typedef struct AnimationEvent {
        struct Header {
            uint32_t start_time_offset = 0;
            uint32_t end_time_offset = 0;
            uint32_t body_offset = 0; // Note: Event body is not contiguous with EventHeader
        };

        struct Body {
            uint32_t type;
            uint32_t params_offset;
        };

        float start_time = 0.0f;
        float end_time = 0.0f;
        Header header;
        Body body;
        std::vector<uint32_t> params;
    } Event;


    // Animation file structure
    typedef struct AnimationFile {
        struct Header {
            uint32_t type = 0;
            uint32_t data_offset = 0;
        };

        Header header;
        uint32_t name_offset;
        wchar_t  *name;
        uint32_t next_file_offset;
        uint32_t linked_anim_id;
    } AnimFile;


    // Animation data structure
    typedef struct AnimationData {
        struct Header {
            uint16_t event_count = 0; // Number of events in this animation
            uint16_t unknown = 0; // Filled in when file is loaded by the game
            uint32_t events_offset = 0; // Offset (from start of file) of the TaeEvent array for this animation
            const uint32_t const32b_0[2] = { 0x00000000, 0x00000000 };
            uint32_t time_constant_count = 0; // Number of time constants in this animation (a time constant is a 32-bit value; events_offset = time_constants_offset + (time_constants_count * 4) )
            uint32_t time_constants_offset = 0; // Offset (from start of file) of the TaeTimeConstant array for this animation
            uint32_t anim_file_offset = 0; // Offset (from start of file) of the AnimFile::Header for this animation
            // Note: Events and File are not contiguous with Header
        };

        Header header;
        std::vector<TimeActionEventFile::Event> events;
        TimeActionEventFile::AnimFile anim_file;
        std::vector<float> time_constants;
    } AnimData;



    Header    *header;
    SubHeader *subheader; // Skeleton/SIB name offsets
    wchar_t   *skeleton_name;
    wchar_t   *sib_name;
    AnimId    *ids; // Animation ID array
    uint32_t  *group_count;
    uint32_t  *groups_offset; // Offset (from start of file) of animation group array (should just be +4 from here)
    AnimGroup *groups;        // Animation group array
    AnimData::Header *data;


    // Constructor (reading from game memory)
    TimeActionEventFile(void *new_header_start) : from_file(false)
    {
        init_from_memory((Header*)new_header_start);
    }

    // Empty constructor (object is not initialized, but from_file must be specified if the user plans to read TAE data from a file)
    TimeActionEventFile(bool will_load_from_file = false) : from_file(will_load_from_file)
    {
    }

    // Destructor
    ~TimeActionEventFile()
    {
    }


    Header* init_from_memory(Header *new_header_start) {
        if (!from_file) {
            if (new_header_start != NULL)
            {
                offset_base = 0;
                id_to_index.clear();
                header = (Header*)new_header_start;
                subheader = (SubHeader*)(offset_base + header->subheader_offset);
                skeleton_name = (wchar_t*)(file_start() + subheader->skeleton_name_offset);
                sib_name = (wchar_t*)(file_start() + subheader->sib_name_offset);
                ids = (AnimId*)(offset_base + header->anim_ids_offset);
                group_count = (uint32_t*)(offset_base + header->groups_section_offset);
                groups_offset = &group_count[1];
                groups = (AnimGroup*)(offset_base + (*groups_offset));
                data = (AnimData::Header*)(file_start() + header->anim_data_offset);

                // Map animation IDs to indices
                for (int i = 0; i < (int)header->anim_id_count; i++) {
                    id_to_index.insert(std::make_pair(ids[i].id, i));
                }
                initialized = true;
                return header;
            }
            else
            {
                header = NULL;
                subheader = NULL;
                skeleton_name = NULL;
                sib_name = NULL;
                ids = NULL;
                group_count = NULL;
                groups_offset = NULL;
                groups = NULL;
                SetLastError(ERROR_CANNOT_MAKE);
                return NULL;
            }
        }
        SetLastError(ERROR_BAD_ENVIRONMENT);
        return NULL;
    }

    Header *init_from_file(char *file) {
        // @TODO: Set offset_base = header after loading file into memory (and before initializing member data)

        initialized = true;
        return NULL;
    }

    /*
        Initializes (or re-initializes) a TAE instance using data from a TAE file in memory that starts with the given unique byte pattern.

        If from_file is true or the byte pattern is not found, the function fails and the Tae object is not modified.

        Returns the address of the header structure on success, or NULL on failure.
    */
    Header* init_from_aob_scan(char *scan_pattern) {
        if (scan_pattern == NULL) {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }

        if (!from_file) {
            Header *new_header_start = (Header*)aob_scan(scan_pattern);
            if (new_header_start) {
                return init_from_memory(new_header_start);
            }
            // Byte pattern was not found
            SetLastError(ERROR_FILE_NOT_FOUND);
            return NULL;
        }
        SetLastError(ERROR_BAD_ENVIRONMENT);
        return NULL;
    }

    // Returns whether this TAE object was loaded from a file (if not, it was already loaded into memory, probably by the game)
    bool loaded_from_file() {
        return from_file;
    }

    bool is_initialized() {
        return initialized;
    }

    // Header data getters
    uint32_t file_id() {
        return header == NULL ? NULL : header->file_id;
    }

    uint32_t file_size() {
        return header == NULL ? NULL : header->file_size;
    }

    uint32_t anim_id_count() {
        return header == NULL ? NULL : header->anim_id_count;
    }

    uint32_t anim_data_count() {
        return header == NULL ? NULL : header->anim_data_count;
    }

    // Get file starting address in memory as a non-pointer integer value
    inline uint32_t file_start() {
        return (uint32_t)header;
    }

    // Returns animation ID at index i
    inline int32_t get_id(int unsigned i) {
        if (i < anim_id_count())
            return ids[i].id;
        else
            return -1;
    }

    // Returns the index of the animation with the specified ID
    inline int get_index(uint32_t id) {
        try {
            return id_to_index.at(id);
        }
        catch (const std::out_of_range&) {
            return -1;
        }
    }

    // Returns pointer to animation ID structure at index i
    inline AnimId* get_id_struct(int unsigned i) {
        if (i < anim_id_count())
            return &ids[i];
        else
            return NULL;
    }

    // Returns pointer to animation ID structure with the specified animation ID
    inline AnimId* get_id_struct_by_id(uint32_t id) {
        try {
            return &ids[id_to_index.at(id)];
        }
        catch (const std::out_of_range&) {
            return NULL;
        }
    }

    // Returns pointer to animation data structure at index i
    inline AnimData::Header* get_data(int unsigned i) {
        if (i < anim_id_count())
            //return (AnimData::Header*)(ids[i].offset + offset_base);
            return &data[i];
        else
            return NULL;
    }

    // Returns pointer to animation data structure with the specified animation ID
    inline AnimData::Header* get_data_by_id(uint32_t id) {
        try {
            return get_data(id_to_index.at(id));
        }
        catch (const std::out_of_range&) {
            return NULL;
        }
    }

    // Returns event count for animation at index i
    inline int get_event_count(int unsigned i) {
        return get_data(i) == NULL ? -1 : get_data(i)->event_count;
    }

    // Returns event count for animation with the specified animation ID
    inline int get_event_count_by_id(uint32_t id) {
        return get_data_by_id(id) == NULL ? -1 : get_data_by_id(id)->event_count;
    }

    // Returns time constant count for animation at index i
    inline int get_time_constant_count(int unsigned i) {
        return get_data(i) == NULL ? -1 : get_data(i)->time_constant_count;
    }

    // Returns time constant value at index tci of the time constant array for animation at index i
    inline float get_time_constant(int unsigned i, int unsigned tci) {
        float* constants = get_data(i) == NULL ? NULL : (float*)(offset_base + get_data(i)->time_constants_offset);
        if (constants != NULL && (int)tci < get_time_constant_count(i)) {
            return constants[tci];
        }
        return -1.0f;
    }

    // Returns time constant value at index tci of the time constant array for animation with the specified animation ID
    inline int get_time_constant_count_by_id(uint32_t id) {
        return get_data_by_id(id) == NULL ? -1 : get_data_by_id(id)->time_constant_count;
    }

    // Returns time constant value at index tci of the time constant array for animation with the specified ID
    inline float get_time_constant_by_id(uint32_t id, int unsigned tci) {
        float* constants = get_data_by_id(id) == NULL ? NULL : (float*)(offset_base + get_data_by_id(id)->time_constants_offset);
        if (constants != NULL && (int)tci < get_time_constant_count_by_id(id)) {
            return constants[tci];
        }
        return -1.0f;
    }

    // Returns pointer to animation file structure for animation at index i
    inline AnimFile::Header* get_anim_file_header(int unsigned i) {
        return get_data(i) == NULL ? NULL : (AnimFile::Header*)(get_data(i)->anim_file_offset + offset_base);
    }

    // Returns pointer to animation file structure for animation with the specified animation ID
    inline AnimFile::Header* get_anim_file_header_by_id(uint32_t id) {
        return get_data_by_id(id) == NULL ? NULL : (AnimFile::Header*)(get_data_by_id(id)->anim_file_offset + offset_base);
    }

    // Returns animation type for animation at index i
    inline int get_anim_file_type(int unsigned i) {
        AnimFile::Header* fsh = get_data(i) == NULL ? NULL : (AnimFile::Header*)(get_data(i)->anim_file_offset + offset_base);
        return fsh == NULL ? -1 : fsh->type;
    }

    // Returns animation type for animation with the specified animation ID
    inline int get_anim_file_type_by_id(uint32_t id) {
        AnimFile::Header* fsh = get_data_by_id(id) == NULL ? NULL : (AnimFile::Header*)(get_data_by_id(id)->anim_file_offset + offset_base);
        return fsh == NULL ? -1 : fsh->type;
    }

    // Returns animation file name for animation at index i
    inline wchar_t* get_anim_file_name(int unsigned i) {
        AnimFile::Header* fsh = get_data(i) == NULL ? NULL : (AnimFile::Header*)(get_data(i)->anim_file_offset + offset_base);
        if (fsh != NULL) {
            uint32_t name_offset = *((uint32_t*)(offset_base + fsh->data_offset));
            return (wchar_t*)(offset_base + name_offset);
        }
        return NULL;
    }

    // Returns animation file name for animation with the specified animation ID
    inline wchar_t* get_anim_file_name_by_id(uint32_t id) {
        AnimFile::Header* fsh = get_data_by_id(id) == NULL ? NULL : (AnimFile::Header*)(get_data_by_id(id)->anim_file_offset + offset_base);
        if (fsh != NULL) {
            uint32_t name_offset = *((uint32_t*)(offset_base + fsh->data_offset));
            return (wchar_t*)(offset_base + name_offset);
        }
        return NULL;
    }

    // Returns animation event header with index ei in event header array for animation at index i
    inline Event::Header* get_event_header(int unsigned i, int unsigned ei) {
        AnimData::Header* d = get_data(i);
        if (d != NULL && ei < d->event_count) {
            return &((Event::Header*)(offset_base + d->events_offset))[ei];
        }
        return NULL;
    }

    // Returns animation event header with index ei in event header array for animation with the specified ID
    inline Event::Header* get_event_header_by_id(uint32_t id, int unsigned ei) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            return &((Event::Header*)(offset_base + d->events_offset))[ei];
        }
        return NULL;
    }

    // Returns animation start time of event with index ei in event header array for animation at index i
    inline float get_event_start(int unsigned i, int unsigned ei) {
        AnimData::Header* d = get_data(i);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                return *(float*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].start_time_offset);
            else
                return *(float*)&(((Event::Header*)(offset_base + d->events_offset))[ei].start_time_offset);
        }
        return -1.0f;
    }

    // Returns animation start time of event with index ei in event header array for animation with the specified ID
    inline float get_event_start_by_id(uint32_t id, int unsigned ei) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                return *(float*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].start_time_offset);
            else
                return *(float*)&(((Event::Header*)(offset_base + d->events_offset))[ei].start_time_offset);
        }
        return -1.0f;
    }

    // Sets animation start time of event with index ei in event header array for animation with the specified ID
    void set_event_start_by_id(uint32_t id, int unsigned ei, float new_start) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                *(float*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].start_time_offset) = new_start;
            else
                *(float*)&(((Event::Header*)(offset_base + d->events_offset))[ei].start_time_offset) = new_start;
        }
    }

    // Sets animation end time of event with index ei in event header array for animation with the specified ID
    void set_event_end_by_id(uint32_t id, int unsigned ei, float new_start) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                *(float*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].end_time_offset) = new_start;
            else
                *(float*)&(((Event::Header*)(offset_base + d->events_offset))[ei].end_time_offset) = new_start;
        }
    }

    // Returns animation end time of event with index ei in event header array for animation at index i
    inline float get_event_end(int unsigned i, int unsigned ei) {
        AnimData::Header* d = get_data(i);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                return *(float*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].end_time_offset);
            else
                return *(float*)&(((Event::Header*)(offset_base + d->events_offset))[ei].end_time_offset);
        }
        return -1.0f;
    }

    // Returns animation end time of event with index ei in event header array for animation with the specified ID
    inline float get_event_end_by_id(uint32_t id, int unsigned ei) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                return *(float*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].end_time_offset);
            else
                return *(float*)&(((Event::Header*)(offset_base + d->events_offset))[ei].end_time_offset);
        }
        return -1.0f;
    }

    // Returns animation type for event with index ei in event header array for animation at index i
    inline int get_event_type(int unsigned i, int unsigned ei) {
        AnimData::Header* d = get_data(i);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                return *(uint32_t*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].body_offset);
            else
                return *(uint32_t*)(((Event::Header*)(offset_base + d->events_offset))[ei].body_offset);
        }
        return -1;
    }

    // Returns animation type for event with index ei in event header array for animation with the specified ID
    inline int get_event_type_by_id(uint32_t id, int unsigned ei) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            if (from_file)
                return *(uint32_t*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].body_offset);
            else
                return *(uint32_t*)(((Event::Header*)(offset_base + d->events_offset))[ei].body_offset);
        }
        return -1;
    }

    // Returns address of parameter array for event with index ei in event header array for animation at index i
    inline uint32_t* get_event_params(int unsigned i, int unsigned ei) {
        AnimData::Header* d = get_data(i);
        if (d != NULL && ei < d->event_count) {
            return (uint32_t*)(offset_base + (((uint32_t*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].body_offset))[1]));
        }
        return NULL;
    }

    // Returns address of parameter array for event with index ei in event header array for animation with the specified ID
    inline uint32_t* get_event_params_by_id(uint32_t id, int unsigned ei) {
        AnimData::Header* d = get_data_by_id(id);
        if (d != NULL && ei < d->event_count) {
            return (uint32_t*)(offset_base + (((uint32_t*)(offset_base + ((Event::Header*)(offset_base + d->events_offset))[ei].body_offset))[1]));
        }
        return NULL;
    }

    // Returns animation parameter count for event with index ei in event header array for animation at index i
    inline int get_event_param_count(int unsigned i, int unsigned ei) {
        return TimeActionEventFile::event_param_count_from_type(get_event_type(i, ei));
    }

    // Returns animation parameter count for event with index ei in event header array for animation with the specified ID
    inline int get_event_param_count_by_id(uint32_t id, int unsigned ei) {
        return TimeActionEventFile::event_param_count_from_type(get_event_type_by_id(id, ei));
    }

    // Returns animation parameter value at index pi for event with index ei in event header array for animation at index i (or INT_MIN on failure)
    inline int32_t get_event_param(int unsigned i, int unsigned ei, int unsigned pi) {
        int count = TimeActionEventFile::event_param_count_from_type(get_event_type(i, ei));
        if (count > -1 && (int)pi < count) {
            return get_event_params(i, ei)[pi];
        }
        return INT_MIN;
    }

    // Returns animation parameter value at index pi for event with index ei in event header array for animation with the specified ID (or INT_MIN on failure)
    inline int32_t get_event_param_by_id(uint32_t id, int unsigned ei, int unsigned pi) {
        int count = TimeActionEventFile::event_param_count_from_type(get_event_type_by_id(id, ei));
        if (count > -1 && (int)pi < count) {
            return get_event_params_by_id(id, ei)[pi];
        }
        return INT_MIN;
    }

    // Returns the number of parameters for the specified event type
    static int event_param_count_from_type(int type) {
        // Shout-out to RavagerChris for doing all the work for these values
        switch (type) {
            case 32:
            case 33:
            case 65:
            case 66:
            case 67:
            case 101:
            case 110:
            case 145:
            case 224:
            case 225:
            case 226:
            case 229:
            case 231:
            case 232:
            case 301:
            case 302:
            case 308:
            case 401:
                return 1;
                break;

            case 5:
            case 64:
            case 112:
            case 121:
            case 128:
            case 193:
            case 233:
            case 304:
                return 2;
                break;

            case 0:
            case 1:
            case 96:
            case 100:
            case 104:
            case 109:
            case 114:
            case 115:
            case 116:
            case 118:
            case 144:
            case 228:
            case 236:
            case 307:
                return 3;
                break;

            case 2:
            case 16:
            case 24:
            case 130:
            case 300:
                return 4;
                break;

            case 120:
                return 6;
                break;

            case 8:
                return 12;
                break;

            default:
                return -1;
        }
    }
} Tae;




#endif // DS1_FILE_LIB_TAE_ANIMATION_FILE_H_
