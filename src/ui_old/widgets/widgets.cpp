#define IMGUI_DEFINE_MATH_OPERATORS
#include "widgets.hpp"
#include "Android_draw/draw.h"
#include <cmath>
#include <algorithm>
#include <functional>

namespace ui {

namespace style {
    static float dt = 0.016f;
    static bool clicked = false;

    static float lrp(float a, float b, float t) { return a + (b - a) * t; }
    static ImVec4 lrp_col(const ImVec4& a, const ImVec4& b, float t) {
        return ImVec4(lrp(a.x, b.x, t), lrp(a.y, b.y, t), lrp(a.z, b.z, t), lrp(a.w, b.w, t));
    }

    void tick() {
        clicked = false;
        static float lt = 0.f;
        float ct = ImGui::GetTime();
        if (lt > 0.f) {
            dt = ct - lt;
            dt = ImClamp(dt, 0.001f, 0.1f);
        }
        lt = ct;
    }

    float anim(const std::string& id, float tgt, float spd) {
        auto it = anims.find(id);
        if (it == anims.end()) {
            anims[id] = tgt;
            return tgt;
        }
        float t = ImClamp(spd * dt, 0.f, 1.f);
        it->second = lrp(it->second, tgt, t);
        if (fabsf(it->second - tgt) < 0.001f) it->second = tgt;
        return it->second;
    }

    ImVec4 anim_col(const std::string& id, const ImVec4& tgt, float spd) {
        auto it = anim_colors.find(id);
        if (it == anim_colors.end()) {
            anim_colors[id] = tgt;
            return tgt;
        }
        float t = ImClamp(spd * dt, 0.f, 1.f);
        it->second = lrp_col(it->second, tgt, t);
        return it->second;
    }

    ImU32 col(const ImVec4& c, float a) {
        return IM_COL32((int)(c.x * 255), (int)(c.y * 255), (int)(c.z * 255), (int)(c.w * 255 * a * content_alpha));
    }

    bool popup() { return popup_open || !active_popup.empty(); }
    void close() { active_popup = ""; popup_open = false; }

    void popups() {
        if (active_popup.empty()) return;
        float pa = anim(active_popup + "_pa", 1.f, 18.f);
        if (pa < 0.01f) { active_popup = ""; return; }
        if (pa > 0.15f && ImGui::IsMouseClicked(0) && !clicked) {
            active_popup = "";
            popup_open = false;
        }
    }
}

namespace widgets {
    using namespace style;

    bool checkbox(const char* name, bool* v, float a) {
        if (a < 0.01f) return false;

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        if (w->SkipItems) return false;

        std::string ids = std::string("cb_") + name;
        ImGuiID id = w->GetID(ids.c_str());

        float ww = content_w > 0 ? content_w : ImGui::GetContentRegionAvail().x;
        float h = 28.f * S;
        float pad = 8.f * S;
        float bsz = 14.f * S;

        ImVec2 pos = w->DC.CursorPos;
        ImRect r(pos, ImVec2(pos.x + ww, pos.y + h));

        ImGui::ItemSize(r);
        if (!ImGui::ItemAdd(r, id)) return false;
        bool hov = false, held = false;
        bool press = ImGui::ButtonBehavior(r, id, &hov, &held, ImGuiButtonFlags_PressedOnClick);
        if (press && !popup()) {
            *v = !*v;
        }


        float ca = anim(ids + "_c", *v ? 1.f : 0.f, 14.f);

        ImDrawList* dl = w->DrawList;

        float bx = r.Min.x + pad;
        float by = r.GetCenter().y - bsz * 0.5f;
        ImVec2 bmin(bx, by);
        ImVec2 bmax(bx + bsz, by + bsz);

        ImVec2 imin(bmin.x + 2, bmin.y + 2);
        ImVec2 imax(bmax.x - 2, bmax.y - 2);
        dl->AddRectFilledMultiColor(imin, imax, col(clr::bg_two, a), col(clr::bg_two, a), col(clr::bg, a), col(clr::bg, a));

        if (ca > 0.01f) {
            dl->AddRectFilledMultiColor(imin, imax, col(clr::accent, a * ca), col(clr::accent, a * ca), col(clr::accent_dark, a * ca), col(clr::accent_dark, a * ca));
        }

        ImVec4 bc = anim_col(ids + "_bc", *v ? clr::accent : clr::border_light, 14.f);
        dl->AddRect(bmin, bmax, col(bc, a));
        dl->AddRect(ImVec2(bmin.x + 1, bmin.y + 1), ImVec2(bmax.x - 1, bmax.y - 1), col(clr::border_dark, a));

        if (ca > 0.01f) {
            dl->AddRectFilledMultiColor(ImVec2(r.Min.x, r.Min.y), ImVec2(r.Min.x + 3, r.Max.y), col(clr::accent, a * ca), col(clr::accent, 0.f), col(clr::accent, 0.f), col(clr::accent, a * ca));
        }

        ImGui::PushFont(fontMedium);
        float tx = bmax.x + pad;
        float ty = r.GetCenter().y - fontMedium->FontSize * 0.5f;
        ImVec4 tc = anim_col(ids + "_tc", *v ? clr::text : clr::text_dim, 12.f);
        dl->AddText(ImVec2(tx, ty), col(tc, a), name);
        ImGui::PopFont();

        return press;
    }

