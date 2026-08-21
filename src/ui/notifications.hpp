#define IMGUI_NOTIFY

#pragma once
#include <vector>
#include <string>
#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

using namespace ImGui;

#include <thread>
#include <chrono>
#include <random>
using namespace std;
using namespace std::chrono;

static int dismiss = 0;

namespace font
{
	extern ImFont* inter_bold;
	extern ImFont* icon;
}

#define NOTIFY_MAX_MSG_LENGTH			4096		
#define NOTIFY_PADDING_X				15.f		
#define NOTIFY_PADDING_Y				15.f		
#define NOTIFY_PADDING_MESSAGE_Y		15.f		
#define NOTIFY_FADE_IN_OUT_TIME			500
#define NOTIFY_DEFAULT_DISMISS			3000		
#define NOTIFY_OPACITY					1.0f		
#define NOTIFY_TOAST_FLAGS				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing

#define NOTIFY_INLINE					inline
#define NOTIFY_NULL_OR_EMPTY(str)		(!str ||! strlen(str))
#define NOTIFY_FORMAT(fn, format, ...)	if (format) { va_list args; va_start(args, format); fn(format, args, __VA_ARGS__); va_end(args); }

typedef int ImGuiToastType;
typedef int ImGuiToastPhase;
typedef int ImGuiToastPos;

enum ImGuiToastType_
{
	ImGuiToastType_None,
	ImGuiToastType_Success,
	ImGuiToastType_Warning,
	ImGuiToastType_Error,
	ImGuiToastType_Info,
	ImGuiToastType_Config,
	ImGuiToastType_COUNT
};

enum ImGuiToastPhase_
{
	ImGuiToastPhase_FadeIn,
	ImGuiToastPhase_Wait,
	ImGuiToastPhase_FadeOut,
	ImGuiToastPhase_Expired,
	ImGuiToastPhase_COUNT
};

enum ImGuiToastPos_
{
	ImGuiToastPos_TopLeft,
	ImGuiToastPos_TopCenter,
	ImGuiToastPos_TopRight,
	ImGuiToastPos_BottomLeft,
	ImGuiToastPos_BottomCenter,
	ImGuiToastPos_BottomRight,
	ImGuiToastPos_Center,
	ImGuiToastPos_COUNT
};

class ImGuiToast
{
private:
	ImGuiToastType	type = ImGuiToastType_None;
	char			title[NOTIFY_MAX_MSG_LENGTH];
	char			content[NOTIFY_MAX_MSG_LENGTH];
	int				dismiss_time = NOTIFY_DEFAULT_DISMISS;
	uint64_t		creation_time = 0;

private:
	// Setters

	NOTIFY_INLINE auto set_title(const char* format, va_list args) { vsnprintf(this->title, sizeof(this->title), format, args); }

	NOTIFY_INLINE auto set_content(const char* format, va_list args) { vsnprintf(this->content, sizeof(this->content), format, args); }

public:

	NOTIFY_INLINE auto set_type(const ImGuiToastType& type) -> void { IM_ASSERT(type < ImGuiToastType_COUNT); this->type = type; };

public:
	// Getters

	NOTIFY_INLINE auto get_title() -> const char* { return this->title; };

	NOTIFY_INLINE auto get_default_title() -> char*
	{
		if (!strlen(this->title))
		{
			switch (this->type)
			{
			case ImGuiToastType_None:
				return NULL;
			case ImGuiToastType_Success:
				return "Success";
			case ImGuiToastType_Warning:
				return "Warning";
			case ImGuiToastType_Error:
				return "Error";
			case ImGuiToastType_Info:
				return "Info";
			case ImGuiToastType_Config:
				return "Config";
			}
		}

		return this->title;
	};

	NOTIFY_INLINE auto get_type() -> const ImGuiToastType& { return this->type; };

	NOTIFY_INLINE auto get_color() -> ImVec4
	{
		switch (this->type)
		{
		case ImGuiToastType_None:
			return { 255, 255, 255, 255 };
		case ImGuiToastType_Success:
			return { 0, 255, 0, 255 };
		case ImGuiToastType_Warning:
			return { 255, 255, 0, 255 };
		case ImGuiToastType_Error:
			return { 255, 0, 0, 255 };
		case ImGuiToastType_Info:
			return { 0, 157, 255, 255 };
		case ImGuiToastType_Config:
			return { 100, 0, 100, 255 };
		}
	}

