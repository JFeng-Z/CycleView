/*
 * This file is part of the MiaoUI Library.
 *
 * Copyright (c) 2025, JFeng-Z, <2834294740@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Created on: 2025-02-08
 */
#ifndef __UI_H__
#define __UI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "ui_conf.h"
#include "ui_logger.h"

/**
 * Retrieves the instance of the user interface.
 */
ui_t *Get_UI_Instance(void);
/**
 * Adds an item to the user interface.
 * 
 * This function creates a new UI item with the specified properties and adds it 
 * to the current page. The item can have an associated image, type, and behavior 
 * defined by the callback function.
 * 
 * @param name Name of the item (e.g., label or identifier).
 * @param type Type of the UI item (e.g., button, text field).
 * @param image Pointer to the image data for the item (e.g., icon).
 * @param item Pointer to the ui_item_t structure where the item will be stored.
 * @param localPage Pointer to the current page where the item will be added.
 * @param nextPage Pointer to the page to navigate to when the item is activated.
 * @param function Callback function to execute when the item is interacted with.
 * @param param Optional parameter to pass to the callback function.
 */
void AddItem(const char *name, 
             UI_ITEM_TYPE type, 
             const uint8_t *image, 
             ui_item_t *item, 
             ui_page_t *localPage, 
             ui_page_t *nextPage, 
             ui_item_function function, 
             void *param);
/**
 * Dynamically adds a UI item to the specified page.
 * 
 * This function is used to dynamically add a UI item, such as a button or label, to the user interface. 
 * By specifying the item's name, type, image, associated pages, callback function, and additional parameters, 
 * it allows flexible construction and extension of the user interface.
 * 
 * @param name The name of the UI item, used to identify the item.
 * @param type The type of the UI item, determining its visual and behavioral characteristics.
 * @param image The icon or image data for the UI item, displayed on the item.
 * @param localPage The current page where the UI item belongs, indicating where it should be displayed.
 * @param nextPage The page to navigate to when the UI item is clicked.
 * @param function The callback function executed when the UI item is clicked, used to respond to user actions.
 * @param param Additional parameters passed to the callback function, providing extra operational data.
 * 
 * @return A pointer to the newly added UI item, used for subsequent operations or references.
 */
ui_item_pt Dynamic_AddItem(const char *name, 
                           UI_ITEM_TYPE type, 
                           const uint8_t *image, 
                           ui_page_t *localPage, 
                           ui_page_t *nextPage, 
                           ui_item_function function, 
                           void *param);
/**
 * Dynamically delete an item from the user interface.
 * 
 * This function is responsible for removing a dynamically allocated item 
 * from the user interface. It locates the item using the provided pointer 
 * and removes it from the interface, while also releasing all resources 
 * associated with the item.
 * 
 * @param item Pointer to the user interface item to be deleted.
 *             This pointer should be a valid item previously created 
 *             by an appropriate creation function.
 */
void Dynamic_DeleteItem(ui_item_pt item);
/**
 * Adds a page to the user interface system.
 * 
 * This function is used to add a new page object to the UI system, initializing it with specified parameters such as name, type, and parent page.
 * 
 * @param name A pointer to a string constant representing the name of the page. Used to identify the page.
 * @param page A pointer to a ui_page_t structure representing the page to be added. The actual content and layout of the page are defined by this structure.
 * @param type A value from the UI_PAGE_TYPE enumeration representing the type of the page. Determines how the page behaves and is rendered.
 * @param Parent A pointer to a ui_page_t structure representing the parent page of the page to be added. Defines the hierarchical relationship between pages.
 * 
 * Note: This function only handles the addition of the page and does not handle display logic or event binding. These need to be handled separately.
 */
void AddPage(const char *name, ui_page_t *page, UI_PAGE_TYPE type, ui_page_t *Parent);
float UI_Animation(float targrt, float now, ui_animation_param_t *obj);
float easeInOutCirc(float t, float b, float c, float d);
uint32_t Get_StrLen(const char *str);
uint8_t Dialog_Show(ui_t *ui, int16_t x,int16_t y,int16_t targrtW,int16_t targrtH);
void Draw_Scrollbar(ui_t *ui, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, float step);
void Draw_Scrollbar_Custom(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, float value, float min, float max);
void Draw_CenteredLabel(ui_font_t font, const char* text, int16_t x, int16_t y);
void Draw_Piece(ui_font_t font, const char* text, ui_piece_pt piece);
/**
 * Creates a new UI element.
 * 
 * This function creates a new UI element associated with the specified item.
 * 
 * @param item Pointer to the UI item.
 * @param element Pointer to the UI element structure.
 */
void Create_element(ui_item_t *item, ui_element_t *element);

/**
 * Creates a new micro item in the UI.
 * 
 * This function creates a new micro item within the specified parent item.
 * 
 * @param m_name Name of the micro item.
 * @param m_parent_item Pointer to the parent UI item.
 * @param micro_item Pointer to the micro item structure.
 * @param m_function Function associated with the micro item.
 * @param m_data Data to pass to the function.
 * @param m_piece Pointer to the piece structure.
 */
void Create_micro_item(const char *m_name, 
                       ui_item_t *m_parent_item, 
                       ui_micro_item_t *micro_item, 
                       ui_micro_item_function m_function, 
                       void *m_data, 
                       ui_piece_pt m_piece);

/**
 * Creates the entire UI structure.
 * 
 * This function initializes the UI with the specified item.
 * 
 * @param ui Pointer to the UI structure.
 * @param item Pointer to the UI item.
 */
void Create_UI(ui_t *ui, ui_item_t *item);

/**
 * Runs the main loop of the UI.
 * 
 * This function starts the main event loop of the UI, handling user interactions and updates.
 * 
 * @param ui Pointer to the UI structure.
 */
void ui_loop(ui_t *ui);

#ifdef __cplusplus
}
#endif

#endif