    static std::unordered_map<ImGuiID, bool> sl_active;
    static std::unordered_map<ImGuiID, bool> sl_locked;

    bool slider(const char* name, float* v, float mn, float mx, float a, const char* fmt) {
        if (a < 0.01f) return false;

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        if (w->SkipItems) return false;

        std::string ids = std::string("sl_") + name;
        ImGuiID id = w->GetID(ids.c_str());

        float ww = content_w > 0 ? content_w : ImGui::GetContentRegionAvail().x;
        float h = 40.f * S;
        float pad = 8.f * S;

        ImVec2 pos = w->DC.CursorPos;
        ImRect r(pos, ImVec2(pos.x + ww, pos.y + h));

        float bh = 6.f * S;
        float by = r.Max.y - pad - bh;
        ImRect bar(ImVec2(r.Min.x + pad, by), ImVec2(r.Max.x - pad, by + bh));

        ImGui::ItemSize(r);
        if (!ImGui::ItemAdd(r, id)) return false;

        bool hov = false, held = false;
        ImGui::ButtonBehavior(bar, id, &hov, &held, ImGuiButtonFlags_None);

        if (held && !popup()) {
            if (sl_active.find(id) == sl_active.end()) {
                sl_active[id] = false;
                sl_locked[id] = false;
            }

            ImVec2 drag = ImGui::GetMouseDragDelta(0, 0.f);
            float dx = fabsf(drag.x);
            float dy = fabsf(drag.y);

            if (!sl_active[id] && !sl_locked[id]) {
                if (dy > 8.f) sl_locked[id] = true;
                else if (dx > 5.f) sl_active[id] = true;
            }

            if (sl_active[id]) {
                float mx_pos = ImGui::GetIO().MousePos.x;
                float nn = ImClamp((mx_pos - bar.Min.x) / bar.GetWidth(), 0.f, 1.f);
                *v = mn + nn * (mx - mn);
            }
        } else {
            sl_active.erase(id);
            sl_locked.erase(id);
        }

        float norm = (*v - mn) / (mx - mn);
        float na = anim(ids + "_n", norm, 20.f);
        bool is_held = held && !popup();
        float ha = anim(ids + "_h", is_held ? 1.f : 0.f, is_held ? 20.f : 12.f);

        ImDrawList* dl = w->DrawList;

        dl->AddRectFilled(r.Min, r.Max, col(clr::panel, a));
        dl->AddRect(r.Min, r.Max, col(clr::border, a));

        if (ha > 0.01f) {
            dl->AddRectFilledMultiColor(
                r.Min, ImVec2(r.Min.x + ww * 0.3f, r.Max.y),
                col(clr::accent, a * ha * 0.15f), col(clr::accent, 0.f),
                col(clr::accent, 0.f), col(clr::accent, a * ha * 0.15f)
            );
        }

        ImGui::PushFont(fontMedium);
        float ty = r.Min.y + 6.f * S;
        dl->AddText(ImVec2(r.Min.x + pad, ty), col(clr::text, a), name);

        char vb[32];
        snprintf(vb, sizeof(vb), fmt, *v);
        ImVec2 vs = ImGui::CalcTextSize(vb);
        dl->AddText(ImVec2(r.Max.x - pad - vs.x, ty), col(clr::accent, a), vb);
        ImGui::PopFont();

        dl->AddRectFilled(bar.Min, bar.Max, col(clr::widget, a));
        dl->AddRect(bar.Min, bar.Max, col(clr::border_dark, a));

        float fw = bar.GetWidth() * na;
        if (fw > 0.f) {
            dl->AddRectFilledMultiColor(
                bar.Min, ImVec2(bar.Min.x + fw, bar.Max.y),
                col(clr::accent_light, a), col(clr::accent, a),
                col(clr::accent, a), col(clr::accent_light, a)
            );
        }

        float gx = bar.Min.x + fw;
        float gw = 4.f * S;
        dl->AddRectFilled(
            ImVec2(gx - gw * 0.5f, bar.Min.y - 2.f * S),
            ImVec2(gx + gw * 0.5f, bar.Max.y + 2.f * S),
            col(clr::text_light, a)
        );

        return held && sl_active[id];
    }

