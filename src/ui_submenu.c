#include "ui_submenu.h"
#include "ui_common.h"
#include "input_hal.h"
#include <string.h>


typedef struct {
    const SubMenuItem *items;
    int item_count;
    int selection;
    void (*on_menu)(void);
    lv_obj_t **labels;
    lv_obj_t *key_recv;
    lv_group_t *group;
    lv_color_t focus_bg;
} SubMenuCtx;

static SubMenuCtx *active_submenu = NULL;


static void update_highlight(SubMenuCtx *ctx) {
    for (int i = 0; i < ctx->item_count; i++) {
        if (i == ctx->selection) {
            lv_obj_set_style_bg_color(ctx->labels[i], ctx->focus_bg, 0);
            lv_obj_set_style_bg_opa(ctx->labels[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(ctx->labels[i], LV_OPA_TRANSP, 0);
        }
    }
}


static void submenu_key_cb(lv_event_t *e) {
    uint32_t key = lv_event_get_key(e);
    SubMenuCtx *ctx = active_submenu;
    if (!ctx) return;

    switch (key) {
        case LV_KEY_UP:
            if (ctx->selection > 0) {
                ctx->selection--;
                update_highlight(ctx);
            }
            break;
        case LV_KEY_DOWN:
            if (ctx->selection < ctx->item_count - 1) {
                ctx->selection++;
                update_highlight(ctx);
            }
            break;
        case LV_KEY_ENTER:
            if (ctx->items[ctx->selection].callback) {
                ctx->items[ctx->selection].callback();
            }
            break;
        case 'M':
            if (ctx->on_menu) {
                ctx->on_menu();
            }
            break;
        default:
            break;
    }
}


lv_group_t* ui_create_submenu(
    const SubMenuItem *items,
    int item_count,
    const SubMenuStyle *style,
    void (*on_menu)(void)
) {
    
    if (active_submenu) {
        ui_submenu_cleanup(active_submenu->group);
    }

    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    ui_setup_screen(scr);

    
    SubMenuCtx *ctx = lv_malloc(sizeof(SubMenuCtx));
    ctx->items = items;
    ctx->item_count = item_count;
    ctx->selection = 0;
    ctx->on_menu = on_menu;
    ctx->focus_bg = style->focus_bg_color;
    ctx->labels = lv_malloc(sizeof(lv_obj_t*) * item_count);

    
    ctx->group = lv_group_create();

    
    ctx->key_recv = lv_obj_create(scr);
    lv_obj_set_size(ctx->key_recv, 0, 0);
    lv_obj_add_flag(ctx->key_recv, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ctx->key_recv, submenu_key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(ctx->group, ctx->key_recv);

    
    int y = CONTENT_TOP;
    for (int i = 0; i < item_count; i++) {
        lv_obj_t *lbl = lv_label_create(scr);
        lv_label_set_text(lbl, items[i].text);
        lv_obj_set_pos(lbl, CONTENT_SIDE + 4, y);
        lv_obj_set_size(lbl, LCD_H_RES - 2 * CONTENT_SIDE - 8, 24);
        lv_obj_set_style_text_font(lbl, FONT_PRIMARY, 0);
        lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
        lv_obj_set_style_pad_left(lbl, 6, 0);
        lv_obj_set_style_pad_top(lbl, 4, 0);
        lv_obj_set_style_radius(lbl, 3, 0);
        
        ctx->labels[i] = lbl;
        y += 26;
    }

    
    ui_create_hint_bar(scr, style->hint_text);

    
    lv_indev_t *indev = get_navigation_indev();
    if (indev) lv_indev_set_group(indev, ctx->group);

    
    lv_group_focus_obj(ctx->key_recv);
    update_highlight(ctx);

    active_submenu = ctx;
    return ctx->group;
}

void ui_submenu_cleanup(lv_group_t *group) {
    if (active_submenu && active_submenu->group == group) {
        if (active_submenu->labels) {
            lv_free(active_submenu->labels);
        }
        lv_group_del(group);
        lv_free(active_submenu);
        active_submenu = NULL;
    }
}
