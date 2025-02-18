#ifndef ACTRUI_H
#define ACTRUI_H
#include "actrwasm.h"
#include "actrquadtree.h"
#include "actrcanvas.h"
#include "actrhashtable.h"
#include "actrmergesortmutate.h"
#include "actrlog.h"

enum ActrUIType
{
    ActrUITypeButton,
    ActrUITypeText,
    ActrUITypeContainer,
    ActrUITypeGradient
};

struct ActrUIControl
{
    enum ActrUIType type;
    int identity;
    struct ActrQuadTreeLeaf *leaf;
    int hidden;
    struct ActrUIControlContainer *container;
    unsigned int foregroundColor;
    unsigned int backgroundColor;
    unsigned int borderColor;
    
    unsigned int foregroundColorHovered;
    unsigned int backgroundColorHovered;
    unsigned int borderColorHovered;
    
    unsigned int foregroundColorFocused;
    unsigned int backgroundColorFocused;
    unsigned int borderColorFocused;
    
    void * state;
    int zindex;
};

struct ActrUIState
{
    struct ActrHashTable *controls;
    struct ActrVector *results;
    struct ActrQuadTree *tree;
    int sequence;
    struct ActrUIControl * hovered;
    struct ActrUIControl * focused;
    int valid;
};

struct ActrUIControlGradient
{
    struct ActrUIControl control;
    int all;
};

struct ActrUIControlButton
{
    struct ActrUIControl control;
    char *label;
};
struct ActrUIControlText
{
    struct ActrUIControl control;
    char *value;
    int cursor;
};
struct ActrUIControlContainer
{
    struct ActrUIControl control;
};

struct ActrUIState * actr_ui_state;

void actr_ui_invalidate() {
    actr_ui_state->valid = 0;
    actr_log("invalidated");
}

void _actr_ui_control_dispose(struct ActrUIControl *control)
{
    actr_free(control->leaf);
    actr_free(control);
}

void _actr_ui_button_dispose(struct ActrUIControlButton *button)
{
    actr_free(button->label);
    _actr_ui_control_dispose((struct ActrUIControl *)button);
}

void _actr_ui_text_dispose(struct ActrUIControlText *text)
{
    actr_free(text->value);
    _actr_ui_control_dispose((struct ActrUIControl *)text);
}

void _actr_ui_container_dispose(struct ActrUIControlContainer *container)
{
    _actr_ui_control_dispose((struct ActrUIControl *)container);
}

void _actr_ui_gradient_dispose(struct ActrUIControlGradient *gradient)
{
    _actr_ui_control_dispose((struct ActrUIControl *)gradient);
}

void actr_ui_init()
{
    actr_ui_state = actr_malloc(sizeof(struct ActrUIState));
    actr_ui_state->controls = actr_hash_table_init();
    actr_ui_state->results = actr_vector_init(8, 8);
    actr_ui_state->tree = actr_quad_tree_init(1, 256, 256, 64, 0);
    actr_ui_state->sequence = 1;
    actr_ui_state->focused = (struct ActrUIControl *)0;
    actr_ui_state->hovered = (struct ActrUIControl *)0;
    actr_ui_invalidate();
}

struct ActrUIControl *actr_ui_get_control(int identity)
{
    return actr_hash_table_find(actr_ui_state->controls, identity);
}

void actr_ui_remove_control(void *remove)
{
    if (remove == 0) {
        return;
    }
    actr_ui_invalidate();
    struct ActrUIControl * control = remove;
    actr_quad_tree_remove(control->leaf);
    switch (control->type)
    {
    case ActrUITypeButton:
        _actr_ui_button_dispose((struct ActrUIControlButton *)control);
        break;
    case ActrUITypeText:
        _actr_ui_text_dispose((struct ActrUIControlText *)control);
        break;
    case ActrUITypeContainer:
        _actr_ui_container_dispose((struct ActrUIControlContainer *)control);
        break;
    case ActrUITypeGradient:
        _actr_ui_gradient_dispose((struct ActrUIControlGradient *)control);
        break;
    }
    actr_hash_table_delete(actr_ui_state->controls, control->identity);
}

