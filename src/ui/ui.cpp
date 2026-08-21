#include "ui.hpp"

#include "widgets.hpp"
#include "fonts.hpp"
#include "cfg.hpp"
#include "lang.hpp"
#include "menu_vars.hpp"
#include "watermark.hpp"
#include "config.hpp"

#include "bytes.hpp"
#include "font_awesome.hpp"
#include "notifications.hpp"
#include "log.hpp"
#include <cstdio>
#include <cstdlib>

// Шрифты (montserrat_med из fonts.hpp + font_awesome_binary из bytes.hpp) — уже заинклужены выше

using namespace ImGui;

template <typename T>
inline T _clamp(const T& n, const T& lower, const T& upper) {
	return std::max(lower, std::min(n, upper));
}
inline float lerp(float a, float b, float f) {
	return _clamp<float>(a + f * (b - a), a > b ? b : a, a > b ? a : b);
}

void custom_interface_t::initialize() {
	auto& style = GetStyle();
	auto& col = GetStyle().Colors;

	// акцент из настроек (cfg::style::accent в menu_vars.hpp)
	c_utils->accent = cfg::style::accent;

	style.ScrollbarSize = 6;
	style.ScrollbarRounding = 3;
	style.FrameRounding = 4;
	style.WindowRounding = 6;
	style.WindowBorderSize = 0;
	style.ItemSpacing = { 10, 10 };
	style.WindowMinSize = { 2, 2 };

	col[ImGuiCol_Border] = ImColor(28, 28, 28).Value;
	col[ImGuiCol_WindowBg] = ImColor(16, 16, 16).Value;
	col[ImGuiCol_ChildBg] = { 0, 0, 0, 0 };
	col[ImGuiCol_ScrollbarBg] = ImColor(10, 10, 10).Value;

	// Адаптивный масштаб: на маленьких экранах меню не должно вылезать
	// за границы. Базовый размер меню ~620x370 "единиц", поэтому ограничиваем
	// scale так, чтобы меню помещалось в ширину/высоту с запасом.
	ImGuiIO& io_pre = ImGui::GetIO();
	float sw_pre = io_pre.DisplaySize.x;
	float sh_pre = io_pre.DisplaySize.y;
	float adaptive = 2.f; // базовый (для 1080p+)
	if (sw_pre > 1.f && sh_pre > 1.f) {
		float by_w = sw_pre / 800.f;
		float by_h = sh_pre / 520.f;
		adaptive = (by_w < by_h) ? by_w : by_h;
		if (adaptive > 2.f) adaptive = 2.f;
		if (adaptive < 0.6f) adaptive = 0.6f;
	}
	c_scale->current_scale = adaptive;

	style.ScaleAllSizes(c_scale->current_scale);
}

void custom_interface_t::initialize_fonts() {
	auto& io = GetIO();

	ImFontConfig cfg;
	//cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Monochrome;
	//cfg.RasterizerFlags = ImGuiFreeType::RasterizerFlags::MonoHinting | ImGuiFreeType::RasterizerFlags::Monochrome;

	// Menu font: montserrat_med (English) — has good Latin coverage
	this->fonts.regular = io.Fonts->AddFontFromMemoryTTF(montserrat_med, sizeof montserrat_med, 14.f * c_scale->current_scale, &cfg, io.Fonts->GetGlyphRangesCyrillic());

	// For Russian/Cyrillic: load esp_font from draw.cpp (esp_font.h) — better Cyrillic rendering
	extern ImFont* espFont; // from draw.cpp
	this->fonts.esp_font = espFont;

	// Set default font to montserrat for English, widgets will push esp_font for Cyrillic
	io.FontDefault = this->fonts.regular;

	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.SizePixels = 14.f * c_scale->current_scale;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0x0 };
	ImFontConfig icons_config2;
	icons_config2.SizePixels = 26 * c_scale->current_scale;

	this->fonts.icons = io.Fonts->AddFontFromMemoryTTF(font_awesome_binary, sizeof font_awesome_binary, 9 * c_scale->current_scale, &icons_config, icon_ranges);

	// Bold and big_and_bold — same montserrat_med
	this->fonts.bold = io.Fonts->AddFontFromMemoryTTF(montserrat_med, sizeof montserrat_med, 14.f * c_scale->current_scale, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	this->fonts.big_and_bold = io.Fonts->AddFontFromMemoryTTF(montserrat_med, sizeof montserrat_med, 20 * c_scale->current_scale, &cfg, io.Fonts->GetGlyphRangesCyrillic());

	this->fonts.gondon = io.Fonts->AddFontFromMemoryTTF(font_awesome_binary, sizeof font_awesome_binary, 16 * c_scale->current_scale, &icons_config2, icon_ranges);

	// только то, что реально работает в чите: Visuals / Aimbot / Misc / Configs / Settings / Credits
	this->tabs = {
		{("Visuals"),  nullptr, 0},
		{("Aimbot"),   nullptr, 0},
		{("Misc"),     nullptr, 0},
		{("Configs"),  nullptr, 0},
		{("Settings"), nullptr, 0},
		{("Credits"),  nullptr, 0},
	};

	// сабтабы Visuals: ESP (работает) / Chams и World — заглушки без реализации в func, убираем
	this->subtabs = {
		{("ESP"), 0},
	};

	c_widgets->arrow_elem.font = this->fonts.icons;
	c_widgets->arrow_elem.glyph = ICON_FA_ANGLE_DOWN;
	c_widgets->arrow_elem.glyph_up = ICON_FA_ANGLE_UP;
}