    static ImVec2 cmb_pos;
    static float cmb_w;
    static std::vector<const char*> cmb_items;
    static int* cmb_ptr = nullptr;

    bool combo(const char* name, int* v, const std::vector<const char*>& items, float a) {
        if (a < 0.01f) return false;

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        if (w->SkipItems) return false;

        std::string ids = std::string("cmb_") + name;
        ImGuiID id = w->GetID(ids.c_str());

        float ww = content_w > 0 ? content_w : ImGui::GetContentRegionAvail().x;
        float h = 50.f * S;
        float pad = 8.f * S;

        ImVec2 pos = w->DC.CursorPos;
        ImRect r(pos, ImVec2(pos.x + ww, pos.y + h));

        float bh = 22.f * S;
        ImRect btn(ImVec2(r.Min.x + pad, r.Max.y - pad - bh), ImVec2(r.Max.x - pad, r.Max.y - pad));

        ImGui::ItemSize(r);
        if (!ImGui::ItemAdd(r, id)) return false;


        bool hov = false, held = false;
        bool other = !active_popup.empty() && active_popup != ids;
        bool press = ImGui::ButtonBehavior(btn, id, &hov, &held, ImGuiButtonFlags_PressedOnClick) && !other;


        bool open = (active_popup == ids);

        if (press) {
            if (open) { active_popup = ""; popup_open = false; }
            else {
                active_popup = ids;
                popup_open = true;
                cmb_pos = ImVec2(btn.Min.x, btn.Max.y + 4.f * S);
                cmb_w = btn.GetWidth();
                cmb_items = items;
                cmb_ptr = v;
            }
            clicked = true;
        }

        float oa = anim(ids + "_o", open ? 1.f : 0.f, 10.f);
        bool act = open || (hov && ImGui::IsMouseDown(0) && !popup());
        float ha = anim(ids + "_h", act ? 1.f : 0.f, act ? 20.f : 12.f);
        float arrow_rot = anim(ids + "_ar", open ? 1.f : 0.f, 12.f);

        ImDrawList* dl = w->DrawList;

        dl->AddRectFilled(r.Min, r.Max, col(clr::panel, a));
        dl->AddRect(r.Min, r.Max, col(clr::border, a));

        if (ha > 0.01f) {
            dl->AddRectFilledMultiColor(
                r.Min, ImVec2(r.Min.x + ww * 0.3f, r.Max.y),
                col(clr::accent, a * ha * 0.15f), col(clr::accent, 0.f),
                col(clr::accent, 0.f), col(clr::accent, a * ha * 0.15f)
            );
        }

        ImGui::PushFont(fontMedium);
        float ty = r.Min.y + 6.f * S;
        dl->AddText(ImVec2(r.Min.x + pad, ty), col(clr::text, a), name);
        ImGui::PopFont();

        dl->AddRectFilled(btn.Min, btn.Max, col(clr::widget, a));
        dl->AddRect(btn.Min, btn.Max, col(clr::border, a));

        ImGui::PushFont(fontMedium);
        const char* prev = (*v >= 0 && *v < (int)items.size()) ? items[*v] : "";
        float py = btn.GetCenter().y - fontMedium->FontSize * 0.5f;
        dl->AddText(ImVec2(btn.Min.x + 8.f * S, py), col(clr::text, a), prev);
        ImGui::PopFont();

        float ax = btn.Max.x - 10.f * S;
        float ay = btn.GetCenter().y;
        float ar_sz = 4.f * S;
        float ar_off = 2.f * S * (1.f - arrow_rot * 2.f);
        ImVec2 p1(ax - ar_sz, ay - ar_off);
        ImVec2 p2(ax + ar_sz, ay - ar_off);
        ImVec2 p3(ax, ay + ar_off + ar_sz * 0.5f * (1.f - arrow_rot * 2.f));
        if (arrow_rot > 0.5f) {
            p1 = ImVec2(ax - ar_sz, ay + ar_off);
            p2 = ImVec2(ax + ar_sz, ay + ar_off);
            p3 = ImVec2(ax, ay - ar_off - ar_sz * 0.5f * (arrow_rot * 2.f - 1.f));
        }
        dl->AddTriangleFilled(p1, p2, p3, col(clr::accent, a));

        if (oa > 0.01f && cmb_ptr == v) {
            ImDrawList* fg = ImGui::GetForegroundDrawList();

            float ih = 24.f * S;
            float ph = items.size() * ih;
            float visible_h = ph * oa;

            ImVec2 pmin = cmb_pos;
            ImVec2 pmax(pmin.x + cmb_w, pmin.y + ph);

            fg->PushClipRect(pmin, ImVec2(pmax.x, pmin.y + visible_h), true);
            fg->AddRectFilled(pmin, pmax, col(clr::panel, oa));
            fg->AddRect(pmin, pmax, col(clr::border, oa));

            float iy = pmin.y;
            for (int i = 0; i < (int)items.size(); i++) {
                ImRect ir(ImVec2(pmin.x, iy), ImVec2(pmax.x, iy + ih));
                bool ihov = ImGui::IsMouseHoveringRect(ir.Min, ir.Max, false) && oa > 0.7f;
                bool isel = (i == *v);

                std::string item_id = ids + "_i" + std::to_string(i);
                float item_ha = anim(item_id + "_h", ihov ? 1.f : 0.f, 16.f);
                float item_sa = anim(item_id + "_s", isel ? 1.f : 0.f, 12.f);

                if (item_ha > 0.01f) {
                    fg->AddRectFilled(ir.Min, ir.Max, col(clr::widget, oa * item_ha));
                }

                if (item_sa > 0.01f) {
                    fg->AddRectFilledMultiColor(
                        ir.Min, ImVec2(ir.Min.x + 3.f * S, ir.Max.y),
                        col(clr::accent, oa * item_sa), col(clr::accent, 0.f),
                        col(clr::accent, 0.f), col(clr::accent, oa * item_sa)
                    );
                }

                ImGui::PushFont(fontMedium);
                float tty = ir.GetCenter().y - fontMedium->FontSize * 0.5f;
                ImVec4 txt_col = lrp_col(clr::text, clr::accent, item_sa);
                fg->AddText(ImVec2(ir.Min.x + 8.f * S, tty), col(txt_col, oa), items[i]);
                ImGui::PopFont();

                if (ihov && ImGui::IsMouseClicked(0) && !clicked && open && oa > 0.7f) {
                    *v = i;
                    active_popup = "";
                    popup_open = false;
                    clicked = true;
                }

                iy += ih;
            }

            fg->PopClipRect();
        }

        return false;
    }