void _actr_ui_key_down_text(struct ActrUIControlText *text, int key)
{
    int newLength;
    char *newValue;
    int currentLength = strlen(text->value);
    if (key == 1)
    {
        // left arrow
        text->cursor--;
        if (text->cursor < 0)
        {
            text->cursor = 0;
        } else {
            actr_ui_invalidate();
        }
    }
    else if (key == 2)
    {
        // right arrow
        text->cursor++;
        if (text->cursor > currentLength)
        {
            text->cursor = currentLength;
        } else {
            actr_ui_invalidate();
        }
    }
    else if (key == 3)
    {
        // backspace
        if (text->cursor == 0)
        {
            return;
        }
        actr_ui_invalidate();
        newLength = currentLength - 1;
        newValue = actr_malloc(newLength + 1);
        for (int i = 0; i < newLength; i++)
        {
            if (i < text->cursor - 1)
            {
                newValue[i] = text->value[i];
            }
            else
            {
                newValue[i] = text->value[i + 1];
            }
        }
        actr_free(text->value);
        text->value = newValue;
        text->cursor--;
    }
    else if (key == 4)
    {
        // delete
        if (text->cursor == currentLength)
        {
            return;
        }
        actr_ui_invalidate();
        newLength = currentLength - 1;
        newValue = actr_malloc(newLength + 1);
        for (int i = 0; i < newLength; i++)
        {
            if (i <= text->cursor - 1)
            {
                newValue[i] = text->value[i];
            }
            else
            {
                newValue[i] = text->value[i + 1];
            }
        }
        actr_free(text->value);
        text->value = newValue;
    }
    else if (key >= 32 && key <= 126)
    {
        newLength = currentLength + 1;
        newValue = actr_malloc(newLength + 1);
        actr_ui_invalidate();
        for (int i = 0; i < newLength; i++)
        {
            if (i < text->cursor)
            {
                newValue[i] = text->value[i];
            }
            else if (i == text->cursor)
            {
                newValue[i] = key;
            }
            else
            {
                newValue[i] = text->value[i - 1];
            }
        }
        actr_free(text->value);
        text->value = newValue;
        text->cursor++;
    }
}

struct ActrUIControl * actr_ui_key_down(int key)
{

    // key
    // 1 left arrow
    // 2 right arrow
    // 3 backspace
    //  - 31 are available
    // when key >= 32 and key <= 126 normal ascii codes
    struct ActrUIControl *control = (struct ActrUIControl *)actr_ui_state->focused;

    if (control == 0)
    {
        return 0;
    }

    switch (control->type)
    {
    case ActrUITypeButton:
        break;
    case ActrUITypeText:
        _actr_ui_key_down_text((struct ActrUIControlText *)control, key);
        break;
    case ActrUITypeContainer:
        break;
    case ActrUITypeGradient:
        break;
    }
    return control;
}

int _actr_ui_query_sort_comparator(void * a, void * b) {
    struct ActrUIControl * c1 = (struct ActrUIControl *)(((struct ActrQuadTreeLeaf *)a)->item);
    struct ActrUIControl * c2 = (struct ActrUIControl *)(((struct ActrQuadTreeLeaf *)b)->item);
    if (c1->zindex == c2->zindex) {
        return c1->identity < c2->identity;    
    }
    return c1->zindex < c2->zindex;
}

void _actr_ui_query(int x, int y, int w, int h)
{
    struct ActrQuadTreeBounds area;
    area.point.x = x;
    area.point.y = y;
    area.size.w = w;
    area.size.h = h;
    actr_quad_tree_query(actr_ui_state->tree, &area, actr_ui_state->results);
    actr_merge_sort_mutate(actr_ui_state->results, 0, actr_ui_state->results->count - 1, _actr_ui_query_sort_comparator, 0);
}

