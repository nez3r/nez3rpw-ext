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
    const char* color;
    const char* nick;
    const char* health;
    const char* distance;
    const char* skeleton;
    const char* line;
    const char* line_pos;
    const char* line_thick;
    const char* top;
    const char* left;
    const char* right;
    const char* position;
    const char* head;
    const char* neck;
    const char* chest;
    const char* hips;
    const char* middle;
    const char* bottom;
    // Aim
    const char* aimbot;
    const char* aim_fov;
    const char* bone;
    const char* aim_smooth;
    const char* aim_visible;
    const char* aim_fov_draw;
    const char* aim_info;
    // Silent Aim
    const char* silent_aim;
    const char* silent_fov;
    const char* silent_visible;
    const char* silent_smoke_check;
    const char* silent_recoil_mult;
    const char* silent_fov_draw;
    // Weapon
    const char* inf_ammo;
    const char* inf_ammo_val;
    const char* rapid_fire;
    const char* rapid_fire_interval;
    const char* wallshot;
    // Misc
    const char* air_strafe;
    const char* air_strafe_speed;
    // Configs
    const char* cfg_list;
    const char* cfg_empty;
    const char* cfg_load;
    const char* cfg_save;
    const char* cfg_new;
    const char* cfg_delete;
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
    // Credits
    const char* thanks;
};

inline const lang ru = {
    // ESP (25)
    "включить esp", "радуга", "бокс", "толщина", "форма", "квадрат", "углы", "цвет",
    "ник", "здоровье", "дистанция", "скелет", "линия", "позиция линии", "жирность",
    "сверху", "слева", "справа", "позиция", "голова", "шея", "грудь", "таз", "середина", "низ",
    // Aim (7)
    "аимбот", "фов", "кость", "плавность", "чек видимости", "круг фова", "точка у кости",
    // Silent Aim (6)
    "сайлент аим", "фов", "чек видимости", "чек дыма", "млтр рекоила", "круг фова",
    // Weapon (5)
    "бесконечные патроны", "кол-во патронов", "быстрая стрельба", "интервал стрельбы", "стрельба сквозь стены",
    // Misc (2)
    "эйр стрейф", "скорость эйр стрейфа",
    // Configs (6)
    "конфиги", "пусто", "загрузить", "сохранить", "новый", "удалить",
    // Settings (12)
    "акцент", "язык", "водяной знак", "иконки", "сайт", "тип", "фпс", "время",
    "версия", "цвет", "масштаб", "полоса прокрутки", "выход из чита",
    // Credits (1)
    "спасибо за функции и поддержку:",
};

inline const lang en = {
    // ESP (25)
    "enable esp", "rainbow", "box", "thickness", "shape", "square", "corners", "color",
    "nickname", "health", "distance", "skeleton", "line", "line pos", "thickness",
    "top", "left", "right", "position", "head", "neck", "chest", "hips", "middle", "bottom",
    // Aim (7)
    "aimbot", "aim fov", "bone", "smooth", "visible check", "fov circle", "dot in bone",
    // Silent Aim (6)
    "silent aim", "fov", "visible check", "smoke check", "recoil mult", "fov circle",
    // Weapon (5)
    "Infinity Ammo", "ammo value", "Fire Rate", "fire interval", "wallshot",
    // Misc (2)
    "air strafe", "air strafe speed",
    // Configs (6)
    "configs", "empty", "load", "save", "new", "delete",
    // Settings (12)
    "accent", "language", "watermark", "icons", "site", "type", "fps", "time",
    "version", "color", "scale", "scrollbar size", "exit cheat",
    // Credits (1)
    "thanks for features and support:",
};

inline const lang* g_lang = &ru;

// s_lang: 0 = русский, 1 = english
inline void apply_lang(int s_lang) { g_lang = (s_lang == 1) ? &en : &ru; }