    static std::string cp_id = "";
    static ImVec4* cp_ptr = nullptr;
    static std::unordered_map<std::string, ImVec4> hsv_cache;
    static std::string picker = "";

    void colorpick(const char* name, ImVec4* v, float a) {
        if (a < 0.01f) return;

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        if (w->SkipItems) return;

        std::string ids = std::string("cp_") + name;
        ImGuiID id = w->GetID(ids.c_str());

        float ww = content_w > 0 ? content_w : ImGui::GetContentRegionAvail().x;
        float h = 28.f * S;
        float pad = 8.f * S;

        ImVec2 pos = w->DC.CursorPos;
        ImRect r(pos, ImVec2(pos.x + ww, pos.y + h));

        ImGui::ItemSize(r);
        if (!ImGui::ItemAdd(r, id)) return;

        bool hov = ImGui::IsMouseHoveringRect(r.Min, r.Max);
        bool other = !active_popup.empty() && active_popup != ids;
        bool press = hov && ImGui::IsMouseClicked(0) && !other && !clicked;

        bool open = (active_popup == ids);

        if (press) {
            if (open) { active_popup = ""; popup_open = false; }
            else {
                active_popup = ids;
                popup_open = true;
                cp_id = ids;
                cp_ptr = v;
                if (hsv_cache.find(ids) == hsv_cache.end()) {
                    float hh, ss, vv;
                    ImGui::ColorConvertRGBtoHSV(v->x, v->y, v->z, hh, ss, vv);
                    hsv_cache[ids] = ImVec4(hh, ss, vv, 1.f);
                }
            }
            clicked = true;
        }

        float oa = anim(ids + "_o", open ? 1.f : 0.f, 18.f);
        bool act = open || (hov && ImGui::IsMouseDown(0) && !popup());
        float ha = anim(ids + "_h", act ? 1.f : 0.f, act ? 20.f : 12.f);

        ImDrawList* dl = w->DrawList;

        dl->AddRectFilled(r.Min, r.Max, col(clr::panel, a));
        dl->AddRect(r.Min, r.Max, col(clr::border, a));

        if (ha > 0.01f) {
            dl->AddRectFilledMultiColor(
                r.Min, ImVec2(r.Min.x + ww * 0.3f, r.Max.y),
                col(clr::accent, a * ha * 0.15f), col(clr::accent, 0.f),
                col(clr::accent, 0.f), col(clr::accent, a * ha * 0.15f)
            );
        }

        ImGui::PushFont(fontMedium);
        float ty = r.GetCenter().y - fontMedium->FontSize * 0.5f;
        dl->AddText(ImVec2(r.Min.x + pad, ty), col(clr::text, a), name);
        ImGui::PopFont();

        float bw = 28.f * S;
        float bh = 12.f * S;
        float bx = r.Max.x - bw - pad;
        float by = r.GetCenter().y - bh * 0.5f;

        dl->AddRectFilled(ImVec2(bx + 2, by + 2), ImVec2(bx + bw - 2, by + bh - 2), IM_COL32((int)(v->x*255), (int)(v->y*255), (int)(v->z*255), (int)(255*a*content_alpha)));
        ImVec4 bc = anim_col(ids + "_bc", open ? clr::accent : clr::border_light, 14.f);
        dl->AddRect(ImVec2(bx, by), ImVec2(bx + bw, by + bh), col(bc, a));
        dl->AddRect(ImVec2(bx + 1, by + 1), ImVec2(bx + bw - 1, by + bh - 1), col(clr::border_dark, a));

        if (oa > 0.01f && cp_ptr == v) {
            ImDrawList* fg = ImGui::GetForegroundDrawList();

            if (hsv_cache.find(ids) == hsv_cache.end()) {
                float hh, ss, vv;
                ImGui::ColorConvertRGBtoHSV(v->x, v->y, v->z, hh, ss, vv);
                hsv_cache[ids] = ImVec4(hh, ss, vv, 1.f);
            }
            ImVec4& hsv = hsv_cache[ids];

            float sv_sz = 90.f * S;
            float hue_w = 16.f * S;
            float p = 8.f * S;
            float gap = 6.f * S;
            float pw = p + sv_sz + gap + hue_w + p;
            float ph = p + sv_sz + p;

            ImVec2 pmin(r.Max.x - pw, r.Max.y + 4.f * S);
            ImVec2 pmax(pmin.x + pw, pmin.y + ph);

            fg->AddRectFilled(pmin, pmax, col(clr::bg, oa));
            fg->AddRect(pmin, pmax, col(clr::border_dark, oa));
            fg->AddRect(ImVec2(pmin.x + 1, pmin.y + 1), ImVec2(pmax.x - 1, pmax.y - 1), col(clr::border, oa));

            fg->AddRectFilledMultiColor(
                ImVec2(pmin.x + 2, pmin.y + 2), ImVec2(pmax.x - 2, pmin.y + 20),
                col(clr::sidebar, oa), col(clr::sidebar, oa),
                col(clr::bg, oa), col(clr::bg, oa)
            );

            if (oa > 0.1f) {
                ImVec2 sv_min(pmin.x + p, pmin.y + p);
                ImVec2 sv_max(sv_min.x + sv_sz, sv_min.y + sv_sz);

                ImVec2 hue_min(sv_max.x + gap, sv_min.y);
                ImVec2 hue_max(hue_min.x + hue_w, sv_max.y);

                ImVec4 hc;
                ImGui::ColorConvertHSVtoRGB(hsv.x, 1.f, 1.f, hc.x, hc.y, hc.z);
                ImU32 hu32 = IM_COL32((int)(hc.x * 255), (int)(hc.y * 255), (int)(hc.z * 255), (int)(255 * oa));

                fg->AddRectFilled(sv_min, sv_max, col(clr::widget, oa));
                fg->AddRectFilledMultiColor(sv_min, sv_max, IM_COL32(255, 255, 255, (int)(255 * oa)), hu32, hu32, IM_COL32(255, 255, 255, (int)(255 * oa)));
                fg->AddRectFilledMultiColor(sv_min, sv_max, IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, (int)(255 * oa)), IM_COL32(0, 0, 0, (int)(255 * oa)));
                fg->AddRect(sv_min, sv_max, col(clr::border_dark, oa));

                fg->AddRectFilled(hue_min, hue_max, col(clr::widget, oa));
                float seg = (hue_max.y - hue_min.y) / 6.f;
                for (int i = 0; i < 6; i++) {
                    ImVec4 c1, c2;
                    ImGui::ColorConvertHSVtoRGB(i / 6.f, 1.f, 1.f, c1.x, c1.y, c1.z);
                    ImGui::ColorConvertHSVtoRGB((i + 1) / 6.f, 1.f, 1.f, c2.x, c2.y, c2.z);
                    fg->AddRectFilledMultiColor(
                        ImVec2(hue_min.x + 2, hue_min.y + seg * i),
                        ImVec2(hue_max.x - 2, hue_min.y + seg * (i + 1)),
                        IM_COL32((int)(c1.x * 255), (int)(c1.y * 255), (int)(c1.z * 255), (int)(255 * oa)),
                        IM_COL32((int)(c1.x * 255), (int)(c1.y * 255), (int)(c1.z * 255), (int)(255 * oa)),
                        IM_COL32((int)(c2.x * 255), (int)(c2.y * 255), (int)(c2.z * 255), (int)(255 * oa)),
                        IM_COL32((int)(c2.x * 255), (int)(c2.y * 255), (int)(c2.z * 255), (int)(255 * oa))
                    );
                }
                fg->AddRect(hue_min, hue_max, col(clr::border_dark, oa));

                ImGuiIO& io = ImGui::GetIO();
                ImVec2 mp = io.MousePos;

                if (io.MouseDown[0]) {
                    if (picker.empty()) {
                        if (mp.x >= sv_min.x && mp.x <= sv_max.x && mp.y >= sv_min.y && mp.y <= sv_max.y) picker = ids + "_sv";
                        else if (mp.x >= hue_min.x && mp.x <= hue_max.x && mp.y >= hue_min.y && mp.y <= hue_max.y) picker = ids + "_hue";
                    }

                    if (picker == ids + "_sv") {
                        hsv.y = ImClamp((mp.x - sv_min.x) / (sv_max.x - sv_min.x), 0.f, 1.f);
                        hsv.z = 1.f - ImClamp((mp.y - sv_min.y) / (sv_max.y - sv_min.y), 0.f, 1.f);
                        ImGui::ColorConvertHSVtoRGB(hsv.x, hsv.y, hsv.z, v->x, v->y, v->z);
                        clicked = true;
                    } else if (picker == ids + "_hue") {
                        hsv.x = ImClamp((mp.y - hue_min.y) / (hue_max.y - hue_min.y), 0.f, 1.f);
                        ImGui::ColorConvertHSVtoRGB(hsv.x, hsv.y, hsv.z, v->x, v->y, v->z);
                        clicked = true;
                    }
                } else {
                    picker = "";
                }

                float cx = sv_min.x + hsv.y * (sv_max.x - sv_min.x);
                float cy = sv_min.y + (1.f - hsv.z) * (sv_max.y - sv_min.y);
                fg->AddRect(ImVec2(cx - 5.f * S, cy - 5.f * S), ImVec2(cx + 5.f * S, cy + 5.f * S), IM_COL32(0, 0, 0, (int)(200 * oa)), 0, 0, 2.f);
                fg->AddRect(ImVec2(cx - 4.f * S, cy - 4.f * S), ImVec2(cx + 4.f * S, cy + 4.f * S), IM_COL32(255, 255, 255, (int)(255 * oa)), 0, 0, 1.5f);

                float hy = hue_min.y + hsv.x * (hue_max.y - hue_min.y);
                fg->AddTriangleFilled(ImVec2(hue_min.x - 4.f * S, hy - 4.f * S), ImVec2(hue_min.x - 4.f * S, hy + 4.f * S), ImVec2(hue_min.x + 2.f * S, hy), IM_COL32(255, 255, 255, (int)(255 * oa)));
                fg->AddTriangleFilled(ImVec2(hue_max.x + 4.f * S, hy - 4.f * S), ImVec2(hue_max.x + 4.f * S, hy + 4.f * S), ImVec2(hue_max.x - 2.f * S, hy), IM_COL32(255, 255, 255, (int)(255 * oa)));
            }
        }
    }

