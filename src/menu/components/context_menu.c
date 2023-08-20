#include "../components.h"
#include "../fonts.h"
#include "constants.h"


void component_context_menu_init (component_context_menu_t *cm) {
    cm->selected = -1;
    cm->count = 0;
    for (int i = 0; (cm->list[i].text) != NULL; i++) {
        cm->count += 1;
    }
}

void component_context_menu_show (component_context_menu_t *cm) {
    cm->selected = 0;
}

bool component_context_menu_process (menu_t *menu, component_context_menu_t *cm) {
    if (!cm || (cm->selected < 0)) {
        return false;
    }

    if (menu->actions.back) {
        cm->selected = -1;
    } else if (menu->actions.enter) {
        if (cm->list[cm->selected].action) {
            cm->list[cm->selected].action(menu);
            cm->selected = -1;
        }
    } else if (menu->actions.go_up) {
        cm->selected -= 1;
        if (cm->selected < 0) {
            cm->selected = 0;
        }
    } else if (menu->actions.go_down) {
        cm->selected += 1;
        if (cm->selected >= cm->count) {
            cm->selected = (cm->count - 1);
        }
    }

    return true;
}

void component_context_menu_draw (component_context_menu_t *cm) {
    if (!cm || (cm->selected < 0)) {
        return;
    }

    rdpq_paragraph_builder_begin(
        &(rdpq_textparms_t) {
            .width = VISIBLE_AREA_WIDTH,
            .height = VISIBLE_AREA_HEIGHT,
            .align = ALIGN_CENTER,
            .valign = VALIGN_CENTER,
        },
        FNT_DEFAULT,
        NULL
    );

    for (int i = 0; i < cm->count; i++) {
        const char *text = cm->list[i].text;
        rdpq_paragraph_builder_span(text, strlen(text));
        if (cm->list[i + 1].text != NULL) {
            rdpq_paragraph_builder_newline();
        }
    }

    rdpq_paragraph_t *layout = rdpq_paragraph_builder_end();

    int width = layout->bbox[2] - layout->bbox[0] + MESSAGEBOX_MARGIN;
    int height = layout->bbox[3] - layout->bbox[1] + MESSAGEBOX_MARGIN;

    component_dialog_draw(width, height);

    int highlight_x0 = DISPLAY_CENTER_X - (width / 2);
    int highlight_x1 = DISPLAY_CENTER_X + (width / 2);
    int highlight_height = (layout->bbox[3] - layout->bbox[1]) / layout->nlines;
    int highlight_y = VISIBLE_AREA_Y0 + layout->bbox[1] + ((cm->selected) * highlight_height);

    component_box_draw(
        highlight_x0,
        highlight_y,
        highlight_x1,
        highlight_y + highlight_height,
        CONTEXT_MENU_HIGHLIGHT_COLOR
    );

    rdpq_paragraph_render(layout, VISIBLE_AREA_X0, VISIBLE_AREA_Y0);

    rdpq_paragraph_free(layout);
}