// ─────────────────── только то, что реально работает в чите ───────────────────
namespace menu_tabs {

static void toggle(const char* l, bool* v) { char b[64]; snprintf(b, sizeof(b), "%s##%p", l, (void*)v); c_widgets->toggle(b, v); }
static void slider_f(const char* l, float* v, float mn, float mx, const char* f = "%.1f") { char b[64]; snprintf(b, sizeof(b), "%s##%p", l, (void*)v); c_widgets->slider_float(b, v, mn, mx, f); }
static void slider_i(const char* l, int* v, int mn, int mx) { char b[64]; snprintf(b, sizeof(b), "%s##%p", l, (void*)v); c_widgets->slider_int(b, v, mn, mx, "%d"); }
static void color4(const char* l, ImVec4* c) { char b[64]; snprintf(b, sizeof(b), "%s##%p", l, (void*)c); c_widgets->color_edit_4(b, (float*)c); }

static const char* langs[] = { "русский", "english" };

static void dim_text(const char* t) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
    ImGui::TextWrapped("%s", t);
    ImGui::PopStyleColor();
}

// Visuals → ESP (всё, что рисуется в visuals.cpp: box/name/health/dist/skeleton/line/rgb)
static void visuals_esp() {
    const char* shapes[] = { g_lang->square, g_lang->corners };
    const char* line_pos[] = { g_lang->left, g_lang->right, g_lang->top };
    toggle(g_lang->enable_esp, &opt_esp);
    toggle(g_lang->rgb, &opt_rgb_esp);
    toggle(g_lang->box, &opt_box);
    if (opt_box) {
        ImGui::PushID("boxthick"); slider_f(g_lang->thickness, &s_box_thick, 1, 5); ImGui::PopID();
        c_widgets->combo(g_lang->shape, &s_box_shape, shapes, IM_ARRAYSIZE(shapes));
        ImGui::PushID("boxcol"); color4(g_lang->color, &s_box_col); ImGui::PopID();
    }
    toggle(g_lang->nick, &opt_name);
    if (opt_name) { ImGui::PushID("namecol"); color4(g_lang->color, &s_name_col); ImGui::PopID(); }
    toggle(g_lang->health, &opt_health);
    if (opt_health) { ImGui::PushID("hpcol"); color4(g_lang->color, &s_health_col1); ImGui::PopID(); }
    toggle(g_lang->distance, &opt_dist);
    if (opt_dist) { ImGui::PushID("distcol"); color4(g_lang->color, &s_dist_col); ImGui::PopID(); }
    toggle(g_lang->skeleton, &opt_skeleton);
    if (opt_skeleton) {
        ImGui::PushID("skelthick"); slider_f(g_lang->thickness, &s_skel_thick, 1, 5); ImGui::PopID();
        ImGui::PushID("skelcol"); color4(g_lang->color, &s_skel_color); ImGui::PopID();
    }
    toggle(g_lang->line, &opt_line);
    if (opt_line) {
        slider_f(g_lang->line_thick, &s_line_thick, 1, 10);
        ImGui::PushID("linecol"); color4(g_lang->color, &s_line_col); ImGui::PopID();
        c_widgets->combo(g_lang->position, &s_line_pos, line_pos, IM_ARRAYSIZE(line_pos));
    }
}