struct ActrUIControl *actr_ui_move(int x, int y)
{
    _actr_ui_query(x, y, 1, 1);
    struct ActrUIControl * hovered = 0;
    for (int i = 0; i < actr_ui_state->results->count; i++)
    {
        struct ActrQuadTreeLeaf * leaf = (struct ActrQuadTreeLeaf *)actr_ui_state->results->head[i];
        struct ActrUIControl *result = (struct ActrUIControl *)leaf->item;
        if (hovered == 0 || result->zindex > hovered->zindex)
        {
            hovered = result;
        }
    }
    actr_ui_state->results->count = 0;
    if (actr_ui_state->hovered == hovered)
    {
        return actr_ui_state->hovered;
    }
    actr_ui_invalidate();
    actr_ui_state->hovered = hovered;
    return actr_ui_state->hovered;
}

void _actr_ui_set_focus(struct ActrUIControl * control)
{
    if (actr_ui_state->focused == control)
    {
        return;
    }
    actr_ui_state->focused = control;
    if (control->type == ActrUITypeText)
    {
        struct ActrUIControlText *text = (struct ActrUIControlText *)control;
        text->cursor = strlen(text->value);
    }
    actr_ui_invalidate();
}

struct ActrUIControl * actr_ui_tap(int x, int y)
{
    _actr_ui_query(x, y, 1, 1);

    struct ActrUIControl * target = 0;
    for (int i = 0; i < actr_ui_state->results->count; i++)
    {
        struct ActrQuadTreeLeaf * leaf = (struct ActrQuadTreeLeaf *)actr_ui_state->results->head[i];
        struct ActrUIControl *result = (struct ActrUIControl *)leaf->item;
        
        if (target == 0 || result->zindex > target->zindex)
        {
            target = result;
        }
    }
    actr_ui_state->results->count = 0;
    _actr_ui_set_focus(target);
    return target;
}

void _actr_ui_default(struct ActrUIControl * control)
{
    control->backgroundColor = actr_pack_bytes(127, 127, 127, 100);
    control->foregroundColor = actr_pack_bytes(0, 0, 0, 100);
    control->borderColor = actr_pack_bytes(255, 255, 255, 100);
    
    control->backgroundColorHovered = actr_pack_bytes(255, 255, 255, 100);
    control->foregroundColorHovered = actr_pack_bytes(0, 0, 0, 100);
    control->borderColorHovered = actr_pack_bytes(255, 0, 0, 100);
    
    control->backgroundColorFocused = actr_pack_bytes(0, 0, 0, 100);
    control->foregroundColorFocused = actr_pack_bytes(255, 255, 255, 100);
    control->borderColorFocused = actr_pack_bytes(255, 255, 255, 100);
    
}

struct ActrUIControlButton *actr_ui_button(int x, int y, int w, int h, char *label)
{
    struct ActrUIControlButton *button = actr_malloc(sizeof(struct ActrUIControlButton));
    _actr_ui_default(&button->control);
    button->control.type = ActrUITypeButton;
    button->control.identity = actr_ui_state->sequence++;
    button->control.leaf = actr_quad_tree_leaf(x, y, w, h, button);
    actr_heap_string(&button->label, label);
    actr_quad_tree_insert(actr_ui_state->tree, button->control.leaf);
    actr_hash_table_insert(actr_ui_state->controls, button->control.identity, button);
    actr_ui_invalidate();
    return button;
}

struct ActrUIControlGradient *actr_ui_gradient(int x, int y, int w, int h, unsigned int color, int all)
{
    struct ActrUIControlGradient *gradient = actr_malloc(sizeof(struct ActrUIControlGradient));
    _actr_ui_default(&gradient->control);
    gradient->control.type = ActrUITypeGradient;
    gradient->control.identity = actr_ui_state->sequence++;
    gradient->control.leaf = actr_quad_tree_leaf(x, y, w, h, gradient);
    gradient->control.backgroundColor = color;
    gradient->all = all;