    void separator(float a) {
        if (a < 0.01f) return;

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImVec2 pos = w->DC.CursorPos;
        float ww = content_w > 0 ? content_w : ImGui::GetContentRegionAvail().x;

        ImDrawList* dl = w->DrawList;
        float y = pos.y + 4.f * S;

        dl->AddRectFilledMultiColor(
            ImVec2(pos.x, y), ImVec2(pos.x + ww * 0.4f, y + 1),
            col(clr::border, a * 0.6f), col(clr::border, 0.f),
            col(clr::border, 0.f), col(clr::border, a * 0.6f)
        );
        dl->AddRectFilledMultiColor(
            ImVec2(pos.x + ww * 0.6f, y), ImVec2(pos.x + ww, y + 1),
            col(clr::border, 0.f), col(clr::border, a * 0.6f),
            col(clr::border, a * 0.6f), col(clr::border, 0.f)
        );

        ImGui::Dummy(ImVec2(0, 8.f * S));
    }

    bool button(const char* name, float a, std::function<void()> on_click) {
        if (a < 0.01f) return false;

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        if (w->SkipItems) return false;

        std::string ids = std::string("btn_") + name;
        ImGuiID id = w->GetID(ids.c_str());

        float ww = content_w > 0 ? content_w : ImGui::GetContentRegionAvail().x;
        float h = 36.f * S;

        ImVec2 pos = w->DC.CursorPos;
        ImRect r(pos, ImVec2(pos.x + ww, pos.y + h));

        ImGui::ItemSize(r);
        if (!ImGui::ItemAdd(r, id)) return false;

        bool hov = ImGui::IsMouseHoveringRect(r.Min, r.Max) && !popup();
        bool press = hov && ImGui::IsMouseClicked(0) && !clicked;
        if (press) clicked = true;

        float ha = anim(ids + "_h", hov ? 1.f : 0.f, 14.f);

        ImDrawList* dl = w->DrawList;

        ImVec4 bg = lrp_col(clr::widget, clr::panel, ha);
        dl->AddRectFilled(r.Min, r.Max, col(bg, a));

        if (ha > 0.01f) {
            dl->AddRectFilledMultiColor(
                r.Min, ImVec2(r.Min.x + ww * 0.3f, r.Max.y),
                col(clr::accent, a * ha * 0.2f), col(clr::accent, 0.f),
                col(clr::accent, 0.f), col(clr::accent, a * ha * 0.2f)
            );
        }

        ImVec4 bc = anim_col(ids + "_bc", hov ? clr::accent : clr::border, 14.f);
        dl->AddRect(r.Min, r.Max, col(bc, a));
        dl->AddRect(ImVec2(r.Min.x + 1, r.Min.y + 1), ImVec2(r.Max.x - 1, r.Max.y - 1), col(clr::border_dark, a));

        ImGui::PushFont(fontMedium);
        ImVec2 ts = ImGui::CalcTextSize(name);
        float tx = r.Min.x + (ww - ts.x) * 0.5f;
        float ty = r.GetCenter().y - ts.y * 0.5f;
        ImVec4 tc = anim_col(ids + "_tc", hov ? clr::accent_light : clr::text, 12.f);
        dl->AddText(ImVec2(tx, ty), col(tc, a), name);
        ImGui::PopFont();

        if (press && on_click) {
            on_click();
        }

        return press;
    }
}

}