// Aimbot (aim.cpp: enabled/fov/bone/smooth/visible_check/lock_line/lock_dot/aspect_ratio)
static void aim_tab() {
    const char* bones[] = { g_lang->head, g_lang->neck, g_lang->chest, g_lang->hips };
    toggle(g_lang->aimbot, &opt_aim);
    if (opt_aim) {
        slider_f(g_lang->aim_fov, &s_aim_fov, 5, 90);
        ImGui::PushID("aimbone"); c_widgets->combo(g_lang->bone, &s_aim_bone, bones, IM_ARRAYSIZE(bones)); ImGui::PopID();
        slider_f(g_lang->aim_smooth, &s_aim_smooth, 0, 1, "%.2f");
        toggle(g_lang->aim_visible, &opt_aim_visible);
        toggle(g_lang->aim_fov_draw, &opt_aim_fov_draw);
        toggle(g_lang->aim_info, &opt_aim_info);
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Silent Aim
    toggle(g_lang->silent_aim, &opt_silent_aim);
    if (opt_silent_aim) {
        slider_f(g_lang->silent_fov, &s_silent_fov, 5, 90);
        ImGui::PushID("silentbone"); c_widgets->combo(g_lang->bone, &s_silent_target_bone, bones, IM_ARRAYSIZE(bones)); ImGui::PopID();
        toggle(g_lang->silent_visible, &opt_silent_visible);
        toggle(g_lang->silent_smoke_check, &opt_silent_smoke_check);
        slider_f(g_lang->silent_recoil_mult, &s_silent_recoil_mult, 0.05f, 10.f, "%.2f");
        toggle(g_lang->silent_fov_draw, &opt_silent_fov_draw);
    }
}

// Misc → Infinity Ammo, Fire Rate, Wallshot, Air Strafe
static void misc_tab() {
    toggle(g_lang->inf_ammo, &opt_inf_ammo);
    if (opt_inf_ammo) {
        slider_i(g_lang->inf_ammo_val, &s_inf_ammo, 1, 999);
    }

    toggle(g_lang->rapid_fire, &opt_rapid_fire);
    if (opt_rapid_fire) {
        slider_f(g_lang->rapid_fire_interval, &s_rapid_interval, 0.01f, 0.5f);
    }

    toggle(g_lang->wallshot, &opt_wallshot);

    toggle(g_lang->air_strafe, &opt_air_strafe);
    if (opt_air_strafe) {
        slider_f(g_lang->air_strafe_speed, &s_air_strafe_speed, 1.f, 50.f);
    }
}

static void settings_tab() {
    color4(g_lang->accent, &cfg::style::accent);
    c_utils->accent = cfg::style::accent;

    if (c_widgets->combo(g_lang->language, &s_lang, langs, IM_ARRAYSIZE(langs)))
        apply_lang(s_lang);

    toggle(g_lang->wm, &opt_wm);
    if (opt_wm) {
        const char* wm_sides[] = { g_lang->left, g_lang->right };
        toggle(g_lang->wm_icons, &opt_wm_icons);
        toggle(g_lang->wm_site, &opt_wm_show_site);
        toggle(g_lang->wm_type, &opt_wm_show_type);
        toggle(g_lang->wm_fps, &opt_wm_show_fps);
        toggle(g_lang->wm_time, &opt_wm_show_time);
        toggle(g_lang->wm_version, &opt_wm_show_version);
        c_widgets->combo(g_lang->position, &s_wm_side, wm_sides, IM_ARRAYSIZE(wm_sides));
        color4(g_lang->wm_color, &s_wm_col);
        slider_f(g_lang->wm_scale, &s_wm_scale, 0.5f, 2.f);
    }

    slider_i(g_lang->scrollbar, &s_scrollbar_size, 2, 20);

    // Кнопка выхода из чита
    if (c_widgets->button(g_lang->exit_cheat)) {
        exit(0);
    }
}

// Configs tab
static void configs_tab() {
    static char new_name[64] = "";
    static int selected_idx = -1;

    auto files = config::get_config_files();

    ImGui::Text("%s", g_lang->cfg_list);
    ImGui::Separator();

    if (files.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "%s", g_lang->cfg_empty);
    } else {
        ImGui::BeginChild("##config_list", ImVec2(0, 200), true);
        for (int i = 0; i < (int)files.size(); i++) {
            bool is_selected = (selected_idx == i);
            if (ImGui::Selectable(files[i].c_str(), is_selected)) {
                selected_idx = i;
            }
        }
        ImGui::EndChild();
    }

    ImGui::Spacing();

    // Buttons
    ImGui::BeginGroup();

    if (c_widgets->button(g_lang->cfg_load)) {
        if (selected_idx >= 0 && selected_idx < (int)files.size()) {
            if (config::load(files[selected_idx])) {
                ImGui::Notification({ImGuiToastType_Success, 3000, "Config loaded"});
            } else {
                ImGui::Notification({ImGuiToastType_Error, 3000, "Failed to load config"});
            }
        } else {
            ImGui::Notification({ImGuiToastType_Warning, 3000, "Select a config first"});
        }
    }

    ImGui::SameLine();

    if (c_widgets->button(g_lang->cfg_save)) {
        int next_num = config::get_next_config_number();
        if (config::save(next_num)) {
            ImGui::Notification({ImGuiToastType_Success, 3000, "Config saved"});
        } else {
            ImGui::Notification({ImGuiToastType_Error, 3000, "Failed to save config"});
        }
    }

    ImGui::SameLine();

    if (c_widgets->button(g_lang->cfg_delete)) {
        if (selected_idx >= 0 && selected_idx < (int)files.size()) {
            if (config::delete_config(files[selected_idx])) {
                ImGui::Notification({ImGuiToastType_Success, 3000, "Config deleted"});
                selected_idx = -1;
            } else {
                ImGui::Notification({ImGuiToastType_Error, 3000, "Failed to delete config"});
            }
        } else {
            ImGui::Notification({ImGuiToastType_Warning, 3000, "Select a config first"});
        }
    }

    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("%s", g_lang->cfg_new_name);
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##new_cfg_name", new_name, sizeof(new_name))) {
        // Input changed
    }
    ImGui::PopItemWidth();

    if (c_widgets->button(g_lang->cfg_save_as)) {
        if (strlen(new_name) > 0) {
            std::string fname = std::string(config::CONFIG_PREFIX) + new_name + config::CONFIG_EXT;
            if (config::save(fname)) {
                ImGui::Notification({ImGuiToastType_Success, 3000, "Config saved as custom name"});
                new_name[0] = '\0';
            } else {
                ImGui::Notification({ImGuiToastType_Error, 3000, "Failed to save config"});
            }
        } else {
            ImGui::Notification({ImGuiToastType_Warning, 3000, "Enter config name"});
        }
    }
}