    actr_quad_tree_insert(actr_ui_state->tree, gradient->control.leaf);
    actr_hash_table_insert(actr_ui_state->controls, gradient->control.identity, gradient);
    actr_ui_invalidate();
    return gradient;
}

struct ActrUIControlContainer *actr_ui_container(int x, int y, int w, int h)
{
    struct ActrUIControlContainer *container = actr_malloc(sizeof(struct ActrUIControlContainer));
    _actr_ui_default(&container->control);
    container->control.type = ActrUITypeContainer;
    container->control.identity = actr_ui_state->sequence++;
    container->control.leaf = actr_quad_tree_leaf(x, y, w, h, container);

    actr_quad_tree_insert(actr_ui_state->tree, container->control.leaf);
    actr_hash_table_insert(actr_ui_state->controls, container->control.identity, container);
    actr_ui_invalidate();
    return container;
}

struct ActrUIControlText *actr_ui_text(int x, int y, int w, int h, char *value)
{
    struct ActrUIControlText *text = actr_malloc(sizeof(struct ActrUIControlText));
    _actr_ui_default(&text->control);
    text->control.type = ActrUITypeText;
    text->control.identity = actr_ui_state->sequence++;
    text->control.leaf = actr_quad_tree_leaf(x, y, w, h, text);
    actr_heap_string(&text->value, value);

    actr_quad_tree_insert(actr_ui_state->tree, text->control.leaf);
    actr_hash_table_insert(actr_ui_state->controls, text->control.identity, text);
    actr_ui_invalidate();
    return text;
}

struct ActrPoint32 _actr_ui_get_control_position(struct ActrUIControl *control)
{
    struct ActrPoint32 result;

    result.x = control->leaf->bounds.point.x;
    result.y = control->leaf->bounds.point.y;

    control = (struct ActrUIControl *)control->container;

    while (control)
    {
        result.x += control->leaf->bounds.point.x;
        result.y += control->leaf->bounds.point.y;
        control = (struct ActrUIControl *)control->container;
    }
    return result;
}

int _actr_ui_control_is_hovered(struct ActrUIControl * control) {
    if (actr_ui_state->hovered == control)
    {
        return 1;
    }
    return 0;
}

int _actr_ui_control_is_focused(struct ActrUIControl * control) {
    if (actr_ui_state->focused == control)
    {
        return 1;
    }
    return 0;
}
void _actr_ui_set_fill(unsigned int color)
{
    unsigned char r, g, b, a;
    actr_unpack_bytes(color, &r, &g, &b, &a);
    actr_canvas2d_fill_style(r, g, b, a);
}
void _actr_ui_set_stroke(unsigned int color)
{
    unsigned char r, g, b, a;
    actr_unpack_bytes(color, &r, &g, &b, &a);
    actr_canvas2d_stroke_style(r, g, b, a);
}

