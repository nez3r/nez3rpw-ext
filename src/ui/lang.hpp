#pragma once

// Локализация меню, как было в старом ui_old/lang. Новый menu_tabs (ui.cpp)
// читает g_lang->key. Названия вкладок/подвкладок (Visuals/ESP/...) оставлены
// английскими как в исходнике; переводятся метки опций внутри вкладок.

struct lang {
    // ESP
    const char* enable_esp;
    const char* rgb;
    const char* box;
    const char* thickness;
    const char* shape;
    const char* square;
    const char* corners;
    const char* filled;
    const char* color;
    const char* gradient;
    const char* color2;
    const char* nick;
    const char* health;
    const char* size;
    const char* position;
    const char* text;
    const char* distance;
    const char* skeleton;
    const char* weapon;
    const char* line;
    const char* line_pos;
    const char* line_thick;
    const char* dist_render;
    const char* style;
    const char* full;
    const char* minimal;
    const char* top;
    const char* left;
    const char* right;
    // Chams
    const char* chams;
    const char* mattype;      // "тип"
    const char* solid;
    const char* flat;
    const char* self;
    const char* hands;
    const char* shader;
    const char* weapon_chams;
    // World
    const char* fog;
    const char* fog_start;
    const char* fog_end;
    const char* sky;
    const char* night_sky;
    const char* model;
    const char* model_dist;
    const char* model_height;
    const char* model_rot;
    const char* world_color;
    const char* world_hint;
    const char* world_solid;
    const char* rainbow;
    const char* rainbow_speed;
    // Rage
    const char* rage_soon;
    // Aim
    const char* aimbot;
    const char* aim_fov;
    const char* bone;
    const char* aim_smooth;
    const char* aim_visible;
    const char* aim_fov_draw;
    const char* aim_360;
    const char* aim_info;
    // Silent Aim
    const char* silent_aim;
    const char* silent_fov;
    const char* silent_visible;
    const char* silent_smoke_check;
    const char* silent_recoil_mult;
    const char* silent_fov_draw;
    const char* triggerbot;
    const char* trigger_vis;
    const char* trigger_radius;
    const char* trigger_delay;
    const char* head;
    const char* neck;
    const char* chest;
    const char* hips;
    // Weapon
    const char* ammo;
    const char* fire_interval;
    const char* set_hp;
    const char* hp;
    const char* set_kills;
    const char* kills;
    const char* set_score;
    const char* score;
    const char* no_recoil;
    const char* recoil;
    // Misc
    const char* inf_ammo;
    const char* inf_ammo_val;
    const char* rapid_fire;
    const char* rapid_fire_interval;
    const char* wallshot;
    const char* air_strafe;
    const char* air_strafe_speed;
    const char* middle;
    const char* bottom;
    // AntiAim
    const char* antiaim;
    const char* aa_pitch;
    const char* aa_yaw;
    const char* aa_jitter;
    const char* aa_jitter_speed;
    const char* aa_range;
    const char* level;
    const char* up;
    const char* down;
    const char* forward;
    const char* backward;
    const char* spin;
    const char* chaos;
    // Settings
    const char* accent;
    const char* language;
    const char* wm;
    const char* wm_icons;
    const char* wm_site;
    const char* wm_type;
    const char* wm_fps;
    const char* wm_time;
    const char* wm_version;
    const char* wm_color;
    const char* wm_scale;
    const char* scrollbar;
    const char* exit_cheat;
    // Configs
    const char* cfg_save;
    const char* cfg_load;
    const char* cfg_new;
    const char* cfg_delete;
    // Anticheat
    const char* bypass;
    const char* ac_kill;
    const char* ac_audit;
    // Credits
    const char* thanks;
};