static void credits_tab() {
    dim_text(g_lang->thanks);
    ImGui::TextWrapped("@DanyaVoredom // github.com/FANATFANATA");
    ImGui::TextWrapped("@panwoc // github.com/player901090-design");
    ImGui::TextWrapped("and @WhyIW // github.com/WhyIW-TG");
}

} // namespace menu_tabs

void custom_interface_t::render(void *_vars) {
    (void)_vars;

	// Язык из сохранённого конфига (s_lang: 0 = русский, 1 = english)
	apply_lang(s_lang);

	// Safety check: ensure fonts are initialized
	if (!fonts.regular) return;

	// Push font based on language: esp_font for Russian (Cyrillic), regular (montserrat) for English
	if (s_lang == 0 && fonts.esp_font) {
	    ImGui::GetIO().FontDefault = fonts.esp_font;
	    ImGui::PushFont(fonts.esp_font);
	} else {
	    ImGui::GetIO().FontDefault = fonts.regular;
	    ImGui::PushFont(fonts.regular);
	}

	// Толщина полосы прокрутки (с учётом глобального масштаба интерфейса)
	GetStyle().ScrollbarSize = s_scrollbar_size * c_scale->current_scale;

	logs::render(ImGui::GetIO().DisplaySize, ImGui::GetBackgroundDrawList(), 25);

	ImGui::RenderNotifications();

	PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

	static ImVec2 main_window_pos{};

	static animation child_bg{};

	child_bg.update(1);

	SetNextWindowSize({ c_scale->get(620), c_scale->get(370) - c_scale->get(45) });

	Begin("##t.me/imguiseller", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus); {
		GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + GetWindowSize(), ImColor(15, 15, 15), GetStyle().WindowRounding, ImDrawFlags_RoundCornersBottom);

		main_window_pos = GetWindowPos();

		auto& col = GetStyle().Colors;
		col[ImGuiCol_ScrollbarGrabActive] = c_utils->get_accent_imv4(GetStyle().Alpha, 0.8f);
		col[ImGuiCol_ScrollbarGrabHovered] = c_utils->get_accent_imv4(GetStyle().Alpha, 0.8f);
		col[ImGuiCol_ScrollbarGrab] = c_utils->get_accent_imv4(GetStyle().Alpha, 0.8f);
		//col[ImGuiCol_ScrollbarBg] = c_utils->get_accent_imv4(GetStyle().Alpha, 0.05f);

		// --- //

		/* tabs --> */ {
			BeginChild("##tabs", { c_scale->get(150), GetWindowSize().y }, false);

			static animation_vec2 background{};

			SetCursorPosY(GetCursorPosY() + c_scale->get(10));

			const ImVec2& size = { GetContentRegionAvail().x, c_scale->get(30) };

			//GetWindowDrawList()->AddShadowRect(GetWindowPos() + background.value + ImVec2(c_scale->get(9), 0), GetWindowPos() + background.value + size - ImVec2(c_scale->get(9), 0), static_cast<ImColor>(c_utils->process_alpha(ImColor(0, 0, 0).Value, GetStyle().Alpha)), c_scale->get(10), { 0, 0 }, 0, c_scale->get(4));
			GetWindowDrawList()->AddRectFilled(GetWindowPos() + background.value + ImVec2(c_scale->get(9), 0), GetWindowPos() + background.value + size - ImVec2(c_scale->get(9), 0), static_cast<ImColor>(c_utils->process_alpha(ImColor(20, 20, 20).Value, GetStyle().Alpha)), GetStyle().WindowRounding);
			//GetWindowDrawList()->AddRect(GetWindowPos() + background.value + ImVec2(c_scale->get(9), 0), GetWindowPos() + background.value + size - ImVec2(c_scale->get(9), 0), static_cast<ImColor>(c_utils->process_alpha(ImColor(24, 24, 24).Value, GetStyle().Alpha)), GetStyle().WindowRounding, 0, c_scale->get(1));

			PushStyleVar(ImGuiStyleVar_ItemSpacing, { c_scale->get(10), 0 });

			for (int it{}; it < this->tabs.size(); it++) {
				auto& tab = this->tabs[it];

				const ImVec2& current_cursor = GetCursorScreenPos();
				const ImVec2& current_cursor_window = GetCursorPos();

				if (InvisibleButton(tab.name, size)) {
					this->current_tab = it;
					this->current_subtab = tab.default_sub;
					child_bg.value = 0;
					//ImGui::Notification({ ImGuiToastType_None, 4000, "Example notification | Buy best menus at t.me/imguiseller" });
				}

				const ImRect btn_rect{
					current_cursor + ImVec2(c_scale->get(9), 0),
					current_cursor + size - ImVec2(c_scale->get(9), 0)
				};

				const auto& draw_list = GetWindowDrawList();

				const auto& icon_size = this->fonts.icons->CalcTextSizeA(c_scale->get(9), FLT_MAX, -1, tab.name);

				const auto& label_size = CalcTextSize(tab.name);

				tab.text.interpolate(ImColor(255, 255, 255).Value, ImColor(150, 150, 150).Value, (it == this->current_tab));

				tab._icon.interpolate(c_utils->get_accent_imv4(GetStyle().Alpha), ImColor(150, 150, 150).Value, (it == this->current_tab));

				const ImVec2 text_pos{
					btn_rect.Min.x + btn_rect.GetHeight() / 2 - label_size.y / 2,
					btn_rect.GetCenter().y - label_size.y / 2,
				};

				if (tab.icon)
					draw_list->AddText(text_pos - ImVec2(0, 1 /* THIS NIGGA IS FUCKER LMAO RETARDED OCORNUT */), static_cast<ImColor>(c_utils->process_alpha(tab._icon.value, GetStyle().Alpha)), tab.icon);

				draw_list->AddText(text_pos + ImVec2(c_scale->get(20), 0) - ImVec2(0, 1 /* THIS NIGGA IS FUCKER LMAO RETARDED OCORNUT */), static_cast<ImColor>(c_utils->process_alpha(tab.text.value, GetStyle().Alpha)), tab.name);

				if (this->current_tab == it)
					background.update(current_cursor_window);

			}

			PopStyleVar();

			EndChild();

			GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + ImVec2(c_scale->get(150), GetWindowSize().y), ImColor(16, 16, 16), GetStyle().WindowRounding, ImDrawFlags_RoundCornersBottomLeft);
			GetWindowDrawList()->AddLine(GetWindowPos() + ImVec2(c_scale->get(150), 0), GetWindowPos() + ImVec2(c_scale->get(150), GetWindowSize().y), ImColor(24, 24, 24), c_scale->get(1));

		}

		// --- //

		/* main --> */ {
			PushStyleVar(ImGuiStyleVar_Alpha, child_bg.value * GetStyle().Alpha);

			SetCursorPos({ c_scale->get(150) + c_scale->get(10), 0 + 1 + (c_scale->get(20) * (1.0f - child_bg.value)) });

			const float& width = GetContentRegionAvail().x;

			if (this->current_tab != 999) {
				BeginChild("##container_main", { width, GetContentRegionAvail().y - c_scale->get(1) }, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				Spacing();
				switch (this->current_tab) {
					case 0:
						if (this->current_subtab == 0) menu_tabs::visuals_esp();
						break;
					case 1: menu_tabs::aim_tab(); break;
					case 2: menu_tabs::misc_tab(); break;
					case 3: menu_tabs::configs_tab(); break;
					case 4: menu_tabs::settings_tab(); break;
					case 5: menu_tabs::credits_tab(); break;
				}
				Spacing();
				EndChild();
			}
			else {
				BeginChild("##container_settings", { width, GetContentRegionAvail().y - c_scale->get(1) }, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				Spacing();
				menu_tabs::settings_tab();
				Spacing();
				EndChild();
			}

			PopStyleVar();
		}

		//GetWindowDrawList()->AddRect(GetWindowPos(), GetWindowPos() + GetWindowSize(), ImColor(24, 24, 24), GetStyle().WindowRounding, ImDrawFlags_RoundCornersBottom, c_scale->get(1));

		End();
	}

	SetNextWindowPos(main_window_pos - ImVec2(0, c_scale->get(70)));
	SetNextWindowSize({ c_scale->get(620), c_scale->get(70) });

	Begin("##heading", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove); {
		GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + GetWindowSize(), ImColor(15, 15, 15), GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop);

		int heading_height = c_scale->get(70);
		const auto& base = GetWindowPos();

		const ImRect h_rect{
			base,
			base + ImVec2(GetWindowSize().x, heading_height)
		};

		auto draw_list = GetWindowDrawList();

		// центр левой 150px-колонки шапки
		const float ccx = h_rect.Min.x + (c_scale->get(150) * 0.5f);

		PushFont(this->fonts.bold);
		const char* logo_text = "nez3r.pw";
		const ImVec2& logo_sz = CalcTextSize(logo_text);
		const ImVec2& beta_sz = CalcTextSize("[beta]");

		// beta над логотипом, оба по центру колонки
		const float beta_y = h_rect.Min.y + c_scale->get(3);
		draw_list->AddText({ ccx - beta_sz.x * 0.5f, beta_y }, ImColor(255, 0, 0), "beta");
		draw_list->AddText({ ccx - logo_sz.x * 0.5f, beta_y + beta_sz.y + c_scale->get(2) },
			c_utils->get_accent_imc(), logo_text);
		PopFont();

		// --- //
		//GetWindowDrawList()->AddLine({ GetWindowPos().x + GetWindowSize().x - GetWindowSize().y, GetWindowPos().y}, {GetWindowPos().x + GetWindowSize().x - GetWindowSize().y, GetWindowPos().y + GetWindowSize().y - 1}, ImColor(28, 28, 28), c_scale->get(1));
		GetWindowDrawList()->AddLine({ GetWindowPos().x + c_scale->get(150), GetWindowPos().y }, { GetWindowPos().x + c_scale->get(150), GetWindowPos().y + GetWindowSize().y - 1 }, ImColor(28, 28, 28), c_scale->get(1));
		GetWindowDrawList()->AddLine({ GetWindowPos().x, GetWindowPos().y + GetWindowSize().y - 1 }, GetWindowPos() + GetWindowSize() - ImVec2(0, 1), ImColor(28, 28, 28), c_scale->get(1));

		SetCursorPos({ c_scale->get(150) + GetStyle().ItemSpacing.x + c_scale->get(16), 10 });

		/* subtabs --> */ {
			for (int it{}; it < subtabs.size(); it++) {
				if (subtabs[it].parent != current_tab) continue;

				PushStyleVar(ImGuiStyleVar_ItemSpacing, { c_scale->get(8), 0 });

				SetCursorPosY(c_scale->get(10));

				const auto& current_cursor = GetCursorScreenPos();
				const auto& btn_sz = ImVec2(CalcTextSize(subtabs[it].name).x + c_scale->get(6), c_scale->get(50));

				if (InvisibleButton(subtabs[it].name, btn_sz)) {
					current_subtab = it;
					child_bg.value = 0;
				}

				const ImRect btn_rect = {
					current_cursor,
					current_cursor + btn_sz
				};

				subtabs[it].text.interpolate(ImColor(255, 255, 255).Value, ImColor(150, 150, 150).Value, (it == this->current_subtab));

				subtabs[it]._icon.interpolate(c_utils->get_accent_imv4(GetStyle().Alpha), ImColor(150, 150, 150).Value, (it == this->current_subtab));

				GetWindowDrawList()->AddText(btn_rect.GetCenter() - CalcTextSize(subtabs[it].name) / 2, ImColor(subtabs[it].text.value), subtabs[it].name);
				GetWindowDrawList()->AddLine({ btn_rect.Min.x, btn_rect.GetCenter().y + CalcTextSize(subtabs[it].name).y / 2 + c_scale->get(2) }, { btn_rect.Max.x, btn_rect.GetCenter().y + CalcTextSize(subtabs[it].name).y / 2 + c_scale->get(2) }, ImColor(subtabs[it]._icon.value), c_scale->get(2));

				SameLine();

				PopStyleVar();
			}

			const ImVec2 btn_sz = { GetWindowSize().y, GetWindowSize().y };
			SetCursorPos({ GetWindowSize().x - btn_sz.x, 0 });
			const auto& current_cursor_gear = GetCursorScreenPos();
			//GOVNOCODER VO MNE $$$
			if (InvisibleButton(ICON_FA_GEAR, btn_sz)) {
				current_tab = 3; //settings tab
				child_bg.value = 0;
			}
			const ImRect btn_rect_gear = {
				current_cursor_gear,
				current_cursor_gear + btn_sz
			};
			const auto& ic_sz = fonts.gondon->CalcTextSizeA(16 * c_scale->current_scale, FLT_MAX, 0, ICON_FA_GEAR);
			GetWindowDrawList()->AddText(fonts.gondon, 16 * c_scale->current_scale, btn_rect_gear.GetCenter() - ic_sz / 2, ImColor(255, 255, 255, 100), ICON_FA_GEAR);
		}

		End();
	}

	// Pop the language-specific font
	ImGui::PopFont();

	// Render watermark
	watermark::render();

	PopStyleVar();
}