void _actr_ui_draw_text(struct ActrUIControlText *text)
{
    
    struct ActrPoint32 position;
    struct ActrSize32 size;
    
    size.w = text->control.leaf->bounds.size.w;
    size.h = text->control.leaf->bounds.size.h;

    if (text->control.container)
    {
        position = _actr_ui_get_control_position((struct ActrUIControl *)text);
    }
    else
    {
        position.x = text->control.leaf->bounds.point.x;
        position.y = text->control.leaf->bounds.point.y;
    }
    
    int focused = _actr_ui_control_is_focused((struct ActrUIControl *)text);
    int hovered = _actr_ui_control_is_hovered((struct ActrUIControl *)text);

    if (focused) _actr_ui_set_fill(text->control.backgroundColorFocused);
    else if (hovered) _actr_ui_set_fill(text->control.backgroundColorHovered);
    else _actr_ui_set_fill(text->control.backgroundColor);
    

    actr_canvas2d_fill_rect(position.x, position.y, size.w, size.h);

    int charWidth = 9;
    int padSide = 5;
    int maxChars = (size.w - padSide * 2) / charWidth;
    int halfChars = maxChars / 2;
    int charCount = strlen(text->value);
    int substart = text->cursor - halfChars;
    
    if (focused) _actr_ui_set_fill(text->control.foregroundColorFocused);
    else if (hovered) _actr_ui_set_fill(text->control.foregroundColorHovered);
    else _actr_ui_set_fill(text->control.foregroundColor);

    if (charCount > maxChars)
    {
        if (substart < 0)
        {
            substart = 0;
        }
        if (substart + maxChars > charCount)
        {
            substart -= substart + maxChars - charCount;
        }
        char *display = substr(text->value, substart, maxChars);
        actr_canvas2d_fill_text(position.x + padSide, position.y + size.h - 5, display);
        actr_free(display);
    }
    else
    {
        substart = 0;
        actr_canvas2d_fill_text(position.x + padSide, position.y + size.h - 5, text->value);
    }

    if (focused) _actr_ui_set_stroke(text->control.borderColorFocused);
    else if (hovered) _actr_ui_set_stroke(text->control.borderColorHovered);
    else _actr_ui_set_stroke(text->control.borderColor);
    
    actr_canvas2d_stroke_rect(position.x, position.y, size.w, size.h);

    if (focused)
    {
        _actr_ui_set_stroke(text->control.borderColorFocused);
        actr_canvas2d_begin_path();

        int cursorStart = position.x + 5;
        actr_canvas2d_moveto(cursorStart + (text->cursor - substart) * charWidth, position.y + size.h - 3);
        actr_canvas2d_lineto(cursorStart + (text->cursor + 1 - substart) * charWidth, position.y + size.h - 3);
        actr_canvas2d_stroke();
    }

}

void _actr_ui_draw_gradient(struct ActrUIControlGradient *gradient)
{
    struct ActrQuadTreeBounds *bounds = &gradient->control.leaf->bounds;
    if (gradient->all == 1)
    {
        actr_canvas2d_fill_gradient_all(bounds->point.x, bounds->point.y, bounds->size.w, bounds->size.h);
    }
    else
    {
        unsigned char r, g, b, a;
        actr_unpack_bytes(gradient->control.backgroundColor, &r, &g, &b, &a);
        actr_canvas2d_fill_gradient_pick(
            bounds->point.x, bounds->point.y, bounds->size.w, bounds->size.h,
            r, g, b
        );
    }
}

void _actr_ui_draw_container(struct ActrUIControlContainer *container)
{
    unsigned char r, g, b, a;

    actr_unpack_bytes(container->control.backgroundColor, &r, &g, &b, &a);
    actr_canvas2d_fill_style(r, g, b, a);
    actr_canvas2d_fill_rect(container->control.leaf->bounds.point.x, container->control.leaf->bounds.point.y, container->control.leaf->bounds.size.w, container->control.leaf->bounds.size.h);

    actr_unpack_bytes(container->control.foregroundColor, &r, &g, &b, &a);
    actr_canvas2d_stroke_style(r, g, b, a);
    actr_canvas2d_stroke_rect(container->control.leaf->bounds.point.x, container->control.leaf->bounds.point.y, container->control.leaf->bounds.size.w, container->control.leaf->bounds.size.h);
}