	NOTIFY_INLINE static auto get_tick_count() -> const unsigned long long
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
	}

	NOTIFY_INLINE auto get_content() -> char* { return this->content; };

	NOTIFY_INLINE auto get_elapsed_time() { return get_tick_count() - this->creation_time; }

	NOTIFY_INLINE auto get_phase() -> ImGuiToastPhase
	{
		const auto elapsed = get_elapsed_time();

		if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME)
		{
			return ImGuiToastPhase_Expired;
		}
		else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time)
		{
			return ImGuiToastPhase_FadeOut;
		}
		else if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
		{
			return ImGuiToastPhase_Wait;
		}
		else
		{
			return ImGuiToastPhase_FadeIn;
		}
	}

	NOTIFY_INLINE auto get_fade_percent() -> const float
	{
		const auto phase = get_phase();
		const auto elapsed = get_elapsed_time();

		if (phase == ImGuiToastPhase_FadeIn)
		{
			return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
		}
		else if (phase == ImGuiToastPhase_FadeOut)
		{
			return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
		}

		return 1.f * NOTIFY_OPACITY;
	}

public:

	ImGuiToast(ImGuiToastType type, int dismiss_time = NOTIFY_DEFAULT_DISMISS)
	{
		IM_ASSERT(type < ImGuiToastType_COUNT);

		this->type = type;
		this->dismiss_time = dismiss_time;
		this->creation_time = get_tick_count();

		memset(this->title, 0, sizeof(this->title));
		memset(this->content, 0, sizeof(this->content));

		dismiss = dismiss_time;

	}

	ImGuiToast(ImGuiToastType type, int dismiss_time, const char* format, ...) : ImGuiToast(type, dismiss_time)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(this->content, sizeof(this->content), format, args);
		va_end(args);
	}

	ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(this->content, sizeof(this->content), format, args);
		va_end(args);
	}
};

namespace ImGui
{
	NOTIFY_INLINE std::vector<ImGuiToast> notifications;

	NOTIFY_INLINE void Notification(const ImGuiToast& toast)
	{
		notifications.push_back(toast);
	}

	NOTIFY_INLINE void RemoveNotification(int index)
	{
		notifications.erase(notifications.begin() + index);
	}

	NOTIFY_INLINE void RenderNotifications()
	{
		const auto vp_size = GetMainViewport()->Size;

		float height = 0.f;

		for (auto i = 0; i < notifications.size(); i++)
		{
			auto* current_toast = &notifications[i];

			if (current_toast->get_phase() == ImGuiToastPhase_Expired)
			{
				RemoveNotification(i);
				continue;
			}

			const char* title = current_toast->get_title();
			const auto content = current_toast->get_content();
			const auto default_title = current_toast->get_default_title();
			const auto opacity = current_toast->get_fade_percent();
			const auto delay = current_toast->get_elapsed_time();

			auto text_color = current_toast->get_color();
			text_color.w = opacity;

			char window_name[50];
			sprintf(window_name, "##TOAST%d", i);

			PushStyleVar(ImGuiStyleVar_Alpha, opacity);

			SetNextWindowPos(ImVec2(vp_size.x - NOTIFY_PADDING_X, vp_size.y - NOTIFY_PADDING_Y - height), ImGuiCond_Always, ImVec2(1.f, 1.0f));

			PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 10));
			PushStyleVar(ImGuiStyleVar_WindowRounding, 4);
			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
			PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);

			PushStyleColor(ImGuiCol_WindowBg, ImColor(15, 15, 15).Value);
			PushStyleColor(ImGuiCol_Border, ImColor(24, 24, 24).Value);

			Begin(window_name, NULL, NOTIFY_TOAST_FLAGS);
			{
				const ImVec2 pos = ImGui::GetWindowPos(), spacing = GetStyle().ItemSpacing, region = ImGui::GetContentRegionMax();

				GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 20, pos.y + region.y - 16), ImVec2(pos.x + region.x, pos.y + region.y - 10), ImColor(24, 24, 24), 30.f);

				//GetWindowDrawList()->AddShadowRect(ImVec2(pos.x + 20, pos.y + region.y - 16), ImVec2(pos.x + opacity * region.x, pos.y + region.y - 10), GetColorU32(text_color), 10.f, ImVec2(0, 0), 30.f);
				GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 20, pos.y + region.y - 16), ImVec2(pos.x + opacity * region.x, pos.y + region.y - 10), c_utils->get_accent_imc(), 30.f);

				SetCursorPosY(GetCursorPosY() + 10);

				PushTextWrapPos(vp_size.x / 3.f);

				TextColored(c_utils->get_accent_imv4(), "Notification");

				if (!NOTIFY_NULL_OR_EMPTY(content))
				{
					Text("%s", content);
				}

				PopTextWrapPos();
			}

			height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

			SetCursorPosY(GetCursorPosY() + 21);

			End();
			PopStyleVar(5);
			PopStyleColor(2);
		}
	}
}