inline const lang ru = {
    // ESP
    "включить esp", "радуга", "бокс", "толщина", "форма", "квадрат", "углы", "залитый", "цвет", "градиент", "цвет 2",
    "ник", "здоровье", "размер", "позиция", "текст", "дистанция", "скелет", "оружие", "линия", "позиция линии", "жирность",
    "дистанция прорисовки", "стиль", "полный", "минимал", "сверху", "слева", "справа",
    // Chams
    "чамсы", "тип", "сплошной", "однотонный", "по себе", "руки", "шейдер", "оружие",
    // World
    "туман", "старт", "финиш", "небо", "ночное небо",
    "кастом модель тунг тунг тунг сахур", "дистанция", "высота", "поворот",
    "перекраска мира", "работает не на\nвсю карту", "сплошной", "радуга", "скорость радуги",
    // Rage
    "soon...",
    // Aim
    "аимбот", "фов", "кость", "плавность", "чек видимости", "круг фова", "360", "точка у кости",
    // Silent Aim
    "сайлент аим", "фов", "чек видимости", "чек дыма", "млтр рекоила", "круг фова",
    "триггербот", "только видимые", "радиус (пкс)", "задержка (мс)", "голова", "шея", "грудь", "таз",
    // Weapon
    "патроны", "интервал",
    "выдать hp", "hp", "выдать kills", "kills", "выдать score", "score", "анти-отдача", "отдача",
    // Misc
    "бесконечные патроны", "кол-во патронов", "быстрая стрельба", "интервал стрельбы", "стрельба сквозь стены", "эйр стрейф", "скорость эйр стрейфа", "середина", "низ",
    // AntiAim
    "антиаим", "питч", "яв", "джиттер", "скорость джиттера", "размах",
    "ровно", "вверх", "вниз", "вперёд", "назад", "вращение", "хаос",
    // Settings
    "акцент", "язык", "водяной знак", "иконки", "сайт", "тип", "фпс", "время",
    "версия", "цвет", "масштаб", "полоса прокрутки", "выход из чита",
    // Configs
    "сохранить конфиг", "загрузить конфиг", "создать конфиг", "удалить конфиг",
    // Anticheat
    "байпасс античита", "нейтрализация античита", "аудит античита",
    // Credits
    "спасибо за функции и поддержку:",
};

inline const lang en = {
    // ESP
    "enable esp", "rainbow", "box", "thickness", "shape", "square", "corners", "filled", "color", "gradient", "color 2",
    "nickname", "health", "size", "position", "text", "distance", "skeleton", "weapon", "line", "line pos", "thickness",
    "render distance", "style", "full", "minimal", "top", "left", "right",
    // Chams
    "chams", "type", "solid", "flat", "on yourself", "hands", "shader", "weapon",
    // World
    "fog", "start", "end", "sky", "night sky",
    "tung tung tung sahur model", "distance", "height", "rotation",
    "world color", "may not color\nthe whole map", "solid", "rainbow", "rainbow speed",
    // Rage
    "soon...",
    // Aim
    "aimbot", "aim fov", "bone", "smooth", "visible check", "fov circle", "360", "dot in bone",
    // Silent Aim
    "silent aim", "fov", "visible check", "smoke check", "recoil mult", "fov circle",
    "triggerbot", "visible only", "radius (px)", "delay (ms)", "head", "neck", "chest", "hips",
    // Weapon
    "ammo", "fire interval",
    "set hp", "hp", "set kills", "kills", "set score", "score", "no recoil", "recoil",
    // Misc
    "Infinity Ammo", "ammo value", "Fire Rate", "fire interval", "wallshot", "air strafe", "air strafe speed", "middle", "bottom",
    // AntiAim
    "antiaim", "pitch", "yaw", "jitter", "jitter speed", "range",
    "level", "up", "down", "forward", "back", "spin", "chaos",
    // Settings
    "accent", "language", "watermark", "icons", "site", "type", "fps", "time",
    "version", "color", "scale", "scrollbar size", "exit cheat",
    // Configs
    "save config", "load config", "create config", "delete config",
    // Anticheat
    "anticheat bypass", "kill anticheat", "audit anticheat",
    // Credits
    "thanks for features and support:",
};

inline const lang* g_lang = &ru;

// s_lang: 0 = русский, 1 = english
inline void apply_lang(int s_lang) { g_lang = (s_lang == 1) ? &en : &ru; }