void _actr_ui_draw_button(struct ActrUIControlButton *button)
{
    struct ActrQuadTreeBounds *bounds = &button->control.leaf->bounds;
    unsigned char r, g, b, a;

    int focused = _actr_ui_control_is_focused((struct ActrUIControl *)button);
    int hovered = _actr_ui_control_is_hovered((struct ActrUIControl *)button);

    // background
    if (focused) _actr_ui_set_fill(button->control.backgroundColorFocused);
    else if (hovered) _actr_ui_set_fill(button->control.backgroundColorHovered);
    else _actr_ui_set_fill(button->control.backgroundColor);
    
    actr_canvas2d_fill_rect(bounds->point.x, bounds->point.y, bounds->size.w, bounds->size.h);

    int charWidth = 9;
    int padSide = 6;
    int maxChars = (bounds->size.w - padSide * 2) / charWidth;
    int charCount = strlen(button->label);

    // label
    if (focused) _actr_ui_set_fill(button->control.foregroundColorFocused);
    else if (hovered) _actr_ui_set_fill(button->control.foregroundColorHovered);
    else _actr_ui_set_fill(button->control.foregroundColor);
    
    int textLift = 8;
    if (charCount > maxChars)
    {
        char *label = substr(button->label, 0, maxChars);
        actr_canvas2d_fill_text(bounds->point.x + padSide, bounds->point.y + bounds->size.h - textLift, label);
        actr_free(label);
    }
    else
    {
        actr_canvas2d_fill_text(bounds->point.x + padSide, bounds->point.y + bounds->size.h - textLift, button->label);
    }

    // border
    if (focused) _actr_ui_set_stroke(button->control.borderColorFocused);
    else if (hovered) _actr_ui_set_stroke(button->control.borderColorHovered);
    else _actr_ui_set_stroke(button->control.borderColor);
    
    actr_canvas2d_stroke_rect(bounds->point.x, bounds->point.y, bounds->size.w, bounds->size.h);
}
void mty(long long y);
void actr_ui_draw(double delta)
{
    if (actr_ui_state->valid == 1) {
        return;
    }
    actr_ui_state->valid = 1;
    // clear canvas
    actr_canvas2d_fill_style(0, 0, 0, 100);
    actr_canvas2d_fill_rect(-10, -10, actrState->canvasSize.w + 20, actrState->canvasSize.h + 20);

    _actr_ui_query(0, 0, actrState->canvasSize.w, actrState->canvasSize.h);
    
    for (int i = 0; i < actr_ui_state->results->count; i++)
    {
        struct ActrQuadTreeLeaf * leaf = (struct ActrQuadTreeLeaf *)actr_ui_state->results->head[i];
        
        struct ActrUIControl *control = (struct ActrUIControl *)leaf->item;

        if (control->hidden)
        {
            continue;
        }
        int hidden = 0;
        /*struct ActrUIControlContainer *container = control->container;
        while (container)
        {
            if (container->control.hidden)
            {
                hidden = 1;
                break;
            }
            container = container->control.container;
        }*/
        if (hidden)
        {
            continue;
        }
        switch (control->type)
        {
        case ActrUITypeButton:
            _actr_ui_draw_button((struct ActrUIControlButton *)control);
            break;
        case ActrUITypeText:
            _actr_ui_draw_text((struct ActrUIControlText *)control);
            break;
        case ActrUITypeContainer:
            _actr_ui_draw_container((struct ActrUIControlContainer *)control);
            break;
        case ActrUITypeGradient:
            _actr_ui_draw_gradient((struct ActrUIControlGradient *)control);
            break;
        }
    }

    actr_ui_state->results->count = 0;

    if (actrState->debug)
    {
        // draw memory report
        actr_log("drawing memory");
        char *mem = actr_memory_report();
        actr_canvas2d_measure_text(mem);
        actr_canvas2d_fill_style(0, 0, 0, 100);

        actr_canvas2d_fill_rect(0, 0, actrState->textSize.w + 2, actrState->textSize.h + 3);
        actr_canvas2d_fill_style(255, 255, 255, 100);
        mty(actrState->textSize.h);
        actr_canvas2d_fill_text(1, actrState->textSize.h, mem);
        actr_free(mem);
        struct ActrPoint64 offset;
        offset.x = 0;
        offset.y = 0;
        actr_quad_tree_draw(actr_ui_state->tree, offset);
    }
}

#endif
